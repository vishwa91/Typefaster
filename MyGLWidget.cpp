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
 *
 *  Note that a portion of this code (it has been commented as such) does not
 *  fall under the above license and is Copyright Jeff Molofee 2000
 */

#include "MyGLWidget.h"
#include "glut.h" //for the stroke font, should use gltext.sourceforge.net
//or oglft.sourceforge.net so can use unicode
#include <qmessagebox.h>
#include <iostream>
#include <string>
#include <vector>
#include <qfile.h>
#include <qtextstream.h>
#include <fstream>
using namespace std;

extern bool MULTI;
extern bool TEACHERSUPPORT;
extern bool ISTEACHER;
extern QString USERNAME;
extern QString TEACHERSNAME;

extern bool READGAMEFILES;

void MyGLWidget::LoadTGA(TextureImage *texture, char *filename)
{
	//BEGIN Copyright Jeff Molofee 2000 with slight modifications
	GLubyte		TGAheader[12]={0,0,2,0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
	GLubyte		TGAcompare[12];								// Used To Compare TGA Header
	GLubyte		header[6];									// First 6 Useful Bytes From The Header
	GLuint		bytesPerPixel;								// Holds Number Of Bytes Per Pixel Used In The TGA File
	GLuint		imageSize;									// Used To Store The Image Size When Setting Aside Ram
	GLuint		temp;										// Temporary Variable
	GLuint		type=GL_RGBA;								// Set The Default GL Mode To RBGA (32 BPP)

	FILE *file = fopen(filename, "rb");						// Open The TGA File

	if(	file==NULL ||										// Does File Even Exist?
		fread(TGAcompare,1,sizeof(TGAcompare),file)!=sizeof(TGAcompare) ||	// Are There 12 Bytes To Read?
		memcmp(TGAheader,TGAcompare,sizeof(TGAheader))!=0				||	// Does The Header Match What We Want?
		fread(header,1,sizeof(header),file)!=sizeof(header))				// If So Read Next 6 Header Bytes
	{
		if (file == NULL)
		{						// Did The File Even Exist? *Added Jim Strong*
			error = true;									// Return False
			cout << "texture file not found" << endl;
			return;
		}
		else
		{
			fclose(file);									// If Anything Failed, Close The File
			error = true;
			cout << "texture file not right" << endl;
			return;									// Return False
		}
	}

	texture->width  = header[1] * 256 + header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	texture->height = header[3] * 256 + header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
    
 	if(	texture->width	<=0	||								// Is The Width Less Than Or Equal To Zero
		texture->height	<=0	||								// Is The Height Less Than Or Equal To Zero
		(header[4]!=24 && header[4]!=32))					// Is The TGA 24 or 32 Bit?
	{
		fclose(file);										// If Anything Failed, Close The File
		error = true;
		cout << "texture problems" << endl;
		return;										// Return False
	}

	texture->bpp	= header[4];							// Grab The TGA's Bits Per Pixel (24 or 32)
	bytesPerPixel	= texture->bpp/8;						// Divide By 8 To Get The Bytes Per Pixel
	imageSize		= texture->width*texture->height*bytesPerPixel;	// Calculate The Memory Required For The TGA Data

	texture->imageData=(GLubyte *)malloc(imageSize);		// Reserve Memory To Hold The TGA Data

	if(	texture->imageData==NULL ||							// Does The Storage Memory Exist?
		fread(texture->imageData, 1, imageSize, file)!=imageSize)	// Does The Image Size Match The Memory Reserved?
	{
		if(texture->imageData!=NULL)						// Was Image Data Loaded
			free(texture->imageData);						// If So, Release The Image Data

		fclose(file);										// Close The File
		error = true;
		cout << "more texture problems" << endl;
		return;										// Return False
	}

	for(GLuint i=0; i<int(imageSize); i+=bytesPerPixel)		// Loop Through The Image Data
	{														// Swaps The 1st And 3rd Bytes ('R'ed and 'B'lue)
		temp=texture->imageData[i];							// Temporarily Store The Value At Image Data 'i'
		texture->imageData[i] = texture->imageData[i + 2];	// Set The 1st Byte To The Value Of The 3rd Byte
		texture->imageData[i + 2] = temp;					// Set The 3rd Byte To The Value In 'temp' (1st Byte Value)
	}

	fclose (file);											// Close The File

	// Build A Texture From The Data

	glGenTextures(1, &texture[0].texID);					// Generate OpenGL texture IDs

	glBindTexture(GL_TEXTURE_2D, texture[0].texID);			// Bind Our Texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	// Linear Filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	// Linear Filtered
	
	if (texture[0].bpp==24)									// Was The TGA 24 Bits
	{
		type=GL_RGB;										// If So Set The 'type' To GL_RGB
	}

	glTexImage2D(GL_TEXTURE_2D, 0, type, texture[0].width, texture[0].height, 0, type, GL_UNSIGNED_BYTE, texture[0].imageData);

	//return true;	
	//END Copyright Jeff Molofee 2000
}

void MyGLWidget::readFiles()
{
	QString gameloc = "defaultuser/game";
	if(MULTI)
	{
		if(TEACHERSUPPORT)
		{	
			if(ISTEACHER)
				gameloc = "users/" + USERNAME + "/game";//teacher gets her own medicine
			else
			{
				if(TEACHERSNAME!=" ")
					gameloc = "users/" + TEACHERSNAME + "/game";
			}
		}
		else
			gameloc = "users/" + USERNAME + "/game";

	}
	int start = 10;
	int end = 90;
	QFile sfile(gameloc+"/settings.txt");
	if(sfile.open(IO_ReadOnly))
	{
		QTextStream sstream(&sfile);
		QString line;
		if((line = sstream.readLine())!=0)
		{
			bool ok;
			start = line.toInt(&ok);
			if(!ok || start<1 || start>100)
				start = 10;
			if((line = sstream.readLine())!=0)
			{
				end = line.toInt(&ok);
				if(!ok || end<1 || end>100)
					end = 90;
			}
		}
		sfile.close();
	}
	words.clear();

	QFile wfile(gameloc+"/words.txt");
	if(wfile.open(IO_ReadOnly))
	{
		QTextStream wstream(&wfile);
		QString line;
		while((line=wstream.readLine())!=0)
		{
			if(line.length()>0)
			{
				word w;
				w.s = line.latin1();
				int width = 0;
				for (int i=0;i<w.s.length();i++)
					width += glutStrokeWidth(GLUT_STROKE_ROMAN,w.s[i]);
				w.across = -0.025*width;
				words.push_back(w);
			}
		}
		wfile.close();
	}
	
	INITIALWORDDISTPERFRAME = 0.25 + start*0.025;
	//now (endspeed-startspeed)/words gives the distincrement
	DISTINCREMENT = ((0.25+end*0.025)-INITIALWORDDISTPERFRAME)/(float)words.size();

}
MyGLWidget::MyGLWidget( QWidget *parent, QToolButton* p,const char *name ):QGLWidget(parent,name)
{
	setFocusPolicy(QWidget::WheelFocus);
	setFocus();

	playb = p;

	readFiles();

	pmodel = 0;

	gltimer = new QTimer(this);
	connect( gltimer, SIGNAL(timeout()),this, SLOT(idle()) );
	error = false;

	if(words.size()>0)
		play();
	else
	{
		cout << "no words in words.txt (or no words.txt)" << endl;
		error=true;
	}
      
}
void MyGLWidget::keyPressEvent(QKeyEvent* k)
{
	
	if(!won && !lost && !fucked && !dodging && !firing && k->text().length()>0 && wordcount<words.size())
	{
		if(needed < words[wordcount].s.length())
		{
			if(k->text().at(0).latin1() == words[wordcount].s[needed])
			{
				needed++;
				if(needed == words[wordcount].s.length())
					firemissile();

			}
		}
	}
	
}
void MyGLWidget::initializeGL()
{
 	glEnable(GL_NORMALIZE);

	glEnable(GL_LINE_SMOOTH);
	//glLineWidth(2.0);

    glClearColor( 0.0, 0.0, 0.0, 0.0 ); //what should the alpha value be
    glClearDepth( 1.0 );
   
	//GLfloat pos[] = { -25.0, 25.0, 0.0, 1.0 };
	GLfloat pos[] = { -25.0, 15, 0.0, 0.0 };
	
	glEnable( GL_LIGHT0 );
    glEnable( GL_DEPTH_TEST );

	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0,GL_POSITION,pos);

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE);

//	glShadeModel(GL_SMOOTH);
	
	LoadTGA(&textures[0],"game-data/Particle.tga");
	if (error)				
	{
		cout << "could not load texture " << endl;
	//	textureloaded = false;
	}
	else
	{
		for(int i=0;i<NUMSTARS;i++)
		{
			stars[i].x = rand()%290 - 145;
			stars[i].y = rand()%180 - 120;
			stars[i].z = rand()%185 - 200;
		}
		
	}

}

void MyGLWidget::resizeGL(int width, int height)
{
 	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(60.0f,(float)width/(float)height,0.1f,200.0f); //sets aspect correctly

	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();

	glTranslatef(0,-7,0);
	//glRotatef(10,1,0,0);
//	glTranslatef(0,0,35);
}

void MyGLWidget::paintGL()
{
	//static int arbcount = 0;

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	if(writewon)
	{
		int width = 0;
		string s = "You Succeeded";
		int i;
		for (i=0;i<s.length();i++)
			width += glutStrokeWidth(GLUT_STROKE_ROMAN,s[i]);
		float across = -0.025*width;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
   
	//	glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);

			glTranslatef(across,0,-40);
			glScalef(0.05,0.05,0.05);
			glTranslatef(0,10,0);
		//	glRotatef(-10,1,0,0);

			glColor3f(1,1,1);
			
			for(i=0;i<s.length();i++)
				glutStrokeCharacter(GLUT_STROKE_ROMAN,s.c_str()[i]);

		//glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);

		glPopMatrix();
		gltimer->stop();
		playb->setDisabled(false);
	}
	else
	{
		if(lost && dontbother) //ie explosion has faded
		{
	//		arbcount = 0;
		int width = 0;
		string s = "Type Faster";
		int i;
		for (i=0;i<s.length();i++)
			width += glutStrokeWidth(GLUT_STROKE_ROMAN,s[i]);
		float across = -0.025*width;

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
   
		//glEnable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHTING);

			glTranslatef(across,0,-40);
			glScalef(0.05,0.05,0.05);
			glTranslatef(0,10,0);
		//	glRotatef(-10,1,0,0);

			glColor3f(1,1,1);
			
			for(i=0;i<s.length();i++)
				glutStrokeCharacter(GLUT_STROKE_ROMAN,s.c_str()[i]);

		//glDisable(GL_COLOR_MATERIAL);
		glEnable(GL_LIGHTING);

		glPopMatrix();
			gltimer->stop();
			playb->setDisabled(false);
		}
		else
		{
		  if(!error)
		  {
			drawstars();
			if(!dontbother)
				drawexploding();
			if(!lost)
				drawf16();
			if(!error) //drawf16 might have set error true
			{
				if(drawingword)
					drawText();
				if(drawinglives)
					drawLives();
				if(firing)
					drawmissile();
			}
		  }
		}
	}
}
void MyGLWidget::drawexploding()
{
		glDisable(GL_LIGHTING);

		glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
    	glEnable(GL_BLEND);
    	glDisable(GL_DEPTH_TEST);
    	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    	glEnable(GL_TEXTURE_2D);
    	glBindTexture(GL_TEXTURE_2D, textures[0].texID);
    
		bool test = false;

    		for(int i=0;i<NUMPARTICLES;i++)
    		{
			  if(particles[i].available==false)
			  {
				glColor4f(particles[i].r,particles[i].g,particles[i].b,particles[i].life);

    			glBegin(GL_TRIANGLE_STRIP);
    			 glTexCoord2d(1,1); glVertex3f(particles[i].x+1,particles[i].y+1,particles[i].z); // Top Right
    			 glTexCoord2d(0,1); glVertex3f(particles[i].x,particles[i].y+1,particles[i].z); // Top Left
    			 glTexCoord2d(1,0); glVertex3f(particles[i].x+1,particles[i].y,particles[i].z); // Bottom Right
    			 glTexCoord2d(0,0); glVertex3f(particles[i].x,particles[i].y,particles[i].z); // Bottom Left
    			glEnd();
				test = true;
			  }
    		}
    	
		if(test==false)
			dontbother = true;

    	glDisable(GL_TEXTURE_2D);
    	glEnable(GL_DEPTH_TEST);
    	glDisable(GL_BLEND);
    	glPopMatrix();

		glEnable(GL_LIGHTING);
}
void MyGLWidget::drawstars()
{
	//glEnable(GL_COLOR_MATERIAL);
	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
	glColor4f(0.7,0.7,0.7,1);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textures[0].texID);

		for(int i=0;i<NUMSTARS;i++)
		{
			if(!lost)
				stars[i].z += WORDDISTPERFRAME;
			if(stars[i].z >= 0)
				stars[i].z = -200;
			glBegin(GL_TRIANGLE_STRIP);
			 glTexCoord2d(1,1); glVertex3f(stars[i].x+1,stars[i].y+1,stars[i].z); // Top Right
			 glTexCoord2d(0,1); glVertex3f(stars[i].x,stars[i].y+1,stars[i].z); // Top Left
			 glTexCoord2d(1,0); glVertex3f(stars[i].x+1,stars[i].y,stars[i].z); // Bottom Right
			 glTexCoord2d(0,0); glVertex3f(stars[i].x,stars[i].y,stars[i].z); // Bottom Left
			glEnd();
		}
	

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glPopMatrix();
	//glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
}
void MyGLWidget::setwinvars()
{
	rotatewon = 0;
	countwon = 0;
	aforwardwon = 1;
}
void MyGLWidget::drawf16()
{
	
    if (!pmodel) 
	{
		ifstream test("game-data/f-16.obj");
		ifstream test2("game-data/f-16.mtl");
		if(!test || !test2)
		{
			error = true;
			cout << "f-16.obj or f-16.mtl not there" << endl;
			return;
		}
		else
		{
			test.close();
			test2.close();
			pmodel = glmReadOBJ("game-data/f-16.obj");
			if (!pmodel) 
			{
				error = true;
				cout << "f-16.obj exists but glmReadOBJ failed" << endl;
				return;
			}
			else
			{
				glmUnitize(pmodel);
				glmFacetNormals(pmodel);
				glmVertexNormals(pmodel, 90.0);
			}
		}
    }

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

		if(up)
			glTranslatef(0,-moveplane,-25);
		else
			glTranslatef(0,moveplane,-25);
		
		if(won)
		{
		
			if(rotatewon==180 && countwon<20)
				countwon++;
			else
				rotatewon += 5;

			if(rotatewon >= 360)
			{
				glTranslatef(0,0,-aforwardwon);
				if(aforwardwon < 200)
				{
					aforwardwon *= 1.3;
				}
				else
					writewon = true;
			}
			else
				glRotatef(rotatewon,0,0,1);
		}
		glRotatef(180,0.0,1.0,0.0);
		glTranslatef(0,2,0);
	    glScalef(10.0,10.0,10.0);
		//
		glmDraw(pmodel,GLM_SMOOTH |  GLM_MATERIAL);
	glPopMatrix();

}

void MyGLWidget::play()
{
	playb->setDisabled(true);

	if(READGAMEFILES)
		readFiles();

	WORDDISTPERFRAME = INITIALWORDDISTPERFRAME;
	worddistance = 200;
	drawingword = true;
	drawinglives = true;
	needed = 0;
	lives = 3;
	dodging = false;
	up = true;
	moveplane = 0.0;
	fucked = false;
	firing = false;
	mx = -6.3; //right missile co-ords (roughly)
	my = 0.4;
	mz = -23.4;
	won=false;
	writewon = false;
	dontbother = true; //only bother about explosion once there has been one
	pastcenter = true; //if not pastcenter, will do nextword() once past (or explosion faded)
	lost = false;

	for(int i=0;i<NUMPARTICLES;i++)
		particles[i].available = true;

	wordcount = 0;

	if(!error)
		gltimer->start( 20);  //call idle every 20ms
	else
		QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("game-data files not available"));
}
void MyGLWidget::hideEvent(QHideEvent*)
{
	gltimer->stop();
}
void MyGLWidget::showEvent(QShowEvent*)
{
	play();
}
void MyGLWidget::explosion(float x,float y,float z,float width,float height,float depth)
{
	dontbother = false;
	explosiondepth = z;
	drawingword = false;
	pastcenter = false;

	for(int i=0;i<NUMPARTICLES;i++)
	{
		particles[i].assignPosition(x,y,z,width,height,depth);//also gives life
	}
}
void MyGLWidget::idle( void )
{
	static int count = 0;
	static float cosx = 0;
	
  if(!won)
  {
	  if(drawingword)
		worddistance -= WORDDISTPERFRAME;
	
	if(dodging)
	{
		moveplane = 3*cos(count*cosx)-3;
		count++;
		if(worddistance<=0)
		{
			dodging = false;
			moveplane = 0;
			nextword();
		}
	}

	if(worddistance<=30 && fucked && !lost)
	{
		explosion(-6.3,0,-30,12.6,2,10);
		lost = true;
	}
	
	if(drawingword && worddistance <= 40.0 && !firing && !dodging) //&& needed < words[words.size()].s.length()
	{
		if(lives>0)
		{
			dodging = true;
			up = !up;
			lives--;
			count = 0;
			int frames = worddistance/WORDDISTPERFRAME;
			cosx = 2*3.1415926535/frames;
		}
		else
			fucked = true;
	}
	if(firing)
	{
		tmx += ix;
		tmy += iy;
		tmz += iz;
		if(tmz <= tz-mz) //shooting down negative z axis
		{
			firing = false;
			explosion(words[wordcount].across,0,-worddistance,-2*words[wordcount].across,4,0);
		//	float width = 1;
		//	explosion(-width/2.0,0,-worddistance,width,width,0);
		}
	}
  }

    if(!dontbother)
	for(int i=0;i<NUMPARTICLES;i++)
		  particles[i].nextframe(WORDDISTPERFRAME,lost);

	if(!pastcenter && !lost)
	{
		explosiondepth += WORDDISTPERFRAME; //center of explosion
		if(explosiondepth >=0 || dontbother)
		{
			pastcenter = true;
			nextword();
		}
	}

	glDraw();
	//updateGL();
}

void MyGLWidget::nextword(void)
{
	WORDDISTPERFRAME += DISTINCREMENT;
	if(wordcount < words.size()-1)
	{
		worddistance = 200;
		needed = 0;
		wordcount++;
		drawingword = true;
	}
	else
	{
		drawingword = false;
		drawinglives = false;
		setwinvars();
		won = true;
	}
}
void MyGLWidget::drawText()
{
  if(drawingword && wordcount < words.size()) //not nec
  {
	int i;
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
   glDisable(GL_LIGHTING);
//	glEnable(GL_COLOR_MATERIAL);

    glTranslatef(words[wordcount].across,0,-worddistance);
//	glRotatef(-10,1,0,0);
    glScalef(0.05,0.05,0.05);
    for (i=0;i<words[wordcount].s.length();i++)
    {
	   if(i < needed)
			glColor3f(0,0,0.78);
	   else
			glColor3f(0.6,0,0);
       glutStrokeCharacter(GLUT_STROKE_ROMAN,words[wordcount].s[i]);
    }

//    glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
    glPopMatrix();
  }
}
void MyGLWidget::drawLives(void)
{
  glColor3f(1.0,1.0,1.0);
 // glEnable(GL_COLOR_MATERIAL);
  glDisable(GL_LIGHTING);
  glMatrixMode(GL_MODELVIEW);
  for(int i=0;i<lives;i++)
  { 
    glPushMatrix();
   
    //glTranslatef(-20+4*i,19.0,-35.0);
	glTranslatef(-22+4*i,22.0,-35.0);
	//glRotatef(-10,1,0,0);
	glScalef(0.5,0.5,0.5);
   
	glBegin(GL_LINE_STRIP);
		glVertex2f(-0.5,0.0);
		glVertex2f(-2.5,-2.0);
		glVertex2f(-1.5,-2.0);
		glVertex2f(0.0,-0.5);
		glVertex2f(1.5,-2.0);
		glVertex2f(2.5,-2.0);
		glVertex2f(0.5,0.0);
		glVertex2f(0.5,1.5);
		glVertex2f(2.0,0.0);
		glVertex2f(2.0,1.0);
		glVertex2f(0.5,2.5);
		glVertex2f(0.35,2.5);

		glVertex2f(0.85,3.0);
		glVertex2f(0.85,3.7);
		glVertex2f(0.35,4.2);
		glVertex2f(-0.35,4.2);
		glVertex2f(-0.85,3.7);
		glVertex2f(-0.85,3.0);

		glVertex2f(-0.35,2.5);
		glVertex2f(-0.5,2.5);
		glVertex2f(-2.0,1.0);
		glVertex2f(-2.0,0.0);
		glVertex2f(-0.5,1.5);
		glVertex2f(-0.5,0.0);
	glEnd();
    

    glPopMatrix();
  }
 // glDisable(GL_COLOR_MATERIAL);
  glEnable(GL_LIGHTING);
 
}
void MyGLWidget::drawmissile()
{
	glDisable(GL_COLOR_MATERIAL);

/*	glEnable( GL_LIGHT0 );
    glEnable( GL_DEPTH_TEST );

	glEnable(GL_LIGHTING);

	glEnable(GL_NORMALIZE);*/

    GLfloat ambientm[]={0.4,0.4,0.4,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,ambientm);
    GLfloat diffusem[]={0.3,0.0343,0.0343,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,diffusem);
    GLfloat specularm[]={0.3,0.3,0.3,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,specularm);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,7.68);
	glMatrixMode(GL_MODELVIEW);


	glPushMatrix();
	if(mx<0) //left missile
		glTranslatef(-12.6+tmx*tmx,tmy,tmz-25);
	else
		glTranslatef(-tmx*tmx,tmy,tmz-25);
		
		glTranslatef(0,2,0);

		angle += iangle;
		glRotatef(-angle*180.0/(3.1415926535),cross.x,cross.y,cross.z);

		glRotatef(180,0.0,1.0,0.0);
	    glScalef(10.0,10.0,10.0);

		missilevertices();

	glPopMatrix();
}

#define F 1.5
void MyGLWidget::firemissile(void)
//works out the target z that we must aim for so we hit the word
//missile travels at F times the speed of the word
//{mx,my,mz} is missile's starting co-ords
//not really general, signs a problem
{
	mx = -mx; //other side of plane so shoots from alternate sides

	float amz = -mz;

	firing = true;
	float at = 1-F*F;
	float bt = -2*amz+2*F*F*worddistance;
	float c = mx*mx + (my-2)*(my-2) + amz*amz - F*F*worddistance*worddistance;
	tz = - (-bt + sqrt(bt*bt - 4*at*c))/(2*at);


	int frames = (worddistance + tz)/WORDDISTPERFRAME;
	iz = (tz-mz)/frames;
	//ix = -mx/frames;
	iy = (2-my)/frames;


	float temp;
	if(mx>0)
		temp = sqrt(mx); //I'm sure this is not physically accurate, but looks cool
	else
		temp = sqrt(-mx);
	ix = temp/(float)frames;

	tmx =  0;
	tmy = tmz = 0; //how much to translate

	//how much to rotate it by so it looks like the missile is powered from its rear end (not crabbing)
	//tx==0;ty==2;tz has been calculated

	Vector a(0.0-mx,2.0-my,tz-mz);
	Vector b(0.0,0.0,-1.0);
    a.normalize();
    //b.normalize();
	angle = 0;
    float anglet = acos(a.x*b.x+a.y*b.y+a.z*b.z);
	iangle = anglet/frames;
	
    cross.x = (a.y * b.z) - (a.z * b.y);
    cross.y = (a.z * b.x) - (a.x * b.z);
    cross.z = (a.x * b.y) - (a.y * b.x);

}
void MyGLWidget::about()
{
	QMessageBox::about(this, QMessageBox::tr("About TypeFaster F-16 Game v0.1 (in space?!)"),QMessageBox::tr("Some code (glm-source) copyright Nate Robins (used with permission)\nSome code (texture loading etc) based on Jeff Molofee's (http://nehe.gamedev.net)\nUses Glut for text rendering\nThe rest is under the GPL version2 license, copyright David le Roux"));
}
MyGLWidget::~MyGLWidget()
{
//	in->close();
//	delete in;
	delete gltimer;
}

void MyGLWidget::missilevertices()
//now you can stop reading the code unless you have a really good imagination
{
glBegin(GL_TRIANGLES);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(-0.028859,0.551837,-0.833452);
glVertex3f(-0.634066,-0.149155,-0.553025);
glNormal3f(-0.213206,0.923936,-0.317624);
glVertex3f(-0.637204,-0.149652,-0.553025);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(-0.213206,0.923936,-0.317624);
glVertex3f(-0.637204,-0.149652,-0.553025);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(0.308572,0.896601,-0.317631);
glVertex3f(-0.62782,-0.150143,-0.553025);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(0.308572,0.896601,-0.317631);
glVertex3f(-0.62782,-0.150143,-0.553025);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.872818,0.370554,-0.317614);
glVertex3f(-0.619076,-0.158889,-0.553025);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.872818,0.370554,-0.317614);
glVertex3f(-0.619076,-0.158889,-0.553025);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(-0.749322,-0.486568,-0.449186);
glVertex3f(-0.64602,-0.172618,-0.553025);
glNormal3f(-0.938631,0.04539,0.341922);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(-1,-2.1e-005,0);
glVertex3f(-0.6247,-0.153466,-0.517749);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.517749);
glNormal3f(-0.938631,0.04539,0.341922);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.517749);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.472749);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.968489,0.050906,-0.243798);
glVertex3f(-0.648453,-0.163542,-0.553025);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.968489,0.050906,-0.243798);
glVertex3f(-0.648453,-0.163542,-0.553025);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(0.130477,-0.130481,0.982828);
glVertex3f(-0.622397,-0.172914,-0.472749);
glNormal3f(0.130477,-0.130481,0.982828);
glVertex3f(-0.6247,-0.175218,-0.472749);
glNormal3f(0.130477,-0.130481,0.982828);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(0.130478,-0.13048,0.982828);
glVertex3f(-0.622397,-0.172914,-0.472749);
glNormal3f(0.130478,-0.13048,0.982828);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(0.130478,-0.13048,0.982828);
glVertex3f(-0.592589,-0.20272,-0.480663);
glNormal3f(0,0,-1);
glVertex3f(-0.642453,-0.171416,-0.257801);
glNormal3f(0.632438,-0.632462,-0.44723);
glVertex3f(-0.640377,-0.173491,-0.257801);
glNormal3f(0.235669,-0.707117,-0.666668);
glVertex3f(-0.663893,-0.197006,-0.257801);
glNormal3f(0,0,-1);
glVertex3f(-0.642453,-0.171416,-0.257801);
glNormal3f(0.235669,-0.707117,-0.666668);
glVertex3f(-0.663893,-0.197006,-0.257801);
glNormal3f(-0.632427,0.632496,-0.447197);
glVertex3f(-0.665969,-0.19493,-0.257801);
glNormal3f(0.707116,0.235672,-0.666668);
glVertex3f(-0.640314,-0.155148,-0.257801);
glNormal3f(0,0,-1);
glVertex3f(-0.64239,-0.157224,-0.257801);
glNormal3f(0,0,-1);
glVertex3f(-0.665904,-0.133708,-0.257801);
glNormal3f(0.707116,0.235672,-0.666668);
glVertex3f(-0.640314,-0.155148,-0.257801);
glNormal3f(0,0,-1);
glVertex3f(-0.665904,-0.133708,-0.257801);
glNormal3f(0.632461,0.632439,-0.447228);
glVertex3f(-0.663829,-0.131632,-0.257801);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(0.316135,0.948714,-6e-005);
glVertex3f(-0.62411,-0.17148,-0.231498);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.62411,-0.17148,-0.257801);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.62411,-0.17148,-0.257801);
glNormal3f(-0.948674,-0.316256,3.9e-005);
glVertex3f(-0.626185,-0.173556,-0.257801);
glNormal3f(0.707084,-0.70713,0);
glVertex3f(-0.602671,-0.19707,-0.257801);
glNormal3f(0.707084,-0.70713,0);
glVertex3f(-0.600595,-0.194995,-0.257801);
glNormal3f(0.876009,-0.380018,0.296975);
glVertex3f(-0.600595,-0.194995,-0.254198);
glNormal3f(0.707084,-0.70713,0);
glVertex3f(-0.602671,-0.19707,-0.257801);
glNormal3f(0.876009,-0.380018,0.296975);
glVertex3f(-0.600595,-0.194995,-0.254198);
glNormal3f(0.707084,-0.70713,0);
glVertex3f(-0.602671,-0.19707,-0.254198);
glNormal3f(-2e-006,1,-1e-006);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.640377,-0.173491,-0.231498);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.640377,-0.173491,-0.257801);
glNormal3f(-2e-006,1,-1e-006);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.640377,-0.173491,-0.257801);
glNormal3f(-0.316216,0.948687,2.1e-005);
glVertex3f(-0.642453,-0.171416,-0.257801);
glNormal3f(1,-6.3e-005,-4.9e-005);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(0.707102,-0.707112,0);
glVertex3f(-0.64239,-0.157224,-0.231498);
glNormal3f(0.707102,-0.707112,0);
glVertex3f(-0.64239,-0.157224,-0.257801);
glNormal3f(1,-6.3e-005,-4.9e-005);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(0.707102,-0.707112,0);
glVertex3f(-0.64239,-0.157224,-0.257801);
glNormal3f(0.707116,0.235672,-0.666668);
glVertex3f(-0.640314,-0.155148,-0.257801);
glNormal3f(-0.376222,0.376214,0.846712);
glVertex3f(-0.663829,-0.131632,-0.254198);
glNormal3f(-0.876025,0.379972,0.296986);
glVertex3f(-0.665904,-0.133708,-0.254198);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.376222,0.376214,0.846712);
glVertex3f(-0.663829,-0.131632,-0.254198);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.376222,0.376214,0.846712);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.968493,-0.050842,-0.243798);
glVertex3f(-0.618086,-0.165133,-0.553025);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.968493,-0.050842,-0.243798);
glVertex3f(-0.618086,-0.165133,-0.553025);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.829262,-0.459825,-0.317624);
glVertex3f(-0.619722,-0.17124,-0.553025);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.570423,-0.757452,-0.317622);
glVertex3f(-0.623702,-0.176154,-0.553025);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.213205,-0.923936,-0.317624);
glVertex3f(-0.629334,-0.179024,-0.553025);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.721526,-0.691777,0.02907);
glVertex3f(-0.64402,-0.175089,-0.236418);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.829288,0.459777,-0.317627);
glVertex3f(-0.646816,-0.157435,-0.553025);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.829288,0.459777,-0.317627);
glVertex3f(-0.646816,-0.157435,-0.553025);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.829288,0.459777,-0.317627);
glVertex3f(-0.646816,-0.157435,-0.553025);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.570423,0.757452,-0.317622);
glVertex3f(-0.642837,-0.152522,-0.553025);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.570423,0.757452,-0.317622);
glVertex3f(-0.642837,-0.152522,-0.553025);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.570423,0.757452,-0.317622);
glVertex3f(-0.642837,-0.152522,-0.553025);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.948212,-0.041609,0.314902);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(-0.841623,-0.070898,-0.535392);
glVertex3f(-0.673956,-0.20272,-0.509834);
glNormal3f(-0.707367,-0.706846,0);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(-0.948212,-0.041609,0.314902);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(-0.707367,-0.706846,0);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(-0.707367,-0.706846,0);
glVertex3f(-0.671653,-0.205025,-0.480663);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.130475,0.130475,-0.982829);
glVertex3f(-0.671653,-0.123659,-0.509834);
glNormal3f(-0.130475,0.130475,-0.982829);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.130477,0.130473,-0.982829);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(-0.59226,-0.407741,-0.694964);
glVertex3f(-0.644149,-0.155769,-0.517749);
glNormal3f(-0.59226,-0.40774,0.694964);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(-0.130477,0.130474,0.982829);
glVertex3f(-0.641845,-0.153466,-0.472749);
glNormal3f(-0.130477,0.130474,0.982829);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(-0.59226,-0.40774,0.694964);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(-0.130476,0.130475,0.982829);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(-0.841566,0.071084,0.535457);
glVertex3f(-0.673956,-0.125962,-0.480663);
glNormal3f(0.235702,-0.707107,-0.666666);
glVertex3f(-0.624045,-0.157287,-0.257801);
glNormal3f(-0.632457,0.632454,-0.447214);
glVertex3f(-0.626121,-0.155211,-0.257801);
glNormal3f(-0.235703,0.707106,-0.666667);
glVertex3f(-0.602606,-0.131697,-0.257801);
glNormal3f(0.235702,-0.707107,-0.666666);
glVertex3f(-0.624045,-0.157287,-0.257801);
glNormal3f(-0.235703,0.707106,-0.666667);
glVertex3f(-0.602606,-0.131697,-0.257801);
glNormal3f(0.632455,-0.632456,-0.447214);
glVertex3f(-0.600531,-0.133772,-0.257801);
glNormal3f(-0.553356,0.553344,0.622581);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.521019,0.15307,0.839708);
glVertex3f(-0.648065,-0.151547,-0.232579);
glNormal3f(0.551845,0.82017,0.150958);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(-0.521019,0.15307,0.839708);
glVertex3f(-0.648065,-0.151547,-0.232579);
glNormal3f(-0.553345,0.553353,0.622583);
glVertex3f(-0.64599,-0.149471,-0.232579);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.663829,-0.131632,-0.257801);
glNormal3f(-0.948687,-0.316216,1.9e-005);
glVertex3f(-0.665904,-0.133708,-0.257801);
glNormal3f(-0.876025,0.379972,0.296986);
glVertex3f(-0.665904,-0.133708,-0.254198);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.663829,-0.131632,-0.257801);
glNormal3f(-0.876025,0.379972,0.296986);
glVertex3f(-0.665904,-0.133708,-0.254198);
glNormal3f(-0.707114,0.707099,0);
glVertex3f(-0.663829,-0.131632,-0.254198);
glNormal3f(-0.094294,-0.094295,0.991069);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(-0.094294,-0.094295,0.991069);
glVertex3f(-0.64813,-0.177093,-0.232579);
glNormal3f(-0.011684,-0.486119,0.873814);
glVertex3f(-0.646054,-0.179168,-0.232579);
glNormal3f(-0.094294,-0.094295,0.991069);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(-0.011684,-0.486119,0.873814);
glVertex3f(-0.646054,-0.179168,-0.232579);
glNormal3f(0.590285,-0.674626,0.44322);
glVertex3f(-0.640377,-0.173491,-0.231498);
glNormal3f(0.376225,-0.37625,0.846694);
glVertex3f(-0.602671,-0.19707,-0.254198);
glNormal3f(0.876009,-0.380018,0.296975);
glVertex3f(-0.600595,-0.194995,-0.254198);
glNormal3f(0.376225,-0.37625,0.846694);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.376236,-0.376241,0.846693);
glVertex3f(-0.602671,-0.19707,-0.254198);
glNormal3f(0.376236,-0.376241,0.846693);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.376236,-0.376241,0.846693);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(-0.213206,0.923936,-0.317624);
glVertex3f(-0.637204,-0.149652,-0.553025);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(-0.968489,0.050906,-0.243798);
glVertex3f(-0.648453,-0.163542,-0.553025);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(-0.968489,0.050906,-0.243798);
glVertex3f(-0.648453,-0.163542,-0.553025);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(0.707116,-0.707098,0);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(0.707116,-0.707098,0);
glVertex3f(-0.644149,-0.155769,-0.517749);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(0.707116,-0.707098,0);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.472749);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(-0.308434,-0.896651,-0.317624);
glVertex3f(-0.638718,-0.178532,-0.553025);
glNormal3f(0.968493,-0.050842,-0.243798);
glVertex3f(-0.618086,-0.165133,-0.553025);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.968493,-0.050842,-0.243798);
glVertex3f(-0.618086,-0.165133,-0.553025);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.646529,0.693626,-0.317621);
glVertex3f(-0.622519,-0.153587,-0.553025);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(0.642746,-0.028195,0.76556);
glVertex3f(-0.61837,-0.151612,-0.232579);
glNormal3f(0.153053,0.52102,0.83971);
glVertex3f(-0.620445,-0.149536,-0.232579);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(0.153053,0.52102,0.83971);
glVertex3f(-0.620445,-0.149536,-0.232579);
glNormal3f(-0.590274,0.674636,0.443218);
glVertex3f(-0.626121,-0.155211,-0.231498);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.829262,-0.459825,-0.317624);
glVertex3f(-0.619722,-0.17124,-0.553025);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.829262,-0.459825,-0.317624);
glVertex3f(-0.619722,-0.17124,-0.553025);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(-0.707104,-0.707109,0);
glVertex3f(-0.626121,-0.155211,-0.231498);
glNormal3f(-0.707104,-0.707109,0);
glVertex3f(-0.626121,-0.155211,-0.257801);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(-0.707104,-0.707109,0);
glVertex3f(-0.626121,-0.155211,-0.257801);
glNormal3f(0.235702,-0.707107,-0.666666);
glVertex3f(-0.624045,-0.157287,-0.257801);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(-0.629033,0.777,0.024243);
glVertex3f(-0.642837,-0.152522,-0.236418);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(-0.376241,-0.376235,0.846694);
glVertex3f(-0.665969,-0.19493,-0.254198);
glNormal3f(-0.38,-0.876018,0.296973);
glVertex3f(-0.663893,-0.197006,-0.254198);
glNormal3f(-0.376241,-0.376235,0.846694);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(-0.376242,-0.376234,0.846694);
glVertex3f(-0.665969,-0.19493,-0.254198);
glNormal3f(-0.376242,-0.376234,0.846694);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(-0.376242,-0.376234,0.846694);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.094308,0.094309,0.991066);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(-0.094308,0.094309,0.991066);
glVertex3f(-0.64599,-0.149471,-0.232579);
glNormal3f(-0.521019,0.15307,0.839708);
glVertex3f(-0.648065,-0.151547,-0.232579);
glNormal3f(-0.094308,0.094309,0.991066);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(-0.521019,0.15307,0.839708);
glVertex3f(-0.648065,-0.151547,-0.232579);
glNormal3f(-0.674685,-0.590184,0.443264);
glVertex3f(-0.64239,-0.157224,-0.231498);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(0.376216,0.376219,0.846712);
glVertex3f(-0.600531,-0.133772,-0.254198);
glNormal3f(0.379978,0.876023,0.296987);
glVertex3f(-0.602606,-0.131697,-0.254198);
glNormal3f(0.376216,0.376219,0.846712);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(0.376216,0.376219,0.846712);
glVertex3f(-0.600531,-0.133772,-0.254198);
glNormal3f(0.376216,0.376219,0.846712);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(0.376216,0.376219,0.846712);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.706843,0.707371,0);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(0.706843,0.707371,0);
glVertex3f(-0.592589,-0.125962,-0.509834);
glNormal3f(0.041607,0.948212,-0.314901);
glVertex3f(-0.594895,-0.123659,-0.509834);
glNormal3f(0.706843,0.707371,0);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(0.041607,0.948212,-0.314901);
glVertex3f(-0.594895,-0.123659,-0.509834);
glNormal3f(0.070896,0.841624,0.535391);
glVertex3f(-0.594895,-0.123659,-0.480663);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.721526,-0.691777,0.02907);
glVertex3f(-0.64402,-0.175089,-0.236418);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.749322,-0.486568,-0.449186);
glVertex3f(-0.64602,-0.172618,-0.553025);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.308434,-0.896651,-0.317624);
glVertex3f(-0.638718,-0.178532,-0.553025);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.308434,-0.896651,-0.317624);
glVertex3f(-0.638718,-0.178532,-0.553025);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(0.028857,-0.551838,-0.833452);
glVertex3f(-0.632473,-0.17952,-0.553025);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(-0.028859,0.551837,-0.833452);
glVertex3f(-0.634066,-0.149155,-0.553025);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.600531,-0.133772,-0.257801);
glNormal3f(-0.235703,0.707106,-0.666667);
glVertex3f(-0.602606,-0.131697,-0.257801);
glNormal3f(0.379978,0.876023,0.296987);
glVertex3f(-0.602606,-0.131697,-0.254198);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.600531,-0.133772,-0.257801);
glNormal3f(0.379978,0.876023,0.296987);
glVertex3f(-0.602606,-0.131697,-0.254198);
glNormal3f(0.707104,0.707109,0);
glVertex3f(-0.600531,-0.133772,-0.254198);
glNormal3f(0.553349,0.553353,0.622579);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(-0.551788,0.820201,0.150998);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(0.153053,0.52102,0.83971);
glVertex3f(-0.620445,-0.149536,-0.232579);
glNormal3f(0.553349,0.553353,0.622579);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.153053,0.52102,0.83971);
glVertex3f(-0.620445,-0.149536,-0.232579);
glNormal3f(0.642746,-0.028195,0.76556);
glVertex3f(-0.61837,-0.151612,-0.232579);
glNormal3f(-0.553356,-0.553344,0.622581);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.553356,-0.553344,0.622581);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(-0.553356,-0.553344,0.622581);
glVertex3f(-0.646054,-0.179168,-0.232579);
glNormal3f(-0.553347,-0.553355,0.622579);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.011684,-0.486119,0.873814);
glVertex3f(-0.646054,-0.179168,-0.232579);
glNormal3f(-0.553347,-0.553355,0.622579);
glVertex3f(-0.64813,-0.177093,-0.232579);
glNormal3f(-0.707112,-0.707102,0);
glVertex3f(-0.665969,-0.19493,-0.257801);
glNormal3f(0.235669,-0.707117,-0.666668);
glVertex3f(-0.663893,-0.197006,-0.257801);
glNormal3f(-0.38,-0.876018,0.296973);
glVertex3f(-0.663893,-0.197006,-0.254198);
glNormal3f(-0.707112,-0.707102,0);
glVertex3f(-0.665969,-0.19493,-0.257801);
glNormal3f(-0.38,-0.876018,0.296973);
glVertex3f(-0.663893,-0.197006,-0.254198);
glNormal3f(-0.707112,-0.707102,0);
glVertex3f(-0.665969,-0.19493,-0.254198);
glNormal3f(-0.433236,-0.566721,0.700809);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(-0.028141,-0.642743,0.765565);
glVertex3f(-0.620508,-0.179231,-0.232579);
glNormal3f(0.094309,-0.094307,0.991066);
glVertex3f(-0.618433,-0.177156,-0.232579);
glNormal3f(-0.433236,-0.566721,0.700809);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(0.094309,-0.094307,0.991066);
glVertex3f(-0.618433,-0.177156,-0.232579);
glNormal3f(0.094309,-0.094307,0.991066);
glVertex3f(-0.62411,-0.17148,-0.231498);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.594895,-0.205025,-0.509834);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.592589,-0.20272,-0.509834);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.592589,-0.20272,-0.509834);
glNormal3f(0.707102,-0.707111,0);
glVertex3f(-0.592589,-0.20272,-0.480663);
glNormal3f(-0.707108,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(-0.707108,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.509834);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.707108,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.841566,0.071084,0.535457);
glVertex3f(-0.673956,-0.125962,-0.480663);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.872818,0.370554,-0.317614);
glVertex3f(-0.619076,-0.158889,-0.553025);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.570423,-0.757452,-0.317622);
glVertex3f(-0.623702,-0.176154,-0.553025);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(0.570423,-0.757452,-0.317622);
glVertex3f(-0.623702,-0.176154,-0.553025);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(0.028857,-0.551838,-0.833452);
glVertex3f(-0.632473,-0.17952,-0.553025);
glNormal3f(0.213205,-0.923936,-0.317624);
glVertex3f(-0.629334,-0.179024,-0.553025);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(0.213205,-0.923936,-0.317624);
glVertex3f(-0.629334,-0.179024,-0.553025);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.965636,-0.258766,0.024244);
glVertex3f(-0.618584,-0.168274,-0.236418);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.553346,-0.553354,0.622581);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(0.553346,-0.553354,0.622581);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.553346,-0.553354,0.622581);
glVertex3f(-0.618433,-0.177156,-0.232579);
glNormal3f(0.553357,-0.553345,0.622579);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(0.553357,-0.553345,0.622579);
glVertex3f(-0.618433,-0.177156,-0.232579);
glNormal3f(-0.028141,-0.642743,0.765565);
glVertex3f(-0.620508,-0.179231,-0.232579);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(0.130474,0.130477,0.982829);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(0.130474,0.130477,0.982829);
glVertex3f(-0.622397,-0.155769,-0.472749);
glNormal3f(0.130474,0.130477,0.982829);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(-0.938631,0.04539,0.341922);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(0.13043,0.130528,0.982828);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(0.070896,0.841624,0.535391);
glVertex3f(-0.594895,-0.123659,-0.480663);
glNormal3f(-0.130481,-0.130477,0.982828);
glVertex3f(-0.641845,-0.175218,-0.472749);
glNormal3f(-0.130481,-0.130477,0.982828);
glVertex3f(-0.644149,-0.172914,-0.472749);
glNormal3f(-0.130481,-0.130477,0.982828);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(-0.130524,-0.130427,0.982829);
glVertex3f(-0.641845,-0.175218,-0.472749);
glNormal3f(-0.948212,-0.041609,0.314902);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(-0.130524,-0.130427,0.982829);
glVertex3f(-0.671653,-0.205025,-0.480663);
glNormal3f(-0.130527,-0.13043,-0.982828);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(-0.841623,-0.070898,-0.535392);
glVertex3f(-0.673956,-0.20272,-0.509834);
glNormal3f(-0.130527,-0.13043,-0.982828);
glVertex3f(-0.644149,-0.172914,-0.517749);
glNormal3f(-0.130477,-0.130473,-0.982829);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(-0.130477,-0.130473,-0.982829);
glVertex3f(-0.644149,-0.172914,-0.517749);
glNormal3f(-0.130477,-0.130473,-0.982829);
glVertex3f(-0.641845,-0.175218,-0.517749);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(-0.685385,0.516784,0.513013);
glVertex3f(-0.636874,-0.161417,-0.166652);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(-0.453876,0.890735,0.024245);
glVertex3f(-0.640171,-0.150792,-0.236418);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(0.258823,-0.96562,0.024246);
glVertex3f(-0.629334,-0.179024,-0.236418);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.453875,-0.890736,0.024245);
glVertex3f(-0.626368,-0.177884,-0.236418);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.629033,-0.777,0.024242);
glVertex3f(-0.623702,-0.176154,-0.236418);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.689689,-0.719381,0.08258);
glVertex3f(-0.643329,-0.174398,-0.216274);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(0.562038,0.826884,0.019392);
glVertex3f(-0.62499,-0.151586,-0.236418);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.704278,0.704048,0.091153);
glVertex3f(-0.62321,-0.154279,-0.216274);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(0.707107,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.509834);
glNormal3f(0.707107,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.517749);
glNormal3f(0.707107,0.707106,0);
glVertex3f(-0.671653,-0.123659,-0.480663);
glNormal3f(1,6e-006,0);
glVertex3f(-0.641845,-0.153466,-0.472749);
glNormal3f(0.948683,-0.31623,0);
glVertex3f(-0.641845,-0.175218,-0.472749);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.671653,-0.205025,-0.480663);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(0.948683,-0.31623,0);
glVertex3f(-0.641845,-0.175218,-0.472749);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.671653,-0.205025,-0.509834);
glNormal3f(0.948686,0.316218,0);
glVertex3f(-0.641845,-0.175218,-0.517749);
glNormal3f(-0.59226,-0.40774,0.694964);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(-0.841566,0.071084,0.535457);
glVertex3f(-0.673956,-0.125962,-0.480663);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.59226,-0.40774,0.694964);
glVertex3f(-0.644149,-0.155769,-0.472749);
glNormal3f(-0.841566,0.071083,-0.535457);
glVertex3f(-0.673956,-0.125962,-0.509834);
glNormal3f(-0.59226,-0.407741,-0.694964);
glVertex3f(-0.644149,-0.155769,-0.517749);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.517749);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.592589,-0.125962,-0.509834);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.517749);
glNormal3f(0.707107,-0.707106,0);
glVertex3f(-0.592589,-0.125962,-0.480663);
glNormal3f(7e-006,-1,0);
glVertex3f(-0.622397,-0.155769,-0.472749);
glNormal3f(-0.938631,0.04539,0.341922);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(0.070896,0.841624,0.535391);
glVertex3f(-0.594895,-0.123659,-0.480663);
glNormal3f(0.041607,0.948212,-0.314901);
glVertex3f(-0.594895,-0.123659,-0.509834);
glNormal3f(-0.938631,0.04539,0.341922);
glVertex3f(-0.6247,-0.153466,-0.472749);
glNormal3f(0.041607,0.948212,-0.314901);
glVertex3f(-0.594895,-0.123659,-0.509834);
glNormal3f(-1,-2.1e-005,0);
glVertex3f(-0.6247,-0.153466,-0.517749);
glNormal3f(0.316209,0.94869,0);
glVertex3f(-0.622397,-0.172914,-0.472749);
glNormal3f(0.707087,0.707126,0);
glVertex3f(-0.592589,-0.20272,-0.480663);
glNormal3f(0.707087,0.707126,0);
glVertex3f(-0.592589,-0.20272,-0.509834);
glNormal3f(0.316209,0.94869,0);
glVertex3f(-0.622397,-0.172914,-0.472749);
glNormal3f(0.707087,0.707126,0);
glVertex3f(-0.592589,-0.20272,-0.509834);
glNormal3f(-0.316223,0.948685,0);
glVertex3f(-0.622397,-0.172914,-0.517749);
glNormal3f(-0.948691,-0.316204,0);
glVertex3f(-0.6247,-0.175218,-0.517749);
glNormal3f(-0.707127,-0.707086,0);
glVertex3f(-0.594895,-0.205025,-0.509834);
glNormal3f(-0.707127,-0.707086,0);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(-0.948691,-0.316204,0);
glVertex3f(-0.6247,-0.175218,-0.517749);
glNormal3f(-0.707127,-0.707086,0);
glVertex3f(-0.594895,-0.205025,-0.480663);
glNormal3f(-0.948678,0.316243,0);
glVertex3f(-0.6247,-0.175218,-0.472749);
glNormal3f(-0.316203,0.948691,0);
glVertex3f(-0.644149,-0.172914,-0.517749);
glNormal3f(-0.841623,-0.070898,-0.535392);
glVertex3f(-0.673956,-0.20272,-0.509834);
glNormal3f(-0.948212,-0.041609,0.314902);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(-0.316203,0.948691,0);
glVertex3f(-0.644149,-0.172914,-0.517749);
glNormal3f(-0.948212,-0.041609,0.314902);
glVertex3f(-0.673956,-0.20272,-0.480663);
glNormal3f(0.316243,0.948678,0);
glVertex3f(-0.644149,-0.172914,-0.472749);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(-0.25882,0.965621,0.024245);
glVertex3f(-0.637204,-0.149652,-0.236418);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.811529,0.279708,0.513014);
glVertex3f(-0.628938,-0.162675,-0.166652);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(0.811529,0.279708,0.513014);
glVertex3f(-0.628938,-0.162675,-0.166652);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.811529,0.279708,0.513014);
glVertex3f(-0.628938,-0.162675,-0.166652);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.998105,-0.052362,0.032333);
glVertex3f(-0.618086,-0.165133,-0.236418);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.627935,0.585215,0.51305);
glVertex3f(-0.629989,-0.161057,-0.166652);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.998102,0.052406,0.032332);
glVertex3f(-0.648453,-0.163542,-0.236418);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(0.776878,-0.629185,0.024235);
glVertex3f(-0.621454,-0.173906,-0.236418);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.773984,-0.626611,0.091138);
glVertex3f(-0.622213,-0.17329,-0.216274);
glNormal3f(0.890783,-0.453782,0.024242);
glVertex3f(-0.619722,-0.17124,-0.236418);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(-0.965643,0.258738,0.024237);
glVertex3f(-0.647955,-0.160404,-0.236418);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.562038,0.826884,0.019392);
glVertex3f(-0.62499,-0.151586,-0.236418);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(0.838455,0.544431,0.024248);
glVertex3f(-0.620519,-0.156057,-0.236418);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(0.835222,0.542308,0.091139);
glVertex3f(-0.621339,-0.156589,-0.216274);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.629588,-0.629407,0.455485);
glVertex3f(-0.636549,-0.167619,-0.166652);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.629588,-0.629407,0.455485);
glVertex3f(-0.636549,-0.167619,-0.166652);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(0.993076,-0.051987,0.105347);
glVertex3f(-0.619062,-0.165082,-0.216274);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(-0.689689,-0.719381,0.08258);
glVertex3f(-0.643329,-0.174398,-0.216274);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(-0.776871,0.629193,0.024236);
glVertex3f(-0.645085,-0.154769,-0.236418);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(-0.890827,0.453696,0.024243);
glVertex3f(-0.646816,-0.157435,-0.236418);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.887429,-0.451841,0.091153);
glVertex3f(-0.620594,-0.170796,-0.216274);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.961922,-0.257682,0.091136);
glVertex3f(-0.619529,-0.16802,-0.216274);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(-0.154127,-0.975479,0.157116);
glVertex3f(-0.635183,-0.176419,-0.201483);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(0.154135,0.975478,0.157115);
glVertex3f(-0.631356,-0.152256,-0.201483);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(0.88007,-0.448104,0.157097);
glVertex3f(-0.622371,-0.169891,-0.201483);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(-0.721526,-0.691777,0.02907);
glVertex3f(-0.64402,-0.175089,-0.236418);
glNormal3f(-0.689689,-0.719381,0.08258);
glVertex3f(-0.643329,-0.174398,-0.216274);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.721526,-0.691777,0.02907);
glVertex3f(-0.64402,-0.175089,-0.236418);
glNormal3f(-0.827483,-0.555014,0.085034);
glVertex3f(-0.6452,-0.172086,-0.216274);
glNormal3f(-0.838585,-0.544531,0.016164);
glVertex3f(-0.64602,-0.172618,-0.236418);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.689689,-0.719381,0.08258);
glVertex3f(-0.643329,-0.174398,-0.216274);
glNormal3f(-0.573525,-0.81883,0.024224);
glVertex3f(-0.641549,-0.177089,-0.236418);
glNormal3f(-0.689689,-0.719381,0.08258);
glVertex3f(-0.643329,-0.174398,-0.216274);
glNormal3f(-0.721526,-0.691777,0.02907);
glVertex3f(-0.64402,-0.175089,-0.236418);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(-0.10436,-0.852013,0.513014);
glVertex3f(-0.633996,-0.16892,-0.166652);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(-0.10436,-0.852013,0.513014);
glVertex3f(-0.633996,-0.16892,-0.166652);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.629588,-0.629407,0.455485);
glVertex3f(-0.636549,-0.167619,-0.166652);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(-0.629588,-0.629407,0.455485);
glVertex3f(-0.636549,-0.167619,-0.166652);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(-0.074569,0.855131,0.513021);
glVertex3f(-0.633513,-0.159704,-0.166652);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.074569,0.855131,0.513021);
glVertex3f(-0.633513,-0.159704,-0.166652);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.836446,0.192711,0.51305);
glVertex3f(-0.63775,-0.163137,-0.166652);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(-0.836446,0.192711,0.51305);
glVertex3f(-0.63775,-0.163137,-0.166652);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(-0.074569,0.855131,0.513021);
glVertex3f(-0.633513,-0.159704,-0.166652);
glNormal3f(-0.851988,-0.104271,0.513074);
glVertex3f(-0.637851,-0.165063,-0.166652);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(-0.851988,-0.104271,0.513074);
glVertex3f(-0.637851,-0.165063,-0.166652);
glNormal3f(-0.973368,0.05103,0.223495);
glVertex3f(-0.642293,-0.163866,-0.184063);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.933297,0.358287,0.024233);
glVertex3f(-0.619076,-0.158889,-0.236418);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(0.987386,0.156467,0.024246);
glVertex3f(-0.618253,-0.16196,-0.236418);
glNormal3f(0.929754,0.356721,0.091149);
glVertex3f(-0.619988,-0.15924,-0.216274);
glNormal3f(0.983587,0.155712,0.091164);
glVertex3f(-0.619219,-0.162112,-0.216274);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(0.626728,-0.773891,0.091128);
glVertex3f(-0.624316,-0.175393,-0.216274);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.961919,0.257697,0.091128);
glVertex3f(-0.64701,-0.160655,-0.216274);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(-0.953956,0.255518,0.157096);
glVertex3f(-0.645085,-0.161173,-0.201483);
glNormal3f(-0.993076,0.051991,0.105346);
glVertex3f(-0.647476,-0.163593,-0.216274);
glNormal3f(-0.984961,0.051585,0.164894);
glVertex3f(-0.645484,-0.163697,-0.201483);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(0.052334,-0.994461,0.091155);
glVertex3f(-0.632524,-0.178544,-0.216274);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(0.037282,-0.999089,0.020774);
glVertex3f(-0.632473,-0.17952,-0.236418);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(-0.037285,0.999089,0.020774);
glVertex3f(-0.634066,-0.149155,-0.236418);
glNormal3f(-0.052328,0.994461,0.091153);
glVertex3f(-0.634015,-0.150131,-0.216274);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.953955,-0.255522,0.157094);
glVertex3f(-0.621454,-0.167503,-0.201483);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.984961,-0.051599,0.164892);
glVertex3f(-0.621055,-0.164978,-0.201483);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.646529,0.693626,-0.317621);
glVertex3f(-0.622519,-0.153587,-0.553025);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.562038,0.826884,0.019392);
glVertex3f(-0.62499,-0.151586,-0.236418);
glNormal3f(0.646529,0.693626,-0.317621);
glVertex3f(-0.622519,-0.153587,-0.553025);
glNormal3f(0.562038,0.826884,0.019392);
glVertex3f(-0.62499,-0.151586,-0.236418);
glNormal3f(0.706933,0.706864,0.024252);
glVertex3f(-0.622519,-0.153587,-0.236418);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(-0.62673,0.77389,0.091129);
glVertex3f(-0.642223,-0.153282,-0.216274);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(-0.819553,-0.532045,0.212744);
glVertex3f(-0.640847,-0.169259,-0.184063);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(-0.828395,-0.537657,0.157121);
glVertex3f(-0.643528,-0.170999,-0.201483);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.922065,0.353711,0.157114);
glVertex3f(-0.621851,-0.159954,-0.201483);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.975417,0.154554,0.157084);
glVertex3f(-0.621189,-0.162425,-0.201483);
glNormal3f(0.912233,0.350115,0.212722);
glVertex3f(-0.624835,-0.161099,-0.184063);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.621777,0.767272,0.157124);
glVertex3f(-0.640967,-0.154832,-0.201483);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.308572,0.896601,-0.317631);
glVertex3f(-0.62782,-0.150143,-0.553025);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.562038,0.826884,0.019392);
glVertex3f(-0.62499,-0.151586,-0.236418);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.774026,0.626555,0.091169);
glVertex3f(-0.644326,-0.155385,-0.216274);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(-0.767585,0.621391,0.157122);
glVertex3f(-0.642774,-0.15664,-0.201483);
glNormal3f(-0.887397,0.451901,0.091165);
glVertex3f(-0.645945,-0.157879,-0.216274);
glNormal3f(-0.879926,0.448386,0.157099);
glVertex3f(-0.644168,-0.158784,-0.201483);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(-0.10436,-0.852013,0.513014);
glVertex3f(-0.633996,-0.16892,-0.166652);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(-0.152338,-0.965159,0.212749);
glVertex3f(-0.634683,-0.173262,-0.184063);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(0.051903,-0.986216,0.15711);
glVertex3f(-0.63263,-0.176552,-0.201483);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(0.051149,-0.975775,0.212712);
glVertex3f(-0.632796,-0.17336,-0.184063);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(-0.983589,-0.155697,0.091168);
glVertex3f(-0.64732,-0.166564,-0.216274);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(-0.929745,-0.356744,0.091153);
glVertex3f(-0.646551,-0.169435,-0.216274);
glNormal3f(-0.987381,-0.156492,0.024251);
glVertex3f(-0.648286,-0.166717,-0.236418);
glNormal3f(-0.933271,-0.358355,0.024232);
glVertex3f(-0.647463,-0.169786,-0.236418);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(-0.975417,-0.15455,0.157083);
glVertex3f(-0.64535,-0.166251,-0.201483);
glNormal3f(-0.922063,-0.353716,0.157115);
glVertex3f(-0.644688,-0.168721,-0.201483);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(-0.051898,0.986216,0.15711);
glVertex3f(-0.633909,-0.152124,-0.201483);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.051149,0.975775,0.212713);
glVertex3f(-0.633742,-0.155315,-0.184063);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.685385,0.516784,0.513013);
glVertex3f(-0.636874,-0.161417,-0.166652);
glNormal3f(-0.759397,0.614866,0.212732);
glVertex3f(-0.64029,-0.158651,-0.184063);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(-0.685385,0.516784,0.513013);
glVertex3f(-0.636874,-0.161417,-0.166652);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.390698,0.9202,0.024244);
glVertex3f(-0.62782,-0.150143,-0.236418);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.190777,0.981335,0.024221);
glVertex3f(-0.630891,-0.149322,-0.236418);
glNormal3f(0.155647,0.983601,0.09112);
glVertex3f(-0.631044,-0.150286,-0.216274);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.155647,-0.983601,0.09112);
glVertex3f(-0.635495,-0.178389,-0.216274);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(-0.190778,-0.981334,0.02422);
glVertex3f(-0.635648,-0.179354,-0.236418);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(-0.390628,-0.920229,0.024244);
glVertex3f(-0.638718,-0.178532,-0.236418);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(-0.356787,-0.92973,0.091136);
glVertex3f(-0.638368,-0.177618,-0.216274);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(-0.542099,-0.835355,0.091162);
glVertex3f(-0.641017,-0.176269,-0.216274);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.543057,0.836162,0.076954);
glVertex3f(-0.625522,-0.152406,-0.216274);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.356788,0.929729,0.091136);
glVertex3f(-0.628171,-0.151057,-0.216274);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(0.257917,-0.961856,0.091166);
glVertex3f(-0.629588,-0.178079,-0.216274);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(0.452117,-0.887292,0.091123);
glVertex3f(-0.626812,-0.177013,-0.216274);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.257923,0.961855,0.091167);
glVertex3f(-0.636951,-0.150596,-0.216274);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.255524,0.953958,0.157071);
glVertex3f(-0.636435,-0.152523,-0.201483);
glNormal3f(-0.452121,0.88729,0.091123);
glVertex3f(-0.639727,-0.151663,-0.216274);
glNormal3f(-0.448376,0.879938,0.157063);
glVertex3f(-0.638822,-0.153439,-0.201483);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(0.621655,-0.767373,0.15711);
glVertex3f(-0.625571,-0.173843,-0.201483);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(0.767547,-0.621443,0.1571);
glVertex3f(-0.623763,-0.172035,-0.201483);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(0.255516,-0.95396,0.15707);
glVertex3f(-0.630103,-0.176152,-0.201483);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(0.448376,-0.879937,0.157063);
glVertex3f(-0.627717,-0.175237,-0.201483);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(0.685502,-0.516594,0.513048);
glVertex3f(-0.629665,-0.167258,-0.166652);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.698412,0.698249,0.157064);
glVertex3f(-0.62462,-0.155689,-0.201483);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(0.82839,0.537671,0.1571);
glVertex3f(-0.623011,-0.157676,-0.201483);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(0.790886,-0.041888,0.610528);
glVertex3f(-0.628637,-0.16458,-0.166652);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(-0.3498,-0.912345,0.212757);
glVertex3f(-0.636507,-0.172773,-0.184063);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(-0.353902,-0.921998,0.157076);
glVertex3f(-0.637654,-0.175757,-0.201483);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.538685,0.829179,0.149265);
glVertex3f(-0.626607,-0.15408,-0.201483);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.353905,0.921997,0.157075);
glVertex3f(-0.628885,-0.152918,-0.201483);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(-0.416482,0.750555,0.51304);
glVertex3f(-0.635376,-0.160205,-0.166652);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.416482,0.750555,0.51304);
glVertex3f(-0.635376,-0.160205,-0.166652);
glNormal3f(-0.615163,0.75915,0.212757);
glVertex3f(-0.638955,-0.157316,-0.184063);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.533413,0.821382,0.201993);
glVertex3f(-0.628348,-0.15676,-0.184063);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(0.279253,0.811674,0.513034);
glVertex3f(-0.631607,-0.160007,-0.166652);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.943891,-0.2526,0.21275);
glVertex3f(-0.624542,-0.166676,-0.184063);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.870622,0.443574,0.212744);
glVertex3f(-0.64132,-0.160235,-0.184063);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.943904,0.252547,0.212754);
glVertex3f(-0.641997,-0.161999,-0.184063);
glNormal3f(-0.836446,0.192711,0.51305);
glVertex3f(-0.63775,-0.163137,-0.166652);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.691275,0.690561,0.212756);
glVertex3f(-0.62688,-0.157949,-0.184063);
glNormal3f(0.627935,0.585215,0.51305);
glVertex3f(-0.629989,-0.161057,-0.166652);
glNormal3f(0.819685,0.531841,0.212749);
glVertex3f(-0.625692,-0.159417,-0.184063);
glNormal3f(0.627935,0.585215,0.51305);
glVertex3f(-0.629989,-0.161057,-0.166652);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.416471,-0.750562,0.513038);
glVertex3f(-0.631163,-0.168471,-0.166652);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(0.416471,-0.750562,0.513038);
glVertex3f(-0.631163,-0.168471,-0.166652);
glNormal3f(0.614937,-0.759334,0.212754);
glVertex3f(-0.627584,-0.17136,-0.184063);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(-0.252897,0.943816,0.212731);
glVertex3f(-0.635609,-0.15561,-0.184063);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(-0.44417,0.87031,0.212776);
glVertex3f(-0.637371,-0.156287,-0.184063);
glNormal3f(-0.416482,0.750555,0.51304);
glVertex3f(-0.635376,-0.160205,-0.166652);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.252901,-0.943815,0.212731);
glVertex3f(-0.63093,-0.173066,-0.184063);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.444173,-0.870308,0.212777);
glVertex3f(-0.629168,-0.172389,-0.184063);
glNormal3f(0.416471,-0.750562,0.513038);
glVertex3f(-0.631163,-0.168471,-0.166652);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(-0.532171,-0.819471,0.212748);
glVertex3f(-0.638191,-0.171916,-0.184063);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(-0.537842,-0.828282,0.157084);
glVertex3f(-0.639931,-0.174595,-0.201483);
glNormal3f(-0.692649,-0.692407,0.202013);
glVertex3f(-0.639658,-0.170727,-0.184063);
glNormal3f(-0.699156,-0.699203,0.149319);
glVertex3f(-0.641919,-0.172988,-0.201483);
glNormal3f(0.279253,0.811674,0.513034);
glVertex3f(-0.631607,-0.160007,-0.166652);
glNormal3f(0.349572,0.912426,0.212785);
glVertex3f(-0.630032,-0.155902,-0.184063);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(0.279253,0.811674,0.513034);
glVertex3f(-0.631607,-0.160007,-0.166652);
glNormal3f(0.152609,0.965113,0.212762);
glVertex3f(-0.631856,-0.155414,-0.184063);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(0.685502,-0.516594,0.513048);
glVertex3f(-0.629665,-0.167258,-0.166652);
glNormal3f(0.759481,-0.614751,0.212767);
glVertex3f(-0.626247,-0.170024,-0.184063);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(0.685502,-0.516594,0.513048);
glVertex3f(-0.629665,-0.167258,-0.166652);
glNormal3f(0.870722,-0.443374,0.21275);
glVertex3f(-0.625219,-0.16844,-0.184063);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(0.965083,0.152857,0.212722);
glVertex3f(-0.624345,-0.162925,-0.184063);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.973355,-0.051337,0.223481);
glVertex3f(-0.624246,-0.164811,-0.184063);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.790886,-0.041888,0.610528);
glVertex3f(-0.628637,-0.16458,-0.166652);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.851988,-0.104271,0.513074);
glVertex3f(-0.637851,-0.165063,-0.166652);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.965116,-0.152602,0.212752);
glVertex3f(-0.642194,-0.165751,-0.184063);
glNormal3f(-0.912227,-0.35013,0.212724);
glVertex3f(-0.641704,-0.167576,-0.184063);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(0.235702,-0.707107,-0.666666);
glVertex3f(-0.624045,-0.157287,-0.257801);
glNormal3f(0.707107,-0.707107,1e-006);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.057054,-0.798364,0.599466);
glVertex3f(-0.624045,-0.157287,-0.231498);
glNormal3f(0.707107,-0.707107,1e-006);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.642746,-0.028195,0.76556);
glVertex3f(-0.61837,-0.151612,-0.232579);
glNormal3f(-2e-006,1,-1e-006);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(-0.316216,0.948687,2.1e-005);
glVertex3f(-0.642453,-0.171416,-0.257801);
glNormal3f(-0.707088,0.707126,1.1e-005);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-2e-006,1,-1e-006);
glVertex3f(-0.642453,-0.171416,-0.231498);
glNormal3f(-0.707088,0.707126,1.1e-005);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.707106,0.707107,-3e-006);
glVertex3f(-0.64813,-0.177093,-0.232579);
glNormal3f(-0.590274,0.674636,0.443218);
glVertex3f(-0.626121,-0.155211,-0.231498);
glNormal3f(0.153053,0.52102,0.83971);
glVertex3f(-0.620445,-0.149536,-0.232579);
glNormal3f(-0.551788,0.820201,0.150998);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(-0.590274,0.674636,0.443218);
glVertex3f(-0.626121,-0.155211,-0.231498);
glNormal3f(-0.551788,0.820201,0.150998);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(-0.632457,0.632454,-0.447214);
glVertex3f(-0.626121,-0.155211,-0.257801);
glNormal3f(0.590285,-0.674626,0.44322);
glVertex3f(-0.640377,-0.173491,-0.231498);
glNormal3f(-0.011684,-0.486119,0.873814);
glVertex3f(-0.646054,-0.179168,-0.232579);
glNormal3f(0.707087,-0.707127,-1.1e-005);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(0.590285,-0.674626,0.44322);
glVertex3f(-0.640377,-0.173491,-0.231498);
glNormal3f(0.707087,-0.707127,-1.1e-005);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(0.632438,-0.632462,-0.44723);
glVertex3f(-0.640377,-0.173491,-0.257801);
glNormal3f(-0.674685,-0.590184,0.443264);
glVertex3f(-0.64239,-0.157224,-0.231498);
glNormal3f(-0.521019,0.15307,0.839708);
glVertex3f(-0.648065,-0.151547,-0.232579);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.674685,-0.590184,0.443264);
glVertex3f(-0.64239,-0.157224,-0.231498);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.707144,-0.707069,2.1e-005);
glVertex3f(-0.64239,-0.157224,-0.257801);
glNormal3f(0.316135,0.948714,-6e-005);
glVertex3f(-0.62411,-0.17148,-0.231498);
glNormal3f(0.706987,0.707226,-0.000134);
glVertex3f(-0.618433,-0.177156,-0.232579);
glNormal3f(0.707077,0.707136,-5.5e-005);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.316135,0.948714,-6e-005);
glVertex3f(-0.62411,-0.17148,-0.231498);
glNormal3f(0.707077,0.707136,-5.5e-005);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.632422,0.632467,-0.447245);
glVertex3f(-0.62411,-0.17148,-0.257801);
glNormal3f(-0.433236,-0.566721,0.700809);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(-0.948674,-0.316256,3.9e-005);
glVertex3f(-0.626185,-0.173556,-0.257801);
glNormal3f(-0.707078,-0.707136,5.6e-005);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(-0.433236,-0.566721,0.700809);
glVertex3f(-0.626185,-0.173556,-0.231498);
glNormal3f(-0.707078,-0.707136,5.6e-005);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(-0.028141,-0.642743,0.765565);
glVertex3f(-0.620508,-0.179231,-0.232579);
glNormal3f(1,-6.3e-005,-4.9e-005);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(0.707116,0.235672,-0.666668);
glVertex3f(-0.640314,-0.155148,-0.257801);
glNormal3f(0.551845,0.82017,0.150958);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(1,-6.3e-005,-4.9e-005);
glVertex3f(-0.640314,-0.155148,-0.231498);
glNormal3f(0.551845,0.82017,0.150958);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(0.707227,0.706987,-0.000139);
glVertex3f(-0.64599,-0.149471,-0.232579);
glNormal3f(0.707107,-0.707107,1e-006);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.235702,-0.707107,-0.666666);
glVertex3f(-0.624045,-0.157287,-0.257801);
glNormal3f(0.632455,-0.632456,-0.447214);
glVertex3f(-0.600531,-0.133772,-0.257801);
glNormal3f(0.707107,-0.707107,1e-006);
glVertex3f(-0.611883,-0.145125,-0.24411);
glNormal3f(0.632455,-0.632456,-0.447214);
glVertex3f(-0.600531,-0.133772,-0.257801);
glNormal3f(0.707107,-0.707107,0);
glVertex3f(-0.600531,-0.133772,-0.254198);
glNormal3f(-0.551788,0.820201,0.150998);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(0.379978,0.876023,0.296987);
glVertex3f(-0.602606,-0.131697,-0.254198);
glNormal3f(-0.235703,0.707106,-0.666667);
glVertex3f(-0.602606,-0.131697,-0.257801);
glNormal3f(-0.551788,0.820201,0.150998);
glVertex3f(-0.613959,-0.143049,-0.24411);
glNormal3f(-0.235703,0.707106,-0.666667);
glVertex3f(-0.602606,-0.131697,-0.257801);
glNormal3f(-0.632457,0.632454,-0.447214);
glVertex3f(-0.626121,-0.155211,-0.257801);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.876025,0.379972,0.296986);
glVertex3f(-0.665904,-0.133708,-0.254198);
glNormal3f(-0.948687,-0.316216,1.9e-005);
glVertex3f(-0.665904,-0.133708,-0.257801);
glNormal3f(-0.830613,-0.305865,0.465325);
glVertex3f(-0.654552,-0.14506,-0.24411);
glNormal3f(-0.948687,-0.316216,1.9e-005);
glVertex3f(-0.665904,-0.133708,-0.257801);
glNormal3f(-0.707144,-0.707069,2.1e-005);
glVertex3f(-0.64239,-0.157224,-0.257801);
glNormal3f(0.551845,0.82017,0.150958);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(0.707116,0.235672,-0.666668);
glVertex3f(-0.640314,-0.155148,-0.257801);
glNormal3f(0.632461,0.632439,-0.447228);
glVertex3f(-0.663829,-0.131632,-0.257801);
glNormal3f(0.551845,0.82017,0.150958);
glVertex3f(-0.652476,-0.142985,-0.24411);
glNormal3f(0.632461,0.632439,-0.447228);
glVertex3f(-0.663829,-0.131632,-0.257801);
glNormal3f(0.707107,0.707107,0);
glVertex3f(-0.663829,-0.131632,-0.254198);
glNormal3f(-0.707088,0.707126,1.1e-005);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.316216,0.948687,2.1e-005);
glVertex3f(-0.642453,-0.171416,-0.257801);
glNormal3f(-0.632427,0.632496,-0.447197);
glVertex3f(-0.665969,-0.19493,-0.257801);
glNormal3f(-0.707088,0.707126,1.1e-005);
glVertex3f(-0.654617,-0.18358,-0.24411);
glNormal3f(-0.632427,0.632496,-0.447197);
glVertex3f(-0.665969,-0.19493,-0.257801);
glNormal3f(-0.707054,0.70716,0);
glVertex3f(-0.665969,-0.19493,-0.254198);
glNormal3f(0.707087,-0.707127,-1.1e-005);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(-0.38,-0.876018,0.296973);
glVertex3f(-0.663893,-0.197006,-0.254198);
glNormal3f(0.235669,-0.707117,-0.666668);
glVertex3f(-0.663893,-0.197006,-0.257801);
glNormal3f(0.707087,-0.707127,-1.1e-005);
glVertex3f(-0.652541,-0.185655,-0.24411);
glNormal3f(0.235669,-0.707117,-0.666668);
glVertex3f(-0.663893,-0.197006,-0.257801);
glNormal3f(0.632438,-0.632462,-0.44723);
glVertex3f(-0.640377,-0.173491,-0.257801);
glNormal3f(-0.707078,-0.707136,5.6e-005);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(-0.948674,-0.316256,3.9e-005);
glVertex3f(-0.626185,-0.173556,-0.257801);
glNormal3f(-0.63249,-0.632443,-0.447183);
glVertex3f(-0.602671,-0.19707,-0.257801);
glNormal3f(-0.707078,-0.707136,5.6e-005);
glVertex3f(-0.614022,-0.185718,-0.24411);
glNormal3f(-0.63249,-0.632443,-0.447183);
glVertex3f(-0.602671,-0.19707,-0.257801);
glNormal3f(-0.707159,-0.707054,0);
glVertex3f(-0.602671,-0.19707,-0.254198);
glNormal3f(0.707077,0.707136,-5.5e-005);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.876009,-0.380018,0.296975);
glVertex3f(-0.600595,-0.194995,-0.254198);
glNormal3f(0.500007,0.499971,-0.707122);
glVertex3f(-0.600595,-0.194995,-0.257801);
glNormal3f(0.707077,0.707136,-5.5e-005);
glVertex3f(-0.611946,-0.183642,-0.24411);
glNormal3f(0.500007,0.499971,-0.707122);
glVertex3f(-0.600595,-0.194995,-0.257801);
glNormal3f(0.632422,0.632467,-0.447245);
glVertex3f(-0.62411,-0.17148,-0.257801);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(0.308572,0.896601,-0.317631);
glVertex3f(-0.62782,-0.150143,-0.553025);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.829288,0.459777,-0.317627);
glVertex3f(-0.646816,-0.157435,-0.553025);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(-0.968489,0.050906,-0.243798);
glVertex3f(-0.648453,-0.163542,-0.553025);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(-0.572012,0.174854,-0.801391);
glVertex3f(-0.647955,-0.160404,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.968493,-0.050842,-0.243798);
glVertex3f(-0.618086,-0.165133,-0.553025);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.829262,-0.459825,-0.317624);
glVertex3f(-0.619722,-0.17124,-0.553025);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.571987,-0.174918,-0.801395);
glVertex3f(-0.618584,-0.168274,-0.553025);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.213205,-0.923936,-0.317624);
glVertex3f(-0.629334,-0.179024,-0.553025);
glNormal3f(0.028857,-0.551838,-0.833452);
glVertex3f(-0.632473,-0.17952,-0.553025);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.028857,-0.551838,-0.833452);
glVertex3f(-0.632473,-0.17952,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(-0.308434,-0.896651,-0.317624);
glVertex3f(-0.638718,-0.178532,-0.553025);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.213206,0.923936,-0.317624);
glVertex3f(-0.637204,-0.149652,-0.553025);
glNormal3f(-0.028859,0.551837,-0.833452);
glVertex3f(-0.634066,-0.149155,-0.553025);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.028859,0.551837,-0.833452);
glVertex3f(-0.634066,-0.149155,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.749322,-0.486568,-0.449186);
glVertex3f(-0.64602,-0.172618,-0.553025);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.646529,0.693626,-0.317621);
glVertex3f(-0.622519,-0.153587,-0.553025);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.30805,0.512711,-0.801394);
glVertex3f(-0.62499,-0.151588,-0.553025);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.570423,-0.757452,-0.317622);
glVertex3f(-0.623702,-0.176154,-0.553025);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0.451345,-0.392486,-0.8014);
glVertex3f(-0.621454,-0.173906,-0.553025);
glNormal3f(0.252771,-0.5421,-0.801395);
glVertex3f(-0.626368,-0.177884,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.570423,0.757452,-0.317622);
glVertex3f(-0.642837,-0.152522,-0.553025);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(-0.451342,0.39249,-0.8014);
glVertex3f(-0.645085,-0.154769,-0.553025);
glNormal3f(-0.252769,0.542102,-0.801395);
glVertex3f(-0.640171,-0.150792,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.587104,0.11429,-0.801403);
glVertex3f(-0.618253,-0.16196,-0.553025);
glNormal3f(0.489916,0.34312,-0.801405);
glVertex3f(-0.620519,-0.156057,-0.553025);
glNormal3f(0.872818,0.370554,-0.317614);
glVertex3f(-0.619076,-0.158889,-0.553025);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.279253,0.811674,0.513034);
glVertex3f(-0.631607,-0.160007,-0.166652);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.424619,0.832714,0.355368);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(-0.416482,0.750555,0.51304);
glVertex3f(-0.635376,-0.160205,-0.166652);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(0.790886,-0.041888,0.610528);
glVertex3f(-0.628637,-0.16458,-0.166652);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.416471,-0.750562,0.513038);
glVertex3f(-0.631163,-0.168471,-0.166652);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.923304,-0.145915,0.355272);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(-0.851988,-0.104271,0.513074);
glVertex3f(-0.637851,-0.165063,-0.166652);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.521845,-0.617221,0.588826);
glVertex3f(-0.63035,-0.167943,-0.166652);
glNormal3f(0.685502,-0.516594,0.513048);
glVertex3f(-0.629665,-0.167258,-0.166652);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(-0.10436,-0.852013,0.513014);
glVertex3f(-0.633996,-0.16892,-0.166652);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.456906,0.703917,0.543818);
glVertex3f(-0.630743,-0.160446,-0.166652);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.627935,0.585215,0.51305);
glVertex3f(-0.629989,-0.161057,-0.166652);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.660984,-0.660977,0.355261);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.629588,-0.629407,0.455485);
glVertex3f(-0.636549,-0.167619,-0.166652);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.726177,0.588554,0.355346);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.521916,0.617145,0.588843);
glVertex3f(-0.636189,-0.160732,-0.166652);
glNormal3f(-0.685385,0.516784,0.513013);
glVertex3f(-0.636874,-0.161417,-0.166652);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(-0.903038,0.241422,0.355301);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.760554,0.039711,0.648059);
glVertex3f(-0.637902,-0.164095,-0.166652);
glNormal3f(-0.727874,0.351323,0.588873);
glVertex3f(-0.637403,-0.162231,-0.166652);
glNormal3f(-0.836446,0.192711,0.51305);
glVertex3f(-0.63775,-0.163137,-0.166652);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.811529,0.279708,0.513014);
glVertex3f(-0.628938,-0.162675,-0.166652);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.800823,0.10909,0.588882);
glVertex3f(-0.628686,-0.163612,-0.166652);
glNormal3f(0.687058,0.425703,0.588837);
glVertex3f(-0.629379,-0.161811,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(-0.048597,0.933462,0.355369);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(-0.226075,0.775945,0.588897);
glVertex3f(-0.63447,-0.159856,-0.166652);
glNormal3f(0.109498,0.80079,0.58885);
glVertex3f(-0.632543,-0.159755,-0.166652);
glNormal3f(-0.074569,0.855131,0.513021);
glVertex3f(-0.633513,-0.159704,-0.166652);
glNormal3f(0.085858,0.700142,-0.708823);
glVertex3f(-0.630891,-0.149322,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.028859,0.551837,-0.833452);
glVertex3f(-0.634066,-0.149155,-0.553025);
glNormal3f(-0.692366,-0.134845,-0.708834);
glVertex3f(-0.648286,-0.166717,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.649284,-0.275684,-0.708822);
glVertex3f(-0.647463,-0.169786,-0.553025);
glNormal3f(-0.842206,-0.283411,0.458658);
glVertex3f(-0.637601,-0.166001,-0.166652);
glNormal3f(-0.745499,-0.48333,0.458937);
glVertex3f(-0.63716,-0.166865,-0.166652);
glNormal3f(-0.830991,-0.423705,0.360455);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.564563,-0.564569,-0.602105);
glVertex3f(-0.64402,-0.175089,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(-0.363274,-0.604642,-0.70883);
glVertex3f(-0.641549,-0.177089,-0.553025);
glNormal3f(-0.483256,-0.745536,0.458956);
glVertex3f(-0.635796,-0.168229,-0.166652);
glNormal3f(-0.283368,-0.84223,0.45864);
glVertex3f(-0.634932,-0.16867,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(-0.085859,-0.700142,-0.708823);
glVertex3f(-0.635648,-0.179354,-0.553025);
glNormal3f(0,0,-1);
glVertex3f(-0.633269,-0.164339,-0.553025);
glNormal3f(0.028857,-0.551838,-0.833452);
glVertex3f(-0.632473,-0.17952,-0.553025);
glNormal3f(0.047146,-0.88722,0.458931);
glVertex3f(-0.633026,-0.168971,-0.166652);
glNormal3f(0.265901,-0.847891,0.45867);
glVertex3f(-0.632068,-0.16882,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.792056,-0.402478,0.458977);
glVertex3f(-0.629135,-0.166445,-0.166652);
glNormal3f(0.867262,-0.193669,0.458639);
glVertex3f(-0.628788,-0.165538,-0.166652);
glNormal3f(0.705354,-0.418561,0.572086);
glVertex3f(-0.633269,-0.164339,-0.154713);
glNormal3f(0.948686,0.316218,0);
glVertex3f(-0.641845,-0.175218,-0.517749);
glNormal3f(-0.316203,0.948691,0);
glVertex3f(-0.644149,-0.172914,-0.517749);
glNormal3f(0.316243,0.948678,0);
glVertex3f(-0.644149,-0.172914,-0.472749);
glNormal3f(0.948686,0.316218,0);
glVertex3f(-0.641845,-0.175218,-0.517749);
glNormal3f(0.316243,0.948678,0);
glVertex3f(-0.644149,-0.172914,-0.472749);
glNormal3f(0.948683,-0.31623,0);
glVertex3f(-0.641845,-0.175218,-0.472749);
glNormal3f(-0.316223,0.948685,0);
glVertex3f(-0.622397,-0.172914,-0.517749);
glNormal3f(-0.948691,-0.316204,0);
glVertex3f(-0.6247,-0.175218,-0.517749);
glNormal3f(-0.948678,0.316243,0);
glVertex3f(-0.6247,-0.175218,-0.472749);
glNormal3f(-0.316223,0.948685,0);
glVertex3f(-0.622397,-0.172914,-0.517749);
glNormal3f(-0.948678,0.316243,0);
glVertex3f(-0.6247,-0.175218,-0.472749);
glNormal3f(0.316209,0.94869,0);
glVertex3f(-0.622397,-0.172914,-0.472749);
glEnd();
//no I didn't handcraft these, in case you were wondering
}
