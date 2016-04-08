// Main.cpp

// Implements the main application entrypoint





#include "Globals.h"
#include <QApplication>
#include "MainWindow.h"





int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.showMaximized();

	// Any cmd-line parameters are interpreted as filenames to open:
	for (int i = 1; i < argc; i++)
	{
		w.addSnapshotsFromFile(QString::fromUtf8(argv[i]));
	}

	return a.exec();
}




