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
#include "NewUser.h"
#include "UserDetails.h"
#include "md5.h"
#include <qdict.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qtextstream.h>
#include <stdio.h>
#include <cstdlib>

#ifdef WIN
#include <windows.h>
#endif

#include <stdio.h>

extern bool MULTI;

extern QString USERNAME;
extern bool ISTEACHER;
extern QString TEACHERSNAME;

extern bool TEACHERSUPPORT;
extern QString TEACHERHASH;
extern QDict<userDetails>* DICT;

//MyLoginDialog::MyLoginDialog():QDialog(0,"newuser",true,WStyle_Customize|WStyle_DialogBorder|WStyle_Title)//  | WStyle_Tool) //| WStyle_NoBorder
NewUser::NewUser():QDialog(0,"newuser",true)
{
	setCaption(tr("TypeFaster Typing Tutor"));
	setFixedSize(320,200);
	grid = new QGrid(2,this);
	grid->setSpacing(10);
	grid->setMargin(10);
	grid->setFixedSize(320,200);

	text1 = new QLabel(tr("Username: "),grid);

	user = new QLineEdit(grid);
	user->setMaxLength(50);
	user->setFocus();
	//connect(user, SIGNAL(textChanged(const QString&)), this, SLOT(userChanged(const QString&)));

	text2 = new QLabel(tr("Password: "),grid);

	pass = new QLineEdit(grid);
	pass->setEchoMode(QLineEdit::Password);
	pass->setMaxLength(50);

	text3 = new QLabel(tr("Confirm password: "),grid);

	pass2 = new QLineEdit(grid);
	pass2->setEchoMode(QLineEdit::Password);
	pass2->setMaxLength(50);

	if(TEACHERSUPPORT)
	{
		check = new QCheckBox("Teacher privileges",grid);
		connect(check, SIGNAL(toggled(bool)), this, SLOT(checkToggled(bool)));

		blank = new QLabel("",grid);

		text4 = new QLabel(tr("Teacher password: "),grid);
		text4->setDisabled(true);

		teacherpass = new QLineEdit(grid);
		teacherpass->setEchoMode(QLineEdit::Password);
		teacherpass->setMaxLength(50);
		teacherpass->setDisabled(true);

	
	}

	save = new QPushButton(tr("Save New User"),grid);
	save->setDefault(true);
	connect(save, SIGNAL(clicked()),this, SLOT(savePressed()));

	cancel = new QPushButton(tr("Cancel"),grid);
	connect(cancel,SIGNAL(clicked()),this,SLOT(cancelPressed()));

	
}
NewUser::~NewUser()
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
void NewUser::checkToggled(bool p)
{
	text4->setEnabled(p);
	teacherpass->setEnabled(p);
	
}
void NewUser::cancelPressed()
{
	done(0);
}
void NewUser::getLayout(QString l,QString n) //layoutname, username
{
#ifdef WIN
	
	/*
	STARTUPINFO startInfo;
	PROCESS_INFORMATION processInfo;
	ZeroMemory(&startInfo,sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);
	if(CreateProcess(NULL,(char*)s.latin1(),NULL,NULL,false,HIGH_PRIORITY_CLASS|CREATE_NO_WINDOW,NULL,NULL,&startInfo,&processInfo))
	{
		CloseHandle(&processInfo.hThread);
		CloseHandle(&processInfo.hProcess);
	}*/
	QString from = "layouts\\"+l+".xml";
	QString to = "users\\"+n+"\\layouts\\"+l+".xml";
	CopyFile(from.latin1(),to.latin1(),false);

	QString dir = "users\\"+n+"\\layouts\\"+l;
	CreateDirectory(dir.latin1(),false);

	QString src = "layouts\\"+l;
	QDir d(src.latin1()); 
	d.setFilter(QDir::Files);
    if ( d.exists() )
	{
		unsigned int i;
		for (i=0; i<d.count(); i++ )
			if(d[i]!="."&&d[i]!="..")
			{
				from = "layouts\\"+l+"\\"+d[i];
				to = "users\\"+n+"\\layouts\\"+l+"\\"+d[i];
				CopyFile(from.latin1(),to.latin1(),false);
			}
	}
	
#else
	QString cmd = "cp -r layouts/"+l+" users/"+n+"/layouts";
	system(cmd.latin1());
	cmd = "cp layouts/"+l+".xml users/"+n+"/layouts";
	system(cmd.latin1());
#endif
}
void NewUser::savePressed()
{
	QString p = pass->text();
	QString p2 = pass2->text();
	if(p!=p2)
	{
		QMessageBox::critical(0,"TypeFaster",tr("Passwords are not the same"));
		return;
	}
	if(p.length()<3)
	{
		QMessageBox::critical(0,"TypeFaster",tr("Password must be at least 3 characters long"));
		return;
	}
	QString n = user->text().latin1();
	if(n.length()<3)
	{
		QMessageBox::critical(0,"TypeFaster",tr("Username must be at least 3 characters long"));
		return;
	}
	if(n.contains("/")>0 || n.contains("\\")>0 || n.contains(":")>0 || n.contains("*")>0 || n.contains("?")>0 || n.contains("\"")>0 || n.contains("<")>0 || n.contains(">")>0 || n.contains("|")>0)
	{
		QMessageBox::critical(0,"TypeFaster",tr("Username cannot contain any of the following characters: \n \\ / : * ? \" < > |"));
		return;
	}
	userDetails* t = DICT->find(n);
	if(t!=0||n==" ")
	{
		QMessageBox::critical(0,"TypeFaster",tr("Please choose a different username"));
		return;
	}
	bool teacherpriv = false;
	if(TEACHERSUPPORT)
	{
		if(check->isChecked())
		{ //see if got teacher pass right
					QString tp = teacherpass->text();
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
                        QMessageBox::critical(0,"TypeFaster",tr("Incorrect teacher password"));
                        return;
                    }
					else
						teacherpriv = true;
		}
	}
	
	QDir d;
	QString path = "users/";
	path += n;
	if(d.mkdir(path.latin1()))
	{
		//now append user file
		QFile m("users.txt");
		if ( m.open(IO_WriteOnly|IO_Append) ) 
		{   
			QTextStream tstream( &m );        // use a text stream
			tstream << n.latin1() << endl; //write out username

			//now write out the hash of the password
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
			QString hashofp = hex_output;
			tstream << hashofp << endl;

			
			if(teacherpriv)
				tstream << "1" << endl;
			else
				tstream << "0" << endl;
			
			//now this is the teachers name
			tstream << " " <<  endl;
			tstream << "                                                 " << endl;
			//49 spaces, because max length of any username is 50

			tstream << flush;
			m.close();

			if(teacherpriv || (MULTI && !TEACHERSUPPORT))
			{
				
				#ifdef WIN
					QString mk = "users\\"+n+"\\layouts";
					CreateDirectory(mk.latin1(),NULL);
				#else
					QString mk2 = "mkdir users/"+n+"/layouts";
					system(mk2.latin1());
				#endif
				QDir gamedir("users/"+n+"/game"); //another way of making a dir
				gamedir.mkdir(gamedir.absPath());

				#ifdef WIN
					QString from = "game-data\\settings.txt";
					QString from2 = "game-data\\words.txt";

					QString to = "users\\"+n+"\\game\\settings.txt";
					QString to2 = "users\\"+n+"\\game\\words.txt";

					CopyFile(from.latin1(),to.latin1(),false);
					CopyFile(from2.latin1(),to2.latin1(),false);

				#else
					QString cp = "cp game-data/settings.txt users/"+n+"/game";
					system(cp.latin1());
					cp = "cp game-data/words.txt users/"+n+"/game";
					system(cp.latin1());
				#endif

				getLayout("Numeric-Keypad",n);
				getLayout("US-English",n);

			
				/*
				system(cmd.latin1());
				cmd = "mkdir users\\"+n+"\\layouts\\US-English";
				system(cmd.latin1());
				cmd = "xcopy layouts\\Numeric-Keypad.xml users\\"+n+"\\layouts /Y /Q";
				system(cmd.latin1());
				cmd = "xcopy layouts\\US-English.xml users\\"+n+"\\layouts /Y /Q";
				system(cmd.latin1());
				cmd = "xcopy layouts\\US-English users\\"+n+"\\layouts\\US-English /Y /Q";
				system(cmd.latin1());
				cmd = "xcopy layouts\\Numeric-Keypad users\\"+n+"\\layouts\\Numeric-Keypad /Y /Q";
				system(cmd.latin1());*/
			}
		

			//now give them a settings file that shows help
			QString settingslocation = "users/"+n+"/settings.txt";
			QFile settings(settingslocation.latin1()); //do not translate these
			settings.open(IO_WriteOnly);	
			QTextStream settingsstream(&settings);
			settingsstream.setEncoding(QTextStream::Unicode);
			settingsstream << QString("version1") << endl;
			settingsstream << QString("showMaximized") << endl;
			settingsstream << QString("layout=US-English.xml") << endl;
			settingsstream << QString("fontsize=15") << endl;
			settingsstream << QString("sound=true") << endl;
			settingsstream << QString("Lesson 1") << endl;
			settingsstream << QString("showhelp=true")<<endl;//set to true for first startup
			settingsstream << QString("toolbardock=top") << endl;
			settings.close();


			//put in dict so it works straight away
			userDetails* ud = new userDetails(n,hashofp,teacherpriv," ");
			DICT->insert(ud->username,ud);

			done(1);
		}
		else
		{
			QMessageBox::critical(0,"TypeFaster",tr("Could not open users file, cannot save user details"));
            return;
		}
		
	}
	else
	{
		QMessageBox::critical(0,"TypeFaster",tr("Could not create user directory, please try another username"));
        return;
	}
	
	
}
