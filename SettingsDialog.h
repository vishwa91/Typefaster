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
#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H
#include <qdialog.h>
#include <qgrid.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qstringlist.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qmultilineedit.h>
#include <qlineedit.h>
#include <qdial.h>
#include <qslider.h>
#include <vector>
using namespace std;

class SettingsDialog : public QDialog
{
	Q_OBJECT
public:
	SettingsDialog(QComboBox*,QComboBox*,QWidget*);
	~SettingsDialog();

	void dothere();
	void doavailable();

	void getLayout(QString);

	void populatelessoneditd();
	void lessoneditdefaults();

	QPushButton* layoutsenabled;
	QComboBox* layouts;
	QPushButton* editlessons;
	QPushButton* gamesettings;
	QPushButton* ok;

	QButtonGroup* layoutsenabledb;
	QButtonGroup* lessonsb;
	QButtonGroup* gamesettingsb;

	QDialog* layoutsd;
	QButtonGroup* layoutsdb;
	QPushButton* savelayouts;
	QPushButton* cancellayouts;

	QComboBox* layoutcombo;

	vector<QCheckBox*> there;
	vector<QCheckBox*> available;

	QDialog* lessonsd;
	QLabel* lessonslabel;

	QButtonGroup* lessonsgroup;
	QComboBox* lessonscombo;
	QPushButton* lessonsedit;
	QPushButton* lessonsdelete;

	QButtonGroup* lessonsgroup2;
	QPushButton* lessonsnew;

	QPushButton* lessonsok;

	QString lessonsloc;

	struct lessonandnum
	{
		QString lesson;
		int num;
		lessonandnum(QString l,int n)
		{
			lesson = l;
			num = n;
		}
	};

	QComboBox* reallessoncombo;

	QDialog* lessoneditd;
	QLabel* lessoneditlabel;
	QLineEdit* lessoneditline;
	QLabel* lessoneditlabel2;
	QLineEdit* lessoneditline2;
	QLabel* lessoneditlabel3;
	QComboBox* lessoneditcombo;
	QCheckBox* lessoneditcheck;
	QMultiLineEdit* lessoneditmulti;
	QPushButton* lessoneditsave;
	QPushButton* lessoneditcancel;
	QGridLayout* grid;
	bool newlessonwanted;

	QDialog* gamed;
	QGridLayout* gamegrid;
	QLabel* gamelabel;
	QMultiLineEdit* gamemulti;
	QLabel* gamelabel2;
	QDial* gamedial;
//	QSlider* gameslider;
	QLabel* gamelabel3;
	QDial* gamedial2;
	//QSlider* gameslider2;
	QPushButton* gamesave;
	QPushButton* gamecancel;

public slots:

	void lessonseditpressed();
	void lessonsdeletepressed();
	void lessonsnewpressed();

	void layoutsenabledpressed();
	void editlessonspressed();
	void gamesettingspressed();
	void okpressed();

	void savelayoutspressed();
	void cancellayoutspressed();

	void lessoneditsavepressed();

	void gamesavepressed();
	void gamecancelpressed();
	

signals:
	void somethingpressed(int);
	void activatelayout(int);
	


};
#endif
