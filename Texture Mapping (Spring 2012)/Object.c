#include "Object.h"
#include <string>

#include <GL/glew.h>
#include <GL/glut.h>
#include <cmath>

int width; 	int height;
float ymax;

//function caluculating the length of a vector
double magnitude (Vect p)
{
	double total = p.x* p.x + p.y*p.y + p.z*p.z ;
	total = sqrt(total);
	return total;
}

//calulate unit vector...remeber to unitize
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

//calulate corss product of two vectors...rember to unitize....
Vect crossproduct (Vect v1, Vect v2 )
{
	Vect cros;
	cros.x = v1.y * v2.z - v2.y * v1.z;
	cros.y = v1.z * v2.x - v2.z * v1.x;
	cros.z = v1.x * v2.y - v2.x * v1.y;
	return cros;
}

//for counting lines in file
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


GLuint loadTexture(char * name)
{
	GLuint gtexture;

	unsigned char * image = readPPM (name, &width, &height);
	
	glGenTextures(1,&gtexture);  //for name id of tecture..
	glBindTexture (GL_TEXTURE_2D, gtexture);  //remeber to call bind....
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //setup for combined 
	
	//Mipmap parameter GL_LINEAR_MIPMAP_NEAREST for minificaton...
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //repeat the texture pattern 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // not clamp !!!
	
	//upload this texture to memory
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	// texture mipmap,render to display image
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	
	glBindTexture( GL_TEXTURE_2D,0);
	//printf("&&&&&&&&&&&&&&&\n");
	return gtexture;
}

GLuint loadSpheremap ( char * name)
{
	GLuint stexture = loadTexture(name);
	//printf ("load shpheremap here~ \n");
	return stexture;
}

// ================================== Public Functions ========================================
// yeah I wrote an object loader for triangle meshes....
//read the teapot.obj file
void Object :: loadObject(char*name)
{
	vc = 0, fc = 0 , fnc = 0  ,  vnc = 0; 
	ymax = 0.5;
	FILE * readFile;
	readFile = fopen(name, "r");
	int lines = count_lines(readFile);
	int cl = 0;
	for (cl = 0 ; cl < lines; cl++)
	{
		char t1[2];
		fscanf(readFile, "%s", t1);
		if ( strcmp (t1 ,"v") == 0 )
		{
			Point p; float f2  ;
			fscanf(readFile, "%E", &f2);
			p.x = f2;
			float f3 ;
			fscanf(readFile, "%E", &f3);
			p.y = f3;
			float f4;
			fscanf(readFile, "%E", &f4);
			p.z = f4;
			//if(p.y>ymax)
			//ymax = p.y;
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
	texture = 0;
//--------------------------Compute and Store Normals ----------------------------
	int i = 0;
	for (i =  0; i < vc; i++)
	{
		vnArray[i].x = 0; vnArray[i].y = 0; vnArray[i].z = 0;
		//tArray[i].x = 0; tArray[i].y =0;
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
	/*for ( i = 0; i < vc; i++)
	{
		theta = atan2f(vArray[i].z , vArray[i].x);
		tArray[i].x = (theta+PI)/(2*PI);
		tArray[i].y = vArray[i].y/ymax;
	}*/
	//printf("Yeah~end of loadfile\n");
}

void Object :: load_Texture(char * name)
{
	texture = loadTexture(name);
}

void Object :: load_Environment(char * name)
{
	environment = loadSpheremap(name);
}

void Object :: load_Combination (char * name)
{
	combination = loadTexture(name);
}


void Object :: draw (int shader_id)
{
	//printf("....................\n");
	glUseProgram(shader_id);
	
	/* queries the previously linked program object specified by program for the attribute
	 variable specified by name and returns the index of the generic vertex attribute 
	 that is bound to that attribute variable.*/
	int texCoordLoc = glGetAttribLocation(shader_id,"tCoordinate");
	
	//returns an integer that represents the location of a specific uniform variable within a program object.
    int texturePos = glGetUniformLocation(shader_id,"texture");
    int environmentPos = glGetUniformLocation(shader_id,"environment");
    int combinationPos = glGetUniformLocation(shader_id,"combination");
    
    //Specifies which texture unit to make active
    glActiveTexture(GL_TEXTURE0 ); // nameid starts at 0
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glActiveTexture(GL_TEXTURE0+1); //assign new nameid...
    glBindTexture(GL_TEXTURE_2D, environment);
    
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP); //GL_SPHERE here directly for environment map...
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
    glEnable(GL_TEXTURE_GEN_S); // needs to be enable here for environemnt map
    glEnable(GL_TEXTURE_GEN_T);
    
    glActiveTexture(GL_TEXTURE0 + 2); // assign new nameid...
    glBindTexture(GL_TEXTURE_2D, combination);
	
	//Specify the value of a uniform variable for the current program object.
    glUniform1i(environmentPos, 1);
    glUniform1i(texturePos, 0);
    glUniform1i(combinationPos, 2);
    
    // for the actual redering part in diplay func !!!!!
    // drawing traigles the order specified in face Array...do loops....
    for (int i = 0; i < fc; i++) 
    {
    	glBegin(GL_TRIANGLES);
    	
		int a = fArray[i].a; 
		int b = fArray[i].b;
		int c = fArray[i].c;
		
		float aNx = vnArray[a].x;  float aNy = vnArray[a].y;  float aNz = vnArray[a].z;
		float bNx = vnArray[b].x;  float bNy = vnArray[b].y;  float bNz = vnArray[b].z;
		float cNx = vnArray[c].x;  float cNy = vnArray[c].y;  float cNz = vnArray[c].z;
		
		// Calulating cylindral coordinate for texture coordinate....
		//theta = atan2(z,x). (s,t) as s = (theta+PI)/2PI and t = y/ymax 
		float thetaa = atan2(vArray[a].z , vArray[a].x);
		float thetab = atan2(vArray[b].z , vArray[b].x);
		float thetac = atan2(vArray[c].z , vArray[c].x);
		
		float PI = 3.1415926535897932354626;
		float pa = (thetaa + PI)/(2 * PI);  if (pa > 0.5) pa = 1 - pa;
		float pb = (thetab + PI)/(2 * PI);  if (pb > 0.5) pb = 1 - pb;
		float pc = (thetac + PI)/(2 * PI);  if (pc > 0.5) pc = 1 - pc;
		
		glNormal3f(aNx,aNy,aNz);
		glVertexAttrib2f(texCoordLoc, 15 * pa, (vArray[a].y)/ymax);
		glVertex3f(fArray[i].v1.x, fArray[i].v1.y, fArray[i].v1.z);
		
		glNormal3f(bNx,bNy,bNz);
		glVertexAttrib2f(texCoordLoc, 15 * pb,(vArray[b].y)/ymax);
		glVertex3f(fArray[i].v2.x, fArray[i].v2.y, fArray[i].v2.z);
		
		glNormal3f(cNx,cNy,cNz);
		glVertexAttrib2f(texCoordLoc, 15 * pc, (vArray[c].y) /ymax);
		glVertex3f(fArray[i].v3.x, fArray[i].v3.y, fArray[i].v3.z);
		
		//printf("%f\n",pa);
		//printf("%f\n",pb);
		//printf("%f\n",pc);	
    	glEnd();
    }
}
