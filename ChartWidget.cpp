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
 


#include "ChartWidget.h"
#include <qpainter.h>
#include <qdict.h>
#include "numandtime.h"
#include <algorithm>
#include <iostream>
using namespace std;

extern QDict<bool>* lettersonlydict;

ChartWidget::ChartWidget(QWidget* parent, QCheckBox* l):QWidget(parent)
{
	letters = l;
	lettersonlyt = false;
	connect(l,SIGNAL(toggled(bool)),this,SLOT(letterstoggled(bool)));
	//setBackgroundColor("white");
	//setFixedSize(109*10+30,500);
	setMaximumWidth(4000);
	setFixedHeight(800);

}
void ChartWidget::letterstoggled(bool on)
{
	bool* u = lettersonlydict->find(heading);
	if(u!=0)
	{
		*u = on;
	}
	else
	{
		bool* n =  new bool;
		*n = on;
		lettersonlydict->insert(heading,n);
	}
	lettersonlyt = on;
	doCalc(all);
	repaint();
}
void ChartWidget::setletters(bool o)
{
	letters->setChecked(o);
	lettersonlyt = o;
}
void ChartWidget::doCalc(vector<numandtime*> vec)
{
	resize(140+40+20*vec.size()+100,700);

	all.clear();
	all = vec;

	useful.clear();
	nuseful.clear();
	int i;
	for(i=0;i<vec.size();i++)
		if((lettersonlyt && vec[i]->c.isLetter())||!lettersonlyt)
			if(vec[i]->num>0)
				useful.push_back(vec[i]);
			else
				nuseful.push_back(vec[i]);


	int j;
	int test = useful.size()-1; 
	for(i=0;i<test ;i++)
	{
		for(j=i+1;j<useful.size();j++)
		{
			if( ((useful[i]->c > useful[j]->c) && !(useful[j]->c.category()==QChar::Letter_Uppercase && useful[i]->c.category()==QChar::Letter_Lowercase) && !(!useful[j]->c.isLetter() && useful[i]->c.isLetter())) || 
				 (useful[i]->c.category()==QChar::Letter_Uppercase && useful[j]->c.category()==QChar::Letter_Lowercase) || \
				 (!useful[i]->c.isLetter() && useful[j]->c.isLetter())
			  )
			{
				numandtime* tmp = useful[i];
				useful[i] = useful[j];
				useful[j] = tmp;
			}
		}
	}


	test = nuseful.size()-1;
	for(i=0;i<test;i++)
		for(j=i+1;j<nuseful.size();j++)
		{
			if( ((nuseful[i]->c > nuseful[j]->c) && !(nuseful[j]->c.category()==QChar::Letter_Uppercase && nuseful[i]->c.category()==QChar::Letter_Lowercase) && !(!nuseful[j]->c.isLetter() && nuseful[i]->c.isLetter())) || 
				 (nuseful[i]->c.category()==QChar::Letter_Uppercase && nuseful[j]->c.category()==QChar::Letter_Lowercase) || \
				 (!nuseful[i]->c.isLetter() && nuseful[j]->c.isLetter())
			  )
			{
				numandtime* tmp = nuseful[i];
				nuseful[i] = nuseful[j];
				nuseful[j] = tmp;
			}
		}


	int pixelheight = 300; //this is how big our graph is in pixel height
	for(i=0;i<useful.size();i++)
	{

		float perc = (useful[i]->totaltime/(float)useful[i]->num)/3000;
		useful[i]->sheight = (int)(perc*pixelheight);

		int y = (int)(390-perc*390);
		int atg = y; //add to green
		int tfr = 0; //take from red
		if(y>195)
		{
			atg = 195;
			tfr = y-195;
		}

		useful[i]->sr = 205 - tfr;
		useful[i]->sg = 10 + atg;
		useful[i]->sb = 10;

		perc = useful[i]->num/((float)useful[i]->num+(float)useful[i]->missed);
		useful[i]->aheight = (int)(perc*pixelheight);

		y = (int)(perc*390);
		atg = y; //add to green
		tfr = 0; //take from red
		if(y>195)
		{
			atg = 195;
			tfr = y-195;
		}

		useful[i]->ar = 205 - tfr;
		useful[i]->ag = 10 + atg;
		useful[i]->ab = 10;
	
	}
	slowest = "";
	worst = "";

	vector<numandtime*> slowestv = useful;
	int max = slowestv.size()<5 ? slowestv.size() : 5;
	for(i=0;i<max;i++)
	{
		for(j=i+1;j<slowestv.size();j++)
			if(slowestv[j]->sheight > slowestv[i]->sheight)
			{
				numandtime* tmp = slowestv[j];
				slowestv[j] = slowestv[i];
				slowestv[i] = tmp;
			}
		slowest += slowestv[i]->c;
	}

	vector<numandtime*> worstv = useful;
	for(i=0;i<max;i++)
	{
		for(j=i+1;j<worstv.size();j++)
			if(worstv[j]->aheight < worstv[i]->aheight)
			{
				numandtime* tmp = worstv[j];
				worstv[j] = worstv[i];
				worstv[i] = tmp;
			}
		worst += worstv[i]->c;
	}

}
void ChartWidget::paintEvent (QPaintEvent* pe)
{
	QPainter paint( this );
	paint.setFont(QFont("courier", 16, QFont::Bold));
    //paint.setPen( Qt::blue );
    paint.drawText( 250,25,heading+tr(" layout"));

	paint.setBrush(QBrush(QBrush::SolidPattern));

	paint.drawRect(125,40,3,300);

	paint.drawRect(125,340,5+20*useful.size()+20*nuseful.size(),3);

	paint.setFont(QFont("courier", 10));
	paint.drawText(5,55,65,85,AlignLeft,"Average\ntime\nneeded\nto press\n(ms)");

	paint.drawText(92,334,32,12,AlignRight,"0");
	paint.drawText(92,234,32,12,AlignRight,"1000");
	paint.drawText(92,134,32,12,AlignRight,"2000");
	paint.drawText(92,34,32,12,AlignRight,"3000");

	int i=0;
	for(i=0;i<useful.size();i++)
	{
		paint.setPen(QColor(0,0,0));
		paint.drawText(135+20*i,345,15,12,AlignCenter,useful[i]->c);
		paint.setPen(QColor(useful[i]->sr,useful[i]->sg,useful[i]->sb));
		paint.drawRect(140+20*i,340-useful[i]->sheight,3,useful[i]->sheight);
	}
	paint.setPen(QColor(0,0,0));
	int j;
	for(j=0;j<nuseful.size();j++)
		paint.drawText(135+20*i+20*j,345,15,12,AlignCenter,nuseful[j]->c);

	paint.drawText(5,365,200,20,AlignLeft,tr("Slowest: ")+slowest);


	int d = 400;
	paint.drawRect(125,d+40,3,300);

	paint.drawRect(125,d+340,5+20*useful.size()+20*nuseful.size(),3);

	paint.setFont(QFont("courier", 10));
	paint.drawText(5,d+55,65,85,AlignLeft,"Accuracy\n(%)");

	paint.drawText(92,d+334,32,12,AlignRight,"0");
	paint.drawText(92,d+234,32,12,AlignRight,"33");
	paint.drawText(92,d+134,32,12,AlignRight,"66");
	paint.drawText(92,d+34,32,12,AlignRight,"100");

	i=0;
	for(i=0;i<useful.size();i++)
	{
		paint.setPen(QColor(0,0,0));
		paint.drawText(135+20*i,d+345,15,12,AlignCenter,useful[i]->c);
		paint.setPen(QColor(useful[i]->ar,useful[i]->ag,useful[i]->ab));
		paint.drawRect(140+20*i,d+340-useful[i]->aheight,3,useful[i]->aheight);
	}
	paint.setPen(QColor(0,0,0));
	for(j=0;j<nuseful.size();j++)
		paint.drawText(135+20*i+20*j,d+345,15,12,AlignCenter,nuseful[j]->c);

	paint.drawText(5,d+365,200,20,AlignLeft,tr("Least accurate: ")+worst);
}
