/*
 * Copyright 2012 Jonathan David Page. All rights reserved.
 *
 * Original filename: jpeg.c
 *
 * This file contains the code for saving the generated image as a jpeg file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "cfractal.h"

int write_jpeg(const char *file, image *img);
int string_to_int(const char *s);

const char *OUT_OF_MEMORY = "cfractal: could not allocate memory!";

int main(int argc, char **argv) {
	image *i;
	int size, branch, level, seed, k, vsize = 1;
	vector *v;
	double *a;
	colour *c;
	
	if (argc != 6) {
		fprintf(stderr, "usage: cfractal size branch level seed outfile\n");
		return 1;
	}
	
	size = string_to_int(argv[1]);
	branch = string_to_int(argv[2]);
	level = string_to_int(argv[3]);
	seed = string_to_int(argv[4]);
	
	if ((v = (vector *)malloc(sizeof(vector))) == NULL) {
		fprintf(stderr, OUT_OF_MEMORY);
		return 2;
	}
	
	// initialize
	v->x = v->y = size * 0.5;
	v->angle = 0;
	v->magnitude = size * 0.5 * 0.5;
	v->c.red = v->c.green = v->c.blue = 0xff;
	
	if ((a = new_angle_set(branch, seed)) == NULL) {
		fprintf(stderr, OUT_OF_MEMORY);
		return 2;
	}
	
	if ((c = new_colour_set(branch)) == NULL) {
		fprintf(stderr, OUT_OF_MEMORY);
		return 2;
	}
	
	for (k = 0; k < level; k++) {
		if (iterate(&v, &vsize, a, c, branch, k + 1) > 0) {
			fprintf(stderr, OUT_OF_MEMORY);
			return 2;
		}
	}
	
	if ((i = new_image(size, size, 3)) == NULL) {
		fprintf(stderr, OUT_OF_MEMORY);
		return 2;
	}
	
	render(v, vsize, i);
	if (write_jpeg(argv[5], i) > 0) {
		fprintf(stderr, "cfractal: image not written\n");
		return 3;
	}
	
	del_image(i);
	free(a);
	free(v);
	free(c);
	
	return 0;
}

int string_to_int(const char *s) {
	int out = 0;
	for (; *s != 0; s++)
		out = out * 10 + (*s - '0');
	return out;
}

int write_jpeg(const char *file, image *img) {
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	JSAMPROW row_pointer[1];
	int row_stride;
	FILE *output;
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	if ((output = fopen(file, "wb")) == NULL) {
		fprintf(stderr, "cfractal: error opening file %s\n", file);
		return 1;
	}
	
	jpeg_stdio_dest(&cinfo, output);
	cinfo.image_width = img->width;
	cinfo.image_height = img->height;
	cinfo.input_components = img->channels;
	
	if (img->channels == 1)
		cinfo.in_color_space = JCS_GRAYSCALE;
	else if (img->channels == 3)
		cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&cinfo);
	
	jpeg_start_compress(&cinfo, TRUE);
	
	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & img->buf[cinfo.next_scanline * img->pitch];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	
	fclose(output);
	
	return 0;
}
