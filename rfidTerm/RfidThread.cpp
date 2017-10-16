#include "RfidThread.h"

#include <QDebug>

bool bStop = false;

// Converts LPSKYETEK_DATA to QString
QString printData(const LPSKYETEK_DATA data)
{
	QString temp = QString("");

	for (int i = 0; i < data->size; i++)
	{
		temp.append((char)(data->data + i));
	}

	return temp;
}

// Converts tchar to QString
QString tcharToQString(const TCHAR* str)
{
	QString temp = QString("");

	for (int i = 0; str[i] != '\0'; i++)
	{
		temp.append((LPSTR)(str + i));
	}

	return temp;
}

// Callback that is feed to SkyeTek_SelectTags()
unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
{
	// While not being told to stop
	if (!bStop)
	{
		// If the tag is not null
		if (lpTag != NULL)
		{
			// Print tag information
			qDebug() << "Tag:" << tcharToQString(lpTag->friendly);

			// Free the memory
			SkyeTek_FreeTag(lpTag);
		}
	}

	// Tell SkyeTek_SelectTags to stop reading this specific tag
	return !bStop;
}

// Constructor
RfidThread::RfidThread(QObject* parent)
	: QThread(parent)
	, mbStop(false)
	, mDevices(NULL)
	, mReaders(NULL)
	, mNumOfDevices(NULL)
	, mNumOfReaders(NULL)
{
}

// Destructor
RfidThread::~RfidThread()
{
	// QThread stuff
	mbStop = true;
	wait();

	// SkyeTek memory management
	qDebug() << "Freeing Devices";
	SkyeTek_FreeDevices(mDevices, mNumOfDevices);
	SkyeTek_FreeReaders(mReaders, mNumOfReaders);

	// C++ memory management
	delete mDevices;
	delete mReaders;
}

// Finds readers that are connected to computer
bool RfidThread::getReaders()
{
	// Save number of usb devices found
	mNumOfDevices = SkyeTek_DiscoverDevices(&mDevices);

	// If no devices are found
	if (mNumOfDevices == 0)
	{
		// Manage memory
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		delete mDevices;
		qDebug() << "No Devices";
		return false;
	}
	// If devices are found
	else
	{
		// Display information
		qDebug() << "Num of Devices: " << mNumOfDevices;
		for (int i = 0; i < mNumOfDevices; i++)
		{
			qDebug() << "Device " << i << ": " << tcharToQString(mDevices[0]->friendly);
		}
	}

	// Save the number of readers found
	mNumOfReaders = SkyeTek_DiscoverReaders(mDevices, mNumOfDevices, &mReaders);

	// If no readers are found
	if (mNumOfReaders == 0)
	{
		// Manage memeory
		qDebug() << "Could not find readers";
		SkyeTek_FreeDevices(mDevices, mNumOfDevices);
		SkyeTek_FreeReaders(mReaders, mNumOfReaders);
		delete mDevices;
		delete mReaders;
		return false;
	}
	// If readers are found
	else
	{
		// Display information
		qDebug() << "Number of Readers: " << mNumOfReaders;
	}

	return (mNumOfReaders > 0);
}

// QThread main entry point when started
void RfidThread::run()
{
	bool foundReader = false;

	// loop until a reader is found
	while (!foundReader)
	{
		foundReader = getReaders();
	}

	// Selects all tags within range of the reader and passes the tag to the callback
	st = SkyeTek_SelectTags(mReaders[0], AUTO_DETECT, SelectLoopCallback, 0, 1, mReaders);

	if (st != SKYETEK_SUCCESS)
	{
		qDebug() << "Loop ended due to failure";
	}

	qDebug() << "Reading finished";
}
