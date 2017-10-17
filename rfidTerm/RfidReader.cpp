/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		RfidReader.cpp - A class that acts as a layer to seperate hardware level from the user level.

-- PROGRAM:			rfidTerm 
--
-- FUNCTIONS:
--					RfidReader::RfidReader() 
--					RfidReader::~RfidReader()
--					bool RfidReader::findReaders()
--					void RfidReader::readOnce()
--
--					QString RfidReader::tcharToQString(const TCHAR*) const
--					void RfidReader::SendTagReadSignal(const LPSKYETEK_TAG lpTag) 
--					void RfidReader::SendIOMessageSignal(const QString stats)
--					void RfidReader::SendIOErrorSignal(const QString error)
--					void RfidReader::SendIOErrorSignal(const SKYETEK_STATUS status)
-- 
-- Qt Signals:		
--					void TagReadSignal(QString data)
--					void IOMessageSignal(QString message)
--					void IOErrorSignal(QString error)
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
-- This class creates an interface to abstract the hardware api calls.
----------------------------------------------------------------------------------------------------------------------*/
#include "RfidReader.h"

RfidReader::RfidReader(QObject* parent)
	: QObject(parent)
{
	mbHasReaders = findReaders();
}

RfidReader::~RfidReader()
{
	SkyeTek_FreeReaders(mReaders, mNumDevices);
	SkyeTek_FreeDevices(mDevices, mNumReaders);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidReader::findReaders
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A	
--
-- DESIGNER:		Benny Wang and Roger Zhang	
--
-- PROGRAMMER:		Benny Wang	
--
-- INTERFACE:		void RfidReader::findReaders()
--
-- RETURNS:			void.	
--
-- NOTES:
-- First this function tries to discover all devices connected to the system. If no devices are found the function
-- deallocates any memory it has used, emit an IOErrorSignal and returns false. If devices were found, the function
-- will try to find any SkyTek RFID readers in the list of devices. If no readers are found the function will again 
-- deallocate any memory it has used, emit an IOErrorSignal and return false. If a reader has been found function
-- emits a IOmessageSignal with information about the discovered reader and saves it to the mReaders.
--
-- TODO: FIX MEMORY LEAK
----------------------------------------------------------------------------------------------------------------------*/
bool RfidReader::findReaders()
{
	QMutex mutex;
	bool isFound = false;

	mutex.lock();
	mNumDevices = SkyeTek_DiscoverDevices(&mDevices);
	if (mNumDevices == 0)
	{
		sendIOErrorSignal("No Devices Were Found");
		SkyeTek_FreeDevices(mDevices, mNumDevices);
		isFound = false;
	}

	mNumReaders = SkyeTek_DiscoverReaders(mDevices, mNumReaders, &mReaders);
	if (mNumReaders == 0)
	{
		sendIOErrorSignal("No Readers Were Found");
		SkyeTek_FreeDevices(mDevices, mNumDevices);
		SkyeTek_FreeReaders(mReaders, mNumReaders);
		isFound =  false;
	}
	
	sendIOMessageSignal(QString("Devices found %1, Readers found %2").arg(mNumDevices).arg(mNumReaders));

	isFound = true;
	mutex.unlock();

	return isFound;
}

void RfidReader::ReadOnce()
{
	if (mbHasReaders)
	{
		SKYETEK_STATUS st;
		LPSKYETEK_TAG* lpTags;
		unsigned short tagCount = 0;

		for (int i = 0; i < mNumReaders; i++)
		{
			st = SkyeTek_GetTags(mReaders[i], AUTO_DETECT, &lpTags, &tagCount);
			if (st != SKYETEK_SUCCESS)
			{
				sendIOErrorSignal(st);
			}
			else
			{
				for (unsigned short j = 0; j < tagCount; j++)
				{
					sendTagReadSignal(*(lpTags + j));
					SkyeTek_FreeTag(*(lpTags + j));
				}
			}
		}
	}
	else
	{
		mbHasReaders = findReaders();
	}
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidReader::tcharToQString
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void RfidThread::tcharToQString(const TCHAR* str)
--						const TCHAR* str: A null terminated TCHAR array that will be converted to a QString
--
-- RETURNS:			A QString that has the same ASCII representation as the null terminated TCHAR array that was
--					passed in.
--
-- NOTES:
-- Converts a null terminated TCHAR array used by the SkyeTek api to a QString object is used by the Qt framework.
----------------------------------------------------------------------------------------------------------------------*/
QString RfidReader::tcharToQString(const TCHAR* str) const
{
	QString tmp = QString("Tag Found");
	for (int i = 0; str[i] != '\0'; i++)
	{
		tmp.append((LPSTR)(str + i));
	}
	return tmp;
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidThread::SendTagReadSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void RfidThread::SendTagReadSignal(const LPSKYETEK_TAG lpTag)	
--						const LPSKYETEK_TAG lpTag: A pointer to a tag that will be parsed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Takes the tag that was passed in, converts the friendly variable from a null terminated TCHAR array to a QString
-- and emits the QString in a TagReadSignal().
----------------------------------------------------------------------------------------------------------------------*/
void RfidReader::sendTagReadSignal(const LPSKYETEK_TAG lpTag)
{
	emit TagReadSignal(tcharToQString(lpTag->friendly));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidThread::sendIOMessageSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void RfidThread::sendIOMessageSignal(const QString message)	
--						const QString message: A message that should be displayed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Prepends "IO Message: " to the function parameter message and emits the new QString in an IOMEssageSignal().
----------------------------------------------------------------------------------------------------------------------*/
void RfidReader::sendIOMessageSignal(const QString message)
{
	emit IOMessageSignal(QString("IO Message: %1").arg(message));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidReader::sendIOErrorSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void RfidReader::sendIOErrorSignal(const QString error)	
--						const QString error: An error message that should be displayed.
--
-- RETURNS:			void.
--
-- NOTES:
-- Prepends "IO Error: " to the function parameter error and emits the new QString in an IOErrorSignal().
----------------------------------------------------------------------------------------------------------------------*/
void RfidReader::sendIOErrorSignal(const QString error)
{
	emit IOErrorSignal(QString("IO Error: %1").arg(error));
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		RfidThread::sendIOErrorSignal
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang
--
-- PROGRAMMER:		Benny Wang
--
-- INTERFACE:		void RfidThread::sendIOErrorSignal(const SKYETEK_STATUS status)	
--						const SKYETEK_STATUS status: The status of an operation.
--
-- RETURNS:			void.
--
-- NOTES:
-- Retrives the status message that corrisponds to to the function parameter status, converts it to a QString, prepends
-- "IO Error: ", and emits the new QString in an IOErrorSignal(). 
----------------------------------------------------------------------------------------------------------------------*/
void RfidReader::sendIOErrorSignal(const SKYETEK_STATUS status)
{
	TCHAR* tcharStatusMessage = SkyeTek_GetStatusMessage(status);
	QString statusMessage = tcharToQString(tcharStatusMessage);
	emit IOErrorSignal(QString("IO Error: %1").arg(statusMessage));
}

void RfidReader::connectToReaderSlot()
{
	mbHasReaders = findReaders();
}
