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
#ifndef MYPARSER_H
#define MYPARSER_H

#include <qxml.h>
#include "MyButton.h"
//#include "KeyBoardWidget.h"
#include <qwidget.h>
#include <vector>
//using namespace std;

class QString;

class MyParser : public QXmlDefaultHandler
{
public:
	//MyParser(std::vector<MyButton*>*,KeyBoardWidget*, float*);
	MyParser(std::vector<MyButton*>*,QWidget*, float*,bool*);
    bool startDocument();
    bool startElement(const QString&, const QString&, const QString&, const QXmlAttributes&);
    bool endElement(const QString&, const QString&, const QString&);
	bool endDocument();
	bool characters(const QString&);

	bool warning (const QXmlParseException & exception) ;
	bool error (const QXmlParseException & exception) ;
	bool fatalError (const QXmlParseException & exception); 

private:
	std::vector<MyButton*>* v;
//	KeyBoardWidget* m;
	QWidget* m;
	//QWidget* m;
	float* ratio;
	
	bool* lefttoright;

	bool layout;
	bool withinrow;
	bool withinkey;
	bool withinvalue;
	bool version1;

	float horizgap;
	float vertgap;

	float kwidth;
	float tkwidth;
	float kheight;

	float va;
	float vd;

//	std::vector<MyButton*> lastrow;
	MyButton* lastbutton;
	float lastrowscale;

	//MyButton::keytype when;
	std::vector<MyButton::keytype> when;
	bool draw;
	bool newline;
	QString say;
};

#endif
