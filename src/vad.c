#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "pav_analysis.h"
#include "vad.h"
 
const float FRAME_TIME = 10.0F; /* in ms. */
const short N_INIT_MAX = 3;
const int MAYBE_SILENCE_MAX = 
const int MAYBE_VOICE_MAX =
const short VOICE_MIN =
const short SILENCE_MIN =
/* 
   As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
   only this labels are needed. You need to add all labels, in case
   you want to print the internal state in string format */

const char *state_str[] = {
  "UNDEF", "S", "V", "INIT", "M_S","M_V"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  //feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.p = compute_power(x,N);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->last_state = ST_UNDEF;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->k0_th=0;
  vad_data->k1_th=0;
  vad_data->k2_th=0;
  vad_data->n_init=0; 
  vad_data->aplha1=5;
  vad_data->aplha2=5;
  vad_data->voice_count=0;
  vad_data->silence_count=0;

  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x) {

  /* TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */

  switch (vad_data->state) {
  case ST_INIT:
    vad_data->n_init ++;

    vad_data->k0_th = 10.0 * log10((1.0/vad_data->n_init)*pow(10.0,f.p/10.0) + ((vad_data->n_init-1)/vad_data->n_init)*pow (10.0,k0_th/10.0));
    if (vad_data->n_init >= N_INIT_MAX) {
      vad_data->k1_th = vad_data->k0_th + 4;
      vad_data->k2_th = vad_data->k0_th + 8;
      vad_data->state = ST_SILENCE;
    }
    
    break;

  case ST_SILENCE:
    
    if(f.p > vad_data->k1_th) {
      vad_data->state = ST_MAYBE_VOICE;
      vad_data->last_state = ST_SILENCE;
    }
    //faltará caso de que se acabe la señal (END)
    break;

  case ST_VOICE:
    if (f.p < vad_data->k1_th  ) {
      vad_data->state = ST_SILENCE;
    }
      
    break;

  case ST_MAYBE_SILENCE:
  
  break;
  
  case ST_MAYBE_VOICE:
    
    while(f.p > vad_data->k1_th) {
      vad_data->voice_count++;
      if(vad_data->voice_count > MAYBE_SILENCE_MAX) {
        if(f.p <= vad_data->k2_th) {
          
        }else {
        vad_data->state = ST_VOICE;
        vad_data->last_state = ST_MAYBE_VOICE;
        //habrá que restarle a algo el contador TRAMA_EMPIEZA_VOZ = TRAMA_ACTUAL- vad_data->voice_count++;
        vad_data->voice_count = 0;
        }
        
        break;
      }
    }

  break;

  case ST_UNDEF:
    break;
  }

  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
