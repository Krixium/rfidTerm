/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		Console.cpp - A console that displays the appropriate slot for the signal recieves
-- PROGRAM:			rfidTerm
--
-- FUNCTIONS:
--					Console::Console(QWidget *parent)
--					void Console::printTag(const QString data)
--					void Console::printMessage(const QString data)
--					void Console::printError(const QString data)
--					void Console::keyPressEvent(QKeyEvent *e)
--					
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- NOTES:
-- When Console is first created, only reading is enabled. Console is responsible for displaying the information 
-- recieved. All of the information recieved in QString is appended into plain text with the exception of the error message.
-- When the 'ESC' Key is pressed, a StopReadingSignal() is emitted.
----------------------------------------------------------------------------------------------------------------------*/
#include "console.h"


/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		Console::Console
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- INTERFACE:		Console::Console(QWidget *parent)

--						QWidget* parent: A pointer to this object's parent widget.
--
-- RETURNS:			void.
--
-- NOTES:
-- The constructor for Console. Sets the Console to isReadOnly() to disable write.
----------------------------------------------------------------------------------------------------------------------*/
Console::Console(QWidget *parent)
	: QPlainTextEdit(parent)
{
	isReadOnly();
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		Console::printTag
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- INTERFACE:		void Console::printTag(const QString data)
--
-- RETURNS:			void.
--
-- NOTES:
-- Receives the tag in QString and appends it into plain text onto the console.
--
----------------------------------------------------------------------------------------------------------------------*/
void Console::printTag(const QString data)
{
	appendPlainText(data);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		Console::printMessage
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- INTERFACE:		void Console::printMessage(const QString data)
--
-- RETURNS:			void.
--
-- NOTES:
-- Receives the message in QString and appends it into plain text to the console.
--
----------------------------------------------------------------------------------------------------------------------*/
void Console::printMessage(const QString data)
{
	appendPlainText(data);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		Console::printError
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- INTERFACE:		void Console::printError(const QString data)
--
-- RETURNS:			void.
--
-- NOTES:
-- Receives the error information in QString and displays the information in an error box.
--
----------------------------------------------------------------------------------------------------------------------*/
void Console::printError(const QString data)
{
	QMessageBox::information(this, tr("Error"), data);
}

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		Console::keyPressEvent
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Benny Wang, Roger Zhang, Michaela Yoon
--
-- PROGRAMMER:		Michaela Yoon
--
-- INTERFACE:		void Console::keyPressEvent(QKeyEvent *e)
--
-- RETURNS:			void.
--
-- NOTES:
-- Emits a signal to stop reading when the 'ESC' key is pressed.
--
----------------------------------------------------------------------------------------------------------------------*/
void Console::keyPressEvent(QKeyEvent *e)
{
	switch (e->key()) {
	case Qt::Key_Escape:
		emit StopReadingSignal();
		break;
	default:
		break;
	}
}
