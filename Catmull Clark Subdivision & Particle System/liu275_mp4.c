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

#define PI 3.1415926535897932354626535897932

GLfloat angle = 0, angle2 = 0, angle3 = 0; 
float tt = 0, tt2 = 0 , tt3 = 0; 
int flag = 1;
int f = 0;
float ymax = 0;
float theta;
int level = 2;

int camerachange = 1;

unsigned char *image2;
GLuint texture1 , texture2;

float p0[] = {-5,5,7};
float p1[] = {-5,0,3};
float p2[] = {1,0,5};
float p3[] = {6,-4,0};
float camstep = 0.005;
float t = 0;

float camx = -5;
float camy = 5;
float camz= 7;


typedef struct _v
{
	float x;
	float y;
	float z;
}Vect;

typedef struct _p
{
	float x;
	float y;
	float z;
	
	int atF[20];
	int count;
	
	//int atE[20];
	int countE;
	
	Vect avF;
	Vect avR;
	Vect newp;
}Point;


typedef struct _e
{
	Point ep1;
	Point ep2;
	
	Point mid;
	
	Vect edgeM;
	Vect edgePoint;
}Edge;

typedef struct _Face
{
	Point  v1;
	Point  v2;
	Point  v3;
	Point  v4;
	Point  cen;

	int a;	
	int b;
	int c;
	int d;
	
	Edge e1; //4 edges
	Edge e2;
	Edge e3;
	Edge e4;
	
	int f1;  //nearby faces
	int f2;
	int f3;
	int f4;
	
	Vect facePoint;
	
}Face; 


Point eye,lookAt, up, dis;

Point vA[2000] ;
Face  fA[2000] ;
Point voA[40] ;
Face  foA[40] ;
Face fnA[2000] ;
Point vnA[2000] ;
Point vnnA[2000];
Vect tArray[2000];
Point ovnA[2000];
Edge eA[200][4];

int vc = 0 , fc = 0 , fnc = 0 ,  vnc = 0;
int voc = 0; int foc = 1;
static int lighton = 1;

int width1, width2, width3; 
int height1,height2,height3;


//====================GLOBAL DATA OBAVE========================
void printv(Vect v)
{
	printf("%f, %f, %f\n", v.x, v.y, v.z);
}

void printp(Point v)
{
	printf("v %f %f %f\n", v.x, v.y, v.z);
}

Point Pointt (Vect v)
{
	Point p;
	p.x = v.x; p.y = v.y; p.z = v.z;
	return p;
}

Vect Vecc (Point p)
{
	Vect v;
	v.x = p.x; v.y = p.y; v.z = p.z;
	return v;
}
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

Vect addition2(Point p1, Point p2)
{
	Vect v;
	v.x = p1.x + p2.x;
	v.y = p1.y + p2.y;
	v.z = p1.z + p2.z;
	return v;
}

Vect addv2(Vect p1, Vect p2)
{
	Vect v;
	v.x = p1.x + p2.x;
	v.y = p1.y + p2.y;
	v.z = p1.z + p2.z;
	return v;
}

Vect addition3(Point p1, Point p2, Point p3)
{
	Vect v;
	v.x = p1.x + p2.x + p3.x;
	v.y = p1.y + p2.y + p3.y;
	v.z = p1.z + p2.z + p3.z;
	return v;
}

Vect addition4(Point p1, Point p2, Point p3, Point p4)
{
	Vect v;
	v.x = p1.x + p2.x + p3.x + p4.x;
	v.y = p1.y + p2.y + p3.y + p4.y;
	v.z = p1.z + p2.z + p3.z + p4.z;
	return v;
}

Vect addv4(Vect p1, Vect p2, Vect p3, Vect p4)
{
	Vect v;
	v.x = p1.x + p2.x + p3.x + p4.x;
	v.y = p1.y + p2.y + p3.y + p4.y;
	v.z = p1.z + p2.z + p3.z + p4.z;
	return v;
}

Vect div(Vect v, int n)
{
	v. x = v.x/n; v.y = v.y/n; v.z = v.z/n;
	return v;
}

Vect findEdgeMid (Edge e)
{
	Point e1 = e.ep1;
	Point e2 = e.ep2;
	Vect ret = addition2(e1, e2);
	ret = div(ret, 2);
	return ret;
}

Vect findCentroid (Face & f)
{
	Vect edp1 = f.e1.edgeM; Vect edp3 = f.e3.edgeM;
	Vect edp2 = f.e2.edgeM; Vect edp4 = f.e4.edgeM;
	
	Vect me13 = addv2(edp1, edp3); me13 = div (me13,2);
	Vect me24 = addv2(edp2, edp4); me24 = div (me24,2);
	
	Vect me1234 = addv2(me13,me24); me1234 = div(me1234,2);
	return me1234;
}

Vect findcen (Face f)
{
	Vect ret = addition4(f.v1, f.v2, f.v3,f.v4);
	ret  = div(ret,4);
	return ret;
}

void * findEdgePoint (int f)
{
	
	Face ff = fA[f];  Vect ffp = ff.facePoint;
	
	int f1 = ff.f1; Face ff1 = fA[f1]; 
	Vect ffp1 = ff1.facePoint;
	Vect edp11 = Vecc (ff.e1.ep1); 
	Vect edp12 = Vecc (ff.e1.ep2);
	Vect avg1 = addv4(edp11,edp12, ffp1, ffp);
	avg1 = div (avg1 , 4);
	fA[f].e1.edgePoint = avg1;
	
	int f2 = ff.f2; Face ff2 = fA[f2];
	Vect ffp2 = ff2.facePoint; 
	Vect edp21 = Vecc (ff.e2.ep1); 
	Vect edp22 = Vecc (ff.e2.ep2);
	Vect avg2 = addv4(edp21,edp22, ffp2, ffp);
	avg2 = div (avg2 , 4);
	fA[f].e2.edgePoint = avg2;
	
	int f3 = ff.f3; Face ff3 = fA[f3];
	Vect ffp3 = ff3.facePoint; 
	Vect edp31 = Vecc (ff.e3.ep1); 
	Vect edp32 = Vecc (ff.e3.ep2);
	Vect avg3 = addv4(edp31,edp32, ffp3, ffp);
	avg3 = div (avg3 , 4);
	fA[f].e3.edgePoint = avg3;
	
	int f4 = ff.f4; Face ff4 = fA[f4];
	Vect ffp4 = ff4.facePoint; 
	Vect edp41 = Vecc (ff.e4.ep1); 
	Vect edp42 = Vecc (ff.e4.ep2);
	Vect avg4 = addv4(edp41,edp42, ffp4, ffp);
	avg4 = div (avg4 , 4);
	fA[f].e4.edgePoint = avg4;

}

Vect findAVF(Point p)
{
	int i = 0; 
	Vect ret;  
	ret.x = 0; ret.y = 0; ret.z = 0;
	
	for (i = 0; i < p.count; i++)
	{
		Vect F = fA[ p.atF[i] ].facePoint;
		ret = addv2(ret, F);
	}
	
	ret = div (ret, p.count);
	//printv(ret);
	return ret;
}

int pointEqual(Point p1, Point p2)
{
	return (p1.x == p2.x && p1.y == p2.y && p1.z == p2.z);
}

int haspoint ( Face f , Point p)
{
	return (pointEqual (f.v1, p) ||
			pointEqual (f.v2, p) || 
			pointEqual (f.v3, p) || 
			pointEqual(f.v4, p) );
}

int haspointE ( Edge e , Point p)
{
	return (pointEqual (e.ep1, p) ||
			pointEqual (e.ep2, p) );
}

int formEdge (Edge e, Point p1, Point p2)
{
	Point ep1 = e.ep1;
	Point ep2 = e.ep2;
	
	return ( (pointEqual(p1, ep1) && 
				pointEqual(p2, ep2)) || 
				(pointEqual(p1, ep2) && 
				pointEqual(p2, ep1))  );
	
}

int isAdj (Face f, Point p1, Point p2)
{
	return (formEdge (f.e1,p1, p2) ||
			formEdge (f.e2,p1, p2) || 
			formEdge (f.e3,p1, p2) || 
			formEdge (f.e4,p1, p2)  );
}
		
int findF (Point p1, Point p2 , int noo )
{
	int i = 0;
	int ret;
	for ( i = 0; i< fc; i++)
	{
		Face f = fA[i];
		if (isAdj (f, p1, p2) == 1)
		{
			if (i != noo )
			return i;
		}
	}
	
	return -1;
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


void loadBlockI(char * filename)
{
	FILE * readFile;
	readFile = fopen(filename, "r");
	//==========================PARSING============================
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
			
			
			vA[vc] = p;
			
			vc++;
		}
		
		if(strcmp (t1 ,"f") == 0)
		{	
			Face f;
			int f2  ;
			fscanf(readFile, "%d", &f2);
			f.v1 = vA[f2]; f.a = f2;
			int f3 ;
			fscanf(readFile, "%d", &f3);
			f.v2 = vA[f3];
			f.b = f3;		
			int f4 ;
			fscanf(readFile, "%d", &f4);
			f.v3 = vA[f4];
			f.c = f4;
			int f5 ;
			fscanf(readFile, "%d", &f5);
			f.v4 = vA[f5];
			f.d = f5;	
			
			f.e1.ep1 = f.v1; f.e1.ep2 = f.v2;
			f.e2.ep1 = f.v2; f.e2.ep2 = f.v3;
			f.e3.ep1 = f.v3; f.e3.ep2 = f.v4;
			f.e4.ep1 = f.v4; f.e4.ep2 = f.v1;
			
			f.e1.edgeM = (findEdgeMid(f.e1));
			f.e2.edgeM = (findEdgeMid(f.e2));
			f.e3.edgeM = (findEdgeMid(f.e3));
			f.e4.edgeM = (findEdgeMid(f.e4));
			
			
			fA[fc] = f;	
			
			fc++;
		}
	}		
		
	int i = 0;
	
	for (i =  0; i < vc; i++)
	{
		vnA[i].x = 0; vnA[i].y = 0; vnA[i].z = 0;
		tArray[i].x = 0; tArray[i].y =0;
	}
	for (i = 0 ; i < fc; i++)
	{
		Vect vec1 = disInPoints(fA[i].v1, fA[i].v2);
		Vect vec2 = disInPoints(fA[i].v1, fA[i].v3);
		Vect fNm = crossproduct(vec1, vec2);
		
		int a = fA[i].a; 
		int b = fA[i].b;
		int c = fA[i].c;
		vnA[a].x += fNm.x; vnA[a].y += fNm.y; vnA[a].z += fNm.z;
		vnA[b].x += fNm.x; vnA[b].y += fNm.y; vnA[b].z += fNm.z;
		vnA[c].x += fNm.x; vnA[c].y += fNm.y; vnA[c].z += fNm.z;
	}
	//---------------------------------------------------------------------
	for (i = 0; i< vc; i++)
	{
		Vect v = Vecc (vnA[i]);
		v = unitize(v);
		vnA[i]  = Pointt (v)    ;
	}
	
	rewind(readFile);
	
}

void loadOrigin(char * filename)
{
	FILE * readFile;
	readFile = fopen(filename, "r");
	int lines = count_lines(readFile);
	int cl = 0;
	for (cl = 0 ; cl < lines; cl++)
	{
		char t1[2];
		fscanf(readFile, "%s", t1);
		if ( strcmp (t1 ,"v") == 0 )
		{
			Point p;float f2  ;
			fscanf(readFile, "%E", &f2);
			p.x = f2;float f3 ;
			fscanf(readFile, "%E", &f3);
			p.y = f3;float f4 ;
			fscanf(readFile, "%E", &f4);
			p.z = f4;
			voA[voc] = p;
			voc++;
		}
		
		if(strcmp (t1 ,"f") == 0)
		{	
			Face f;
			int f2  ;
			fscanf(readFile, "%d", &f2);
			f.v1 = voA[f2]; f.a = f2;
			int f3 ;
			fscanf(readFile, "%d", &f3);
			f.v2 = voA[f3];
			f.b = f3;		
			int f4 ;
			fscanf(readFile, "%d", &f4);
			f.v3 = voA[f4];
			f.c = f4;
			int f5 ;
			fscanf(readFile, "%d", &f5);
			f.v4 = voA[f5];
			f.d = f5;	
			
			foA[foc] = f;	
			foc++;
		}
			
	}
	
	int i = 0;
	
	for (i =  0; i < voc; i++)
	{
		ovnA[i].x = 0; ovnA[i].y = 0; ovnA[i].z = 0;
		//tArray[i].x = 0; tArray[i].y =0;
	}
	for (i = 0 ; i < foc; i++)
	{
		Vect vec1 = disInPoints(foA[i].v1, foA[i].v2);
		Vect vec2 = disInPoints(foA[i].v1, foA[i].v3);
		Vect fNm = crossproduct(vec1, vec2);
		
		int a = foA[i].a; 
		int b = foA[i].b;
		int c = foA[i].c;
		ovnA[a].x += fNm.x; ovnA[a].y += fNm.y; ovnA[a].z += fNm.z;
		ovnA[b].x += fNm.x; ovnA[b].y += fNm.y; ovnA[b].z += fNm.z;
		ovnA[c].x += fNm.x; ovnA[c].y += fNm.y; ovnA[c].z += fNm.z;
	}
	//---------------------------------------------------------------------
	for (i = 0; i< voc; i++)
	{
		Vect v = Vecc (ovnA[i]);
		v = unitize(v);
		ovnA[i]  = Pointt (v)    ;
	}
	
	rewind(readFile);
}

void setNeigbourFace(int f)
{
	fA[f].f1 = findF(fA[f].v1, fA[f].v2 , f );
	fA[f].f2 = findF(fA[f].v2, fA[f].v3 , f );
	fA[f].f3 = findF(fA[f].v3, fA[f].v4 , f );
	fA[f].f4 = findF(fA[f].v4, fA[f].v1 , f );	
}


void * searchAttachElement(Point  & p)
{
	int i = 0;
	p.count = 0;
	p.countE = 0;
	p.avR.x = 0; p.avR.y = 0; p.avR.z = 0;
	for (i = 0; i < fc; i++)
	{
		if ( haspoint (fA[i] ,p))
		{
			p.atF[p.count] = i;
			p.count++;
			
			if (haspointE (fA[i].e1 ,p) )
			{
				Vect mid = fA[i].e1.edgeM;
				p.avR = addv2 (mid, p.avR);
				p.countE++;
			}
			if (haspointE (fA[i].e2 ,p) )
			{
				Vect mid = fA[i].e2.edgeM;
				p.avR = addv2 (mid, p.avR);
				p.countE++;
			}
			if (haspointE (fA[i].e3 ,p) )
			{
				Vect mid = fA[i].e3.edgeM;
				p.avR = addv2 (mid, p.avR);
				p.countE++;
			}
			if (haspointE (fA[i].e4 ,p) )
			{
				Vect mid = fA[i].e4.edgeM;
				p.avR = addv2 (mid, p.avR);
				p.countE++;
			}	
		}
	}
	
	p.avR = div (p.avR, p.countE);
	
}

Vect BigMove(Point p)
{
	Vect F = p.avF;
	Vect R = p.avR;
	Vect P = Vecc(p);
	
	Vect newp = addv4(F,R,R,P);
	newp = div(newp,4);
	
	//printv(newp);
	return newp;
}

void setLight()
{
	GLfloat white[] = {1.,1.,1.,0.0};
	GLfloat lpos[] = {2.0,1.0,1.0,0.0};
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, white);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
    glLightfv(GL_LIGHT0, GL_SPECULAR, white);
}

void * addElement(Point p)
{
	int i = 0; int flag = 0;
	for (i = 0; i < vnc; i++)
	{
		
		Point p2 = vnA[i];
		if (pointEqual (p, p2 ) )
		{
			flag = 1;
			break;
		}
	}
	if (flag != 1)
	{
		//printf("enter~\n");
	
		vnA[i] = p;
		vnc++;
	}
}


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

void Regroup()
{
	int i = 0;
	fnc = 0;
	for (i = 0; i < fc; i++)
	{
		int vv1 = fA[i].a;
		Point p11 = Pointt(fA[i].v1.newp);
		Point p12 = Pointt(fA[i].e1.edgePoint);
		Point pc = Pointt(fA[i].facePoint);
		Point p13 = Pointt(fA[i].e4.edgePoint);
		
		fnA[fnc].v1 = p11; fnA[fnc].v2= p12; fnA[fnc].v3 = pc; fnA[fnc].v4 = p13;
		fnc ++;
		addElement(p11); addElement(p12); addElement(p13); addElement(pc);
		
		int vv2 = fA[i].b;
		Point p21 = Pointt (fA[i].e1.edgePoint);
		Point p22 = Pointt (fA[i].v2.newp);
		Point p23 = Pointt (fA[i].e2.edgePoint);
		
		fnA[fnc].v1 = p21; fnA[fnc].v2= p22; fnA[fnc].v3 = p23; fnA[fnc].v4 = pc;
		fnc ++;
		addElement(p21); addElement(p22); addElement(p23); addElement(pc);
		
		int vv3 = fA[i].c;
		Point p31 = Pointt (fA[i].e2.edgePoint);
		Point p32 = Pointt (fA[i].v3.newp);
		Point p33 = Pointt (fA[i].e3.edgePoint);
		
		fnA[fnc].v1 = pc; fnA[fnc].v2 = p31; fnA[fnc].v3 = p32; fnA[fnc].v4 = p33;
		fnc ++;
		addElement(p31); addElement(p32); addElement(p33);
		
		int vv4 = fA[i].d;
		Point p41 = Pointt (fA[i].e4.edgePoint);
		Point p42 = Pointt (fA[i].e3.edgePoint);
		Point p43 = Pointt (fA[i].v4.newp);
		Face nf4;
		nf4.v1 = p41; nf4.v2 = pc; nf4.v3 = p42; nf4.v4 = p43;
		fnA[fnc] = nf4;  fnc ++;
		addElement(p41); addElement(p42); addElement(p43); 
	}	
}

void loadImage()
{
	width1 = 512;
	height1 = 512;
	
	width2 = 512;
	height2 = 512;
	
	image2 = readPPM("texture.ppm", &width1, &height1);
}


void loadImage2()
{
	width1 = 512;
	height1 = 512;
	
	width2 = 512;
	height2 = 512;
	
	image2 = readPPM("t55.ppm", &width1, &height1);
}



void calcNorm()
{
	int i = 0;
	
	for (i =  0; i < vnc; i++)
	{
		vnnA[i].x = 0; vnnA[i].y = 0; vnnA[i].z = 0;
	}
	for (i = 0 ; i < fnc; i++)
	{
		Vect vec1 = disInPoints(fnA[i].v1, fnA[i].v2);
		Vect vec2 = disInPoints(fnA[i].v1, fnA[i].v3);
		Vect fNm = crossproduct(vec1, vec2);
		
		int a = fnA[i].a; 
		int b = fnA[i].b;
		int c = fnA[i].c;
		vnnA[a].x += fNm.x; vnnA[a].y += fNm.y; vnnA[a].z += fNm.z;
		vnnA[b].x += fNm.x; vnnA[b].y += fNm.y; vnnA[b].z += fNm.z;
		vnnA[c].x += fNm.x; vnnA[c].y += fNm.y; vnnA[c].z += fNm.z;
	}
	//---------------------------------------------------------------------
	for (i = 0; i< vnc; i++)
	{
		Vect v = Vecc (vnnA[i]);
		v = unitize(v);
		vnnA[i]  = Pointt (v)    ;
	}
	
}


void init() 
{
	glEnable(GL_TEXTURE_2D);
	loadImage();
	//setLight();
	loadTexture();
	
	loadOrigin("base.txt");
	loadBlockI("level1.txt");
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glClearColor (0.0, 0.0, 0.0, 0.0);
	
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	
	
	GLfloat ks[] = {1.1,1.,1.0,1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, ks);
	glMaterialf(GL_FRONT,GL_SHININESS,6.0);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
    
	//------------Add initialization -----------
	//setLight();
	
	int j = 0;
	for ( j = 0; j < fc; j++)
	{
		setNeigbourFace(j);
		fA[j].facePoint = findCentroid(fA[j]);	
	}
	for ( j = 0; j < fc; j++)
	{
		findEdgePoint(j);
	}
	int k = 0;
	for (k = 0; k < vc; k++)
	{
		searchAttachElement(vA[k]);
		vA[k].avF = findAVF(vA[k]);	
	}
	
	
	int z = 0;
	for (z= 0; z < fc; z++)
	{
		Face f = fA[z];
		int a =f.a; int b =f.b; int c =f.c; int d =f.d;
		fA[z].v1.avF = vA[a].avF;
		fA[z].v1.avR = vA[a].avR;
		fA[z].v1.newp = BigMove(fA[z].v1);
		
		fA[z].v2.avF = vA[b].avF;
		fA[z].v2.avR = vA[b].avR;
		fA[z].v2.newp = BigMove(fA[z].v2);
		
		fA[z].v3.avF = vA[c].avF;
		fA[z].v3.avR = vA[c].avR;
		fA[z].v3.newp = BigMove(fA[z].v3);
		
		fA[z].v4.avF = vA[d].avF;
		fA[z].v4.avR = vA[d].avR;
		fA[z].v4.newp = BigMove(fA[z].v4);
	}
	 Regroup();
	 
	 int x = 0; int y = 0;
		
	 
	 for (x = 0; x < fnc; x++)
	 {
	 	Point p1 = fnA[x].v1;
	 	Point p2 = fnA[x].v2;
	 	Point p3 = fnA[x].v3;
	 	Point p4 = fnA[x].v4;
	 	
	 	int i = 0;
	 	for (i = 0; i < vnc; i++)
	 	{
	 		if (pointEqual (p1, vnA[i]) )
	 		{
	 			fnA[x].a = i;
	 		}
	 		if (pointEqual (p2, vnA[i]) )
	 		fnA[x].b = i;
	 		if (pointEqual (p3, vnA[i]) )
	 		fnA[x].c = i;
	 		if (pointEqual (p4, vnA[i]) )
	 		fnA[x].d = i;
	 	}
	 
	 }
	 
	 calcNorm();
}


void getBezier ()
{
	float c1 = 3*t;
	float c2 = 3 * t * t;
	float c3 = t * t * t;
	
	float t0 = 1 + (-1)*c1 + c2 + (-1) * c3;
	float t1 = c1 - 2*c2 + 3 * c3;
	float t2 = c2 - 3*c3;
	
	camx = t0*p0[0] + t1*p1[0] + t2*p2[0] + c3*p3[0];
	camy = t0*p0[1] + t1*p1[1] + t2*p2[1] + c3*p3[1];
	camz = t0*p0[2] + t1*p1[2] + t2*p2[2] + c3*p3[2];
	//printf("%f, %f, %f \n", camx, camy, camz);
	t+=camstep;
	if(t>1||t<0)
		camstep*=-1;
	
}




void display(void)
{
	glClearColor(0.0f,0.0f,.3f,0.0f);  //set background color
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();             /* clear the matrix */
	
	
	if (camerachange)
	getBezier();
	gluLookAt(camx,camy ,camz, 0.f,0.f,0.f ,0.f,1.f,0.f);	
	
	
	
	int i = 0;

	glScalef (1,1.7,1.2);
	glRotated(-angle, 1, 0, 0);
	glRotated(-angle2, 0, 1, 0);
	glRotated(-angle3, 0, 0, 1);
	
	if (level == 0)
	{
		glBegin(GL_QUADS);
		for(i = 0 ; i< foc; i++)
		{
			//glColor3f(.7,0.2,0);
			glVertex3f(foA[i].v1.x, foA[i].v1.y, foA[i].v1.z);
			glVertex3f(foA[i].v2.x, foA[i].v2.y, foA[i].v2.z);
			glVertex3f(foA[i].v3.x, foA[i].v3.y, foA[i].v3.z);	
			glVertex3f(foA[i].v4.x, foA[i].v4.y, foA[i].v4.z);	
			
			int a = foA[i].a; 
			int b = foA[i].b;
			int c = foA[i].c;
			int d = foA[i].d;
			
			float aNx = ovnA[a].x;  float aNy = ovnA[a].y;  float aNz = ovnA[a].z;
			float bNx = ovnA[b].x;  float bNy = ovnA[b].y;  float bNz = ovnA[b].z;
			float cNx = ovnA[c].x;  float cNy = ovnA[c].y;  float cNz = ovnA[c].z;
			float dNx = ovnA[d].x;  float dNy = ovnA[d].y;  float dNz = ovnA[d].z;
		
			//glColor3f(.7,0.2,0);
			glTexCoord2f(aNx, aNy);
			glNormal3f(aNx,aNy,aNz);
			glVertex3f(foA[i].v1.x, foA[i].v1.y, foA[i].v1.z);
			
			glTexCoord2f(bNx, bNy);
			glNormal3f(bNx,bNy,bNz);
			glVertex3f(foA[i].v2.x, foA[i].v2.y, foA[i].v2.z);
			
			glTexCoord2f(cNx, cNy);
			glNormal3f(cNx,cNy,cNz);
			glVertex3f(foA[i].v3.x, foA[i].v3.y, foA[i].v3.z);	
			
			glTexCoord2f(dNx, dNy);
			glNormal3f(dNx,dNy,dNz);
			glVertex3f(foA[i].v4.x, foA[i].v4.y, foA[i].v4.z);	
		}
		glEnd();	
	}	
	
	
	if (level == 1)
	{
		//printf("First-level Subdivision \n");
		glBegin(GL_QUADS);
		for(i = 0 ; i< fc; i++)
		{
			int a = fA[i].a; 
			int b = fA[i].b;
			int c = fA[i].c;
			int d = fA[i].d;
		
			float aNx = vnA[a].x;  float aNy = vnA[a].y;  float aNz = vnA[a].z;
			float bNx = vnA[b].x;  float bNy = vnA[b].y;  float bNz = vnA[b].z;
			float cNx = vnA[c].x;  float cNy = vnA[c].y;  float cNz = vnA[c].z;
			float dNx = vnA[d].x;  float dNy = vnA[d].y;  float dNz = vnA[d].z;
		
			//glColor3f(.7,0.2,0);
			glTexCoord2f(aNx, aNy);
			glNormal3f(aNx,aNy,aNz);
			glVertex3f(fA[i].v1.x, fA[i].v1.y, fA[i].v1.z);
			
			glTexCoord2f(bNx, bNy);
			glNormal3f(bNx,bNy,bNz);
			glVertex3f(fA[i].v2.x, fA[i].v2.y, fA[i].v2.z);
			
			glTexCoord2f(cNx, cNy);
			glNormal3f(cNx,cNy,cNz);
			glVertex3f(fA[i].v3.x, fA[i].v3.y, fA[i].v3.z);	
			
			glTexCoord2f(dNx, dNy);
			glNormal3f(dNx,dNy,dNz);
			glVertex3f(fA[i].v4.x, fA[i].v4.y, fA[i].v4.z);	
		}
		glEnd();	
	}	
	
	if (level == 2)
	{
		glBegin(GL_QUADS);
		for(i = 0 ; i< fnc; i++)
		{
			int a = fnA[i].a; 
			int b = fnA[i].b;
			int c = fnA[i].c;
			int d = fnA[i].d;
		
			float aNx = vnnA[a].x;  float aNy = vnnA[a].y;  float aNz = vnnA[a].z;
			float bNx = vnnA[b].x;  float bNy = vnnA[b].y;  float bNz = vnnA[b].z;
			float cNx = vnnA[c].x;  float cNy = vnnA[c].y;  float cNz = vnnA[c].z;
			float dNx = vnnA[d].x;  float dNy = vnnA[d].y;  float dNz = vnnA[d].z;
			
			glTexCoord2f(aNx, aNy);
			glNormal3f(aNx,aNy,aNz);
			glVertex3f(fnA[i].v1.x, fnA[i].v1.y, fnA[i].v1.z);
			
			glTexCoord2f(bNx, bNy);
			glNormal3f(bNx,bNy,bNz);
			glVertex3f(fnA[i].v2.x, fnA[i].v2.y, fnA[i].v2.z);
			
			glTexCoord2f(cNx, cNy);
			glNormal3f(cNx,cNy,cNz);
			glVertex3f(fnA[i].v3.x, fnA[i].v3.y, fnA[i].v3.z);	
			
			glTexCoord2f(dNx, dNy);
			glNormal3f(dNx,dNy,dNz);
			glVertex3f(fnA[i].v4.x, fnA[i].v4.y, fnA[i].v4.z);	
		}
		glEnd();	
	}	
	
	
	glutSwapBuffers();
	glFlush ();
}

void reshape (int w, int h)
{
	glViewport(0, 0, w, h);
	float fAspect = ((float)w)/h; 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluPerspective(70.1,1.0,0.01,100.0); //Perspective mode
	glMatrixMode (GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) 
	{
		case 27:
		exit(0);
		break;
	
		
		case 'z': case 'Z':
		{
			if (level == 2)
			level = 0;
			else
			level = level+1;
			break;
		}
		
		case 'c':case 'C':
		{
			if (camerachange == 0)
			camerachange = 1;
			else
			camerachange = 0;
			break;
		}
		case 'p':case 'P':
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
	switch (key)
	{
		case GLUT_KEY_UP:
		angle +=1;
		break;
		case GLUT_KEY_DOWN:
		angle -=1;
		break;
		case GLUT_KEY_RIGHT:
		angle2 = angle2+1;
		break;
		case GLUT_KEY_LEFT:
		angle2 = angle2-1;
		break;
		
	}
}

void timer (int v)
{	
	glutPostRedisplay();

	glutTimerFunc(1000/30, timer, v);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (800, 800); 
	glutInitWindowPosition (100, 100);
	glutCreateWindow ("BLOCK I _liu275");
	
	init ();
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(100,timer,30);
	glutMainLoop();
	return 0;
}
