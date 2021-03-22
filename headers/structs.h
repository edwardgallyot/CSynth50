#include <porttime.h>
#include <portmidi.h>
#include <portaudio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

/* Firstly we define all our structs so that the compiler
and VS Code don't throw errors when we try reference
our Data Structures Across the MIDI and Audio Engines*/

#define AUDIO_OUT_BUFFER_SIZE (256)
#define PI (3.14159)

typedef struct
{
    float left_phase;
    float right_phase;
    float amp;
    float freq;
    float release_freq;
    float target_frequency;
    int wave;
    float filter_buffer[AUDIO_OUT_BUFFER_SIZE];
    int on;
    float attack_time;
    int peak;
    float release_time;
    float sustain;
    float decay;
    float volume;
} AudioMixer;

typedef struct
{
    PmEvent currentEvent;
    float currentFreq;
    float releaseFreq;
    double hex_places[8];
    PmEvent note_history[128];
    int on;
    float freq_lookup[128];
} MIDI_data;

typedef struct
{
    GtkWidget *lbl_osc_type;
    GtkWidget *random_osc;
    GtkWidget *btn_change_osc;
    GtkWidget *attack_slider;
    GtkWidget *decay_slider;
    GtkWidget *sustain_slider;
    GtkWidget *release_slider;
    GtkWidget *volume_slider;
} GUIpointers;

typedef struct
{
    MIDI_data MIDI;
    AudioMixer Audio;
    GUIpointers *GUI;
} AudioMIDI;

static AudioMIDI engine;