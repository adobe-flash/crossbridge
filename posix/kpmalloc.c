// Copyright (c) 2013 Adobe Systems Inc

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>

#define PAGE_SIZE (4096)
#define PAGE_COUNT(T) (PAGE_SIZE / sizeof(T))

static void *pcalloc(size_t size)
{
	void *res = mmap(NULL, (size + PAGE_SIZE - 1) & -PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if(res == MAP_FAILED)
		res = NULL;
	return res;
}


static void pfree(void *p, size_t size)
{
	munmap(p, (size + PAGE_SIZE - 1) & -PAGE_SIZE);
}

// kind of fast, kind of space-efficient, simple pool allocator for the pseudo-kernel

// generic quadratic probe for (power of 2 sized) array of keys given a key + hash
static off_t qprobe(uintptr_t key, uintptr_t hash, uintptr_t *keys, size_t size)
{
	unsigned mask = size - 1;
	unsigned n = 7, i = hash & mask;

	for(;;)
	{
		uintptr_t curKey = keys[i];

		if(!curKey || curKey == key)
			break;
		i = (i + n++) & mask;
	}
	return i;
}

static size_t sPageHashSize = 0; // size of hash table
static size_t sPageHashPop = 0; // current population of hash table
static uintptr_t *sPageHash = NULL; // keys (page addresses)
static size_t *sPageSize = NULL; // sizes for each page (indexed by offset in hash)
static void *sFreeHead[12] = { NULL }; // free list head for each size class (powers of 2, 8k max)

// hash value for a page
static uintptr_t pageHash(uintptr_t p)
{
	return (p >> 12) + (p >> 11);
}

// size of a given size class in bytes
static int classSize(int c)
{
	return sizeof(void *) << c;
}

// size class for a given size in bytes
static int sizeClass(size_t size)
{
	size_t n = sizeof(void *);
	int c = 0;

	while(n < size)
	{
		c++;
		n <<= 1;
	}
	return c;
}

// add a page to the hash table
static off_t addPage(void *p)
{
	uintptr_t page = (uintptr_t)p;
	uintptr_t hash = pageHash(page);
	// 90% max loading
	size_t exactLoadedSize = (10 * ++sPageHashPop) / 9;
	size_t desiredHTSize = PAGE_COUNT(uintptr_t);
	off_t off;

	while(desiredHTSize < exactLoadedSize)
		desiredHTSize *= 2; // needs to be power of 2 for qprobe to work
	if(desiredHTSize != sPageHashSize) // rehash
	{
		uintptr_t *oldHash = sPageHash;
		size_t *oldSizes = sPageSize;
		uintptr_t key;
		size_t n;

		sPageHash = (uintptr_t *)pcalloc(desiredHTSize * sizeof(uintptr_t));
		if(!sPageHash)
			return -1;
		sPageSize = (size_t *)pcalloc(desiredHTSize * sizeof(size_t));
		if(!sPageSize)
		{
			pfree(sPageHash, desiredHTSize * sizeof(uintptr_t));
			return -1;
		}
		for(n = 0; n < sPageHashSize; n++)
			if((key = oldHash[n]))
			{
				uintptr_t khash = pageHash(key);
				off = qprobe(key, khash, sPageHash, desiredHTSize);
				sPageHash[off] = key;
				sPageSize[off] = oldSizes[n];
			}
		if(oldHash)
			pfree(oldHash, sizeof(uintptr_t) * sPageHashSize);
		if(oldSizes)
			pfree(oldSizes, sizeof(size_t) * sPageHashSize);
		sPageHashSize = desiredHTSize;
	}
	off = qprobe(page, hash, sPageHash, sPageHashSize);
	sPageHash[off] = page;
	return off;
}

static void freePageOfElems(void *p, size_t elemSize)
{
	size_t allocSize = (elemSize + PAGE_SIZE - 1) & -PAGE_SIZE;
	pfree(p, allocSize);
}

// alloc a page worth of elemSize-sized elements and link them all together
static void *allocPageOfElems(size_t elemSize)
{
	size_t allocSize = (elemSize + PAGE_SIZE - 1) & -PAGE_SIZE;
	void *page = pcalloc(allocSize);
	size_t count = allocSize / elemSize;
	void *last = (elemSize * (count - 1)) + (uint8_t *)page;
	void *cur = page;

	if(!page)
		return NULL;
	// link them all
	while(cur < last)
	{
		void *next = elemSize + (uint8_t *)cur;
		*(void **)cur = next;
		cur = next;
	}
	*(void **)cur = NULL;
	return page;
}

// attempt to allocate a pointer to mem of at least specified size
void *kpmalloc(size_t size)
{
	void *result;
	int c = sizeClass(size);

	if(c >= sizeof(sFreeHead) / sizeof(sFreeHead[0]))
		return NULL; // cap on size! could fall back to mmap here...

	size = classSize(c);
	if(!sFreeHead[c]) // ensure something's there
	{
		void *page = allocPageOfElems(size);
		off_t hoff = page ? addPage(page) : -1;

		if(!page)
			return NULL;
		if(hoff == -1)
		{
			freePageOfElems(page, size);
			return NULL;
		}
		sPageSize[hoff] = size;
		sFreeHead[c] = page;
	}

	result = sFreeHead[c];
	sFreeHead[c] = *(void **)result;
	memset(result, 0, size);
	return result;
}

// get allocation size of a pointer
static size_t allocSize(void *p)
{
	uintptr_t page = -PAGE_SIZE & (uintptr_t)p;
	off_t hoff  = qprobe(page, pageHash(page), sPageHash, sPageHashSize);
	return sPageSize[hoff];
}

// attempt to free a pointer (returning false if we don't own it!)
bool kpfree(void *p)
{
	size_t size = allocSize(p);
	int c;

	if(!size)
		return false;
	c = sizeClass(size);
	*(void **)p = sFreeHead[c];
	sFreeHead[c] = p;
	return true;
}

#if KPMALLOC_TEST
static void test()
{
	static void *allocs[10000] = { 0 };
	int i;

	for(i = 0; i < 10000000; i++)
	{
		size_t size = rand() % 8193;
		int index = rand() % (sizeof(allocs) / sizeof(allocs[0]));
		if(allocs[index])
			kpfree(allocs[index]);
		allocs[index] = kpmalloc(size);
	}
}

int main()
{
	test();
	return 0;
}
#endif
