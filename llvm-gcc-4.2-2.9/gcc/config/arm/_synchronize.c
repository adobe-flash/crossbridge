#if __APPLE__
extern OSMemoryBarrier(void);
void __sync_synchronize(void)
{
  OSMemoryBarrier();
}
#endif
