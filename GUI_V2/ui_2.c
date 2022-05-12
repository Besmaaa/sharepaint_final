#include "ui_2.h"

// Widgets.
GtkWindow *window;
GtkPaned *paned;
// Drawing area.
GtkWidget *drawing_area;
// Menu Bar.
GtkMenuBar *menu_bar;
// Menu Items.
// File.
GtkWidget *load_menu_item;
GtkWidget *save_menu_item;
GtkWidget *quit_menu_item;
// Edit.
GtkWidget *grayscale_menu_item;
GtkWidget *negative_menu_item;
GtkWidget *sepia_menu_item;
GtkWidget *binarize_menu_item;
GtkWidget *lc_menu_item;
GtkWidget *hsv_menu_item;
GtkWidget *ca_menu_item;
GtkWidget *co_menu_item;
GtkWidget *gauss_menu_item;
GtkWidget *motion_menu_item;
GtkWidget *mirror_menu_item;
GtkWidget *rotation_menu_item;
GtkWidget *mosaic_menu_item;
GtkWidget *dithering_menu_item;
// Canvas
Canvas canvas;
// Pixbuf
GdkPixbuf *pixbuf;
// For motion blur.
gint k = 0;
gdouble angle = 0;

static gboolean LoadFile(GtkWidget *file_chooser)
{
    GtkDialog *dialog;
    gint res;

    dialog = gtk_file_chooser_dialog_new("Open File", window, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        pixbuf = gdk_pixbuf_new_from_file(filename, NULL);
        g_assert(pixbuf != NULL);

        if (!gdk_pixbuf_get_has_alpha(pixbuf))
            pixbuf = gdk_pixbuf_add_alpha(pixbuf, FALSE, NULL, NULL, NULL);

        canvas.pixbuf = pixbuf;
        canvas.n_channels = gdk_pixbuf_get_n_channels(canvas.pixbuf);
        canvas.width = gdk_pixbuf_get_width(canvas.pixbuf);
        canvas.height = gdk_pixbuf_get_height(canvas.pixbuf);
        canvas.rowstride = gdk_pixbuf_get_rowstride(canvas.pixbuf);

        canvas.initial = gdk_pixbuf_copy(pixbuf);
        canvas.modified = 0;

        g_free(filename);
    }

    gtk_widget_destroy(GTK_WIDGET(dialog));
    return FALSE;
}

static gboolean SaveFile(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->pixbuf)
        gdk_pixbuf_save(canvas->pixbuf, "new_image", "jpeg", NULL, NULL);

    return FALSE;
}

static gboolean on_draw(GtkWidget *da, cairo_t *cr, gpointer user_data)
{
    if (canvas.pixbuf != NULL)
    {
        gdk_cairo_set_source_pixbuf(cr, canvas.pixbuf, 0, 0);
        cairo_paint(cr);
        cairo_fill(cr);
    }

    int da_width = gtk_widget_get_allocated_width(da);
    int da_height = gtk_widget_get_allocated_height(da);

    gtk_widget_queue_draw_area(da, 0, 0, da_width, da_height);

    return G_SOURCE_REMOVE;
}

void on_quit(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer user_data)
{
    // HEEEEEEEEEEEEE JSP PK YA DES LEAKS TA3 ZEBI
    g_print("Quitting application ...");

    if (canvas.pixbuf != NULL)
        gdk_pixbuf_unref(canvas.pixbuf);
    if (canvas.initial != NULL)
        gdk_pixbuf_unref(canvas.initial);

    gtk_main_quit();
}

gboolean window_delete(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) GdkEvent *event, __attribute__((unused)) gpointer user_data)
{
    g_print("Trying to close window heh ?");
    return TRUE; // Stops window from being deleted.
}

gboolean on_cancel(__attribute__((unused)) GtkWidget *widget, GdkPixbuf *before_modif)
{
    // ANNULE UN SEUL APPLY OUE BON FLEMME DECRIRE EN ANGLAIS FRERO
    if (canvas.modified)
    {
        gdk_pixbuf_unref(canvas.pixbuf);
        canvas.pixbuf = gdk_pixbuf_copy(before_modif);
        gdk_pixbuf_unref(canvas.initial);
        canvas.initial = gdk_pixbuf_copy(before_modif);
        canvas.modified = 0;
        gdk_pixbuf_unref(before_modif);
    }
    return FALSE;
}

gboolean on_apply(__attribute__((unused)) GtkWidget *widget, Canvas *canvas)
{
    gdk_pixbuf_unref(canvas->initial);
    canvas->initial = gdk_pixbuf_copy(canvas->pixbuf);
    canvas->modified = 0;
    return FALSE;
}

gboolean on_scale(__attribute__((unused)) GtkWidget *widget, Canvas *canvas)
{
    return FALSE;
}

//////////////////////////////////////////// Lightness & Contrast ////////////////////////////////////

static gboolean on_lc_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_lc = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_lc, "builders/lc_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_lc, "dialog_lc"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_lc, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_lc, "apply_button"));

    gtk_builder_connect_signals(builder_lc, NULL);

    GtkWidget *l_scale = GTK_WIDGET(gtk_builder_get_object(builder_lc, "l_scale"));
    GtkWidget *c_scale = GTK_WIDGET(gtk_builder_get_object(builder_lc, "c_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_lc);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(l_scale), "value_changed", G_CALLBACK(Lightness), canvas);
    g_signal_connect(G_OBJECT(c_scale), "value_changed", G_CALLBACK(Contrast), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////// HSV /////////////////////////////////////////////////////

static gboolean on_hsv_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_hsv = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_hsv, "builders/hsv_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_hsv, "dialog_hsv"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_hsv, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_hsv, "apply_button"));

    gtk_builder_connect_signals(builder_hsv, NULL);

    GtkWidget *v_scale = GTK_WIDGET(gtk_builder_get_object(builder_hsv, "v_scale"));
    GtkWidget *s_scale = GTK_WIDGET(gtk_builder_get_object(builder_hsv, "s_scale"));
    GtkWidget *h_scale = GTK_WIDGET(gtk_builder_get_object(builder_hsv, "h_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_hsv);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(v_scale), "value_changed", G_CALLBACK(Brightness), canvas);
    g_signal_connect(G_OBJECT(s_scale), "value_changed", G_CALLBACK(Saturation), canvas);
    g_signal_connect(G_OBJECT(h_scale), "value_changed", G_CALLBACK(Hue), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////// Gaussian Blur ///////////////////////////////////////////

static gboolean on_gauss_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_gauss = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_gauss, "builders/gauss_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_gauss, "dialog_gauss"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_gauss, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_gauss, "apply_button"));

    gtk_builder_connect_signals(builder_gauss, NULL);

    GtkWidget *k_scale = GTK_WIDGET(gtk_builder_get_object(builder_gauss, "k_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_gauss);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(k_scale), "value_changed", G_CALLBACK(Gaussian), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////// Motion Blur /////////////////////////////////////////////

gboolean on_k_change(GtkWidget *widget, Canvas *canvas)
{
    k = gtk_range_get_value(GTK_RANGE(widget));
    Motion(canvas, k, angle);

    return FALSE;
}

gboolean on_angle_change(GtkWidget *widget, Canvas *canvas)
{
    angle = gtk_range_get_value(GTK_RANGE(widget));
    Motion(canvas, k, angle);
    return FALSE;
}

static gboolean on_motion_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_motion = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_motion, "builders/motion_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_motion, "dialog_motion"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_motion, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_motion, "apply_button"));

    gtk_builder_connect_signals(builder_motion, NULL);

    GtkWidget *k_scale = GTK_WIDGET(gtk_builder_get_object(builder_motion, "k_scale"));
    GtkWidget *angle_scale = GTK_WIDGET(gtk_builder_get_object(builder_motion, "angle_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_motion);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(k_scale), "value_changed", G_CALLBACK(on_k_change), canvas);
    g_signal_connect(G_OBJECT(angle_scale), "value_changed", G_CALLBACK(on_angle_change), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

///////////////////////////////////////// Channel Operations /////////////////////////////////////////

static gboolean on_co_a(GtkWidget *widget, Canvas *canvas)
{
    char *value = gtk_combo_box_text_get_active_text(widget);
    Channel_Op(canvas, 'A', value);
    g_free(value);
    return FALSE;
}

static gboolean on_co_r(GtkWidget *widget, Canvas *canvas)
{
    char *value = gtk_combo_box_text_get_active_text(widget);
    Channel_Op(canvas, 'R', value);
    g_free(value);
    return FALSE;
}

static gboolean on_co_g(GtkWidget *widget, Canvas *canvas)
{
    char *value = gtk_combo_box_text_get_active_text(widget);
    Channel_Op(canvas, 'G', value);
    g_free(value);
    return FALSE;
}

static gboolean on_co_b(GtkWidget *widget, Canvas *canvas)
{
    char *value = gtk_combo_box_text_get_active_text(widget);
    Channel_Op(canvas, 'B', value);
    g_free(value);
    return FALSE;
}

static gboolean on_co_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_co = gtk_builder_new();
    

    if (gtk_builder_add_from_file(builder_co, "builders/co_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_co, "dialog_co"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_co, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_co, "apply_button"));
    
    gtk_builder_connect_signals(builder_co, NULL);

    GtkComboBoxText *co_a_val = GTK_WIDGET(gtk_builder_get_object(builder_co, "co_a_val"));
    GtkComboBoxText *co_r_val = GTK_WIDGET(gtk_builder_get_object(builder_co, "co_r_val"));
    GtkComboBoxText *co_g_val = GTK_WIDGET(gtk_builder_get_object(builder_co, "co_g_val"));
    GtkComboBoxText *co_b_val = GTK_WIDGET(gtk_builder_get_object(builder_co, "co_b_val"));
    
    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_co);
    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(co_a_val), "changed", G_CALLBACK(on_co_a), canvas);
    g_signal_connect(G_OBJECT(co_r_val), "changed", G_CALLBACK(on_co_r), canvas);
    g_signal_connect(G_OBJECT(co_g_val), "changed", G_CALLBACK(on_co_g), canvas);
    g_signal_connect(G_OBJECT(co_b_val), "changed", G_CALLBACK(on_co_b), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    return FALSE;
}


///////////////////////////////////////// Chromatic Aberration ///////////////////////////////////////

static gboolean on_ca_filter(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_ca = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_ca, "builders/ca_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_ca, "dialog_ca"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_ca, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_ca, "apply_button"));

    gtk_builder_connect_signals(builder_ca, NULL);

    GtkWidget *hor_scale = GTK_WIDGET(gtk_builder_get_object(builder_ca, "hor_scale"));
    GtkWidget *ver_scale = GTK_WIDGET(gtk_builder_get_object(builder_ca, "ver_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_ca);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(hor_scale), "value-changed", G_CALLBACK(Horizontal_CA), canvas);
    g_signal_connect(G_OBJECT(ver_scale), "value-changed", G_CALLBACK(Vertical_CA), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

////////////////////////////////////////////// Rotation //////////////////////////////////////////////

static gboolean on_rotation(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_rot = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_rot, "builders/rot_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_rot, "dialog_rot"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_rot, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_rot, "apply_button"));

    gtk_builder_connect_signals(builder_rot, NULL);

    GtkWidget *rot_scale = GTK_WIDGET(gtk_builder_get_object(builder_rot, "rot_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_rot);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(rot_scale), "value-changed", G_CALLBACK(Rotation), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

////////////////////////////////////////////// Mosaic //////////////////////////////////////////////

static gboolean on_mosaic(GtkWidget *widget, Canvas *canvas)
{
    gint res;
    GtkBuilder *builder_mosaic = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_mosaic, "builders/mosaic_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_mosaic, "dialog_mosaic"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_mosaic, "cancel_button"));
    GtkWidget *apply_button = GTK_WIDGET(gtk_builder_get_object(builder_mosaic, "apply_button"));

    gtk_builder_connect_signals(builder_mosaic, NULL);

    GtkWidget *mosaic_scale = GTK_WIDGET(gtk_builder_get_object(builder_mosaic, "mosaic_scale"));

    GdkPixbuf *before_modif = gdk_pixbuf_copy(canvas->pixbuf);

    g_object_unref(builder_mosaic);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), before_modif);
    g_signal_connect(G_OBJECT(apply_button), "clicked", G_CALLBACK(on_apply), canvas);
    g_signal_connect(G_OBJECT(mosaic_scale), "value-changed", G_CALLBACK(Mosaic), canvas);

    res = gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// MAIN //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    gtk_init(&argc, &argv); // Init.

    GError *err = NULL;
    // Builder.
    GtkBuilder *builder = gtk_builder_new();

    if (gtk_builder_add_from_file(builder, "builders/ui_2.glade", &err) == 0)
    {
        g_printerr("Error loading file :%s\n", err->message);
        g_clear_error(&err);
        return 1;
    }

    // Get widgets.
    window = GTK_WINDOW(gtk_builder_get_object(builder, "window"));
    paned = GTK_PANED(gtk_builder_get_object(builder, "paned"));
    menu_bar = GTK_MENU_BAR(gtk_builder_get_object(builder, "menu_bar"));

    gtk_builder_connect_signals(builder, NULL); // Connect signals.

    drawing_area = GTK_WIDGET(gtk_builder_get_object(builder, "drawing_area"));

    load_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "load_menu_item"));
    save_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "save_menu_item"));
    quit_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "quit_menu_item"));

    grayscale_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "grayscale_menu_item"));
    negative_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "negative_menu_item"));
    sepia_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "sepia_menu_item"));
    binarize_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "binarize_menu_item"));

    lc_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "lc_menu_item"));
    hsv_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "hsv_menu_item"));
    ca_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "ca_menu_item"));
    co_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "co_menu_item"));
    gauss_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "gauss_menu_item"));
    motion_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "motion_menu_item"));
    mirror_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "mirror_menu_item"));
    rotation_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "rotation_menu_item"));
    mosaic_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "mosaic_menu_item"));
    dithering_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "dithering_menu_item"));


    g_object_unref(builder);

    // Connect signals.
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_quit), NULL);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(on_draw), NULL);
    g_signal_connect(G_OBJECT(load_menu_item), "activate", G_CALLBACK(LoadFile), NULL);
    g_signal_connect(G_OBJECT(save_menu_item), "activate", G_CALLBACK(SaveFile), &canvas);
    g_signal_connect(G_OBJECT(quit_menu_item), "activate", G_CALLBACK(on_quit), NULL);

    g_signal_connect(G_OBJECT(grayscale_menu_item), "activate", G_CALLBACK(Grayscale), &canvas);
    g_signal_connect(G_OBJECT(negative_menu_item), "activate", G_CALLBACK(Negative), &canvas);
    g_signal_connect(G_OBJECT(sepia_menu_item), "activate", G_CALLBACK(Sepia), &canvas);
    g_signal_connect(G_OBJECT(binarize_menu_item), "activate", G_CALLBACK(Binarize), &canvas);

    g_signal_connect(G_OBJECT(lc_menu_item), "activate", G_CALLBACK(on_lc_filter), &canvas);
    g_signal_connect(G_OBJECT(hsv_menu_item), "activate", G_CALLBACK(on_hsv_filter), &canvas);
    g_signal_connect(G_OBJECT(ca_menu_item), "activate", G_CALLBACK(on_ca_filter), &canvas);
    g_signal_connect(G_OBJECT(co_menu_item), "activate", G_CALLBACK(on_co_filter), &canvas);
    g_signal_connect(G_OBJECT(gauss_menu_item), "activate", G_CALLBACK(on_gauss_filter), &canvas);
    g_signal_connect(G_OBJECT(motion_menu_item), "activate", G_CALLBACK(on_motion_filter), &canvas);
    g_signal_connect(G_OBJECT(mirror_menu_item), "activate", G_CALLBACK(Mirror), &canvas);
    g_signal_connect(G_OBJECT(rotation_menu_item), "activate", G_CALLBACK(on_rotation), &canvas);
    g_signal_connect(G_OBJECT(mosaic_menu_item), "activate", G_CALLBACK(on_mosaic), &canvas);
    g_signal_connect(G_OBJECT(dithering_menu_item), "activate", G_CALLBACK(Dithering), &canvas);

    gtk_widget_set_app_paintable(drawing_area, TRUE);
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    return EXIT_SUCCESS;
}