/* 
 * Virtual Solar System Animation 
 *
 */

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Bitmap.h"

#define 	SOLAR_YEAR	200	/* Rotation Period of the solar system */
#define 	MERCURY_YEAR	-360	/* Rotation Period of the Moon */	
#define 	EARTH_YEAR 	4320	/* Rotation Period of the Earth */	
#define 	VENUS_YEAR 	2880	/* Rotation Period of the Venus */	
#define 	MARS_YEAR 	8640	/* Rotation Period of the Mars */	
#define 	MOON_YEAR 	360	/* Rotation Period of the Moon */	
#define 	JUPITER_YEAR	-40000	/* Rotation Period of the Jupiter */	
#define 	SATURN_YEAR 	120000	/* Rotation Period of the Saturn */	
#define 	URANUS_YEAR 	-360000	/* Rotation Period of the Uranus */	
#define 	NEPTUNE_YEAR 	720000	/* Rotation Period of the Neptune */	
#define 	PLUTO_YEAR 	360	/* Rotation Period of the Pluto */	

#define 	MERCURY_DAY 	66	/* Self-rotation Period of the Mercury */	
#define 	EARTH_DAY 	11	/* Self-rotation Period of the Earth */	
#define 	VENUS_DAY 	66	/* Self-rotation Period of the Venus */	
#define 	MARS_DAY 	11	/* Self-rotation Period of the Mars */	
#define 	JUPITER_DAY 	4	/* Self-rotation Period of the Jupiter */	
#define 	SATURN_DAY 	5	/* Self-rotation Period of the Saturn */	
#define 	URANUS_DAY 	8	/* Self-rotation Period of the Uranus */	
#define 	NEPTUNE_DAY 	8	/* Self-rotation Period of the Neptune */	
#define 	PLUTO_DAY 	66	/* Self-rotation Period of the Pluto */	

#define		SUN_RADIUS	0.10	/* Radius of the Sun */
#define		MERCURY_RADIUS	0.06	/* Radius of the Mercury*/
#define		VENUS_RADIUS	0.25	/* Radius of the Venus */
#define		EARTH_RADIUS	0.25	/* Radius of the Earth */
#define		MOON_RADIUS	0.08	/* Radius of the Moon */
#define		MARS_RADIUS	0.15	/* Radius of the Mars */
#define		JUPITER_RADIUS	0.75	/* Radius of the Jupiter */
#define		SATURN_RADIUS	0.65	/* Radius of the Saturn */
#define		URANUS_RADIUS	0.40	/* Radius of the Uranus*/
#define		NEPTUNE_RADIUS	0.30	/* Radius of the Neptune*/
#define		PLUTO_RADIUS	0.04	/* Radius of the Pluto*/

#define		MERCURY_ORBIT	0.25	/* Orbit Radius of the Mercury */
#define		VENUS_ORBIT	0.75	/* Orbit Radius of the Venus */
#define		EARTH_ORBIT	1.75	/* Orbit Radius of the Earth */
#define		MOON_ORBIT	0.35	/* Orbit Radius of the Moon */
#define		MARS_ORBIT	2.5	/* Orbit Radius of the Mars */
#define		JUPITER_ORBIT	3.8	/* Orbit Radius of the Jupiter */
#define		SATURN_ORBIT	6.5	/* Orbit Radius of the Saturn */
#define		URANUS_ORBIT	8.5	/* Orbit Radius of the Uranus */
#define		NEPTUNE_ORBIT	10.5	/* Orbit Radius of the Neptune */
#define		PLUTO_ORBIT	12.0	/* Orbit Radius of the Pluto */

#define MAX_STAR 1000
#define TRUE 1
#define FALSE 0

float angle=0.0;
char G_Lmousedown = FALSE; 
char G_Rmousedown = FALSE; 

typedef struct tmp_star
{
	float		pos[3];
	float		col[4];
} Star;

Star star[MAX_STAR];

float winwidth, winheight;
int _ScreenMode=3;

// mouse position
int mouse_x;
int mouse_y;

// zoom
float zooming = -15.0f;
int zoom = 0;

// angle of revolution
float fSolarRot = 0.0f;
float fMoonRot = 0.0f;
float fEarthRot = 0.0f;
float fVenusRot = 0.0f;
float fMarsRot = 0.0f;
float fMercuryRot = 0.0f;
float fJupiterRot = 0.0f;
float fSaturnRot = 0.0f;
float fUranusRot = 0.0f;
float fNeptuneRot = 0.0f;
float fPlutoRot = 0.0f;

// angle of self-revoluation
float fselfEarthRot = 0.0f;
float fselfVenusRot = 0.0f;
float fselfMarsRot = 0.0f;
float fselfMercuryRot = 0.0f;
float fselfJupiterRot = 0.0f;
float fselfSaturnRot = 0.0f;
float fselfUranusRot = 0.0f;
float fselfNeptuneRot = 0.0f;
float fselfPlutoRot = 0.0f;

// control the coordinate
int Rotate = FALSE;
float rotation = 0.0f;

// global roation is set or not
int Grotation = TRUE;
int Pause = FALSE;
int RDrag = FALSE;
int LDrag = FALSE;


GLubyte *texmoon_w, *texearth_w, *texvenus_w, *texmars_w, *texmercury_w,
		*texjupiter_w, *texsaturn_w, *texuranus_w, *texneptune_w, *texpluto_w;
int texmoonWidth, texearthWidth, texvenusWidth, texmarsWidth, texmercuryWidth,
	texjupiterWidth, texsaturnWidth, texuranusWidth, texneptuneWidth, texplutoWidth,
	texmoonHeight, texearthHeight, texvenusHeight, texmarsHeight, texmercuryHeight,
	texjupiterHeight, texsaturnHeight, texuranusHeight, texneptuneHeight, texplutoHeight;
	

GLUquadricObj *venus;
GLUquadricObj *earth;
GLUquadricObj *moon;
GLUquadricObj *mars;
GLUquadricObj *mercury;
GLUquadricObj *jupiter;
GLUquadricObj *saturn;
GLUquadricObj *uranus;
GLUquadricObj *neptune;
GLUquadricObj *pluto;

// Lighting values
GLfloat  whiteLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat  sourceLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat	 lightPos[] = { 0.0f, 0.0f, 0.0f, 1.0f };

// capture the BMP file
GLubyte* TextureLoadBitmap(char *filename, int *w, int *h)		/* I - Bitmap file to load */
{
  BITMAPINFO	*info;				/* Bitmap information */
  void		*bits;				/* Bitmap pixel bits */
  GLubyte	*rgb;				/* Bitmap RGB pixels */

 /*
  * Try loading the bitmap and converting it to RGB...
  */

  bits = LoadDIBitmap(filename, &info);
  if (bits == NULL)
	  return (NULL);
  rgb = ConvertRGB(info, bits);
  if (rgb == NULL)
  {
    free(info);
    free(bits);
  };

  *w = info->bmiHeader.biWidth;
  *h = info->bmiHeader.biHeight;

  /*
   * Free the bitmap and RGB images, then return 0 (no errors).
   */

  free(info);
  free(bits);
  return (rgb);

}

/* keyboard CALLBACK for handling keyboard event */
void keyboard(unsigned char key, int x, int y)
{
	/* control the animation interactively */
	switch (key) {
	case 'g': if (Grotation == FALSE)
				  Grotation = TRUE;
			  else
				  Grotation = FALSE;
			  
			  break;

	case 't': if (Pause == FALSE)
				  Pause = TRUE;
			  else 
				  Pause = FALSE;
			  break;

	case 'r': fSolarRot = 0.0f;
			  fMoonRot = 0.0f;
			  fEarthRot = 0.0f;
			  fVenusRot = 0.0f;
			  fMarsRot = 0.0f;
			  fMercuryRot = 0.0f;
			  fJupiterRot = 0.0f;
			  fSaturnRot = 0.0f;
			  fUranusRot = 0.0f;
			  fNeptuneRot = 0.0f;
			  fPlutoRot = 0.0f;

			  fselfEarthRot = 0.0f;
			  fselfVenusRot = 0.0f;
			  fselfMarsRot = 0.0f;
			  fselfMercuryRot = 0.0f;
			  fselfJupiterRot = 0.0f;
			  fselfSaturnRot = 0.0f;
			  fselfUranusRot = 0.0f;
			  fselfNeptuneRot = 0.0f;
 			  fselfPlutoRot = 0.0f;

			  break;
	}

	glutPostRedisplay();
}

/* mouse CALLBACK for handling mouse click */
void mouse(int button, int state, int x, int y)
{	
	LDrag = FALSE;
	RDrag = FALSE;

	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		G_Lmousedown = TRUE;
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		G_Lmousedown = FALSE;
	}

	if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		G_Rmousedown = TRUE;
	}
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		G_Rmousedown = FALSE;
	}

	mouse_x = x;
	mouse_y = y;

	glutPostRedisplay();
}


void drag(int ix, int iy)
{
//	float x, y;
	if ((!G_Lmousedown) && (!G_Rmousedown))
		return;
    
	LDrag = FALSE;
	RDrag = FALSE;

	if ((iy > mouse_y)&&(G_Lmousedown))
	{
		Rotate = 1;
		LDrag = TRUE;
	}
	else if ((iy < mouse_y)&&(G_Lmousedown))
	{
		Rotate = -1;
		LDrag = TRUE;
	}
	else if (G_Lmousedown)
	{
		Rotate = 0;
		LDrag = FALSE;
	}

	if ((iy > mouse_y)&&(G_Rmousedown))
	{
		zoom = 1;
		RDrag = TRUE;
	}
	else if ((iy < mouse_y)&&(G_Rmousedown))
	{
		zoom = -1;		
		RDrag = TRUE;
	}
	else if (G_Rmousedown)
	{
		zoom = 0;
		RDrag = FALSE;
	}

	mouse_x = ix;
	mouse_y = iy;

	glutPostRedisplay();
}


/* special CALLBACK */ 
void Special(int key, int x, int y)
{
	if(key == GLUT_KEY_F1) {
		_ScreenMode ++;
		if(_ScreenMode > 3) _ScreenMode=0;
		if(_ScreenMode == 0)
			glutReshapeWindow(350, 200);
		else if(_ScreenMode == 1)
			glutReshapeWindow(700, 400);
		else if(_ScreenMode == 2)
			glutReshapeWindow(900, 600);
		else if(_ScreenMode == 3)
			glutReshapeWindow(150, 75);
	}
}

/* reshape CALLBACK */
void reshape(w, h)
GLsizei w;
GLsizei h;
{
	//int size;

	//printf("reshape");

	winwidth  = w;
	winheight = h;
	//size=w>h?h:w;
	//glViewport(0, 0, size, size);		/* viewport transformation */
	glViewport(0, 0, w, h);		/* viewport transformation */
	glMatrixMode(GL_PROJECTION);	/* projection transformation */
	glLoadIdentity(); 
	//gluPerspective(40,1.0,1.0,20.0); 
	gluPerspective(40,(GLfloat)w/(GLfloat)h,1.0,45.0); 
	glutPostRedisplay();
}

/* display CALLBACK */
void display(void)
{
	int i,j;

	GLfloat SunSource[] ={0.5,0.5,0.5,1.0};
	GLfloat StarSource[] = {0.3, 0.3, 0.3, 1.0}; 

	/* clear the window color before drawing is performed */
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set light position before viewing transformation
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	// Translate the whole scene out and into view	
	glTranslatef(0.0f, 0.0f, zooming);	
 
	// Global rotation	
	glRotatef(fSolarRot, 0.0f, 1.0f, 0.0f);

	// control coordinate system
	glRotatef(rotation, 1.0f, 0.0f,0.0f);
	
	glPushMatrix();		

	// Set material color, 
// Sun	
	glColor3f(1,1,1); 
	glMaterialfv(GL_FRONT, GL_EMISSION, SunSource);
	glutSolidSphere(SUN_RADIUS, 15, 15);

	// Move the light after we draw the sun!
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);

	glPopMatrix();
	glPushMatrix(); 

// Stars
	glPointSize(1.3);
	glBegin(GL_POINTS);

		for (i=0; i< MAX_STAR; i++)		
		{
			glColor4fv(star[i].col);
			glMaterialfv(GL_FRONT, GL_EMISSION, StarSource);
			glVertex3fv(star[i].pos);
		}

	glEnd();

	glPopMatrix();

	glEnable(GL_TEXTURE_2D);

	glPushMatrix();

// VENUS

	glRotatef(20.0, 0.5f, 0.0f, -0.1f); 
	// Rotate coordinate system
	glRotatef(fVenusRot, 0.0f, 1.0f, 0.0f);
	//Draw Venus
	glTranslatef(VENUS_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfVenusRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	
	glTexImage2D(GL_TEXTURE_2D,0, 3, texvenusWidth, texvenusHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texvenus_w);
	gluSphere(venus, VENUS_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//EARTH

	// Rotate coordinate system
	glRotatef(30.0, 0.5f, 0.0f, -0.1f); 
	glRotatef(fEarthRot, 0.0f, 1.0f, 0.0f);

	// Draw the Earth
	glTranslatef(EARTH_ORBIT,0.0f,0.0f);

	glPushMatrix();
	
	glRotatef(360.0-fselfEarthRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texearthWidth, texearthHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texearth_w);
	gluSphere(earth, EARTH_RADIUS, 15,15);
	
	glPopMatrix();

//MOON

	// Rotate from Earth based coordinates and draw Moon
	glRotatef(fMoonRot,0.0f, 1.0f, 0.0f);
	glTranslatef(MOON_ORBIT, 0.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texmoonWidth, texmoonHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texmoon_w);
	gluSphere(moon, MOON_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//MARS

	// Rotate coordinate system
	glRotatef(25.0, 1.0f, 0.0f, -0.1f); 
	glRotatef(fMarsRot, 0.0f, 1.0f, 0.0f);
	//Draw Mars
	glTranslatef(MARS_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfMarsRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texmarsWidth, texmarsHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texmars_w);
	gluSphere(mars, MARS_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//MERCURY
	// Rotate coordinate system
	glRotatef(fMercuryRot, 0.0f, 1.0f, 0.0f);
	//Draw MERCURY
	glTranslatef(MERCURY_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfMercuryRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texmercuryWidth, texmercuryHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texmercury_w);
	gluSphere(mercury, MERCURY_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//Jupiter
	// Rotate coordinate system
	glRotatef(35.0, 0.0f, 0.0f, 0.5f); 
	glRotatef(fJupiterRot, 0.0f, 1.0f, 0.0f);
	//Draw Jupiter
	glTranslatef(JUPITER_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfJupiterRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texjupiterWidth, texjupiterHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texjupiter_w);
	gluSphere(jupiter, JUPITER_RADIUS, 15,15);
	glPopMatrix();
	glPushMatrix();

//Saturn
	// Rotate coordinate system
	glRotatef(195.0, -1.0f, 0.0f, -0.1f); 
	glRotatef(fSaturnRot, 0.0f, 1.0f, 0.0f);
	//Draw Saturn
	glTranslatef(SATURN_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfSaturnRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texsaturnWidth, texsaturnHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texsaturn_w);
	gluSphere(saturn, SATURN_RADIUS, 15,15);

	// to draw the ring of the saturn
	for (i=0; i<20; i++)
	{
		float ring;		

		glLineWidth(2.0f);
		glBegin(GL_LINE_STRIP);
			for (j=0; j <360 ; j++)
			{
				ring = JUPITER_RADIUS+0.1+ 0.015*i;
				glVertex3f(ring*cos(j* 3.1415 /180), ring*sin(j* 3.1415 /180), 0.0f);
			}
		glEnd();
	}

	glPopMatrix();
	glPushMatrix();

//Uranus
	// Rotate coordinate system
	glRotatef(20.0, 1.0f, 0.0f, 0.1f); 
	glRotatef(fUranusRot, 0.0f, 1.0f, 0.0f);
	//Draw Uranus
	glTranslatef(URANUS_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfUranusRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texuranusWidth, texuranusHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texuranus_w);
	gluSphere(uranus, URANUS_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//Neptune
	// Rotate coordinate system
	glRotatef(20.0, 1.0f, 0.0f, -0.3f); 
	glRotatef(fNeptuneRot, 0.0f, 1.0f, 0.0f);
	//Draw Neptune
	glTranslatef(NEPTUNE_ORBIT,0.0f,0.0f);
	//glRotatef(40,1,1,0); 
	glRotatef(360.0-fselfNeptuneRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texneptuneWidth, texneptuneHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texneptune_w);
	gluSphere(neptune, NEPTUNE_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

//Pluto
	// Rotate coordinate system
	glRotatef(fPlutoRot, 0.0f, 1.0f, 0.0f);
	//Draw Pluto
	glTranslatef(PLUTO_ORBIT,0.0f,0.0f);
	glRotatef(360.0-fselfPlutoRot, 0.0f,1.0f, 0.0f);
	glRotatef(270.0f, 1.0f,0.0f,0.0f);
	glTexImage2D(GL_TEXTURE_2D,0, 3, texplutoWidth, texplutoHeight,0,GL_RGB,GL_UNSIGNED_BYTE,texpluto_w);
	gluSphere(pluto, PLUTO_RADIUS, 15,15);

	glPopMatrix();
	glPushMatrix();

	// Restore the matrix state
	glPopMatrix();	// Modelview matrix

	/* transform the planets & draw them, draw the 
	 * star-sphere by points, material properties of the Sun & planets 
	 * should be set correctly.
	 * use glTranslate, glRotate, glLoadIdentity, glPushMatrix, glPopMatrix, 
	 * glMaterial, glBegin, gluSphere...etc
	 *
	 *  Texture Mapping for each planets except the Sun
	 * use glTexImage2D..
	 */

	glDisable(GL_TEXTURE_2D);

	glFlush();
	glutSwapBuffers();
}

void init()
{
	int i,j;
	float angle1, angle2;

	GLfloat light_amb0[] = {0.0, 0.0, 0.0, 1.0};  
	GLfloat light_dif0[] = {1.0, 1.0, 1.0, 1.0};  
	GLfloat light_spc0[] = {1.0, 1.0, 1.0, 1.0};  
	
	glEnable(GL_LIGHTING);
	
	/* Clear background with black */
	glClearColor(0,0,0,0);
	glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	/* OpenGL setting for texture mapping to function correctly */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
	glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	/* initialize lighting, read bitmap images, 
	 * initialize different GLU spheres and the position of the stars.
	 * use glLight, TextureLoadBitmap, gluNewQuadric...etc
	 */

	// Setup and enable light 0
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT,light_amb0);
	glLightfv(GL_LIGHT0,GL_DIFFUSE,light_dif0);
	glLightfv(GL_LIGHT0,GL_SPECULAR,light_spc0);
	glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
	glEnable(GL_LIGHT0);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f );

	texmoon_w = TextureLoadBitmap("moon.bmp", &texmoonWidth, &texmoonHeight);
	texearth_w = TextureLoadBitmap("earth.bmp", &texearthWidth, &texearthHeight);
	texvenus_w = TextureLoadBitmap("venus.bmp", &texvenusWidth, &texvenusHeight);
	texmars_w = TextureLoadBitmap("mars.bmp", &texmarsWidth, &texmarsHeight);
	texmercury_w = TextureLoadBitmap("mercury.bmp", &texmercuryWidth, &texmercuryHeight);
	texjupiter_w = TextureLoadBitmap("jupiter.bmp", &texjupiterWidth, &texjupiterHeight);
	texsaturn_w = TextureLoadBitmap("saturn.bmp", &texsaturnWidth, &texsaturnHeight);
	texuranus_w = TextureLoadBitmap("uranus.bmp", &texuranusWidth, &texuranusHeight);
	texneptune_w = TextureLoadBitmap("neptune.bmp", &texneptuneWidth, &texneptuneHeight);
	texpluto_w = TextureLoadBitmap("pluto.bmp", &texplutoWidth, &texplutoHeight);

	venus = gluNewQuadric();
	gluQuadricTexture(venus, GL_TRUE);

	earth = gluNewQuadric();
	gluQuadricTexture(earth, GL_TRUE);

	moon = gluNewQuadric();
	gluQuadricTexture(moon, GL_TRUE);

	mars = gluNewQuadric();
	gluQuadricTexture(mars, GL_TRUE);

	mercury = gluNewQuadric();
	gluQuadricTexture(mercury, GL_TRUE);

	jupiter = gluNewQuadric();
	gluQuadricTexture(jupiter, GL_TRUE);

	saturn = gluNewQuadric();
	gluQuadricTexture(saturn, GL_TRUE);

	uranus = gluNewQuadric();
	gluQuadricTexture(uranus, GL_TRUE);

	neptune = gluNewQuadric();
	gluQuadricTexture(neptune, GL_TRUE);

	pluto = gluNewQuadric();
	gluQuadricTexture(pluto, GL_TRUE);


	for (i=0; i< MAX_STAR; i++)
	{
		for (j=0; j<4; j++) 
		{
			star[i].col[j] = 1.0;
		}

		angle1 = (float)rand()/RAND_MAX * 2 * 3.1415;
		angle2 = (float)rand()/RAND_MAX * 2 * 3.1415;

		star[i].pos[0] = 10 * sin(angle1) * cos(angle2);
		star[i].pos[1] = 10 * sin(angle1) * sin(angle2);
		star[i].pos[2] = 10 * cos(angle1);
	}

	
}

void idle(void)
{
	/* update the rotation angles, & force the system 
	 * to display in the event
	 */

	if (RDrag)
	{	
		zooming += -0.5f*zoom;

		if (zooming < -28.0f)
			zooming = -28.0f;
		if (zooming > -2.0f)
			zooming = -2.0f;

		RDrag = FALSE;
	}

	if (LDrag)
	{
		rotation += 1.0*Rotate;

		LDrag = FALSE;
	}
		
	if (Grotation == TRUE)
		fSolarRot+= 360.0f/SOLAR_YEAR;
	
	if(fSolarRot > 360.0f)
		fSolarRot = 0.0f;

	if (Pause == FALSE)
	{
		fselfMercuryRot += 360.0/MERCURY_DAY; 
		fMercuryRot += 360.0/MERCURY_YEAR; 
		fselfVenusRot += 360.0/VENUS_DAY;
		fVenusRot += 360.0/VENUS_YEAR;
		fselfEarthRot += 360.0/EARTH_DAY;
		fEarthRot += 360.0/EARTH_YEAR;
		fMoonRot+= 360.0/MOON_YEAR;
		fselfMarsRot += 360.0/MARS_DAY;
		fMarsRot+= 360.0/MARS_YEAR;
		fselfJupiterRot += 360.0/JUPITER_DAY;
		fJupiterRot+= 360.0/JUPITER_YEAR;
		fselfSaturnRot += 360.0/SATURN_DAY;
		fSaturnRot+= 360.0/SATURN_YEAR;
		fselfUranusRot += 360.0/URANUS_DAY;
		fUranusRot+= 360.0/URANUS_YEAR;
		fselfNeptuneRot += 360.0/NEPTUNE_DAY;
		fNeptuneRot+= 360.0/NEPTUNE_YEAR;
		fselfPlutoRot += 360.0/PLUTO_DAY;
		fPlutoRot+= 360.0/PLUTO_YEAR;

	}

	if(fMercuryRot > 360.0f)
		fMercuryRot = 0.0f;

	if(fselfMercuryRot > 360.0f)
		fselfMercuryRot = 0.0f;

	if(fVenusRot > 360.0f)
		fVenusRot = 0.0f;

	if(fselfVenusRot > 360.0f)
		fselfVenusRot = 0.0f;
	
	if(fEarthRot > 360.0f)
		fEarthRot = 0.0f;

	if(fselfEarthRot > 360.0f)
		fselfEarthRot = 0.0f;
	
	if(fMoonRot > 360.0f)
		fMoonRot = 0.0f;

	if(fMarsRot > 360.0f)
		fMarsRot = 0.0f;

	if(fselfMarsRot > 360.0f)
		fselfMarsRot = 0.0f;

	if(fJupiterRot > 360.0f)
		fJupiterRot = 0.0f;

	if(fselfJupiterRot > 360.0f)
		fselfJupiterRot = 0.0f;

	if(fSaturnRot > 360.0f)
		fSaturnRot = 0.0f;

	if(fselfSaturnRot > 360.0f)
		fselfSaturnRot = 0.0f;

	if(fUranusRot > 360.0f)
		fUranusRot = 0.0f;

	if(fselfUranusRot > 360.0f)
		fselfUranusRot = 0.0f;

	if(fNeptuneRot > 360.0f)
		fNeptuneRot = 0.0f;

	if(fselfNeptuneRot > 360.0f)
		fselfNeptuneRot = 0.0f;

	if(fPlutoRot > 360.0f)
		fPlutoRot = 0.0f;

	if(fselfPlutoRot > 360.0f)
		fselfPlutoRot = 0.0f;

	glutPostRedisplay();

}

void main(int argc, char** argv)
{
	/*Initialization of GLUT Library */
	glutInitWindowSize(700, 400);
	glutInitWindowPosition(100, 100);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

	/*Create a window with title specified */
	glutCreateWindow("Solar System");

	/*Register different CALLBACK function for GLUT to response 
	with different events, e.g. window sizing, mouse click or
	keyboard stroke */
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(drag);
	glutSpecialFunc(Special);
	glutIdleFunc(idle);
	init();	/*not GLUT call, initialize several parameters */

	
	
	/*Enter the GLUT event processing loop which never returns.
	it will call different registered CALLBACK according
	to different events. */
	glutMainLoop();

}
