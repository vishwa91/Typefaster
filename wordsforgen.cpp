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

#include "wordsforgen.h"
#include <qfile.h>
#include <qtextstream.h>
#include <iostream>
//typedef vector<QString*> vecqstrp;

wordsforgen::wordsforgen(QString lname)
{
	layoutname = lname;
	hasfile = false;

	if(lname.length()>4)
		lname = lname.mid(0,lname.length()-4);

	QFile file("words/"+lname+".txt"); 
	
	if(file.exists())
	{
		if(file.open(IO_ReadOnly))
		{
			hasfile = true;
			words = new QDict<vecqstrp>(109);

			QTextStream stream(&file);
			stream.setEncoding(QTextStream::Unicode);

			while(!stream.atEnd())
			{
				QString* line = new QString();
				*line = stream.readLine();
				fordelete.push_back(line);
				int i;
				for(i=0;i<line->length();i++)
				{
					QString let = (QChar)line->at(i);
					vecqstrp* tmp = words->find(let);
					if(tmp == 0)
					{
						vector<QString*>* tmp2 = new vector<QString*>();
						tmp2->push_back(line);
						words->insert(let,tmp2);
					}
					else
						tmp->push_back(line);
				}
			}

			file.close();
		}
	}

		
}

wordsforgen::~wordsforgen()
{
	if(hasfile)
	{
		int i;
		for(i=0;i<fordelete.size();i++)
			delete fordelete[i];
		words->setAutoDelete(true);
		words->clear();
		delete words;
	}
}
