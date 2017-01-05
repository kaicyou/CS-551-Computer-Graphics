/* Place this in the same file as your main program */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <windows.h>		// Header File For Windows
#include "ObjLoader.h"
#include "osuVector.h"
#include "osuGraphics.h"

void loadAndDrawObj(char *fname);
void objTest();

typedef double matrix[4][4];
typedef double vectorp[4];

double vectorDotX(vectorp &a, vectorp &b);
void vectorCrossX(vectorp &a, vectorp &b, vectorp &r);
double vectorLength(vectorp a);
void mXv(matrix m, vectorp a);
void lengthTo1(vectorp &a);
void matrixreset(matrix a);
void mXm(matrix a, matrix b, matrix r);

void osuOrtho(double left, double right, double bottom, double top, double nearp,double farp);
void osuPerspective(double fovy, double nearp, double farp) ;
void osuInitialize() ;
void osuPushMatrix() ;
void osuPopMatrix() ;
void osuLoadIdentityMatrix();
void osuTranslate(double tx, double ty, double tz) ;
void osuScale(double sx, double sy, double sz) ;
void osuRotate(double angle, double ax, double ay, double az) ;
void osuLookat(double from[3], double at[3], double up[3]);

void osuNormal3f(double x, double y, double z);
void osuEnable(int depthTestBit);
void osuClearZ();
void osuShadeModel(int model);
void osuPointLight(float pos[3], float i);
void osuDirectionalLight(float dir[3], float i);
void osuAmbientLight(float i);
void osuDiffuse(float r, float g, float b);
void osuSpecular(float r, float g, float b, float s);
void osuBegin(OSUDrawable);
void osuVertex3f (double x,double y,double z); 
void osuEnd();

struct vertex
{
	double x;
	double y;
	double z;
	double r;
	double g;
	double b;
};

struct lightinfo
{
	float x;
	float y;
	float z;
	float i;
	int type;
};

struct Specular{
    float r;
    float g;
    float b;
    float s;
} globalspec{1.0,1.0,1.0,0.0};

matrix matrix_project;
matrix matrix_current;
matrix matrix_look;
double matrix_stack[16][4][4];
int matrix_count;

int width;
int height;

//typedef double vector[4];
OSUDrawable polygon_flag;
int shade_model;
double *zbuffer;
struct lightinfo light;
float ambientI = 1;
double color[3];
float diffcolor[3];
struct vertex vertex_list[128];
struct lightinfo light_list[128];
int vertex_count;
int light_count;
vectorp vNormal;
int projectionmethod;
int zbufferflag = -1;
int color_set = 0;

//operation for vectors
#define mmax(a,b) (a>b?a:b)
#define mmin(a,b) (a<b?a:b)

double vectorDotX(vectorp &a, vectorp &b)
{
	double result;
	result = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return result;
}

void vectorCrossX(vectorp &a, vectorp &b, vectorp &r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
	r[3] = 0;
}

double vectorLength(vectorp a)
{
	double result;
	result = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	return result;
}

void mXv(matrix m, vectorp a)
{
	int i;
	vectorp temp;
	for (i = 0; i < 4; i++)
	{
		temp[i] = m[0][i] * a[0] + m[1][i] * a[1] + m[2][i] * a[2] + m[3][i] * a[3];
	}
	for (i = 0; i < 4; i++)
	{
		a[i] = temp[i];
	}
}

void lengthTo1(vectorp &a)
{
	int i;
	double length = vectorLength(a);
	if (length == 0)
	{
		printf("Length = 0 !\n");
	}
	else
	{
		for (i = 0; i < 3; i++)
		{
			a[i] = a[i] / length;
		}
	}
}

void matrixreset(matrix a)
{
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
				a[i][j] = 0;
		}
	}
	for (i = 0; i < 4; i++)
	{
		a[i][i] = 1;
	}
}

void mXm(matrix a, matrix b, matrix r)
{
	int i;
	int j;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			r[i][j] = a[0][j] * b[i][0] + a[1][j] * b[i][1] + a[2][j] * b[i][2] + a[3][j] * b[i][3];
		}
	}
}
//opeartion end

//functions from assign2
void osuOrtho(double left, double right, double bottom, double top, double nearp,double farp)
{ 
	projectionmethod = 1;
	matrixreset(matrix_project);
	matrix_project[0][0] = 2 / (right - left);
	matrix_project[1][1] = 2 / (top - bottom);
	matrix_project[2][2] = 2 / (nearp - farp);
	matrix_project[3][0] = - (right + left) / (right - left);
	matrix_project[3][1] = - (top + bottom) / (top - bottom);
	matrix_project[3][2] = - (farp + nearp) / (farp - nearp);
}

void osuPerspective(double fovy, double nearp, double farp) 
{  	
	projectionmethod = 2;
	double f = 1. / tan((fovy / 2) * 3.1415927 / 180.);
	matrixreset(matrix_project);
	matrix_project[0][0] = f;
	matrix_project[1][1] = f;
	matrix_project[2][2] = (farp + nearp) / (nearp - farp);
	matrix_project[2][3] = -1.;
	matrix_project[3][2] = 2. * (farp * nearp) / (nearp - farp);
	matrix_project[3][3] = 0;
}

void osuInitialize() 
{ 
	int i;
	int j;

	matrix_count = 0;
	matrixreset(matrix_current);
	matrixreset(matrix_look);
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			matrix_stack[matrix_count][i][j] = matrix_current[i][j];
		}
	}
	matrix_count ++;
}


void osuPushMatrix() 
{ 
	int i;
	int j;

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			matrix_stack[matrix_count][i][j] = matrix_current[i][j];
		}
	}
	matrix_count ++;
}

void osuPopMatrix() 
{ 
	int i;
	int j;

	matrix_count --;
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			matrix_current[i][j] = matrix_stack[matrix_count][i][j];
		}
	}
}

void osuLoadIdentityMatrix()
{
	matrixreset(matrix_current);
}

void osuTranslate(double tx, double ty, double tz) 
{ 
	matrix matrix_temp;

	matrixreset(matrix_temp);
	matrix_temp[3][0] = tx;
	matrix_temp[3][1] = ty;
	matrix_temp[3][2] = tz;
	mXm(matrix_current, matrix_temp, matrix_current);
}

void osuScale(double sx, double sy, double sz) 
{ 
	matrix matrix_temp;

	matrixreset(matrix_temp);
	matrix_temp[0][0] = sx;
	matrix_temp[1][1] = sy;
	matrix_temp[2][2] = sz;
	mXm(matrix_current, matrix_temp, matrix_current);
}

void osuRotate(double angle, double ax, double ay, double az) 
{ 	
	double angle_t;
	double ssin;
	double ccos;
	double t;
	vectorp temp;
	matrix matrix_temp;

	matrixreset(matrix_temp);
	angle_t = angle * 3.1415927 / 180;
	ssin = sin(angle_t);
	ccos = cos(angle_t);
	temp[0] = ax;
	temp[1] = ay;
	temp[2] = az;
	lengthTo1(temp);

	t = 1. - ccos;
	matrix_temp[0][0] = temp[0] * temp[0] * t + ccos;
	matrix_temp[1][1] = temp[1] * temp[1] * t + ccos;
	matrix_temp[2][2] = temp[2] * temp[2] * t + ccos;

	matrix_temp[0][1] = temp[1] * temp[0] * t + temp[2] * ssin;
	matrix_temp[1][0] = temp[0] * temp[1] * t - temp[2] * ssin;
	matrix_temp[0][2] = temp[2] * temp[0] * t - temp[1] * ssin;
	matrix_temp[2][0] = temp[0] * temp[2] * t + temp[1] * ssin;
	matrix_temp[1][2] = temp[2] * temp[1] * t + temp[0] * ssin;
	matrix_temp[2][1] = temp[1] * temp[2] * t - temp[0] * ssin;

	mXm(matrix_current, matrix_temp, matrix_current);
}

void osuLookat(float from[3], float at[3], float up[3])
{
	int i;
	vectorp x;
	vectorp y;
	vectorp z;
	vectorp temp;

	x[3] = 0;
	y[3] = 0;
	z[3] = 0;
	temp[3] = 0;
	for (i = 0; i < 3; i++)
	{
		z[i] = at[i] - from[i];
		temp[i] = up[i];
	}
	lengthTo1(z);
	vectorCrossX(temp, z, x);
	vectorCrossX(z, x, y);
	matrixreset(matrix_look);
	for (i = 0; i < 3; i++)
	{
		matrix_look[0][i] = x[i];
		matrix_look[1][i] = y[i];
		matrix_look[2][i] = z[i];
	}
	for (i = 0; i < 3; i++)
	{
		matrix_look[3][0] = vectorDotX(temp, x);
		matrix_look[3][1] = vectorDotX(temp, y);
		matrix_look[3][3] = vectorDotX(temp, z);
	}
}
//functions in assign2 end

void osuNormal3f(double x, double y, double z)
{
	vNormal[0] = x;
	vNormal[1] = y;
	vNormal[2] = z;
}

void osuEnable(int depthTestBit)
{
	int i;
	if (depthTestBit != OSU_DEPTH_TEST)
	{
		printf("Wrong passing value!\n");
		exit(-1);
	}
	zbufferflag = OSU_DEPTH_TEST;
	zbuffer = (double *)malloc(sizeof(double) * 360 * 360);
	for (i = 0; i < 360 * 360; i++)
	{
		zbuffer[i] = - 65535.0;
	}
}

void osuClearZ()
{
	int i;
	for (i = 0; i < 360 * 360; i++)
	{
		zbuffer[i] = -65535.0;
	}
}

void osuShadeModel(int model)
{
	shade_model = model;
}

void osuPointLight(float pos[3], float i)
{
	struct lightinfo light;
	struct lightinfo *ptr = new lightinfo;
	light.x = pos[0];
	light.y = pos[1];
	light.z = pos[2];
	light.i = i;
	light.type = 1;
	ptr = &light;
	light_list[light_count] = light;
	light_count ++;
}

void osuDirectionalLight(float dir[3], float i)
{
	struct lightinfo light;
	struct lightinfo *ptr = new lightinfo;
	light.x = dir[0];
	light.y = dir[1];
	light.z = dir[2];
	light.i = i;
	light.type = 2;
	ptr = &light;
	light_list[light_count] = light;
	light_count ++;
}

void osuAmbientLight(float i)
{
	ambientI = i;
}

void osuDiffuse(float r, float g, float b)
{
	diffcolor[0] = r;
	diffcolor[1] = g;
	diffcolor[2] = b;
	color_set = 1;
}

void osuSpecular(float r, float g, float b, float s)
{
	globalspec.r = r;
	globalspec.g = g;
	globalspec.b = b;
	globalspec.s = s;
	color_set = 1;
}

void osuBegin(OSUDrawable mode)
{
	polygon_flag = mode;
	vertex_count = 0;
	light_count = 0;
    osuGetFramebufferSize(&width, &height);
}

void osuVertex3f (double x,double y,double z)
{
	int i;
	float sc;

	vectorp light;
	struct vertex vertx;
	struct vertex *ptr = new vertex;
    vertx.x = x;
    vertx.y = y;
    vertx.z = z;
    if (color_set == 1) {
        vertx.r = diffcolor[0] * ambientI;
    	vertx.g = diffcolor[1] * ambientI;
    	vertx.b = diffcolor[2] * ambientI;
    }

    for (i = 0; i < light_count; i++)
    {
    	if (light_list[i].type == 1)
    	{
    		light[0] = light_list[i].x - vertx.x;
    		light[1] = light_list[i].y - vertx.y;
    		light[2] = light_list[i].z - vertx.z;
    	}
    	else if (light_list[i].type == 2)
    	{
    		light[0] = -light_list[i].x;
    		light[1] = -light_list[i].y;
    		light[2] = -light_list[i].z;
    	}
    	lengthTo1(light);
    	lengthTo1(vNormal);
    	sc = pow(mmax(0.0, vectorDotX(light, vNormal)), globalspec.s);
    	vertx.r = vertx.r + mmax(0.0, vectorDotX(light, vNormal)) * diffcolor[0] * light_list[i].i + light_list[i].i * sc * globalspec.r;
    	vertx.g = vertx.g + mmax(0.0, vectorDotX(light, vNormal)) * diffcolor[1] * light_list[i].i + light_list[i].i * sc * globalspec.g;
    	vertx.b = vertx.b + mmax(0.0, vectorDotX(light, vNormal)) * diffcolor[2] * light_list[i].i + light_list[i].i * sc * globalspec.b;
    }
	ptr = &vertx;
	vertex_list[vertex_count] = vertx;
    vertex_count ++;
}

void osuEnd()
{
	double max_x;
    double min_x;
    double max_y;
    double min_y;
    struct vertex vertex_list_depth[128];
    int i;
    int x;
    int y;
    vectorp temp;
    struct vertex vertex1;
	struct vertex vertex2;
	struct vertex vertex3;
	double alpha;
    double beta;
    double gamma;
    double pixelcolor[3];
    double da;
    double db;
    double dc;
	double xx;
	double yy;
	double a1;
	double a2;
	double b1;
	double b2;
	double daa;
	double dbb;
	double dcc;

	double area;
	double s;
    double aa1;
    double bb1;
    double cc1;
    double aa2;
    double bb2;
    double cc2;
    double aa3;
    double bb3;
    double cc3;
    double alphap;
    double betap;
    double gammap;
    double edge1;
    double edge2;
    double edge3;
	double zfv;

	if (polygon_flag == OSU_POLYGON)
	{
		for (i = 0; i < 128; i++)
		{
			vertex_list_depth[i] = vertex_list[i];
		}
		if (projectionmethod == 1 && zbufferflag == OSU_DEPTH_TEST)
		{
			for (i = 0; i < vertex_count; i++)
			{
				temp[0] = vertex_list[i].x;
				temp[1] = vertex_list[i].y;
				temp[2] = vertex_list[i].z;
				temp[3] = 1.0;
				mXv(matrix_current, temp);

				vertex_list[i].x = temp[0] / temp[3];
				vertex_list[i].y = temp[1] / temp[3];
				vertex_list[i].z = temp[2] / temp[3];

				vertex_list_depth[i].z = temp[2] / temp[3];
			}
		}
		else if (projectionmethod == 2 && zbufferflag == OSU_DEPTH_TEST)
		{
			for (i = 0; i < vertex_count; i++)
			{
				temp[0] = vertex_list[i].x;
				temp[1] = vertex_list[i].y;
				temp[2] = vertex_list[i].z;
				temp[3] = 1.0;
				mXv(matrix_current, temp);

				vertex_list[i].x = temp[0] / temp[3];
				vertex_list[i].y = temp[1] / temp[3];
				vertex_list[i].z = temp[2] / temp[3];

				vertex_list_depth[i].z = 1.0 / temp[2];
			}
		}

		for (i = 0; i < vertex_count; i++)
		{
			temp[0] = vertex_list[i].x;
			temp[1] = vertex_list[i].y;
			temp[2] = vertex_list[i].z;
			temp[3] = 1.0;
			mXv(matrix_project, temp);

			vertex_list[i].x = temp[0] / temp[3];
			vertex_list[i].y = temp[1] / temp[3];
			vertex_list[i].z = temp[2] / temp[3];
		}

		struct vertex vertex_window[128];
		//vertex_window = new vertex[vertex_count];

		for (i = 0; i < vertex_count; i++)
		{
			vertex_window[i].x = vertex_list[i].x * (width / 2.0) + (width / 2.0);
			vertex_window[i].y = vertex_list[i].y * (width / 2.0) + (width / 2.0);
			vertex_window[i].z = vertex_list[i].z * (width / 2.0) + (width / 2.0);

			if (i == 0)
			{
				max_x = min_x = vertex_window[i].x;
				max_y = min_y = vertex_window[i].y;
			}

			max_x = mmax(max_x, vertex_window[i].x);
			min_x = mmin(min_x, vertex_window[i].x);
			max_y = mmax(max_y, vertex_window[i].y);
			min_y = mmin(min_y, vertex_window[i].y);

			if (shade_model == OSU_FLAT)
			{
				vertex_window[i].r = vertex_list[0].r;
				vertex_window[i].g = vertex_list[0].g;
				vertex_window[i].b = vertex_list[0].b;
			}
			else
			{
				vertex_window[i].r = vertex_list[i].r;
				vertex_window[i].g = vertex_list[i].g;
				vertex_window[i].b = vertex_list[i].b;
			}
		}

		if (vertex_count >= 3)
		{

			for (i = 1; i < vertex_count - 1; i++)
			{
				//get three vertexes
   				vertex1.x = vertex_window[0].x;
				vertex1.y = vertex_window[0].y;
				vertex1.r = vertex_window[0].r;
				vertex1.g = vertex_window[0].g;
				vertex1.b = vertex_window[0].b;
				vertex2.x = vertex_window[i].x;
				vertex2.y = vertex_window[i].y;
				vertex2.r = vertex_window[i].r;
				vertex2.g = vertex_window[i].g;
				vertex2.b = vertex_window[i].b;
				vertex3.x = vertex_window[i + 1].x;
				vertex3.y = vertex_window[i + 1].y;
				vertex3.r = vertex_window[i + 1].r;
				vertex3.g = vertex_window[i + 1].g;
				vertex3.b = vertex_window[i + 1].b;
				//for (x = min_x - 2; x < max_x + 5; x++)
				for (x = 0; x < 360; x++)
				{
					//for (y = min_y - 2; y < max_y + 5; y++)
					for (y = 0; y < 360; y++)
					{
						//beta = ((vertex1.y - vertex3.y) * (float)x / 360. + (vertex3.x - vertex1.x) * (float)y / 360. + vertex1.x * vertex3.y - vertex3.x * vertex1.y) / ((vertex1.y - vertex3.y) * vertex2.x + (vertex3.x - vertex1.x) * vertex2.y + vertex1.x * vertex3.y - vertex3.x * vertex1.y);
						//gamma = ((vertex1.y - vertex2.y) * (float)x / 360. + (vertex2.x - vertex1.x) * (float)y / 360. + vertex1.x * vertex2.y - vertex2.x * vertex1.y) / ((vertex1.y - vertex2.y) * vertex3.x + (vertex2.x - vertex1.x) * vertex3.y + vertex1.x * vertex2.y - vertex2.x * vertex1.y);

						beta = ((vertex1.y - vertex3.y) * (float)x + (vertex3.x - vertex1.x) * (float)y + vertex1.x * vertex3.y - vertex3.x * vertex1.y) / ((vertex1.y - vertex3.y) * vertex2.x + (vertex3.x - vertex1.x) * vertex2.y + vertex1.x * vertex3.y - vertex3.x * vertex1.y);
						gamma = ((vertex1.y - vertex2.y) * (float)x + (vertex2.x - vertex1.x) * (float)y + vertex1.x * vertex2.y - vertex2.x * vertex1.y) / ((vertex1.y - vertex2.y) * vertex3.x + (vertex2.x - vertex1.x) * vertex3.y + vertex1.x * vertex2.y - vertex2.x * vertex1.y);
						alpha = 1 - beta - gamma;
            			if (alpha>= 0 && beta >= 0 && gamma >= 0) {
							//da = sqrt(pow(vertex1.x - (float)x / 360., 2) + pow(vertex1.y - (float)y / 360., 2));
							//db = sqrt(pow(vertex2.x - (float)x / 360., 2) + pow(vertex2.y - (float)y / 360., 2));
							//dc = sqrt(pow(vertex3.x - (float)x / 360., 2) + pow(vertex3.y - (float)y / 360., 2));

							da = sqrt(pow(vertex1.x - (float)x, 2) + pow(vertex1.y - (float)y, 2));
							db = sqrt(pow(vertex2.x - (float)x, 2) + pow(vertex2.y - (float)y, 2));
							dc = sqrt(pow(vertex3.x - (float)x, 2) + pow(vertex3.y - (float)y, 2));

							//first cross point
							//if (vertex1.x == (float)x / 360.)
							if (vertex1.x == (float)x)
							{
								a2 = (vertex2.y - vertex3.y) / (vertex2.x - vertex3.x);
								b2 = vertex2.y - a2 * vertex2.x;
								xx = vertex1.x;
								yy = a2 * xx + b2;
							}
							else
							{
								//a1 = (vertex1.y - (float)y / 360.) / (vertex1.x - (float)x / 360.);
								a1 = (vertex1.y - (float)y) / (vertex1.x - (float)x);
								b1 = vertex1.y - a1 * vertex1.x;
								if (vertex2.x == vertex3.x)
								{
									xx = vertex2.x;
									yy = a1 * xx + b1;
								}
								else
								{
									a2 = (vertex2.y - vertex3.y) / (vertex2.x - vertex3.x);
									b2 = vertex2.y - a2 * vertex2.x;
									xx = (b1 - b2) / (a2 - a1);
												yy = a1 * xx + b1;
								}
							}
							daa = sqrt(pow(vertex1.x - xx, 2) + pow(vertex1.y - yy, 2));
							//second cross point
							//if (vertex2.x == (float)x / 360.)
							if (vertex2.x == (float)x)
							{
								a2 = (vertex3.y - vertex1.y) / (vertex3.x - vertex1.x);
								b2 = vertex3.y - a2 * vertex3.x;
								xx = vertex2.x;
								yy = a2 * xx + b2;
							}
							else
							{
								//a1 = (vertex2.y - (float)y / 360.) / (vertex2.x - (float)x / 360.);
								a1 = (vertex2.y - (float)y) / (vertex2.x - (float)x);
								b1 = vertex2.y - a1 * vertex2.x;
								if (vertex3.x == vertex1.x)
								{
									xx = vertex3.x;
									yy = a1 * xx + b1;
								}
								else
								{
									a2 = (vertex3.y - vertex1.y) / (vertex3.x - vertex1.x);
									b2 = vertex3.y - a2 * vertex3.x;
									xx = (b1 - b2) / (a2 - a1);
									yy = a1 * xx + b1;
								}
							}
							dbb = sqrt(pow(vertex2.x - xx, 2) + pow(vertex2.y - yy, 2));
							//third cross point
							//if (vertex3.x == (float)x / 360.)
							if (vertex3.x == (float)x)
							{
								a2 = (vertex1.y - vertex2.y) / (vertex1.x - vertex2.x);
								b2 = vertex1.y - a2 * vertex1.x;
								xx = vertex3.x;
								yy = a2 * xx + b2;
							}
							else
							{
								//a1 = (vertex3.y - (float)y / 360.) / (vertex3.x - (float)x / 360.);
								a1 = (vertex3.y - (float)y) / (vertex3.x - (float)x);
								b1 = vertex3.y - a1 * vertex3.x;
								if (vertex1.x == vertex2.x)
								{
									xx = vertex1.x;
									yy = a1 * xx + b1;
								}
								else
								{
									a2 = (vertex1.y - vertex2.y) / (vertex1.x - vertex2.x);
									b2 = vertex1.y - a2 * vertex1.x;
									xx = (b1 - b2) / (a2 - a1);
									yy = a1 * xx + b1;
								}
							}
							dcc = sqrt(pow(vertex3.x - xx, 2) + pow(vertex3.y - yy, 2));
							//set color
							pixelcolor[0] = (1 - da / daa) * vertex1.r + (1 - db / dbb) * vertex2.r + (1 - dc / dcc) * vertex3.r;
							pixelcolor[1] = (1 - da / daa) * vertex1.g + (1 - db / dbb) * vertex2.g + (1 - dc / dcc) * vertex3.g;
							pixelcolor[2] = (1 - da / daa) * vertex1.b + (1 - db / dbb) * vertex2.b + (1 - dc / dcc) * vertex3.b;
							
							if (zbufferflag == OSU_DEPTH_TEST)
							{
								vertex_window[0].z = vertex_list_depth[0].z;
								vertex_window[i].z = vertex_list_depth[i].z;
								vertex_window[i + 1].z = vertex_list_depth[i + 1].z;

								edge1 = sqrt((vertex_window[i + 1].x - vertex_window[i].x) * (vertex_window[i + 1].x - vertex_window[i].x) + (vertex_window[i + 1].y - vertex_window[i].y) * (vertex_window[i + 1].y - vertex_window[i].y));
    							edge2 = sqrt((vertex_window[0].x - vertex_window[i + 1].x) * (vertex_window[0].x - vertex_window[i + 1].x) + (vertex_window[0].y - vertex_window[i + 1].y) * (vertex_window[0].y - vertex_window[i + 1].y));
    							edge3 = sqrt((vertex_window[i].x - vertex_window[0].x) * (vertex_window[i].x - vertex_window[0].x) + (vertex_window[i].y - vertex_window[0].y) * (vertex_window[i].y - vertex_window[0].y));

    							s = (edge1 + edge2 + edge3) * 0.5;
							    area = sqrt(s * (s - edge1) * (s - edge2) * (s - edge3));

							    aa1 = vertex_window[i + 1].y - vertex_window[i].y;
    							bb1 = vertex_window[i].x - vertex_window[i + 1].x;
    							cc1 = vertex_window[i + 1].x * vertex_window[i].y - vertex_window[i].x * vertex_window[i + 1].y;

    							aa2 = vertex_window[0].y - vertex_window[i + 1].y;
    							bb2 = vertex_window[i + 1].x - vertex_window[0].x;
    							cc2 = vertex_window[0].x * vertex_window[i + 1].y - vertex_window[i + 1].x * vertex_window[0].y;

    							aa3 = vertex_window[i].y - vertex_window[0].y;
    							bb3 = vertex_window[0].x - vertex_window[i].x;
    							cc3 = vertex_window[i].x * vertex_window[0].y - vertex_window[0].x * vertex_window[i].y;

    							alphap = (fabs(x * aa1 + y * bb1 + cc1) * 0.5 * edge1) / (sqrt(aa1 * aa1 + bb1 * bb1) * area);
    							betap = (fabs(x * aa2 + y * bb2 + cc2) * 0.5 * edge2) / (sqrt(aa2 * aa2 + bb2 * bb2) * area);
    							gammap = (fabs(x * aa3 + y * bb3 + cc3) * 0.5 * edge3) / (sqrt(aa3 * aa3 + bb3 * bb3) * area);
    							zfv = alphap * vertex_window[0].z + betap * vertex_window[i].z + gammap * vertex_window[i + 1].z;

								if (zbuffer[y * width + x] < zfv)
								{
									osuWritePixel(x, y, pixelcolor[0] * 255, pixelcolor[1] * 255, pixelcolor[2] * 255);
									zbuffer[y * width + x] = zfv;
								}
							}
							else
							{
								osuWritePixel(x, y, pixelcolor[0] * 255, pixelcolor[1] * 255, pixelcolor[2] * 255);
							}        			   
            			}
					}
				}
			}
			vertex_count = 0;
		}
	}
}

void loadAndDrawObj(char *fname)
{
	ObjModel data;
	ObjLoader LoaderClass;

	LoaderClass.LoadObj(fname);
	data = LoaderClass.ReturnObj();


	//For flat shading, only make one call to osuNormal in the beginning, and use the following
	// to access the faceNormal
	//
	//
	// osuNormal((data.TriangleArray[i].faceNormal[0], 
	//		 (data.TriangleArray[i].faceNormal[1], 
	//		  data.TriangleArray[i].faceNormal[2]); 
		
	for(int i = 0; i < data.NumTriangle; i++)  {						

		osuBegin(OSU_POLYGON);


		osuNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[0]].X, 
				data.NormalArray[data.TriangleArray[i].Vertex[0]].Y, 
				data.NormalArray[data.TriangleArray[i].Vertex[0]].Z); 
		osuVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[0]].X, 
				data.VertexArray[data.TriangleArray[i].Vertex[0]].Y, 
				data.VertexArray[data.TriangleArray[i].Vertex[0]].Z);

	
		osuNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[1]].X, 
				data.NormalArray[data.TriangleArray[i].Vertex[1]].Y, 
				data.NormalArray[data.TriangleArray[i].Vertex[1]].Z); 
		osuVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[1]].X, 
				data.VertexArray[data.TriangleArray[i].Vertex[1]].Y, 
				data.VertexArray[data.TriangleArray[i].Vertex[1]].Z);

		osuNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[2]].X, 
				data.NormalArray[data.TriangleArray[i].Vertex[2]].Y, 
				data.NormalArray[data.TriangleArray[i].Vertex[2]].Z); 
		osuVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[2]].X, 
				data.VertexArray[data.TriangleArray[i].Vertex[2]].Y, 
				data.VertexArray[data.TriangleArray[i].Vertex[2]].Z);
		osuEnd();

	}
}


void objTest()
{
	osuPerspective(90.0, 1.0, 1000);
	osuClear(0.0,0.0,0.0);	
	
	float from[3]={0.0,1,-3.0};
	float at[3] = {0.0,0.0,10.0};
	float up[3] = {0.0, 1.0, 0.0};

	osuLookat (from, at, up);
	//Diffuse blue color
	osuDiffuse(0.0, 0.0 , 1.0 );

	//Specular white color
	osuSpecular(1.0, 1.0, 1.0, 5.0);

	float lpos[3]={0.0, 3.0, -4.0};

	osuPointLight(lpos,0.7);
	osuAmbientLight(0.2);

	// To test flath shading, use test.obj 
	// Look at it from behind... ie osuLookAt(0,1,-3, 0,0,10,0,1,0)
	loadAndDrawObj("test.obj");

	// To test smooth shading, use face.ws.obj
	// Look at it from in front osuLookAt(0, 0, 2, 0, 0, 0, 0, 1, 0)
	//loadAndDrawObj("face.ws.obj");
}

//Test functions
void DepthTest()
{
    osuPerspective(90.0, 1.0, -1000.);

    float from[3] = { 3.0, 3.0, 3.0 };
    float at[3] = { 0.0, 0.0, -8.0 };
    float up[3] = { 0.0, 1.0, 0.0 };
    osuLookat(from, at, up);

    osuClear(0.0, 0.0, 0.0);
    osuShadeModel(OSU_SMOOTH);
    osuEnable(OSU_DEPTH_TEST);
    osuClearZ();

    osuDiffuse(0.0, 0.0, 1.0);
    osuSpecular(1.0, 1.0, 1.0, 3);

    float lpos[3] = { 0.0, 20.5, 15.0 };
    osuPointLight(lpos, 1.0);
    osuAmbientLight(1.0);

    osuNormal3f(0.0, 1.0, 0.0);
    //YOU MUST CONVERT THIS TO TWO TRIANGLES!!!
    osuBegin(OSU_POLYGON);
    osuVertex3f(-4.5, -1.75, -5.5);
    osuVertex3f(-4.5, 1.75, -5.5);
    osuVertex3f(4.5, 1.75, -5.5);
    osuVertex3f(4.5, -1.75, -5.5);
    osuEnd();

    osuDiffuse(0.0, 1.0, 0.0);
    osuSpecular(1.0, 1.0, 1.0, 1.0);
    //osuColor3f(0.0, 1.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(-2.0, -1.0, -3.5);
    osuVertex3f(-2.0, 1.0, -3.5);
    osuVertex3f(5.5, 1.5, -6.5);
    osuVertex3f(5.5, -1.5, -6.5);
    osuEnd();

    osuDiffuse(1.0, 0.0, 0.0);
    osuSpecular(1.0, 1.0, 1.0, 1.0);

    //osuColor3f(0.0, 0.0, 1.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(0.0, -1.75, -2.5);
    osuVertex3f(0.0, 1.75, -2.5);
    osuVertex3f(0.0, 1.75, -7.5);
    osuVertex3f(0.0, -1.75, -7.5);
    osuEnd();

}

void simpleTest()
{
    osuPerspective(90.0, 1.0, -1000.);

    float from[3] = { 3.0, 0.0, 3.0 };
    float at[3] = { 0.0, 0.0, -8.0 };
    float up[3] = { 0.0, 1.0, 0.0 };
    osuLookat(from, at, up);

    osuClear(0.0, 0.0, 0.0);
    osuShadeModel(OSU_SMOOTH);
    osuEnable(OSU_DEPTH_TEST);
    osuClearZ();

    osuDiffuse(0.0, 0.0, 1.0);
    osuSpecular(1.0, 1.0, 1.0, 1.0);

    float lpos[3] = { 0.0, 1.5, 5.0 };

    osuPointLight(lpos, 1.0);
    osuAmbientLight(1.0);

    //YOU MUST CONVERT THIS TO TWO TRIANGLES!!!
    //osuColor3f(0.0, 1.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuNormal3f(0.0, 1.0, 0.0);
    osuVertex3f(-4.5, -1.75, -5.5);
    osuVertex3f(-4.5, 1.75, -5.5);
    osuVertex3f(4.5, 1.75, -5.5);
    osuVertex3f(4.5, -1.75, -5.5);
    osuEnd();

    osuDiffuse(1.0, 0.0, 0.0);
    osuSpecular(1.0, 1.0, 1.0, 1.0);

    //osuColor3f(0.0, 0.0, 1.0);
    osuBegin(OSU_POLYGON);
    osuNormal3f(0.0, 1.0, 0.0);
    osuVertex3f(0.0, -1.75, -2.5);
    osuVertex3f(0.0, 1.75, -2.5);
    osuVertex3f(0.0, 1.75, -7.5);
    osuVertex3f(0.0, -1.75, -7.5);
    osuEnd();

}

void blueCube()
{
    osuPerspective(40, 7.5, 100);

    float from[3] = { 4.0, 4.0, 4.0 };
    float at[3] = { 0.0, 0.0, 0.0 };
    float up[3] = { 0.0, 1.0, 0.0 };
    osuLookat(from, at, up);

    osuClear(0.0, 0.0, 0.0);
    osuShadeModel(OSU_SMOOTH);
    osuEnable(OSU_DEPTH_TEST);
    osuClearZ();

    osuDiffuse(0.0, 0.0, 1.0);
    osuSpecular(1.0, 1.0, 1.0, 2.0);

    float lpos[3] = { 2.0, 2.0, 2.0 };
    float dir[3] = { 0.0, -1.0, 0.0 };

    osuPointLight(lpos, 1.0);
    //osuDirectionalLight(dir, 0.5);
    osuAmbientLight(1.0);

    //YOU MUST CONVERT THESE TO USE TRIANGLES!!!
    //back
    //osuColor3f(1.0, 0.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuNormal3f(0.0, 0.0, -1.0);
    osuVertex3f(-1, -1, -1);
    osuVertex3f(1, -1, -1);
    osuVertex3f(1, 1, -1);
    osuVertex3f(-1, 1, -1);
    osuEnd();

    //right
    //osuColor3f(0.0, 1.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuNormal3f(1.0, 0.0, 0.0);
    osuVertex3f(1, -1, -1);
    osuVertex3f(1, -1, 1);
    osuVertex3f(1, 1, 1);
    osuVertex3f(1, 1, -1);
    osuEnd();


    //front
    //osuColor3f(0.0, 0.0, 1.0);
    osuNormal3f(0.0, 0.0, 1.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(-1, -1, 1);
    osuVertex3f(-1, 1, 1);
    osuVertex3f(1, 1, 1);
    osuVertex3f(1, -1, 1);
    osuEnd();

    //top
    //osuColor3f(1.0, 1.0, 0.0);
    osuNormal3f(0.0, 1.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(-1, 1, -1);
    osuVertex3f(1, 1, -1);
    osuVertex3f(1, 1, 1);
    osuVertex3f(-1, 1, 1);
    osuEnd();


    //bottom
    //osuColor3f(0.0, 1.0, 1.0);
    osuNormal3f(0.0, -1.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(-1, -1, -1);
    osuVertex3f(-1, -1, 1);
    osuVertex3f(1, -1, 1);
    osuVertex3f(1, -1, -1);
    osuEnd();


    //left
    //osuColor3f(1.0, 0.0, 1.0);
    osuNormal3f(-1.0, 0.0, 0.0);
    osuBegin(OSU_POLYGON);
    osuVertex3f(-1, -1, -1);
    osuVertex3f(-1, 1, -1);
    osuVertex3f(-1, 1, 1);
    osuVertex3f(-1, -1, 1);
    osuEnd();
}

//Test functions end


void main(int argc , char **argv)
{
    int xsize = 360;
    int ysize = 360;

  
    int num = atoi(argv[1]);
	//int num = 4;

    /* create a framebuffer */
    osuBeginGraphics (xsize, ysize);

    osuFlush();

   /* initialize the matrix stack */
    osuInitialize();
    osuEnable (OSU_DEPTH_TEST);
 
  
  switch (num) {
    case 1:
     	osuShadeModel(OSU_FLAT);
     	objTest();
     	break;
    case 2:
		simpleTest();
        break;
    case 3:
    	blueCube();
        break;
    case 4:
		DepthTest();
        break;
	default:
		fprintf(stderr, "No such test case\n");
		exit(-1);
  }

  osuFlush();
  printf ("Press 'escape' to exit.\n");
  osuWaitOnEscape();
  osuEndGraphics();

}