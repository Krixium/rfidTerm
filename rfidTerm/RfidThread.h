#pragma once

#include <QThread>
#include <QString>

#include <stdio.h>
#include <stapi\SkyeTekAPI.h>
#include <stapi\SkyeTekProtocol.h>

#include <stapi\SkyeTekAPI.h>
#include <stapi\SkyeTekProtocol.h>

#include <QThread>
#include <QString>

class RfidThread
	: public QThread
{
	Q_OBJECT
public:
	RfidThread(QObject* parent = nullptr);
	~RfidThread();

	

protected:
	void run();

private:
	bool mbStop;
	LPSKYETEK_DEVICE* mDevices;
	LPSKYETEK_READER* mReaders;
	SKYETEK_STATUS st;
	unsigned int mNumOfDevices;
	unsigned int mNumOfReaders;

	bool getReaders();

	unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user);
	SKYETEK_STATUS ReadTagData(LPSKYETEK_READER lpReader, LPSKYETEK_TAG lpTag);
};

