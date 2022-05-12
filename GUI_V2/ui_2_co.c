#include "ui_2.h"

void Channel_Op(Canvas *canvas, gchar component, char *value)
{
    for (int i = 0; i < canvas->height; i++)
    {
        for (int j = 0; j < canvas->width; j++)
        {
            guchar *rgba = get_RGBA(canvas, i, j);

            switch (component)
            {
            case 'R':
                if (strcmp(value, "0") == 0)
                {
                    rgba[0] = 0;
                }

                else if (strcmp(value, "255") == 0)
                {
                    rgba[0] = 255;
                }

                else if (strcmp(value, "A") == 0)
                {
                    rgba[0] = rgba[3];
                }

                else if (strcmp(value, "R") == 0)
                {
                    rgba[0] = rgba[0];
                }

                else if (strcmp(value, "G") == 0)
                {
                    rgba[0] = rgba[1];
                }

                else if (strcmp(value, "B") == 0)
                {
                    rgba[0] = rgba[2];
                }

                else if (strcmp(value, "255 - A") == 0)
                {
                    rgba[0] = 255 - rgba[3];
                }

                else if (strcmp(value, "255 - R") == 0)
                {
                    rgba[0] = 255 - rgba[0];
                }

                else if (strcmp(value, "255 - G") == 0)
                {
                    rgba[0] = 255 - rgba[1];
                }

                else if (strcmp(value, "255 - B") == 0)
                {
                    rgba[0] = 255 - rgba[2];
                }

                break;

            case 'G':
                if (strcmp(value, "0") == 0)
                {
                    rgba[1] = 0;
                }

                else if (strcmp(value, "255") == 0)
                {
                    rgba[1] = 255;
                }

                else if (strcmp(value, "A") == 0)
                {
                    rgba[1] = rgba[3];
                }

                else if (strcmp(value, "R") == 0)
                {
                    rgba[1] = rgba[0];
                }

                else if (strcmp(value, "G") == 0)
                {
                    rgba[1] = rgba[1];
                }

                else if (strcmp(value, "B") == 0)
                {
                    rgba[1] = rgba[2];
                }

                else if (strcmp(value, "255 - A") == 0)
                {
                    rgba[1] = 255 - rgba[3];
                }

                else if (strcmp(value, "255 - R") == 0)
                {
                    rgba[1] = 255 - rgba[0];
                }

                else if (strcmp(value, "255 - G") == 0)
                {
                    rgba[1] = 255 - rgba[1];
                }

                else if (strcmp(value, "255 - B") == 0)
                {
                    rgba[1] = 255 - rgba[2];
                }

                break;

            case 'B':
                if (strcmp(value, "0") == 0)
                {
                    rgba[2] = 0;
                }

                else if (strcmp(value, "255") == 0)
                {
                    rgba[2] = 255;
                }

                else if (strcmp(value, "A") == 0)
                {
                    rgba[2] = rgba[3];
                }

                else if (strcmp(value, "R") == 0)
                {
                    rgba[2] = rgba[0];
                }

                else if (strcmp(value, "G") == 0)
                {
                    rgba[2] = rgba[1];
                }

                else if (strcmp(value, "B") == 0)
                {
                    rgba[2] = rgba[2];
                }

                else if (strcmp(value, "255 - A") == 0)
                {
                    rgba[2] = 255 - rgba[3];
                }

                else if (strcmp(value, "255 - R") == 0)
                {
                    rgba[2] = 255 - rgba[0];
                }

                else if (strcmp(value, "255 - G") == 0)
                {
                    rgba[2] = 255 - rgba[1];
                }

                else if (strcmp(value, "255 - B") == 0)
                {
                    rgba[2] = 255 - rgba[2];
                }

                break;

            case 'A':
                if (strcmp(value, "0") == 0)
                {
                    rgba[3] = 0;
                }

                else if (strcmp(value, "255") == 0)
                {
                    rgba[3] = 255;
                }

                else if (strcmp(value, "A") == 0)
                {
                    rgba[3] = rgba[3];
                }

                else if (strcmp(value, "R") == 0)
                {
                    rgba[3] = rgba[0];
                }

                else if (strcmp(value, "G") == 0)
                {
                    rgba[3] = rgba[1];
                }

                else if (strcmp(value, "B") == 0)
                {
                    rgba[3] = rgba[2];
                }

                else if (strcmp(value, "255 - A") == 0)
                {
                    rgba[3] = 255 - rgba[3];
                }

                else if (strcmp(value, "255 - R") == 0)
                {
                    rgba[3] = 255 - rgba[0];
                }

                else if (strcmp(value, "255 - G") == 0)
                {
                    rgba[3] = 255 - rgba[1];
                }

                else if (strcmp(value, "255 - B") == 0)
                {
                    rgba[3] = 255 - rgba[2];
                }

                break;
            }

            put_RGBA(canvas, i, j, rgba[0], rgba[1], rgba[2], rgba[3]);

            g_free(rgba);
        }
    }
}