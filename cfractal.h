typedef struct {
	double x;
	double y;
	double angle; // radians
	double magnitude;
} vector;

typedef struct {
	unsigned char *buf;
	int channels;
	int width;
	int height;
	int pitch;
} image;

double *new_angle_set(int n, int seed);
int iterate(vector **old, int *vsize, double *angles, int asize);
void render(vector *v, int vsize, image *i);
image *new_image(int width, int height, int channels);
void del_image(image *i);