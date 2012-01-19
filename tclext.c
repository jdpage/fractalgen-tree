/*
 * Copyright 2012 Jonathan David Page. All rights reserved.
 *
 * Original filename: tclext.c
 *
 * Exposes the functions defined in cfractal.c to the Tcl interpreter.
 */

#include <stdlib.h>
#include <tcl.h>
#include <tk.h>
#include "cfractal.h"

#define CMD_ARGS ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[]

const char *OUT_OF_MEMORY = "oh snap, couldn't allocate memory!";

int anglesCmd(CMD_ARGS) {
	int seed = 0, branch, k;
	double *a;
	Tcl_Obj *list, **items;
	
	if (objc < 2 || 3 < objc) {
		Tcl_WrongNumArgs(interp, 1, objv, "branch ?seed?");
		return TCL_ERROR;
	}
	
	if (Tcl_GetIntFromObj(interp, objv[1], &branch) == TCL_ERROR)
		return TCL_ERROR;
	if (objc == 3) {
		if (Tcl_GetIntFromObj(interp, objv[2], &seed) == TCL_ERROR)
			return TCL_ERROR;
	}
	
	if ((a = new_angle_set(branch, seed)) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	if ((items = (Tcl_Obj **)malloc(branch * sizeof(Tcl_Obj *))) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	for (k = 0; k < branch; k++)
		items[k] = Tcl_NewDoubleObj(a[k]);
	
	free(a);
	
	list = Tcl_NewListObj(branch, items);
	
	free(items);
	
	Tcl_SetObjResult(interp, list);
	return TCL_OK;
}

int frameCmd(CMD_ARGS) {
	Tk_PhotoHandle himage;
	Tk_PhotoImageBlock frame;
	image *i;
	int size, branch, level, seed, k, vsize = 1;
	Tcl_Obj **list;
	vector *v;
	double *a;
	colour *c;
	
	if (objc != 5) {
		Tcl_WrongNumArgs(interp, 1, objv, "size kernel level image");
		return TCL_ERROR;
	}
	
	if (Tcl_GetIntFromObj(interp, objv[1], &size) == TCL_ERROR)
		return TCL_ERROR;
	
	if (Tcl_ListObjGetElements(interp, objv[2], &branch, &list) == TCL_ERROR)
		return TCL_ERROR;
	
	if ((a = (double *)malloc(branch * sizeof(double))) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	for (k = 0; k < branch; k++) {
		if (Tcl_GetDoubleFromObj(interp, list[k], a + k) == TCL_ERROR) {
			return TCL_ERROR;
		}
	}
	
	if (Tcl_GetIntFromObj(interp, objv[3], &level) == TCL_ERROR)
		return TCL_ERROR;
	himage = Tk_FindPhoto(interp, Tcl_GetString(objv[4]));
	Tk_PhotoGetImage(himage, &frame);
	
	if ((c = new_colour_set(branch)) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	if ((v = (vector *)malloc(sizeof(vector))) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	v->x = v->y = size * 0.5;
	v->angle = 0;
	v->magnitude = size * 0.5 * 0.5;
	v->c.red = v->c.green = v->c.blue = 0xff;
	
	for (k = 0; k < level; k++) {
		if (iterate(&v, &vsize, a, c, branch, k + 1) > 0) {
			Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
			free(a);
			free(c);
			free(v);
			return TCL_ERROR;
		}
	}
	
	if ((i = new_image(size, size, 3)) == NULL) {
		Tcl_SetObjResult(interp, Tcl_NewStringObj(OUT_OF_MEMORY, -1));
		return TCL_ERROR;
	}
	
	render(v, vsize, i);
	
	frame.width = i->width;
	frame.height = i->height;
	frame.pitch = i->pitch;
	frame.pixelSize = i->channels;
	frame.pixelPtr = i->buf;
	for (k = 0; k < 4; k++) frame.offset[k] = k;
	
	Tk_PhotoSetSize(interp, himage, frame.width, frame.height);
	Tk_PhotoPutBlock(interp, himage, &frame, 0, 0, frame.width, frame.height, TK_PHOTO_COMPOSITE_SET);
	
	del_image(i);
	free(a);
	free(c);
	free(v);
	
	return TCL_OK;
}

int Cfractal_Init(Tcl_Interp *interp) {
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;
	if (Tk_InitStubs(interp, "8.1", 0) == NULL) return TCL_ERROR;
	
	Tcl_CreateObjCommand(interp, "cfractal::frame", frameCmd, NULL, NULL);
	Tcl_CreateObjCommand(interp, "cfractal::newkernel", anglesCmd, NULL, NULL);
	
	Tcl_PkgProvide(interp, "cfractal", "0.1");
	
	return TCL_OK;
}
