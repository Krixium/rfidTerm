#pragma once

#include <QThread>

#include <stapi/SkyeTekAPI.h>

class IOThread : public QThread
{
	Q_OBJECT

private:
	bool mbRunning;
	int mNumOfDevices;
	int mNumOfReaders;
	LPSKYETEK_DEVICE* mDevices;
	LPSKYETEK_READER* mReaders;

	QString tcharToQString(const TCHAR* str) const;

	bool findReaders();

public:
	IOThread(QObject* parent);
	~IOThread();

	void sendTagReadSignal(const LPSKYETEK_TAG lpTag);
	void sendIOMessageSignal(const QString status);
	void sendIOErrorSignal(const QString error);
	void sendIOErrorSignal(const SKYETEK_STATUS error);

protected:
	void run();

signals:
	void NoReaderFoundSignal();
	void TagReadSignal(QString data);
	void IOMessageSignal(QString message);
	void IOErrorSignal(QString error);
};