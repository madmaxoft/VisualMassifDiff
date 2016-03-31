// main.cpp

// Implements the main application entrypoint





#include "globals.h"
#include "mainwindow.h"
#include <QApplication>





int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	// Any cmd-line parameters are interpreted as filenames to open:
	for (int i = 1; i < argc; i++)
	{
		w.addSnapshotsFromFile(QString::fromUtf8(argv[i]));
	}

	return a.exec();
}




