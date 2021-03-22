#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "structs.h"

#define STRING_MAX 80
#define INPUT_BUFFER 200
#define SAMPLE_RATE (44100)

int get_message(int32_t x, double y[8]);
int get_velocity(int32_t x, double y[8]);
int get_note(int32_t x, double y[8]);
void MIDI_lookup(float x[127]);
float MIDI_to_freq(int32_t x);
static unsigned int m, n;
static int32_t message, note, velocity;
static PmMessage latestMessage;
static PmEvent previous_on;

/* We firstly Define a stream and then set up the  
MIDI call back function.

This function reads data into an event buffer and we pass that event
into our data structure for our audio midi_input to work with
when it calls back its buffer.
*/
PmStream *in;

void pt_callback(PtTimestamp timestamp, void *userData)
{
    PmError status;
    PmEvent buffer[1];
    AudioMIDI *MIDI_input = (AudioMIDI *)userData;

    previous_on.timestamp = 0;
    latestMessage = 0x000000;
    do
    {
        status = Pm_Poll(in);
        if (status == TRUE)
        {
            Pm_Read(in, buffer, 1);
            note = get_note(buffer[0].message, MIDI_input->MIDI.hex_places);
            velocity = get_velocity(buffer[0].message, MIDI_input->MIDI.hex_places);
            message = get_message(buffer[0].message, MIDI_input->MIDI.hex_places);
            if (message == 0x80 || velocity == 0)
                MIDI_input->MIDI.releaseFreq = MIDI_input->MIDI.freq_lookup[note];
            // Find a message for the Oscillators release

            MIDI_input->MIDI.note_history[note].message = buffer[0].message;
            MIDI_input->MIDI.note_history[note].timestamp = Pt_Time();
            MIDI_input->MIDI.on = 1;
            // Update the Arrays

            for (m = 0; m < 128; m++)
            {
                message = get_message(MIDI_input->MIDI.note_history[m].message, MIDI_input->MIDI.hex_places);
                velocity = get_velocity(MIDI_input->MIDI.note_history[m].message, MIDI_input->MIDI.hex_places);
                note = get_note(MIDI_input->MIDI.note_history[m].message, MIDI_input->MIDI.hex_places);
                if (MIDI_input->MIDI.note_history[m].timestamp > MIDI_input->MIDI.currentEvent.timestamp && (message == 0x90 && velocity > 0))
                {
                    MIDI_input->MIDI.currentEvent = MIDI_input->MIDI.note_history[m];
                    MIDI_input->MIDI.currentFreq = MIDI_input->MIDI.freq_lookup[note];
                }
            }

            // For the latest Note_Off messagee there still any notes on?

            // Find the latest message

            for (m = 0; m < 128; m++)
            {
                if (MIDI_input->MIDI.note_history[m].timestamp > MIDI_input->MIDI.currentEvent.timestamp)
                {
                    latestMessage = MIDI_input->MIDI.note_history[m].message;
                    velocity = get_velocity(latestMessage, MIDI_input->MIDI.hex_places);
                    message = get_message(latestMessage, MIDI_input->MIDI.hex_places);
                    note = get_note(latestMessage, MIDI_input->MIDI.hex_places);
                    if (velocity == 0 || message == 0x80)
                    {
                        for (n = 0; n < 128; n++)
                        {
                            message = get_message(MIDI_input->MIDI.note_history[n].message, MIDI_input->MIDI.hex_places);
                            velocity = get_velocity(MIDI_input->MIDI.note_history[n].message, MIDI_input->MIDI.hex_places);
                            if (latestMessage != MIDI_input->MIDI.note_history[n].message && (message == 0x90 && velocity > 0))
                            {
                                if (MIDI_input->MIDI.note_history[n].timestamp > previous_on.timestamp)
                                    previous_on = MIDI_input->MIDI.note_history[n];
                            }
                        }
                    }
                    MIDI_input->MIDI.currentEvent.message = previous_on.message;
                    MIDI_input->MIDI.currentFreq = MIDI_input->MIDI.freq_lookup[get_note(previous_on.message, MIDI_input->MIDI.hex_places)];
                }
            }
        }
        // Are there any notes on?
        // What was the last note_on message?
    } while (status == TRUE);
    return;
}

int get_number(char *prompt)
{
    char line[STRING_MAX];
    int n = 0, i;
    printf("%s", prompt);
    while (n != 1)
    {
        n = scanf("%d", &i);
        fgets(line, STRING_MAX, stdin);
    }
    return i;
}

float MIDI_to_freq(int32_t x)
{
    /* https://en.wikipedia.org/wiki/MIDI_tuning_standard
    The key thing that I'm doing here is computing the algorithm found in this document!
    That's all this function does.
    */
    double y = (x - 69) / (double)12;
    double multiply = pow(2, y);
    float frequency = 440 * (float)multiply;
    return frequency;
}

// Some nice little abstractions for parsing the MIDI data into stuff I can actually understand.

int32_t get_note(int32_t x, double y[8])
{
    return x % (int32_t)y[4] / (int32_t)y[2];
}

int32_t get_velocity(int32_t x, double y[8])
{
    return x % (int32_t)y[6] / (int32_t)y[4];
}

int32_t get_message(int32_t x, double y[8])
{
    return x % (int32_t)y[2] / (int32_t)y[0];
}

int32_t hexp_assign(double x[8])
{
    for (int i = 0; i < 8; i++)
    {
        x[i] = pow(16, i);
    }
    return 0;
}

void MIDI_lookup(float x[128])
{
    for (int y = 0; y < 128; y++)
    {
        x[y] = MIDI_to_freq(y) / SAMPLE_RATE;
    }
}
