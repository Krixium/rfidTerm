#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_rfidTerm.h"

#include "IOThread.h"

class rfidTerm : public QMainWindow
{
	Q_OBJECT

public:
	rfidTerm(QWidget *parent = Q_NULLPTR);

private:
	Ui::rfidTermClass ui;

	IOThread* mThread;

private slots:
	void receiveData(QString data);
	void receiveMessage(QString message);
	void receiveError(QString error);
};
