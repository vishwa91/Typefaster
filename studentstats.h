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
#ifndef STUDENTSTATS_H
#define STUDENTSTATS_H

#include <qstring.h>
#include <qpushbutton.h>
#include <qwidget.h>

class studentstats : public QObject
	{
	Q_OBJECT

public:
		QString studentname;

		bool nodata;

		float tried;
		int tred,tg,tb;

		float accuracy;
		int ar,ag,ab;

		float speed;
		int sr,sg,sb;

	
		QPushButton* removebutton;

		studentstats(QString);
		
		studentstats(QString,float t,float a,float s);

		~studentstats();

		void calccolours(float,int&,int&,int&);

		void createbutton(QWidget*);

public slots:
	void removepressed();

signals:
		void removepressed(QString);
	};

#endif
