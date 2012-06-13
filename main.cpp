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


#include <qapplication.h>
#include "MyMainWindow.h"
#include "MyLoginDialog.h"
#include "UserDetails.h"
//#include "MyThread.h"
#include <qtranslator.h>
#include <qfile.h>
#include <qstring.h>
#include <qdict.h>
#include <qstringlist.h>
#include <vector>


#include <cstdlib>
#include <time.h>

QString USERNAME;
bool ISTEACHER = false;
QString TEACHERSNAME;

bool MULTI = false;
bool TEACHERSUPPORT = false;
QString TEACHERHASH;
QDict<userDetails>* DICT;
//std::vector<MyThread>* THREADS;

int main( int argc, char** argv ) 
{
	srand(time(0));

	QApplication app( argc, argv );
#ifdef SPANISH
	QTranslator tor(0);
    tor.load("spanish.qm", ".");
    app.installTranslator(&tor);
#endif
/*
	QTranslator tor(0);
    tor.load(QTextCodec::locale(),".");
    app.installTranslator(&tor);*/
//Here, we determine which translation to use according to the current locale

	QStringList teacherlist;
	teacherlist.append(" ");

	bool quitpressed = false;

	QFile m("users.txt"); //do not translate
    if ( m.exists() )
	{
		DICT = new QDict<userDetails>(727);//arbitrary number (about num of students in school)
//		THREADS = new std::vector<MyThread>;
		MULTI = true;
		QFile t("teacherhash.txt");
		if(t.exists())
		{
			if(t.open(IO_ReadOnly))
			{
				t.readLine(TEACHERHASH,33);
				if(TEACHERHASH.length()==32)
				{
					
					TEACHERSUPPORT = true;
				}
				t.close();
			}
		}

		if ( m.open(IO_ReadOnly) ) 
		{   
			QTextStream tstream( &m );        // use a text stream
			
			
			while ( !tstream.eof() ) 
			{
				QString user = tstream.readLine();       // line of text excluding '\n'
				if(user==NULL)
					break;
				QString pass = tstream.readLine();
				if(pass==NULL||pass.length()!=32)
					break;
				QString prob = tstream.readLine();
				if(prob==NULL||prob.length()!=1)
					break;
				QString tname = tstream.readLine();
				if(tname==NULL)
					break;
				if(tstream.readLine()==0)
					break;
				bool p =false;
				if(prob=="1")
				{
					p=true;
					teacherlist.append(user);
				}
				userDetails* ud = new userDetails(user,pass,p,tname);
				DICT->insert(ud->username,ud);
			
			}
			m.close();
		}
		teacherlist.sort();
		MyLoginDialog login(teacherlist);
		//login.show();
		if(login.exec()==0)
		{
		//	app.quit();
			quitpressed = true;
		}
	}
	if(!quitpressed)
	{
		MyMainWindow window;
		return app.exec();
	}
	else
	{
		app.quit();
		return 1;
	}
}
