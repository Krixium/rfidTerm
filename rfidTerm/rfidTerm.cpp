#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(new IOThread(this))
{
	ui.setupUi(this);

	connect(mThread, &IOThread::TagReadSignal, this, &rfidTerm::receiveData);
	connect(mThread, &IOThread::IOMessageSignal, this, &rfidTerm::receiveMessage);
	connect(mThread, &IOThread::IOErrorSignal, this, &rfidTerm::receiveError);

	mThread->start();
}

void rfidTerm::receiveData(QString data)
{
	qDebug() << data;
}

void rfidTerm::receiveMessage(QString msg)
{
	qDebug() << msg;
}

void rfidTerm::receiveError(QString error)
{
	qDebug() << error;
}