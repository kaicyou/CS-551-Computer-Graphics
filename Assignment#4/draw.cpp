#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <malloc.h>
#include "osuGraphics.h"

typedef double matrix[4][4];
typedef double vectorp[4];

struct vertex
{
	double x;
	double y;
	double r;
	double g;
	double b;
};

struct lightinfo
{
	double r;
	double g;
	double b;
	vectorp lp; //light position
};

struct Specular{
	float r;
	float g;
	float b;
	float s;
} globalspec{ 1.0, 1.0, 1.0, 0.0 };

matrix matrix_project;
matrix matrix_current;
matrix matrix_look;
double matrix_stack[16][4][4];
int matrix_count;

int width;
int height;

OSUDrawable polygon_flag;
double color[3];
double ambient[4];
double diff[4];
double spec[4];
double light_color[3];
struct vertex vertex_list[3];
int vertex_count;
double att;
double shine_N;
vectorp vNormal;
vectorp view;
struct lightinfo light;
double RR;

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

double dot(vectorp a, vectorp b)
{
	double r1, r2, r3, r4;
	r1 = a[0] * b[0];
	r2 = a[1] * b[2];
	r3 = a[2] * b[2];
	r4 = a[3] * b[3];
	return (r1 + r2 + r3 + r4);
}
//opeartion end

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
	matrix_count++;
}

void osuAmbientLight(double red, double green, double blue, double i)
{
	ambient[0] = red;
	ambient[1] = green;
	ambient[2] = blue;
	ambient[3] = i;
}

void osuDiffuse(double r, double g, double b, double i)
{
	diff[0] = r;
	diff[1] = g;
	diff[2] = b;
	diff[3] = i;
}

void osuSpecular(double r, double g, double b, double i)
{
	spec[0] = r;
	spec[1] = g;
	spec[2] = b;
	spec[3] = i;
}

void osuLight(double pos[3], double r, double g, double b)
{
	light.lp[0] = pos[0];
	light.lp[1] = pos[1];
	light.lp[2] = pos[2];
	light.r = r;
	light.g = g;
	light.b = b;
}

void osuAttenuation(double i)
{
	att = i;
}

void osuShine(double i)
{
	double shine_N = i;
}

void osuNormal3f(double x, double y, double z)
{
	vNormal[0] = x;
	vNormal[1] = y;
	vNormal[2] = z;
}

void osuView(double v[3])
{
	view[0] = v[0];
	view[1] = v[1];
	view[2] = v[2];
	view[3] = 0;
}

void osuBegin(OSUDrawable mode)
{
	polygon_flag = mode;
	//color_set = 0;
	vertex_count = 0;
}
void osuColor3f(double red, double green, double blue)
{
	color[0] = red;
	color[1] = green;
	color[2] = blue;
}

void osuVertex2f(double x, double y)
{
	struct vertex vertx;
	struct vertex *ptr = new vertex;

	vertx.x = x;
	vertx.y = y;
	vertx.r = color[0];
	vertx.g = color[1];
	vertx.b = color[2];
	ptr = &vertx;
	vertex_list[vertex_count] = vertx;
	vertex_count++;
}

void osuEnd()
{
	struct vertex vertex1;
	struct vertex vertex2;
	struct vertex vertex3;
	double max_x, min_x, max_y, min_y;
	int i, j, q;
	double alpha, beta, gamma;
	double pixelcolor[3];
	double da, db, dc;
	double xx, yy;
	double a1, a2, b1, b2;
	double daa, dbb, dcc;
	double Ir, Ig, Ib;
	vectorp R;
	

	//make sure w can draw triangle
	if (polygon_flag != OSU_TRIANGLE) {
		fprintf(stderr, "Please use OSU_TRIANGLE.\n");
		exit(-1);
	}
	if (vertex_count < 3) {
		fprintf(stderr, "Please input 3 vertexes.\n");
		exit(-1);
	}
	//get three vertexes
	vertex1.x = vertex_list[0].x;
	vertex1.y = vertex_list[0].y;
//	vertex1.r = vertex_list[0].r;
//	vertex1.g = vertex_list[0].g;
//	vertex1.b = vertex_list[0].b;
	vertex2.x = vertex_list[1].x;
	vertex2.y = vertex_list[1].y;
//	vertex2.r = vertex_list[1].r;
//	vertex2.g = vertex_list[1].g;
//	vertex2.b = vertex_list[1].b;
	vertex3.x = vertex_list[2].x;
	vertex3.y = vertex_list[2].y;
//	vertex3.r = vertex_list[2].r;
//	vertex3.g = vertex_list[2].g;
//	vertex3.b = vertex_list[2].b;
	//get the max and min xy
	//max x
	if (vertex1.x >= vertex2.x) {
		max_x = vertex1.x;
	}
	else
		max_x = vertex2.x;
	if (max_x <= vertex3.x) {
		max_x = vertex3.x;
	}
	//min x
	if (vertex1.x <= vertex2.x) {
		min_x = vertex1.x;
	}
	else
		min_x = vertex2.x;
	if (min_x >= vertex3.x) {
		min_x = vertex3.x;
	}
	//max y
	if (vertex1.y >= vertex2.y) {
		max_y = vertex1.y;
	}
	else
		max_y = vertex2.y;
	if (max_y <= vertex3.y) {
		max_y = vertex3.y;
	}
	//min y
	if (vertex1.y <= vertex2.y) {
		min_y = vertex1.y;
	}
	else
		min_y = vertex2.y;
	if (min_y >= vertex3.y) {
		min_y = vertex3.y;
	}
	//exam every pixel in the square limited by max and min xy, if it is in the triangle, draw it.
	for (i = min_x * 500; i <= max_x * 500; i++) {
		for (j = min_y * 500; j <= max_y * 500; j++) {
			beta = ((vertex1.y - vertex3.y) * (float)i / 500. + (vertex3.x - vertex1.x) * (float)j / 500. + vertex1.x * vertex3.y - vertex3.x * vertex1.y) / ((vertex1.y - vertex3.y) * vertex2.x + (vertex3.x - vertex1.x) * vertex2.y + vertex1.x * vertex3.y - vertex3.x * vertex1.y);
			gamma = ((vertex1.y - vertex2.y) * (float)i / 500. + (vertex2.x - vertex1.x) * (float)j / 500. + vertex1.x * vertex2.y - vertex2.x * vertex1.y) / ((vertex1.y - vertex2.y) * vertex3.x + (vertex2.x - vertex1.x) * vertex3.y + vertex1.x * vertex2.y - vertex2.x * vertex1.y);
			alpha = 1 - beta - gamma;
			if (alpha >= 0 && beta >= 0 && gamma >= 0) {
/*				da = sqrt(pow(vertex1.x - (float)i / 500., 2) + pow(vertex1.y - (float)j / 500., 2));
				db = sqrt(pow(vertex2.x - (float)i / 500., 2) + pow(vertex2.y - (float)j / 500., 2));
				dc = sqrt(pow(vertex3.x - (float)i / 500., 2) + pow(vertex3.y - (float)j / 500., 2));
				//first cross point
				if (vertex1.x == (float)i / 500.)
				{
					a2 = (vertex2.y - vertex3.y) / (vertex2.x - vertex3.x);
					b2 = vertex2.y - a2 * vertex2.x;
					xx = vertex1.x;
					yy = a2 * xx + b2;
				}
				else
				{
					a1 = (vertex1.y - (float)j / 500.) / (vertex1.x - (float)i / 500.);
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
				if (vertex2.x == (float)i / 500.)
				{
					a2 = (vertex3.y - vertex1.y) / (vertex3.x - vertex1.x);
					b2 = vertex3.y - a2 * vertex3.x;
					xx = vertex2.x;
					yy = a2 * xx + b2;
				}
				else
				{
					a1 = (vertex2.y - (float)j / 500.) / (vertex2.x - (float)i / 500.);
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
				if (vertex3.x == (float)i / 500.)
				{
					a2 = (vertex1.y - vertex2.y) / (vertex1.x - vertex2.x);
					b2 = vertex1.y - a2 * vertex1.x;
					xx = vertex3.x;
					yy = a2 * xx + b2;
				}
				else
				{
					a1 = (vertex3.y - (float)j / 500.) / (vertex3.x - (float)i / 500.);
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
				*/


				lengthTo1(vNormal);
				for (q = 0; q < 4; q++)
				{
					R[q] = 2 * vNormal[q] * (vNormal[q] * light.lp[q]) - light.lp[q];
				}
				Ir = ambient[0] * ambient[3] * diff[0] + att * light.r * (diff[3] * diff[0] * dot(vNormal, light.lp) + spec[3] * spec[0] * pow(dot(R, view), shine_N));
				Ig = ambient[1] * ambient[3] * diff[1] + att * light.g * (diff[3] * diff[1] * dot(vNormal, light.lp) + spec[3] * spec[1] * pow(dot(R, view), shine_N));
				Ib = ambient[1] * ambient[3] * diff[2] + att * light.b * (diff[3] * diff[2] * dot(vNormal, light.lp) + spec[3] * spec[2] * pow(dot(R, view), shine_N));
				osuWritePixel(i, j, Ir * 255, Ig * 255, Ib * 255);
				//osuWritePixel(i, j, pixelcolor[0] * 255, pixelcolor[1] * 255, pixelcolor[2] * 255);
			}
		}
	}
	//clear some global variables
	vertex_count = 0;
}

void wedges(double xc, double yc, double radius, int steps)
{
	int i;
	double theta;
	double x, y;
	double xold, yold;
	RR = radius;

	theta = 2 * 3.1415926535 * (0.5) / (double)steps;
	xold = xc + radius * cos(theta);
	yold = yc + radius * sin(theta);

	for (i = 1; i <= steps; i++) {

		theta = 2 * 3.1415926535 * (i + 0.5) / (double)steps;
		x = xc + radius * cos(theta);
		y = yc + radius * sin(theta);

		osuBegin(OSU_TRIANGLE);
		osuVertex2f(xc, yc);
		osuVertex2f(x, y);
		osuVertex2f(xold, yold);
		osuEnd();

		xold = x;
		yold = y;
	}
}

void main(int argc, char **argv)
{
	double lpos[3] = { 0.0, 3.0, 4.0 };
	double vv[3] = { 3.0, 0.0, 3.0 };

	osuBeginGraphics(500, 500);
	osuInitialize();

	osuAmbientLight(1.0, 0.0, 0.0, 0.2);
	osuDiffuse(1.0, 0.0, 0.0, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuSetWriteMode(OSU_XOR);
	wedges(0.2, 0.2, 0.18, 50);

	osuBegin(OSU_TRIANGLE);
	osuAmbientLight(0.30, 0.35, 0.5, 0.2);
	osuDiffuse(0.10, 0.5, 0.35, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuVertex2f(0.11, 0.96);
	osuVertex2f(0.23, 0.65);
	osuVertex2f(0.37, 0.6);
	osuEnd();

	osuBegin(OSU_TRIANGLE);
	osuAmbientLight(0.80, 0.75, 0.1, 0.2);
	osuDiffuse(0.60, 0.21, 0.73, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuVertex2f(0.43, 0.86);
	osuVertex2f(0.54, 0.48);
	osuVertex2f(0.72, 0.67);
	osuEnd();

	osuAmbientLight(0.0, 1.0, 0.0, 0.2);
	osuDiffuse(0.0, 1.0, 0.0, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuSetWriteMode(OSU_XOR);
	wedges(0.8, 0.8, 0.12, 50);

	osuBegin(OSU_TRIANGLE);
	osuAmbientLight(0.0, 0.5, 0.5, 0.2);
	osuDiffuse(0.0, 0.5, 0.5, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuVertex2f(0.95, 0.12);
	osuVertex2f(0.73, 0.18);
	osuVertex2f(0.77, 0.48);
	osuEnd();

	osuBegin(OSU_TRIANGLE);
	osuAmbientLight(0.12, 0.0, 0.54, 0.2);
	osuDiffuse(0.0, 0.24, 0.69, 0.3);
	osuSpecular(1.0, 1.0, 1.0, 0.5);
	osuLight(lpos, 1.0, 1.0, 1.0);
	osuAttenuation(0.5);
	osuShine(200);
	osuNormal3f(0.0, 1.0, 0.0);
	osuView(vv);
	osuVertex2f(0.62, 0.07);
	osuVertex2f(0.54, 0.42);
	osuVertex2f(0.69, 0.34);
	osuEnd();

	osuFlush();
	osuWaitOnEscape();
	osuEndGraphics();
}