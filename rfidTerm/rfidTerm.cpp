#include "rfidTerm.h"

#include <string>
#include <QDebug>

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mRfidThread(new RfidThread(this))
{
	ui.setupUi(this);

	mRfidThread->start();
}

