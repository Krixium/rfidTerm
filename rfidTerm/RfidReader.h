#pragma once

#include <QObject>
#include <QMutex>
#include <QString>

#include <stapi\SkyeTekAPI.h>

class RfidReader
	: public QObject
{
	Q_OBJECT
public:
	RfidReader(QObject* parent = nullptr);
	~RfidReader();

	void ReadOnce();

private:
	bool mbHasReaders;

	unsigned int mNumDevices;
	unsigned int mNumReaders;

	LPSKYETEK_DEVICE* mDevices;
	LPSKYETEK_READER* mReaders;

	QString tcharToQString(const TCHAR* str) const;
	bool findReaders();

	void sendTagReadSignal(const LPSKYETEK_TAG lpTag);
	void sendIOMessageSignal(const QString status);
	void sendIOErrorSignal(const QString error);
	void sendIOErrorSignal(const SKYETEK_STATUS error);

signals:
	void TagReadSignal(QString data);
	void IOMessageSignal(QString message);
	void IOErrorSignal(QString error);

public slots:
	void connectToReaderSlot();
};
