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
#ifndef MYBUTTON_H
#define MYBUTTON_H

#include "ButtonLine.h"
#include <qbutton.h>
#include <qabstractlayout.h>
#include <qmessagebox.h>
#include <vector>
//#include <iostream>
//using namespace std;

class MyButton : public QButton
{
public:
	enum shapetype
	{
		irregular,
		square,
		rect
	};
	enum keytype
	{
		normal,
		altgr,
		alt,
		enter,
		backspace,
		tab,
		capslock,
		control,
		leftshift,
		rightshift,
		numlock,
		forwardaccent,
		doubledot,
		hat,
		backwardaccent,
		squiggle,
		cedilla,
		caron,
		breve,
		degreesign,
		ogonek,
		dotabove,
		doubleacuteaccent
	};
	static float kwidth; 
	static float kheight;
	static float horizgap;
	static float vertgap;

	static std::vector<MyButton*> mybuttons; //they all know about each other,
	//used for highlighting its homekey when it is told to highlight

	bool drawtext;
//	bool rightborder;
//	bool botborder;

	shapetype shape;

	float pacross;
	float pdown;
	float pheight;
	float pwidth;

	MyButton(shapetype s,std::vector<keytype>,bool,QString size,int,float va,float vd,QWidget* parent=0,const char* nam=0, WFlags f=0);
	//need a std::vector because e.g in Spanish the same key is forward accent and doubledot
	~MyButton();
	void drawButton(QPainter*);
	void drawButtonLabel(QPainter*);
//	QSize sizeHint() const;
	bool hitButton ( const QPoint & pos ) const;
	void resizeEvent(QResizeEvent*);
/*	void setGeometry(const QRect& rect)
	{
	//	cout << "button set geometry called" << endl;
	//	QMessageBox::information(0,"button class","set Geom called");
	}*/

	void addvalue(QString,std::vector<keytype>,bool,bool,QString);
	//the reason need a std::vector is because e.g. in Spanish you need to press shift,
	//then doubledot,then the key you want a double dot on top of

	void calcps();
	
	QString name;

	struct value
	{
		QString val;
		std::vector<keytype> when;
		QString say;
		value(QString v,std::vector<keytype> w,QString s)
		{
			val = v;
			when = w;
			say = s;
		}
	};
	std::vector<value> values;

	std::vector<keytype> type;

	void highlight(BrushStyle);
	void unHighlight();
	bool homekey;
	int homeindex;
private:
	BrushStyle currentstyle;
	void calcText();
	void calcBounding();
	void calcbw();

	std::vector<ButtonLine*> blacks;
	std::vector<ButtonLine*> whites;
	std::vector<ButtonLine*> all;
	std::vector<ButtonLine*> hor;
	std::vector<ButtonLine*> ver;

	std::vector<ButtonLine*> north;
	std::vector<ButtonLine*> south;
	std::vector<ButtonLine*> east;
	std::vector<ButtonLine*> west;

	bool highlighted;
	bool homehighlight;

	
	float vacross;
	float vdown;

	float vwidth;
	float vheight;


	
	int minx;
	int iminx;
	int maxx;
	int imaxx;

	int miny;
	int iminy;
	int maxy;
	int imaxy;

	int textfromy;
	int itextfromy;

	int ity;

	int textwidth;
	int textheight;
	float fontsize;
	
	bool runbw;
	bool runtext;
	bool runbounding;


	bool doublelayered;
	QString toplayer;
};

#endif
