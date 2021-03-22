#include "headers/Audio_engine.h"

int main(int argc, char **argv)
{

    // Initialise MIDI timestamps Just to be on the safe side
    engine.MIDI.currentEvent.message = 0x000000;
    engine.MIDI.currentEvent.timestamp = 0;
    // Initialise some audio parameters
    engine.Audio.peak = 0;
    engine.Audio.attack_time = 0.0001;
    engine.Audio.release_time = 0.0001;
    engine.Audio.volume = 0.8;
    engine.Audio.sustain = 0.0001;
    engine.Audio.decay = 0.0001;
    engine.Audio.wave = 4;

    int i;
    // So we can do decimal maths on hexadecimal MIDI info and lookup frequencys quick
    hexp_assign(engine.MIDI.hex_places);
    MIDI_lookup(engine.MIDI.freq_lookup);

    // Intialise Both Port MIDI and Port audio
    PmError merr = Pm_Initialize();
    if (merr != pmNoError)
        printf("ERROR!");

    PaError aerr;
    aerr = Pa_Initialize();
    if (error_check(aerr) == 1)
        return 1;

    PaStream *stream;

    // LIST MIDI OPTIONS:

    printf("Choose Your MIDI Device: \n");
    for (i = 0; i < Pm_CountDevices(); i++)
    {
        const PmDeviceInfo *info = Pm_GetDeviceInfo(i);
        if (info->input)
            printf("%d: %s, %s \n", i, info->interf, info->name);
    }
    // Get the option from the user
    i = get_number("MIDI input device number: ");

    // Open an input stream with the users choice
    // Pass in the engine to the Audio stream too.

    Pm_OpenInput(&in,
                 i,
                 NULL,
                 INPUT_BUFFER,
                 (PmTimestamp(*)(void *))Pt_Time,
                 NULL);

    aerr = Pa_OpenDefaultStream(&stream,
                                0,
                                2,
                                paFloat32,
                                SAMPLE_RATE,
                                AUDIO_OUT_BUFFER_SIZE,
                                patestCallback,
                                &engine);
    if (error_check(aerr) == 1)
        return 1;

    int period = 1;

    // Start the Callback Functions and Pass in the engine

    Pt_Start(period, &pt_callback, &engine);
    aerr = Pa_StartStream(stream);
    if (error_check(aerr) == 1)
        return 1;

    GtkBuilder *builder;
    GtkWidget *window;

    engine.GUI = g_slice_new(GUIpointers);

    gtk_init(&argc, &argv);
    load_css();
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "main.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main"));
    gtk_builder_connect_signals(builder, &engine);

    // Set Up some widgets

    // Oscillator Selections
    engine.GUI->btn_change_osc = GTK_WIDGET(gtk_builder_get_object(builder, "btn_change_osc"));
    engine.GUI->random_osc = GTK_WIDGET(gtk_builder_get_object(builder, "random_osc"));
    engine.GUI->lbl_osc_type = GTK_WIDGET(gtk_builder_get_object(builder, "lbl_osc_type"));

    // ADSR Sliders
    engine.GUI->attack_slider = GTK_WIDGET(gtk_builder_get_object(builder, "attack_slider"));
    engine.GUI->decay_slider = GTK_WIDGET(gtk_builder_get_object(builder, "decay_slider"));
    engine.GUI->sustain_slider = GTK_WIDGET(gtk_builder_get_object(builder, "sustain_slider"));
    engine.GUI->release_slider = GTK_WIDGET(gtk_builder_get_object(builder, "release_slider"));
    engine.GUI->volume_slider = GTK_WIDGET(gtk_builder_get_object(builder, "volume_slider"));

    g_object_unref(builder);
    gtk_widget_show(window);

    // Set Up some parameter ranges
    gtk_range_set_range(GTK_RANGE(engine.GUI->attack_slider), 0.0, 1.5);
    gtk_range_set_inverted(GTK_RANGE(engine.GUI->attack_slider), 1);
    gtk_range_set_range(GTK_RANGE(engine.GUI->decay_slider), 0.0, 1.0);
    gtk_range_set_inverted(GTK_RANGE(engine.GUI->decay_slider), 1);
    gtk_range_set_range(GTK_RANGE(engine.GUI->sustain_slider), 0.0, 1);
    gtk_range_set_inverted(GTK_RANGE(engine.GUI->sustain_slider), 1);
    gtk_range_set_range(GTK_RANGE(engine.GUI->release_slider), 0.0, 2.0);
    gtk_range_set_inverted(GTK_RANGE(engine.GUI->release_slider), 1);
    gtk_range_set_range(GTK_RANGE(engine.GUI->volume_slider), 0.0, 1.0);
    gtk_range_set_inverted(GTK_RANGE(engine.GUI->volume_slider), 0);

    gtk_main();

    // Close up portmidi and return main!
    aerr = Pa_StopStream(stream);
    if (error_check(aerr) == 1)
        return 1;
    aerr = Pa_Terminate();
    if (error_check(aerr) == 1)
        return 1;

    g_slice_free(GUIpointers, engine.GUI);

    Pt_Stop();
    Pm_Close(in);
    Pm_Terminate();
}