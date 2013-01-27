#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>

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
#include <cmath>
#include "readppm.h"

// Point(x,y,z)
typedef struct _p
{
	float x;
	float y;
	float z;
}Point;

// Face (Point1, Point2, Point3)
typedef struct _f
{
	Point  v1;
	Point  v2;
	Point  v3;

	int a;
	int b;
	int c;
}Face; 

//calculated.... Vector (x, y, z)
typedef struct _v
{
	float x;
	float y;
	float z;
}Vect;

// Final Object Struct !!! store for Arrays and all data info...
typedef struct _o
{
	int vc, fc , fnc  ,  vnc ;
	
	Point vArray[2500] ; // Array of vertices
	Face  fArray[2500] ; // Array of faces
	Vect fnArray[2500] ; // Array of facenormals :( not much use
	Vect vnArray[2500] ; // Array of vertex normals
	
	GLuint environment; // for only environment mapping
	GLuint texture;		// for only texture mapping
    GLuint combination;	// for both...
    
	//===functions.....
	void loadObject(char *name);	//parsing fuction to read .obj, and store data in 4 arrays
	void load_Texture( char * name); 		// func for texturemapping
	void load_Environment (char * name);  // func for sphere mapping
	void load_Combination (char * name);  // func for both two combined in one scene
	void draw(int shaders); 			// for redering, in display() func of main...
}Object;

#endif

