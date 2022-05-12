#include "ui_2.h"


gboolean Saturation(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    gdouble n = gtk_range_get_value(GTK_RANGE(widget)) / 100.0;

    gdouble *hsv = g_malloc(3 * sizeof(gdouble));

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            RGBToHSV(rgba, hsv);

            if (n > 1.0)
                hsv[1] += n * (255.0 - hsv[1]) * (hsv[1] / 255.0);

            else
                hsv[1] += n * hsv[1];

            if (hsv[1] > 1.0)
                hsv[1] = 1.0;

            else if (hsv[1] < 0.0)
                hsv[1] = 0.0;

            HSVToRGB(hsv, rgba);
            put_RGBA(canvas, i, j, rgba[0], rgba[1], rgba[2], rgba[3]);

            g_free(rgba);
        }
    }

    g_free(hsv);
    canvas->modified = 1;
    return FALSE;
}

gboolean Brightness(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    gdouble n = gtk_range_get_value(GTK_RANGE(widget)) / 100.0;

    gdouble *hsv = g_malloc(3 * sizeof(gdouble));

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            RGBToHSV(rgba, hsv);

            if (n > 1.0)
                hsv[2] += n * (255.0 - hsv[2]) * (hsv[2] / 255.0);

            else
                hsv[2] += n * hsv[2];

            if (hsv[2] > 1.0)
                hsv[2] = 1.0;

            else if (hsv[2] < 0.0)
                hsv[2] = 0.0;

            HSVToRGB(hsv, rgba);
            put_RGBA(canvas, i, j, rgba[0], rgba[1], rgba[2], rgba[3]);

            g_free(rgba);
        }
    }

    g_free(hsv);
    canvas->modified = 1;
    return FALSE;
}

gboolean Hue(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);
        
    gdouble n = gtk_range_get_value(GTK_RANGE(widget));

    gdouble *hsv = g_malloc(3 * sizeof(gdouble));

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            RGBToHSV(rgba, hsv);
            hsv[0] = fmod(hsv[0] + fabs(n), 360.0);
            
            HSVToRGB(hsv, rgba);
            put_RGBA(canvas, i, j, rgba[0], rgba[1], rgba[2], rgba[3]);

            g_free(rgba);
        }
    }

    g_free(hsv);
    canvas->modified = 1;
    return FALSE;
}