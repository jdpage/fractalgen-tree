/*
 * Copyright 2012 Jonathan David Page. All rights reserved.
 *
 * Original filename: cfractal.h
 *
 * This file contains the exports for cfractal.c
 */

typedef struct {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
} colour;

typedef struct {
	double x;
	double y;
	double angle; // radians
	double magnitude;
	colour c;
} vector;

typedef struct {
	unsigned char *buf;
	int channels;
	int width;
	int height;
	int pitch;
} image;

double *new_angle_set(int n, int seed);
colour *new_colour_set(int n);
int iterate(vector **old, int *vsize, double *angles, colour *colours, int asize, int level);
void render(vector *v, int vsize, image *i);
image *new_image(int width, int height, int channels);
void del_image(image *i);
