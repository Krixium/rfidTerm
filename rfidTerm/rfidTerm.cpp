#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(new IOThread(this))
{
	ui.setupUi(this);
	console = new Console(this);
	setCentralWidget(console);

	connect(mThread, &IOThread::TagReadSignal, console, &Console::printTag);
	connect(mThread, &IOThread::IOMessageSignal, console, &Console::printMessage);
	connect(mThread, &IOThread::IOErrorSignal, console, &Console::printError);

	mThread->start();
}