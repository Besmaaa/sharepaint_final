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
GtkWidget *median_menu_item;
GtkWidget *deinter_odd_menu_item;
GtkWidget *deinter_even_menu_item;

GtkWidget *login_menu_item;
GtkWidget *signup_menu_item;
GtkWidget *logout_menu_item;

GtkWidget *create_menu_item;
GtkWidget *push_menu_item;
GtkWidget *pull_menu_item;
GtkWidget *addpic_menu_item;

GtkWidget *username_entry;
GtkWidget *password_entry;

GtkWidget *project_name;

Point *brush;
GtkColorSelection *color_selection;
GtkWidget *brush_size;

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

    if (brush == NULL)
        return FALSE;

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

gboolean on_cancel(__attribute__((unused)) GtkWidget *widget, GdkPixbuf *before_modif)
{
    if (before_modif)
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

gboolean on_undo(__attribute__((unused)) GtkWidget *widget, CanvasList *canvas)
{
    return FALSE;
}

gboolean on_scale(__attribute__((unused)) GtkWidget *widget, Canvas *canvas)
{
    return FALSE;
}

gboolean entry_changed(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer user_data)
{
    return FALSE;
}

//////////////////////////////////////////// Log In & Sign Up ////////////////////////////////////
gboolean destroy_dialog(GtkWidget *widget, GtkDialog *dialog)
{
    gtk_widget_destroy(dialog);
    return FALSE;
}

gboolean send_user_info(GtkWidget *widget, UserInfo* user_info)
{
    user_info->username = "zebi";
    user_info->password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // if le serv veut pas
    GtkBuilder *builder_fail = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_fail, "builders/login_failed_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkWindow *window_fail = GTK_WIDGET(gtk_builder_get_object(builder_fail, "window_fail"));
    gtk_builder_connect_signals(builder_fail, NULL);
    gtk_widget_show_all(window);



    /*if(user_info->fct == 0)
        login(user_info->cfd, user_info->username, user_info->password, &user_info->token);

    else if(user_info->fct == 1)
        signup(user_info->cfd, &user_info->username, &user_info->password);

    printf("infos af loged : %s,%s,%d \n",user_info->username,user_info->password,user_info->token); */
    return FALSE;
}

gboolean on_login(__attribute__((unused)) GtkWidget *widget, UserInfo* user_data)
{
    
    user_data->fct = 0;

    GtkBuilder *builder_log = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_log, "builders/login_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_log, "dialog_login"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_log, "cancel_button"));
    GtkWidget *login_button = GTK_WIDGET(gtk_builder_get_object(builder_log, "login_button"));
    username_entry = GTK_WIDGET(gtk_builder_get_object(builder_log, "username_entry"));
    password_entry = GTK_WIDGET(gtk_builder_get_object(builder_log, "password_entry"));

    gtk_builder_connect_signals(builder_log, NULL);

    g_object_unref(builder_log);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), NULL);
    g_signal_connect(G_OBJECT(login_button), "clicked", G_CALLBACK(send_user_info), user_data);
    g_signal_connect(G_OBJECT(username_entry), "changed", G_CALLBACK(entry_changed), NULL);
    g_signal_connect(G_OBJECT(password_entry), "changed", G_CALLBACK(entry_changed), NULL);
    
    gtk_dialog_run(GTK_DIALOG(dialog));

    
    printf("infos af loged : %s,%s,%d \n",user_data->username,user_data->password,user_data->token); 
    return FALSE;
}

gboolean on_signup(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer user_data)
{
    GtkBuilder *builder_sign = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_sign, "builders/signup_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_sign, "dialog_signup"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_sign, "cancel_button"));
    GtkWidget *signup_button = GTK_WIDGET(gtk_builder_get_object(builder_sign, "signup_button"));
    username_entry = GTK_WIDGET(gtk_builder_get_object(builder_sign, "username_entry"));
    password_entry = GTK_WIDGET(gtk_builder_get_object(builder_sign, "password_entry"));

    gtk_builder_connect_signals(builder_sign, NULL);

    g_object_unref(builder_sign);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(destroy_dialog), dialog);
    g_signal_connect(G_OBJECT(signup_button), "clicked", G_CALLBACK(send_user_info), dialog);
    g_signal_connect(G_OBJECT(username_entry), "changed", G_CALLBACK(entry_changed), NULL);
    g_signal_connect(G_OBJECT(password_entry), "changed", G_CALLBACK(entry_changed), NULL);

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

gboolean on_logout(__attribute__((unused)) GtkWidget *widget, __attribute__((unused)) gpointer user_data)
{
    return FALSE;
}

//////////////////////////////////////////// Project /////////////////////////////////////////////

gboolean create_new_project(GtkWidget *widget, UserInfo* user_info)
{
    g_print("yoooo\n");
    //canvas = *(user_info->canvas);
    Project project;
    project.name = gtk_entry_get_text(GTK_ENTRY(project_name));
    user_info->proj = gtk_entry_get_text(GTK_ENTRY(project_name));
    printf("before canvas assign\n");
    project.displayed_canvas = &canvas;
    project.id_users = NULL;
    printf("before pcreate call\n");
    
    g_assert(user_info->username != NULL);
    
    g_print("Username : %s\n",user_info->username); 
    project_create(user_info->cfd,user_info->proj,user_info->username,&user_info->token);
    printf("after pcreate\n");
    return FALSE;
}

gboolean on_create_project(GtkWidget *widget, UserInfo* user_info)
{
    g_print("infos bf pcreate: %s\n",user_info->username); 
    GtkBuilder *builder_create = gtk_builder_new();

    if (gtk_builder_add_from_file(builder_create, "builders/create_project_window.glade", NULL) == 0)
    {
        return 1;
    }

    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder_create, "dialog_create_proj"));
    GtkWidget *cancel_button = GTK_WIDGET(gtk_builder_get_object(builder_create, "cancel_button"));
    GtkWidget *create_button = GTK_WIDGET(gtk_builder_get_object(builder_create, "create_button"));
    project_name = GTK_WIDGET(gtk_builder_get_object(builder_create, "project_name"));

    gtk_builder_connect_signals(builder_create, NULL);

    g_object_unref(builder_create);

    g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(on_cancel), NULL);
    g_signal_connect(G_OBJECT(create_button), "clicked", G_CALLBACK(create_new_project), user_info);
    g_signal_connect(G_OBJECT(project_name), "changed", G_CALLBACK(entry_changed), NULL);

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

gboolean on_pull_project(GtkWidget *widget, Canvas *canvas)
{
    // Pull project
    return FALSE;
}

gboolean on_push_project(GtkWidget *widget, Canvas *canvas)
{
    // Push project
    return FALSE;
}

gboolean on_add_pic(GtkWidget *widget, Canvas *canvas)
{
    // Add picture
    return FALSE;
}

//////////////////////////////////////////// Drawing /////////////////////////////////////////////

gboolean on_color_change(GtkColorSelection *widget, Point *brush)
{
    gtk_color_selection_get_current_rgba(widget, brush->color);
    return FALSE;
}

gboolean on_brush_size_change(GtkWidget *widget, Point *brush)
{
    brush->size = gtk_range_get_value(GTK_RANGE(widget));
    return FALSE;
}

static void draw_brush(GtkWidget *widget, gdouble x, gdouble y)
{
    if (brush == NULL)
    {
        printf("out of memory\n");
        abort();
    }

    brush->x = x;
    brush->y = y;
    g_print("x = %d\n", x);
    g_print("y = %d\n", y);
    brush->next = brush;
    gtk_widget_queue_draw(drawing_area);
}

gboolean on_draw1_button_press_event(GtkWidget *widget, GdkEventButton *event)
{
    g_print("hey");
    draw_brush(widget, event->x, event->y);
    return TRUE;
}

gboolean on_draw1_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    g_print("heyyy");
    if (event->state & GDK_BUTTON1_MASK)
        draw_brush(widget, event->x, event->y);

    return TRUE;
}

//////////////////////////////////////////// Lightness & Contrast ////////////////////////////////////

static gboolean on_lc_filter(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////// HSV /////////////////////////////////////////////////////

static gboolean on_hsv_filter(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////// Gaussian Blur ///////////////////////////////////////////

static gboolean on_gauss_filter(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

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

    gtk_dialog_run(GTK_DIALOG(dialog));

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

    gtk_dialog_run(GTK_DIALOG(dialog));
    return FALSE;
}

///////////////////////////////////////// Chromatic Aberration ///////////////////////////////////////

static gboolean on_ca_filter(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

////////////////////////////////////////////// Rotation //////////////////////////////////////////////

static gboolean on_rotation(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

////////////////////////////////////////////// Mosaic //////////////////////////////////////////////

static gboolean on_mosaic(GtkWidget *widget, Canvas *canvas)
{
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

    gtk_dialog_run(GTK_DIALOG(dialog));

    return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////// MAIN //////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    /////////////////CLOUD////////////
    UserInfo user_info;
    int cfd = cloud_launch();
    user_info.token = -1;
    user_info.cfd = cfd; 
    user_info.fct = 1;
    user_info.username = malloc(64*sizeof(char));
    user_info.password = malloc(64*sizeof(char));
    user_info.proj = malloc(64*sizeof(char)); 
    ////////////////////////////////

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
    brush = malloc(sizeof(struct Point));

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
    median_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "median_menu_item"));
    deinter_odd_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "deinter_odd_menu_item"));
    deinter_even_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "deinter_even_menu_item"));

    signup_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "signup_menu_item"));
    login_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "login_menu_item"));
    logout_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "logout_menu_item"));

    create_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "create_menu_item"));
    push_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "push_menu_item"));
    pull_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "pull_menu_item"));
    addpic_menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "addpic_menu_item"));

    color_selection = GTK_WIDGET(gtk_builder_get_object(builder, "color_selection"));
    brush_size = GTK_WIDGET(gtk_builder_get_object(builder, "brush_size"));

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
    g_signal_connect(G_OBJECT(median_menu_item), "activate", G_CALLBACK(MedianFiltering), &canvas);

    g_signal_connect(G_OBJECT(deinter_odd_menu_item), "activate", G_CALLBACK(DeinterlacingSameSceneOdd), &canvas);
    g_signal_connect(G_OBJECT(deinter_even_menu_item), "activate", G_CALLBACK(DeinterlacingSameSceneEven), &canvas);

    g_signal_connect(G_OBJECT(signup_menu_item), "activate", G_CALLBACK(on_signup), &user_info);
    g_signal_connect(G_OBJECT(login_menu_item), "activate", G_CALLBACK(on_login), &user_info);
    g_signal_connect(G_OBJECT(logout_menu_item), "activate", G_CALLBACK(on_logout), &user_info);

    g_signal_connect(G_OBJECT(create_menu_item), "activate", G_CALLBACK(on_create_project), &user_info);
    g_signal_connect(G_OBJECT(push_menu_item), "activate", G_CALLBACK(on_push_project), &canvas);
    g_signal_connect(G_OBJECT(pull_menu_item), "activate", G_CALLBACK(on_pull_project), &canvas);
    g_signal_connect(G_OBJECT(addpic_menu_item), "activate", G_CALLBACK(on_add_pic), &canvas);

    g_signal_connect(G_OBJECT(color_selection), "color-changed", G_CALLBACK(on_color_change), brush);
    g_signal_connect(G_OBJECT(brush_size), "value-changed", G_CALLBACK(on_brush_size_change), brush);

    gtk_widget_set_events(drawing_area, GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK);
    gtk_widget_set_app_paintable(drawing_area, TRUE);
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_main();

    return EXIT_SUCCESS;
}