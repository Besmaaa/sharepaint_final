#include "ui_2.h"

/////////////////////////////////////// Dithering //////////////////////////////////////////////

static double *rgbToLab(guchar R, guchar G, guchar B)
{
    double *xyz = calloc(3, sizeof(double));
    double *lab = calloc(3, sizeof(double));
    double *rgb = calloc(3, sizeof(double));

    rgb[0] = R / 255.0f;
    rgb[1] = G / 255.0f;
    rgb[2] = B / 255.0f;

    if (rgb[0] > .04045f)
        rgb[0] = pow((rgb[0] + .055) / 1.055, 2.4);

    else
        rgb[0] = rgb[0] / 12.92f;

    if (rgb[1] > .04045f)
        rgb[1] = pow((rgb[1] + .055) / 1.055, 2.4);

    else
        rgb[1] = rgb[1] / 12.92f;

    if (rgb[2] > .04045f)
        rgb[2] = pow((rgb[2] + .055) / 1.055, 2.4);

    else
        rgb[2] = rgb[2] / 12.92f;

    rgb[0] *= 100.0f;
    rgb[1] *= 100.0f;
    rgb[2] *= 100.0f;

    xyz[0] = ((rgb[0] * .412453f) + (rgb[1] * .357580f) + (rgb[2] * .180423f));
    xyz[1] = ((rgb[0] * .212671f) + (rgb[1] * .715160f) + (rgb[2] * .072169f));
    xyz[2] = ((rgb[0] * .019334f) + (rgb[1] * .119193f) + (rgb[2] * .950227f));

    xyz[0] /= 95.047f;
    xyz[1] /= 100.0f;
    xyz[2] /= 108.883f;

    if (xyz[0] > .008856f)
        xyz[0] = pow(xyz[0], (1.0 / 3.0));

    else
        xyz[0] = (xyz[0] * 7.787f) + (16.0f / 116.0f);

    if (xyz[1] > .008856f)
        xyz[1] = pow(xyz[1], 1.0 / 3.0);

    else
        xyz[1] = (xyz[1] * 7.787f) + (16.0f / 116.0f);

    if (xyz[2] > .008856f)
        xyz[2] = pow(xyz[2], 1.0 / 3.0);

    else
        xyz[2] = (xyz[2] * 7.787f) + (16.0f / 116.0f);

    lab[0] = (116.0f * xyz[1]) - 16.0f;
    lab[1] = 500.0f * (xyz[0] - xyz[1]);
    lab[2] = 200.0f * (xyz[1] - xyz[2]);

    free(rgb);
    free(xyz);

    return lab;
}

static double labDist(double l1, double a1, double b1, double l2, double a2, double b2)
{
    double deltaL = l1 - l2;
    double deltaA = a1 - a2;
    double deltaB = b1 - b2;

    double c1 = sqrt(pow(a1, 2) + pow(b1, 2));
    double c2 = sqrt(pow(a2, 2) + pow(b2, 2));
    double deltaC = c1 - c2;

    double deltaH = pow(deltaA, 2) + pow(deltaB, 2) - pow(deltaC, 2);
    deltaH = deltaH < 0 ? 0 : sqrt(deltaH);

    double sl = 1.0;
    double kc = 1.0;
    double kh = 1.0;

    double Kl = 1.0;
    double K1 = .045;
    double K2 = .015;

    double sc = 1.0 + K1 * c1;
    double sh = 1.0 + K2 * c1;

    double i = pow(deltaL / (Kl * sl), 2) + pow(deltaC / (kc * sc), 2) + pow(deltaH / (kh * sh), 2);
    double finalResult = i < 0 ? 0 : sqrt(i);

    return finalResult;
}

color_pix find_closest_from_palette(guchar *rgba, Palette palette)
{
    double error = 1e99;
    int index = 0;

    for (int c = 0; c < palette.length; c++)
    {
        double *lab_pixel = rgbToLab(rgba[0], rgba[1], rgba[2]);
        double *lab_palette = rgbToLab(palette.colors[c].r, palette.colors[c].g, palette.colors[c].b);
        double dist = labDist(lab_pixel[0], lab_pixel[1], lab_pixel[2], lab_palette[0], lab_palette[1], lab_palette[2]);
        if (error > dist)
        {
            error = dist;
            index = c;
        }

        free(lab_pixel);
        free(lab_palette);
    }

    return palette.colors[index];
}

gboolean Dithering(GtkWidget *widget, Canvas *canvas)
{
    guchar *rgba = malloc(4 * sizeof(guchar));

    Palette *palette = create_palette_from_file("palettes/3_bit_pal", 8);

    for (int j = 0; j < canvas->height; j++)
    {
        for (int i = 0; i < canvas->width; i++)
        {
            get_RGBA_given_guchar(canvas, i, j, rgba);
            color_pix old_color;
            old_color.r = rgba[0];
            old_color.g = rgba[1];
            old_color.b = rgba[2];

            color_pix new_color = find_closest_from_palette(rgba, *palette);
            put_RGBA(canvas, i, j, new_color.r, new_color.g, new_color.b, 255);

            color_pix err_pix = subToColorPix(old_color, new_color);

            if (i + 1 < canvas->width)
            {
                get_RGBA_given_guchar(canvas, i + 1, j, rgba);
                color_pix old_color;
                old_color.r = rgba[0];
                old_color.g = rgba[1];
                old_color.b = rgba[2];
                old_color = addToColorPix(old_color, multColorPix(err_pix, 7.0 / 16.0));
                put_RGBA(canvas, i + 1, j, old_color.r, old_color.g, old_color.b, rgba[3]);
            }

            if (j + 1 < canvas->height)
            {
                if (i - 1 > 0)
                {
                    get_RGBA_given_guchar(canvas, i - 1, j + 1, rgba);
                    color_pix old_color;
                    old_color.r = rgba[0];
                    old_color.g = rgba[1];
                    old_color.b = rgba[2];
                    old_color = addToColorPix(old_color, multColorPix(err_pix, 3.0 / 16.0));
                    put_RGBA(canvas, i - 1, j + 1, old_color.r, old_color.g, old_color.b, rgba[3]);
                }

                get_RGBA_given_guchar(canvas, i, j + 1, rgba);
                color_pix old_color;
                old_color.r = rgba[0];
                old_color.g = rgba[1];
                old_color.b = rgba[2];
                old_color = addToColorPix(old_color, multColorPix(err_pix, 5.0 / 16.0));
                put_RGBA(canvas, i, j + 1, old_color.r, old_color.g, old_color.b, rgba[3]);

                if (i + 1 < canvas->width)
                {
                    get_RGBA_given_guchar(canvas, i + 1, j + 1, rgba);
                    color_pix old_color;
                    old_color.r = rgba[0];
                    old_color.g = rgba[1];
                    old_color.b = rgba[2];
                    old_color = addToColorPix(old_color, multColorPix(err_pix, 1.0 / 16.0));
                    put_RGBA(canvas, i + 1, j + 1, old_color.r, old_color.g, old_color.b, rgba[3]);
                }
            }
        }
    }

    g_free(rgba);
    palette_free(palette);
    return FALSE;
}

/////////////////////////////////////// Noise Reduction /////////////////////////////////////////

static void sort_guchar_list(guchar *list)
{
    int i = 0;

    while (i + 1 < 9)
    {
        if (list[i] > list[i + 1])
        {
            guchar tmp = list[i];
            list[i] = list[i + 1];
            list[i + 1] = tmp;
            i = 0;
        }

        else
            i++;
    }
}

static color_pix find_median(color_pix *color_list)
{
    guchar *reds = malloc(9 * sizeof(guchar));
    guchar *greens = malloc(9 * sizeof(guchar));
    guchar *blues = malloc(9 * sizeof(guchar));

    for (int i = 0; i < 9; i++)
    {
        reds[i] = color_list[i].r;
        greens[i] = color_list[i].g;
        blues[i] = color_list[i].b;
    }

    free(color_list);

    sort_guchar_list(reds);
    sort_guchar_list(greens);
    sort_guchar_list(blues);

    color_pix res;
    res.r = reds[4];
    res.g = greens[4];
    res.b = blues[4];

    g_free(reds);
    g_free(greens);
    g_free(blues);

    return res;
}

gboolean MedianFiltering(GtkWidget *widget, Canvas *canvas)
{
    guchar *rgba = malloc(4 * sizeof(guchar));

    for (int j = 1; j < canvas->height - 1; j++)
    {
        for (int i = 1; i < canvas->width - 1; i++)
        {
            color_pix *median = malloc(9 * sizeof(color_pix));

            rgba = get_RGBA_given_guchar(canvas, i - 1, j - 1, rgba);
            median[0].r = rgba[0];
            median[0].g = rgba[1];
            median[0].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i, j - 1, rgba);
            median[1].r = rgba[0];
            median[1].g = rgba[1];
            median[1].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i + 1, j - 1, rgba);
            median[2].r = rgba[0];
            median[2].g = rgba[1];
            median[2].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i - 1, j, rgba);
            median[3].r = rgba[0];
            median[3].g = rgba[1];
            median[3].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i + 1, j, rgba);
            median[5].r = rgba[0];
            median[5].g = rgba[1];
            median[5].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i - 1, j + 1, rgba);
            median[6].r = rgba[0];
            median[6].g = rgba[1];
            median[6].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i, j + 1, rgba);
            median[7].r = rgba[0];
            median[7].g = rgba[1];
            median[7].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i + 1, j + 1, rgba);
            median[8].r = rgba[0];
            median[8].g = rgba[1];
            median[8].b = rgba[2];

            rgba = get_RGBA_given_guchar(canvas, i, j, rgba);
            median[4].r = rgba[0];
            median[4].g = rgba[1];
            median[4].b = rgba[2];


            color_pix med = find_median(median);

            put_RGBA(canvas, i, j, med.r, med.g, med.b, rgba[3]);
        }
    }

    g_free(rgba);
    return FALSE;
}

/////////////////////////////////////// Deblur /////////////////////////////////////////////////

gboolean WienerDeblur(GtkWidget *widget, Canvas *canvas)
{
    // Y = H * X + N :
    // Y : Blurred image, X : Original image

    return FALSE;
}
