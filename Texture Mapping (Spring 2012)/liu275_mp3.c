#include <GL/glew.h>
#include <GL/glut.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <cmath>
#include "readppm.h"
#include "Object.h"

GLfloat anglex = 0, angley = 0, anglez = 0; 
float xd = 0, yd = 0 ;
int flag = 1;
int f = 0;
float z;

//for vertex shader & fragment shader & id...
GLuint vShader, fShader, shader_program;

// Difine teapot as an Object
Object teapot;

void setLight()
{
	GLfloat white[] = {1.,1.,1.,0.0};
	GLfloat lpos[] = {-1.0,1.0,0.0,0.0};
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
}

void setBackground()
{
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glOrtho(0,1,0,1,0,1);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,teapot.texture);
	
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDepthMask(GL_FALSE);  // turn off the depth mask here....
	

	glBegin(GL_QUADS);  // draw background quads here
	glTexCoord2f(0,0);
	glVertex2f(1.,1.);

	glTexCoord2f(1.,0);
	glVertex2f(0,1.);

	glTexCoord2f(1.,1.);
	glVertex2f(0,0);

	glTexCoord2f(0,1.);
  	glVertex2f(1.,0);
	glEnd();
	
	glDepthMask(GL_TRUE);//enable the depth mode.

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);		
}

/*Helper function to read the shader file.
  Code referenced from:
  http://www.evl.uic.edu/aej/525/code/ogl3.cpp
*/

char * readShader (const char * shadername)
{
	FILE * f; char * c = NULL; int num = 0;
	f = fopen(shadername, "r");
	if (f != NULL)
	{
		fseek(f, 0, SEEK_END);
		num = ftell (f);
		rewind(f);
		
		if (num > 0)
		{
			c = (char *)malloc (sizeof (char ) *(num+1));
			num = fread (c, sizeof (char),num, f);
			c[num] = '\0';
		}
		fclose(f);
	}
	return c;
}

//Helper function to create,initalize,compile and link shaderprograms.
void shader_HelperFunc(char * shadername)
{
	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);
	
	char *vtx = readShader("helloshader.vert");
	char *frag = readShader (shadername);
	
	const char * vtx2 =vtx;
	const char * frag2 = frag;
	
	glShaderSource(vShader, 1, &vtx2, NULL);
	glShaderSource(fShader, 1, &frag2, NULL);
	
	glCompileShader(vShader);
	glCompileShader(fShader);
	
	shader_program = glCreateProgram();
	glAttachShader (shader_program, vShader);
	glAttachShader (shader_program, fShader);
	glLinkProgram(shader_program);
	
	//debugging.......
	//printShaderInfoLog(vShader);
	//printShaderInfoLog(fShader);
	//printProgramInfoLog(shader_program);
	//printf("....................\n");
}

void init(void) 
{	
	GLenum err = glewInit(); // segfault here ...yyyy
	if (err != GLEW_OK)
 	exit(1);  // handle error in a nicer way :(
	if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
 	exit(1); //or handle the error in a nicer way

	shader_HelperFunc("combination.frag");
	
	/* Call loadObject to read teapot.obj file and store all data into arrays.
	 calculating all the normals are done in this step also,detailed computation 
	 in Object.cpp. */
	teapot.loadObject("teapot.obj");
	
	setLight();
	
	/*Seperately triggers the shaders here, stores texture varibles in
	 Object struct for texture, environment and combined mapping*/
	teapot.load_Environment("probe2.ppm");
	teapot.load_Texture("marble.ppm");
	teapot.load_Combination("marble.ppm");
	
	glClearColor(0,0,0,0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST); // Hiden suferface removel, via z-buffer!!!
    glEnable(GL_COLOR_MATERIAL);
    
    printf("\n\n");
    printf("Press 'e' or 'E' to see environment mapping.\n");
    printf("Press 't' or 'T' to see texture mapping.\n");
    printf("Press 'c' or 'C' to see the combination.\n");
   
    printf("Keep pressing 'RIGHT' to rotate right. (about y-axis)\n");
    printf("Keep pressing 'LEFT'  to rotate left.  (about y-axis)\n");
    printf("Keep pressing 'UP'    to rotate up.    (about x-axis)\n");
    printf("Keep pressing 'DOWN'  to rotate down.  (about x-axis)\n");
    
    printf("Press 'i' or 'I' to zoom in.\n");
    printf("Press 'o' or 'O' to zoom out.\n");
    printf("Press 'p' or 'P' to see model outline.\n");
    printf("\n\n");
    
}



void display(void)
{
	// for hiden suface removel, clear color,depth buffer
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
	
	
	//enable the depth test when displaying.
	glEnable(GL_DEPTH_TEST);
	 
	//clear the matrix here
	
	//setProjectionMatrix(1,1);
	glLoadIdentity ();            
	
	//set up light, eye, lookat	
	gluLookAt(0.f,3.5f,6.f+z,0.f,1.f,0.f+z,0.f,1.f,0.f);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); //incase I want to add anything...
	//glMatrixMode(GL_MODELVIEW);
	
	// define & setup material here
	GLfloat ambient[] = {.1,0.1,0.1,1.0};
	GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, diffuse);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0);
	
	
	glRotatef(anglex+xd,1,0,0);
	glRotatef(angley+yd,0,1,0);
	
	//draw computes the texture coordinates and renders the teapot...
    teapot.draw(shader_program);
	glPopMatrix();	
	
	//glDisable(GL_TEXTURE_2D);
	//glDisable(GL_TEXTURE_GEN_S);
	//glDisable(GL_TEXTURE_GEN_T);	
	//glEnable(GL_TEXTURE_GEN_R);
	//glTexGeni(GL_R,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
	
	glutSwapBuffers();
    glFlush ();
    glutPostRedisplay();	
}

void timer(int v)
{
	anglex += xd;
	angley += yd;
	glutPostRedisplay();
	glutTimerFunc(1000/30,timer,v);
}

/*re-size the viewport whenever OpenGL window is resized.
It is important that the aspect ratio of the projecton
clipping volume is re-computed to match the viewport's
aspect ratio, inorder not to distort the shapes. */
void reshape (int w, int h) 
{
	if (h ==0)  h =1; //prevent divide by 0
	GLfloat aspect = ((float)w)/((GLfloat) h);
		
	glViewport (0, 0,  w, h); 
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	
	float near = 0.1f;
	float far  = 100.f;
	
	//gluPerspective(90.0,aspect,0.01,10.0);   // orginal setting
	gluPerspective(85,aspect,near,far);   
	glMatrixMode (GL_MODELVIEW);
	//printf(".....%f.....\n",aspect);
	
}

void keyboard(unsigned char key, int x, int y)
{glRotatef(anglex+xd,1,0,0);
	switch (key) 
	{
		case 27:
		exit(0);
		break;
		
		case 'e': case 'E':
        	shader_HelperFunc("environment.frag");
            break;
        case 'c': case 'C':
            shader_HelperFunc("combination.frag");
            break;
        case 't': case 'T':
            shader_HelperFunc("texture.frag");
            break;
			
		case 'i': case 'I' :
			z -= .2; 
			break;
		case 'o': case 'O':
			z += .2; 
			break;
		case 'p': case 'P':
			if (flag == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				flag = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				flag = 0;
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
		anglex+= -1.5;
		break;
		
		case GLUT_KEY_DOWN:
		anglex += 1.5;
		break;
		
		case GLUT_KEY_RIGHT:
		angley += 1.5;
		break;
		
		case GLUT_KEY_LEFT:
		angley -= 1.5;
		break;
		 
	}
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 800); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("Teapot_liu275");
	
	init ();
	glutTimerFunc(100,timer,30);
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeys);
	glutMainLoop();
	return 0;
}
