/*
 *		This Code Was Created By Jeff Molofee 2000
 *		A HUGE Thanks To Fredric Echols For Cleaning Up
 *		And Optimizing This Code, Making It More Flexible!
 *		If You've Found This Code Useful, Please Let Me Know.
 *		Visit My Site At nehe.gamedev.net
 */

#include <windows.h>		// Header File For Windows
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <stdio.h>
#include <math.h>


float points[45][45][3];
int wiggle_count=0;
GLfloat hold;
HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application
GLfloat xrot;
GLfloat yrot;
GLfloat zrot;
GLuint texture[1];

bool	keys[256];			// Array Used For The Keyboard Routine
bool	active=TRUE;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=TRUE;	// Fullscreen Flag Set To Fullscreen Mode By Default

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc
AUX_RGBImageRec *LoadBMP(char *Filename)
{
	FILE *File=NULL;
	if(!Filename)
	{
		return NULL;
	}
	File=fopen(Filename,"r");
	if(File)
	{
		fclose(File);
		return auxDIBImageLoad(Filename);
	}
	return NULL;
}
int LoadGLTextures()
{
	int Status=FALSE;
	AUX_RGBImageRec *TextureImage[1];
	memset(TextureImage,0,sizeof(void*)*1);
	if(TextureImage[0]=LoadBMP("C:\\Tim.bmp"))
	{
		Status=TRUE;
		glGenTextures(1,&texture[0]);
		glBindTexture(GL_TEXTURE_2D,texture[0]);
		glTexImage2D(GL_TEXTURE_2D,0,3,TextureImage[0]->sizeX,TextureImage[0]->sizeY,0,GL_RGB,GL_UNSIGNED_BYTE,TextureImage[0]->data);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	if(TextureImage[0])
	{
		if(TextureImage[0]->data)
		{
			free(TextureImage[0]->data);
		}
		free(TextureImage[0]);
	}
	return Status;
}

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

int InitGL(GLvoid)									// All Setup For OpenGL Goes Here
{
	if(!LoadGLTextures())
	{
		return FALSE;
	}
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f); 								// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glPolygonMode(GL_BACK,GL_FILL);
	glPolygonMode(GL_FRONT,GL_LINE);
	for(int x=0;x<45;x++)
	{
		for(int y=0;y<45;y++)
		{
			points[x][y][0]=float((x/5.0f)-4.5f);
			points[x][y][1]=float((y/5.0f)-4.5f);
			points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));
		}
	}
	return TRUE;										// Initialization Went OK
}

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	int x,y;
	float float_x,float_y,float_xb,float_yb;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f,0.0f,-12.0f);
	glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	glRotatef(zrot,0.0f,0.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D,texture[0]);
	glBegin(GL_QUADS);
	for(x=0;x<44;x++)
	{
		for(y=0;y<44;y++)
		{
			float_x=float(x)/44.0f;
			float_y=float(y)/44.0f;
			float_xb=float(x+1)/44.0f;
			float_yb=float(y+1)/44.0f;
			glTexCoord2f(float_x,float_y);
			glVertex3f(points[x][y][0],points[x][y][1],points[x][y][2]);
			glTexCoord2f(float_x,float_yb);
			glVertex3f(points[x][y+1][0],points[x][y+1][1],points[x][y+1][2]);
			glTexCoord2f(float_xb,float_yb);
			glVertex3f(points[x+1][y+1][0],points[x+1][y+1][1],points[x+1][y+1][2]);
			glTexCoord2f(float_xb,float_y);
			glVertex3f(points[x+1][y][0],points[x+1][y][1],points[x+1][y][2]);
		}
	}
	glEnd();
	if(wiggle_count==2)
	{
		for(y=0;y<45;y++)
		{
			hold=points[0][y][2];
			for(x=0;x<44;x++)
			{
				points[x][y][2]=points[x+1][y][2];

			}
			points[44][y][2]=hold;
		}
		wiggle_count=0;
	}
	wiggle_count++;
	xrot+=0.3f;
	yrot+=0.2f;
	zrot+=0.4f;
	return TRUE;

/*
	glTexCoord2f(0.0f,0.0f);glVertex3f(-1.0f,-1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,-1.0f,1.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,1.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f,1.0f,1.0f);
	
	glTexCoord2f(1.0f,0.0f);glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(-1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(1.0f,-1.0f,-1.0f);

	glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(-1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,1.0f,1.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,-1.0f);

	glTexCoord2f(1.0f,1.0f);glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(1.0f,-1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(1.0f,-1.0f,1.0f);
	glTexCoord2f(1.0f,0.0f);glVertex3f(-1.0f,-1.0f,1.0f);

	glTexCoord2f(1.0f,0.0f);glVertex3f(1.0f,-1.0f,-1.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(1.0f,1.0f,-1.0f);
	glTexCoord2f(0.0f,0.0f);glVertex3f(1.0f,-1.0f,1.0f);

	glTexCoord2f(0.0f,0.0f);glVertex3f(-1.0f,-1.0f,-1.0f);
	glTexCoord2f(1.0f,0.0f);glVertex3f(-1.0f,-1.0f,1.0f);
	glTexCoord2f(1.0f,1.0f);glVertex3f(-1.0f,1.0f,1.0f);
	glTexCoord2f(0.0f,1.0f);glVertex3f(-1.0f,1.0f,-1.0f);

	glEnd();
	
	xrot+=0.3f;
	yrot+=0.2f;
	zrot+=0.4f;
	return true;
	GLfloat rtri;
	GLfloat rquad;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	glLoadIdentity();// Reset The Current Modelview Matrix
	glTranslatef(-1.5f,0.0f,-6.0f);
	glRotatef(rtri,0.0f,0.0f,1.0f);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f,0.0f,0.0f);
	glVertex3f(0.0f,1.0f,0.0f);
	glColor3f(0.0f,1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glColor3f(0.0f,0.0f,1.0f);
	glVertex3f(1.0f,-1.0f,0.0f);
	glEnd();
	glLoadIdentity();
	glTranslatef(1.5f,0.0f,-6.0f);
	glRotatef(rquad,1.0f,0.0f,0.0f);
	glColor3f(0.5f,0.5f,1.0f);
	glBegin(GL_QUADS);
	glVertex3f(-1.0f,1.0f,0.0f);
	glVertex3f(1.0f,1.0f,0.0f);
	glVertex3f(1.0f,-1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glVertex3f(-1.0f,-1.0f,0.0f);
	glEnd();
	rtri+=0.2f;
	rquad-=0.15f;
	return TRUE;// Everything Went OK
*/
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIc
