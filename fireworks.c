/* 
 * Rendering Fireworks Display using Particle System
 * Feature : when using the hongkong.ppm as a background, once the firework explodes
 *			 the background lights up and fades. If there is no firework, then the 
 *			 background becomes dark.
 * 
 *
 */

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#define MAX_FIREWORK 50
#define DENSITY 100
#define BRIGHTNESS 0.4
#define RANDOMNESS 0.7
#define SCALE 0.02
#define G -0.002
#define FALSE 0
#define TRUE  1
enum mode {RANDOM, PERIOD, MOUSE, KEY};

#define mean_vel_x 0.08	// mean horizontal velocity(x)
#define mean_vel_y 0.06		// mean vertical velocity(y)
#define delta_alpha_value -0.04	// change of the aplha value 

/* Hermite coefficents */
#define H0(u) ( 2*u*u*u - 3*u*u + 1 )
#define H1(u) ( -2*u*u*u + 3*u*u )
#define H2(u) ( u*u*u - 2*u*u + u )
#define H3(u) ( u*u*u - u*u )

/* Data structure of a particle */
typedef struct tmp_particle
{
	float pos[2];		/* current position of the particle */
	float vel[2];		/* current velocity of the particle */
	float acc[2];		/* current acceleration of the particle */
	float init_vel[2];	/* initial velocity of the particle */
	float col[4];		/* current color of the particle */
	float d_col[4];		/* delta color of the particle */
	int death;			/* Boolean for death of particle */
} Particle;

/* Data structure of a firework */
typedef struct tmp_firework
{
	float t;					/* age of firework */
	float init_pos[2];			/* initial position of firework */
	int total_no;				/* total no of particles */
	int death;					/* Boolean for death of firework */
	Particle particle[DENSITY]; /* Particle array */
} Firework;


/* Global variables */
float bg_alpha=0.5, EPSILON=0.000001;	// alpha value for the background
float _winwidth, _winheight;
static int _teximageWidth, _teximageHeight;static int _teximageWidth_new = 1, _teximageHeight_new = 1;
int _background = TRUE;
GLubyte *_teximage, *_teximage_new;
int _timeout, _ScreenMode=3;
int _head = 0, _tail = 0, _empty = TRUE, _full = FALSE; 
enum mode _displayMode = RANDOM;
Firework fire[MAX_FIREWORK];

///////////////////////////////////////////////////////////////////////////
// Convert the mouse pos to our coordinate ////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void TransformXY(int ix, int iy, float *x, float *y)
{
  *x = 2.0*(float)ix/(float)_winwidth - 1.0;
  *y = -2.0*(float)iy/(float)_winheight + 1.0;
}

///////////////////////////////////////////////////////////////////////////
// capture the PPM file..//////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void getPPMFile(char *filename)
{
   int h, w;
   char buf[200], temp[80];
   FILE *fp;

   if ((fp = fopen(filename, "r"))==NULL) {
      printf("File cannot open!!\n");
      exit(1);
   }
   else {
     fscanf(fp, "%s\n",buf);  // GET "P6"
	 fscanf(fp, "%s", temp);
	 while(!strcmp(temp, "#")) {
		while(fgetc(fp)!='\n'){}
        fscanf(fp, "%s", temp);
     }
     w=atoi(temp);
     fscanf(fp, " %d\n", &h); //  height
   	 fscanf(fp, "%s\n", temp);  // max value for each channel. 

     _teximage=(GLubyte*) malloc(sizeof(GLubyte)*w*h*3);
     fread(_teximage, sizeof(GLubyte)*3, w*h, fp);
     _teximageWidth=w;
     _teximageHeight=h;
     fclose(fp);
   }
}


///////////////////////////////////////////////////////////////////////////
// Initialize the attributes of the firework //////////////////////////////
///////////////////////////////////////////////////////////////////////////
void InitFirework(int no, float init_pos[2])
{
	/* initialize the attributes of 
	 * each firework particle 
	 */

	int i,j,angle;
	float range;

	// initialize the firework
	fire[no].t = 0;	// age
	fire[no].death = FALSE;	
	fire[no].total_no = DENSITY;
	fire[no].init_pos[0] = init_pos[0];	// initial position of the firework
	fire[no].init_pos[1] = init_pos[1];

	for (j=0; j<3; j++) 
	{
		fire[no].particle[0].col[j] = (float)rand()/RAND_MAX;		// colour of the firework, randomness of the color		
		fire[no].particle[0].d_col[j] = -1 * (float)rand()/RAND_MAX * RANDOMNESS * 0.1;		// assign the delta RGB value by random
	}

	// initialize each particle in the firework
	for (i=0; i< DENSITY; i++)
	{
		fire[no].particle[i].death = FALSE;

		fire[no].particle[i].pos[0] = init_pos[0]; // initialize the position of the particles
		fire[no].particle[i].pos[1] = init_pos[1];
				
		fire[no].particle[i].acc[0] = 0; // horizontal direction, no gravity
		fire[no].particle[i].acc[1] = G; // vertical direction, has gravity

		// randomness of the shape
		angle = rand()%360;
		range = (float)rand()/RAND_MAX * SCALE;

		fire[no].particle[i].init_vel[0] = range * cos(angle); // initialize horizontal velocity
		fire[no].particle[i].init_vel[1] = range * sin(angle) ; // initialize vertical velocity

		// assign the initial velocities to current velocities
		fire[no].particle[i].vel[0] = fire[no].particle[i].init_vel[0]; 
		fire[no].particle[i].vel[1] = fire[no].particle[i].init_vel[1];

		// make sure the particles in the same firework to be same colour at the beginning and the same delta change value
		for (j=0; j<3; j++) 
		{
			fire[no].particle[i].col[j] = fire[no].particle[0].col[j];	
			fire[no].particle[i].d_col[j] = fire[no].particle[0].d_col[j];	
		}

		// assign the alpha value (randomness)
		fire[no].particle[i].col[3] = 0.8 + 0.2 * (float)rand()/RAND_MAX;
	
		// assign the delta alpha value as negative
		fire[no].particle[i].d_col[3] = delta_alpha_value;
	}
}

///////////////////////////////////////////////////////////////////////////
// Updating Firework ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void UpdateFirework(int no)
{
	/* update the attribute of 
	 * each firework particle 
	 */

	int i,j;

	for (i=0; i< fire[no].total_no; i++)
	{
		// Update RGB value of the particle and fading
		for (j=0; j<4; j++)
			fire[no].particle[i].col[j] +=  fire[no].particle[i].d_col[j];

		// if the alpha value is below 0.1, it is regarded as a "dead" particle
		if (fire[no].particle[i].col[3] < 0.1)
			fire[no].particle[i].death = TRUE;

		// Update the velocity 
		fire[no].particle[i].vel[0] += fire[no].particle[i].acc[0];
		fire[no].particle[i].vel[1] += fire[no].particle[i].acc[1];			
		
		// change of position
		fire[no].particle[i].pos[0] += fire[no].particle[i].vel[0];
		fire[no].particle[i].pos[1] += fire[no].particle[i].vel[1];
	}
}

///////////////////////////////////////////////////////////////////////////
// Drawing Firework ///////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void DrawFirework(int no)
{
	/* to display 
	 * each firework particle 
	 * You should calculate and display the trail of each
	 * particle here 
	 */

	int i, j, k, u;
	float interpolated_pt[2];
	float Color[4];

	for (i=0; i< fire[no].total_no; i++)
	{
		// if the paticle is still "alive"
		if (!fire[no].particle[i].death)
		{
			glLineWidth(1.5);
			glBegin(GL_LINE_STRIP); // for each paticle, generate its spline curve
				Color[0] = fire[no].particle[i].col[0];	
				Color[1] = fire[no].particle[i].col[1];	
				Color[2] = fire[no].particle[i].col[2];	
				Color[3] = fire[no].particle[i].col[3];	

				for (k=0; k<3; k++)
					Color[k] = 1.0;

				// print the initial position
				glColor4fv(Color);
				glVertex2fv(fire[no].init_pos);

				// interpolation
				for (u=1; u<=10; u++) 
				{
					for (j=0; j<2; j++) 
						// using the Hermite Spline Interpolation technique to find the interpolated points
						interpolated_pt[j] = fire[no].init_pos[j]*H0(u/10.0)+
											 fire[no].particle[i].pos[j]*H1(u/10.0)+
											 fire[no].particle[i].init_vel[j]*fire[no].t*H2(u/10.0)+
											 fire[no].particle[i].vel[j]*fire[no].t*H3(u/10.0);
					
					// print the interpolated points
					for (k=0; k<3; k++)
						Color[k] = (1-fire[no].particle[i].col[k])*0.49 + fire[no].particle[i].col[k];

					glColor4fv(Color);
					glVertex2fv(interpolated_pt);
				}

				// print the current point
				for (k=0; k<3; k++)
					Color[k] = (1-fire[no].particle[i].col[k])*0.4 + fire[no].particle[i].col[k];

				glColor4fv(Color);
				glVertex2fv(fire[no].particle[i].pos);
			glEnd();
		}
	}	

	

}

///////////////////////////////////////////////////////////////////////////
// Initialize OpenGL State Setting for blending & texture mapping /////////
///////////////////////////////////////////////////////////////////////////
static void Init(void)
{

  /* one-time init (clearColor, set palette, etc) */
  glClearIndex(0.0);
  glShadeModel(GL_FLAT);

  if(_background) 
  {
	  glPixelStorei(GL_PACK_ALIGNMENT, 4);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, _teximageWidth, 
	  _teximageHeight, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *)_teximage);
  }  

  // enable the alpha channel
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  _timeout = rand()%20;
}

///////////////////////////////////////////////////////////////////////////
// Timer function to update attributes of firework ////////////////////////
///////////////////////////////////////////////////////////////////////////
void Timer(void)
{
	int no;
	float pos[2];

	/// Initialize one firework when timeout occurs ///
	/// new _timeout is randomly generated			///
	if(_displayMode == RANDOM) {
		if(_timeout == 0 && !_full) {
			pos[0] = ((float)rand()/RAND_MAX-0.5); 
			pos[1] = ((float)rand()/RAND_MAX*0.5);
			InitFirework(_tail, pos);
			_tail++;

//			printf("%d ", _tail);
			if(_tail == MAX_FIREWORK) _tail=0;
			if(_tail == _head) _full = TRUE;
			if(_empty) _empty = FALSE;
			_timeout = 1 + rand()%20;
		}
		else _timeout--;
	}
	//printf("h:%d, t:%d, f:%d, e:%d\n", _head, _tail, _full, _empty);

	/// Call UpdateFirework for each firework in the queue ///
	/// The parameters of the queue is updated			   ///
	no = _head;
	while(no!=_tail || _full ) {
		if(!fire[no].death) {
			fire[no].t++;
			UpdateFirework(no); // Update Firework "fire[no]"
			if(fire[no].t > 40) 
				fire[no].death = TRUE;
		}		
		if(fire[no].death) {
			fire[no].death = FALSE;
			fire[no].t = 0;
			_head++;
			if(_head == MAX_FIREWORK) _head=0;
			if(_head == _tail) _empty = TRUE;
			if(_full) _full = FALSE;
		}
		no++;
		if(no==MAX_FIREWORK) no=0;
	}
	glutPostRedisplay();
}


///////////////////////////////////////////////////////////////////////////
// Reshape Callback Function //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Reshape(int width, int height)
{
   glViewport(0, 0, (GLint)width, (GLint)height);
   _winwidth = width;
   _winheight = height;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
   glMatrixMode(GL_MODELVIEW);
}

///////////////////////////////////////////////////////////////////////////
// Display Callback Function //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Display(void)
{
	int no;

	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT );

	// Draw Background //
    if(_background) {
	    glEnable(GL_TEXTURE_2D);
		glBegin(GL_POLYGON);
		  glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,-1.0, -0.0);
		  glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,-1.0, -0.0);
		  glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, 1.0, -0.0);
		  glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, 1.0, -0.0);

		  // wrote by me
		  glColor4f(1.0, 1.0, 1.0, bg_alpha);

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	
	// Draw All Fireworks //
	glPushMatrix();
	glPushAttrib(GL_CURRENT_BIT);
	no = _head;
	while(no != _tail || _full ) {
		if(!fire[no].death) 
			DrawFirework(no); // Draw Firework "fire[no]"
		no++;
		if(no==MAX_FIREWORK) no=0;
	}

	
	// alter the alpha value for the background 
	if (bg_alpha > ((_tail-_head)*0.1+0.5)+EPSILON)
		bg_alpha = ((bg_alpha-0.05) < 0.5 ? 0.5 : (bg_alpha-0.1)); 
	else if (bg_alpha < ((_tail-_head)*0.1+0.5)-EPSILON)
		bg_alpha = ((bg_alpha+0.05) > 1.0 ? 1.0 : (bg_alpha+0.1)); 
	
	glPopAttrib();
	glPopMatrix();
	glFlush();

	glutSwapBuffers();
}

///////////////////////////////////////////////////////////////////////////
// Keyborad Callback Function /////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Keyboard(unsigned char key, int x, int y)
{
	float pos[2];

	switch (key) {
	case 27: 
		free(_teximage);
		exit(0);

	case 't':
		_background = !_background; break;

	case 'r':
		_displayMode = RANDOM; break;

	case 'm':
		_displayMode = MOUSE; break;

	case 'k':
		_displayMode = KEY; break;

	case 'f':
		if(_displayMode == KEY) {
			pos[0] = 0; 
			pos[1] = 0;
			InitFirework(_tail, pos);
			_tail++;
			if(_tail == MAX_FIREWORK) _tail=0;
			if(_tail == _head) _full = TRUE;
			if(_empty) _empty = FALSE;
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// Mouse Click Callback Function //////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Mouse(int button, int state, int ix, int iy)
{
  float x, y, pos[2];

  TransformXY(ix, iy, &x, &y);
  if(state == GLUT_DOWN && !_full && _displayMode == MOUSE) {
	pos[0] = x; pos[1] = y;
	InitFirework(_tail, pos);
	_tail++;
	if(_tail == MAX_FIREWORK) _tail=0;
	if(_tail == _head) _full = TRUE;
	if(_empty) _empty = FALSE;
	glutPostRedisplay();
  }
}

///////////////////////////////////////////////////////////////////////////
// Special Key-stroke Callback Function ///////////////////////////////////
///////////////////////////////////////////////////////////////////////////
void Special(int key, int x, int y)
{
	if(key == GLUT_KEY_F1) {
		_ScreenMode ++;
		if(_ScreenMode > 3) _ScreenMode=0;
		if(_ScreenMode == 0)
			glutReshapeWindow(200, 200);
		else if(_ScreenMode == 1)
			glutReshapeWindow(400, 400);
		else if(_ScreenMode == 2)
			glutReshapeWindow(600, 600);
		else if(_ScreenMode == 3)
			glutFullScreen();
	}
}

///////////////////////////////////////////////////////////////////////////
// Main //////////////////// //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	glutInit(&argc, argv);  
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutCreateWindow("Firework Display");
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutSpecialFunc(Special);
	glutKeyboardFunc(Keyboard);
	glutIdleFunc(Timer);

	if(argc!=2) {
		_background = FALSE;
	}
	else { 
		getPPMFile(argv[1]);
		_background = TRUE;
	}
	Init();
	glutMainLoop();
	return 0;
}

