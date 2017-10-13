#pragma once

#include <stdio.h>
#include <stapi\SkyeTekAPI.h>
#include <stapi\SkyeTekProtocol.h>

#include <QtWidgets/QMainWindow>
#include "ui_rfidTerm.h"

class rfidTerm : public QMainWindow
{
	Q_OBJECT

public:
	rfidTerm(QWidget *parent = Q_NULLPTR);

private:
	Ui::rfidTermClass ui;
};

