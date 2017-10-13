#include "RfidThread.h"

#include <QDebug>

RfidThread::RfidThread(QObject* parent)
	: QThread(parent)
	, mbStop(false)
	, mDevices(NULL)
	, mReaders(NULL)
	, mNumOfDevices(NULL)
	, mNumOfReaders(NULL)
{
}

RfidThread::~RfidThread()
{
	qDebug() << "Freeing Devices";
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);
	SkyeTek_FreeReaders(mReaders, mNumOfReaders);

	delete mDevices;
	delete mReaders;

	mbStop = true;
	wait();
}

bool RfidThread::getReaders()
{
	mNumOfDevices = SkyeTek_DiscoverDevices(&mDevices);

	if (mNumOfDevices == 0)
	{
		qDebug() << "No Devices";
		return false;
	}
	else
	{
		qDebug() << "Num of Devices: " << mNumOfDevices;
		for (int i = 0; i < mNumOfDevices; i++)
		{
			qDebug() << "Device " << i << ": " << mDevices[0]->friendly;
		}
	}

	mNumOfReaders = SkyeTek_DiscoverReaders(mDevices, mNumOfDevices, &mReaders);

	if (mNumOfReaders == 0)
	{
		qDebug() << "Could not find readers";
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		return false;
	}
	else
	{
		qDebug() << "Number of Readers: " << mNumOfReaders;
	}

	return (mNumOfReaders > 0);
}

void RfidThread::run()
{
	mbStop = !getReaders();

	while (!mbStop)
	{
		LPSKYETEK_TAG* lpTags = NULL;
		unsigned short count = 0;
		st = SkyeTek_GetTags(mReaders[0], AUTO_DETECT, &lpTags, &count);

		if (st == SKYETEK_TIMEOUT)
		{
			qDebug() << "Timed out";
			continue;
		}

		if (st != SKYETEK_SUCCESS)
		{
			qDebug() << "Reading tags failed";
			continue;
		}

		qDebug() << "SKYETEK_SUCCESS, count:" << count;
		for (int x = 0; x < count; x++)
		{
			st = ReadTagData(mReaders[0], lpTags[x]);
			if (st == SKYETEK_SUCCESS)
			{
				mbStop = true;
			}
		}

		SkyeTek_FreeTags(mReaders[0], lpTags, count);

		qDebug() << "Finished a loop";
		sleep(1);
	}
}

unsigned char RfidThread::SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
{
	SKYETEK_MEMORY* mem;

	if (mbStop)
	{
		if (lpTag != NULL)
		{
			qDebug() << "Tag: ";
			QString* tmp = new QString();
			for (int i = 0; i < sizeof(lpTag->friendly); i++)
			{
				tmp->append(new QString((LPSTR)lpTag->friendly + 1));
			}
			qDebug() << tmp;

			SkyeTek_FreeTag(lpTag);
		}
	}
	return !mbStop;
}

QString convertQStr(TCHAR* str)
{
	QString temp = QString("");

	for (int i = 0; str[i] != '\0'; i++)
	{
		temp += (LPSTR)str[i];
	}

	return temp;
}

SKYETEK_STATUS RfidThread::ReadTagData(LPSKYETEK_READER lpReader, LPSKYETEK_TAG lpTag)
{
	SKYETEK_STATUS st;
	LPSKYETEK_DATA lpData = NULL;
	SKYETEK_ADDRESS addr;
	SKYETEK_MEMORY mem;
	unsigned long length;
	unsigned char data[2048];
	bool didFail = false;
	unsigned char stat = 0;


	// Get memory info
	memset(&mem, 0, sizeof(SKYETEK_MEMORY));
	lpTag = NULL;
	st = SkyeTek_GetTagInfo(lpReader, lpTag, &mem);
	if (st != SKYETEK_SUCCESS)
	{
		if (lpTag == NULL)
		{
			qDebug() << "Tag was null";
		}

		if (lpTag != NULL)
		{
			qDebug() << "Error getting tag info for " << convertQStr(lpTag->friendly);
		}
		return st;
	}

	// Allocate the memory now that we know how much it has
	memset(data, 0, 2048);
	length = (mem.maxBlock - mem.startBlock + 1) * mem.bytesPerBlock;

	if (length > 2048)
	{
		qDebug() << "Error tag length too big";
		return SKYETEK_FAILURE;
	}

	qDebug() << QString("Number of bytes per block: ") << mem.bytesPerBlock;
	qDebug() << QString("Number of blocks: ") << ((mem.maxBlock - mem.startBlock) + 1);
	qDebug() << QString("Total number of bytes: ") << length;


	// Initialize address based on tag type
	memset(&addr, 0, sizeof(SKYETEK_ADDRESS));
	addr.start = mem.startBlock;
	addr.blocks = 1;

	// Now we loop but lock and release each time so as not to starve the rest of the app
	unsigned char *ptr = data;
	for (; addr.start <= mem.maxBlock; addr.start++)
	{

		// Read data
		st = SkyeTek_ReadTagData(lpReader, lpTag, &addr, 0, 0, &lpData);
		if (st != SKYETEK_SUCCESS)
		{
			didFail = true;
			ptr += mem.bytesPerBlock;
			qDebug() << QString("Read failed on address: ") << addr.start << " Reason for fail: " << SkyeTek_GetStatusMessage(st);
			continue;
		}

		// Get lock status
		stat = 0;
		st = SkyeTek_GetLockStatus(lpReader, lpTag, &addr, &stat);
		if (st != SKYETEK_SUCCESS)
		{
			didFail = true;
			ptr += mem.bytesPerBlock;
			qDebug() << QString("Read failed on address: ") << addr.start << " Reason for fail: " << SkyeTek_GetStatusMessage(st);
			continue;
		}

		// Copy over data to buffer
		qDebug() << QString("Address: ") << addr.start << "Lock Pass: " << (stat ? "Locked" : "Unlocked");
		memcpy(ptr, lpData->data, lpData->size);
		ptr += mem.bytesPerBlock;
		SkyeTek_FreeData(lpData);
		lpData = NULL;

	} // End loop

	// Report data
	lpData = SkyeTek_AllocateData((int)length);
	SkyeTek_CopyBuffer(lpData, data, length);
	TCHAR *str = SkyeTek_GetStringFromData(lpData);

	qDebug() << "Data for :" << lpTag->friendly << " - " << str;

	SkyeTek_FreeString(str);
	SkyeTek_FreeData(lpData);

	// Return
	if (didFail)
	{
		return SKYETEK_FAILURE;
	}
	else
	{
		return SKYETEK_SUCCESS;
	}
}

