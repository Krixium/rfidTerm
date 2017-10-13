#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_rfidTerm.h"

#include "RfidThread.h"

class rfidTerm : public QMainWindow
{
	Q_OBJECT

public:
	rfidTerm(QWidget *parent = Q_NULLPTR);

private:
	Ui::rfidTermClass ui;

	RfidThread* mRfidThread;
};

