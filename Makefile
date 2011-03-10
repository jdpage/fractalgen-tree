CFLAGS := -Wall -Wextra -lm -ljpeg -fPIC

all: cfractal cfractal.so

cfractal: cfractal.o jpeg.o
	gcc $(CFLAGS) -o cfractal cfractal.o jpeg.o

cfractal.so: cfractal.o tclext.o
	gcc $(CFLAGS) -shared -ltclstub8.5 -ltkstub8.5 -o cfractal.so cfractal.o tclext.o

%.o: %.c cfractal.h
	gcc $(CFLAGS) -c $< -o $@

clean:
	rm *.o cfractal *.so