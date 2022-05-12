#include "ui_2.h"

guchar guchar_min_3(guchar a, guchar b, guchar c)
{
    guchar min = a;

    if (min > b)
        min = b;
    if (min > c)
        min = c;

    return min;
}

guchar guchar_max_3(guchar a, guchar b, guchar c)
{
    guchar min = a;

    if (min < b)
        min = b;
    if (min < c)
        min = c;

    return min;
}

guchar truncateU(gint n)
{
    if (n > 255)
        return 255;
    else if (n < 0)
        return 0;

    return (guchar)n;
}

Canvas *return_to_initial(Canvas *canvas)
{
    gdk_pixbuf_unref(canvas->pixbuf);
    canvas->pixbuf = gdk_pixbuf_copy(canvas->initial);
    canvas->modified = 0;
    return canvas;
}

void put_RGBA(Canvas *canvas, int x, int y, guchar red, guchar green, guchar blue, guchar alpha)
{
    guchar *pixels, *p;

    g_assert(x >= 0 && x < canvas->width);
    g_assert(y >= 0 && y < canvas->height);

    pixels = gdk_pixbuf_get_pixels(canvas->pixbuf);

    p = pixels + y * canvas->rowstride + x * canvas->n_channels;
    p[0] = red;
    p[1] = green;
    p[2] = blue;
    p[3] = alpha;
}

guchar *get_RGBA(Canvas *canvas, int x, int y)
{
    guchar *rgba = g_malloc(4 * sizeof(guchar));

    guchar *pixels, *p;

    g_assert(x >= 0 && x < canvas->width);
    g_assert(y >= 0 && y < canvas->height);

    pixels = gdk_pixbuf_get_pixels(canvas->pixbuf);

    p = pixels + y * canvas->rowstride + x * canvas->n_channels;

    for (int i = 0; i < 4; i++)
        rgba[i] = p[i];

    // g_free(pixels);
    // g_free(p);

    return rgba;
}

guchar *get_RGBA_given_guchar(Canvas *canvas, int x, int y, guchar *rgba)
{
    guchar *pixels, *p;

    g_assert(x >= 0 && x < canvas->width);
    g_assert(y >= 0 && y < canvas->height);

    pixels = gdk_pixbuf_get_pixels(canvas->pixbuf);

    p = pixels + y * canvas->rowstride + x * canvas->n_channels;

    for (int i = 0; i < 4; i++)
        rgba[i] = p[i];

    // g_free(pixels);
    // g_free(p);
    return rgba;
}

guchar *histogram(Canvas *canvas)
{
    guchar average;
    guchar *rgba;

    guchar *histo = g_malloc(256 * sizeof(guchar));

    for (int x = 0; x < 256; x++)
        histo[x] = 0;

    for (int i = 0; i < canvas->width; i++)
    {
        for (int j = 0; j < canvas->height; j++)
        {
            rgba = get_RGBA(canvas, i, j);
            average = 0.3 * rgba[0] + 0.59 * rgba[1] + 0.11 * rgba[2];

            histo[truncateU(average)] += 1;
        }
    }

    g_free(rgba);
    return histo;
}

guchar threshold(Canvas *canvas, guchar *histogram)
{
    gdouble *probability = g_malloc(256 * sizeof(gdouble));
    gdouble *mean = g_malloc(256 * sizeof(gdouble));
    gdouble max_between;
    gdouble *between = g_malloc(256 * sizeof(gdouble));
    gint threshold;

    /*
    probability = class probability
    mean = class mean
    between = between class variance
    */

    for (int i = 0; i < 256; i++)
    {
        probability[i] = 0.0;
        mean[i] = 0.0;
        between[i] = 0.0;
    }

    probability[0] = histogram[0];

    for (int i = 1; i < 256; i++)
    {
        probability[i] = probability[i - 1] + histogram[i];
        mean[i] = mean[i - 1] + i * histogram[i];
    }

    threshold = 0;
    max_between = 0.0;

    for (int i = 0; i < 255; i++)
    {
        if (probability[i] != 0.0 && probability[i] != 1.0)
            between[i] = pow(mean[255] * probability[i] - mean[i], 2) / (probability[i] * (1.0 - probability[i]));

        else
        {
            between[i] = 0.0;

            if (between[i] > max_between)
            {
                max_between = between[i];
                threshold = i;
            }
        }
    }

    g_free(probability);
    g_free(mean);
    g_free(between);
    g_free(histogram);
    g_print("threshold = %d\n", threshold);
    return threshold;
}

void RGBToHSV(guchar *rgba, gdouble *hsv)
{
    guchar min = guchar_min_3(rgba[0], rgba[1], rgba[2]);
    guchar max = guchar_max_3(rgba[0], rgba[1], rgba[2]);

    hsv[2] = max / 255.0; // v = brightness
    float delta = max - min;

    if (!max)
        hsv[1] = 0.0;
    else
        hsv[1] = delta / (float)max;

    if (delta)
    {
        if (rgba[0] == max)
            hsv[0] = (rgba[1] - rgba[2]) / delta; // between yellow & magenta

        else if (rgba[1] == max)
            hsv[0] = 2 + ((rgba[2] - rgba[0]) / delta); // between cyan & yellow

        else
            hsv[0] = 4 + ((rgba[0] - rgba[1]) / delta); // between magenta & cyan

        hsv[0] *= 60;

        if (hsv[0] < 0.0)
            hsv[0] += 360.0;
    }

    else
        hsv[0] = 0.0;
}

void HSVToRGB(gdouble *hsv, guchar *rgba)
{
    // g_print("H = %f; S = %f; V = %f\n", hsv[0], hsv[1], hsv[2]);
    double nr, ng, nb;
    double c, x, m;
    c = hsv[2] * hsv[1];
    x = c * (1.0 - fabs(fmod(hsv[0] / 60.0, 2) - 1.0));
    m = hsv[2] - c;

    if (hsv[0] >= 0.0 && hsv[0] < 60.0)
    {
        nr = c;
        ng = x;
        nb = 0;
    }

    else if (hsv[0] >= 60.0 && hsv[0] < 120.0)
    {
        nr = x;
        ng = c;
        nb = 0;
    }

    else if (hsv[0] >= 120.0 && hsv[0] < 180.0)
    {
        nr = 0;
        ng = c;
        nb = x;
    }

    else if (hsv[0] >= 180.0 && hsv[0] < 240.0)
    {
        nr = 0;
        ng = x;
        nb = c;
    }

    else if (hsv[0] >= 240.0 && hsv[0] < 300.0)
    {
        nr = x;
        ng = 0;
        nb = c;
    }

    else
    {
        nr = c;
        ng = 0;
        nb = x;
    }

    rgba[0] = (nr + m) * 255.0;
    rgba[1] = (ng + m) * 255.0;
    rgba[2] = (nb + m) * 255.0;

    // g_print("R = %d; G = %d; B = %d\n", rgba[0], rgba[1], rgba[2]);
}

color_pix addToColorPix(color_pix color, color_pix toAdd)
{
    color.r = truncateU(color.r + toAdd.r);
    color.g = truncateU(color.g + toAdd.g);
    color.b = truncateU(color.b + toAdd.b);

    return color;
}

color_pix subToColorPix(color_pix color, color_pix toSub)
{
    color.r = truncateU(color.r - toSub.r);
    color.g = truncateU(color.g - toSub.g);
    color.b = truncateU(color.b - toSub.b);

    return color;
}

color_pix multColorPix(color_pix color, double mult)
{
    color.r = truncateU(color.r * mult);
    color.g = truncateU(color.g * mult);
    color.b = truncateU(color.b * mult);

    return color;
}

void palette_free(Palette * plt)
{
    free(plt->colors);
    free(plt);
}

int str_to_int(char str[])
{
    int res = 0;
    int mult = 100;
    for (size_t i = 0; i < 3; i++)
    {
        res+= ((str[i] - '0')*mult);
        mult/=10;
    }
    return res;
}

Palette * create_palette_from_file(const char * file_path, int num_colors)
{

    Palette * palette = malloc(sizeof(Palette));
    palette->length = num_colors;
    palette->colors = malloc(sizeof(color_pix)*num_colors);

    FILE *fp = fopen(file_path,"r");

    char * buffer_byte = calloc(1,sizeof(char));
    int cpt = 0;
    fread(buffer_byte,1,1,fp);
    while(num_colors)
    {
        color_pix new_pix;
        int cpt_color = 0;
        char num[4] = "000";
        int mult = 0;
        int next = 0;
        while (next == 0)
        {
            if(';' == *buffer_byte || *buffer_byte == '\n')
            {
                if(mult == 2)
                {
                    //Only two numbers were in
                    num[2] = num[1];
                    num[1] = num[0];
                    num[0] = '0';
                }
                if(mult == 1)
                {
                    num[2] = num[0];
                    num[0] = '0';
                }
                //Add color parameter here
                switch (cpt_color)
                {
                case 0:
                    new_pix.r = str_to_int(num);
                    break;
                case 1:
                    new_pix.g = str_to_int(num);
                    break;
                case 2:
                    new_pix.b = str_to_int(num);
                    next = 1;
                default:
                    break;
                }
                num[0] = '0';
                num[1] = '0';
                num[2] = '0';
                mult = 0;
                cpt_color++;
            }
            else
            {
                num[mult]= (*buffer_byte);
                mult++;
            }
            fread(buffer_byte,1,1,fp);
        }
        palette->colors[cpt] = new_pix;
        cpt++;
        num_colors--;
    }
    
    free(buffer_byte);
    fclose(fp);
    return palette;
}