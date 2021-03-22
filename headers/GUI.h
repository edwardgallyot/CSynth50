#include "MIDI_engine.h"

void main_destroy_cb()
{
    gtk_main_quit();
}

void on_volume_slider_value_changed(GtkRange *attack_slider)
{
    engine.Audio.volume = (float)gtk_range_get_value(attack_slider);
}
void on_attack_slider_value_changed(GtkRange *attack_slider)
{
    engine.Audio.attack_time = (float)gtk_range_get_value(attack_slider);
}
void on_decay_slider_value_changed(GtkRange *decay_slider)
{
    engine.Audio.decay = (float)gtk_range_get_value(decay_slider);
}
void on_sustain_slider_value_changed(GtkRange *sustain_slider)
{
    engine.Audio.sustain = (float)gtk_range_get_value(sustain_slider);
}
void on_release_slider_value_changed(GtkRange *release_slider)
{
    engine.Audio.release_time = (float)gtk_range_get_value(release_slider);
}

void on_btn_change_osc_clicked(GtkButton *btn_change_osc, GtkLabel *lbl_osc_type)
{
    if (engine.Audio.wave == 4)
        engine.Audio.wave = 1;
    else
        engine.Audio.wave += 1;

    if (engine.Audio.wave == 1)
    {
        gtk_label_set_text(lbl_osc_type, "Analog Square");
    }
    else if (engine.Audio.wave == 2)
    {
        gtk_label_set_text(lbl_osc_type, "Digital Square");
    }
    else if (engine.Audio.wave == 3)
    {
        gtk_label_set_text(lbl_osc_type, "Digital Saw");
    }
    else
    {
        gtk_label_set_text(lbl_osc_type, "Analog Saw");
    }
}

void on_random_osc_clicked(GtkButton *random_osc, GtkLabel *lbl_osc_type)
{
    gboolean result, result2;

    result = g_random_boolean();
    result2 = g_random_boolean();

    if (result == TRUE)
    {
        if (result2 == true)
        {
            engine.Audio.wave = 3;
            gtk_label_set_text(lbl_osc_type, "Digital Saw");
        }
        else
        {
            engine.Audio.wave = 1;
            gtk_label_set_text(lbl_osc_type, "Analog Square");
        }
    }

    else
    {
        if (result2 == true)
        {
            engine.Audio.wave = 2;
            gtk_label_set_text(lbl_osc_type, "Digital Square");
        }
        if (result2 == false)
        {
            engine.Audio.wave = 4;
            gtk_label_set_text(lbl_osc_type, "Analog Saw");
        }
    }
}

static void load_css(void)
{
    GtkCssProvider *provider;
    GdkDisplay *display;
    GdkScreen *screen;

    const gchar *css_style_file = "style.css";
    GFile *css_fp = g_file_new_for_path(css_style_file);
    GError *error = 0;

    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen(display);

    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    gtk_css_provider_load_from_file(provider, css_fp, &error);

    g_object_unref(provider);
}
