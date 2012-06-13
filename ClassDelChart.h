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
#ifndef CLASSDELCHART_H
#define CLASSDELCHART_H
#include <qwidget.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qstring.h>
#include <vector>
using namespace std;

class ClassDelChart: public QWidget
{
	Q_OBJECT
public:
	ClassDelChart(QWidget* parent);
	void paintEvent(QPaintEvent*);
	void hideEvent(QHideEvent* e);

	void setup();
	void delstudent(QString);

	struct nameandbox
	{
		QString name;
		QCheckBox* box;
		nameandbox(QString s,QWidget* w)
		{
			name = s;
			box = new QCheckBox(w);
		}
		~nameandbox()
		{
			delete box;
		}
	};
	vector<nameandbox*> names;

	QPushButton* del;
	QPushButton* check;
	bool checkb;

public slots:
	void delpressed();
	void checkpressed();
};

#endif
