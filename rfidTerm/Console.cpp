#include "console.h"
#include <qmessagebox.h>
#include <QtCore/QDebug>

Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
{
}

void Console::printTag(const QString data)
{
	appendPlainText(data);
}

void Console::printMessage(const QString data)
{
	appendPlainText(data);
}

void Console::printError(const QString data)
{
	QMessageBox::information(this, tr("Error"), data);
}

void Console::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()) {
	case Qt::Key_Escape:
		emit StopReadingSignal();
		break;
	default:
		break;
	}
}
