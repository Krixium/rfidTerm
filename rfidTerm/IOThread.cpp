#include "IOThread.h"

bool bStop = false;

IOThread::IOThread(QObject *parent)
	: QThread(parent)
	, mbRun(true)
	, mNumOfDevices(0)
	, mNumOfReaders(0)
	, mDevices(NULL)
	, mReaders(NULL)
{
}

IOThread::~IOThread()
{
	mbRun = false;
	wait();

	SkyeTek_FreeReaders(mReaders, mNumOfReaders);
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);

	delete mDevices;
	delete mReaders;
}

bool IOThread::findReaders()
{
	mNumOfDevices = SkyeTek_DiscoverDevices(&mDevices);
	if (mNumOfDevices == 0)
	{
		sendIOErrorSignal("No Devices Were Found");
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		return false;
	}

	mNumOfReaders = SkyeTek_DiscoverReaders(mDevices, mNumOfDevices, &mReaders);
	if (mNumOfReaders == 0)
	{
		sendIOErrorSignal("No Readers Were Found");
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		SkyeTek_FreeReaders(mReaders, mNumOfReaders);
		return false;
	}
	
	sendIOMessageSignal(QString("Devices found %1, Readers found %2").arg(mNumOfDevices).arg(mNumOfReaders));
	return true;
}

QString IOThread::tcharToQString(const TCHAR* str) const
{
	QString tmp = QString("");
	for (int i = 0; str[i] != '\0'; i++)
	{
		tmp.append(str[i]);
	}
	return tmp;
}

void IOThread::sendIOMessageSignal(const QString message)
{
	emit IOMessageSignal(message);
}

void IOThread::sendIOErrorSignal(const SKYETEK_STATUS status)
{
	TCHAR* tcharStatusMessage = SkyeTek_GetStatusMessage(status);
	QString statusMessage = tcharToQString(tcharStatusMessage);
	emit IOErrorSignal(QString("Error: %1").arg(statusMessage));
}

void IOThread::sendIOErrorSignal(const QString error)
{
	emit IOErrorSignal(QString("Error: %1").arg(error));
}

void IOThread::sendTagReadSignal(const LPSKYETEK_TAG lpTag)
{
	emit TagReadSignal(tcharToQString(lpTag->friendly));
}

void IOThread::run()
{
	while (mbRun)
	{
		while (mNumOfReaders == 0)
		{
			findReaders();
		}

		SkyeTek_SelectTags(mReaders[0], AUTO_DETECT, ReadTagCallback, 0, 1, this);
	}
}

unsigned char ReadTagCallback(LPSKYETEK_TAG lpTag, void* user)
{
	if (!bStop)
	{
		if (!lpTag != NULL)
		{
			((IOThread*)user)->sendTagReadSignal(lpTag);
			SkyeTek_FreeTag(lpTag);
		}
	}

	return !bStop;
}