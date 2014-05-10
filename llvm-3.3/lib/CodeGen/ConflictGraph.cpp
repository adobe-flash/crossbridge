#include "ConflictGraph.h"

#include <string.h>
#include <memory.h>
#ifdef _WIN32
#include <malloc.h>
#else
#include <alloca.h>
#endif

#if defined(__has_include)
	#if __has_include(<unordered_set>)
		#include <unordered_set>
	#else
		#include <tr1/unordered_set>
		namespace std {
			using tr1::unordered_set;
		}
	#endif
#else
	#include <tr1/unordered_set>
	namespace std {
		using tr1::unordered_set;
	}
#endif

#include <algorithm>

#if defined(NDEBUG) || defined(_DEBUG)
static void debugFail() { *(char *)0 = 0; }
#endif

class ConflictGraphVertex
{
	unsigned _color;
	typedef std::unordered_set<const ConflictGraphVertex *> Edges;
	Edges _edges;

public:
	typedef Edges::iterator iterator;
	typedef Edges::const_iterator const_iterator;

	iterator edges_begin() { return _edges.begin(); }
	iterator edges_end() { return _edges.end(); }
	const_iterator edges_begin() const { return _edges.begin(); }
	const_iterator edges_end() const { return _edges.end(); }

	Edges::size_type getEdgeCount() const
	{
		return _edges.size();
	}

	unsigned getColor() const
	{
		return _color;
	}

	void setColor(unsigned color)
	{
		_color = color;
	}

	void addEdge(ConflictGraphVertex *vertex)
	{
		_edges.insert(vertex);
		vertex->_edges.insert(this);
	}
};

struct ConflictGraphVertexDegreeIndirectLess
{
	bool operator()(const ConflictGraphVertex *a, const ConflictGraphVertex *b) const
	{
		return a->getEdgeCount() > b->getEdgeCount(); // higher degrees first
	}
};

ConflictGraph::~ConflictGraph()
{
	Vertices::const_iterator i = _vertices.begin(), e = _vertices.end();

	for(; i != e; i++)
		delete i->second;
}

void ConflictGraph::addVertex(uintptr_t a)
{
	ConflictGraphVertex *&rvertexa = _vertices[a];

	if(!rvertexa)
		rvertexa = new ConflictGraphVertex();
}

void ConflictGraph::addEdge(uintptr_t a, uintptr_t b)
{
#if defined(NDEBUG) || defined(_DEBUG)
	if(a == b)
		debugFail();
#endif
	ConflictGraphVertex *&rvertexa = _vertices[a];
	ConflictGraphVertex *&rvertexb = _vertices[b];

	if(!rvertexa)
		rvertexa = new ConflictGraphVertex();
	if(!rvertexb)
		rvertexb = new ConflictGraphVertex();
	rvertexa->addEdge(rvertexb);
}

void ConflictGraph::color(Coloring *coloring) const
{
	// color!
	{
		std::vector<ConflictGraphVertex *> sortedVertices;

		// sort vertices by degree
		{
			sortedVertices.reserve(_vertices.size());

			Vertices::const_iterator i = _vertices.begin(), e = _vertices.end();
			unsigned noColor = _vertices.size();

			for(; i != e; i++)
			{
				ConflictGraphVertex *vertex = i->second;
				
				vertex->setColor(noColor); // this will make it hit the "scratch" bit in the bitset below...
				sortedVertices.push_back(vertex);
			}

			std::sort(sortedVertices.begin(), sortedVertices.end(),
				ConflictGraphVertexDegreeIndirectLess());
		}

		// greedily color
		{
			enum { intBits = sizeof(int) * 8 }; // bits in an int
			// bit set big enough for a bit for each vertex + one scratch bit for uncolored ones
			unsigned intCount = (sortedVertices.size() + intBits) / intBits;
			int *conflictColors = (int *)alloca(sizeof(int) * intCount);

			memset(conflictColors, 0, intCount * sizeof(int));

			std::vector<ConflictGraphVertex *>::const_iterator i = sortedVertices.begin(),
				e = sortedVertices.end();

			if(i != e) for(;;) // for each vertex...
			{
				ConflictGraphVertex *vertex = *i;
				ConflictGraphVertex::const_iterator ni, ne;

				// set bits in the bitset for each conflict
				ni = vertex->edges_begin();
				ne = vertex->edges_end();
				for(; ni != ne; ni++)
				{
					unsigned color = (*ni)->getColor();

					conflictColors[color / intBits] |= (1 << (color % intBits));
				}

				// find first int that isn't all 1 bits
				int *cur = conflictColors;

				while(*cur == -1)
				{
					cur++;
#if defined(NDEBUG) || defined(_DEBUG) // validate we don't go past the end in debug
					if(cur >= (conflictColors + intCount))
						debugFail();
#endif
				}

				// find first 0 bit
				int bits = *cur;
				unsigned bitNum = 0;

				while(bits & 1)
				{
					bits >>= 1;
					bitNum++;
				}

				// set the color!
				vertex->setColor(bitNum + intBits * (cur - conflictColors));

				i++;
				if(i == e)
					break; // done -- don't bother clearing bit set

				// memset for lots of edges
				if(vertex->getEdgeCount() > intCount)
					memset(conflictColors, 0, intCount * sizeof(int));
				else // otherwize zero out piecemeal
				{
					ni = vertex->edges_begin();
					ne = vertex->edges_end();
					for(; ni != ne; ni++)
					{
						unsigned color = (*ni)->getColor();

						conflictColors[color / intBits] = 0;
					}
				}
			}
		}
	}

	// serialize coloring
	coloring->clear();
	coloring->reserve(_vertices.size());

	Vertices::const_iterator i = _vertices.begin(), e = _vertices.end();

	for(; i != e; i++)
	{
		ConflictGraphVertex *vertex = i->second;
#if defined(NDEBUG) || defined(_DEBUG) // validate in debug
		ConflictGraphVertex::const_iterator ei = vertex->edges_begin(), ee = vertex->edges_end();

		for(; ei != ee; ei++)
			if((*ei)->getColor() == vertex->getColor())
				debugFail();
#endif
		coloring->push_back(std::make_pair(i->first, vertex->getColor()));
	}
}

#if 0
// testing code
#include <stdio.h>

int main()
{
	ConflictGraph g;
#if 0
	uintptr_t petersen[][3] = {
		{ },
		{ 2, 5, 6 }, //1
		{ 1, 3, 7 }, //2
		{ 2, 4, 8 }, //3
		{ 5, 9, 1 }, //4
		{ 1, 4, 10 }, //5
		{ 1, 8, 9 }, //6
		{ 2, 9, 10 }, //7
		{ 3, 6, 10 }, //8
		{ 4, 6, 7 }, //9
		{ 5, 7, 8 } // 10
	};

	for(int k = 0; k <= 100; k += 10)
	for(int i = 1; i <= 10; i++)
	{
		g.addEdge(k+i, k+petersen[i][0]);
		g.addEdge(k+i, k+petersen[i][1]);
		g.addEdge(k+i, k+petersen[i][2]);
	}
#else
	for(uintptr_t a = 0; a < 100000; a++)
	for(uintptr_t b = a+1; b < 100000 && b < a + 100; b++)
		g.addEdge(a, b);
#endif

	ConflictGraph::Coloring c;

	g.color(&c);

#if 1
	ConflictGraph::Coloring::const_iterator i = c.begin(), e = c.end();

	for(; i != e; i++)
	{
		printf("%d => %d\n", i->first, i->second);
	}
#endif
	return 0;
}
#endif
