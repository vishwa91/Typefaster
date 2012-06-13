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
#ifndef BUTTONLINE_H
#define BUTTONLINE_H
/*
#include <iostream>

class ButtonPoint
{
public:
	int** x;
	int** y;
	int* interx;
	int* intery;
	ButtonPoint(int** a,int** b) 
	{
		if(a==0)
		{
			interx = new int(0);
			x = &interx;
		}
		else
			x=a;
		
		if(b==0)
		{
			intery=new int(0);
			y=&intery;
		}
		else
			y=b;
}
	~ButtonPoint()
	{
		if(*x!=0)
		{
			delete *x;
			*x=0;
		}
		if(*y!=0)
		{
			delete *y;
			*y=0;
		}
	}
};
class ButtonLine
{

public:
	enum direction
	{
		north,
		east,
		south,
		west
	};
	
	int* fromx;
	int* fromy;
	int* tox;
	int* toy;
	
	direction dir;
	float pbutton;
	bool horiz;
	ButtonLine(ButtonLine* prev,direction d,float p=0.0);
	~ButtonLine();
	void resize(int,int);
	void setLength(int);

	int length;
	
private:
	bool first;
	ButtonPoint* one; 
	ButtonPoint* two;
	
	
};
*/
class ButtonPoint
{
public:
	int x;
	int y;
	ButtonPoint(int a,int b) 
	{
		x=a;
		y=b;
	}

};
class ButtonLine
{

public:
	enum direction
	{
		north,
		east,
		south,
		west
	};
	
	int* fromx;
	int* fromy;
	int* tox;
	int* toy;
	
	direction dir;
	float pbutton;
	bool horiz;
	ButtonLine(ButtonLine* prev,direction d,float p=0.0);
	~ButtonLine();
	void resize(int,int);
	void setLength(int);
	int length;
	
private:
	bool first;
	ButtonPoint* one;
	ButtonPoint* two;
	
	
};
#endif
