#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(NULL)
{
	ui.setupUi(this);
	console = new Console(this);
	setCentralWidget(console);

	connect(ui.actionConnect, &QAction::triggered, this, &rfidTerm::StartReading);
	connect(ui.actionDisconnect, &QAction::triggered, this, &rfidTerm::StopReading);


}

void rfidTerm::StartReading()
{
	mThread = new IOThread(this);
	mThread->start();

	connect(mThread, &IOThread::TagReadSignal, console, &Console::printTag);
	connect(mThread, &IOThread::IOMessageSignal, console, &Console::printMessage);
	connect(mThread, &IOThread::IOErrorSignal, console, &Console::printError);
}

void rfidTerm::StopReading()
{
	delete mThread;
}