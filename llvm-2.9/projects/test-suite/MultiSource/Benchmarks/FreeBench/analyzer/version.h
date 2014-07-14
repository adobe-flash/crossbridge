/* String to identify version */

void version()
{
  fprintf(stderr,"--- Dependency Analyzer version: %s, Copyleft 1999 Peter Rundberg ---\n",VERSION);
  fprintf(stderr,"--- Compiled: %s ---\n",COMPDATE);
  fprintf(stderr,"--- Flags: %s ---\n",CFLAGS);
  fprintf(stderr,"--- Host: %s ---\n\n",HOSTNAME);
}
