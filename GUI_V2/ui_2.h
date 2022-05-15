#include <gtk/gtk.h>
#include <math.h>
#include <cairo.h>
#include <stdio.h>

// For Image Display In Drawing Area
typedef struct Canvas{
    GdkPixbuf *pixbuf;
    int width;
    int height;
    int rowstride;
    int n_channels;
    GdkPixbuf *initial;
    int modified;
} Canvas;

typedef struct CanvasList
{
    int size;
    Canvas *prev;
} CanvasList;


// ??
typedef struct Project{
    guchar *name;
    Canvas *displayed_canvas;
    int *id_users;
} Project;

// RGB Pixel
typedef struct color_pix
{
    int r;
    int b;
    int g;
} color_pix;

// For Drawing
typedef struct Point{
    int x;
	int y;
	GdkRGBA *color;
    gdouble size;
	struct Point *next;
} Point;

// For Dithering
typedef struct Palette{
    int length;
    color_pix *colors;
} Palette;

// For Serv
typedef struct UserInfo{
    char *username;
    char *password;
    char *proj;
    int token;
    int cfd;
    int id;
    int fct; //0=LOGIN,1=SIGNUP
    Canvas* canvas;
} UserInfo;

// For FFT
typedef struct complex_number
{
    float real;
    float imaginary;
} complex_number;

// In ui_2_complex.c
float complex_coef(float n, float k, float N);
complex_number *table_complex_init(size_t size);
complex_number complex_exp(float num);
complex_number complex_plus(complex_number num1, complex_number num2);
complex_number complex_minus(complex_number num1, complex_number num2);
complex_number complex_times(complex_number num1, complex_number num2);
complex_number complex_time_natural(complex_number num, float number);
complex_number inverse_complex_real_with_imaginary(complex_number num1);
complex_number complex_divide_natural(complex_number num, float number);
void inverse_complex_array(complex_number * num, size_t size);
void divide_complex_array(complex_number * num, size_t size, float number);
void printf_complex(complex_number val);
void print_arr_img(complex_number *arr, size_t size, size_t in_line);


// In ui_2_tools.c
guchar guchar_min_3(guchar a, guchar b, guchar c);
guchar guchar_max_3(guchar a, guchar b, guchar c);
guchar truncateU(gint n);
Canvas *return_to_initial(Canvas *canvas);
void put_RGBA(Canvas *canvas, int x, int y, guchar red, guchar green, guchar blue, guchar alpha);
guchar* get_RGBA(Canvas *canvas, int x, int y);
guchar *get_RGBA_given_guchar(Canvas *canvas, int x, int y, guchar *rgba);
int* histogram(Canvas *canvas);
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