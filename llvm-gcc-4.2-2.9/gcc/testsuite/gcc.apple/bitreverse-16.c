/* APPLE LOCAL file 4401224 */
extern void abort();
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */
#pragma reverse_bitfields on
#pragma ms_struct on

#pragma pack(push,2)
typedef struct _S5
{
        unsigned short s;
        unsigned int lbf1 : 8;
        unsigned int lbf2 : 8;
        unsigned int lbf3 : 8;
        unsigned int lbf4 : 8;
} S5;
#pragma pack(pop)

void TestS5_1(void)
{
        S5 s5 = {0};
        unsigned char * rgb = (unsigned char *) &s5;

        rgb[0] = 0x1;
        rgb[1] = 0x2;
        rgb[2] = 0x3;
        rgb[3] = 0x4;
        rgb[4] = 0x5;
        rgb[5] = 0x6;

	if (s5.lbf1!=6 ||
	    s5.lbf2!=5 ||
	    s5.lbf3!=4 ||
	    s5.lbf4!=3 )
	  abort();
};

void TestS5_2(void)
{
        S5 s5 = {0};
        unsigned char * rgb = (unsigned char *) &s5;

        s5.lbf1 = 0x3;
        s5.lbf2 = 0x4;
        s5.lbf3 = 0x5;
        s5.lbf4 = 0x6;

	if (rgb[2]!=6 ||
	    rgb[3]!=5 ||
	    rgb[4]!=4 ||
	    rgb[5]!=3 )
	  abort();
};



#pragma pack(push,2)

typedef struct _S6
{
        unsigned int lbf1 : 8;
        unsigned int lbf2 : 8;
        unsigned int lbf3 : 8;
        unsigned int lbf4 : 8;
} S6;

#pragma pack(pop)

void TestS6_1(void)
{
        S6 s6 = {0};
        unsigned char * rgb = (unsigned char *) &s6;
        rgb[0] = 0x3;
        rgb[1] = 0x4;
        rgb[2] = 0x5;
        rgb[3] = 0x6;

	if (s6.lbf1!=6 ||
	    s6.lbf2!=5 ||
	    s6.lbf3!=4 ||
	    s6.lbf4!=3 )
	  abort();
};



void TestS6_2(void)
{
        S6 s6 = {0};
        unsigned char * rgb = (unsigned char *) &s6;
        s6.lbf1 = 0x3;
        s6.lbf2 = 0x4;
        s6.lbf3 = 0x5;
        s6.lbf4 = 0x6;
	if (rgb[0]!=6 ||
	    rgb[1]!=5 ||
	    rgb[2]!=4 ||
	    rgb[3]!=3 )
	  abort();
};

int main(void)
{
	if(sizeof(S5)!=6 || sizeof(S6)!=4)
	  abort();
        TestS5_1();
        TestS6_1();
        TestS5_2();
        TestS6_2();
        return 0;
}
