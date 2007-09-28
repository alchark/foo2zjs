//	Hannah, GUI for downloading specific printer firmware
//	Copyright (C) 2007 Steffen Joeris <white@debian.org>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


#ifndef GUI_H
#define GUI_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QApplication>
#include <QProcess>
#include <QCheckBox>
#include <QAbstractButton>
#include <QButtonGroup>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0);

private:
	int state;
	void createMainWidget();
	QCheckBox *hp2600Button;
	QCheckBox *hp1600Button;
	QCheckBox *hp1020Button;
	QCheckBox *hp1018Button;
	QCheckBox *hp1005Button;
	QCheckBox *hp1000Button;
	QCheckBox *minolta2530Button;
	QCheckBox *minolta2490Button;
	QCheckBox *minolta2480Button;
	QCheckBox *minolta2430Button;
	QCheckBox *minolta2300Button;
	QCheckBox *minolta2200Button;
	QCheckBox *minoltaproButton;
	QCheckBox *samsung3160Button;
	QCheckBox *samsung600Button;
	QCheckBox *samsung300Button;
	QCheckBox *xerox6115Button;
	QCheckBox *xerox6110Button;
	QCheckBox *allButton;
	QButtonGroup *buttonGroup;
	QPushButton *downloadButton;
	QPushButton *exitButton;
	QStatusBar *statusBar;
	QProcess *callProgram;
	QString string;

private slots:
	void callGetweb();
	void setCheckBoxes(int);
	void showInfo();
	void checkProgram();
};

#endif // GUI_H
