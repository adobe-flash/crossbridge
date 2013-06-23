#include <sys/socket.h>
#include <netinet/in.h>

struct rtpbits {
  int sequence:16;	/* sequence number: random */
  int pt:7;	/* payload type: 14 for MPEG audio */
  int m:1;	/* marker: 0 */
  int cc:4;	/* number of CSRC identifiers: 0 */
  int x:1;	/* number of extension headers: 0 */
  int p:1;	/* is there padding appended: 0 */
  int v:2;	/* version: 2 */
};

struct rtpheader {      /* in network byte order */
  struct rtpbits b;
  int timestamp;        /* start: random */
  int ssrc;             /* random */
  int iAudioHeader;	/* =0?! */
};

void initrtp(struct rtpheader *foo);
int sendrtp(int fd, struct sockaddr_in *sSockAddr, struct rtpheader *foo, void *data, int len);
int makesocket(char *szAddr,unsigned short port,int TTL,struct sockaddr_in *sSockAddr);
void rtp_output(char *mp3buffer,int mp3size);
