/* APPLE LOCAL file 6311054 */
/* { dg-do compile } */
/* { dg-options { -fPIC -quiet -mfix-and-continue -m64 -mmacosx-version-min=10.5 } } */
typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
static const uint8_t kObfuscationKey[] = {
  0x7d, 0x89, 0x52, 0x23, 0xd2, 0xbc, 0xdd, 0xea, 0xa3, 0xb9, 0x1f };
void obfuscate( void *buffer, uint32_t bufferLength ) {
  uint8_t *pBuf = (uint8_t *) buffer;
  const uint8_t *pKey = kObfuscationKey, *eKey = pKey + sizeof( kObfuscationKey );
  while ( bufferLength-- )  {
    *pBuf = *pBuf ^ *pKey;
    ++pKey;
    if ( pKey == eKey )    pKey = kObfuscationKey;
  }
}
