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

#include "MyHelpDialog.h"
#include "MyMainWindow.h"

#ifdef FEST
MyHelpDialog::MyHelpDialog(TextWidget* t,MyMainWindow* m) : QDialog(0,0,true,WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu)
#else
MyHelpDialog::MyHelpDialog(TextWidget* t,MyMainWindow* m) : QDialog(0,0,false,WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WStyle_MinMax)
#endif
{
				textw = t;
				mymainwindow = m;
}

void MyHelpDialog::closeEvent(QCloseEvent* e)
{
#ifdef FEST
		textw->stopsayhelpfile();
#endif
		//e->accept();
		hide();
		mymainwindow->setActiveWindow();
}

void MyHelpDialog::keyPressEvent(QKeyEvent* k)
{
	if(k->key()==Key_Escape)
	{
#ifdef FEST
		textw->stopsayhelpfile();
#endif
		hide();
		mymainwindow->setActiveWindow();
	}
	else
		k->ignore();
}
	
void MyHelpDialog::closed()
{
#ifdef FEST
	textw->stopsayhelpfile();
#endif
	hide();
	//accept();
	mymainwindow->setActiveWindow();
}
