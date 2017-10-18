#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_rfidTerm.h"

#include "console.h"
#include "IOThread.h"

class Console;
class rfidTerm : public QMainWindow
{
	Q_OBJECT

public:
	rfidTerm(QWidget *parent = Q_NULLPTR);

private:
	Ui::rfidTermClass ui;
	Console *console;
	IOThread* mThread;

};
