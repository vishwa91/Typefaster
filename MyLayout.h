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
#ifndef MYLAYOUT_H
#define MYLAYOUT_H
#include "TextWidget.h"
#include <qlayout.h>
#include <vector>
//using namespace std;
class MyLayout : public QLayout
{
public:
	
	MyLayout(QWidget *parent, float r,int dist=0);
	MyLayout(QLayout* parent, float r, int dist=0);
//
    ~MyLayout();

    void addItem(QLayoutItem *item);
	void addDetails(float,float,float,float);

    QSize sizeHint() const;
    QLayoutIterator iterator();
    void setGeometry(const QRect &rect);
	void removeAndDelete();

	float ratio;

private:
    QList<QLayoutItem> list;

	
struct details
{
	float pacross;
	float pdown;
	float pheight;
	float pwidth;
	details(float pa,float pd,float ph,float pw)
	{
		pacross = pa;
		pdown = pd;
		pheight = ph;
		pwidth = pw;
	}
};
	std::vector<details> d;

	class MyLayoutIterator :public QGLayoutIterator
	{
	public:
	  MyLayoutIterator(QList<QLayoutItem> *l): idx(0), list(l)  
		{}

	 QLayoutItem *current()
	  { return idx < int(list->count()) ? list->at(idx) : 0;  }

	  QLayoutItem *next()
	  { idx++; return current(); }

	  QLayoutItem *takeCurrent()
	  { return list->take( idx ); }

	private:
	  int idx;
		QList<QLayoutItem> *list;
	};

//	MyLayoutIterator* memoryleak;

};

#endif
