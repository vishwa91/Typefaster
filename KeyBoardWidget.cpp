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
#include "KeyBoardWidget.h"
#include "HardCoded.h"
#include "MyMainWindow.h"

#include <stdlib.h> // added by vishwanath on 6/5/12
#include <qmessagebox.h>
#include <qxml.h>
#include <qtextstream.h>
#include <qdir.h>
#include <qcstring.h>
#include <qstring.h>
#include <qinputdialog.h>
//#include <iostream>
//using namespace std;

#ifdef FEST
extern bool resultfocus;
#endif

KeyBoardWidget::KeyBoardWidget(QString lay,QWidget* parent,TextWidget* t,QLabel* auth,QComboBox* le,QComboBox* la,MyMainWindow* m,const char* name):QWidget(parent,name)
{
	layoutslocation = lay;

	mymainwindow = m;

	setFocusPolicy(QWidget::NoFocus);
	setDisabled(true);
	text = t;
	author = auth;
	layoutcombo = la;
	connect(layoutcombo, SIGNAL(activated(int)), this, SLOT(layoutActivated(int)));

	lessoncombo = le;
	connect(lessoncombo, SIGNAL(activated(int)), this, SLOT(lessonActivated(int)));
//	fontsize = fs;
	l=0;
}
KeyBoardWidget::~KeyBoardWidget()
{
	if(l!=0)
		delete l;
	int i;
	for(i=0;i<wordsforgens.size();i++)
		delete wordsforgens[i];
}
void KeyBoardWidget::parseFile(QString layoutwanted,bool hardcoded,QString lessonwanted)
{
//	cout << "parsing file "<<layoutwanted.latin1()<<" hardcoded "<<hardcoded << endl;

	MyButton::mybuttons.clear();
	for(int x=0;x<v.size();x++)
		delete v[x];
	v.clear();

	MyParser handler(&v,this,&ratio,&lefttoright);

	QTextStream* xmlstream;
	QFile* xmlFile;
	QByteArray* arr;

	if(hardcoded)
	{
		currentlayout = "US-English.xml";
		arr = new QByteArray();
		arr->assign(USEnglish.latin1(),USEnglish.length());
		xmlstream = new QTextStream(*arr,IO_ReadOnly);
	}
	else
	{
		xmlFile = new QFile(layoutslocation+"/"+layoutwanted);
		if(!xmlFile->exists())
		{
			//cout << "Error, file does not exist" <<endl;
			delete xmlFile;
			//exit(1);
			currentlayout = "US-English.xml";
			arr = new QByteArray();
			arr->assign(USEnglish.latin1(),USEnglish.length());
			xmlstream = new QTextStream(*arr,IO_ReadOnly);
			hardcoded = true;
		}
		else
		{

			currentlayout = layoutwanted;
			xmlFile->open(IO_ReadOnly);
		//	QTextStream tmp(xmlFile);
				//QTextStream xmlstream(&xmlFile);
		//	xmlstream = &tmp;
			xmlstream = new QTextStream(xmlFile);
		}
	}

	if(!hardcoded)
		xmlstream->setEncoding(QTextStream::Unicode);
	

    QXmlInputSource source(*xmlstream);
	
	QXmlSimpleReader reader;
	reader.setContentHandler(&handler);
	reader.setErrorHandler(&handler);

	reader.parse(source);


	delete xmlstream;

	if(!hardcoded)
	{
		xmlFile->close();
		delete xmlFile;
	}
//	else
//		delete arr;


	int i;
	for(i=0;i<v.size();i++)
		v[i]->calcps(); //can calc %s now that it has kwidth and kheight

	if(l==0)
		l = new MyLayout(this,ratio);
	else
	{
//		cout << "removing and deleting " <<endl;
		l->removeAndDelete();
		l->ratio = ratio;
	}

	for(i=0; i<v.size(); i++) //irregular buttons first
	{
		if(v[i]->shape == MyButton::irregular)
		{
			l->add(v[i]);
			l->addDetails(v[i]->pacross,v[i]->pdown,v[i]->pheight,v[i]->pwidth);
		}
	}
	for(i=0; i<v.size(); i++)
	{
		if(v[i]->shape != MyButton::irregular)
		{
			l->add(v[i]);
			l->addDetails(v[i]->pacross,v[i]->pdown,v[i]->pheight,v[i]->pwidth);
		}
	}

	l->activate();
	
	for(i=0; i<v.size(); i++)
		v[i]->show();

//	cout << "initializing text " <<endl;
	text->initialize(v,currentlayout);
	bool found = false;
	for(i=0;i<wordsforgens.size();i++)
	{
		if(wordsforgens[i]->layoutname == currentlayout)
		{
			found = true;
			wordsforgensi = i;
			break;
		}
	}
	if(!found)
	{
		wordsforgens.push_back(new wordsforgen(currentlayout));
		wordsforgensi = wordsforgens.size()-1;
	}
//	cout << "doing lesson" <<endl;
	if(hardcoded)
		doLesson("",lessonwanted);
	else
		doLesson(layoutwanted.mid(0,layoutwanted.length()-4),lessonwanted);
	
	
}
QString KeyBoardWidget::homekeys(QString s)
{
	QString build;
	vector<letandind> ind;
	if(s!=QString::null && s.length()>0)
	{
		int x;
		for(x=0 ; x<v.size();x++)
		{
			int y;
			for(y=0;y<v[x]->values.size();y++)
				if(s.contains(v[x]->values[y].val))
				{
					letandind tmp(v[x]->values[y].val,x);
					if(v[x]->homekey)
						tmp.ishome = true;
					else
						tmp.homeindex = v[x]->homeindex;

					ind.push_back(tmp);
				}
		}
	}
	if(ind.size()>0)
	{
		int x;
		for(x=0;x<ind.size();x++)
		{
			
			if(!ind[x].ishome && ind[x].homeindex > 0 && ind[x].homeindex < v.size())
			{
				int y;
				QString home = "";
				for(y=0;y<v[ind[x].homeindex]->values.size();y++)
					if(v[ind[x].homeindex]->values[y].when.size() > 0)
						if(v[ind[x].homeindex]->values[y].when[0] == MyButton::normal)
						{
							home = v[ind[x].homeindex]->values[y].val;
							break;
						}
				if(home!="")
				{
					build += home+" "+ home + ind[x].let + " " + home + ind[x].let + home + " ";
				}
			}

			build += ind[x].let + " ";
		}
	}
	
	else //just give them the homekeys
	{
		vector<int> homesi;
		int i;
		for(i=0 ; i<v.size();i++)
			if(v[i]->homekey)
				homesi.push_back(i);
		int y;
		for(i=0;i<homesi.size();i++)
			for(y=0;y<v[homesi[i]]->values.size();y++)
					  if(v[homesi[i]]->values[y].when.size() > 0)
						if(v[homesi[i]]->values[y].when[0] == MyButton::normal)
							build += v[homesi[i]]->values[y].val + " ";
						

	}
	return build;
}
QString KeyBoardWidget::buildlesson(QString s)
{
	QString less;

	if(s!=QString::null && s.length()>0)
	{
		s = s.mid(0,10); //don't work with more than this
		less += homekeys(s);
		
		
		if(wordsforgens[wordsforgensi]->hasfile)
		{
			int lettermax = 100/s.length(); //max words for each letter
			vector<int> maxwords; 
			int totalwords = 0;
			int i;
			for(i=0;i<s.length();i++)
			{
				vecqstrp* tmp = wordsforgens[wordsforgensi]->words->find(QString(s.at(i)));
				if(tmp==0)
					maxwords.push_back(0);
				else
				if(tmp->size()<lettermax)
				{
					totalwords += tmp->size();
					maxwords.push_back(tmp->size());
				}
				else
				{
					totalwords += lettermax;
					maxwords.push_back(lettermax);
				}
			} //now maxwords is the same length as s, and it contains the number of words
			//that we can make from each letter

			int made = 0;
			while(made < totalwords)
			{
				for(i=0;i<s.length();i++)
				{
					if(maxwords[i]>0)
					{
						maxwords[i]--;
						made++;
						vecqstrp* tmp = wordsforgens[wordsforgensi]->words->find(QString(s.at(i)));
						if(tmp != 0)
						{
							int r = rand()%tmp->size();
							less += *(*tmp)[r] + " ";
							
						}
					}
				}
			}
			return less;
		}
		else
		//	return less + " sorry, no words file found for this layout in the words directory";
			return less;
	}
	else
	{
		less += homekeys(s);
		return less;
	}
}
QString KeyBoardWidget::slowest()
{
	text->doScoreCalc(); //quite frankly I know this is not so efficient but with today's processors, who gives a fuck
	QString slowest = text->scoredialog->drawon->slowest;
	return buildlesson(slowest);
}
QString KeyBoardWidget::leastaccurate()
{
	text->doScoreCalc();
	QString worst = text->scoredialog->drawon->worst;
	return buildlesson(worst);
}
QString KeyBoardWidget::custom(QString s)
{
	return buildlesson(s);
}
//void KeyBoardWidget::genLessons()
QString KeyBoardWidget::all()
{
	
	QString genlesson = "";
	bool notstandard=false;
	int i,x,y;
	if(v.size()==59 || v.size()==58)
	{
		for(i=29;i<33;i++)
			if(!v[i]->homekey)
			{
				notstandard = true;
				break;
			}
		if(!notstandard)
		for(i=35;i<39;i++)
			if(!v[i]->homekey)
			{
				notstandard = true;
				break;
			}
		if(!notstandard)
		{
			for(i=0;i<5;i++)
			{
				for(x=29;x<33;x++)
				{
					for(y=0;y<v[x]->values.size();y++)
					  if(v[x]->values[y].when.size() > 0)
						if(v[x]->values[y].when[0] == MyButton::normal)
						{
							if(lefttoright)
								genlesson += v[x]->values[y].val;
							else
								genlesson = v[x]->values[y].val + genlesson;
						}

					for(y=0;y<v[x+9-2*(x-29)]->values.size();y++)
					  if(v[x+9-2*(x-29)]->values[y].when.size() > 0)
						if(v[x+9-2*(x-29)]->values[y].when[0] == MyButton::normal)
						{
							if(lefttoright)
								genlesson += v[x+9-2*(x-29)]->values[y].val;
							else
								genlesson = v[x+9-2*(x-29)]->values[y].val + genlesson;
						}
					if(lefttoright)
						genlesson += " ";
					else
						genlesson = " " + genlesson;
				}
			} //end of homerow warmup
			
			for(i=15;i<53;i++)
			{
				if(!v[i]->homekey)
				{
					QString val = "";
					QString home = "";
					for(x=0;x<v[i]->values.size();x++)
						if(v[i]->values[x].when.size() > 0)
							if(v[i]->values[x].when[0] == MyButton::normal)
							{
								val = v[i]->values[x].val;
								break;
							}
					if(val!="" && v[i]->homeindex > 0 && v[i]->homeindex < v.size())
					{
						for(x=0;x<v[v[i]->homeindex]->values.size();x++)
							if(v[v[i]->homeindex]->values[x].when.size() > 0)
								if(v[v[i]->homeindex]->values[x].when[0] == MyButton::normal)
								{
									home = v[v[i]->homeindex]->values[x].val;
									break;
								}
						if(lefttoright)
							genlesson += home+" "+home+val+" "+home+val+home+" ";
						else
							genlesson = " "+home+val+home+" "+val+home+" "+home+genlesson;
					}
				}

			} //rest of normal keys done, with their home keys

			for(i=0 ; i<v.size();i++) //this now does them all
			{
				for(x=0;x<v[i]->values.size();x++)
					if(lefttoright)
						genlesson += v[i]->values[x].val;
					else
						genlesson = v[i]->values[x].val + genlesson;
				if(v[i]->values.size()>0) //just so don't add space for ctrl,alt,shift ...
					if(lefttoright)
						genlesson += " ";
					else
						genlesson = " "+genlesson;
			}

		}
	}
	else
		notstandard = true;

	if(notstandard)
	for(i=0 ; i<v.size();i++) //just do them all
	{
		for(x=0;x<v[i]->values.size();x++)
			if(lefttoright)
				genlesson += v[i]->values[x].val;
			else
				genlesson = v[i]->values[x].val + genlesson;
		if(v[i]->values.size()>0)
			if(lefttoright)
				genlesson += " ";
			else
				genlesson = " "+ genlesson;
	}

	//no translations necessary here
	/*
	if(lefttoright)
		genlesson = "PLEASE SEND LESSON FILES TO MATAAV@YAHOO.CO.UK " + genlesson;
	else
		genlesson += " PLEASE SEND LESSON FILES TO MATAAV@YAHOO.CO.UK";
	*/

	//lesson.clear();
	//lesson.push_back(genlesson);
	//text->displayLesson(&lesson,lefttoright,false);
	//text->displayLesson("No lesson files are available for this keyboard layout",true);
	return genlesson;
}
void KeyBoardWidget::doLesson(QString layoutdir,QString lessonwanted)
{

	lessoncombo->clear();
	lessoncombo->insertItem(tr("All"));
	lessoncombo->insertItem(tr("Slowest"));
	lessoncombo->insertItem(tr("Least accurate"));
	lessoncombo->insertItem(tr("Custom"));
	

	int index = 0;
	if(layoutdir!="")
	{
		QDir d(layoutslocation+"/"+layoutdir);
		if ( !d.exists() )
		{
			//std::cout << "Error, layouts "<<layoutdir.latin1()<<" directory not found" << std::endl;
			//QString b = layoutdir + " directory not found");
			QMessageBox::critical(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lessons directory not found"));
		
		}
		else
		{

			d.setFilter(QDir::Files);
		//	d.setSorting(QDir::Name);
			
			std::vector<lessonandnum> lessons;
			std::vector<QString> othernames;
			int i;
			for (i=0; i<d.count(); i++ )
			{
				if(d[i]!="."&&d[i]!=".."&&d[i].mid(d[i].length()-4,4)==".txt")
				{
					if(d[i].mid(0,7) == "Lesson " || d[i].mid(0,8) == "lecci3n ")
					{
						bool ok = false;
						int num = d[i].mid(7,d[i].length()-11).toInt(&ok);
						if(ok)
							lessons.push_back(lessonandnum(d[i].mid(0,d[i].length()-4),num));
						else
							othernames.push_back(d[i].mid(0,d[i].length()-4));
					}
					else
						othernames.push_back(d[i].mid(0,d[i].length()-4));
				}
			}
			int onelesslessonsize = lessons.size()-1;
			//now sort the lessons by number
			for(int x=0;x < onelesslessonsize ;x++)  
			{
				for(int y = x+1; y<lessons.size(); y++)
					if(lessons[x].num > lessons[y].num)
					{
						lessonandnum tmp = lessons[x]; //default copy constructor
						lessons[x] = lessons[y];
						lessons[y] = tmp;
					}
			}
		
			
				
            for(i=(int)othernames.size()-1;i>=0;i--)
            {
                lessoncombo->insertItem(othernames[i],0);
            }
            for(i=(int)lessons.size()-1;i>=0;i--)
                lessoncombo->insertItem(lessons[i].lesson,0);
            
            if(lessonwanted != "")
            {
                 for(i=0;i<lessoncombo->count();i++)
                    if(lessoncombo->text(i)==lessonwanted)
                    {
                        index = i;
                        break;
                    }
                    
            }
				
			
		} //end dir didn't exist
	} //end we didn't even try for a lessons dir since the layout is hardcoded
	//lessoncombo->adjustSize();
	lessoncombo->setCurrentItem(index);
	lessonActivated(index);
}
void KeyBoardWidget::layoutActivated(int index)
{
	QFile tmp(layoutslocation+"/"+layoutcombo->text(index)+".xml");
	if(tmp.exists())
		parseFile(layoutcombo->text(index)+".xml",false);
	//no translation necessary here
	else
	{
		if(layoutcombo->count()>1)
			layoutcombo->removeItem(layoutcombo->currentItem());
		for(int i=0;i<layoutcombo->count();i++)
			if(layoutcombo->text(i)==currentlayout.mid(0,currentlayout.length()-4))
				layoutcombo->setCurrentItem(i);
			QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, layout file no longer exists"));
	}
}
void KeyBoardWidget::removeLesson()
{
	lessoncombo->removeItem(lessoncombo->currentItem());
	if(lessoncombo->count()>0) //should always be at least 4
	{
		lessoncombo->setCurrentItem(0);
		lessonActivated(0);
	}
//	else
//		genLessons();
}
void KeyBoardWidget::lessonActivated(int index)
{
	bool ltr = lefttoright; //the lesson file defaults to the layouts directio
	QString authortext = tr("Auto generated");
	QString lessontext;
	bool numeric=false;

	lesson.clear();

	lessoncombo->setCurrentItem(index);

	currentlessoni = index;
	currentlesson = lessoncombo->currentText();

	if(lessoncombo->currentText()==tr("All"))
	{
		QString l = all();
		//lesson.push_back(all());
		if(currentlayout==tr("Numeric-Keypad.xml"))
		{
			numeric = true;
			QTextStream stream(&l,IO_ReadOnly);
			while(!stream.atEnd())
			{
				QString word;
				stream >> word;
				lesson.push_back(word);
			}
		}
		else lesson.push_back(l);
	}
	else
	if(lessoncombo->currentText()==tr("Slowest"))
	{
		QString l = slowest();
		//lesson.push_back(slowest());
		if(currentlayout==tr("Numeric-Keypad.xml"))
		{
			numeric = true;
			QTextStream stream(&l,IO_ReadOnly);
			while(!stream.atEnd())
			{
				QString word;
				stream >> word;
				lesson.push_back(word);
			}
		}
		else lesson.push_back(l);
	}
	else
	if(lessoncombo->currentText()==tr("Least accurate"))
	{
		QString l = leastaccurate();
		if(currentlayout==tr("Numeric-Keypad.xml"))
		{
			numeric = true;
			QTextStream stream(&l,IO_ReadOnly);
			while(!stream.atEnd())
			{
				QString word;
				stream >> word;
				lesson.push_back(word);
			}
		}
		else lesson.push_back(l);
	}
	else
	if(lessoncombo->currentText()==tr("Custom"))
	{
		bool ok; //although I don't care if you press cancel
		//lesson.push_back(custom(QInputDialog::getText( tr("TypeFaster"), tr("Please enter the keys you wish to practise"), QString::null, &ok, this )));
		#ifdef WIN
		QString l = custom(QInputDialog::getText( tr("TypeFaster"), tr("Please enter the keys you wish to practise (max 10)"), QString::null, &ok, this ));
		#else
		QString l = custom(QInputDialog::getText( tr("TypeFaster"), tr("Please enter the keys you wish to practise (max 10)")));
		#endif
		if(currentlayout==tr("Numeric-Keypad.xml"))
		{
			numeric = true;
			QTextStream stream(&l,IO_ReadOnly);
			while(!stream.atEnd())
			{
				QString word;
				stream >> word;
				lesson.push_back(word);
			}
		}
		else lesson.push_back(l);
	}
	else
	{

		QFile lessonfile(layoutslocation+"/"+currentlayout.mid(0,currentlayout.length()-4)+"/"+lessoncombo->text(index)+".txt");
		//no translation necessary here
		if(lessonfile.exists())
		{
			lessonfile.open(IO_ReadOnly);
		
			QTextStream lessonstream(&lessonfile);
			lessonstream.setEncoding(QTextStream::Unicode);
			QString line;
			if((line=lessonstream.readLine())!=0)
			{
				if(line=="version=1")
				{
					if((line=lessonstream.readLine())!=0)
					{
						if(line.mid(0,12)=="lefttoright=")
						{
							QString tmp = line.mid(12,5);
							if(tmp=="true" || tmp=="false")
							{
								if(tmp=="true")
									ltr=true;
								else
								{
	#ifdef FEST
									ltr = true;
									QMessageBox::critical(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Sorry, the accessible version does not support right to left text"));
	#else
									ltr=false;
	#endif
								}

								if((line=lessonstream.readLine())!=0)
								{
									if(line.mid(0,7)=="author=")
									{
										authortext = line.mid(7,line.length());
										if((line=lessonstream.readLine())!=0)
										{
											if(line=="numeric=true"||line=="numeric=false"||line=="poem=true")
											{
												
												bool poem=false;
												if(line=="numeric=true")
													numeric=true;
												else
													if(line=="poem=true")
														poem=true;
												//lesson.clear();
												while((line=lessonstream.readLine())!=0)
												{
													if(numeric||poem)
													{
														if(line!="")
															lesson.push_back(line);
													}
													else
													{
														if(ltr)
															lessontext += line;
														else
															lessontext = line + lessontext;
													}
												}
												if(!numeric && !poem)
													lesson.push_back(lessontext);

												

											}
											else
											{
												QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has invalid numeric line"));
												removeLesson();
												return;
											}
										}
										else
										{
											QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no numeric line"));
											removeLesson();
											return;
										}
									}
									else
									{
										QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no author line"));
										removeLesson();
										return;
									}
								}
								else
								{
									QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no author line"));
									removeLesson();
									return;
								}
							}
							else
							{
								QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file's lefttoright is neither true nor false"));
								removeLesson();
								return;
							}
						}
						else
						{
							QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no lefttoright line"));
							removeLesson();
							return;
						}
					}
					else
					{
						QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no lefttoright line"));
						removeLesson();
						return;
					}
				}
				else
				{
					QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file version is not 1, try upgrading the software"));
					removeLesson();
					return;
				}
			}
			else
			{
				QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file has no lines"));
				removeLesson();
				return;
			}
		}
		else
		{
			QMessageBox::warning(0,QMessageBox::tr("TypeFaster Typing Tutor"),QMessageBox::tr("Warning, lesson file no longer exists"));
			removeLesson();
			return;
		}
	}

#ifdef SPANISH
	authortext = "\nAutor de la lecci\F3n: "+authortext+"\n";
#else
	authortext = "\nLesson Author: "+authortext+"\n";
#endif
	author->setText(authortext);
	text->displayLesson(&lesson,ltr,numeric);
}

void KeyBoardWidget::next()
{
	text->mydialog->hide();
	mymainwindow->setActiveWindow();
	
#ifdef FEST
	resultfocus = false;
#endif
	
//	if(currentlessoni == -1)
//		genLessons();
//	else
//	{
		if(currentlessoni == lessoncombo->count()-1)
			lessonActivated(0); //loop round to beginning
		else
			lessonActivated(currentlessoni+1);
//	}
}
void KeyBoardWidget::redo()
{
	text->mydialog->hide();
	mymainwindow->setActiveWindow();
#ifdef FEST
	resultfocus = false;
#endif
	
//	if(currentlessoni == -1)
//		genLessons();
//	else
		lessonActivated(currentlessoni);
}
void KeyBoardWidget::previous()
{
//	if(currentlessoni == -1)
//		genLessons();
//	else
//	{
		if(currentlessoni == 0)
			lessonActivated(lessoncombo->count()-1);
		else
			lessonActivated(currentlessoni-1);
//	}
}
/*
void KeyBoardWidget::combowidget(QComboBox* c)
{
	combobox = c;
	connect(combobox, SIGNAL(activated(const QString&)), this, SLOT(act(const QString&)));
}*/
