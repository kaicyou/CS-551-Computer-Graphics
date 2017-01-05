/*

Dummy routines for matrix transformations.

These are for you to write!

*/


#include <stdio.h>
#include <math.h>
#include <fstream>
#include <Windows.h>
#include <ostream>
#include <sstream>
#include <string>

#include "osuGraphics.h"
#include "lines.h"

//-------------------------------------------------
/*
struct vertex2
{
	double x;
	double y;
	double r;
	double g;
	double b;
};
*/

struct vertex3
{
	double x;
	double y;
	double z;
	double r;
	double g;
	double b;
};

typedef double matrix[4][4];
typedef double vector[4];

OSUDrawable polygon_flag;
double color[3];
int color_set = 0;
struct vertex3 vertex_list[128];
int vertex_count;

matrix matrix_project;
matrix matrix_current;
matrix matrix_look;

int width;
int height;

double matrix_stack[16][4][4];
int matrix_count;
//operation for vectors
double vectorDotX(vector &a, vector &b)
{
	double result;
	result = a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
	return result;
}

void vectorCrossX(vector &a, vector &b, vector &r)
{
	r[0] = a[1] * b[2] - a[2] * b[1];
	r[1] = a[2] * b[0] - a[0] * b[2];
	r[2] = a[0] * b[1] - a[1] * b[0];
	r[3] = 0;
}

double vectorLength(vector a)
{
	double result;
	result = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	return result;
}

void mXv(matrix m, vector a)
{
	int i;
	vector temp;
	for (i = 0; i < 4; i++)
	{
		temp[i] = m[0][i] * a[0] + m[1][i] * a[1] + m[2][i] * a[2] + m[3][i] * a[3];
	}
	for (i = 0; i < 4; i++)
	{
		a[i] = temp[i];
	}
}

void lengthTo1(vector &a)
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

void osuOrtho(double left, double right, double bottom, double top, double nearp,double farp)
{ 
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
	double f = 1. / tan((fovy / 2) * 3.1415927 / 180.);
	matrixreset(matrix_project);
	matrix_project[0][0] = f;
	matrix_project[1][1] = f;
	matrix_project[2][2] = (farp + nearp) / (nearp - farp);
	matrix_project[2][3] = -1.;
	matrix_project[3][2] = 2. * (farp * nearp) / (nearp - farp);
	matrix_project[3][3] = 0;
}
void osuBegin(OSUDrawable mode)
{
	polygon_flag = mode;
    vertex_count = 0;
    osuGetFramebufferSize(&width, &height);
}

void osuEnd()
{
	struct vertex3 vertexS;
	struct vertex3 vertexE;
	vector v1;
	vector v2;
	int temp;
	double x1;
	double y1;
	double x2;
	double y2;

	if (polygon_flag != OSU_LINES) {
        fprintf (stderr, "Please use OSU_LINES.\n");
        exit(-1);
    }
    if (vertex_count < 2) {
        fprintf (stderr, "Please input at least 2 vertexes.\n");
        exit(-1);
    }

    while(vertex_count >= 2)
    {
    	//vetex 1
    	vertexS.x = vertex_list[vertex_count - 2].x;
    	vertexS.y = vertex_list[vertex_count - 2].y;
    	vertexS.z = vertex_list[vertex_count - 2].z;
    	//vertex 2
    	vertexE.x = vertex_list[vertex_count - 1].x;
    	vertexE.y = vertex_list[vertex_count - 1].y;
    	vertexE.z = vertex_list[vertex_count - 1].z;
    	//vector 1
    	v1[0] = vertexS.x;
    	v1[1] = vertexS.y;
    	v1[2] = vertexS.z;
    	v1[3] = 1;
    	//vector 2
    	v2[0] = vertexE.x;
    	v2[1] = vertexE.y;
    	v2[2] = vertexE.z;
    	v2[3] = 1;
    	//transform 1
		mXv(matrix_current, v1);
		mXv(matrix_look, v1);
		mXv(matrix_project, v1);
    	//transform 2
		mXv(matrix_current, v2);
		mXv(matrix_look, v2);
		mXv(matrix_project, v2);
    	//vertexS
    	vertexS.x = v1[0] / v1[3];
    	vertexS.y = v1[1] / v1[3];
    	vertexS.z = v1[2] / v1[3];
    	//vertexE
    	vertexE.x = v2[0] / v2[3];
    	vertexE.y = v2[1] / v2[3];
    	vertexE.z = v2[2] / v2[3];

    	temp = near_far_clip(-1.0, 1.0, &vertexS.x, &vertexS.y, &vertexS.z, &vertexE.x, &vertexE.y, &vertexE.z);
    	if (temp == 1)
    	{
    		x1 = vertexS.x * (width / 2.) + (width / 2.);
    		x2 = vertexE.x * (width / 2.) + (width / 2.);
    		y1 = vertexS.y * (height / 2.) + (height / 2.);
    		y2 = vertexE.y * (height / 2.) + (height / 2.);
    		draw_line(x1, y1, x2, y2);
    	}
    	vertex_count = vertex_count - 2;
    }
	vertex_count = 0;
}

void osuColor3f(double red, double green, double blue)
{
	color[0] = red * 255.;
    color[1] = green * 255.;
    color[2] = blue * 255.;
    color_set = 1;
}

void osuVertex2f(double x, double y)
{
	
}



void osuVertex3f(double x, double y, double z)
{
	struct vertex3 vertx3;
	struct vertex3 *ptr = new vertex3;
    vertx3.x = x;
    vertx3.y = y;
    vertx3.z = z;
    if (color_set == 1) {
        vertx3.r = color[0];
    	vertx3.g = color[1];
    	vertx3.b = color[2];
    }
	ptr = &vertx3;
	vertex_list[vertex_count] = vertx3;
    vertex_count ++;
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
	vector temp;
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

void osuLookat(double from[3], double at[3], double up[3])
{
	int i;
	vector x;
	vector y;
	vector z;
	vector temp;

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