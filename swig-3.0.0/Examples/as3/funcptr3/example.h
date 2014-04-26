/* file: example.h */

//typedef void (*snes_audio_chunk_t)(int16_t *chunk, int32_t size);
typedef void (*snes_audio_chunk_t)(int *chunk, int size);
typedef int (*blah)();

extern snes_audio_chunk_t foo;

//void snes_set_audio_chunk(snes_audio_chunk_t);
void set_blah(blah);
void snes_set_audio_chunk( void (*chunk)(int *a, int b));
