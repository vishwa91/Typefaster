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

#include "ClassDialog.h"

ClassDialog::ClassDialog(QComboBox* l,QWidget* p):QDialog(p," class prog",true,WStyle_Customize | WStyle_NormalBorder | WStyle_Title)// | WStyle_SysMenu)//  | WStyle_Tool) //| WStyle_NoBorder
{
	layouts = l;

	setCaption(tr("TypeFaster"));

	setFixedSize(370,130);

	group = new QButtonGroup(tr("Class scores"),this);
	group->setGeometry(20,20,330,60);

	layout = new QLabel(tr("Layout: "),group);
	layout->setGeometry(20,20,40,30);

	mylayouts = new QComboBox(group);
	int i;
	for(i=0;i<layouts->count();i++)
		mylayouts->insertItem(layouts->text(i));
	mylayouts->setGeometry(70,25,120,20);

	view = new QPushButton(tr("View class scores"),group);
	view->setGeometry(200,20,110,30);

	ok = new QPushButton(tr("Close"),this);
	ok->setGeometry(260,90,90,30);
	ok->setDefault(true);
	connect(ok,SIGNAL(clicked()),this,SLOT(okpressed()));
}

ClassDialog::~ClassDialog()
{
	delete layout;
	delete mylayouts;
	delete view;
	delete group;

	delete ok;
}

void ClassDialog::okpressed()
{
	done(0);
}
