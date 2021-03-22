#include "GUI.h"

#define NUM_SECONDS (2)
#define ONE_HERTZ (1 / 44100)
#define ONE_SAMPLE (1 / (AUDIO_OUT_BUFFER_SIZE / 4))

// So we can do decimal maths on hexadecimal MIDI info
// See MIDI_engine.h

/*https://www.liveabout.com/convert-milliseconds-to-samples-1817867
*/

float make_analog_square(float amp, float freq, unsigned int *time);
float make_digital_square(float amp, float freq, unsigned int *time);
float make_analog_saw(float amp, float freq, unsigned int *time);
float make_digital_saw(float amp, float freq, float signal);
float envelope(int *trig, float incr, float decr, float decr2, float amp, float sustain, int *peak, float volume);
float oscillator(int change, int wave, float amp, float previous_freq, float freq, float sig, unsigned int s, unsigned int a, unsigned int c);

// Global Variables

int up = 1;
int on = 0;
static unsigned int a, b, c;
unsigned int s = 0;
unsigned int change = 0;
static float amp, freq, attack, release, sustain, decay, incr, decr, decr2, volume, previous_freq;
static float sig;

static int
patestCallback(const void *inputBuffer, void *outputBuffer,
               unsigned long framesPerBuffer,
               const PaStreamCallbackTimeInfo *timeInfo,
               PaStreamCallbackFlags statusFlags,
               void *userEngine)

{
    AudioMIDI *engine = (AudioMIDI *)userEngine;
    float *out = (float *)outputBuffer;
    (void)inputBuffer;
    change = 0;
    freq = engine->MIDI.currentFreq;

    for (m = 0; m < 128; m++)
    {
        velocity = get_velocity(engine->MIDI.note_history[m].message, engine->MIDI.hex_places);
        message = get_message(engine->MIDI.note_history[m].message, engine->MIDI.hex_places);
        if (message != 0x90 || velocity == 0)
            engine->Audio.on = 0;
        else
        {
            engine->Audio.on = 1;
            if (previous_freq != freq)
                change = 1;
            break;
        }
    }
    for (c = 0; c < 2; c++)
    {
        amp = engine->Audio.amp;

        // Check the structure 4 times every buffer and load into the global variables

        attack = engine->Audio.attack_time;
        release = engine->Audio.release_time;
        decay = engine->Audio.decay;
        sig = engine->Audio.left_phase;
        volume = engine->Audio.volume;
        sustain = engine->Audio.sustain / 1 * volume;
        incr = (volume / attack) / SAMPLE_RATE;
        decr = (volume / release) / SAMPLE_RATE;
        decr2 = (volume / decay) / SAMPLE_RATE;

        /*if (change == 1)
        {
            printf("Change\n");
            printf("%i\n", AUDIO_OUT_BUFFER_SIZE / 2);
        }*/

        if (!engine->Audio.on)
            freq = engine->MIDI.releaseFreq;

        // Load up our array
        for (a = 0; a < AUDIO_OUT_BUFFER_SIZE / 2; a++, s++)
        {
            amp = envelope(&engine->Audio.on, incr, decr, decr2, amp, sustain, &engine->Audio.peak, volume);

            sig = oscillator(change, engine->Audio.wave, amp, previous_freq, freq, sig, s, a, c);
            // Simple envelope control

            // Simple Sine Wave

            // Load Up Chunk
            engine->Audio.filter_buffer[a] = sig;
            // Reset Envelope Phase
            engine->Audio.amp = amp;
        }

        // Send our array to portaudio
        for (b = 0; b < AUDIO_OUT_BUFFER_SIZE / 2; b++)
        {
            *out++ = engine->Audio.left_phase;
            *out++ = engine->Audio.right_phase;
            engine->Audio.left_phase = engine->Audio.filter_buffer[b];
            engine->Audio.right_phase = engine->Audio.filter_buffer[b];
        }
    }
    // Note the out frequency to account for changes
    previous_freq = freq;
    return 0;
}

float oscillator(int change, int wave, float amp, float previous_freq, float freq, float sig, unsigned int s, unsigned int a, unsigned int c)
{
    if (change == 1)
    {
        // Ramp Down the changes in the oscilators to help mitigate jump discountinuitys
        if (c == 0)
        {
            //Simple Analog Square Wave
            if (wave == 1)
                sig = make_analog_square(amp, previous_freq, &s);

            // Simple Digital Square Wave
            if (wave == 2)
                sig = make_digital_square(amp, previous_freq, &s);

            // Simple Digital Saw Wave
            if (wave == 3)
                sig = make_digital_saw(amp, previous_freq, sig);

            // Simple Analog Saw Wave
            if (wave == 4)
                sig = make_analog_saw(amp, previous_freq, &s);

            if (a == (AUDIO_OUT_BUFFER_SIZE / 2) - 1)
                sig = 0;
            else
                sig *= (((AUDIO_OUT_BUFFER_SIZE / 2) - 1) - (float)a) / ((AUDIO_OUT_BUFFER_SIZE / 2) - 1);
        }
        if (c == 1)
        // Switch to the appropriate frequency and then ramp up
        {
            //Simple Analog Square Wave
            if (wave == 1)
                sig = make_analog_square(amp, freq, &s);

            // Simple Digital Square Wave
            if (wave == 2)
                sig = make_digital_square(amp, freq, &s);

            // Simple Digital Saw Wave
            if (wave == 3)
                sig = make_digital_saw(amp, freq, sig);

            // Simple Analog Saw Wave
            if (wave == 4)
                sig = make_analog_saw(amp, freq, &s);
            if (a == 0)
                sig = 0;
            else
                sig *= ((float)a) / ((AUDIO_OUT_BUFFER_SIZE / 2));
        }
        return sig;
    }

    else
    {
        //Simple Analog Square Wave
        if (wave == 1)
            sig = make_analog_square(amp, freq, &s);

        // Simple Digital Square Wave
        if (wave == 2)
            sig = make_digital_square(amp, freq, &s);

        // Simple Digital Saw Wave
        if (wave == 3)
            sig = make_digital_saw(amp, freq, sig);

        // Simple Analog Saw Wave
        if (wave == 4)
            sig = make_analog_saw(amp, freq, &s);
    }
    return sig;
}

float envelope(int *trig, float incr, float decr, float decr2, float amp, float sustain, int *peak, float volume)
{
    if (*trig == 1)
    {
        //Attack
        if (amp < volume && *peak == 0)
            amp += incr;
        //Decay
        else if (*peak == 0 && amp >= volume)
        {
            amp = volume;
            *peak = 1;
        }
        else if (*peak == 1 && amp > sustain)
            amp -= decr2;
        // Sustain
        else
            amp = sustain;
    }
    //Release
    else
    {
        *peak = 0;
        if (amp > 0.0)
        {
            amp -= decr;
        }
        else
            amp = 0.0;
    }
    return amp;
}

float make_digital_saw(float amp, float freq, float signal)
{
    float out = signal;
    out += amp * freq;
    if (out >= amp)
        out = 0 - amp;
    return out;
}

float make_analog_saw(float amp, float freq, unsigned int *time)
{
    float out = 0.0;
    for (float k = 1.0; k < 40; k++)
        out += (sin(k * (2 * PI) * freq * *time)) / k;
    return amp * (out * (2.0 / PI));
}

float make_analog_square(float amp, float freq, unsigned int *time)
{
    float out = 0.0;
    for (float k = 1.0; k < 40; k++)
        if ((int)k % 2 == 1)
        {
            out += (sin(k * (2 * PI) * freq * *time)) / k;
        }
    return amp * (out * (2.0 / PI));
}

float make_digital_square(float amp, float freq, unsigned int *time)
{
    float out;
    out = amp * sin((2 * PI) * freq * *time);

    if (out > 0)
        out = amp;
    if (out < 0)
        out = 0 - amp;

    return out;
}

int error_check(PaError x)
{
    if (x != paNoError)
    {
        printf("PortAudio error: %s\n", Pa_GetErrorText(x));
        return 1;
    }
    else
        return 0;
}
