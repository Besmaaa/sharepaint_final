#Makefile

CC = gcc -fsanitize=address
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -rdynamic
LDFLAGS = 
LDLIBS = `pkg-config --libs gtk+-3.0` -lm -ldl -lcurl

SRC = ui_2.c ui_2_blur.c ui_2_ca.c ui_2_co.c ui_2_filters.c ui_2_hsv.c ui_2_other.c ui_2_simple_filters.c ui_2_tools.c ui_2_image_fix.c ui_2_fft.c ui_2_complex.c cloud/cloud.c cloud/ftpget.c cloud/ftpupload.c
OBJ = $(SRC:.c=.o)
DEP = $(SRC:.c=.d)

all: ui_2

ui_2: $(OBJ)

clean:
	rm -f $(OBJ)
	rm -f $(DEP)
	rm ui_2

#END