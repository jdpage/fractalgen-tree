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

double normalize(double m) {
	return m - (int)(m / (M_PI * 2)) * (M_PI * 2);
}

int iterate(vector **v, int *vsize, double *angles, int asize) {
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
		}
	}
	
	*v = new;
	*vsize = nvsize;
	
	free(old);
	
	return 0;
}

#define BLACK 0
#define WHITE 255

int pt_okay(double x, double y, image *i) {
	if (x > i->width || y > i->height) {
		fprintf(stderr, "point %f,%f outside bounds!", x, y);
		return 0;
	}
	return 1;
}

void render(vector *v, int vsize, image *i) {
	int k;
	// make black
	for (k = 0; k < (i->width * i->height); k++) {
		i->buf[k] = BLACK;
	}
	
	for (k = 0; k < vsize; k++) {
		if (pt_okay(v[k].x, v[k].y, i)) {
			double xw, yw;
			xw = v[k].x - (int)v[k].x;
			yw = v[k].y - (int)v[k].y;
			i->buf[(int)v[k].x + (int)v[k].y * i->pitch] += (int)(WHITE * (1 - xw) * (1 - yw));
			i->buf[(int)v[k].x + 1 + (int)v[k].y * i->pitch] += (int)(WHITE * xw * (1 - yw));
			i->buf[(int)v[k].x + ((int)v[k].y + 1) * i->pitch] += (int)(WHITE * (1 - xw) * yw);
			i->buf[(int)v[k].x + 1 + ((int)v[k].y + 1) * i->pitch] += (int)(WHITE * xw * yw);
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