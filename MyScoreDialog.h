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
#ifndef MYSCOREDIALOG_H
#define MYSCOREDIALOG_H
#include <qdialog.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qscrollview.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include "ChartWidget.h"

class MyScoreDialog : public QDialog
{
	Q_OBJECT
public:
	MyScoreDialog(QWidget*);
	~MyScoreDialog();
	

	ChartWidget* drawon;

	QPushButton* slowest5;
	QPushButton* accurate5;
	QPushButton* custom;
	QPushButton* history;

	void dodone(int);
//	bool lettersonly;

public slots:
	void okpressed();
	void slowestpressed();
	void accuratepressed();
	void custompressed();

private:
	QVBoxLayout* vboxl;
	QHBoxLayout* hboxl;
	QScrollView* scrollview;
	QPushButton* ok;
	QCheckBox* letters;
	
};

#endif
