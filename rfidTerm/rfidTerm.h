#pragma once

#include <QAction>
#include <QtWidgets/QMainWindow>
#include "ui_rfidTerm.h"

#include "console.h"
#include "IOThread.h"

class rfidTerm : public QMainWindow
{
	Q_OBJECT
public:
	rfidTerm(QWidget *parent = Q_NULLPTR);


private:
	Ui::rfidTermClass ui;
	Console* mConsole;
	IOThread* mThread;

	inline void setConsoleEnabled(const bool isEnabled = false);

public slots:
	void StartReading();
	void StopReading();
};
