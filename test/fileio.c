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

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <AS3/AS3.h>

void test_lseek(int fd, off_t off, int whence, char *expchar, off_t expoff,
char *msg)
{
  char c;
  off_t seekret = lseek(fd, off, whence);
  int bytesread = read(fd, &c, 1);
  int expbytes = (expchar) ? 1 : 0;
  char *fmtstr;

  if (bytesread != expbytes || (expchar && c != *expchar) 
      || seekret != expoff) {
    if (expchar) {
      printf("lseek %s failed (%d, %c, %u)\n", msg, bytesread, c, seekret);
    } else {
      printf("lseek %s failed (%d, %u)\n", msg, bytesread, seekret);
    }
    exit(1);
  }
}
    

int main()
{
  inline_as3("import com.adobe.flascc.CModule;");
  inline_as3("import my.test.BackingStore;");
  inline_as3("CModule.vfs.addBackingStore(new BackingStore(), null)");
  int fd = open("/dev/tty", O_WRONLY);
  if (fd < 0) {
    printf("open returned error %d (tty)\n", fd);
    return 1;
  }
  char *buf = "the next line of output\n";
  int wrote = write(fd, buf, strlen(buf));
  if (wrote != strlen(buf)) {
    printf("write returned unexpected %d (tty)\n", wrote);
    return 1;
  }
  printf("this output is from printf\n");
  
  char fbuf[256];
  int ffd = open("/afile", O_RDONLY);
  if (ffd < 0) {
    printf("open returned error %d (file)\n", ffd);
    return 1;
  }
  int filesize;
  int bytesread = read(ffd, fbuf, 256);
  filesize = bytesread;
  if (bytesread < 0) {
    printf("read returned error %d\n", bytesread);
    return 1;
  }

  test_lseek(ffd, 0, SEEK_SET, fbuf + 0, 0, "SEEK_SET");
  test_lseek(ffd, 1, SEEK_CUR, fbuf + 2, 2, "SEEK_CUR");
  test_lseek(ffd, -1, SEEK_END, fbuf + filesize - 1, filesize - 1, "SEEK_END");
  test_lseek(ffd, 1, SEEK_END, NULL, filesize + 1, "past EOF");
  
  wrote = write(fd, fbuf, bytesread);
  if (wrote < 0) {
    printf("write returned unexpected %d (file)\n", wrote);
    return 1;
  }
  
  struct stat s;
  int ret = fstat(ffd, &s);
  if (ret) {
    printf("stat returned error %d (file)\n", ret);
    return 1;
  }
  
  printf("file size is %u bytes\n", s.st_size);
  printf("links: %d\n", s.st_nlink);
  printf("block size: %d\n", s.st_blksize);
  printf("blocks: %d\n", s.st_blocks);

  if (s.st_mode == S_IFREG) {
    printf("file mode: regular file (%o)\n", s.st_mode);
  }

  ret = stat("/dev/tty", &s);
  if (ret) {
    printf("stat returned error %d (tty)\n", ret);
    return 1;
  }
  if (s.st_mode & S_IFCHR) {
    printf("file mode: character file\n");
  }
  printf("file mode: %o\n", s.st_mode);
  
  ret = close(fd);
  if (ret) {
    printf("close returned error %d (tty)\n", ret);
  }

  ret = close(ffd);
  if (ret) {
    printf("close returned error %d (file)\n", ret);
  }

  return 0;
}
