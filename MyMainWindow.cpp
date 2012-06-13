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

#ifndef WIN
#define QT_THREAD_SUPPORT
#endif

// Added by vishwanath on 6/5/12
#ifndef ETIMEDOUT
#define ETIMEDOUT 145
#endif

#include "HelpFile.h"
#include "MyMainWindow.h"
#include "UserDetails.h"
//#include "MyThread.h"
#include "studentstats.h"

#ifdef WIN
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#endif

#include <qapplication.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qpainter.h>
#include <qdir.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qimage.h>
#include <qthread.h>

#ifndef WIN
#include <time.h>
#include <sys/time.h>
#endif

#include <vector>

#include <iostream>
using namespace std;

extern QString USERNAME;
extern bool ISTEACHER;
extern QString TEACHERSNAME;

extern bool TEACHERSUPPORT;
extern bool MULTI;
extern QDict<userDetails>* DICT;
//extern std::vector<MyThread>* THREADS;

extern bool changingteacher;
#ifdef WIN
extern HANDLE changingteacherhandle;
#else
extern pthread_t changingteacherhandle;
extern pthread_mutex_t changingteachermut;
extern pthread_cond_t changingteachercond;
#endif
extern QFile* changingteacherfile;

extern bool addingteacher;
#ifdef WIN
extern HANDLE addingteacherhandle;
#else
extern pthread_t addingteacherhandle;
extern pthread_mutex_t addingteachermut;
extern pthread_cond_t addingteachercond;
#endif
extern QFile* addingteacherfile;

QDict<bool>* lettersonlydict;

//for some reason, (Linux g++ Qt3) doesn't like this declared globally
#ifdef WIN 
QMutex mutex;
QMutex mutex2;
#else
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
#endif

int threadscomplete;
int totalthreads;
vector<studentstats*> studentstatsv;
bool mustwrite = false;
//ClassChart* classc;

struct passtothreadstruct
{
	QString studentname;
	QWidget* mainwindow;
	QFile* file;

	passtothreadstruct(QString s,QWidget* w,QFile* f)
	{
		studentname = s;
		mainwindow = w;
		file = f;
	}
};

struct threaddetailsstruct
{
#ifdef WIN
	HANDLE handle;
	passtothreadstruct* info;

	threaddetailsstruct(HANDLE h,passtothreadstruct* i)
	{
		handle = h;
		info = i;
	}
#else
    pthread_t handle;
	passtothreadstruct* info;

	threaddetailsstruct(pthread_t h,passtothreadstruct* i)
	{
		handle = h;
		info = i;
	}
#endif
};

vector<threaddetailsstruct> studentthreads;

//this function is started as a new thread, when a teacher wants to see all the data
//about each of her students, one thread for each of her students goes and finds the 
//data
#ifdef WIN
DWORD WINAPI studentthread(LPVOID f)
{
#else
void* studentthread(void* f)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
#endif
	passtothreadstruct* info = (passtothreadstruct*)f;

	QDir d("users/"+info->studentname);
	
	QFile* m = info->file; 

	if(d.exists())
	{
		if(m->exists())
		{		

			if(m->open(IO_ReadOnly))
			{

				int tried = 0;
				int total = 0;

				float accuracyp=0;
				float speedp=0;


				QTextStream mstream(m);
					mstream.setEncoding(QTextStream::Unicode);
					while(!mstream.atEnd())
					{
						QString l = mstream.readLine();
						if(l==0)
							break;
						QString num = mstream.readLine();
						if(num==0)
							break;
						QString totaltime = mstream.readLine();
						if(totaltime==0)
							break;
						QString missed = mstream.readLine();
						if(missed==0)
							break;

						total++;

						float acc = 0.0;

						float speed = 0.0;
						float time = 0.0;
						if(num.toULong()>0)
						{
							tried++;

							acc = num.toULong()/((float)num.toULong()+(float)missed.toULong());

							accuracyp = acc/(float)tried + ((tried-1)/(float)tried)*accuracyp;

							time = totaltime.toULong()/((float)num.toULong());
							if(time<10)
								time = 10;
							if(time>3000)
								time = 3000;
							speed = (2990 - (time - 10))/(float)2990;
						

							speedp = speed/(float)tried + ((tried-1)/(float)tried)*speedp;
						}
						
					}

				if(total==0)
				{
					tried = 0;
					total = 1;
				}

				studentstats* tmp = new studentstats(info->studentname,tried/(float)total,accuracyp,speedp);
				
				#ifdef WIN
				mutex2.lock();
				#else
				pthread_mutex_lock(&mutex2);
				#endif
					studentstatsv.push_back(tmp);
				#ifdef WIN
				mutex2.unlock();
				#else
				pthread_mutex_unlock(&mutex2);
				#endif				

				m->close();
			}
			else
			{
				studentstats* tmp = new studentstats(info->studentname);
				
				#ifdef WIN
				mutex2.lock();
				#else
				pthread_mutex_lock(&mutex2);
				#endif
					studentstatsv.push_back(tmp);
				#ifdef WIN
				mutex2.unlock();
				#else
				pthread_mutex_unlock(&mutex2);
				#endif	
			}
		}
		else
		{
		//	while(true);
			studentstats* tmp = new studentstats(info->studentname);

			#ifdef WIN
			mutex2.lock();
			#else
			pthread_mutex_lock(&mutex2);
			#endif	
				studentstatsv.push_back(tmp);
			#ifdef WIN
			mutex2.unlock();
			#else
			pthread_mutex_unlock(&mutex2);
			#endif	
		}
	}
	else
		mustwrite = true;

	#ifdef WIN
	mutex.lock();
	#else
	pthread_mutex_lock(&mutex);
	#endif	
	threadscomplete++;
	if(threadscomplete == totalthreads)
	{
		//post an event
		QCustomEvent* evstop = new QCustomEvent(346797); //arb big number
	
		QThread::postEvent(info->mainwindow,evstop);
		//QApplication::postEvent(info->mainwindow,evstop);
	}
	#ifdef WIN
	mutex.unlock();
	#else
	pthread_mutex_unlock(&mutex);
	#endif	

	return 0;
}

MyMainWindow::MyMainWindow( QWidget* parent, const char* name, WFlags f )
	: QMainWindow( parent, name, f )
{

	waitingforstudentthreads = false;

	int MINWIDTH = 316;
	int MINHEIGHT = 237;
	//int MINHEIGHT = 260;

	setGeometry(4,22,QApplication::desktop()->width()-8,QApplication::desktop()->height()-60);
	//this is so the first resize events don't force the font size small, when it
	//was wanted really big

	lettersonlydict = new QDict<bool>(17);

	//Be careful what you translate
	QString windowlayout="showMaximized"; //just some default values, they will be overwritten
	QString layoutwanted=
		//qApp->translate("QString",
		"US-English.xml";
		//,"default layout wanted, must be same as one in list");
		//BE CAREFUL WITH TRANSLATION HERE
	//CHANGE THIS TO SPANISH
	QString fontsize="15";
	QString sound="true";
	QString lessonwanted=//qApp->translate("QString",
		"Lesson 1";
		//,"default lesson");
		 //AGAIN BE CAREFUL WITH TRANSLATION
	//CHANGE THIS TO WHATEVER LESSON IN SPANISH IS
	QString showhelp="showhelp=false";
	bool showhelpb = false;
	QString toolbardock = "toolbardock=top";

	bool hardcodedlayout=false;

	bool allowbackspace = true; //hidden setting

	QString settingslocation = "defaultuser/settings.txt";
	if(MULTI)
		settingslocation = "users/"+USERNAME+"/settings.txt";

	QFile settings(settingslocation.latin1()); 
	if(settings.exists())
	{
		settings.open(IO_ReadOnly);	
		QTextStream settingsstream(&settings);
		settingsstream.setEncoding(QTextStream::Unicode);
		QString tmp;
		//so future versions can still work with version 1 files
		if((tmp = settingsstream.readLine()) != 0 && tmp=="version1")
		{
		if((tmp = settingsstream.readLine())!=0)
		{
			windowlayout = tmp;
			if((tmp = settingsstream.readLine())!=0)
			{
				layoutwanted = tmp.mid(7);
				if((tmp = settingsstream.readLine())!=0)
				{
					fontsize = tmp.mid(9);
					if((tmp = settingsstream.readLine())!=0)
					{
						sound = tmp.mid(6);
						if((tmp = settingsstream.readLine())!=0)
						{
							lessonwanted = tmp.mid(7);
							if((tmp = settingsstream.readLine())!=0)
							{
								showhelp = tmp;
								if(showhelp=="showhelp=true")
									showhelpb = true;
								else
									showhelpb = false;
								if((tmp=settingsstream.readLine())!=0)
								{
									toolbardock=tmp;
									//allowbackspace = true;
									if((tmp=settingsstream.readLine())!=0) //hidden setting
									{
										if(tmp=="allowbackspace=false")
											allowbackspace = false;
										else //read in all the names of the layouts that want lettersonly
										{
											bool* t = new bool;
											*t = true;
											lettersonlydict->insert(tmp,t);
											while((tmp=settingsstream.readLine())!=0)
											{
												bool* t = new bool;
												*t = true;
												lettersonlydict->insert(tmp,t);
											}
										}
									}
								}
								else
									QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
							}
							else
								QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
						}
						else
							QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
					}
					else
						QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
				}
				else
					QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
			}
			else
				QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
		}
		 else
			QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
		}
		else
			QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt or new version, try upgrading the software"));
		settings.close();
	}
	else
		QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file does not exist"));

	setCaption(tr("TypeFaster Typing Tutor")); //seems to do nothing
	
	qApp->setMainWidget(this);

	backgroundwidget = new BackgroundWidget(this); //text and keyboard are added to background
	setCentralWidget(backgroundwidget);
	
	
	author = new QLabel(backgroundwidget);

	bool isok = false;
	int fontsizeint= fontsize.toInt(&isok);
	if(!isok || fontsizeint<2 || fontsizeint>1000)//pretty arb numbers
		fontsizeint = 15;

	bool soundwanted = true;
	if(sound=="true" || sound=="false")
	{
		if(sound=="false")
			soundwanted=false;
	}
	else
		QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));

	//text = new TextWidget(fontsizeint,soundwanted,backgroundwidget);
	if(MULTI)
	{
		QMenuBar* menubar = menuBar();
		QLabel* loggedin = new QLabel(tr("Logged in as: ")+USERNAME,menubar);
		menubar->insertItem(loggedin);
	}
	//QLabel* loggedin = new QLabel(tr("Logged in as: ")+USERNAME,this);

	toolbar = new QToolBar(this);
	
	//toolbar->setHorizontalStretchable(true);

	lessonl = new QLabel(tr("Lesson: "),toolbar);
	lessoncombo = new QComboBox(toolbar);
	lessoncombo->setFocusPolicy(QWidget::NoFocus);
	lessoncombo->setMinimumWidth(100);
	


	QString layoutslocation = "defaultuser/layouts";
	if(MULTI)
	{
		if(TEACHERSUPPORT)
		{	
			if(ISTEACHER)
				layoutslocation = "users/" + USERNAME + "/layouts";//teacher gets her own medicine
			else
				layoutslocation = "users/" + TEACHERSNAME + "/layouts";
		}
		else
			layoutslocation = "users/" + USERNAME + "/layouts";

		QDir te(layoutslocation.latin1());
		if(!te.exists())
			layoutslocation = "layouts";
	}

	QDir d(layoutslocation.latin1()); 
	d.setFilter(QDir::Files);
    if ( !d.exists() )
	{
		//std::cout << "Error, layouts directory not found" << std::endl;
        QMessageBox::critical(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, layouts directory not found"));
	//	exit(1);
		hardcodedlayout = true;
	}

	std::vector<QString> layouts;
	int index=0;
	if(!hardcodedlayout)
	{
		int i;
		for (i=0; i<d.count(); i++ )
			if(d[i]!="."&&d[i]!="..")
				layouts.push_back(d[i]);

	 
	 
		bool layoutfound = false;
		
		 for(i=0;i<layouts.size();i++)
			 if(layoutwanted == layouts[i])
			 {
				 index=i;
				 layoutfound=true;
				 break;
			 }
		 
		if(!layoutfound)
		{
			//std::cout << "Error "<<layoutwanted<<" not found"<<std::endl;
			QMessageBox::critical(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, layout specified in settings file was not found"));
		//	exit(1);
			if(layouts.size()>0)
			{
				layoutwanted = layouts[0];
				index = 0;
			}
			else
				hardcodedlayout = true;
		}
	}

	toolbar->addSeparator();
	layoutl = new QLabel(tr("Layout: "),toolbar);
	layoutcombo = new QComboBox(toolbar);
	layoutcombo->setFocusPolicy(QWidget::NoFocus);
	int i;
	for(i=0;i<layouts.size();i++)
		layoutcombo->insertItem(layouts[i].mid(0,layouts[i].length()-4));
	if(!hardcodedlayout)
		layoutcombo->setCurrentItem(index);


	toolbar->addSeparator();

	incr = new QPixmap("pictures/increase_font_size.png","png"); //don't translate
	increasefont = new QToolButton(toolbar);
	increasefont->setUsesTextLabel(true);
	increasefont->setTextLabel(QToolButton::tr("Increase font size"));
	if(incr!=0)
		increasefont->setPixmap(*incr);

	decr = new QPixmap("pictures/decrease_font_size.png","png");
	decreasefont = new QToolButton(toolbar);
	decreasefont->setUsesTextLabel(true);
	decreasefont->setTextLabel(QToolButton::tr("Decrease font size"));
	if(decr != 0)
		decreasefont->setPixmap(*decr);
//	text->decreaseFontWidget(decreasefont);

	toolbar->addSeparator();
	playsoundbox = new QCheckBox(QCheckBox::tr("Play sounds"),toolbar);
	playsoundbox->setFocusPolicy(QWidget::NoFocus);
	connect(playsoundbox,SIGNAL(toggled(bool)),this,SLOT(playsound(bool)));
	
	toolbar->addSeparator();
	helpp = new QPixmap("pictures/help.png","png"); //do not translate
	helpb = new QToolButton(toolbar);
	helpb->setUsesTextLabel(true);
	helpb->setTextLabel(QToolButton::tr("Help"));
	if(helpp != 0)
		helpb -> setPixmap(*helpp);
	helpb->setAccel(Key_Escape);
	connect(helpb, SIGNAL(clicked()), this, SLOT(help()));


	if(MULTI && TEACHERSUPPORT && ISTEACHER)
	{
		toolbar->addSeparator();
		helpb2 = new QToolButton(toolbar);
		helpb2->setUsesTextLabel(true);
		helpb2->setTextLabel(QToolButton::tr("Teacher's help"));
		if(helpp != 0)
			helpb2 -> setPixmap(*helpp);
		connect(helpb2, SIGNAL(clicked()), this, SLOT(help2()));
	}

	

	toolbar->addSeparator();
	aboutp = new QPixmap("./pictures/typefaster_icon.png","png");//do not translate
	aboutb = new QToolButton(toolbar);
	aboutb->setUsesTextLabel(true);
	aboutb->setTextLabel(QToolButton::tr("About"));
	if(aboutp != 0)
		aboutb->setPixmap(*aboutp);
	connect(aboutb, SIGNAL(clicked()), this, SLOT(about()));


		toolbar2 = new QToolBar(this);
	progchart = new QPixmap("pictures/progress_chart.png","png"); //don't translate
	progresschartb = new QToolButton(toolbar2);
	progresschartb->setUsesTextLabel(true);
	progresschartb->setTextLabel(QToolButton::tr("View your progress"));
	if(progchart!=0)
		progresschartb->setPixmap(*progchart);

#ifdef GAME
	toolbar2->addSeparator();
	myglwidget = 0;
	gamep = new QPixmap("./pictures/game.png","png");
	gameb = new QToolButton(toolbar2);
	gameb->setUsesTextLabel(true);
	gameb->setTextLabel(QToolButton::tr("Play game"));
	if(gamep!=0)
		gameb->setPixmap(*gamep);
	connect(gameb,SIGNAL(clicked()),this,SLOT(game()));
#endif

	toolbar2->addSeparator();
	progress = new QProgressBar(toolbar2);
	progress->setMinimumWidth(180);

	QString edittxt = "";
	deleteedit = false;

	if(MULTI)
	{
		if(TEACHERSUPPORT)
		{
			if(ISTEACHER)
				edittxt = "Edit class settings"; //only teacher gets if teachersupport
		}
		else
			edittxt = "Edit settings"; //multi but no teacher support, each has their own
	}
	else
		edittxt = "Edit settings"; //single user

	if(edittxt!="")
	{
		deleteedit = true;
		toolbar2->addSeparator();
		editp = new QPixmap("pictures/edit.png","png"); //don't translate
		editb = new QToolButton(toolbar2);
		editb->setUsesTextLabel(true);
		editb->setTextLabel(QToolButton::tr(edittxt));
		if(editp!=0)
			editb->setPixmap(*editp);

		editd = new SettingsDialog(layoutcombo,lessoncombo,this);
		connect(editb,SIGNAL(clicked()),this,SLOT(edit()));
	}

	if(MULTI && TEACHERSUPPORT && ISTEACHER)
	{
		toolbar2->addSeparator();
		classprogp = new QPixmap("pictures/class_progress_chart.png","png");
		classprogb = new QToolButton(toolbar2);
		classprogb->setUsesTextLabel(true);
		classprogb->setTextLabel(tr("View class progress"));
		if(classprogp!=0)
			classprogb->setPixmap(*classprogp);

		classprogd = new ClassDialog(layoutcombo,this);
		connect(classprogb,SIGNAL(clicked()),this,SLOT(classprog()));
		connect(classprogd->view,SIGNAL(clicked()),this,SLOT(classprogview()));

		//stopdialog = new QDialog(classprogd,"stop",true,WStyle_Customize|WStyle_DialogBorder|WStyle_Title);//  | WStyle_Tool) //| WStyle_NoBorder
		//stopdialog->setFixedSize(200,50);
		//stopdialog->setCaption(tr("TypeFaster"));

	//	stoplabel = new QLabel(tr("Collecting data ..."),stopdialog);
	//	stoplabel->setGeometry(20,10,90,30);

		stoplabel = new QLabel(tr("Collecting data ..."),classprogd->group);
		stoplabel->setGeometry(20,20,90,30);
		stoplabel->hide();

		stopbutton = new QPushButton(tr("Stop"),classprogd->group);
		//stopbutton->setGeometry(130,10,50,30);
		stopbutton->setGeometry(200,20,110,30);
		connect(stopbutton,SIGNAL(clicked()),this,SLOT(stoppressed()));
		stopbutton->hide();



		classd = new QDialog(classprogd,"class chart",true,WStyle_Customize|WStyle_NormalBorder|WStyle_Title|WStyle_SysMenu|WStyle_Maximize);
		classd->setCaption(tr("TypeFaster - Class Progess Charts"));
		classd->setSizeGripEnabled(true);
		int w = QApplication::desktop()->width();
		int h = QApplication::desktop()->height();
		classd->setGeometry(w/8,25,w-w/4,h-h/8);

		classvbox = new QVBoxLayout(classd,10,10);

		classscroll = new QScrollView(classd,"class scrollview",WResizeNoErase|WNorthWestGravity);
		classscroll->viewport()->setBackgroundMode(QWidget::PaletteBackground);
			
		classc = new ClassChart(classscroll->viewport());
		

		classscroll->addChild(classc);
		classvbox->addWidget(classscroll);

		classb = new QPushButton(tr("Ok"),classd);
		classb->setDefault(true);
		classb->setMaximumWidth(80);
		classb->setMaximumHeight(25);
		connect(classb,SIGNAL(clicked()),this,SLOT(classbpressed()));
		classvbox->addWidget(classb);

		//QPushButton* arb = new QPushButton(tr("hi"),(QWidget*)classc);
		//arb->setGeometry(600,20,90,30);

		//now the delete students button
		toolbar2->addSeparator();
		delp = new QPixmap("pictures/delete.png","png");
		delb = new QToolButton(toolbar2);
		delb->setUsesTextLabel(true);
		delb->setTextLabel(tr("Delete students"));
		if(delp!=0)
			delb->setPixmap(*delp);

		connect(delb,SIGNAL(clicked()),this,SLOT(delbpressed()));

		classdeld = new QDialog(this,"class del chart",true,WStyle_Customize|WStyle_NormalBorder|WStyle_Title|WStyle_SysMenu|WStyle_Maximize);
		classdeld->setCaption(tr("TypeFaster - Delete Students"));
		classdeld->setSizeGripEnabled(true);
	
		classdeld->setGeometry(w/4,25,w-w/2,h-h/8);

		classdelvbox = new QVBoxLayout(classdeld,10,10);

		classdelscroll = new QScrollView(classdeld,"class del scrollview",WResizeNoErase|WNorthWestGravity);
		classdelscroll->viewport()->setBackgroundMode(QWidget::PaletteBackground);
			
		classdelc = new ClassDelChart(classscroll->viewport());
		

		classdelscroll->addChild(classdelc);
		classdelvbox->addWidget(classdelscroll);

		classdelb = new QPushButton(tr("Close"),classdeld);
		classdelb->setDefault(true);
		classdelb->setMaximumWidth(80);
		classdelb->setMaximumHeight(25);
		connect(classdelb,SIGNAL(clicked()),this,SLOT(classdelbpressed()));
		classdelvbox->addWidget(classdelb);

	}
	
	text = new TextWidget(fontsizeint,soundwanted,backgroundwidget,increasefont,decreasefont,progresschartb,progress,this,showhelpb,allowbackspace);
	//connections are done in textwidget
	playsoundbox->setChecked(soundwanted); //must be done below textwidget

#ifdef SPANISH
	QPixmap dd("./pictures/doubledot-spanish.png","png");
	QPixmap hk("./pictures/homekeys-spanish.png","png");
#else
	QPixmap dd("./pictures/doubledot.png","png"); //do not translate these
	QPixmap hk("./pictures/homekeys.png","png");
#endif
	QPixmap bk("./pictures/bike.png","png");
	

	//do not translate these
	QMimeSourceFactory::defaultFactory()->setImage( "doubledot", dd.convertToImage() );
	QMimeSourceFactory::defaultFactory()->setImage( "bike", bk.convertToImage() );
	QMimeSourceFactory::defaultFactory()->setImage( "homekeys", hk.convertToImage() );

//	helpbrowser = new QDialog(0,0,false,WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WStyle_MinMax);
	helpbrowser = new MyHelpDialog(text,this);
	helpbrowser->setSizeGripEnabled(true);
	helpbrowser->resize(QApplication::desktop()->width()*2/3,QApplication::desktop()->height()/2);
	helpbrowser->move(QApplication::desktop()->width()/6,QApplication::desktop()->height()/4);
	helpbrowser->setCaption(QTextView::tr("TypeFaster Typing Tutor"));

	browser = new QTextView(helpbrowser);
	browser->setTextFormat(RichText);
	browser->setText(helpfile); //gets it from HelpFile.h
	browser->setMinimumSize(MINWIDTH,MINHEIGHT);
	//browser->resize(QApplication::desktop()->width()*2/3,QApplication::desktop()->height()/2);
	//browser->move(QApplication::desktop()->width()/6,QApplication::desktop()->height()/4);

	ok = new QPushButton(QPushButton::tr("OK"),helpbrowser);
	ok->setDefault(true);
	ok->setFocus();
	ok->setMaximumWidth(80);
	ok->setMaximumHeight(25);
    //connect(ok,SIGNAL(clicked()),helpbrowser,SLOT(accept()));
	connect(ok,SIGNAL(clicked()),helpbrowser,SLOT(closed()));

	vertlayout = new QVBoxLayout(helpbrowser,11,11);
    vertlayout->addWidget(browser);
    vertlayout->addWidget(ok);
    //vertlayout->add(helpbrowser);

	if(MULTI && TEACHERSUPPORT && ISTEACHER)
	{
		helpbrowser2 = new MyHelpDialog(text,this);
		helpbrowser2->setSizeGripEnabled(true);
		helpbrowser2->resize(QApplication::desktop()->width()*2/3,QApplication::desktop()->height()/2);
		helpbrowser2->move(QApplication::desktop()->width()/6,QApplication::desktop()->height()/4);
		helpbrowser2->setCaption(QTextView::tr("TypeFaster Typing Tutor"));

		browser2 = new QTextView(helpbrowser2);
		browser2->setTextFormat(RichText);
		browser2->setText(helpfile2); //gets it from HelpFile.h
		browser2->setMinimumSize(MINWIDTH,MINHEIGHT);
		//browser->resize(QApplication::desktop()->width()*2/3,QApplication::desktop()->height()/2);
		//browser->move(QApplication::desktop()->width()/6,QApplication::desktop()->height()/4);

		ok2 = new QPushButton(QPushButton::tr("OK"),helpbrowser2);
		ok2->setDefault(true);
		ok2->setFocus();
		ok2->setMaximumWidth(80);
		ok2->setMaximumHeight(25);
		//connect(ok,SIGNAL(clicked()),helpbrowser,SLOT(accept()));
		connect(ok2,SIGNAL(clicked()),helpbrowser2,SLOT(closed()));

		vertlayout2 = new QVBoxLayout(helpbrowser2,11,11);
		vertlayout2->addWidget(browser2);
		vertlayout2->addWidget(ok2);

	}
	keyboardwidget = new KeyBoardWidget(layoutslocation,backgroundwidget,text,author,lessoncombo,layoutcombo,this);
	connect(text->redo, SIGNAL(clicked()),keyboardwidget, SLOT(redo()));
	connect(text->next, SIGNAL(clicked()),keyboardwidget, SLOT(next()));

	connect(text->mydialog, SIGNAL(mydialogclosed()),keyboardwidget, SLOT(redo()));

	addToolBar(toolbar);
	
	//		}
	if(toolbardock=="toolbardock=top"||toolbardock=="toolbardock=bottom"||toolbardock=="toolbardock=left"||toolbardock=="toolbardock=right")
	{
		if(toolbardock=="toolbardock=top")
			moveToolBar(toolbar,Top);
		else
		if(toolbardock=="toolbardock=bottom")
			moveToolBar(toolbar,Bottom);
		else
		if(toolbardock=="toolbardock=left")
			moveToolBar(toolbar,Left);
		else
			moveToolBar(toolbar,Right);
	}
	else
	{
		QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
		moveToolBar(toolbar,Top);
	}


	

	addToolBar(toolbar2,QMainWindow::Top,true);
	

	keyboardwidget->parseFile(layoutwanted,hardcodedlayout,lessonwanted);
/*
	bool defaultplacement=true;
	if(defaultplacement)
	{
		window.resize(QApplication::desktop()->width()-8,QApplication::desktop()->height()*2/3);
		window.move(0,0);
	}
	
	window.show();*/
	
	backgroundwidget->setMinimumSize(MINWIDTH,MINHEIGHT);
	if(windowlayout=="showMaximized")
		showMaximized();
	else
	{
		QStringList l = QStringList::split(" ",windowlayout);
		if(l.count() == 6)
		{
			if(l[0] == "pos" && l[3] == "size")
			{
				bool ok = false;
				int width = l[4].toInt(&ok);
				bool ok2 = false;
				int height = l[5].toInt(&ok2);
				bool ok3 = false;
				int x = l[1].toInt(&ok3);
				bool ok4 = false;
				int y = l[2].toInt(&ok4);
				if(ok && ok2 && ok3 && ok4)
				{
					resize(width,height);
					move(x,y);
					show();
				}
				else
				{
					QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
					showMaximized();
				}
			}
			else
			{
				QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
				showMaximized();
			}
		}
		else
		{
			QMessageBox::warning(0,QMessageBox::tr("Typefaster Typing Tutor"),QMessageBox::tr("Settings file is corrupt"));
			showMaximized();
		}
	}

	if(showhelpb)
	{
#ifdef FEST
		text->sayhelpfile();
#endif
		if(MULTI && TEACHERSUPPORT && ISTEACHER)
			helpbrowser2->show(); //if a teacher then just show teacher's help
		else
			helpbrowser->show();
	}

}
void MyMainWindow::classprog()
{
	classprogd->exec();
}
void MyMainWindow::classprogview()
{

  if(classprogd->mylayouts->count()>0)
  {
	//create and then start the threads
	QFile f("users/"+USERNAME+"/students.txt");
	if(f.exists())
	{
		if(f.open(IO_ReadOnly))
		{
			QTextStream s(&f);
		//	s.setEncoding(QTextStream::Unicode);
			QString line;
			vector<QString> tmp;
			while((line=s.readLine())!=0)
			{
				tmp.push_back(line);
			}
			if(tmp.size()>0)
			{

				threadscomplete=0;
				totalthreads = tmp.size();

				//studentstatsv.clear();
				mustwrite = false;
				waitingforstudentthreads = true;

				//now start a thread for each student
				int i;				
				for(i=0;i<tmp.size();i++)
				{
					QFile* f = new QFile("users/"+tmp[i]+"/"+classprogd->mylayouts->currentText()+".txt");
					passtothreadstruct* info = new passtothreadstruct(tmp[i],this,f);
					#ifdef WIN
						DWORD targetThreadID;
						HANDLE threadhandle = (HANDLE)_beginthreadex(NULL,0,(unsigned(_stdcall*)(void*))studentthread,(void*)info,0,(unsigned*)&targetThreadID);
					#else
						pthread_t threadhandle;
						pthread_create(&threadhandle,NULL,studentthread,(void*)info);
					#endif
					threaddetailsstruct tmp(threadhandle,info);
					studentthreads.push_back(tmp);
				//	studentthreads.push_back(new StudentThread(tmp[i],classprogd->mylayouts->currentText(),this));
				}

				

				classprogd->layout->hide();
				classprogd->mylayouts->hide();
				classprogd->view->hide();
				classprogd->ok->setDisabled(true);

				stoplabel->show();
				stopbutton->show();

				
			}
			else
				QMessageBox::warning(0,"TypeFaster",tr("No students found"));
		}
		else
			QMessageBox::warning(0,"TypeFaster",tr("No students found"));
	}
	else
		QMessageBox::warning(0,"TypeFaster",tr("No students found"));
  }
}

void MyMainWindow::restoreclassprogd()
{

	stopbutton->hide();
	stopbutton->setDisabled(false);

	stoplabel->hide();

//	classprogd->group->setText(tr("View class scores"));
	classprogd->layout->show();

	classprogd->mylayouts->show();
	classprogd->view->show();
	classprogd->ok->setDisabled(false);

}
void MyMainWindow::customEvent(QCustomEvent * a)
{

	if(waitingforstudentthreads)
	{
		waitingforstudentthreads = false;

			int i;
			for(i=0;i<studentthreads.size();i++)
			{
				//studentthreads[i]->wait();
				//delete studentthreads[i]->m;
				//delete studentthreads[i];
				delete studentthreads[i].info->file;
				delete studentthreads[i].info;

				#ifdef WIN
				CloseHandle(studentthreads[i].handle);
				#endif
			}
	

		studentthreads.clear();
		restoreclassprogd();

		int j;
		for(i=0;i<(int)studentstatsv.size()-1;i++)
			for(j=i+1;j<studentstatsv.size();j++)
			{
				if(studentstatsv[j]->studentname < studentstatsv[i]->studentname)
				{
					studentstats* tmp = studentstatsv[j];
					studentstatsv[j] = studentstatsv[i];
					studentstatsv[i] = tmp;
				}
			}

		for(i=0;i<studentstatsv.size();i++)
			studentstatsv[i]->createbutton(classc);

		if(mustwrite)
		{
			QFile f("users/"+USERNAME+"/students.txt");	
			if(f.open(IO_WriteOnly))
			{
				QTextStream s(&f);
				for(i=0;i<studentstatsv.size();i++)
					s << studentstatsv[i]->studentname << endl;
				f.close();
			}
		}

		classc->setFixedHeight(studentstatsv.size()*64+100);
		classc->heading = classprogd->mylayouts->currentText();
		classd->show();
	}

}

void MyMainWindow::delbpressed()
{
	classdelc->setup();
	classdeld->show();
}
void MyMainWindow::classbpressed()
{
	classd->hide();
}
void MyMainWindow::classdelbpressed()
{
	classdeld->hide();
}
void MyMainWindow::stoppressed()
{

	stopbutton->setDisabled(true);

	waitingforstudentthreads = false;


	//now forcibly stop the threads
	#ifdef WIN
	mutex.lock();
	#else
	pthread_mutex_lock(&mutex);
	#endif	
	
	#ifdef WIN
	mutex2.lock();
	#else
	pthread_mutex_lock(&mutex2);
	#endif	

		int i;
		for(i=0;i<studentthreads.size();i++)
		{
			#ifdef WIN
				DWORD exitcode = 0;
				TerminateThread(studentthreads[i].handle,exitcode);
				CloseHandle(studentthreads[i].handle);
			#else
				pthread_cancel(studentthreads[i].handle);
			#endif

			if(studentthreads[i].info->file->isOpen())
				studentthreads[i].info->file->close();
			delete studentthreads[i].info->file;

			delete studentthreads[i].info;
/*			if(studentthreads[i]->wait(10)==false)//wait 1 millisecond
			{

				if(studentthreads[i]->m!=0)
				{
					if(studentthreads[i]->m->isOpen())
						studentthreads[i]->m->close();
					delete studentthreads[i]->m;
				}
			}
			else
			{
				if(studentthreads[i]->m!=0)
					delete studentthreads[i]->m;
			}
			delete studentthreads[i];*/
		}

		studentthreads.clear();

	#ifdef WIN
	mutex2.unlock();
	#else
	pthread_mutex_unlock(&mutex2);
	#endif	
		
	#ifdef WIN
	mutex.unlock();
	#else
	pthread_mutex_unlock(&mutex);
	#endif	

	restoreclassprogd();

}
void MyMainWindow::edit()
{
	editd->dothere();
	editd->doavailable();
	editd->exec();
}
void MyMainWindow::game()
{
#ifdef GAME
	if(myglwidget == 0)
	{
		myglwindow = new QMainWindow();
		myglwindow->setGeometry(QApplication::desktop()->width()/2-310,QApplication::desktop()->height()/2-232,620,465);
		myglwindow->setCaption("TypeFaster F-16 Game");

		mygltoolbar = new QToolBar(myglwindow);

		playb = new QToolButton(mygltoolbar);
		playb->setUsesTextLabel(true);
		playb->setTextLabel(QToolButton::tr("Play"));
		if(gamep!=0)
			playb->setPixmap(*gamep);

		myglwidget = new MyGLWidget(myglwindow,playb);
		myglwindow->setCentralWidget(myglwidget);
		myglwidget->setMinimumSize(620,465);

		connect(playb,SIGNAL(clicked()),myglwidget,SLOT(play()));

		glaboutb = new QToolButton(mygltoolbar);
		glaboutb->setUsesTextLabel(true);
		glaboutb->setTextLabel(QToolButton::tr("About"));
		if(aboutp!=0)
			glaboutb->setPixmap(*aboutp);
		connect(glaboutb,SIGNAL(clicked()),myglwidget,SLOT(about()));
	
		myglwindow->showMaximized();
	}
	else
		myglwindow->show();
	//myglwidget->play();
#endif
}

void MyMainWindow::about()
{
	QString atext;
#ifdef FEST
	atext = QMessageBox::tr("Version 0.2.1 Accessible\n\n"
                         "Written by David le Roux using QT.\n\n"
						 "Uses the Flite text-to-speech engine\n\n"
                         "Sounds and some lessons from KTouch.\n\n"
                         "Licensed under the GPL Version 2 (see gpl.txt).\n\n"
						 "Mail bug reports/translation offers to: mataav@yahoo.co.uk\n");
#else
	atext = QMessageBox::tr("Version 0.4\n\n"
						"Written by David le Roux using QT.\n\n"
						"Sounds and some lessons from KTouch.\n\n"
						"MD5 code Copyright Aladdin Enterprises,\n"
						"used with permission.\n\n"
						"Licensed under the GPL Version 2 (see gpl.txt).\n\n"
						"Mail bug reports/feature requests to:\n" 
						"mataav@yahoo.co.uk\n\n"
						"If you want to make a donation, go to\n" 
						"http://typefaster.sourceforge.net\n");
#endif
	 QMessageBox::about(this, QMessageBox::tr("About TypeFaster Typing Tutor"),atext);
}

void MyMainWindow::help()
{
#ifdef FEST
	text->sayhelpfile();
#endif
	helpbrowser->show();
//	cout << helpfile.latin1() << endl;
}

void MyMainWindow::help2()
{
	helpbrowser2->show();
}
void MyMainWindow::playsound(bool ticked)
{
	if(ticked)
		text->playsound = true;
	else
		text->playsound = false;
}

void MyMainWindow::closeEvent(QCloseEvent* c)
{
	hide();
//	if(mylayout != 0)
//		delete mylayout;
//	exit(0);
	QString windowlayout;
	QString layoutwanted;
	QString fontsize;
	QString sound;
	QString lessonwanted;
	QString toolbardock;

	if(isMaximized())
		windowlayout = "showMaximized";
	else
		windowlayout.sprintf("pos %d %d size %d %d",x(),y(),width(),height());

	layoutwanted = "layout="+keyboardwidget->currentlayout;

	fontsize.sprintf("fontsize=%d",text->fontsize);

	if(text->playsound)
		sound = "sound=true";
	else
		sound = "sound=false";

	lessonwanted = "lesson="+keyboardwidget->currentlesson;

	ToolBarDock d; 
	int arb,arb3;
	bool arb2;
	getLocation(toolbar,d,arb,arb2,arb3);
	switch(d)
	{
	case Top:
		toolbardock = "toolbardock=top";
		break;
	case Bottom:
		toolbardock = "toolbardock=bottom";
		break;
	case Left:
		toolbardock = "toolbardock=left";
		break;
	case Right:
		toolbardock = "toolbardock=right";
		break;
	default: //so always shows it maximized
		toolbardock = "toolbardock=top";
	}

	QString settingslocation = "defaultuser/settings.txt";

	if(MULTI)
		settingslocation = "users/"+USERNAME+"/settings.txt";

	QFile settings(settingslocation.latin1()); //do not translate these
	settings.open(IO_WriteOnly);	
	QTextStream settingsstream(&settings);
	settingsstream.setEncoding(QTextStream::Unicode);
	settingsstream << QString("version1") << endl;
	settingsstream << windowlayout << endl;
	settingsstream << layoutwanted << endl;
	settingsstream << fontsize << endl;
	settingsstream << sound << endl;
	settingsstream << lessonwanted << endl;
	settingsstream << QString("showhelp=false")<<endl;//manually set to true for first startup
	settingsstream << toolbardock << endl;
	if(!(text->allowbackspace))
		settingsstream << "allowbackspace=false" << endl;
	
	QDictIterator<bool> it(*(lettersonlydict)); // iterator for dict
	while ( it.current() != 0 ) 
	{
		if(*it.current()==true)
					settingsstream << it.currentKey() << endl;
		++it;
	}
	settings.close();

	

	if(MULTI)
	{
		DICT->setAutoDelete(true);
		DICT->clear();
		delete DICT;
		if(changingteacher)
		{
		#ifdef WIN
			DWORD ans = WaitForSingleObject(changingteacherhandle,1000);
			if(ans == WAIT_TIMEOUT)
			{
				DWORD exitcode = 0;
				TerminateThread(changingteacherhandle,exitcode);

				if(changingteacherfile->isOpen())
					changingteacherfile->close();
				
			}
			CloseHandle(changingteacherhandle);
			delete changingteacherfile;
		#else
				struct timeval now;
              	struct timespec timeout;

              	pthread_mutex_lock(&changingteachermut);
              		gettimeofday(&now,0);
              		timeout.tv_sec = now.tv_sec + 1;
              		timeout.tv_nsec = now.tv_usec * 1000;
					if(pthread_cond_timedwait(&changingteachercond, &changingteachermut, &timeout)==ETIMEDOUT)
					{
						pthread_cancel(changingteacherhandle);
						if(changingteacherfile->isOpen())
							changingteacherfile->close();
					}
				pthread_mutex_unlock(&changingteachermut);
					
				pthread_cond_destroy(&changingteachercond);
				pthread_mutex_destroy(&changingteachermut);
					
				delete changingteacherfile;
		#endif
		}
		if(addingteacher)
		{
		#ifdef WIN
			DWORD ans = WaitForSingleObject(addingteacherhandle,1000);
			if(ans == WAIT_TIMEOUT)
			{
				DWORD exitcode = 0;
				TerminateThread(addingteacherhandle,exitcode);
				
				if(addingteacherfile->isOpen())
					addingteacherfile->close();
				
			}
			CloseHandle(addingteacherhandle);
			delete addingteacherfile;
		#else
				struct timeval now;
              	struct timespec timeout;

              	pthread_mutex_lock(&addingteachermut);
              		gettimeofday(&now,0);
              		timeout.tv_sec = now.tv_sec + 1;
              		timeout.tv_nsec = now.tv_usec * 1000;
					if(pthread_cond_timedwait(&addingteachercond, &addingteachermut, &timeout)==ETIMEDOUT)
					{
						pthread_cancel(addingteacherhandle);
						if(addingteacherfile->isOpen())
							addingteacherfile->close();
					}
				pthread_mutex_unlock(&addingteachermut);
					
				pthread_cond_destroy(&addingteachercond);
				pthread_mutex_destroy(&addingteachermut);
					
				delete addingteacherfile;
		#endif
		}
		/*
		int i;
		for(i=0;i<THREADS->size();i++)
		{
			if((*THREADS)[i].wait(10000)==false)//wait 10 seconds
			{
				if((*THREADS)[i].m!=0)
				{
					if((*THREADS)[i].m->isOpen())
						(*THREADS)[i].m->close();
				}
				QMessageBox::critical(0,"TypeFaster", tr("File write timeout, some settings may not have been saved"));
			}

		}
		*/
	}

	int i;
	QString path;
	if(MULTI)
		path = "users/"+USERNAME+"/";
	else
		path = "defaultuser/";

	for(i=0;i<text->layoutScores.size();i++)
	{

		QString path2 = path + text->layoutScores[i]->layoutname + ".txt";
		QFile scorefile(path2); 
		
			if(scorefile.open(IO_WriteOnly))
			{
				QTextStream scorefilestream(&scorefile);
				scorefilestream.setEncoding(QTextStream::Unicode);
				
				QDictIterator<numandtime> it(*(text->layoutScores[i]->letters)); // iterator for dict

				while ( it.current() != 0 ) 
				{
					scorefilestream << it.currentKey() << endl;
					numandtime* u =  it.current();
					scorefilestream << u->num << endl;
					scorefilestream << u->totaltime << endl;
					scorefilestream << u->missed << endl;
					++it;
				}
				scorefile.close();
			}
		
	}
	for(i=0;i<text->layoutScores.size();i++)
		delete text->layoutScores[i];
	c->accept();


}

MyMainWindow::~MyMainWindow()
{
	lettersonlydict->setAutoDelete(true);
	lettersonlydict->clear();
	delete lettersonlydict;

	delete browser;
	delete ok;
	delete vertlayout;
	delete helpbrowser;

	delete text;
	delete keyboardwidget;
	delete author;
	delete backgroundwidget;
	delete lessonl;
	delete layoutl;
	delete lessoncombo;
	delete incr;
	delete increasefont;
	delete decr;
	delete decreasefont;
	delete layoutcombo;
	delete playsoundbox;
	delete progress;
	delete helpp;
	delete helpb;
	delete aboutp;
	delete aboutb;
	if(deleteedit)
	{
		delete editb;
		delete editp;
		delete editd;
	}
#ifdef GAME
	delete gamep;
	delete gameb;
#endif
	delete toolbar;
	delete progchart;
	delete progresschartb;
	delete toolbar2;
#ifdef GAME
	if(myglwidget != 0)
	{
		delete playb;
		delete glaboutb;
		delete mygltoolbar;
		delete myglwidget;
		delete myglwindow;
	}
#endif
#ifdef FEST
	exit(0); //little hack because it doesn't always shut down
#endif
}
