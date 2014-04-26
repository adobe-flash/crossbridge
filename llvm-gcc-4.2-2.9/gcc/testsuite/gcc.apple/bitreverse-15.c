/* APPLE LOCAL file 4401223 */
/* { dg-do run { target powerpc*-*-darwin* } } */
/* { dg-options "-std=gnu99" } */
extern void abort();
#pragma reverse_bitfields on
#pragma ms_struct on
#pragma pack(push,2)
typedef struct _S2
{
        unsigned char cbf1 : 4;
        unsigned char cbf2 : 4;
        unsigned char cbf3 : 4;
        unsigned char cbf4 : 4;
        unsigned short s;
} S2;
#pragma pack(pop)

void TestS2(void)
{
        S2 s2 = {0};
        unsigned char * rgb = (unsigned char *) &s2;
        rgb[0] = 0x12;
        rgb[1] = 0x34;
        rgb[2] = 0x56;
        rgb[3] = 0x78;
        if (sizeof(S2)!=4 ||
	    s2.cbf1!=2 ||
	    s2.cbf2!=1 ||
	    s2.cbf3!=4 ||
	    s2.cbf4!=3 ||
	    s2.s != 0x5678)
	  abort();
};

#pragma pack(push,4)
typedef struct _S4
{
        unsigned short sbf1 : 8;
        unsigned short sbf2 : 8;
        unsigned short sbf3 : 8;
        unsigned short sbf4 : 8;
        unsigned int l;
} S4;
#pragma pack(pop)

void TestS4(void)
{
        S4 s4 = {0};
        unsigned char * rgb = (unsigned char *) &s4;
        rgb[0] = 0x1;
        rgb[1] = 0x2;
        rgb[2] = 0x3;
        rgb[3] = 0x4;
        rgb[4] = 0x5;
        rgb[5] = 0x6;
        rgb[6] = 0x7;
        rgb[7] = 0x8;
	if (sizeof(S4)!=8 ||
	    s4.sbf1!=2 ||
	    s4.sbf2!=1 ||
	    s4.sbf3!=4 ||
	    s4.sbf4!=3 ||
	    s4.l!=0x05060708)
	  abort();
};

int main(void)
{
        TestS2();
        TestS4();
        return 0;
}
