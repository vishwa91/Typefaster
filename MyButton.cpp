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
 
#include "MyButton.h"
#include "ButtonLine.h"
#include <qpainter.h>
#include <qmessagebox.h> 
#include <qstringlist.h>
//#include <cstdlib>
#include <stdio.h>	// Added by vishwanath on 6/5/12
#include <stdlib.h>
// exit(1) replaced by atexit(error_message)
//#include <iostream>
#include <vector>
using namespace std;

//crazy c++ definition of static members

void error_message(void)
{
	puts("Error in function");
}

float MyButton::kwidth = 0.0;
float MyButton::kheight = 0.0;
float MyButton::horizgap = 0.0;
float MyButton::vertgap = 0.0;

std::vector<MyButton*> MyButton::mybuttons;

MyButton::MyButton(shapetype sh,std::vector<keytype> ke,bool ho,QString size,int hom,float va,float vd,QWidget* parent,const char* nam, WFlags f):QButton(parent,nam,f)
{
	mybuttons.push_back(this);
	currentstyle = SolidPattern;

	runbw = false;
	runtext = false;
	runbounding = false;
	//cout << "Button constructor: "<<sh << " "<< ke<<" "<<ho<<" "<<re<<" "<<size<<" "<<hom<<" "<<va<<" "<<vd<<endl;
	drawtext=true;
	highlighted=false;
	homehighlight=false;
//	rightborder = true;
//	botborder = true;
	doublelayered = false;
	toplayer = "";
	
	shape = sh;
	type = ke;
	homekey = ho;
	homeindex = hom;
	vacross = va;
	vdown = vd;

	vwidth=0;
	vheight=0;

	bool found = false;
	for(int i=0;i<type.size();i++)
	{
	switch(type[i])
	{
	case normal:
		found=true;
		break;
	case altgr:
		found=true;
		name = "altgr";
		break;
	case alt:
		name = "alt";
		found=true;
		break;
	case enter:
#ifdef SPANISH
		name = "Intro";
#else
		name = "enter";
#endif
		found=true;
		break;
	case backspace:
		name = "backspace";
		found=true;
		break;
	case tab:
		name = "tab";
		found=true;
		break;
	case capslock:
#ifdef SPANISH
		name = "Bloq Mayús";
#else
	    name = "caps lock";
#endif
		found=true;
		break;
	case control:
		name = "ctrl";
		found=true;
		break;
	case leftshift:
		name = "shift";
		found=true;
		break;
	case rightshift:
		name = "shift";
		found=true;
		break;
	case numlock:
		doublelayered = true;
#ifdef SPANISH
		name = "Bloq\nNum";
		toplayer = "Bloq";
#else
		name = "num\nlock";		
		toplayer = "lock"; //it's longer than num
#endif
		found=true;
		break;
	default:
		name = "";
	}
	if(found)
		break;
	}

	QStringList sl = QStringList::split(";",size);
	QStringList::Iterator it;
	bool ok=false;
	switch(shape)
	{
	case square:
		vwidth = size.toFloat(&ok);
		if(!ok)
		{
			//std::cout << "Error, square button has non-float size" << std::endl;
			QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, square button has non-float size");
			//no translation necessary
			atexit(error_message);
		}
		else
		{
			vheight = vwidth;
			ButtonLine* b = new ButtonLine(0,ButtonLine::east,1.0);
			all.push_back(b);
			east.push_back(b);
			b = new ButtonLine(all[0],ButtonLine::south,1.0);
			all.push_back(b);
			south.push_back(b);
			b = new ButtonLine(all[1],ButtonLine::west,1.0);
			all.push_back(b);
			west.push_back(b);
			b = new ButtonLine(all[2],ButtonLine::north,1.0);
			all.push_back(b);
			north.push_back(b);
		}
		break;
	case rect:
		bool ok;
		it = sl.begin();
		if(it==sl.end() || it==0)
		{
			//std::cout << "Error, rect button doesn't have size"<<std::endl;
			QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, rect button doesn't have size");
			//no translation necessary
			atexit(error_message);
		}
		else
		{
		//	vwidth = atof((*it).latin1());
			vwidth = (*it).toFloat(&ok);
			if(!ok)
			{
				//std::cout << "Error, rect button has non-float width"<<std::endl;
				QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, rect button has non-float width");
				//no translation necessary
				atexit(error_message);
			}
			else
			{
				it++;
				if(it==sl.end() || it==0)
				{
					//std::cout << "Error, rect button has no part 2 to size"<<std::endl;
						QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, rect button has no part 2 to size");
						//no translation necessary
						atexit(error_message);
				}
				else
				{
					//vheight = atof((*it).latin1());
					vheight = (*it).toFloat(&ok);
					if(!ok)
					{
						//std::cout << "Error, rect button has non-float height"<<std::endl;
						QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, rect button has non-float height");
						//no translation necessary
						atexit(error_message);
					}
					else
					{
						ButtonLine* b = new ButtonLine(0,ButtonLine::east,1.0);
						all.push_back(b);
						east.push_back(b);
						b = new ButtonLine(all[0],ButtonLine::south,1.0);
						all.push_back(b);
						south.push_back(b);
						b = new ButtonLine(all[1],ButtonLine::west,1.0);
						all.push_back(b);
						west.push_back(b);
						b = new ButtonLine(all[2],ButtonLine::north,1.0);
						all.push_back(b);
						north.push_back(b);
					}
				}
			}
			
		}
		break;
	case irregular:
		
		std::vector<float> vsize;
		std::vector<bool> width;

		int i = 0;
		ButtonLine* b;
		for(it=sl.begin();it != sl.end(); ++it)
		{
			QString t = *it;
			t.truncate(t.length()-1);
			float tmp = t.toFloat(&ok);
			if(!ok)
			{
				//std::cout << "Error, irregular button has non float in size"<<std::endl;
				QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, irregular button has non-float in size");
				//no translation necessary
				atexit(error_message);
			}
			else
			{
				switch((*it).at((*it).length()-1).latin1())
				{
				case 'N':
					if(i>0)
						b = new ButtonLine(all[i-1],ButtonLine::north);
					else
						b = new ButtonLine(0,ButtonLine::north);
					all.push_back(b);
					north.push_back(b);
					vsize.push_back(tmp);
					width.push_back(false);
					break;
				case 'E':
					if(i>0)
						b = new ButtonLine(all[i-1],ButtonLine::east);
					else
						b = new ButtonLine(0,ButtonLine::east);
					all.push_back(b);
					east.push_back(b);
					vwidth += tmp;
					vsize.push_back(tmp);
					width.push_back(true);
					break;
				case 'S':
					if(i>0)
						b = new ButtonLine(all[i-1],ButtonLine::south);
					else
						b = new ButtonLine(0,ButtonLine::south);
					all.push_back(b);
					south.push_back(b);
					vheight += tmp;
					vsize.push_back(tmp);
					width.push_back(false);
					break;
				case 'W':
					if(i>0)
						b = new ButtonLine(all[i-1],ButtonLine::west);
					else
						b = new ButtonLine(0,ButtonLine::west);
					all.push_back(b);
					west.push_back(b);
					vsize.push_back(tmp);
					width.push_back(true);
					break;
				default:
					//std::cout << "Error, irregular button doesn't end in N,E,S or W"<<std::endl;
					QMessageBox::critical(0,"TypeFaster Typing Tutor","Parse fatal error, irregular button doesn't end in N,E,S or W");
					//no translation necessary
					atexit(error_message);
				}
			}
			i++;
		}
		for(i=0;i<all.size();i++)
		{
			if(width[i])
				all[i]->pbutton = vsize[i]/vwidth;
			else
				all[i]->pbutton = vsize[i]/vheight;
		}
		break;
	}
	
}
MyButton::~MyButton()
{
//	cout << "inside button destructor " <<endl;
	for(int i=0;i<all.size();i++)
	{
			delete all[i];
	}
}
void MyButton::drawButton(QPainter* p)
{
	if(!isDown())
	{
		p->setPen(QColor(255,255,255));
		int i;
		for(i=0;i<whites.size();i++)
		{
			p->drawLine(*whites[i]->fromx,*whites[i]->fromy,*whites[i]->tox,*whites[i]->toy);
		}

		p->setPen(QColor(64,64,64));
		for(i=0;i<blacks.size();i++)
		{
			p->drawLine(*blacks[i]->fromx,*blacks[i]->fromy,*blacks[i]->tox,*blacks[i]->toy);
		}
	
		p->setPen(QColor(128,128,128));
		for(i=0;i<blacks.size();i++)
		{
			if(blacks[i]->horiz)
				p->drawLine((*blacks[i]->fromx)+1,(*blacks[i]->fromy)-1,(*blacks[i]->tox)-1,(*blacks[i]->toy)-1);
			else
				p->drawLine((*blacks[i]->fromx)-1,(*blacks[i]->fromy)+1,(*blacks[i]->tox)-1,(*blacks[i]->toy)-1);
		}
		if(highlighted)
		{
			if(shape==square)
				p->fillRect(1,1,all[0]->length-2,all[0]->length-2,QBrush("red",currentstyle));
			else
				if(shape==rect)
					p->fillRect(1,1,all[0]->length-2,all[1]->length-2,QBrush("red",currentstyle));
		}
		if(homekey) //if a home row key, give a blue border
		{
			/*
			p->setPen("blue");
			for(i=0;i<whites.size();i++)
				if(whites[i]->horiz)
				{
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+1,*whites[i]->tox-2,*whites[i]->toy+1);
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+2,*whites[i]->tox-2,*whites[i]->toy+2);
				}
				else
				{
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+1,*whites[i]->tox+1,*whites[i]->toy-2);
					p->drawLine(*whites[i]->fromx+2,*whites[i]->fromy+1,*whites[i]->tox+2,*whites[i]->toy-2);
				}

			for(i=0;i<blacks.size();i++)
				if(blacks[i]->horiz)
				{
					p->drawLine((*blacks[i]->fromx)+1,(*blacks[i]->fromy)-2,(*blacks[i]->tox)-2,(*blacks[i]->toy)-2);
					p->drawLine((*blacks[i]->fromx)+1,(*blacks[i]->fromy)-3,(*blacks[i]->tox)-2,(*blacks[i]->toy)-3);
				}
				else
				{
					p->drawLine((*blacks[i]->fromx)-2,(*blacks[i]->fromy)+1,(*blacks[i]->tox)-2,(*blacks[i]->toy)-2);
					p->drawLine((*blacks[i]->fromx)-3,(*blacks[i]->fromy)+1,(*blacks[i]->tox)-3,(*blacks[i]->toy)-2);
				}*/
			p->setPen("blue");
			for(i=0;i<whites.size();i++)
				if(whites[i]->horiz)
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+1,*whites[i]->tox-2,*whites[i]->toy+1);
				else
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+1,*whites[i]->tox+1,*whites[i]->toy-2);

			for(i=0;i<blacks.size();i++)
				if(blacks[i]->horiz)
					p->drawLine((*blacks[i]->fromx)+1,(*blacks[i]->fromy)-2,(*blacks[i]->tox)-2,(*blacks[i]->toy)-2);
				else
					p->drawLine((*blacks[i]->fromx)-2,(*blacks[i]->fromy)+1,(*blacks[i]->tox)-2,(*blacks[i]->toy)-2);

		/*	if(homehighlight)
				p->setPen("yellow");*/
			for(i=0;i<whites.size();i++)
				if(whites[i]->horiz)
					p->drawLine(*whites[i]->fromx+1,*whites[i]->fromy+2,*whites[i]->tox-2,*whites[i]->toy+2);
				else
					p->drawLine(*whites[i]->fromx+2,*whites[i]->fromy+1,*whites[i]->tox+2,*whites[i]->toy-2);

			for(i=0;i<blacks.size();i++)
				if(blacks[i]->horiz)
					p->drawLine((*blacks[i]->fromx)+1,(*blacks[i]->fromy)-3,(*blacks[i]->tox)-2,(*blacks[i]->toy)-3);
				else
					p->drawLine((*blacks[i]->fromx)-3,(*blacks[i]->fromy)+1,(*blacks[i]->tox)-3,(*blacks[i]->toy)-2);

		}
	}
	else //invert blacks and whites
	{
		p->setPen(QColor(255,255,255));
		int i;
		for(i=0;i<blacks.size();i++)
			p->drawLine(*blacks[i]->fromx,*blacks[i]->fromy,*blacks[i]->tox,*blacks[i]->toy);

		p->setPen(QColor(64,64,64));
		for(i=0;i<whites.size();i++)
			p->drawLine(*whites[i]->fromx,*whites[i]->fromy,*whites[i]->tox,*whites[i]->toy);
	
		p->setPen(QColor(128,128,128));//gray
		for(i=0;i<whites.size();i++)
		{
			if(whites[i]->horiz)
				p->drawLine((*whites[i]->fromx)+1,(*whites[i]->fromy)+1,(*whites[i]->tox)-1,(*whites[i]->toy)+1);
			else
				p->drawLine((*whites[i]->fromx)+1,(*whites[i]->fromy)+1,(*whites[i]->tox)+1,(*whites[i]->toy)-1);
		}
		if(highlighted)
		{
			if(shape==square)
				p->fillRect(2,2,all[0]->length-2,all[0]->length-2,QBrush("red",currentstyle));
			else
				if(shape==rect)
					p->fillRect(2,2,all[0]->length-2,all[1]->length-2,QBrush("red",currentstyle));
		}
		if(homekey)
		{
			p->setPen("blue");
			for(i=0;i<whites.size();i++)
				if(whites[i]->horiz)
				{
					p->drawLine(*whites[i]->fromx+2,*whites[i]->fromy+2,*whites[i]->tox-1,*whites[i]->toy+2);
					p->drawLine(*whites[i]->fromx+2,*whites[i]->fromy+3,*whites[i]->tox-1,*whites[i]->toy+3);
				}
				else
				{
					p->drawLine(*whites[i]->fromx+2,*whites[i]->fromy+2,*whites[i]->tox+2,*whites[i]->toy-1);
					p->drawLine(*whites[i]->fromx+3,*whites[i]->fromy+2,*whites[i]->tox+3,*whites[i]->toy-1);
				}

			for(i=0;i<blacks.size();i++)
				if(blacks[i]->horiz)
				{
					p->drawLine((*blacks[i]->fromx)+2,(*blacks[i]->fromy)-1,(*blacks[i]->tox)-1,(*blacks[i]->toy)-1);
					p->drawLine((*blacks[i]->fromx)+2,(*blacks[i]->fromy)-2,(*blacks[i]->tox)-1,(*blacks[i]->toy)-2);
				}
				else
				{
					p->drawLine((*blacks[i]->fromx)-1,(*blacks[i]->fromy)+2,(*blacks[i]->tox)-1,(*blacks[i]->toy)-1);
					p->drawLine((*blacks[i]->fromx)-2,(*blacks[i]->fromy)+2,(*blacks[i]->tox)-2,(*blacks[i]->toy)-1);
				}

		}

	}
	if(drawtext)
		drawButtonLabel(p);	
}

void MyButton::drawButtonLabel(QPainter* p)
{
	
	QFont f("Arial"); //do not translate
	f.setPointSizeFloat(fontsize);
	p->setFont(f);

	//p->setBrush(currentstyle);

	if(homehighlight)
		p->setPen("red");
	else 
		p->setPen("black");
	
	if(!isDown())
		p->drawText(minx-1,textfromy-1,textwidth+8,textheight+8,AlignCenter,name);
	else
		p->drawText(minx,textfromy,textwidth+8,textheight+8,AlignCenter,name);


}
bool MyButton::hitButton ( const QPoint & pos ) const
{//once again, not general, will work for the enter buttons I envisage though
	if(pos.x()<maxx && pos.x()>minx && pos.y()<maxy && pos.y()>miny)
	{
		if(shape==square || shape==rect)
			return true;
		else
		{
			int i;
			for(i=0;i<whites.size();i++)
			{
				if(!whites[i]->horiz)
				{
					if(pos.x()< *whites[i]->fromx && pos.y()> *whites[i]->fromy && pos.y()< *whites[i]->toy)
						return false;
				}
			}
			for(i=0;i<blacks.size();i++)
			{
				if(!blacks[i]->horiz)
				{
					if(pos.x()> *blacks[i]->fromx && pos.y()> *blacks[i]->fromy && pos.y()< *blacks[i]->toy)
						return false;
				}
			}
			return true;

		}
	}
	else return false;
}
/*QSize MyButton::sizeHint() const
{
//	char* c = new char(10);
//	sprintf(c,"%d",maxx-minx);
//	 QMessageBox::information(0,"caption",c);
//	return QSize(maxx-minx+1,maxy-miny+1);
}*/
void MyButton::calcbw()
{
		runbw = true;
	for (int i=0;i<all.size();i++)
	{		
		bool black=false;
		if(all[i]->horiz)
		{
			hor.push_back(all[i]);
			int x = (*all[i]->fromx)+1;
			int y = *all[i]->fromy;
			for(int j=0;j<all.size();j++)
			{
				if(all[j]->horiz)				
					//note the following test requires fromx < tox
					if(x<*all[j]->tox && x>*all[j]->fromx && y>*all[j]->fromy) 
					{
						black=true;//found something above, so it's an in-out ie black
						blacks.push_back(all[i]);
						break;
					}
			}
			if(!black)
				whites.push_back(all[i]);
		}
		else
		{
			ver.push_back(all[i]);
			int x = *all[i]->fromx;
			int y = (*all[i]->fromy)+1;
			for(int j=0;j<all.size();j++)
			{
				if(!all[j]->horiz)
					if(y>*all[j]->fromy && y<*all[j]->toy && x>*all[j]->fromx)
					{
						black = true;
						blacks.push_back(all[i]);
						break;
					}
			}
			if(!black)
				whites.push_back(all[i]);
		}
	}
}
void MyButton::calcText()
{
	int ty;
	if(!runtext)
	{
		runtext = true;
		if(*ver[iminx]->fromy > *ver[imaxx]->fromy)		
		{
			textfromy=*ver[iminx]->fromy;
			itextfromy=iminx;
		}
		else
		{
			textfromy=*ver[imaxx]->fromy;
			itextfromy=imaxx;
		}

		if(*ver[iminx]->toy < *ver[imaxx]->toy)
		{
			ty= *ver[iminx]->toy;
			ity=iminx;
		}
		else
		{
			ty= *ver[imaxx]->toy;
			ity=imaxx;
		}
	}
	else
	{
		textfromy= *ver[itextfromy]->fromy;
		ty= *ver[ity]->toy;
	}
		textwidth = maxx-minx-8; //just to give a 4 pixel border
		textheight = ty-textfromy-8;

		int wtextheight;

		if(doublelayered)
			wtextheight = textheight/2;
		else
			wtextheight = textheight;

		int initsize=18;
		QFont f("Arial",initsize); // do not translate
		QFontMetrics fm(f);
		QRect r;
		if(doublelayered)
			r = fm.boundingRect(toplayer);
		else
			r = fm.boundingRect(name);

		if((float)r.width()/(float)r.height() > (float)textwidth/(float)wtextheight)
		{
			float scale = (float)textwidth/(float)r.width();
			fontsize=scale*initsize;
		}
		else
		{
			float scale = (float)wtextheight/(float)r.height();
			fontsize=scale*initsize;
		}
	
}
void MyButton::calcBounding()
{
	if(!runbounding)
	{
		runbounding = true;
    	iminx=0;
    	imaxx=0;
		iminy=0;
		imaxy=0;
    	if(ver.size()>0)
    	{
    		minx=*ver[0]->fromx;
    		maxx=*ver[0]->fromx;
    	}
    	if(hor.size()>0)
    	{
    		miny=*hor[0]->fromy;
    		maxy=*hor[0]->fromy;
    	}
		int i;
    	for(i=1;i<ver.size();i++)
    	{
    		if(*ver[i]->fromx < minx)
    		{
    			minx=*ver[i]->fromx;
    			iminx=i;
    		}
    		else
    		  if(*ver[i]->fromx > maxx)
    		  {
    			maxx = *ver[i]->fromx;
    			imaxx=i;
    		  }
    	}
    	for(i=1;i<hor.size();i++)
    	{
    		if(*hor[i]->fromy < miny)
			{
    			miny= *hor[i]->fromy;
				iminy=i;
			}
    		else
    			if(*hor[i]->fromy > maxy)
				{
    				maxy= *hor[i]->fromy;
					imaxy=i;
				}
    	}
	}
	else
	{
//		cout << "Inside second run MyButton::calcBounding(): name "<<name<<endl;
		maxx=*ver[imaxx]->fromx;
		minx=*ver[iminx]->fromx;

		maxy=*hor[imaxy]->fromy;
		miny=*hor[iminy]->fromy;
	}
//	QString tmp;
//	tmp.sprintf("%d",maxy-miny);
//	QMessageBox::information(0,"Bound",tmp);

}
void MyButton::resizeEvent(QResizeEvent* r)
{
//	cout << "button got resize event" << endl;
//	cout << r->size().width()<<" "<<r->size().height()  << endl;
//	QString s;
//	s.sprintf("width %d height %d",r->size().width(),r->size().height());
//	QMessageBox::information(0,"caption",s);
//	int offwidth=0;
//	int offheight=0;
/*	if(botborder)
		offheight = (int)(horizgap*(float)r->size().height())/(vheight+horizgap);
	if(rightborder)
		offwidth = (int)(vertgap*(float)r->size().width())/(vwidth+vertgap);*/
	int i;
	for(i=0;i<all.size();i++)
	{
	//	cout << "resizing "<<name << " buttonline "<<i << endl;
		all[i]->resize(r->size().width()-1,r->size().height()-1);
	}

	//now just make last west so that sum east ==  sum west
	int lastwest=0;
	for(i=0;i<east.size();i++)
	{
		lastwest += east[i]->length;
	}
	int onelesswestsize = west.size()-1;
	for(i=0;i<onelesswestsize;i++)
		lastwest -= west[i]->length;
	if(west.size()>0)
		west[west.size()-1]->setLength(lastwest);

	int lastnorth=0;
	for(i=0;i<south.size();i++)
		lastnorth += south[i]->length;
	int onelessnorthsize = north.size()-1;
	for(i=0;i<onelessnorthsize;i++)
		lastnorth -= north[i]->length;
	if(north.size()>0)
		north[north.size()-1]->setLength(lastnorth);

	if(!runbw)
		calcbw();
	calcBounding();
	calcText();
}
void MyButton::addvalue(QString n,std::vector<keytype> when,bool draw,bool newline,QString s)
{
	values.push_back(value(n,when,s));
	if(draw)
	{
		if(newline)
		{
			if(doublelayered)
			{
				QMessageBox::information(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Parse warning, there can only be one newline per button"));
				name += " "+n;
			}
			else
			{
				doublelayered = true;
				name += "\n" + n;
			}
		}
		else
		{
			if(name == "")
				name = n;
			else
				name += " "+n;
			if(!doublelayered)
			{
				if(toplayer=="")
					toplayer = n;
				else
					toplayer += " " + n;
			}
		}
	}
}
void MyButton::calcps()
{
	pacross = vacross/kwidth;
	pdown = vdown/kheight;
/*	if(botborder)
		pheight = (vheight+horizgap)/kheight;
	else*/
		pheight = vheight/kheight;
/*	if(rightborder)
		pwidth = (vwidth+vertgap)/kwidth;
	else*/
		pwidth = vwidth/kwidth;
//	cout << "raw mat in calcps "<<vacross<< " " << vdown<< " "<<kwidth<< " "<<kheight << endl;
//	cout << "in calcps "<<pacross<<" "<<pdown<<" "<<pheight<<" "<<pwidth<<endl;
}
void MyButton::highlight(BrushStyle bs)
{
	currentstyle = bs;
	highlighted = true;
	if(homeindex>-1 && homeindex<mybuttons.size() && mybuttons[homeindex]->homekey)
	{
		mybuttons[homeindex]->homehighlight = true;
	//	mybuttons[homeindex]->currentstyle = currentstyle;
		mybuttons[homeindex]->repaint();
	}
}
void MyButton::unHighlight()
{
	highlighted = false;
	//currentstyle = SolidPattern;
	if(homeindex>-1 && homeindex<mybuttons.size() && mybuttons[homeindex]->homekey)
	{
		mybuttons[homeindex]->homehighlight = false;
	//	mybuttons[homeindex]->currentstyle = SolidPattern;
		mybuttons[homeindex]->repaint();
	}
}
