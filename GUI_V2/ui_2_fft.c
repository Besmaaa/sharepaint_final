#include "ui_2.h"

complex_number X_n(float *data, size_t size, float n)
{
    complex_number res;
    res.imaginary = res.real = 0.0;
    for (size_t i = 0; i < size; i++)
    {
        float val = data[i];
        complex_number num = complex_exp(complex_coef(n, i, (float)size));
        num = complex_time_natural(num, val);
        num = complex_plus(res, num);
    }
    return res;
}

complex_number *complex_table(float *data, size_t size)
{
    complex_number *table = malloc(sizeof(complex_number) * size);
    for (size_t i = 0; i < size; i++)
    {
        printf("Working on frequency %lu\n", i);
        table[i] = X_n(data, size, i);
    }
    return table;
}

complex_number *_fft(complex_number *buf, size_t n)
{
    if (n <= 1)
    {
        complex_number *new_buf = table_complex_init(n);
        for (size_t i = 0; i < n; i++)
            new_buf[i] = buf[i];
        return new_buf;
    }
    complex_number t_even[n / 2];
    complex_number t_odd[n / 2];
    for (size_t i = 0; i < n - 1; i += 2)
    {
        t_even[i / 2] = buf[i];
        t_odd[i / 2] = buf[i + 1];
    }

    complex_number *even = _fft(t_even, n / 2);
    complex_number *odd = _fft(t_odd, n / 2);

    complex_number *T = table_complex_init(n / 2);
    for (size_t i = 0; i < n / 2; i++)
    {
        complex_number num = complex_exp(complex_coef(1, i, (float)n));
        complex_number times = complex_times(num, odd[i]);
        T[i] = complex_plus(T[i], times);
    }

    complex_number *res = table_complex_init(n);
    for (size_t i = 0; i < n / 2; i++)
    {
        complex_number num_plus = complex_plus(even[i], T[i]);
        complex_number num_minus = complex_minus(even[i], T[i]);
        res[i] = complex_plus(res[i], num_plus);
        res[i + n / 2] = complex_plus(res[i + n / 2], num_minus);
    }
    free(T);
    free(even);
    free(odd);
    return res;
}

complex_number *data_to_complex(float *data, size_t size)
{
    complex_number *table = malloc(sizeof(complex_number) * size);
    for (size_t i = 0; i < size; i++)
    {
        complex_number num;
        num.real = data[i];
        num.imaginary = 0.0;
        table[i] = num;
    }
    return table;
}

void complex_to_data(complex_number *data, float *table, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        table[i] = data[i].real;
    }
}

complex_number *fft(float *data, size_t size)
{

    complex_number *buf = data_to_complex(data, size);
    complex_number *out = _fft(buf, size);
    free(buf);
    return out;
}

float *ifft(complex_number *buf, float *table, size_t size)
{

    inverse_complex_array(buf, size);
    complex_number *out = _fft(buf, size);
    inverse_complex_array(out, size);
    divide_complex_array(out, size, size);

    complex_to_data(out, table, size);
    free(out);
    return table;
}

float magnitude(complex_number *num)
{
    return sqrt(((num->real * num->real) + (num->imaginary * num->imaginary)));
}

float phase(complex_number *num)
{
    return atanf(num->imaginary / num->real);
}

float *magnitude_table(complex_number *table, size_t size)
{
    float *mag = malloc(sizeof(float) * size / 2);
    for (size_t i = 0; i < size / 2; i++)
    {
        mag[i] = (2.0 * magnitude(&table[i])) / (float)size;
    }
    return mag;
}

void mag_table(complex_number *table, float *mag, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        mag[i] = (2.0 * magnitude(&table[i])) / (float)size;
    }
}

void phase_table(complex_number *table, float *pha, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        pha[i] = phase(&table[i]);
    }
}

void filter_cut_from(complex_number *arr, size_t size, size_t cut)
{
    for (size_t i = cut; i < size / 2; i++)
    {
        arr[i].imaginary *= 0.0;
        arr[i].real *= 0.0;
        arr[size - i - 1].imaginary *= 0.0;
        arr[size - i - 1].real *= 0.0;
    }
}

void filter_cut_before(complex_number *arr, size_t cut, size_t size)
{
    for (size_t i = 0; i < cut; i++)
    {
        arr[i].imaginary *= 0.0;
        arr[i].real *= 0.0;
        arr[size - i - 1].imaginary *= 0.0;
        arr[size - i - 1].real *= 0.0;
    }
}

void filter_cut_between(complex_number *arr, size_t bot, size_t top, size_t size)
{
    for (size_t i = bot; i < top; i++)
    {
        arr[i].imaginary *= 0.0;
        arr[i].real *= 0.0;
        arr[size - i - 1].imaginary *= 0.0;
        arr[size - i - 1].real *= 0.0;
    }
}

void filter_cut_around(complex_number *arr, size_t bot, size_t top, size_t size)
{
    for (size_t i = 0; i < bot; i++)
    {
        arr[i].imaginary *= 0.0;
        arr[i].real *= 0.0;
        arr[size - i - 1].imaginary *= 0.0;
        arr[size - i - 1].real *= 0.0;
    }
    for (size_t i = top; i < size / 2; i++)
    {
        arr[i].imaginary *= 0.0;
        arr[i].real *= 0.0;
        arr[size - i - 1].imaginary *= 0.0;
        arr[size - i - 1].real *= 0.0;
    }
}

void apply_filter_to_buffer(vis_data *data, size_t size)
{
    float *buf = data->sig;
    float *mag = data->harmonics;
    float *pha = data->phase;
    float *rep = data->response;

    complex_number *arr = fft(buf, size);

    // put your filter here

    // filter_cut_from(arr,size,(size_t)(10));
    // filter_cut_before(arr,(size_t)(100),size);

    filter_cut_around(arr,10,300,size);
    // filter_cut_between(arr,400,512,size);

    mag_table(arr, mag, size);
    phase_table(arr, pha, size);

    ifft(arr, rep, size);
    free(arr);

    for (size_t i = 0; i < size; i++)
    {
        buf[i] = rep[i];
    }
}

guchar get_pixel_red(Canvas *canvas, int x, int y)
{
    guchar *rgba = get_RGBA(canvas, x, y);
    guchar red = rgba[0];
    g_free(rgba);

    return red;
}

int closest_p(int my_num)
{
    int mult = 2;

    while(my_num/mult != 0)
    {
        mult *= 2;
    }

    return mult;
}

float *red_channel(Canvas *canvas)
{
    int h = canvas->height;
    int w = canvas->width;
    int clos = closest_p(w);
    float *arr = malloc(clos * sizeof(float));

    float *arr_tot = malloc(h * w * sizeof(float));

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
            arr[j] = (float)get_pixel_red(canvas , j, i); 

        for (size_t j = w; j < clos; j++)
            arr[j] = 0.0; 

        vis_data *data = malloc(sizeof(vis_data));
        data->harmonics = malloc(sizeof(float) *clos);
        data->response = malloc(sizeof(float) * clos);
        data->sig = arr;
        data->phase = malloc(sizeof(float) *clos);
        apply_filter_to_buffer(data,clos);
        

        for (size_t j = 0; j < w; j++)
        {
            arr_tot[j+i*h] = data->response[j];
        }
        free(data->harmonics);
        free(data->phase);

        free(data->response);
        free(data);
    }
    return arr_tot;
}

void translate(Canvas *canvas, float *arr)
{
    int h = canvas->height;
    int w = canvas->width;

    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
        {
            put_RGBA(canvas, j, i, truncatef(arr[i*h+j]), truncatef(arr[i*h+j]), truncatef(arr[i*h+j]), 255);
        }   
    }
}

