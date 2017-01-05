#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <gl\gl.h>
#include <gl\glu.h>	
#include "glut.h"
#include "osuGraphics.h"
#include "ObjLoader.h"

GLfloat image_ambient[500][500][3];
GLfloat camera_zbuffer[500][500];
GLfloat light_zbuffer[500][500];
GLfloat outline_silhouettes[500][500][3];
GLfloat render_buffer_I1[500][500][3];
GLfloat render_buffer_I2[500][500][3];
GLfloat real_coordinate[500][500][5];
GLfloat light_coordinate[500][500][5];
GLfloat testbuffer[500][500];

//function get the max value out of 9
float get_max(float a[9])
{
	float result = a[0];
	for (int i = 1; i < 9; i++)
	{
		if (a[i] >= result)
		{
			result = a[i];
		}
	}
	return result;
}

//function get the min value out of 9
float get_min(float a[9])
{
	float result = a[0];
	for (int i = 1; i < 9; i++)
	{
		if (a[i] <= result)
		{
			result = a[i];
		}
	}
	return result;
}

//function from assignment#3 load and draw objects
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

	for (int i = 0; i < data.NumTriangle; i++)  {

		glBegin(GL_POLYGON);


		glNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[0]].X,
			data.NormalArray[data.TriangleArray[i].Vertex[0]].Y,
			data.NormalArray[data.TriangleArray[i].Vertex[0]].Z);
		glVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[0]].X,
			data.VertexArray[data.TriangleArray[i].Vertex[0]].Y,
			data.VertexArray[data.TriangleArray[i].Vertex[0]].Z);


		glNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[1]].X,
			data.NormalArray[data.TriangleArray[i].Vertex[1]].Y,
			data.NormalArray[data.TriangleArray[i].Vertex[1]].Z);
		glVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[1]].X,
			data.VertexArray[data.TriangleArray[i].Vertex[1]].Y,
			data.VertexArray[data.TriangleArray[i].Vertex[1]].Z);

		glNormal3f(data.NormalArray[data.TriangleArray[i].Vertex[2]].X,
			data.NormalArray[data.TriangleArray[i].Vertex[2]].Y,
			data.NormalArray[data.TriangleArray[i].Vertex[2]].Z);
		glVertex3f(data.VertexArray[data.TriangleArray[i].Vertex[2]].X,
			data.VertexArray[data.TriangleArray[i].Vertex[2]].Y,
			data.VertexArray[data.TriangleArray[i].Vertex[2]].Z);
		glEnd();

	}
}

void draw_3D_scene()
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glPushMatrix();
	glutSolidTeapot(0.8);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, -1.58, 0.0);
	glutSolidCube(2.0);
	glPopMatrix();

	glPopMatrix();
	glFlush();
}

void three_velues_set_image()
{
	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			for (int q = 0; q < 3; q++)
			{
				if (image_ambient[i][j][q] < 0.33)
				{
					image_ambient[i][j][q] = image_ambient[i][j][q] / 1.5;
					//image_ambient[i][j][q] = 0.165;
				}
				else if ((image_ambient[i][j][q] >= 0.34) && (image_ambient[i][j][q] < 0.66))
				{
					image_ambient[i][j][q] = (image_ambient[i][j][q] - 0.33) / 2 + 0.33;
					//image_ambient[i][j][q] = 0.5;
				}
				else if (image_ambient[i][j][q] >= 0.67)
				{
					image_ambient[i][j][q] = (image_ambient[i][j][q] - 0.66) / 2 + 0.66;
					//image_ambient[i][j][q] = 0.835;
				}

				if (outline_silhouettes[i][j][q] == 0)
				{
					image_ambient[i][j][q] = 0;
				}
			}
		}
	}
	glDrawPixels(500, 500, GL_RGB, GL_FLOAT, &image_ambient);
	glFlush();
}

void ambient_only_init(void)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position[] = { 5.0, 5.0, 2.0, 0.0 };
	GLfloat Ambient_light[] = { 1.0, 0.0, 0.0, 2.0 };
	GLfloat Diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 };

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambient_light);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
}

void get_ambient_scene()
{
	glReadBuffer(GL_FLOAT);
	glReadPixels(0, 0, 500, 500, GL_RGB, GL_FLOAT, &image_ambient);
}

void get_camera_zbuffer()
{
	glReadBuffer(GL_FLOAT);
	glReadPixels(0, 0, 500, 500, GL_DEPTH_COMPONENT, GL_FLOAT, &camera_zbuffer);
}

void get_light_zbuffer()
{
	glReadBuffer(GL_FLOAT);
	glReadPixels(0, 0, 500, 500, GL_DEPTH_COMPONENT, GL_FLOAT, &light_zbuffer);
}

void get_outline_silhouettes()
{
	float g[500][500];
	float q;
	float pass[9];
	float g_max, g_min;
	float kp = 0.0008;
	float edge_limit = 0.7;

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			g[i][j] = 0;
			outline_silhouettes[i][j][0] = 1.0;
			outline_silhouettes[i][j][1] = 1.0;
			outline_silhouettes[i][j][2] = 1.0;
		}
	}
	for (int i = 1; i < 499; i++)
	{
		for (int j = 1; j < 499; j++)
		{
			g[i][j] = (fabs(camera_zbuffer[i - 1][j + 1] - camera_zbuffer[i][j]) +
					   2 * fabs(camera_zbuffer[i][j + 1] - camera_zbuffer[i][j]) +
					   fabs(camera_zbuffer[i + 1][j + 1] - camera_zbuffer[i][j]) +
					   2 * fabs(camera_zbuffer[i - 1][j] - camera_zbuffer[i][j]) +
					   2 * fabs(camera_zbuffer[i + 1][j] - camera_zbuffer[i][j]) +
					   fabs(camera_zbuffer[i - 1][j - 1] - camera_zbuffer[i][j]) +
					   2 * fabs(camera_zbuffer[i][j - 1] - camera_zbuffer[i][j]) +
					   fabs(camera_zbuffer[i + 1][j - 1] - camera_zbuffer[i][j])) / 8;
		}
	}
	for (int i = 1; i < 499; i++)
	{
		for (int j = 1; j < 499; j++)
		{
			pass[0] = g[i - 1][j + 1];
			pass[1] = g[i][j + 1];
			pass[2] = g[i + 1][j + 1];
			pass[3] = g[i - 1][j];
			pass[4] = g[i][j];
			pass[5] = g[i + 1][j];
			pass[6] = g[i - 1][j - 1];
			pass[7] = g[i][j - 1];
			pass[8] = g[i + 1][j - 1];

			g_max = get_max(pass);
			g_min = get_min(pass);

			q = min(pow(((g_max - g_min) / kp), 2), 1);

			if (q >= edge_limit)
			{
				outline_silhouettes[i][j][0] = 0;
				outline_silhouettes[i][j][1] = 0;
				outline_silhouettes[i][j][2] = 0;
			}
		}
	}
}

void set_normal_I1()
{
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position_I1_R1[] = { -10.0, 0.0, 0.0, 0.0 };
	GLfloat light_position_I1_G2[] = { 0.0, -10.0, 0.0, 0.0 };
	GLfloat light_position_I1_B3[] = { 0.0, 0.0, -10.0, 0.0 };

	GLfloat Ambient_light[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat Diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat Red_light[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat Green_light[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat Blue_light[] = { 0.0, 0.0, 1.0, 1.0 };

	//For I1
	glDisable(GL_LIGHTING);
	glClearColor(0.6, 0.8, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambient_light);

	glLightfv(GL_LIGHT1, GL_POSITION, light_position_I1_R1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, Red_light);
	glLightfv(GL_LIGHT2, GL_POSITION, light_position_I1_G2);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, Green_light);
	glLightfv(GL_LIGHT3, GL_POSITION, light_position_I1_B3);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, Blue_light);

	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	glDisable(GL_LIGHT5);
	glDisable(GL_LIGHT6);
	glEnable(GL_DEPTH_TEST);
}

void set_normal_I2()
{
	GLfloat mat_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	GLfloat light_position_I2_R1[] = { 10.0, 0.0, 0.0, 0.0 };
	GLfloat light_position_I2_G2[] = { 0.0, 10.0, 0.0, 0.0 };
	GLfloat light_position_I2_B3[] = { 0.0, 0.0, 10.0, 0.0 };
	GLfloat Ambient_light[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat Diffuse_light[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat Red_light[] = { 1.0, 0.0, 0.0, 1.0 };
	GLfloat Green_light[] = { 0.0, 1.0, 0.0, 1.0 };
	GLfloat Blue_light[] = { 0.0, 0.0, 1.0, 1.0 };

	glDisable(GL_LIGHTING);
	glClearColor(0.6, 0.8, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	//glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, Diffuse_light);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ambient_light);

	glLightfv(GL_LIGHT4, GL_POSITION, light_position_I2_R1);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, Red_light);
	glLightfv(GL_LIGHT5, GL_POSITION, light_position_I2_G2);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, Green_light);
	glLightfv(GL_LIGHT6, GL_POSITION, light_position_I2_B3);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, Blue_light);

	glEnable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glEnable(GL_LIGHT4);
	glEnable(GL_LIGHT5);
	glEnable(GL_LIGHT6);
	glEnable(GL_DEPTH_TEST);
}

void get_real_coordinate()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble posX, posY, posZ;

	glLoadIdentity();
	glPushMatrix();
	gluLookAt(3.5, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glPopMatrix();

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			gluUnProject(i, (int)(viewport[3] - (float)j), camera_zbuffer[i][j], modelview, projection, viewport, &posX, &posY, &posZ);
			real_coordinate[i][j][0] = posX;
			real_coordinate[i][j][1] = posY;
			real_coordinate[i][j][2] = posZ;
			real_coordinate[i][j][3] = i;
			real_coordinate[i][j][4] = j;
		}
	}
}

void get_light_coordinate()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	GLdouble winX, winY, winZ;
	
	glLoadIdentity();
	glPushMatrix();
	gluLookAt(5.0, 3.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glPopMatrix();

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			gluProject(real_coordinate[i][j][0], real_coordinate[i][j][1], real_coordinate[i][j][2], modelview, projection, viewport, &winX, &winY, &winZ);
			light_coordinate[i][j][0] = winX;
			light_coordinate[i][j][1] = winY;
			light_coordinate[i][j][2] = winZ;
			light_coordinate[i][j][3] = real_coordinate[i][j][3];
			light_coordinate[i][j][4] = real_coordinate[i][j][4];
		}
	}
}

void draw_projected_shadow()
{
	GLdouble real_coordinate_z;
	GLfloat buffer_z;
	int light_x;
	int light_y;
	int camera_x;
	int camera_y;

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			light_x = light_coordinate[i][j][0];
			light_y = light_coordinate[i][j][1];
			if (light_x <0 || light_x > 500 || light_y < 0 || light_y > 500)
			{
				light_x = 0;
				light_y = 0;
			}
			//real_coordinate_z = real_coordinate[i][j][2];
			buffer_z = light_zbuffer[light_x][light_y];
			//testbuffer[i][j] = light_coordinate[i][j][2];
			if ((buffer_z != 1) && (buffer_z > light_coordinate[i][j][2]))
			{
				camera_x = light_coordinate[i][j][3];
				camera_y = light_coordinate[i][j][4];
				for (int q = 0; q < 3; q++)
				{
					//image_ambient[light_x][light_y][q] = image_ambient[light_x][light_y][q] / 2;
					image_ambient[camera_x][camera_y][q] = image_ambient[camera_x][camera_y][q] / 2;
				}
			}
		}
	}
}

void do_backface_shadow()
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble viewVector[3];
	GLdouble projection[16];

	GLdouble winX, winY, winZ;

	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

	viewVector[0] = modelview[8];
	viewVector[1] = modelview[9];
	viewVector[2] = modelview[10];

	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 0; i < 500; i++)
	{
		for (int j = 0; j < 500; j++)
		{
			//int res = gluProject(real_coordinate[i][j][0], real_coordinate[i][j][1], real_coordinate[i][j][2], modelview, projection, viewport, &winX, &winY, &winZ);

			if ((viewVector[0] * (real_coordinate[i][j][0]) + viewVector[1] * (real_coordinate[i][j][1]) + viewVector[2] * (real_coordinate[i][j][2]) < 0))
			{
				int camera_x = (int)real_coordinate[i][j][3];
				int camera_y = (int)real_coordinate[i][j][4];
				for (int q = 0; q < 3; q++)
				{
					image_ambient[camera_x][camera_y][q] = image_ambient[camera_x][camera_y][q] / 2;
				}
			}
		}
	}
}

void display(void)
{
	glClearColor(0.6, 0.8, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(3.5, 3.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	//draw 3d scene
	draw_3D_scene();

	//ambient-only; zbuffer; silhouettes
	get_ambient_scene();
	get_camera_zbuffer();
	get_outline_silhouettes();
	
	//get camera position's coordinates
	get_real_coordinate();

	
	//normal buffer I1
	set_normal_I1();
	draw_3D_scene();
	glReadBuffer(GL_FLOAT);
	glReadPixels(0, 0, 500, 500, GL_RGB, GL_FLOAT, &render_buffer_I1);

	//normal buffer I2
	set_normal_I2();
	draw_3D_scene();
	glReadBuffer(GL_FLOAT);
	glReadPixels(0, 0, 500, 500, GL_RGB, GL_FLOAT, &render_buffer_I2);
	
	
	//draw projected shadow;
	//set camera to light position
	glClearColor(0.6, 0.8, 0.4, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(5.0, 3.0, 2.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	GLfloat light_position[] = { 5.0, 3.0, 2.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	draw_3D_scene();

	get_light_zbuffer();
	get_light_coordinate();
	draw_projected_shadow();
	//do_backface_shadow();

	//test part
	//glDrawPixels(500, 500, GL_RGB, GL_FLOAT, &render_buffer_I2);
	//glDrawPixels(500, 500, GL_RGB, GL_FLOAT, &outline_silhouettes);
	//glDrawPixels(500, 500, GL_LUMINANCE, GL_FLOAT, &camera_zbuffer);
	//glFlush();

	three_velues_set_image();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-1.5, 1.5, -1.5*(GLfloat)h / (GLfloat)w,
		1.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-1.5*(GLfloat)w / (GLfloat)h,
		1.5*(GLfloat)w / (GLfloat)h, -1.5, 1.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);
	ambient_only_init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}