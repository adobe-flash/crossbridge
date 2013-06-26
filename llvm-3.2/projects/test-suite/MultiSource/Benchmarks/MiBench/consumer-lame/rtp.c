#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

struct rtpbits {
  int sequence:16;	/* sequence number: random */
  int pt:7;	/* payload type: 14 for MPEG audio */
  int m:1;	/* marker: 0 */
  int cc:4;	/* number of CSRC identifiers: 0 */
  int x:1;	/* number of extension headers: 0 */
  int p:1;	/* is there padding appended: 0 */
  int v:2;	/* version: 2 */
};

struct rtpheader {	/* in network byte order */
  struct rtpbits b;
  int timestamp;	/* start: random */
  int ssrc;		/* random */
  int iAudioHeader;	/* =0?! */
};

void initrtp(struct rtpheader *foo) {
  foo->b.v=2;
  foo->b.p=0;
  foo->b.x=0;
  foo->b.cc=0;
  foo->b.m=0;
  foo->b.pt=14;		/* MPEG Audio */
#ifdef FEFE
  foo->b.sequence=42;
  foo->timestamp=0;
#else
  foo->b.sequence=rand() & 65535;
  foo->timestamp=rand();
#endif
  foo->ssrc=rand();
  foo->iAudioHeader=0;
}

int sendrtp(int fd, struct sockaddr_in *sSockAddr, struct rtpheader *foo, void *data, int len) {
  char *buf=alloca(len+sizeof(struct rtpheader));
  int *cast=(int *)foo;
  int *outcast=(int *)buf;
  outcast[0]=htonl(cast[0]);
  outcast[1]=htonl(cast[1]);
  outcast[2]=htonl(cast[2]);
  outcast[3]=htonl(cast[3]);
  memmove(buf+sizeof(struct rtpheader),data,len);
  return sendto(fd,buf,len+sizeof(*foo),0,(struct sockaddr *)sSockAddr,sizeof(*sSockAddr));
/*  return write(fd,buf,len+sizeof(*foo))==len+sizeof(*foo); */
}

/* create a sender socket. */
int makesocket(char *szAddr,unsigned short port,int TTL,struct sockaddr_in *sSockAddr) {
  int          iRet, iLoop = 1;
  struct       sockaddr_in sin;
  char         cTtl = (char)TTL;
  char         cLoop=0;
  unsigned int tempaddr;

  int iSocket = socket( AF_INET, SOCK_DGRAM, 0 );
  if (iSocket < 0) {
    fprintf(stderr,"socket() failed.\n");
    exit(1);
  }

  tempaddr=inet_addr(szAddr);
  sSockAddr->sin_family = sin.sin_family = AF_INET;
  sSockAddr->sin_port = sin.sin_port = htons(port);
  sSockAddr->sin_addr.s_addr = tempaddr;

  iRet = setsockopt(iSocket, SOL_SOCKET, SO_REUSEADDR, &iLoop, sizeof(int));
  if (iRet < 0) {
    fprintf(stderr,"setsockopt SO_REUSEADDR failed\n");
    exit(1);
  }

  if ((ntohl(tempaddr) >> 28) == 0xe) {
    /* only set multicast parameters for multicast destination IPs */
    iRet = setsockopt(iSocket, IPPROTO_IP, IP_MULTICAST_TTL, &cTtl, sizeof(char));
    if (iRet < 0) {
      fprintf(stderr,"setsockopt IP_MULTICAST_TTL failed.  multicast in kernel?\n");
      exit(1);
    }

    cLoop = 1;	/* !? */
    iRet = setsockopt(iSocket, IPPROTO_IP, IP_MULTICAST_LOOP,
		      &cLoop, sizeof(char));
    if (iRet < 0) {
      fprintf(stderr,"setsockopt IP_MULTICAST_LOOP failed.  multicast in kernel?\n");
      exit(1);
    }
  }

  return iSocket;
}
