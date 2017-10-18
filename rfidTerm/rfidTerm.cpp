/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		 rfidTerm.cpp - The main function that initializes all the signals and slots. And starts/ends the reading connections.

-- PROGRAM:			rfidTerm
--
-- FUNCTIONS:
--					rfidTerm(QWidget *parent = Q_NULLPTR);
--					inline void setConsoleEnabled(const bool isEnabled = false);
--					void rfidTerm::StartReading()
--					void rfidTerm::EndReading()
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang and Michaela Yoon
--
-- PROGRAMMER:		Benny Wang and Roger Zhang
--
-- NOTES:
-- rfidTerm is the main UI function responsible to start and end connections to the reader. Once a connection action is
-- triggered, a signal is sent and appropriate slot will handle the signal.
----------------------------------------------------------------------------------------------------------------------*/
#include "rfidTerm.h"

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		rfidTerm
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang and Roger Zhang
--
-- PROGRAMMER:		Benny Wang and Roger Zhang
--
-- INTERFACE:		rfidTerm::rfidTerm(QWidget *parent).
--
--
-- RETURNS:			void.
--
-- NOTES:
-- This is the constructor to initialize all the signals/slots. This will also disable the console by default.
----------------------------------------------------------------------------------------------------------------------*/

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
	, mThread(new IOThread(this))
	, mConsole(new Console(this))
{
	ui.setupUi(this);

	setCentralWidget(mConsole);

	connect(ui.actionConnect, &QAction::triggered, this, &rfidTerm::StartReading);
	connect(ui.actionDisconnect, &QAction::triggered, this, &rfidTerm::StopReading);

	connect(mThread, &IOThread::TagReadSignal, mConsole, &Console::PrintTag);
	connect(mThread, &IOThread::IOMessageSignal, mConsole, &Console::PrintMessage);
	connect(mThread, &IOThread::IOErrorSignal, mConsole, &Console::PrintError);

	connect(mThread, &IOThread::NoReaderFoundSignal, this, &rfidTerm::StopReading);
	connect(mConsole, &Console::StopReadingSignal, this, &rfidTerm::StopReading);

	mConsole->setEnabled(false);
	ui.actionConnect->setEnabled(true);
	ui.actionDisconnect->setEnabled(false);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		setConsoleEnabled
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang and Roger Zhang
--
-- PROGRAMMER:		Benny Wang and Roger Zhang
--
-- INTERFACE:		void rfidTerm::setConsoleEnabled(const bool isEnabled).
--
-- RETURNS:			void.
--
-- NOTES:
-- This function enables the console, disconnect button and disable the connect button. The function is called when
-- user enters clicks the connection button from the menu.
----------------------------------------------------------------------------------------------------------------------*/
void rfidTerm::setConsoleEnabled(const bool isEnabled)
{
	mConsole->setEnabled(isEnabled);
	ui.actionConnect->setEnabled(!isEnabled);
	ui.actionDisconnect->setEnabled(isEnabled);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		StartReading
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang and Roger Zhang
--
-- PROGRAMMER:		Benny Wang and Roger Zhang
--
-- INTERFACE:		void rfidTerm::StartReading().
--
-- RETURNS:			void.
--
-- NOTES:
-- This starts the thread and enables the console.
----------------------------------------------------------------------------------------------------------------------*/
void rfidTerm::StartReading()
{
	mThread->start();
	setConsoleEnabled(true);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		StopReading()
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang and Roger Zhang
--
-- PROGRAMMER:		Benny Wang and Roger Zhang
--
-- INTERFACE:		void rfidTerm::StopReading().
--
-- RETURNS:			void.
--
-- NOTES:
-- This stops the thread and disables the console.
----------------------------------------------------------------------------------------------------------------------*/
void rfidTerm::StopReading()
{
	mThread->terminate();
	setConsoleEnabled(false);
}
