#include "rfidTerm.h"

#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mReader(new RfidReader(this))
	, mThread(new IOThread(this, mReader))
{
	ui.setupUi(this);

	connect(mReader, &RfidReader::TagReadSignal, this, &rfidTerm::receiveData);
	connect(mReader, &RfidReader::IOMessageSignal, this, &rfidTerm::receiveMessage);
	connect(mReader, &RfidReader::IOErrorSignal, this, &rfidTerm::receiveError);

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