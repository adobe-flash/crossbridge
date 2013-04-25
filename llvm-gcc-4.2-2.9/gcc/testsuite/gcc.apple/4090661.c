/* APPLE LOCAL file 4090661 */
/* { dg-do compile { target "*-*-darwin*" } } */
const char c29[] = "12345678901234567890123456789";
const char c30[] = "123456789012345678901234567890";
const char c31[] = "1234567890123456789012345678901";
const char *p29 = "12345678901234567890123456789";
const char *p30 = "123456789012345678901234567890";
const char *p31 = "1234567890123456789012345678901";
/* { dg-final { scan-assembler-not "align 5" } } */
