/*
 * Copyright 2012 Jonathan David Page. All rights reserved.
 *
 * Original filename: cfractal.c
 *
 * This file contains the fractal generation algorithm and plotting code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "cfractal.h"

double *new_angle_set(int n, int seed) {
	double *angles;
	int k;
	if ((angles = (double *)malloc(n * sizeof(double))) == NULL)
		return NULL;
	
	if (seed == 0) {
		int s = time(NULL);
		srand(s);
		fprintf(stdout, "using seed %d\n", s);
	} else {
		srand(seed);
	}
	
	for (k = 0; k < n; k++) {
		angles[k] = (rand() % 360) * M_PI / 180;
	}
	
	return angles;
}

colour *new_colour_set(int n) {
	colour *colours;
	int k;
	if ((colours = (colour *)malloc(n * sizeof(colour))) == NULL)
		return NULL;
	
	for (k = 0; k < n; k++) {
		double hue = k * 360.0/n;
		if (hue < 120) {
			// red-green
			colours[k].red   = (unsigned char)(255 * (1 - hue/120));
			colours[k].green = (unsigned char)(255 * (hue/120));
			colours[k].blue  = 0;
		} else if (hue < 240) {
			// green-blue
			colours[k].green = (unsigned char)(255 * (1 - (hue - 120)/120));
			colours[k].blue  = (unsigned char)(255 * ((hue - 120)/120));
			colours[k].red   = 0;
		} else {
			// blue-red
			colours[k].blue  = (unsigned char)(255 * (1 - (hue - 240)/120));
			colours[k].red   = (unsigned char)(255 * ((hue - 240)/120));
			colours[k].green = 0;
		}
	}
	
	return colours;
}

double normalize(double m) {
	return m - (int)(m / (M_PI * 2)) * (M_PI * 2);
}

unsigned char tint(old, new, level) {
	return old * (level - 1) / level + new / level;
}

int iterate(vector **v, int *vsize, double *angles, colour *colours, int asize, int level) {
	vector *new, *old;
	int nvsize, ovsize, k, j;
	
	old = *v;
	ovsize = *vsize;
	nvsize = ovsize * asize;
	
	if ((new = (vector *)malloc(nvsize * sizeof(vector))) == NULL)
		return 1;
	
	for (k = 0; k < ovsize; k++) {
		for (j = 0; j < asize; j++) {
			int i = k * asize + j;
			new[i].angle = normalize(old[k].angle + angles[j]);
			new[i].magnitude = old[k].magnitude * 0.5;
			new[i].x = old[k].x + old[k].magnitude * cos(new[i].angle);
			new[i].y = old[k].y + old[k].magnitude * sin(new[i].angle);
			new[i].c.red   = tint(old[k].c.red,   colours[j].red,   level);
			new[i].c.green = tint(old[k].c.green, colours[j].green, level);
			new[i].c.blue  = tint(old[k].c.blue,  colours[j].blue,  level);
		}
	}
	
	*v = new;
	*vsize = nvsize;
	
	free(old);
	
	return 0;
}

#define PX_MIN 0
#define PX_MAX 255

int pt_okay(double x, double y, image *i) {
	if (x > i->width || y > i->height) {
		fprintf(stderr, "point %f,%f outside bounds!", x, y);
		return 0;
	}
	return 1;
}

void pixel_add(unsigned char *pixel, unsigned char value) {
	if (*pixel + value > PX_MAX) {
		*pixel = PX_MAX;
	} else {
		*pixel += value;
	}
}

colour BLACK = { .red = 0x00, .green = 0x00, .blue = 0x00 };
colour WHITE = { .red = 0xff, .green = 0xff, .blue = 0xff };

void dither_add(image *i, double x, double y, int choffset, unsigned char value) {
	double xw, yw;
	xw = x - (int)x;
	yw = y - (int)y;
	
	pixel_add(i->buf + (int)floor(x) * i->channels + (int)floor(y) * i->pitch + choffset,
			  (unsigned char)(value * (1 - xw) * (1 - yw)));
	pixel_add(i->buf + (int)ceil(x)  * i->channels + (int)floor(y) * i->pitch + choffset,
			  (unsigned char)(value *      xw  * (1 - yw)));
	pixel_add(i->buf + (int)floor(x) * i->channels + (int)ceil(y)  * i->pitch + choffset,
			  (unsigned char)(value * (1 - xw) *      yw ));
	pixel_add(i->buf + (int)ceil(x)  * i->channels + (int)ceil(y)  * i->pitch + choffset,
			  (unsigned char)(value *      xw  *      yw ));
}

void render(vector *v, int vsize, image *i) {
	int k;
	// make black
	for (k = 0; k < (i->width * i->height * i->channels); k++) {
		if (i->channels == 4 && k % 4 == 3)
			i->buf[k] = 0xff;
		else
			i->buf[k] = 0x00;
	}
	
	for (k = 0; k < vsize; k++) {
		if (pt_okay(v[k].x, v[k].y, i)) {
			dither_add(i, v[k].x, v[k].y, 0, v[k].c.red);
			dither_add(i, v[k].x, v[k].y, 1, v[k].c.green);
			dither_add(i, v[k].x, v[k].y, 2, v[k].c.blue);
		}
	}
}

image *new_image(int width, int height, int channels) {
	image *i;
	
	if ((i = (image *)malloc(sizeof(image))) == NULL)
		return NULL;
	
	i->width = width;
	i->height = height;
	i->channels = channels;
	i->pitch = width * channels;
	
	if ((i->buf = (unsigned char *)malloc(
	  width * height * channels * sizeof(char))) == NULL) {
		free(i);
		return NULL;
	}
	
	return i;
}

void del_image(image *i) {
	free(i->buf);
	free(i);
}
