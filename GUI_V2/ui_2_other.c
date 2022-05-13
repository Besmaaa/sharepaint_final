#include "ui_2.h"

gboolean Mirror(GtkWidget *widget, Canvas *canvas)
{
    return FALSE;
}

gboolean Rotation(GtkWidget *widget, Canvas *canvas)
{
    return FALSE;
}

gboolean Mosaic(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    int n = (int)gtk_range_get_value(GTK_RANGE(widget));

    for (int i = 0; i < canvas->width; i += n)
    {
        for (int j = 0; j < canvas->height; j += n)
        {
            guchar *rgba = get_RGBA(canvas, i, j);

            for (int ii = i; (ii < i + n) && (ii < canvas->width); ii++)
            {
                for (int jj = j; (jj < j + n) && (jj < canvas->height); jj++)
                {
                    put_RGBA(canvas, ii, jj, rgba[0], rgba[1], rgba[2], rgba[3]);
                }
            }

            g_free(rgba);
        }
    }

    canvas->modified = 1;
    return FALSE;
}