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
#ifndef MYDIALOG_H
#define MYDIALOG_H
#include <qdialog.h>
#include <qgrid.h>
#include <qlabel.h>


class MyDialog : public QDialog
{
	Q_OBJECT
public:
	MyDialog();
	~MyDialog();
	void closeEvent(QCloseEvent* );
	void keyPressEvent(QKeyEvent*);
//	void focusInEvent(QFocusEvent*);
//	void focusOutEvent(QFocusEvent*);
		/*~MyDialog():
		{
			if(grid!=0)
				delete grid;
			if(text1!=0)
				delete text1;
			if(text2!=0)
				delete text2;
		}*/
	QLabel* text1;
	QLabel* text2;
	QGrid* grid;
public slots:

signals:
	void mydialogclosed();

};
#endif
