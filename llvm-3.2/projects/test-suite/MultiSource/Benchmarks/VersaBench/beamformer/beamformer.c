/*
 * Copyright (c) 2003 David Maze
 *
 * Permission  is hereby  granted,  free  of  charge, to  any  person
 * obtaining a  copy of  this software  and  associated documentation
 * files   (the  "Software"),  to   deal  in  the   Software  without
 * restriction, including without  limitation the rights to use, copy,
 * modify, merge, publish,  distribute, sublicense, and/or sell copies
 * of  the Software,  and to  permit persons to  whom the  Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above  copyright notice  and this  permission notice  shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE  SOFTWARE IS  PROVIDED "AS IS",  WITHOUT WARRANTY OF  ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING  BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY,   FITNESS   FOR   A   PARTICULAR    PURPOSE    AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM,  DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,  OUT OF OR IN
 * CONNECTION  WITH THE SOFTWARE OR  THE USE OR OTHER  DEALINGS IN THE
 * SOFTWARE.  
 */

/*
 * beamformer.c: Standalone beam-former reference implementation
 * David Maze <dmaze@cag.lcs.mit.edu>
 * $Id: beamformer.c,v 1.5 2003/11/07 08:47:00 thies Exp $
 */

/* Modified by: Rodric M. Rabbah 06-03-04 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/* 
This implementation is derived from the StreamIt implementation,
rather than from the PCA VSIPL-based implementation.  It is intended
to be easier to synchronize our reference implementation with our
StreamIt implementation, not have dependencies on extra libraries, and
generally be a fairer comparison with the StreamIt code (that is,
equivalent to our other benchmarks).

This version gets consistent output with the StreamIt version when
compiled with RANDOM_INPUTS and RANDOM_WEIGHTS.  The algorithm should
be equivalent to the SERIALIZED implementation.

FLAGS: the StreamIt TemplateBeamFormer can be built with COARSE
defined or not, with SERIALIZED defined or not, and with RANDOM_INPUTS
defined or not.  Defining COARSE changes the algorithm to add two
decimation stages; this presently isn't implemented.  SERIALIZED
changes where in the graph printing happens; in the non-SERIALIZED
version printing happens in the detector stage, but this doesn't have
a deterministic order.  RANDOM_INPUTS and RANDOM_WEIGHTS change
weights at several points in the code, where "RANDOM" is "something
the author made up" rather than "determined via rand()".  The default
flags are both RANDOM_WEIGHTS and RANDOM_INPUTS.

RANDOM_INPUTS and RANDOM_WEIGHTS are easy to do, so they are
implemented here as well.  We ignore SERIALIZED, though we do go
through the detectors in a deterministic order which should give the
right answer, and add a reordering stage equivalent to what SERIALZIED
adds to get results in the same order.  COARSE is left for a future
implementation.
*/

#define RANDOM_INPUTS

struct BeamFirData
{
  int len, count, pos;
  float *weight, *buffer;
};

void begin(void);
void InputGenerate(int channel, float *inputs, int n);
void BeamFirSetup(struct BeamFirData *data, int n);
void BeamFirFilter(struct BeamFirData *data,
                   int input_length, int decimation_ratio,
                   float *in, float *out);
void BeamFormWeights(int beam, float *weights);
void BeamForm(int beam, const float *weights, const float *input,
              float *output);
void Magnitude(float *in, float *out, int n);
void Detector(int beam, float *data, float *output);

#define NUM_CHANNELS 12
#define NUM_SAMPLES 1024
#define NUM_BEAMS 4
/* Bill writes: under current implementation, decimation ratios must
   divide NUM_SAMPLES. */
#define NUM_COARSE_TAPS 64
#define NUM_FINE_TAPS 64
#define COARSE_DECIMATION_RATIO 1
#define FINE_DECIMATION_RATIO 2
#define NUM_SEGMENTS 1
#define NUM_POST_DEC_1 (NUM_SAMPLES/COARSE_DECIMATION_RATIO)
#define NUM_POST_DEC_2 (NUM_POST_DEC_1/FINE_DECIMATION_RATIO)
#define MF_SIZE (NUM_SEGMENTS*NUM_POST_DEC_2)
#define PULSE_SIZE (NUM_POST_DEC_2/2)
#define PREDEC_PULSE_SIZE \
  (PULSE_SIZE*COARSE_DECIMATION_RATIO*FINE_DECIMATION_RATIO)
#define TARGET_BEAM (NUM_BEAMS/4)
#define TARGET_SAMPLE (NUM_SAMPLES/4)
#define TARGET_SAMPLE_POST_DEC \
   (TARGET_SAMPLE/COARSE_DECIMATION_RATIO/FINE_DECIMATION_RATIO)
#define D_OVER_LAMBDA 0.5
#define CFAR_THRESHOLD (0.95*D_OVER_LAMBDA*NUM_CHANNELS*0.5*PULSE_SIZE)

static int numiters = 100;

int main(int argc, char **argv)
{
  int option;

  while ((option = getopt(argc, argv, "i:")) != -1)
  {
    switch(option)
    {
    case 'i':
      numiters = atoi(optarg);
    }
  }

  begin();
  return 0;
}

/* What are the counts here?  Deriving:
 *
 * Detector takes TARGET_SAMPLE_POST_DEC inputs.
 * Magnitude takes twice as many.
 * The various BeamFirFilters have internal circular buffers,
 *   so their counts are only affected by the decimation rates.
 *   The last BeamFirFilter has no decimation, so its input count
 *   is also 2*TARGET_SAMPLE_POST_DEC.
 * BeamForm takes NUM_CHANNELS times as many.
 *
 * So, coming out of the first for loop, the buffer should have
 * 2*TARGET_SAMPLE_POST_DEC*NUM_CHANNELS values.  This comes from
 * a roundrobin(2) joiner in StreamIt, for NUM_CHANNELS children.
 *
 * The second BeamFirFilter has 2*TARGET_SAMPLE_POST_DEC outputs.
 * The first BeamFirFilter has
 *   2*TARGET_SAMPLE_POST_DEC*FINE_DECIMATION_RATIO outputs.
 * The input generator produces 2*TARGET_SAMPLE values.
 */

void begin(void)
{
#ifdef AVOID_PRINTF
  volatile float result;
#endif
  struct BeamFirData coarse_fir_data[NUM_CHANNELS];
  struct BeamFirData fine_fir_data[NUM_CHANNELS];
  struct BeamFirData mf_fir_data[NUM_BEAMS];
  float inputs[2*NUM_SAMPLES*NUM_CHANNELS];
  float predec[2*NUM_POST_DEC_1*NUM_CHANNELS];
  float postdec[NUM_CHANNELS][2*NUM_POST_DEC_2*NUM_CHANNELS];
  float beam_weights[NUM_BEAMS][2*NUM_CHANNELS];
  float beam_input[2*NUM_CHANNELS*NUM_POST_DEC_2];
  float beam_output[2*NUM_POST_DEC_2];
  float beam_fir_output[2*NUM_POST_DEC_2];
  float beam_fir_mag[NUM_POST_DEC_2];
  float detector_out[NUM_BEAMS][NUM_POST_DEC_2];
  int i, j;

  for (i = 0; i < NUM_CHANNELS; i++)
  {
    BeamFirSetup(&coarse_fir_data[i], NUM_COARSE_TAPS);
    BeamFirSetup(&fine_fir_data[i], NUM_FINE_TAPS);
  }
  for (i = 0; i < NUM_BEAMS; i++)
  {
    BeamFirSetup(&mf_fir_data[i], MF_SIZE);
    BeamFormWeights(i, beam_weights[i]);
  }

  /*** VERSABENCH START ***/
  
  while (numiters == -1 || numiters-- > 0) {      
    for (i = 0; i < NUM_CHANNELS; i++) {
      for (j = 0; j < NUM_CHANNELS; j++)
        InputGenerate(i, inputs + j*NUM_SAMPLES*2,
                      NUM_SAMPLES);
      for (j = 0; j < NUM_POST_DEC_1; j++)
        BeamFirFilter(&coarse_fir_data[i],
                      NUM_SAMPLES, COARSE_DECIMATION_RATIO,
                      inputs + j * COARSE_DECIMATION_RATIO*2,
                      predec + j * 2);
      for (j = 0; j < NUM_POST_DEC_2; j++)
        BeamFirFilter(&fine_fir_data[i],
                      NUM_POST_DEC_1, FINE_DECIMATION_RATIO,
                      predec + j * FINE_DECIMATION_RATIO * 2,
                      postdec[i] + j * 2);
    }
    /* Assemble beam-forming input: */
    for (i = 0; i < NUM_CHANNELS; i++)
      for (j = 0; j < NUM_POST_DEC_2; j++)
      {
        beam_input[j*NUM_CHANNELS*2+2*i] = postdec[i][2*j];
        beam_input[j*NUM_CHANNELS*2+2*i+1] = postdec[i][2*j+1];
      }
    for (i = 0; i < NUM_BEAMS; i++)
    {
      /* Have now rearranged NUM_CHANNELS*NUM_POST_DEC_2 items.
       * BeamForm takes NUM_CHANNELS inputs, 2 outputs. */
      for (j = 0; j < NUM_POST_DEC_2; j++)
        BeamForm(i, beam_weights[i],
                 beam_input + j*NUM_CHANNELS*2,
                 beam_output + j*2);
      for (j = 0; j < NUM_POST_DEC_2; j++)
        BeamFirFilter(&mf_fir_data[i],
                      NUM_POST_DEC_2, 1,
                      beam_output+j*2, beam_fir_output+j*2);
      Magnitude(beam_fir_output, beam_fir_mag, NUM_POST_DEC_2);
      Detector(i, beam_fir_mag, detector_out[i]);
    }
    for (j = 0; j < NUM_POST_DEC_2; j++)
      for (i = 0; i < NUM_BEAMS; i++)
#ifdef AVOID_PRINTF
        result = detector_out[i][j];
#else
        printf("%f\n", detector_out[i][j]);
#endif
  }

  /*** VERSABENCH END ***/
  
}

void InputGenerate(int channel, float *inputs, int n)
{
  int i;
  for (i = 0; i < n; i++)
  {
    if (channel == TARGET_BEAM && i == TARGET_SAMPLE)
    {
#ifdef RANDOM_INPUTS
      inputs[2*i] = sqrt(i*channel);
      inputs[2*i+1] = sqrt(i*channel)+1;
#else
      inputs[2*i] = sqrt(CFAR_THRESHOLD);
      inputs[2*i+1] = 0;
#endif
    } else {
#ifdef RANDOM_INPUTS
      float root = sqrt(i*channel);
      inputs[2*i] = -root;
      inputs[2*i+1] = -(root+1);
#else
      inputs[2*i] = 0;
      inputs[2*i+1] = 0;
#endif
    }
  }
}

void BeamFirSetup(struct BeamFirData *data, int n)
{
  int i, j;
  
  data->len = n;
  data->count = 0;
  data->pos = 0;
  data->weight = malloc(sizeof(float)*2*n);
  data->buffer = malloc(sizeof(float)*2*n);
  
#ifdef RANDOM_WEIGHTS
  for (j = 0; j < n; j++) {
    int idx = j+1;
    data->weight[j*2] = sin(idx) / ((float)idx);
    data->weight[j*2+1] = cos(idx) / ((float)idx);
    data->buffer[j*2] = 0.0;
    data->buffer[j*2+1] = 0.0;
  }
#else
  data->weight[0] = 1.0;
  data->weight[1] = 0.0;
  for (i = 1; i < 2*n; i++) {
    data->weight[i] = 0.0;
    data->buffer[i] = 0.0;
  }
#endif
}

void BeamFirFilter(struct BeamFirData *data,
                   int input_length, int decimation_ratio,
                   float *in, float *out)
{
  /* Input must be exactly 2*decimation_ratio long; output must be
   * exactly 2 long. */
  float real_curr = 0;
  float imag_curr = 0;
  int i;
  int modPos;
  int len, mask, mask2;
  
  len = data->len;
  mask = len - 1;
  mask2 = 2 * len - 1;
  modPos = 2*(len - 1 - data->pos);
  data->buffer[modPos] = in[0];
  data->buffer[modPos+1] = in[1];
  
  /* Profiling says: this is the single inner loop that matters! */
  for (i = 0; i < 2*len; i+=2) {
    float rd = data->buffer[modPos];
    float id = data->buffer[modPos+1];
    float rw = data->weight[i];
    float iw = data->weight[i+1];
    float rci = rd * rw + id * iw;
    /* sign error?  this is consistent with StreamIt --dzm */
    float ici = id * rw + rd * iw;
    real_curr += rci;
    imag_curr += ici;
    modPos = (modPos + 2) & mask2;
  }
  
  data->pos = (data->pos + 1) & mask;
  out[0] = real_curr;
  out[1] = imag_curr;
  data->count += decimation_ratio;
  if (data->count == input_length)
  {
    data->count = 0;
    data->pos = 0;
    for (i = 0; i < 2*data->len; i++) {
      data->buffer[i] = 0.0;
    }
  }
}

void BeamFormWeights(int beam, float *weights)
{
  int i;
  for (i = 0; i < NUM_CHANNELS; i++)
  {
#ifdef RANDOM_WEIGHTS
    int idx = i+1;
    weights[2*i] = sin(idx) / (float)(beam+idx);
    weights[2*i+1] = cos(idx) / (float)(beam+idx);
#else
    if (i == beam) {
      weights[2*i] = 1;
      weights[2*i+1] = 0;
    } else {
      weights[2*i] = 0;
      weights[2*i+1] = 0;
    }
#endif
  }
}

void BeamForm(int beam, const float *weights, const float *input,
              float *output)
{
  /* 2*NUM_CHANNELS inputs and weights; 2 outputs. */
  float real_curr = 0;
  float imag_curr = 0;
  int i;
  for (i = 0; i < NUM_CHANNELS; i++)
  {
    real_curr += weights[2*i] * input[2*i] - weights[2*i+1] * input[2*i+1];
    imag_curr += weights[2*i] * input[2*i+1] + weights[2*i+1] * input[2*i];
  }
  output[0] = real_curr;
  output[1] = imag_curr;
}

void Magnitude(float *in, float *out, int n)
{
  int i;
  /* Should be 2n inputs, n outputs. */
  for (i = 0; i < n; i++)
    out[i] = sqrt(in[2*i]*in[2*i] + in[2*i+1]*in[2*i+1]);
}

void Detector(int beam, float *data, float *output)
{
  int sample;
  /* Should be exactly NUM_POST_DEC_2 samples. */
  for (sample = 0; sample < NUM_POST_DEC_2; sample++)
  {
    float outputVal;
    if (beam == TARGET_BEAM && sample == TARGET_SAMPLE) {
      if (data[sample] >= 0.1)
        outputVal = beam+1;
      else
        outputVal = 0;
    } else {
      if (data[sample] >= 0.1)
        outputVal = -(beam+1);
      else
        outputVal = 0;
    }
    outputVal = data[sample];
    output[sample]= outputVal;
  }
}
