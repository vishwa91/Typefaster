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
/*
#ifdef FEST
#ifdef WIN
//__declspec(dllimport) void initialize_festival(void);
//__declspec(dllimport) void speak(const char* );
#endif
#endif
*/
#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H
#include "MyButton.h"
#include "MyDialog.h"
#include "MyScoreDialog.h"
#include "numandtime.h"
#include <qdict.h>
#include <qstring.h>
#include <qwidget.h>
#include <qtoolbutton.h>
#include <qdialog.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qsound.h>
#include <qdatetime.h>
#include <qprogressbar.h>
#include <qthread.h>
#include <qtimer.h>
#include <vector>
using namespace std;

class MyMainWindow;

#include <vector>
//using namespace std;

typedef unsigned long ulong;

class TextWidget : public QWidget
{
	 Q_OBJECT
public:
	bool allowbackspace;
	TextWidget(int fs,bool psound,QWidget* parent,QToolButton* i,QToolButton* d,QToolButton*,QProgressBar*,MyMainWindow*,bool shb,bool alb,const char* name=0);
	~TextWidget();
	void initialize(std::vector<MyButton*> ve,QString);
	void displayLesson(std::vector<QString>*,bool ltr,bool num); //left to right

	void resizeEvent(QResizeEvent*);
	void paintEvent(QPaintEvent*);
//	int heightHint();
	
	void fromKeyboard(QString,int);
	void backspace();

	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);


//	void increaseFontWidget(QToolButton*);
//	void decreaseFontWidget(QToolButton*);

	bool playsound;
	int fontsize;

	QPushButton* redo;
	QPushButton* next;

	MyDialog* mydialog;
//	QSize SizeHint();
#ifdef FEST

	void customEvent(QCustomEvent*);
	void sayhelpfile();
	void stopsayhelpfile();

	void focusOutEvent(QFocusEvent*);
	void focusInEvent(QFocusEvent*);
	
#endif

	int studentatindex;

	

	class layoutScore
	{
		public:
			QString layoutname;
			QDict<numandtime>* letters;
			layoutScore(QString l)
			{
				layoutname = l;
				letters = new QDict<numandtime>(109);
			}
			~layoutScore()
			{
				letters->setAutoDelete(true);
				letters->clear();
				delete letters;
			}
	};

	std::vector<layoutScore*> layoutScores;
	int layoutScoreIndex;
	vector<numandtime*> passon;

	void doScoreCalc();
	MyScoreDialog* scoredialog;

public slots:
	void increaseFontSize();
	void decreaseFontSize();
	void timeEvent();
	void showScoreDialog();
	void clearHistory();
private:

#ifdef FEST
	QTimer* delay;
	enum sayhowmany
	{
		position=0,
		letter=1,
		oneword=2,
		threewords=3,
		sentence=4
	};
	sayhowmany level;
	QString getnextwords();
	int saiduntil;	
	bool showhelpb;
#endif


	bool enterexpected;
	bool numericfinished;

	QTime* keypresstimer;
	QTime* timer;
	bool newlesson;
	bool withinlesson;

	int totalerrors; //every error that is made
	int errinstud; //only errors that have not been backspaced
	int wholelessonlength;
	std::vector<int> switchwords;
	bool wordssection;

	void calcSubstrings();
	void fontsizeChanged(int);

	std::vector<MyButton*> v;
	
	std::vector<QString>* lessonvec;
	int lessonveci;
	bool numeric;
	//bool poem;

	void displayLessonLine(QString);
#ifdef FEST
	void saysomething(QString *);
#endif

	QToolButton* increasefontwidget;
	QToolButton* decreasefontwidget;
	QProgressBar* progress;
	void calcWidthAndHeight(int fs,int& w,int& h);
//	QString widestchar;

	QSound* good;
	QSound* bad;

	struct HighlightDetails
	{
		int index;
		BrushStyle style;
		HighlightDetails(int i,BrushStyle s)
		{
			index = i;
			style = s;
		}
	};
	typedef std::vector<HighlightDetails> highlightvector;
	//QDict<Highlights>* khighlights;
	QDict<highlightvector>* khighlights;
#ifdef FEST
	QDict<QString>* valuetosay;
#endif
	std::vector<HighlightDetails>* lasthighlighted;
	std::vector<HighlightDetails>* enterhighlighted;

	int altgri;
	int leftshifti;
	int rightshifti;
	int tabi;
	int capslocki;
	int enteri;
	int backspacei;
	int numlocki;
	int forwardaccenti;
	int doubledoti;
	int hati;
	int backwardaccenti;
	int squigglei;
	int cedillai;
	int caroni;
	int brevei;
	int degreesigni;
	int ogoneki;
	int dotabovei;
	int doubleacuteaccenti;

	std::vector<int> controli;
	std::vector<int> alti;
	

	
 	int charwidth;
	int charheight;
	int widgetheight;
	int widgetwidth;
	bool rtl;
	QString lesson;
//	int startblack;
//	int endblack;
//	std::vector<QString> blacks;
	QString blacksubstring;
	QString greysubstring;
	int blackendindex;
	int blackstartindex;
	int blacklength;
	int greys;

	

	int studentatline;
//	int studentkeysindex;
	
	struct keypressed
	{
		QString key;
		bool error;
		bool line;
		keypressed(QString k,bool e,bool l)
		{
			key = k;
			error = e;
			line = l;
		}
	};
	std::vector<keypressed> studentkeys;

	
	

};

#endif
