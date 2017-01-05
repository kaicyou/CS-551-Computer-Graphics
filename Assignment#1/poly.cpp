/*

Test the polygon scan conversion routines.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "osuGraphics.h"

struct vertex
{
	double x;
	double y;
	double r;
	double g;
	double b;
};

void osuColor3f (double, double, double);
void osuVertex2f (double, double);
void osuBegin (int);
void osuEnd();
void simple_triangle();
void abutting_triangles();
void color_polygons();
void wedges(double,double,double,int);
void overlapTest();

OSUDrawable polygon_flag;
double color[3];
int color_set = 0;
struct vertex vertex_list[3];
int vertex_count;

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
    color_set = 1;
}
void osuVertex2f(double x, double y)
{
	struct vertex vertx;
	struct vertex *ptr = new vertex;
    if (color_set != 1) {
        fprintf (stderr, "Please set the color first.\n");
        exit(-1);
    }
    vertx.x = x;
    vertx.y = y;
    vertx.r = color[0];
    vertx.g = color[1];
    vertx.b = color[2];
	ptr = &vertx;
	vertex_list[vertex_count] = vertx;
    vertex_count ++;
}
void osuEnd()
{
	struct vertex vertex1;
	struct vertex vertex2;
	struct vertex vertex3;
    double max_x;
    double min_x;
    double max_y;
    double min_y;
    int i;
    int j;
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
    //make sure w can draw triangle
    if (polygon_flag != OSU_TRIANGLE) {
        fprintf (stderr, "Please use OSU_TRIANGLE.\n");
        exit(-1);
    }
    if (vertex_count < 3) {
        fprintf (stderr, "Please input 3 vertexes.\n");
        exit(-1);
    }
    //get three vertexes
    vertex1.x = vertex_list[0].x;
	vertex1.y = vertex_list[0].y;
	vertex1.r = vertex_list[0].r;
	vertex1.g = vertex_list[0].g;
	vertex1.b = vertex_list[0].b;
	vertex2.x = vertex_list[1].x;
	vertex2.y = vertex_list[1].y;
	vertex2.r = vertex_list[1].r;
	vertex2.g = vertex_list[1].g;
	vertex2.b = vertex_list[1].b;
	vertex3.x = vertex_list[2].x;
	vertex3.y = vertex_list[2].y;
	vertex3.r = vertex_list[2].r;
	vertex3.g = vertex_list[2].g;
	vertex3.b = vertex_list[2].b;
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
            if (alpha>= 0 && beta >= 0 && gamma >= 0) {
				da = sqrt(pow(vertex1.x - (float)i / 500., 2) + pow(vertex1.y - (float)j / 500., 2));
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
                osuWritePixel(i, j, pixelcolor[0] * 255, pixelcolor[1] * 255, pixelcolor[2] * 255);
            }
        }
    }
    //clear some global variables
    vertex_count = 0;
//    vertex_list[0] = NULL;
//   vertex_list[1] = NULL;

//    vertex_list[2] = NULL;
}

/******************************************************************************
Draw a polygon.
*****************************************************************************/
void main(int argc , char **argv)
{

	int num = atoi(argv[1]);
	if((num <0 ) || (num > 5))
	{
		fprintf(stderr, "Please call this program with a number from 1 to 5 \n");
		exit(-1);
	}

  osuBeginGraphics (500, 500);

  /* go to selected routine */

  switch (num) {
      case 1:
      simple_triangle();
      break;
    case 2:
		osuSetWriteMode(OSU_XOR);
      abutting_triangles();
      break;
    case 3:
		osuSetWriteMode(OSU_XOR);
      color_polygons();
      break;
	 case 4:
	   osuColor3f(1.0, 1.0,1.0);
	   osuSetWriteMode(OSU_XOR);
	   wedges(0.501, 0.497, 0.4, 13); 
	   break;
	 case 5:
		//overlap
	   osuSetWriteMode(OSU_XOR);
		overlapTest();
		break;
    default:
      fprintf (stderr, "Please use a number from 1 to 5.\n");
      exit (-1);
  }

  osuFlush();
  osuWaitOnEscape();
  osuEndGraphics();
}


/******************************************************************************
Draw a white triangle.
******************************************************************************/

void simple_triangle()
{
  osuBegin (OSU_TRIANGLE);
  osuColor3f (1.0, 1.0, 1.0);
  osuVertex2f (0.25, 0.25);
  osuVertex2f (0.25, 0.75);
  osuVertex2f (0.75, 0.5);
  osuEnd ();
}


/******************************************************************************
Draw several abutting rectangles.
******************************************************************************/

void abutting_triangles()
{
  double a = 0.1; 
  double b = 0.5;
  double c = 0.9;

  //#ifdef NOTDEF
  osuBegin (OSU_TRIANGLE);
  osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (a, a);
	osuVertex2f (b, b);
	osuVertex2f (a, c);
  osuEnd();
 
  osuBegin(OSU_TRIANGLE);
  osuColor3f(1.0, 0.0, 0.0);
	osuVertex2f(b,b);
	osuVertex2f(c,c);
	osuVertex2f (a,c);
  osuEnd();


  osuBegin(OSU_TRIANGLE);
  osuColor3f(0.0, 1.0, 0.0);
	osuVertex2f(b,b);
	osuVertex2f(c,c);
	osuVertex2f (c,a);
  osuEnd();


  osuBegin(OSU_TRIANGLE);
  osuColor3f(0.0, 0.0, 1.0);
	osuVertex2f(a,a);
	osuVertex2f(c,a);
	osuVertex2f (b,b);
  osuEnd();

}



/******************************************************************************
Draw some triangles that use color interpolation.
******************************************************************************/

void color_polygons()
{
  double x0 = 0.6;
  double y0 = 0.6;
  double x1 = 0.9;
  double y1 = 0.9;
  double x2,y2;

  /* colorful triangle */

  osuBegin (OSU_TRIANGLE);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (0.1, 0.1);
	osuColor3f (0.0, 1.0, 0.0);
	osuVertex2f (0.1, 0.5);
	osuColor3f (0.0, 0.0, 1.0);
	osuVertex2f (0.5, 0.3);
  osuEnd ();

  /* colors for square */
  osuBegin (OSU_TRIANGLE);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x0, y0);
	osuColor3f (0.0, 1.0, 0.0);
	osuVertex2f (x0, y1);
	osuColor3f (0.0, 0.0, 1.0);
	osuVertex2f (x1, y1);
  osuEnd();

  osuBegin(OSU_TRIANGLE);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x0, y0);
	osuColor3f(0.0, 0.0, 1.0);
	osuVertex2f(x1,y1);
	osuColor3f(1.0, 1.0, 1.0);
	osuVertex2f(x1, y0);
  osuEnd ();


  x0 = 0.55;
  y0 = 0.15;
  x1 = 0.7;
  y1 = 0.3;
  x2 = 0.85;
  y2 = 0.45;

  osuBegin (OSU_TRIANGLE);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x0, y1);
	osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x1, y0);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x2, y1);
  osuEnd();

  osuBegin(OSU_TRIANGLE);
	osuColor3f(1.0, 0.0, 0.0);
	osuVertex2f(x0,y1);
	osuColor3f(1.0, 0.0, 0.0);
	osuVertex2f(x2,y1);
	osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x1, y2);
  osuEnd ();

  x0 = 0.15;
  y0 = 0.55;
  x1 = 0.3;
  y1 = 0.7;
  x2 = 0.45;
  y2 = 0.85;

  osuBegin (OSU_TRIANGLE);
	osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x0, y1);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x1, y0);
	osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x2, y1);
  osuEnd();

  osuBegin(OSU_TRIANGLE);
  osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x0, y1);
	osuColor3f (1.0, 1.0, 1.0);
	osuVertex2f (x2, y1);
	osuColor3f (1.0, 0.0, 0.0);
	osuVertex2f (x1, y2);
  osuEnd ();

}

/******************************************************************************
Draw a polygonal approximation to a circle, but draw it with pie wedges
(thin triangles radiating from the center).

Entry:
  xc,yc  - circle center
  radius - radius of circle
  steps  - number of vertices in approximation
******************************************************************************/

void wedges (double xc, double yc, double radius, int steps)
{
  int i;
  double theta;
  double x,y;
  double xold,yold;

  theta = 2 * 3.1415926535 * (0.5) / (double) steps;
  xold = xc + radius * cos(theta);
  yold = yc + radius * sin(theta);

  for (i = 1; i <= steps; i++) {

    theta = 2 * 3.1415926535 * (i + 0.5) / (double) steps;
    x = xc + radius * cos(theta);
    y = yc + radius * sin(theta);

    osuBegin (OSU_TRIANGLE);
    osuVertex2f (xc, yc);
    osuVertex2f (x, y);
    osuVertex2f (xold, yold);
    osuEnd ();

    xold = x;
    yold = y;
  }
}


void overlapTest()
{
	  osuSetWriteMode(OSU_XOR);
	osuBegin(OSU_TRIANGLE);
	  osuColor3f(1.0,1.0,1.0);
	  osuVertex2f(0.1, 0.1);
	  osuVertex2f(0.6,0.5);
	  osuVertex2f(0.1,0.9);
	osuEnd();



	osuBegin(OSU_TRIANGLE);
		osuVertex2f(0.9, 0.1);
		osuVertex2f(0.4,0.5);
	    osuVertex2f(0.9, 0.9);
	osuEnd();
}
