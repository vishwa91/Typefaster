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
#include "MyDialog.h"


MyDialog::MyDialog():QDialog(0,"stats stuff",true)
{
	setCaption(tr("TypeFaster Typing Tutor"));
	setFixedSize(320,120);
	grid = new QGrid(2,this);
	grid->setFixedSize(320,120);
	text1 = new QLabel(grid);
	text1->setBackgroundMode(PaletteBase);
	text2 = new QLabel(grid);
	text2->setBackgroundMode(PaletteBase);
}
MyDialog::~MyDialog()
{
	if(text1!=0)
		delete text1;
	if(text2!=0)
		delete text2;
	if(grid!=0)
		delete grid;
}
void MyDialog::closeEvent(QCloseEvent* c)
{
	emit mydialogclosed(); //so it can restart the lesson
	c->accept();
}

void MyDialog::keyPressEvent(QKeyEvent* k)
{
	if(k->key()==Key_Escape)
		emit mydialogclosed();
	else
		k->ignore();
}

/*
void MyDialog::focusInEvent(QFocusEvent*)
{
	resultfocus = true;
}
*/
/*
void MyDialog::focusOutEvent(QFocusEvent*)
{
	resultfocus = false;
}
*/
