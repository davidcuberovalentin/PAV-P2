#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#include "vad.h"
#include "vad_docopt.h"

#define DEBUG_VAD 0x1

int main(int argc, char *argv[]) {
  int verbose = 0; /* To show internal state of vad: verbose = DEBUG_VAD; */

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile;
  int n_read = 0, i;
  float alpha1;
  float alpha2;
  int N_INIT_MAX;
  int MAYBE_VOICE_MAX;
  int MAYBE_VOICE_MIN;
  int MAYBE_SILENCE_MIN;
  int MAYBE_SILENCE_MAX;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state, last_defined_state;

  float *buffer, *buffer_zeros;
  int frame_size;         /* in samples */
  float frame_duration;   /* in seconds */
  unsigned int t, last_t, t_print; /* in frames */

  char	*input_wav, *output_vad, *output_wav;

  DocoptArgs args = docopt(argc, argv, /* help */ 1, /* version */ "2.0");

  verbose    = args.verbose ? DEBUG_VAD : 0;
  input_wav  = args.input_wav;
  output_vad = args.output_vad;
  output_wav = args.output_wav;
  alpha1 = atof(args.alpha1);
  alpha2 = atof(args.alpha2);
  N_INIT_MAX = atoi(args.Ninitmax);
  MAYBE_VOICE_MAX = atoi(args.Maybevoicemax);
  MAYBE_VOICE_MIN = atoi(args.Maybevoicemin);
  MAYBE_SILENCE_MIN = atoi(args.Maybesilencemin);
  MAYBE_SILENCE_MAX = atoi(args.Maybesilencemax);

  if (input_wav == 0 || output_vad == 0) {
    fprintf(stderr, "%s\n", args.usage_pattern);
    return -1;
  }

  /* Open input sound file */
  if ((sndfile_in = sf_open(input_wav, SFM_READ, &sf_info)) == 0) {
    fprintf(stderr, "Error opening input file: %s\n", input_wav);
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", input_wav);
    return -2;
  }

  /* Open vad file */
  if ((vadfile = fopen(output_vad, "wt")) == 0) {
    fprintf(stderr, "Error opening output vad file: %s\n", output_vad);
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (argc == 4) {
    if ((sndfile_out = sf_open(output_wav, SFM_WRITE, &sf_info)) == 0) {
      fprintf(stderr, "Error opening output wav file: %s\n", output_wav);
      return -1;
    }
  }

  vad_data = vad_open(sf_info.samplerate, alpha1, alpha2, N_INIT_MAX, MAYBE_VOICE_MAX, MAYBE_VOICE_MIN, MAYBE_SILENCE_MIN, MAYBE_SILENCE_MAX);
  /* Allocate memory for buffers */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;

  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  last_state = ST_UNDEF;
  last_defined_state = ST_SILENCE; //assuming there is silence at the beginning

  for (t = last_t = 0; ; t++) { /* For each frame ... */
    /* End loop when file has finished (or there is an error) */
    if  ((n_read = sf_read_float(sndfile_in, buffer, frame_size)) != frame_size) break;

    if (sndfile_out != 0) {
      /* TODO: copy all the samples into sndfile_out */
    }

    state = vad(vad_data, buffer);
    if (verbose & DEBUG_VAD) vad_show_state(vad_data, stdout);

    /* TODO: print only SILENCE and VOICE labels (DOING)*/
    /* As it is, it prints UNDEF segments but is should be merge to the proper value */
    if (state != last_state) {
      if (state == ST_MAYBE_SILENCE || state == ST_MAYBE_VOICE ) {
        t_print = t;
      }
      if ((state == ST_VOICE && last_defined_state == ST_SILENCE) || (state == ST_SILENCE && last_defined_state == ST_VOICE)) {
        fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t_print * frame_duration, state2str(last_defined_state));
        last_defined_state = state;
        last_t = t_print;
      }
     /* if (t != last_t)
        fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration, state2str(last_state));
      last_t = t;*/
      last_state = state;
    }

    if (sndfile_out != 0) {
      /* TODO: go back and write zeros in silence segments */
    }
  }

  state = vad_close(vad_data);
  /* TODO: what do you want to print, for last frames? */
  //CAMBIAR, ESTA PUESTO PARA QUE NO DE ERROR
  if (t != last_t)
    fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration + n_read / (float) sf_info.samplerate, state2str(ST_SILENCE));

  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
