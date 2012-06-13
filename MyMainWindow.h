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
#ifndef MYMAINWINDOW_H
#define MYMAINWINDOW_H
#include <qmainwindow.h>
#include "TextWidget.h"
#include "KeyBoardWidget.h"
#include "BackgroundWidget.h"
#include "SettingsDialog.h"
#include "ClassDialog.h"
#include "ClassChart.h"
#include "ClassDelChart.h"
//#include "StudentThread.h"

#ifdef GAME
#include "MyGLWidget.h"
#endif
#include <qlabel.h>
#include <qtextview.h>
#include <qtoolbar.h>
//#include <qmime.h>
//#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qcombobox.h>
#include <qprogressbar.h>
#include <qcheckbox.h>
#include <qdialog.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qdict.h>
#include "MyHelpDialog.h"
#include <qthread.h>

#include <vector>
using namespace std;

class MyMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MyMainWindow( QWidget* parent = 0, const char* name = 0, WFlags f = WType_TopLevel );
	~MyMainWindow();
	void closeEvent(QCloseEvent*);
	MyHelpDialog* helpbrowser;
	MyHelpDialog* helpbrowser2;
	KeyBoardWidget* keyboardwidget;

public slots:
	void about();
	void playsound(bool);
	void help();
	void help2();
    void game();
	void edit();
	void classprog();
	void classprogview();
	void stoppressed();
	void classbpressed();
	void classdelbpressed();
	void delbpressed();
//	QSize sizeHint();
private:

	//QMutex showmutex;

	void customEvent(QCustomEvent * a);
	void restoreclassprogd();

		TextWidget* text;
		
		BackgroundWidget* backgroundwidget;
		QLabel* author;
		QBoxLayout* vertlayout;
	//	QBoxLayout* hlayout;
		
		QTextView* browser;
		QPushButton* ok;
		QToolBar* toolbar;

		QTextView* browser2;
		QPushButton* ok2;
		QBoxLayout* vertlayout2;
	

		QLabel* lessonl;
		QLabel* layoutl;

		QComboBox* lessoncombo;
		QPixmap* incr;
		QToolButton* increasefont;
		QPixmap* decr;
		QToolButton* decreasefont;
		QComboBox* layoutcombo;
		QCheckBox* playsoundbox;
		QProgressBar* progress;
		QPixmap* helpp;
		QToolButton* helpb;
		QToolButton* helpb2;
		QPixmap* aboutp;
		QToolButton* aboutb;

		QPixmap* classprogp;
		QToolButton* classprogb;
		ClassDialog* classprogd;

		QPixmap* delp;
		QToolButton* delb;
		QDialog* classdeld;
		QPushButton* classdelb;
		QVBoxLayout* classdelvbox;
		QScrollView* classdelscroll;
		ClassDelChart* classdelc;

		QDialog* classd;
		QPushButton* classb;
		ClassChart* classc;
		QVBoxLayout* classvbox;
		QScrollView* classscroll;

		QToolBar* toolbar2;
		QToolButton* progresschartb;
		QPixmap* progchart;

		QToolButton* editb;
		QPixmap* editp;
		bool deleteedit;

		SettingsDialog* editd;

	//	QDialog* stopdialog;
		QLabel* stoplabel;
		QPushButton* stopbutton;

		bool waitingforstudentthreads;
		//vector<StudentThread*> studentthreads;
		
#ifdef GAME
		QPixmap* gamep;
		QToolButton* gameb;
		MyGLWidget* myglwidget;
		QToolBar* mygltoolbar;
		QToolButton* playb;
		QToolButton* glaboutb;
		QMainWindow* myglwindow;
#endif
};

#endif
