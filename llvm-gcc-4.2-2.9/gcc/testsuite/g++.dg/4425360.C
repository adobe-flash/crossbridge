/* APPLE LOCAL file 4425360 */
/* { dg-do run } */
/* { dg-options "-O1" } */
#include <stdio.h>
#include <stdlib.h>
class Packet {
    public:
 Packet * next;
};
int main (int argc, char * const argv[])
{
    Packet * packet=__null;
    Packet * head = 0;
    int count = 0;
    int num = 2;
    do
    {
        if (head == 0)
        {
            head = packet = new Packet;
        }
        else
        {
	  /* printf("Allocating 2nd packet\n"); */
            packet->next = new Packet;
            packet = packet->next;
        }
        packet->next = 0;
        /* printf("in loop %d         head = 0x%X    next = 0x%X\n", count, (unsigned int)head, (unsigned int)head->next); */
        printf("", count, head, head->next);
    } while (++count < num);
    if (!head->next)
      abort ();
    return 0;
}
