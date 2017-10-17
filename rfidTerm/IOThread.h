#pragma once

#include <QThread>

#include <stapi/SkyeTekAPI.h>

class IOThread : public QThread
{
	Q_OBJECT

private:
	bool mbRun;

	int mNumOfDevices;
	int mNumOfReaders;
	LPSKYETEK_DEVICE* mDevices;
	LPSKYETEK_READER* mReaders;

	QString tcharToQString(const TCHAR* str) const;

	bool findReaders();

public:
	IOThread(QObject* parent);
	~IOThread();

	void SendTagReadSignal(const LPSKYETEK_TAG lpTag);
	void SendIOMessageSignal(const QString status);
	void SendIOErrorSignal(const QString error);
	void SendIOErrorSignal(const SKYETEK_STATUS error);

protected:
	void run();

signals:
	void TagReadSignal(QString data);
	void IOMessageSignal(QString message);
	void IOErrorSignal(QString error);
};

unsigned char ReadTagCallback(LPSKYETEK_TAG lpTag, void* user);