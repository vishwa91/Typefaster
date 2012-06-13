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
#include "MyLayout.h"
//#include "MyButton.h"
#include <qapplication.h>
#include <qmessagebox.h>
//#include <vector>
//#include <iostream>
//using namespace std;



MyLayout::MyLayout(QWidget *parent, float r,int dist): QLayout(parent, 0, dist)
{
	ratio = r; 
	//memoryleak = 0;
}
MyLayout::MyLayout(QLayout* parent, float r,int dist): QLayout(parent, dist)
{
	ratio = r;
	//memoryleak = 0;
}
QLayoutIterator MyLayout::iterator()
{       
	//cout << "iterator called " <<endl;
//	if(memoryleak != 0)
//		delete memoryleak;
//	memoryleak = new MyLayoutIterator(&list);
//	return QLayoutIterator(memoryleak);
	return QLayoutIterator( new MyLayoutIterator(&list) );//hopefully this isn't a memory leak
}

void MyLayout::addItem( QLayoutItem *item )
{
//	cout << "add item called" <<endl;
    list.append(item);
}

void MyLayout::addDetails(float pa,float pd,float ph,float pw)
{
//	cout << "details added " <<endl;

	d.push_back(details(pa,pd,ph,pw));
}

MyLayout::~MyLayout()
{
//	delete memoryleak;
	d.clear();
	//cout << "calling deleteAllItems() " <<endl;
    deleteAllItems();
}
void MyLayout::removeAndDelete()
{
	d.clear();
	//cout << " calling deleteAllItems() " <<endl;
	deleteAllItems();

}
void MyLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
	
    QListIterator<QLayoutItem> it(list);
    if (it.count() == 0)
        return;

    QLayoutItem* but;

	int startx;
	int starty;
	int newwidth;
	int newheight;

	int border = 2;

	int kheight = rect.height();

	if(ratio > (float)(rect.width()-2*border)/(float)(kheight-2*border))
	{
		startx = rect.x()+border;
		newwidth = rect.width() - 2*border;
		newheight = (int)((float)(rect.width()- 2*border)/ratio) ;
		starty = rect.y() + 2*border + kheight/2 - newheight/2;
			//starty = rect.y() + 2*border;
	}
	else
	{
		starty = rect.y() + 2*border;
		newheight = kheight - 2*border;
		newwidth = (int)((float)(kheight-2*border)*ratio);
		startx = rect.width()/2 - newwidth/2;
	}

	int i=0;
    while ((but=it.current()) != 0)
        {
		//	cout << "setting button geom "<<endl;
            ++it;
			QRect geom(startx+(int)(d[i].pacross*newwidth),starty+(int)(d[i].pdown*newheight),(int)(d[i].pwidth*newwidth),(int)(d[i].pheight*newheight));			
            but->setGeometry(geom);
			i++;
        }       
} 

QSize MyLayout::sizeHint() const //this method is compulsory
{
   /* QSize s(0,0);
    int n = list.count();
    if ( n > 0 )
        s = QSize(100,70); //start with a nice default size
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        s = s.expandedTo( o->minimumSize() );
    }
    return s + n*QSize(spacing(),spacing());*/
	//return QSize(316,100);
	return QSize(QApplication::desktop()->width(),QApplication::desktop()->height());
}
/*
QSize MyLayout::minimumSize() const
{
 /*   QSize s(0,0);
    int n = list.count();
    QListIterator<QLayoutItem> it(list);
    QLayoutItem *o;
    while ( (o=it.current()) != 0 ) {
        ++it;
        s = s.expandedTo( o->minimumSize() );
    }
    return s + n*QSize(spacing(),spacing()); star/
	return QSize(320,240);
} */
