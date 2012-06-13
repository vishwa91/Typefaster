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
 


#include "ClassChart.h"
#include <qpainter.h>
#include <qfile.h>
#include <qtextstream.h>
#include "studentstats.h"
#include <vector>
#include <iostream>
using namespace std;

extern vector<studentstats*> studentstatsv;
extern QString USERNAME;

ClassChart::ClassChart(QWidget* parent):QWidget(parent)
{

	setMaximumHeight(4000);
	setFixedWidth(800);

}
void ClassChart::paintEvent (QPaintEvent* pe)
{
	

	QPainter paint( this );
	paint.setFont(QFont("courier", 16, QFont::Bold));
    
	paint.drawText( 200,25,heading+tr(" layout class results"));	

	paint.setBrush(QBrush(QBrush::SolidPattern));

	paint.setFont(QFont("courier", 10));
	int i;
	for(i=0;i<studentstatsv.size();i++)
		paint.drawText(20,70+i*64,studentstatsv[i]->studentname+":");

	paint.setFont(QFont("courier",8));
	for(i=0;i<studentstatsv.size();i++)
	{
		paint.setPen(QColor(0,0,0));

		paint.drawText(180,i*64+46,65,16,AlignRight,tr("Attempted"));
		paint.drawText(180,i*64+46+16,65,16,AlignRight,tr("Accuracy"));
		paint.drawText(180,i*64+46+32,65,16,AlignRight,tr("Speed"));

		paint.drawRect(250,i*64+46,2,48);
		paint.drawRect(250,i*64+94,300,2);

		studentstatsv[i]->removebutton->setGeometry(600,55+i*64,120,30);

		if(studentstatsv[i]->nodata)
		{
			paint.setFont(QFont("courier",10));
			paint.drawText(255,70+i*64,tr("No data available"));
			paint.setFont(QFont("courier",8));
		}
		else
		{
			paint.setPen(QColor(studentstatsv[i]->tred,studentstatsv[i]->tg,studentstatsv[i]->tb));
			paint.drawRect(250,i*64+54,(int)(studentstatsv[i]->tried*300),3);

			paint.setPen(QColor(studentstatsv[i]->ar,studentstatsv[i]->ag,studentstatsv[i]->ab));
			paint.drawRect(250,i*64+54+16,(int)(studentstatsv[i]->accuracy*300),3);

			paint.setPen(QColor(studentstatsv[i]->sr,studentstatsv[i]->sg,studentstatsv[i]->sb));
			paint.drawRect(250,i*64+54+32,(int)(studentstatsv[i]->speed*300),3);
			
		}
	}

}

void ClassChart::removepressedslot(QString s)
{
	int i;
	vector<studentstats*>::iterator it = studentstatsv.begin();
	bool found = false;
	for(i=0;i<studentstatsv.size();i++)
	{
		if(studentstatsv[i]->studentname == s)
		{
			delete studentstatsv[i];
			studentstatsv.erase(it);
			found = true;
			break;
		}
		if(!found)
			it++;
	}
	repaint();

	QFile f("users/"+USERNAME+"/students.txt");	
	if(f.open(IO_WriteOnly))
	{
		QTextStream s(&f);
		for(i=0;i<studentstatsv.size();i++)
			s << studentstatsv[i]->studentname << endl;
		f.close();
	}
	
	
}
void ClassChart::hideEvent(QHideEvent* e)
{
	if(!e->spontaneous())
	{
	//	cout << "got a close event" << endl;
		int x;
		for(x=0;x<studentstatsv.size();x++)
			delete studentstatsv[x];
		studentstatsv.clear();
	}

}
