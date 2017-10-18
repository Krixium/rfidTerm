/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE:		main.cpp - starts the program
-- PROGRAM:			rfidTerm
--
-- FUNCTIONS:
--					int main(int argc, char *argv[])
--
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Qt company
--
-- PROGRAMMER:		Qt company
--
-- NOTES:
-- Main function. Creates a new Qt application, sets up the reader/console and executes the application.
----------------------------------------------------------------------------------------------------------------------*/
#include "rfidTerm.h"
#include <QtWidgets/QApplication>

/*--------------------------------------------------------------------------------------------------------------------
-- FUNCTION:		main.cpp
--
-- DATE:			October 16, 2017
--
-- REVISIONS:		N/A
--
-- DESIGNER:		Qt company
--
-- PROGRAMMER:		Qt company
--
-- INTERFACE:		int main(int argc, char *argv[])
--
-- RETURNS:			a.exec()
--
-- NOTES:
-- Main function. Creates a new Qt application, sets up the reader/console and executes the application.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	rfidTerm w;
	w.show();
	return a.exec();
}
