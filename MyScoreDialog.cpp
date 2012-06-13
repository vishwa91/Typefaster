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

#include "MyScoreDialog.h"
#include <qapplication.h>


MyScoreDialog::MyScoreDialog(QWidget* p):QDialog(p,"scoredialog",true,WStyle_Customize|WStyle_NormalBorder|WStyle_Title|WStyle_SysMenu|WStyle_Maximize)
{
	setCaption(tr("TypeFaster - Progress Charts"));
	setSizeGripEnabled(true);
	//this->setMinimumSize(600,440);
	//4,22,-8
	int w = QApplication::desktop()->width();
	int h = QApplication::desktop()->height();
	setGeometry(w/8,25,w-w/4,h-h/8);
	vboxl = new QVBoxLayout(this,10,10);
	scrollview = new QScrollView(this,"scrollview",WResizeNoErase|WNorthWestGravity);
	//scrollview->viewport()->setBackgroundColor("white");

	scrollview->viewport()->setBackgroundMode(QWidget::PaletteBackground);
	//scrollview->viewport()->setBackgroundMode(QWidget::PaletteBase);

	letters = new QCheckBox("Letters only", this);
	drawon = new ChartWidget(scrollview->viewport(),letters);
	scrollview->addChild(drawon);
	vboxl->addWidget(scrollview);

	
	vboxl->addWidget(letters);

	hboxl = new QHBoxLayout(vboxl,10);
	ok = new QPushButton(tr("Ok\n"),this);
	ok->setDefault(true);
	connect(ok,SIGNAL(clicked()),this,SLOT(okpressed()));
	hboxl->addWidget(ok);


	
	slowest5 = new QPushButton(tr("Practise \nslowest"),this);
	connect(slowest5,SIGNAL(clicked()),this,SLOT(slowestpressed()));
	hboxl->addWidget(slowest5);

	accurate5 = new QPushButton(tr("Practise \nleast accurate"),this);
	hboxl->addWidget(accurate5);
	connect(accurate5,SIGNAL(clicked()),this,SLOT(accuratepressed()));

	custom = new QPushButton(tr("Practise \nkeys you choose"),this);
	connect(custom,SIGNAL(clicked()),this,SLOT(custompressed()));
	hboxl->addWidget(custom);

	history = new QPushButton(tr("Clear \nhistory"),this);
	hboxl->addWidget(history);

}

MyScoreDialog::~MyScoreDialog()
{
	delete history;
	delete custom;
	delete accurate5;
	delete slowest5;
	delete ok;
	delete hboxl;
	delete scrollview;
	delete letters;
	delete vboxl;
	
}

void MyScoreDialog::okpressed()
{
	done(0);
}
void MyScoreDialog::slowestpressed()
{
	done(1);
}
void MyScoreDialog::accuratepressed()
{
	done(2);
}
void MyScoreDialog::custompressed()
{
	done(3);
}
void MyScoreDialog::dodone(int x)
{
	done(x);
}
