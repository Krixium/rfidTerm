/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		IOThread.cpp - A QThread that discovers and interacts with a connected SkyeTek RFID reader. 

-- PROGRAM:			rfidTerm 
--
-- FUNCTIONS:
--					IOThread::IOThread (QObject *parent) 
--					IOThread::~IOThread()
--					void IOThread::run()
-- 
-- QT Signals:		
--					void readOnceSignal() 
--
-- QT Slots:
--					void StopRunningSlot()
--
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
IOThread::IOThread(QObject* parent, RfidReader* reader)
	: QThread(parent)
	, mReader(reader)
	, mbRunning(true)
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
-- Deconstructor, deletes the reader and stops the running of the thread.
----------------------------------------------------------------------------------------------------------------------*/
IOThread::~IOThread()
{
	delete mReader;
	mbRunning = false;
	wait();
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
	mReader->ReadOnce();
}

void IOThread::StopRunningSlot()
{
	mbRunning = false;
}