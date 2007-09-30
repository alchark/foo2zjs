//      Hannah, GUI for downloading specific printer firmware
//      Copyright (C) 2007 Steffen Joeris <white@debian.org>
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


#include "gui.h"

MainWindow::MainWindow(QWidget *parent)
        :QWidget(parent)
{
	int check = QMessageBox::question(this, tr("Hannah - Firmware downloader"),
			tr("Do you want to download and install potential non-free printer firmware?"), tr("&Yes"), tr("&No"),
			QString::null, 0, 1);

	
	if (! check == 0 ) {
		exit(0);
	}

	createMainWidget();
	
	connect(downloadButton, SIGNAL(clicked()), this, SLOT(callGetweb()));
};

void MainWindow::createMainWidget()
{
        setWindowTitle(tr("Hannah - Foo2zjs firmware downloader"));
        QVBoxLayout *mainLayout = new QVBoxLayout;
        hp2600Button = new QCheckBox(tr("Download firmware for HP2600"));
        hp1600Button = new QCheckBox(tr("Download firmware for HP1600"));
        hp1020Button = new QCheckBox(tr("Download firmware for HP1020"));
        hp1018Button = new QCheckBox(tr("Download firmware for HP1018"));
        hp1005Button = new QCheckBox(tr("Download firmware for HP1005"));
        hp1000Button = new QCheckBox(tr("Download firmware for HP1000"));
        minolta2530Button = new QCheckBox(tr("Download firmware for Minolta 2530"));
        minolta2490Button = new QCheckBox(tr("Download firmware for Minolta 2490"));
        minolta2480Button = new QCheckBox(tr("Download firmware for Minolta 2480"));
        minolta2430Button = new QCheckBox(tr("Download firmware for Minolta 2430"));
        minolta2300Button = new QCheckBox(tr("Download firmware for Minolta 2300"));
        minolta2200Button = new QCheckBox(tr("Download firmware for Minolta 2200"));
        minoltaproButton = new QCheckBox(tr("Download firmware for Minolta PageWorks/Pro L"));
        samsung3160Button = new QCheckBox(tr("Download firmware for Samsung CLX-3160"));
        samsung600Button = new QCheckBox(tr("Download firmware for Samsung CLP-600"));
        samsung300Button = new QCheckBox(tr("Download firmware for Samsung CLP-300"));
        xerox6115Button = new QCheckBox(tr("Download firmware for Xerox Phaser 6115"));
        xerox6110Button = new QCheckBox(tr("Download firmware for Xerox Phaser 6110"));
        allButton = new QCheckBox(tr("Mark all for download"));
        downloadButton = new QPushButton(tr("Download marked firmwares"));
        exitButton = new QPushButton(tr("Exit"));

        hp2600Button->setFont(QFont("Times", 18));
        hp1600Button->setFont(QFont("Times", 18));
        hp1020Button->setFont(QFont("Times", 18));
        hp1018Button->setFont(QFont("Times", 18));
        hp1005Button->setFont(QFont("Times", 18));
        hp1000Button->setFont(QFont("Times", 18));
        minolta2530Button->setFont(QFont("Times", 18));
        minolta2490Button->setFont(QFont("Times", 18));
        minolta2480Button->setFont(QFont("Times", 18));
        minolta2430Button->setFont(QFont("Times", 18));
        minolta2300Button->setFont(QFont("Times", 18));
        minolta2200Button->setFont(QFont("Times", 18));
        minoltaproButton->setFont(QFont("Times", 18));
        samsung3160Button->setFont(QFont("Times", 18));
        samsung600Button->setFont(QFont("Times", 18));
        samsung300Button->setFont(QFont("Times", 18));
        xerox6115Button->setFont(QFont("Times", 18));
        xerox6110Button->setFont(QFont("Times", 18));
        allButton->setFont(QFont("Times", 18));
        downloadButton->setFont(QFont("Times", 18));
        exitButton->setFont(QFont("Times", 18));

        QMenuBar *menubar = new QMenuBar;
        QMenu *menu = new QMenu;
        menubar->addMenu(menu);
        QAction *about = new QAction(tr("About Hannah"), this);
        menu = menubar->addMenu(tr("Help"));
        menu->addAction(about);

	statusBar = new QStatusBar;

	mainLayout->addWidget(menubar, 0, 0);
        mainLayout->addWidget(hp2600Button, 1, 0);
        mainLayout->addWidget(hp1600Button, 2, 0);
        mainLayout->addWidget(hp1020Button, 3, 0);
        mainLayout->addWidget(hp1018Button, 4, 0);
        mainLayout->addWidget(hp1005Button, 5, 0);
        mainLayout->addWidget(hp1000Button, 6, 0);
        mainLayout->addWidget(minolta2530Button, 7, 0);
        mainLayout->addWidget(minolta2490Button, 8, 0);
        mainLayout->addWidget(minolta2480Button, 9, 0);
        mainLayout->addWidget(minolta2430Button, 10, 0);
        mainLayout->addWidget(minolta2300Button, 11, 0);
        mainLayout->addWidget(minolta2200Button, 12, 0);
        mainLayout->addWidget(minoltaproButton, 13, 0);
        mainLayout->addWidget(samsung3160Button, 14, 0);
        mainLayout->addWidget(samsung600Button, 15, 0);
        mainLayout->addWidget(samsung300Button, 16, 0);
        mainLayout->addWidget(xerox6115Button, 17, 0);
        mainLayout->addWidget(xerox6110Button, 18, 0);
        mainLayout->addWidget(allButton, 19, 0);
        mainLayout->addWidget(statusBar, 20, 0);
        mainLayout->addWidget(downloadButton, 21, 0);
        mainLayout->addWidget(exitButton, 22, 0);
        setMinimumSize(600, 700);
        setLayout(mainLayout);

        buttonGroup = new QButtonGroup;
        buttonGroup->setExclusive(false);
        buttonGroup->addButton(hp2600Button, 0);
        buttonGroup->addButton(hp1600Button, 1);
        buttonGroup->addButton(hp1020Button, 2);
        buttonGroup->addButton(hp1018Button, 3);
        buttonGroup->addButton(hp1005Button, 4);
        buttonGroup->addButton(hp1000Button, 5);
        buttonGroup->addButton(minolta2530Button, 6);
        buttonGroup->addButton(minolta2490Button, 7);
        buttonGroup->addButton(minolta2480Button, 8);
        buttonGroup->addButton(minolta2430Button, 9);
        buttonGroup->addButton(minolta2300Button, 10);
        buttonGroup->addButton(minolta2200Button, 11);
        buttonGroup->addButton(minoltaproButton, 12);
        buttonGroup->addButton(samsung3160Button, 13);
        buttonGroup->addButton(samsung600Button, 14);
        buttonGroup->addButton(samsung300Button, 15);
        buttonGroup->addButton(xerox6115Button, 16);
        buttonGroup->addButton(xerox6110Button, 17);

	connect(about, SIGNAL(triggered()), this, SLOT(showInfo()));
	connect(allButton, SIGNAL(stateChanged(int)), this, SLOT(setCheckBoxes(int)));
	connect(exitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
};

void MainWindow::showInfo()
{
	QMessageBox::information(this, tr("Hannah-Foo2zjs Firmware Downloader"),
			tr("This program is developed by the Debian Foo2ZJS maintainers.\n"
			   "It is offered as a convenient method to download the needed\n"
			   "firmware for one of the printers supported by Foo2ZJS. Please feel\n"
			   "free to report any bugs against the foo2zjs package\n"
			   "to the Debian BTS (http://bugs.debian.org).\n"
			   "The program's name is Hannah:\n" 
			   "(grapHical interfAce for dowNloading your priNter's firmwAre at Home).\n"),
			   QMessageBox::Ok);
};

void MainWindow::setCheckBoxes(int state)
{
	switch (state) {
		case 2:
			hp2600Button->setCheckState(Qt::Checked);
			hp1600Button->setCheckState(Qt::Checked);
			hp1020Button->setCheckState(Qt::Checked);
			hp1018Button->setCheckState(Qt::Checked);
			hp1005Button->setCheckState(Qt::Checked);
			hp1000Button->setCheckState(Qt::Checked);
			minolta2530Button->setCheckState(Qt::Checked);
			minolta2490Button->setCheckState(Qt::Checked);
			minolta2480Button->setCheckState(Qt::Checked);
			minolta2430Button->setCheckState(Qt::Checked);
			minolta2300Button->setCheckState(Qt::Checked);
			minolta2200Button->setCheckState(Qt::Checked);
			minoltaproButton->setCheckState(Qt::Checked);
			samsung3160Button->setCheckState(Qt::Checked);
			samsung600Button->setCheckState(Qt::Checked);
			samsung300Button->setCheckState(Qt::Checked);
			xerox6115Button->setCheckState(Qt::Checked);
			xerox6110Button->setCheckState(Qt::Checked);
			break;
		case 0:
			hp2600Button->setCheckState(Qt::Unchecked);
			hp1600Button->setCheckState(Qt::Unchecked);
			hp1020Button->setCheckState(Qt::Unchecked);
			hp1018Button->setCheckState(Qt::Unchecked);
			hp1005Button->setCheckState(Qt::Unchecked);
			hp1000Button->setCheckState(Qt::Unchecked);
			minolta2530Button->setCheckState(Qt::Unchecked);
			minolta2490Button->setCheckState(Qt::Unchecked);
			minolta2480Button->setCheckState(Qt::Unchecked);
			minolta2430Button->setCheckState(Qt::Unchecked);
			minolta2300Button->setCheckState(Qt::Unchecked);
			minolta2200Button->setCheckState(Qt::Unchecked);
			minoltaproButton->setCheckState(Qt::Unchecked);
			samsung3160Button->setCheckState(Qt::Unchecked);
			samsung600Button->setCheckState(Qt::Unchecked);
			samsung300Button->setCheckState(Qt::Unchecked);
			xerox6115Button->setCheckState(Qt::Unchecked);
			xerox6110Button->setCheckState(Qt::Unchecked);
			break;
		default:
			break;
		}
}

void MainWindow::callGetweb()
{
	QAbstractButton *temporary_button;
	int count = 0;
	QStringList current_list;
	current_list << "2600n" << "1600" << "1020" << "1018" << "1005" << "1000" << "2530" << "2490" << "2480" << "2430" << "2300" << "2200" << "cpwl" << "3160" << "600" << "300" << "6115" << "6110";
	QStringList arguments;

	foreach( temporary_button, buttonGroup->buttons() ) {
		if ( temporary_button->isChecked() ) {
			arguments << current_list.at(count);
		}
		count++;
	}
	
	QString program = "/usr/bin/getweb";
	string = arguments.join(" ");
	callProgram = new QProcess;

	switch (callProgram->state()) {
		case 2:
			QMessageBox::warning(this, tr("Hannah - Firmware downloader 'getweb'"),
				tr("There is already a firmware download in progress. \n"
				    "Refuse to start another one."), QMessageBox::Ok);
			break;
		case 1:
			QMessageBox::warning(this, tr("Hannah - Firmware downloader 'getweb'"),
				tr("There is already a firmware download in progress. \n"
				    "Refuse to start another one."), QMessageBox::Ok);
			break;
		case 0:
			callProgram->start(program, arguments);
			QString message = QString(tr("Downloading firmware for %1").arg(string));
			statusBar->showMessage(message);
			connect(callProgram, SIGNAL(finished(int)), this, SLOT(checkProgram()));
			break;
	}
};

void MainWindow::checkProgram()
{
 	
	switch ( callProgram->exitCode() ) {
		case 0:
			QMessageBox::information(this, tr("Hannah - Firmware downloader"),
				tr("The firmware for the following printers was downloaded and installed successfully:\n"
					"%1\n").arg(string), QMessageBox::Ok);
			statusBar->showMessage(tr("Download and installation finished."));
			break;
		default:
			QMessageBox::critical(this, tr("Hannah - Firmware downloader"),
				tr("Something went wrong during the download and installation process for the following printers:\n"
					"%1\n").arg(string), QMessageBox::Ok);
			statusBar->showMessage(tr("An error occured during download and installation process."));
			break;
	}

};
