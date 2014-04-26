/* APPLE LOCAL file */
/* Radar 4317709 */
/* { dg-do compile { target powerpc-*-darwin* } } */
/* { dg-options "-w" } */

#pragma reverse_bitfields on
#pragma ms_struct on
#pragma pack(1)

typedef struct _bee
{
        unsigned short  cA : 8;
        unsigned short  fB : 1;
        unsigned short  fC : 1;
        unsigned short  wVal : 9;
} BEE;

extern const BEE rgbee[100];

int LaLaFunction()
{
        int foo = 1;
        int bar = 3;
        switch (foo)
                {
        case 200 :
                {
                if (rgbee[bar].wVal == 2)
                        goto LNeverChange;
                break;
                }

        default:
LNeverChange:
                bar = 0x23;
                break;
                }

        return 1;
}
