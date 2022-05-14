#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <stdio.h>

typedef struct Canvas{
    GdkPixbuf *pixbuf;
    int width;
    int height;
    int rowstride;
    int n_channels;
    GdkPixbuf *initial;
    int modified;
} Canvas;

typedef struct Project{
    guchar *name;
    Canvas *displayed_canvas;
    int *id_users;
} Project;

typedef struct color_pix
{
    int r;
    int b;
    int g;
} color_pix;

typedef struct Palette{
    int length;
    color_pix *colors;
} Palette;

typedef struct UserInfo{
    guchar *username;
    guchar *password;
    int id;
} UserInfo;


// In ui_2_tools.c
guchar guchar_min_3(guchar a, guchar b, guchar c);
guchar guchar_max_3(guchar a, guchar b, guchar c);
guchar truncateU(gint n);
Canvas *return_to_initial(Canvas *canvas);
void put_RGBA(Canvas *canvas, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);
guchar* get_RGBA(Canvas *canvas, int x, int y);
guchar *get_RGBA_given_guchar(Canvas *canvas, int x, int y, guchar *rgba);
guchar* histogram(Canvas *canvas);
guchar threshold(Canvas *canvas, guchar* histogram);

void RGBToHSV(guchar *rgba, gdouble *hsv);
void HSVToRGB(gdouble *hsv, guchar *rgba);

color_pix addToColorPix(color_pix color, color_pix toAdd);
color_pix subToColorPix(color_pix color, color_pix toSub);
color_pix multColorPix(color_pix color, double mult);

void palette_free(Palette * plt);
Palette * create_palette_from_file(const char * file_path, int num_colors);

// In ui_2_filters.c
gboolean Grayscale(GtkWidget *widget, Canvas *canvas);
gboolean Negative(GtkWidget *widget, Canvas *canvas);
gboolean Binarize(GtkWidget *widget, Canvas *canvas);
gboolean Sepia(GtkWidget *widget, Canvas *canvas);

// In ui_2_simple_filters.c
gboolean Lightness(GtkWidget *widget, Canvas *canvas);
gboolean Contrast(GtkWidget *widget, Canvas *canvas);

// In ui_2_hsv.c
gboolean Saturation(GtkWidget *widget, Canvas *canvas);
gboolean Brightness(GtkWidget *widget, Canvas *canvas);
gboolean Hue(GtkWidget *widget, Canvas *canvas);

// In ui_2_blur.c
gboolean Gaussian(GtkWidget *widget, Canvas *canvas);
void Motion(Canvas *canvas, gint k, gdouble angle);
gboolean Sharpening(GtkWidget *widget, Canvas *canvas);

// In ui_2_ca.c
gboolean Vertical_CA(GtkWidget *widget, Canvas *canvas);
gboolean Horizontal_CA(GtkWidget *widget, Canvas *canvas);

// In ui_2_co.c
void Channel_Op(Canvas *canvas, gchar component, char* value);

// In ui_2_other.c
gboolean Mirror(GtkWidget *widget, Canvas *canvas);
gboolean Rotation(GtkWidget *widget, Canvas *canvas);
gboolean Mosaic(GtkWidget *widget, Canvas *canvas);

// In ui_2_image_fix.c
gboolean Dithering(GtkWidget *widget, Canvas *canvas);
gboolean MedianFiltering(GtkWidget *widget, Canvas *canvas);

//gboolean DeinterlacingEven(GtkWidget *widget, Canvas *canvas);
//gboolean DeinterlacingOdd(GtkWidget *widget, Canvas *canvas);
gboolean DeinterlacingSameSceneEven(GtkWidget *widget, Canvas *canvas);
gboolean DeinterlacingSameSceneOdd(GtkWidget *widget, Canvas *canvas);
