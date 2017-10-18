#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(new IOThread(this))
	, mConsole(new Console(this))
{
	ui.setupUi(this);

	setCentralWidget(mConsole);

	connect(ui.actionConnect, &QAction::triggered, this, &rfidTerm::StartReading);
	connect(ui.actionDisconnect, &QAction::triggered, this, &rfidTerm::StopReading);

	connect(mThread, &IOThread::TagReadSignal, mConsole, &Console::PrintTag);
	connect(mThread, &IOThread::IOMessageSignal, mConsole, &Console::PrintMessage);
	connect(mThread, &IOThread::IOErrorSignal, mConsole, &Console::PrintError);

	connect(mThread, &IOThread::NoReaderFoundSignal, this, &rfidTerm::StopReading);

	mConsole->setEnabled(false);
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
}

void rfidTerm::setConsoleEnabled(const bool isEnabled)
{
	mConsole->setEnabled(isEnabled);
	ui.actionConnect->setEnabled(!isEnabled);
	ui.actionDisconnect->setEnabled(isEnabled);
}

void rfidTerm::StartReading()
{
	mThread->start();
	setConsoleEnabled(true);
}

void rfidTerm::StopReading()
{
	mThread->terminate();
	setConsoleEnabled(false);
}
