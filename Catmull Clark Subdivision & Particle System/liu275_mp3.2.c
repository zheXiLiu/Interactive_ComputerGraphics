#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "math.h"
#include "readppm.h"

#define PI 3.1415926535897932354626535897932

GLfloat angle = 0, angle2 = 0, angle3 = 0; 
float tt = 0, tt2 = 0 , tt3 = 0; 
int flag = 1;
int f = 0;
float ymax = 0;
float theta;

typedef struct _p
{
	float x;
	float y;
	float z;
}Point;

typedef struct _f
{
	Point  v1;
	Point  v2;
	Point  v3;
	
	int a;
	int b;
	int c;
}Face; 

typedef struct _v
{
	float x;
	float y;
	float z;
}Vect;

Point eye,lookAt, up, dis;

Point vArray[2000] ;
Face  fArray[2000] ;
Vect fnArray[2000] ;
Vect vnArray[2000] ;
Vect tArray[2000];

int vc = 0 , fc = 0 , fnc = 0 ,  vnc = 0;
static int lighton = 1;

int width1, width2, width3; 
int height1,height2,height3;

unsigned char * image1,*image2, *image3;
GLuint texture1 , texture2;

//====================GLOBAL DATA OBAVE========================
double magnitude (Vect p)
{
	double total = p.x* p.x + p.y*p.y + p.z*p.z ;
	total = sqrt(total);
	return total;
}

Vect unitize (Vect p)
{
	double m = magnitude(p);
	p.x = p.x / m;
	p.y = p.y / m;
	p.z = p.z / m;
	return p;
}

//distance between two points
Vect disInPoints (Point p1 , Point p2) 
{
	Vect dis;
	dis.x = p2.x - p1.x;
	dis.y = p2.y - p1.y;
	dis.z = p2.z - p1.z;
	return dis;
}

Vect crossproduct (Vect v1, Vect v2 )
{
	Vect cros;
	cros.x = v1.y * v2.z - v2.y * v1.z;
	cros.y = v1.z * v2.x - v2.z * v1.x;
	cros.z = v1.x * v2.y - v2.x * v1.y;
	return cros;
}

int count_lines(FILE * f)
{
	int c = 0 ;
	char cc;
	do
	{
		cc = fgetc(f);
		if (cc == '\n')
			c++;
	}while (cc!=EOF);
	rewind(f);
	return c;
}

void loadTeapot()
{
	FILE * readFile;
	readFile = fopen("teapot.obj", "r");
	//===================================PARSING===============================
	int lines = count_lines(readFile);
	int cl = 0;
	for (cl = 0 ; cl < lines; cl++)
	{
		char t1[2];
		fscanf(readFile, "%s", t1);
		if ( strcmp (t1 ,"v") == 0 )
		{
			Point p;
			float f2  ;
			fscanf(readFile, "%E", &f2);
			p.x = f2;
			float f3 ;
			fscanf(readFile, "%E", &f3);
			p.y = f3;
			float f4 ;
			
			fscanf(readFile, "%E", &f4);
			p.z = f4;
			
			if(p.y>ymax)
				ymax = p.y;
			
			vArray[vc] = p;
			vc++;
		}
		if(strcmp (t1 ,"f") == 0)
		{	
			Face f;
			int f2  ;
			fscanf(readFile, "%d", &f2);
			f.v1 = vArray[f2-1]; 
			f.a = f2-1;
			int f3 ;
			fscanf(readFile, "%d", &f3);
			f.v2 = vArray[f3-1];
			f.b = f3-1;
			int f4 ;
			fscanf(readFile, "%d", &f4);
			f.v3 = vArray[f4-1];
			f.c = f4-1;
			fArray[fc] = f;
			fc++;
		}
		
		else if (strcmp(t1, "#") == 0)
		{
			fscanf(readFile, "%s", t1);
			fscanf(readFile, "%s", t1);
		}
		else if (strcmp(t1, "g") == 0)
		{
			fscanf(readFile, "%s",t1);
		}
	}
	
	rewind(readFile);
	
	//======================================================================
	int i = 0;
	
	for (i =  0; i < vc; i++)
	{
		vnArray[i].x = 0; vnArray[i].y = 0; vnArray[i].z = 0;
		tArray[i].x = 0; tArray[i].y =0;
	}
	for (i = 0 ; i < fc; i++)
	{
		Vect vec1 = disInPoints(fArray[i].v1, fArray[i].v2);
		Vect vec2 = disInPoints(fArray[i].v1, fArray[i].v3);
		Vect fNm = crossproduct(vec1, vec2);
		fnArray[fc] = fNm;
		
		int a = fArray[i].a; 
		int b = fArray[i].b;
		int c = fArray[i].c;
		vnArray[a].x += fNm.x; vnArray[a].y += fNm.y; vnArray[a].z += fNm.z;
		vnArray[b].x += fNm.x; vnArray[b].y += fNm.y; vnArray[b].z += fNm.z;
		vnArray[c].x += fNm.x; vnArray[c].y += fNm.y; vnArray[c].z += fNm.z;
	}
	//---------------------------------------------------------------------
	for (i = 0; i< vc; i++)
	{
		vnArray[i]  = unitize(vnArray[i]);
		//printf("%f, %f, %f \n"  ,vnArray[i].x,vnArray[i].y,vnArray[i].z);
	}
	
	for ( i = 0; i < vc; i++)
	{
		
		theta = atan2f(vArray[i].z , vArray[i].x);
		tArray[i].x = (theta+PI)/(2*PI);
		tArray[i].y = vArray[i].y/ymax;
	}
}

//setup lighting
void setLight()
{
	GLfloat white[] = {1.,1.,1.,0.0};
	GLfloat lpos[] = {1.0,1.0,1.0,0.0};
	//GLfloat lpos[] = {1.0,1.0,1.0,1.0};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
}

//readin images for mapping
void loadImage()
{
	width1 = 512;
	height1 = 512;
	
	width2 = 512;
	height2 = 512;
	
	//image1 = readPPM("texture_4.ppm", &width1, &height1);
	image2 = readPPM("marble.ppm", &width1, &height1);
}

//setup texture
void loadTexture()
{
	//allocate texture name
	//glGenTextures(1,&texture1);
	glGenTextures(1,&texture2);
		
	//select current texture
	glBindTexture (GL_TEXTURE_2D, texture2);
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_NEAREST);
	
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); 
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	
	//upload this texture to memory
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
	
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	
	//build texture mipmap,render to display image
	glBindTexture(GL_TEXTURE_2D, texture2);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width1, height1, GL_RGB, GL_UNSIGNED_BYTE, image2);
}

void init(void) 
{
	glEnable(GL_TEXTURE_2D);
	loadImage();
	loadTeapot();
	loadTexture();
	setLight();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	
	
	GLfloat ks[] = {1.1,1.,1.0,1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
	glMaterialf(GL_FRONT,GL_SHININESS,6.0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	/* glShadeModel (GL_FLAT); */
	//glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
    
	//------------Add initialization -----------
	//angle = 0;
	//tt = 1.5;
	
	printf("YMAX: %f \n" , ymax);
}

void display(void)
{
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();             /* clear the matrix */
	// light......
	gluLookAt(0.f,4.5f,6.f,0.f,1.f,0.f,0.0f,1.f,0.f);	
	
		//glDisable(GL_TEXTURE_GEN_S);
	//glDisable(GL_TEXTURE_GEN_T);
	
	//glEnable(GL_TEXTURE_GEN_S);
	//glEnable(GL_TEXTURE_GEN_T);
	
	//glEnable(GL_TEXTURE_GEN_R);
	//glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	
	glRotated(-angle, 1, 0, 0);
	glRotated(-angle2, 0, 1, 0);
	glRotated(-angle3, 0, 0, 1);
	
	int i = 0;
	
	GLfloat block [] = {.8,.8,0,1};
	
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glBegin(GL_TRIANGLES);
	for(i = 0 ; i< fc; i++)
	{
		int a = fArray[i].a; 
		int b = fArray[i].b;
		int c = fArray[i].c;
		
		float aNx = vnArray[a].x;  float aNy = vnArray[a].y;  float aNz = vnArray[a].z;
		float bNx = vnArray[b].x;  float bNy = vnArray[b].y;  float bNz = vnArray[b].z;
		float cNx = vnArray[c].x;  float cNy = vnArray[c].y;  float cNz = vnArray[c].z;
		
		glTexCoord2f(aNx, aNy);
		glNormal3f(aNx,aNy,aNz);
		glVertex3f(fArray[i].v1.x, fArray[i].v1.y, fArray[i].v1.z);
		
		glTexCoord2f(bNx, bNy);
		glNormal3f(bNx,bNy,bNz);
		glVertex3f(fArray[i].v2.x, fArray[i].v2.y, fArray[i].v2.z);
		
		glTexCoord2f(cNx, cNy);
		glNormal3f(cNx,cNy,cNz);
		glVertex3f(fArray[i].v3.x, fArray[i].v3.y, fArray[i].v3.z);
		
	}
	glEnd();
	
	
	
	
	glutSwapBuffers();
	glFlush ();
	glDisable(GL_TEXTURE_2D);
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, (GLsizei) w, (GLsizei) h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	gluPerspective(90.0,1.0,0.01,10.0);
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
			exit(0);
			break;
			
		case 'l':
			if (lighton == 0)
			{
				glEnable(GL_LIGHTING);
				lighton = 1;
			}
			else 
			{
				glDisable(GL_LIGHTING);
				lighton = 0;
			}
			break;
			
		case 'r':
			if(tt != 0)
				tt = 0;
			else
				tt = 1.5;
			break;
			
		case 'w':
			if(tt2 != 0)
				tt2 = 0;
			else
				tt2 = 1.5;
			break;
			
		case 'e':
			if(tt3 != 0) 
				tt3 = 0;
			else	
				tt3 = 1.5;
			break;
			
		case 'p':
			if (f==1)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				f = 0;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				f = 1;
			}
			break;
	}
}

void SpecialKeys(int key, int x, int y)
{
	//Point pitchaxis = crossproduct(dis , up);
	switch (key)
	{
		case GLUT_KEY_UP:
			break;
		case GLUT_KEY_DOWN:
			break;
		case GLUT_KEY_RIGHT:
			break;
		case GLUT_KEY_LEFT:
			break;
	}
}

void timer (int v)
{
	angle += tt;
	angle2 += tt2;
	angle3 += tt3;
	glutPostRedisplay();
	glutTimerFunc(1000/30, timer, v);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 800); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Teapot_liu275");
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(100,timer,30);
	glutMainLoop();
	return 0;
}
