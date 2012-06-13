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
#include "studentstats.h"

studentstats::studentstats(QString s):QObject()
		{
			studentname = s;
			nodata = true;
			removebutton = 0;
		//	removebutton = new QPushButton(tr("Remove this student"),w);
		//	removebutton->setGeometry(20,20,90,30);
		//	connect(removebutton,SIGNAL(clicked()),this,SLOT(removepressed()));
		//	connect(this,SIGNAL(removepressed(QString)),w,SLOT(removepressedslot(QString)));
		}

void studentstats::createbutton(QWidget* w)
{
		removebutton = new QPushButton(tr("Exclude from statistics"),w);
		connect(removebutton,SIGNAL(clicked()),this,SLOT(removepressed()));
		connect(this,SIGNAL(removepressed(QString)),w,SLOT(removepressedslot(QString)));
}

void studentstats::calccolours(float x,int& r,int& g,int& b)
{
		int y = (int)(x*390);
		int atg = y; //add to green
		int tfr = 0; //take from red
		if(y>195)
		{
			atg = 195;
			tfr = y-195;
		}

		r = 205 - tfr;
		g = 10 + atg;
		b = 10;
}

studentstats::studentstats(QString st,float t,float a,float s):QObject()
		{
			studentname = st;
			nodata = false;

			tried = t;
			calccolours(t,tred,tg,tb);
			accuracy = a;
			calccolours(a,ar,ag,ab);
			speed = s;
			calccolours(s,sr,sg,sb);

			removebutton = 0;
	//		removebutton = new QPushButton(tr("Remove this student"),w);
	//		removebutton->setGeometry(20,20,90,30);
	//		connect(removebutton,SIGNAL(clicked()),this,SLOT(removepressed()));
	//		connect(this,SIGNAL(removepressed(QString)),w,SLOT(removepressedslot(QString)));
		
		}

void studentstats::removepressed()
{
	emit removepressed(studentname);
}

studentstats::~studentstats()
{
	if(removebutton!=0)
		delete removebutton;
}
