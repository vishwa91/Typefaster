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
#include "ButtonLine.h"
#include <qmessagebox.h>
#include <qapplication.h>
//#include <iostream>
/*
ButtonLine::ButtonLine(ButtonLine* prev,direction d,float p)
{
	dir=d;
	pbutton=p;
	if(prev!=0)
	{
		first=false;
		one = prev->two;
	}
	else
	{
		first = true;
		one = new ButtonPoint(0,0);
	}
	horiz=false;
	switch(dir)
	{
	case north:
		two=new ButtonPoint((one->x),0); 
		fromx=*(one->x);
		tox=*(one->x);
		fromy=*(two->y);
		toy=*(one->y);
		break;
	case east: 
		two=new ButtonPoint(0,(one->y)); 
		fromx=*(one->x);
		tox=*(two->x);
		fromy=*(one->y);
		toy=*(one->y);
		horiz=true; 
		break;
	case south:
		two=new ButtonPoint((one->x),0); 
		fromx=*(one->x);
		tox=*(one->x);
		fromy=*(one->y);
		toy=*(two->y);
		break;
	case west: two=new ButtonPoint(0,(one->y)); 
		fromx=*(two->x);
		tox=*(one->x);
		fromy=*(one->y);
		toy=*(one->y);
		horiz=true; 
		break;
	}
}
ButtonLine::~ButtonLine()
{
	if(first && one!=0) //one should never be 0 because only done on first
		delete one;
	if(two!=0) //two should never be 0
		delete two;
}
void ButtonLine::resize(int wi,int h)
{
	//std::cout << "ButtonLine resize: width "<<wi<<" height " << h << std::endl;
	if(pbutton!=0.0)
	{
		int l;
		switch (dir)
		{
		case north: 
		case south:
			l = (int)(pbutton*h); //it falls through, so if north or south will get this
			break;
		case east:
		case west:
			l = (int)(pbutton*wi);
		}
		setLength(l);
	}
	else
	{
		std::cout << "ButtonLine Fatal Error, pbutton has not been set" << std::endl;
		QMessageBox::critical(0,"ButtonLine Fatal Error","pbutton has not been set");
		exit(1);
	}
}
void ButtonLine::setLength(int l)
{
	switch(dir)
	{
	case north:
		**(two->y) = **(one->y)-l;
		break;
	case east:
		**(two->x) = **(one->x)+l;
		break;
	case south:
		**(two->y) = **(one->y)+l;
		break;
	case west:
		**(two->x) = **(one->x)-l;
	}
	length = l;
}
*/
ButtonLine::ButtonLine(ButtonLine* prev,direction d,float p)
{
	dir=d;
	pbutton=p;
	if(prev!=0)
	{
		first=false;
		one = prev->two;
	}
	else
	{
		first = true;
		one = new ButtonPoint(0,0);
	}
	horiz=false;
	switch(dir)
	{
	case north:
		two=new ButtonPoint(0,0); 
		fromx=&(one->x);
		tox=fromx;
		fromy=&(two->y);
		toy=&(one->y);
		break;
	case east: 
		two=new ButtonPoint(0,0); 
		fromx=&(one->x);
		tox=&(two->x);
		fromy=&(one->y);
		toy=fromy;
		horiz=true; 
		break;
	case south:
		two=new ButtonPoint(0,0); 
		fromx=&(one->x);
		tox=fromx;
		fromy=&(one->y);
		toy=&(two->y);
		break;
	case west: two=new ButtonPoint(0,0); 
		fromx=&(two->x);
		tox=&(one->x);
		fromy=&(one->y);
		toy=fromy;
		horiz=true; 
		break;
	}
}
ButtonLine::~ButtonLine()
{
	if(first && one!=0) //one should never be 0 because only done on first
		delete one;
	if(two!=0) //two should never be 0
		delete two;
}
void ButtonLine::resize(int wi,int h)
{
	//std::cout << "ButtonLine resize: width "<<wi<<" height " << h << std::endl;
	if(pbutton!=0.0)
	{
		int l;
		switch (dir)
		{
		case north: 
		case south:
			l = (int)(pbutton*h); //it falls through, so if north or south will get this
			break;
		case east:
		case west:
			l = (int)(pbutton*wi);
		}
		setLength(l);
	}
	else
	{
		QMessageBox::critical(0,"TypeFaster Typing Tutor","ButtonLine Fatal Error pbutton has not been set");
		//no translation necessary
		qApp->exit();
	}
}
void ButtonLine::setLength(int l)
{
	switch(dir)
	{
	case north:
		two->y = one->y-l;
		two->x = one->x;
		break;
	case east:
		two->x = one->x+l;
		two->y = one->y;
		break;
	case south:
		two->y = one->y+l;
		two->x = one->x;
		break;
	case west:
		two->x = one->x - l;
		two->y = one->y;
	}
	length = l;
}
