#include "ui_2.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

#define EPSILON 0.00001 

gboolean Gaussian(GtkWidget *widget, Canvas *canvas)
{
    if (canvas->modified)
        return_to_initial(canvas);

    int k = gtk_range_get_value(GTK_RANGE(widget));
    double sigma = fmax(k / 2.0, 1.0);
    int kernelWidth = (int)(2 * k + 1);

    double *kernel = calloc(kernelWidth * kernelWidth, sizeof(double));
    float sum = 0.0;

    int line = 0;

    for (int i = -k; i <= k; i++)
    {
        // Initialize Kernel with values from Gaussian function.
        for (int j = -k; j <= k; j++)
        {
            double exponentNum = (double)(-(i * i + j * j));
            double exponentDenom = 2 * sigma * sigma;

            double eExpression = exp(exponentNum / exponentDenom);
            double gaussianVal = eExpression / (2 * M_PI * sigma * sigma);

            kernel[i + (int)k + j + (int)k + line * (kernelWidth - 1)] = gaussianVal;
            sum += gaussianVal;
        }

        line += 1;
    }

    float average_r, average_g, average_b;
    guchar alpha = 255;

    int h = canvas->height;
    int w = canvas->width;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            average_r = average_g = average_b = 0;
            line = 0;

            for (int x = -k; x <= k; x++)
            {
                for (int y = -k; y <= k; y++)
                {
                    double kernelVal = kernel[x + (int)k + y + (int)k + line * (kernelWidth - 1)] / sum;

                    int get_x = i - x;
                    int get_y = j - y;

                    // Edge handling.
                    if (get_x < 0)
                        get_x = -get_x;

                    else if (get_x >= h)
                        get_x -= 2 * k;

                    if (get_y < 0)
                        get_y = -get_y;

                    else if (get_y >= w)
                        get_y -= 2 * k;

                    guchar *rgba = get_RGBA(canvas, get_y, get_x);

                    average_r += (float)rgba[0] * kernelVal;
                    average_g += (float)rgba[1] * kernelVal;
                    average_b += (float)rgba[2] * kernelVal;
                    alpha = rgba[3];

                    g_free(rgba);
                }

                line += 1;
            }

            put_RGBA(canvas, j, i, truncateU(average_r), truncateU(average_g), truncateU(average_b), alpha);
        }
    }

    free(kernel);
    canvas->modified = 1;
    return FALSE;
}

void Motion(Canvas *canvas, gint k, gdouble angle)
{
    //k = 10;
    //angle = M_PI;
    angle = (angle / 180.0) * M_PI;

    if (canvas->modified)
        return_to_initial(canvas);

    double sum = 0.0;

    double *kernel = calloc(k * k, sizeof(double));

    int mid_kernel = k/2;
    int c = mid_kernel * (mid_kernel + sin(angle)) - mid_kernel * (mid_kernel + cos(angle));

    for (int x = 0; x < k; x++)
    {
        for (int y = 0; y < k; y++)
        {
            //printf("x = %d; y = %d\n", x, y);
            //printf("gleg = %f\n", (-sin(angle) * x + cos(angle) * y + c));

            if (fabs(-sin(angle) * x + cos(angle) * y + c) <= EPSILON)
            {
                //printf("x = %d; y = %d\n", x, y);
                //printf("gleg = %f\n", (-sin(angle) * x + cos(angle) * y + c));
                kernel[y * k + x] = 1.0;
                sum += 1.0;
            }
                
        }
    }

    //print_kernel(kernel, k);

    double average_r, average_g, average_b;

    int h = canvas->height;
    int w = canvas->width;
    guchar alpha = 255;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            average_r = average_g = average_b = 0;

            for (int x = -mid_kernel; x < mid_kernel; x++)
            {
                for (int y = -mid_kernel; y < mid_kernel; y++)
                {
                    //printf("x = %d, y = %d\n", x, y);
                    //printf("pos = %d\n", (x + mid_kernel) + (y + mid_kernel) + (x + mid_kernel) * (k - 1));
                    double kernelVal = kernel[(x + mid_kernel) + (y + mid_kernel) + (x + mid_kernel) * ((int)k - 1)] / sum;
                    //printf("Got Kernel Val\n");

                    int get_x = i - x;
                    int get_y = j - y;

                    // Edge handling.
                    if (get_x < 0)
                        get_x = -get_x;

                    else if (get_x >= h)
                        get_x -= 2 * k;

                    if (get_y < 0)
                        get_y = -get_y;
                        
                    else if (get_y >= w)
                        get_y -= 2 * k;

                    guchar *rgba = get_RGBA(canvas, get_y, get_x);

                    average_r += (double)rgba[0] * kernelVal;
                    average_g += (double)rgba[1] * kernelVal;
                    average_b += (double)rgba[2] * kernelVal;
                    alpha = rgba[3];

                    g_free(rgba);
                }
            }

            put_RGBA(canvas, j, i, truncateU(average_r), truncateU(average_g), truncateU(average_b), alpha);
        }
    }

    free(kernel);
    canvas->modified = 1;
}

gboolean Sharpening(GtkWidget *widget, Canvas *canvas)
{
    float average_r, average_g, average_b;

    int h = canvas->height;
    int w = canvas->width;

    float *kernel;
    kernel = malloc(sizeof(float) * 9);
    kernel[0] = 0.0;
    kernel[1] = -0.5;
    kernel[2] = 0.0;
    kernel[3] = -0.5;
    kernel[4] = 3.0;
    kernel[5] = -0.5;
    kernel[6] = 0.0;
    kernel[7] = -0.5;
    kernel[8] = 0.0;

    guchar *rgba = malloc(4 * sizeof(guchar));

    for (int i = 1; i < w - 1; i++)
    {
        for (int j = 1; j < h - 1; j++)
        {
            average_r = average_g = average_b = 0;

            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    int ii = i + x - 1;
                    int jj = j + y - 1;

                    rgba = get_RGBA_given_guchar(canvas, i, j, rgba);

                    average_r += (float)rgba[0] * kernel[x + y];
                    average_g += (float)rgba[1] * kernel[x + y];
                    average_b += (float)rgba[2] * kernel[x + y];
                }
            }

            put_RGBA(canvas, i, j, truncatef(average_r), truncatef(average_g), truncatef(average_b), rgba[3]);
        }
    }

    free(kernel);
    g_free(rgba);
    return FALSE;
}