#include "RfidThread.h"

#include <QDebug>

bool bStop = false;

QString printData(const LPSKYETEK_DATA data)
{
	QString temp = QString("");

	for (int i = 0; i < data->size; i++)
	{
		temp.append((char)(data->data + i));
	}

	return temp;
}

QString tcharToQString(const TCHAR* str)
{
	QString temp = QString("");

	for (int i = 0; str[i] != '\0'; i++)
	{
		temp.append((LPSTR)(str + i));
	}

	return temp;
}

unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
{
	LPSKYETEK_READER* reader = (LPSKYETEK_READER*)user;
	SKYETEK_STATUS st;
	LPSKYETEK_DATA lpData = SkyeTek_AllocateData(1);
	SKYETEK_ADDRESS addr;
	SKYETEK_MEMORY mem;
	const int SIZE_OF_DATA = 2048;
	unsigned char data[SIZE_OF_DATA];

	/*
	mem->startBlock = 0;
	mem->maxBlock = 0;
	mem->bytesPerBlock = 0;
	mem->lpOtherInfo = SkyeTek_AllocateData(1);
	*/

	if (!bStop)
	{
		if (lpTag != NULL)
		{
			qDebug() << "Tag:" << tcharToQString(lpTag->friendly);

			//// Get memory info
			//memset(&mem, 0, sizeof(SKYETEK_MEMORY));
			//mem.lpOtherInfo = SkyeTek_AllocateData(SIZE_OF_DATA);
			//st = SkyeTek_GetTagInfo(reader[0], lpTag, &mem);
			//if (st != SKYETEK_SUCCESS)
			//{
			//	qDebug() << "Getting tag info failed";
			//	qDebug() << tcharToQString(SkyeTek_GetStatusMessage(st));
			//	return bStop;
			//}

			//// Allocate memory
			//memset(data, 0, SIZE_OF_DATA);
			//unsigned long length = (mem.maxBlock - mem.startBlock + 1) * mem.bytesPerBlock;
			//if (length > SIZE_OF_DATA)
			//{
			//	qDebug() << "Data was larger than buffer";
			//	return bStop;
			//}

			//// Read memory
			//memset(&addr, 0, sizeof(SKYETEK_ADDRESS));
			//addr.start = mem.startBlock;
			//addr.blocks = (mem.maxBlock - mem.startBlock + 1);

			//addr.start = 10;
			//addr.blocks = 1;

			//st = SkyeTek_ReadTagData(reader[0], lpTag, &addr, 0, 0, &lpData);
			//if (st != SKYETEK_SUCCESS)
			//{
			//	qDebug() << "Error Reading memory";
			//	return bStop;
			//}

			//qDebug() << printData(lpData);

			//SkyeTek_FreeData(lpData);
			SkyeTek_FreeTag(lpTag);
		}
	}
	return !bStop;
}

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
	mbStop = true;
	wait();

	qDebug() << "Freeing Devices";
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);
	SkyeTek_FreeReaders(mReaders, mNumOfReaders);

	delete mDevices;
	delete mReaders;
}

bool RfidThread::getReaders()
{
	mNumOfDevices = SkyeTek_DiscoverDevices(&mDevices);

	if (mNumOfDevices == 0)
	{
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		delete mDevices;
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
		SkyeTek_FreeReaders(mReaders, mNumOfReaders);
		delete mDevices;
		delete mReaders;
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
	bool foundReader = false;

	while (!foundReader)
	{
		foundReader = getReaders();
	}

	st = SkyeTek_SelectTags(mReaders[0], AUTO_DETECT, SelectLoopCallback, 0, 1, mReaders);

	if (st != SKYETEK_SUCCESS)
	{
		qDebug() << "Loop ended due to failure";
	}

	qDebug() << "Reading finished";
}




