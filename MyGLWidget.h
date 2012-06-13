/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <qgl.h>
#include "glm-source/glm.h"
#include <string>
#include <vector>
#include <qtimer.h>
#include <fstream>
#include <math.h>
#include <iostream>
#include <qtoolbutton.h>

class MyGLWidget : public QGLWidget
{
	Q_OBJECT
public:
    MyGLWidget( QWidget *parent, QToolButton*,const char *name=0);
	~MyGLWidget();
	void initializeGL();
    void resizeGL(int width, int height);
    void paintGL();
	float WORDDISTPERFRAME;
	float INITIALWORDDISTPERFRAME;
	float DISTINCREMENT;

public slots:
			void play(void);
			void about(void);
			void idle(void);
private:
	void readFiles();

	GLMmodel* pmodel;

	QToolButton* playb; //so can enable and disable it
    
    struct word
    {
		std::string s;
    	float across;
    };
    
	std::vector<word> words;
    int wordcount;
//	std::ifstream* in;

    float worddistance;
    int needed;
	bool drawingword;
    bool pastcenter;
    int lives;
    
    bool dodging;
    bool up;
    float moveplane;
    bool fucked;
	bool drawinglives;
    
    bool firing;
    float mx;
    float my;
    float mz;
    float tz; //target z value for missile
    float ix,iy,iz; //increment per frame
    float tmx,tmy,tmz; //translate missile x

	bool won;
	bool writewon;
	bool lost;

	void drawf16();
	void drawstars();
	void nextword();
	void drawText();
	void drawLives();
	void drawmissile();
	void firemissile();

	void hideEvent(QHideEvent*);
	void showEvent(QShowEvent*);
	void keyPressEvent(QKeyEvent* e);

	QTimer* gltimer;

	//BEGIN Copyright Jeff Molofee 2000
	typedef struct												// Create A Structure
	{
		GLubyte	*imageData;										// Image Data (Up To 32 Bits)
		GLuint	bpp;											// Image Color Depth In Bits Per Pixel.
		GLuint	width;											// Image Width
		GLuint	height;											// Image Height
		GLuint	texID;											// Texture ID Used To Select A Texture
	} TextureImage;												// Structure Name

	TextureImage textures[1];									// Storage For One Texture
	//END Copyright Jeff Molofee 2000

	void LoadTGA(TextureImage *texture, char *filename);
//	bool textureloaded;

	struct star
	{
		float x;
		float y;
		float z;
	};
#define NUMSTARS 200

	star stars[NUMSTARS];
//	star teststar;
	void missilevertices();
	
	class Vector
	{
	public:
      float x,y,z;
      Vector(float a,float b,float c)
      {
            x=a;y=b;z=c;
      }
	  Vector()
	  {
		  x = y = z = 0;
	  }
      void normalize()
      {
        float len;
        len = 1.0 / sqrt(x*x + y*y + z*z);
        x *= len;
        y *= len;
        z *= len;
      }
	};
	float angle;
	float iangle;
	Vector cross;


	//EXPLOSION AND PARTICLE CODE BASED ON NEHE LESSON 19 http://nehe.gamedev.net
	class Particle
	{
	public:
		bool available;				
		float life;				
		float fade;					
		float r,g,b;
		float x,y,z; //position
		float xi,yi,zi; //speeds
		int slowdown;
		bool starting;
	//	int startingcount;
		//static float wdpf;
		Particle()
		{	
			slowdown = 500.0;
			r = (float)(rand()%75+180)/255.0;
			g = (float)(rand()%80)/255.0;
			b = 0;
			available = true;
			life = 1.0;
			fade = (rand()%5)/100 + 0.01;
//			fade = 0.001;
			xi = rand()%500-250;
			yi = rand()%500-250;
			zi = rand()%500-250;
		}
		void assignPosition(float xp,float yp,float zp,float width,float height,float depth)
		{
		  if(available)
		  {
			  starting = true;
			 // startingcount = 0;

			  available = false;

			if(depth==0)
				depth = 1;
			if(height==0)
				height = 1;
			if(width ==0)
				width = 1;
			x = (float)(rand()%(int)(width*100))/100.0 + xp;
			y = (float)(rand()%(int)(height*100))/100.0 + yp;
			z = (float)(rand()%(int)(depth*100))/100.0 + zp;
			life = 1.0;
		  }
		}
		void nextframe(float wdpf,bool lost)
		{
		  if(!starting)
		  {
			x += xi/(slowdown);	
			y += yi/(slowdown);	
			z += zi/(slowdown);	
			if(!lost)
				z+=wdpf;
			life -= fade;
			if(z>=0 || life<0.05)
				available = true; 
		  }
		  else
		  {
			 // startingcount++;
			  //if(startingcount>2)
				  starting = false;
		  }
		}

	};
#define NUMPARTICLES 1000
	Particle particles[NUMPARTICLES];
	//vector<Particle> particles;
	
	void explosion(float x,float y,float z,float width,float height,float depth);
	void drawexploding();
	//bool exploding;
	bool dontbother;
	float explosiondepth;
	bool error;

	void setwinvars();
	int rotatewon;
	int countwon;
	float aforwardwon;

};
