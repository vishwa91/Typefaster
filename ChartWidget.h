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
#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H
#include "numandtime.h"
#include <qwidget.h>
#include <qcheckbox.h>
#include <vector>
using namespace std;

class ChartWidget: public QWidget
{
	Q_OBJECT
public:
	ChartWidget(QWidget* parent,QCheckBox*);
	void paintEvent(QPaintEvent*);
	void doCalc(vector<numandtime*>);

	QString heading;
	QString worst;
	QString slowest;
	QCheckBox* letters;
	bool lettersonlyt;

	void setletters(bool);
public slots:
	void letterstoggled(bool);
private:
	vector<numandtime*> useful;
	vector<numandtime*> nuseful;
	vector<numandtime*> all;
};

#endif
