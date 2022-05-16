#include "ui_2.h"

gboolean Lightness(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    gdouble n = gtk_range_get_value(GTK_RANGE(widget)) / 2.0;
    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            put_RGBA(canvas, i, j, truncateU(rgba[0] + n), truncateU(rgba[1] + n), truncateU(rgba[2] + n), rgba[3]);

            g_free(rgba);
        }
    }

    canvas->modified = 1;
    return FALSE;
}

gboolean Contrast(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    gdouble n = gtk_range_get_value(GTK_RANGE(widget)) / 2.0;
    double f = (double)(259.0 * (255.0 + n)) / (255.0 * (259.0 - n));

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);

            rgba[0] = truncateU(f * (double)(rgba[0] - 128) + 128);
            rgba[1] = truncateU(f * (double)(rgba[1] - 128) + 128);
            rgba[2] = truncateU(f * (double)(rgba[2] - 128) + 128);

            put_RGBA(canvas, i, j, rgba[0], rgba[1], rgba[2], rgba[3]);

            g_free(rgba);
        }
    }

    canvas->modified = 1;
    return FALSE;
}