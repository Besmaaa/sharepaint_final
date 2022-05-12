#include "ui_2.h"

gboolean Grayscale(GtkWidget *widget, Canvas *canvas)
{
    guchar average;
    
    //Canvas *canvas = user_data;

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            average = truncateU(0.3 * rgba[0] + 0.59 * rgba[1] + 0.11 * rgba[2]);
            put_RGBA(canvas, i, j, average, average, average, rgba[3]);
            g_free(rgba);

        }
    }

    return FALSE;
}

gboolean Negative(GtkWidget *widget, Canvas *canvas)
{
    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            put_RGBA(canvas, i, j, 255 - rgba[0], 255 - rgba[1], 255 - rgba[2], rgba[3]);
            g_free(rgba);
        }
    }

    return FALSE;
}

gboolean Binarize(GtkWidget *widget, Canvas *canvas)
{
    guchar grey;
    guchar new_pix;

    double s = fmax(canvas->width, canvas->height) / 16;
    double sum;
    double *buff = malloc(canvas->height * canvas->width * sizeof(double));
    
    for (int i = 0; i < canvas->width; i++)
    {
        sum = 0;

        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            grey = truncateU(0.3 * rgba[0] + 0.59 * rgba[1] + 0.11 * rgba[2]);
            sum += grey;

            if (i == 0)
                buff[i * canvas->height + j] = sum;

            else
                buff[i * canvas->height + j] = buff[(i - 1)* canvas->height + j] + sum;

            g_free(rgba);
        }
    }

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            int x1 = fmax(i - s, 1);
            int x2 = fmin(i + s, canvas->width - 1);
            int y1 = fmax(j - s, 1);
            int y2 = fmin(j + s, canvas->height - 1);
            int count = (x2 - x1) * (y2 - y1);

            sum = buff[x2 * canvas->height + y2] - buff[x2 * canvas->height + y1 - 1] - buff[(x1 - 1)* canvas->height + y2] + buff[(x1 - 1) * canvas->height + y1 - 1];
            guchar *rgba = get_RGBA(canvas, i, j);
            grey = truncateU(0.3 * rgba[0] + 0.59 * rgba[1] + 0.11 * rgba[2]);
            if (sum * (100.0 - 15.0) / 100.0 >= grey * count)
                new_pix = 0;

            else
                new_pix = 255;

            put_RGBA(canvas, i, j, new_pix, new_pix, new_pix, 255);

            g_free(rgba);
        }
    }

    free(buff);
    return FALSE;
}

gboolean Sepia(GtkWidget *widget, Canvas *canvas)
{
    guchar r, g, b;

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);
            g_print("R = %d; G = %d; B = %d\n", rgba[0], rgba[1], rgba[2]);
            r = truncateU(0.393 * rgba[0] + 0.769 * rgba[1] + 0.189 * rgba[2]);
            g = truncateU(0.349 * rgba[0] + 0.686 * rgba[1] + 0.168 * rgba[2]);
            b = truncateU(0.272 * rgba[0] + 0.534 * rgba[1] + 0.131 * rgba[2]);
            g_print("NEW R = %d; NEW G = %d; NEW B = %d\n", r, g, b);
            put_RGBA(canvas, i, j, r, g, b, rgba[3]);

            g_free(rgba);
        }
    }

    return FALSE;
}