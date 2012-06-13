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
 


#include "ClassDelChart.h"
#include "UserDetails.h"
#include <qpainter.h>
#include <qfile.h>
#include <qdir.h>
#include <qtextstream.h>
//#include "studentstats.h"
#include <qmessagebox.h>
#include <vector>
#include <iostream>
using namespace std;


extern QString USERNAME;

ClassDelChart::ClassDelChart(QWidget* parent):QWidget(parent)
{

	setMaximumHeight(4000);
	setFixedWidth(345);

	del = new QPushButton(tr("Delete student/s"),this);
	check = new QPushButton(tr("Check all"),this);
	checkb = true;

	connect(del,SIGNAL(clicked()),this,SLOT(delpressed()));
	connect(check,SIGNAL(clicked()),this,SLOT(checkpressed()));

}
void ClassDelChart::setup()
{
	int i;
	for(i=0;i<names.size();i++)
		delete names[i];
	names.clear();
	//clearing is unnec because should be done on hide

	checkb = true;

	QFile f("users/"+USERNAME+"/students.txt");
	if(f.exists())
	{
		if(f.open(IO_ReadOnly))
		{
			QTextStream s(&f);
		//	s.setEncoding(QTextStream::Unicode);
			QString line;
			while((line=s.readLine())!=0)
			{
				nameandbox* tmp = new nameandbox(line,this);
				names.push_back(tmp);
			}
			f.close();
		}
	}
	setFixedHeight(names.size()*20+140);
}
void ClassDelChart::checkpressed()
{
	int i;
	if(checkb)
	{
		for(i=0;i<names.size();i++)
			names[i]->box->setChecked(true);
		check->setText(tr("Uncheck all"));
	}
	else
	{
		for(i=0;i<names.size();i++)
			names[i]->box->setChecked(false);
		check->setText(tr("Check all"));
	}

	checkb = !checkb;
}
void ClassDelChart::paintEvent (QPaintEvent* pe)
{
	

	QPainter paint( this );
	paint.setFont(QFont("courier", 16, QFont::Bold));
    
	paint.drawText(150,25,tr("Delete students"));	

//	paint.setBrush(QBrush(QBrush::SolidPattern));

	paint.setFont(QFont("courier", 10));
	int i;
	for(i=0;i<names.size();i++)
	{
		paint.drawText(20,70+i*20,names[i]->name);
		names[i]->box->setGeometry(180,60+i*20,13,13);
	}

	del->setGeometry(180,60+i*20+20,100,30);
	check->setGeometry(60,60+i*20+20,100,30);

}

void ClassDelChart::delstudent(QString s)
{
	//note that this is not a recursive delete of the directory
	//so it can only be used on students, not on teachers or in 
	//a multi-user system with no teacher support

	//the directory with files in is removed
	QDir d("users/"+s);
	d.setFilter(QDir::Files);
	if ( d.exists() )
	{
		int x;
		for (x=0; x<d.count(); x++ )
			if(d[x]!="."&&d[x]!="..")
			{
				QFile::remove(d.absPath()+"/"+d[x]);
			}
		d.rmdir(d.absPath()); //won't work if there are dirs inside
	}

	//now remove from the users.txt file
	//first read it in to mem, without including the one we are removing,
	//then write it out again
	vector<userDetails> users;

	QFile m("users.txt"); //do not translate
    if ( m.exists() )
	{
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
					p=true;

				if(user!=s)
				{
					userDetails tmp(user,pass,p,tname);
					users.push_back(tmp);
				}
			
			}
			m.close();
		}
	}

		QFile n("users.txt");
		if ( n.open(IO_WriteOnly) )  //overwrite it
		{   
			QTextStream tstream( &n );        // use a text stream

			int i;
			for(i=0;i<users.size();i++)
			{
				tstream << users[i].username.latin1() << endl; //write out username
				tstream << users[i].hash.latin1() << endl;

				if(users[i].probteacher)
					tstream << "1" << endl;
				else
					tstream << "0" << endl;
				
				//now this is the teachers name
				tstream << users[i].teachername.latin1() << endl;
				int j;
				for(j=0;j< 50-users[i].teachername.length();j++)
					tstream << " ";
				tstream << endl;
			
			}
			n.close();
		}

}
void ClassDelChart::delpressed()
{
	bool delled = false; //was even one deleted
	vector<nameandbox*>::iterator it = names.begin();

	switch( QMessageBox::warning( this, "TypeFaster",
                                  "Are you sure you want to delete student profile/s?\n"
                                  "Note that the students selected will no longer be able to login.\n",
                                  QMessageBox::Yes | QMessageBox::Default,
                                  QMessageBox::No | QMessageBox::Escape )) 
	{
    case QMessageBox::Yes: // want to delete
		                                  
		int i;
		
		while(it!=names.end())
		{
			if((*it)->box->isChecked())
			{
				delled = true;
				delstudent((*it)->name);
				delete *it;
				names.erase(it);
			}
			else
				it++;
		}
		
        repaint();

		if(delled)
		{
			QFile f("users/"+USERNAME+"/students.txt");	
			if(f.open(IO_WriteOnly))
			{
				QTextStream s(&f);
				for(i=0;i<names.size();i++)
					s << names[i]->name << endl;
				f.close();
			}
		}

        break;

    case QMessageBox::No: // escape or no
        // abort
        break;
    }
	
	
}
void ClassDelChart::hideEvent(QHideEvent* e)
{
	if(!e->spontaneous())
	{
		int i;
		for(i=0;i<names.size();i++)
			delete names[i];
		names.clear();
	
	}

}
