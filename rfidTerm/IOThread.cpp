/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		IOThread.cpp - A QThread that discovers and interacts with a connected SkyeTek RFID reader. 

-- PROGRAM:			rfidTerm 
--
-- FUNCTIONS:
--					IOThread::IOThread (QObject *parent) 
--					IOThread::~IOThread()
--					void IOThread::run()
--					void IOThread::findReaders()
--					QString IOThread::tcharToQString(const TCHAR*) const
-- 
--					void IOThread::SendTagReadSignal(const LPSKYETEK_TAG lpTag) 
--					void IOThread::SendIOMessageSignal(const QString stats)
--					void IOThread::SendIOErrorSignal(const QString error)
--					void IOThread::SendIOErrorSignal(const SKYETEK_STATUS status)
-- 
--					unsigned char ReadTagCallback(LPSKYETEK_TAG lpTag, void* user)
-- 
-- QT Signals:		
--					void TagReadSignal(QString data)
--					void IOMessageSignal(QString message)
--					void IOErrorSignal(QString error) 
--
-- QT Slots:
--					void StopRunningSlot(bool isRunning)
--
-- DATE:			October 16, 2017 
--
-- REVISIONS:		N/A 
--
-- DESIGNER:		Benny Wang and Roger Zhang 
--
-- PROGRAMMER:		Benny Wang 
--
-- NOTES:
-- IOThread is responsible for two main things, discovering readers and interacting with discovered readerss. If no
-- readers are detected, the main thread goes into a loop until a reader is found. Once a reader is found, it is 
-- immediately sent a command to constantly interrogate all tags in range. Once a tag is interrogated for its data,
-- the read data is emited in a TagReadSignal().
----------------------------------------------------------------------------------------------------------------------*/
#include "IOThread.h"

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::IOThread
--
-- DATE:			October 16, 2017	
--
-- REVISIONS:		N/A	
--
-- DESIGNER:		Benny Wang and Roger Zhang	
--
-- PROGRAMMER:		Benny Wang	
--
-- INTERFACE:		IOThread::IOThread(QObject* parent)
--						QObject* parent: A pointer to this object's parent object.
--
-- RETURNS:			void.	
--
-- NOTES:
-- The constructor for IOThread. Sets all member variables to 0 or null or true.
----------------------------------------------------------------------------------------------------------------------*/
IOThread::IOThread(QObject* parent)
	: QThread(parent)
	, mbRun(true)
	, mNumOfDevices(0)
	, mNumOfReaders(0)
	, mDevices(NULL)
	, mReaders(NULL)
{
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::~IOThread
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A	
--
-- DESIGNER:		Benny Wang and Roger Zhang	
--
-- PROGRAMMER:		Benny Wang	
--
-- INTERFACE:		IOThread::~IOThread()
--
-- RETURNS:			void.	
--
-- NOTES:
-- The deconstructor for IOThread. Tells the thread to stop looping and frees up any memory that was used.
----------------------------------------------------------------------------------------------------------------------*/
IOThread::~IOThread()
{
	mbRun = false;
	wait();

	SkyeTek_FreeReaders(mReaders, mNumOfReaders);
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);

	delete mDevices;
	delete mReaders;
}


/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::run
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A	
--
-- DESIGNER:		Benny Wang and Roger Zhang	
--
-- PROGRAMMER:		Benny Wang	
--
-- INTERFACE:		void IOThread::run()
--
-- RETURNS:			void.	
--
-- NOTES:
-- TODO: WE PROBABLY HAVE TO REDESIGN THIS FUNCTION, OUTER LOOP DOESNT REALLY HELP
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::run()
{
	SKYETEK_STATUS st;
	while (mbRun)
	{
		while (mNumOfReaders == 0)
		{
			findReaders();
		}

		st = SkyeTek_SelectTags(mReaders[0], AUTO_DETECT, ReadTagCallback, 0, 1, this);
		if (st != SKYETEK_SUCCESS)
		{
			SendIOErrorSignal(st);
		}
	}
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::findReaders
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A	
--
-- DESIGNER:		Benny Wang and Roger Zhang	
--
-- PROGRAMMER:		Benny Wang	
--
-- INTERFACE:		void IOThread::findReaders()
--
-- RETURNS:			void.	
--
-- NOTES:
-- First this function tries to discover all devices connected to the system. If no devices are found the function
-- deallocates any memory it has used, emit an IOErrorSignal and returns false. If devices were found, the function
-- will try to find any SkyTek RFID readers in the list of devices. If no readers are found the function will again 
-- deallocate any memory it has used, emit an IOErrorSignal and return false. If a reader has been found function
-- emits a IOmessageSignal with information about the discovered reader and saves it to the IOThread::mReaders.
----------------------------------------------------------------------------------------------------------------------*/
bool IOThread::findReaders()
{
	mNumOfDevices = SkyeTek_DiscoverDevices(&mDevices);
	if (mNumOfDevices == 0)
	{
		SendIOErrorSignal("No Devices Were Found");
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		return false;
	}

	mNumOfReaders = SkyeTek_DiscoverReaders(mDevices, mNumOfDevices, &mReaders);
	if (mNumOfReaders == 0)
	{
		SendIOErrorSignal("No Readers Were Found");
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		SkyeTek_FreeReaders(mReaders, mNumOfReaders);
		return false;
	}
	
	SendIOMessageSignal(QString("Devices found %1, Readers found %2").arg(mNumOfDevices).arg(mNumOfReaders));
	return true;
}


QString IOThread::tcharToQString(const TCHAR* str) const
{
	QString tmp = QString("");
	for (int i = 0; str[i] != '\0'; i++)
	{
		tmp.append((LPSTR)(str + i));
	}
	return tmp;
}


void IOThread::SendTagReadSignal(const LPSKYETEK_TAG lpTag)
{
	emit TagReadSignal(tcharToQString(lpTag->friendly));
}


void IOThread::SendIOMessageSignal(const QString message)
{
	emit IOMessageSignal(message);
}


void IOThread::SendIOErrorSignal(const QString error)
{
	emit IOErrorSignal(QString("Error: %1").arg(error));
}


void IOThread::SendIOErrorSignal(const SKYETEK_STATUS status)
{
	TCHAR* tcharStatusMessage = SkyeTek_GetStatusMessage(status);
	QString statusMessage = tcharToQString(tcharStatusMessage);
	emit IOErrorSignal(QString("Error: %1").arg(statusMessage));
}


bool bStop = false;


unsigned char ReadTagCallback(LPSKYETEK_TAG lpTag, void* user)
{
	if (!bStop)
	{
		if (!lpTag != NULL)
		{
			((IOThread*)user)->SendTagReadSignal(lpTag);
			SkyeTek_FreeTag(lpTag);
		}
	}

	return !bStop;
}