#pragma once

#include <QMessageBox>
#include <QPlainTextEdit>

class Console 
	: public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit Console(QWidget *parent = 0);

protected:
	void keyPressEvent(QKeyEvent *e) Q_DECL_OVERRIDE;

signals:
	void StopReadingSignal();

public slots:
	void printTag(const QString data);
	void printMessage(const QString data);
	void printError(const QString data);
};
