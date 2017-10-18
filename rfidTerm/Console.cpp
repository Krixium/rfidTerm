#include "console.h"

Console::Console(QWidget* parent)
	: QPlainTextEdit(parent)
{
	isReadOnly();
}

void Console::PrintTag(const QString data)
{
	appendPlainText(data);
}

void Console::PrintMessage(const QString data)
{
	appendPlainText(data);
}

void Console::PrintError(const QString data)
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
