#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(NULL)
	, mConsole(new Console(this))
{
	ui.setupUi(this);

	setCentralWidget(mConsole);

	connect(ui.actionConnect, &QAction::triggered, this, &rfidTerm::StartReading);
	connect(ui.actionDisconnect, &QAction::triggered, this, &rfidTerm::StopReading);

	mConsole->setEnabled(false);
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
}

void rfidTerm::StartReading()
{
	mThread = new IOThread(this);
	mThread->start();

	connect(mThread, &IOThread::TagReadSignal, mConsole, &Console::printTag);
	connect(mThread, &IOThread::IOMessageSignal, mConsole, &Console::printMessage);
	connect(mThread, &IOThread::IOErrorSignal, mConsole, &Console::printError);

	mConsole->setEnabled(true);
	ui.actionConnect->setEnabled(false);
	ui.actionDisconnect->setEnabled(true);
}

void rfidTerm::StopReading()
{
	delete mThread;
	mConsole->setEnabled(false);
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
}