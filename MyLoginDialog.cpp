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
#include "MyLoginDialog.h"
#include "UserDetails.h"
//#include "MyThread.h"
#include "md5.h"
#include <qdict.h>
#include <qmessagebox.h>
#include <stdio.h>
#include <qapplication.h>
#include <vector>

#include <qtextstream.h>
#include <qfile.h>

#ifdef WIN
#include <process.h>
#include <windows.h>
#else
#include <pthread.h>
#endif

extern QString USERNAME;
extern bool ISTEACHER;
extern QString TEACHERSNAME;

extern bool TEACHERSUPPORT;
extern QString TEACHERHASH;
extern QDict<userDetails>* DICT;
//extern std::vector<MyThread>* THREADS;

bool changingteacher = false;
#ifdef WIN
HANDLE changingteacherhandle;
#else
pthread_t changingteacherhandle;
pthread_mutex_t changingteachermut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t changingteachercond = PTHREAD_COND_INITIALIZER;
#endif
QFile* changingteacherfile;

bool addingteacher = false;
#ifdef WIN
HANDLE addingteacherhandle;
#else
pthread_t addingteacherhandle;
pthread_mutex_t addingteachermut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t addingteachercond = PTHREAD_COND_INITIALIZER;
#endif
QFile* addingteacherfile;

#ifdef WIN
DWORD WINAPI addteacher(LPVOID f)
{
#else
void* addteacher( void*)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
#endif
			bool found = false;
			if(addingteacherfile->open(IO_ReadOnly))
			{

				QTextStream stream(addingteacherfile);
				QString line;
				while((line=stream.readLine())!=0)
				{
					if(line == USERNAME)
					{
						found = true;
						break;
					}
				}
				addingteacherfile->close();
			}
			if(!found)
			{
				
					if(addingteacherfile->open(IO_WriteOnly | IO_Append))
					{
						QTextStream stream(addingteacherfile);
						stream << USERNAME << endl;
						addingteacherfile->close();
					}
				

			}
			#ifndef WIN
				pthread_mutex_lock(&addingteachermut);
					pthread_cond_signal(&addingteachercond);
				pthread_mutex_unlock(&addingteachermut);
			#else
				return 0;
			#endif
			  
}

#ifdef WIN
DWORD WINAPI changeteacher(LPVOID f)
{
#else
void* changeteacher(void*)
{
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,0);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,0);
#endif
	if ( changingteacherfile->open(IO_ReadOnly|IO_WriteOnly) ) 
			{   
				QString user;
				while(changingteacherfile->readLine(user,51))
				{
					if(user.length()>1 && user.mid(0,user.length()-1) == USERNAME)
					{
						bool error = false;
						QString arb;
						if(changingteacherfile->readLine(arb,51)) //password
						if(changingteacherfile->readLine(arb,51)) //indication of whether teacher or not
						{
							changingteacherfile->at(changingteacherfile->at());//sets it to where it is!!! yes it took me a while to figure this one out
							int i;
							for(i=0;i<TEACHERSNAME.length();i++)
							{
								if(changingteacherfile->putch(TEACHERSNAME[i].latin1())==-1)
								{
									error = true;
									break;
								}
							}
							if(error)
								break;
							else
							{
								if(changingteacherfile->putch('\n'))
								{
									int x;
									for(x=0;x<50-TEACHERSNAME.length();x++)
										if(changingteacherfile->putch(' ')==-1)
										{
											error = true;
											break;
										}
									if(error)
										break;
									else
										changingteacherfile->putch('\n');
								}
							}
						}
						break;
					}
					else
					{
						QString arb;
						if(changingteacherfile->readLine(arb,51)) //password
						if(changingteacherfile->readLine(arb,51)) //indication of whether teacher or not
						if(changingteacherfile->readLine(arb,51)) //first teacher name line
						changingteacherfile->readLine(arb,51);	   //second teacher name line
					}
				}
				changingteacherfile->close();
			}
			#ifndef WIN
				pthread_mutex_lock(&changingteachermut);
					pthread_cond_signal(&changingteachercond);
				pthread_mutex_unlock(&changingteachermut);
			#else
				return 0;
			#endif
}

MyLoginDialog::MyLoginDialog(QStringList teacherlist):QDialog(0,"login",true,WStyle_Customize|WStyle_DialogBorder|WStyle_Title)//  | WStyle_Tool) //| WStyle_NoBorder
{
	addDialog = new NewUser();

	setCaption(tr("TypeFaster Typing Tutor Login"));
	//setFixedSize(320,180);
	//setMinimumSize(320,180);
	grid = new QGrid(2,this);
	grid->setSpacing(10);
	grid->setMargin(10);
	grid->setFixedSize(400,180);
	//grid->setMinimumSize(320,180);

	text1 = new QLabel(tr("Username: "),grid);

	user = new QLineEdit(grid);
	user->setMaxLength(50);
	user->setFocus();
	connect(user, SIGNAL(textChanged(const QString&)), this, SLOT(userChanged(const QString&)));

	text2 = new QLabel(tr("Password: "),grid);

	pass = new QLineEdit(grid);
	pass->setEchoMode(QLineEdit::Password);
	pass->setMaxLength(50);

	if(TEACHERSUPPORT)
	{
		text3 = new QLabel(tr("Your teacher:	"),grid);
		teachers = new QComboBox(false,grid);
		teachers->insertStringList(teacherlist);

		text4 = new QLabel(tr("Teacher password:"),grid);
		text4->hide();

		pass2 = new QLineEdit(grid);
		pass2->setEchoMode(QLineEdit::Password);
		pass2->hide();
	}

	ok = new QPushButton(tr("Login"),grid);
	ok->setDefault(true);
	connect(ok, SIGNAL(clicked()),this, SLOT(okPressed()));

	QGrid* grid2 = new QGrid(2,grid);
	grid2->setSpacing(10);
	//grid2->setMargin(10);
	add = new QPushButton(tr("New User"),grid2);
	connect(add, SIGNAL(clicked()), this, SLOT(addPressed()));

	QPushButton* quit = new QPushButton(tr("Quit"),grid2);
	connect(quit,SIGNAL(clicked()),this,SLOT(quitPressed()));
	
}
MyLoginDialog::~MyLoginDialog()
{
	/*
	if(text1!=0)
		delete text1;
	if(text2!=0)
		delete text2;

	if(grid!=0)
		delete grid;
	if(user!=0)
		delete user;
	if(pass!=0)
		delete pass;
	if(ok!=0)
		delete ok;
	if(add!=0)
		delete add;
	if(TEACHERSUPPORT)
	{
	if(text3!=0)
		delete text3;
	if(text4!=0)
		delete text4;
	if(teachers!=0)
		delete teachers;
	if(pass2!=0)
		delete pass2;
	}*/
}
void MyLoginDialog::quitPressed()
{
	done(0);
}
void MyLoginDialog::okPressed()
{
	//check username and password and see if it works
	//also get a thread to change the users.txt file as to who the teacher is
	//if necessary
	QString n = user->text();
	QString p = pass->text();
	userDetails* t = DICT->find(n);
	if(t!=0)
	{
		p = "salty"+p;
		md5_state_t state;
      	md5_byte_t digest[16];
      	char hex_output[16*2 + 1];
      	int di;

      	md5_init(&state);
      	md5_append(&state, (const md5_byte_t *)p.latin1(), p.length());
      	md5_finish(&state, digest);
      	for (di = 0; di < 16; ++di)
      	    sprintf(hex_output + di * 2, "%02x", digest[di]);
		QString test = hex_output;
		if(t->hash==test)
		{
			USERNAME = user->text();
			if(TEACHERSUPPORT)
			{
				if(t->probteacher)
				{
					QString tp = pass2->text();
					tp = "arb" + tp;
					md5_state_t state;
      				md5_byte_t digest[16];
      				char hex_output[16*2 + 1];
      				int di;

      				md5_init(&state);
      				md5_append(&state, (const md5_byte_t *)tp.latin1(), tp.length());
      				md5_finish(&state, digest);
      				for (di = 0; di < 16; ++di)
      					sprintf(hex_output + di * 2, "%02x", digest[di]);
					QString test2 = hex_output;
					if(test2!=TEACHERHASH)
					{
						QMessageBox::critical(0,"TypeFaster",tr("Incorrect username and/or password/s"));
						return;
					}
					ISTEACHER = true;
					//USERNAME = user->text();
					TEACHERSNAME = " ";
					done(1);
					return;
				}
				else
				{
					TEACHERSNAME = teachers->currentText();
					if(TEACHERSNAME != t->teachername)
					{ //get a thread to write the new teachers name in the users file
					//so that it can automagically be selected next time he types in his username

						changingteacher = true;
						changingteacherfile = new QFile("users.txt");
						#ifdef WIN
							DWORD targetThreadID;
							changingteacherhandle = (HANDLE)_beginthreadex(NULL,0,(unsigned(_stdcall*)(void*))changeteacher,0,0,(unsigned*)&targetThreadID);
						#else
							pthread_create( &changingteacherhandle, NULL, changeteacher, NULL);
						#endif
					
/*
						MyThread* t = new MyThread(MyThread::changeteacher);
						THREADS->push_back(*t);
						t->start();*/
					}
					if(TEACHERSNAME != " ")
					{
						addingteacher = true;
						addingteacherfile = new QFile("users/"+TEACHERSNAME+"/students.txt");
						#ifdef WIN
							DWORD targetThreadID;
							addingteacherhandle = (HANDLE)_beginthreadex(NULL,0,(unsigned(_stdcall*)(void*))addteacher,0,0,(unsigned*)&targetThreadID);
						#else
							pthread_create( &addingteacherhandle, NULL, addteacher, NULL);
						#endif
					
						/*
						MyThread* t2 = new MyThread(MyThread::addtoteacherlist);
						THREADS->push_back(*t2);
						t2->start();*/
					}
				}
			}
			
			//delete this;
			done(1);
			return;
		}
	}
	QMessageBox::critical(0,"TypeFaster",tr("Incorrect username and/or password/s"));
}
void MyLoginDialog::userChanged(const QString& us)
{
	//hash the string, if it's a teacher then remove teachers and show the stuff
	//to ask for teacher password
//	QMessageBox::critical(0,"TypeFaster: Parse Error",us);
	if(TEACHERSUPPORT)
	{
		userDetails* t = DICT->find(us);
		if(t!=0)
		{
			if(t->probteacher)
			{
				text3->hide();
				teachers->hide();
				text4->show();
				pass2->show();
				return;
			}
			else
				//select the right teacher, now this isn't the most efficient
				//should use a hash and then quickly get the index, but really
				//there shouldn't ever be too many teachers so a linear search 
				//prob ain't too bad
			{
				int i;
				for(i=0;i<teachers->count();i++)
				{
					if(teachers->text(i)==t->teachername)
					{
						teachers->setCurrentItem(i);
						break;
					}
				}
			}
		}
		text3->show();
		teachers->show();
		text4->hide();
		pass2->hide();
	}
}
void MyLoginDialog::addPressed()
{
	addDialog->exec();
}
