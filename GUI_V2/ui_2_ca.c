#include "ui_2.h"

gboolean Horizontal_CA(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    int offset = (int)gtk_range_get_value(GTK_RANGE(widget));

    if (offset > 0)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            color_pix *buf = calloc(canvas->width, sizeof(color_pix));

            for (int i = 0; i + offset < canvas->width; i++)
            {
                guchar *rgba_red = get_RGBA(canvas, i + offset, j);
                buf[i].r = rgba_red[0];

                guchar *rgba_blue = get_RGBA(canvas, i, j);
                buf[i + offset].b = rgba_blue[2];

                g_free(rgba_red);
                g_free(rgba_blue);
            }

            for (int i = 0; i < canvas->width; i++)
            {
                guchar *rgba = get_RGBA(canvas, i, j);
                put_RGBA(canvas, i, j, buf[i].r, rgba[1], buf[i].b, rgba[3]);
                g_free(rgba);
            }

            free(buf);
        }
    }

    else
    {
        offset = fabs(offset);
        for (int j = 0; j < canvas->height; j++)
        {
            color_pix *buf = calloc(canvas->width, sizeof(color_pix));

            for (int i = 0; i + offset < canvas->width; i++)
            {
                guchar *rgba_red = get_RGBA(canvas, i, j);
                buf[i + offset].r = rgba_red[0];

                guchar *rgba_blue = get_RGBA(canvas, i + offset, j);
                buf[i].b = rgba_blue[2];

                g_free(rgba_red);
                g_free(rgba_blue);
            }

            for (int i = 0; i < canvas->width; i++)
            {
                guchar *rgba = get_RGBA(canvas, i, j);
                put_RGBA(canvas, i, j, buf[i].r, rgba[1], buf[i].b, rgba[3]);
                g_free(rgba);
            }

            free(buf);
        }
    }

    canvas->modified = 1;
    return FALSE;
}

gboolean Vertical_CA(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    int offset = (int)gtk_range_get_value(GTK_RANGE(widget));

    if (offset > 0)
    {
        for (int i = 0; i < canvas->width; i++)
        {
            for (int j = 0; j + offset < canvas->height; j++)
            {
                guchar *rgba_green = get_RGBA(canvas, i, j + offset);
                guchar *rgba = get_RGBA(canvas, i, j);
                put_RGBA(canvas, i, j, rgba[0], rgba_green[1], rgba[2], rgba[3]);

                g_free(rgba_green);
                g_free(rgba);
            }
        }
    }

    else
    {
        offset = fabs(offset);

        for (int i = 0; i < canvas->width; i++)
        {
            for (int j = canvas->height - 1; j - offset >= 0; j--)
            {
                guchar *rgba_green = get_RGBA(canvas, i, j - offset);
                guchar *rgba = get_RGBA(canvas, i, j);
                put_RGBA(canvas, i, j, rgba[0], rgba_green[1], rgba[2], rgba[3]);

                g_free(rgba_green);
                g_free(rgba);
            }
        }
    }

    canvas->modified = 1;
    return FALSE;
}