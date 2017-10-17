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
--					void StartReadingSlot()
--					void StopReadingSlot()
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
	, mbRunning(true)
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
	mbRunning = false;
	wait();

	SkyeTek_FreeReaders(mReaders, mNumOfReaders);
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);
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
--
-- TODO: WE PROBABLY HAVE TO REDESIGN THIS FUNCTION, OUTER LOOP DOESNT REALLY HELP
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::run()
{
	SKYETEK_STATUS st;
	LPSKYETEK_TAG* lpTags;
	unsigned short tagCount;

	while (mbRunning)
	{
		if (mNumOfReaders == 0)
		{
			findReaders();
		}
		else
		{
			st = SkyeTek_GetTags(mReaders[0], AUTO_DETECT, &lpTags, &tagCount);
			if (st != SKYETEK_SUCCESS)
			{
				sendIOErrorSignal(st);
			}
			else
			{
				for (unsigned short i = 0; i < tagCount; i++)
				{
					sendTagReadSignal(*(lpTags + i));
					SkyeTek_FreeTag(*(lpTags + i));
				}
			}
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
--
-- TODO: FIX MEMORY LEAK
----------------------------------------------------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::tcharToQString
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::tcharToQString(const TCHAR* str)
--						const TCHAR* str: A null terminated TCHAR array that will be converted to a QString
--
-- RETURNS:			A QString that has the same ascii representation as the null terminated TCHAR array that was
--					passed in.
--
-- NOTES:
-- Converts a null terminated TCHAR array used by the SkyeTek api to a QString object is used by the Qt framework.
----------------------------------------------------------------------------------------------------------------------*/
QString IOThread::tcharToQString(const TCHAR* str) const
{
	QString tmp = QString("Tag Found");
	for (int i = 0; str[i] != '\0'; i++)
	{
		tmp.append((LPSTR)(str + i));
	}
	return tmp;
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::SendTagReadSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::SendTagReadSignal(const LPSKYETEK_TAG lpTag)	
--						const LPSKYETEK_TAG lpTag: A pointer to a tag that will be parsed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Takes the tag that was passed in, converts the friendly variable from a null terminated TCHAR array to a QString
-- and emits the QString in a TAGReadSignal().
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::sendTagReadSignal(const LPSKYETEK_TAG lpTag)
{
	emit TagReadSignal(tcharToQString(lpTag->friendly));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::SendIOMessageSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::SendIOMessageSignal(const QString message)	
--						const QString message: A message that should be displayed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Prepends "IO Message: " to the function parameter message and emits the new QString in an IOMEssageSignal().
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::sendIOMessageSignal(const QString message)
{
	emit IOMessageSignal(QString("IO Message: %1").arg(message));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::SendIOErrorSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::SendIOErrorSignal(const QString error)	
--						const QString error: An error message that should be displayed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Prepends "IO Error: " to the function parameter error and emits the new QString in an IOErrorSignal().
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::sendIOErrorSignal(const QString error)
{
	emit IOErrorSignal(QString("IO Error: %1").arg(error));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::SendIOErrorSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::SendIOErrorSignal(const SKYETEK_STATUS status)	
--						const SKYETEK_STATUS status: The status of an operation.
--
-- RETURNS:			void.
--
-- NOTES:
-- Retrives the status message that corrisponds to to the function parameter status, converts it to a QString, prepends
-- "IO Error: ", and emits the new QString in an IOErrorSignal(). 
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::sendIOErrorSignal(const SKYETEK_STATUS status)
{
	TCHAR* tcharStatusMessage = SkyeTek_GetStatusMessage(status);
	QString statusMessage = tcharToQString(tcharStatusMessage);
	emit IOErrorSignal(QString("IO Error: %1").arg(statusMessage));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::StartReadingSlot()
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::StartReadingSlot()	
--
-- RETURNS:			void.
--
-- NOTES:
-- A Qt Slot that starts the thread when it receives a signal.
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::StartReadingSlot()
{
	if (findReaders())
	{
		mbRunning = true;
		start();
	}
	else
	{
		mbRunning = false;
	}
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		IOThread::StopReadingSlot()
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void IOThread::StopReadingSlot()	
--
-- RETURNS:			void.
--
-- NOTES:
-- A Qt Slot that stops the thread when it receives a signal.
----------------------------------------------------------------------------------------------------------------------*/
void IOThread::StopReadingSlot()
{
	mbRunning = false;
	wait();
}