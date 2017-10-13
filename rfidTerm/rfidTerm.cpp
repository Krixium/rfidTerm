#include "rfidTerm.h"

#include <string>
#include <QDebug>

bool isStop = false;

unsigned char SelectLoopCallback(LPSKYETEK_TAG lpTag, void* user)
{
	SKYETEK_MEMORY* mem;

	if (!isStop)
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
	return !isStop;
}

rfidTerm::rfidTerm(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	LPSKYETEK_DEVICE* devices = NULL;
	LPSKYETEK_READER* readers = NULL;
	unsigned int numOfDevices = 0;
	unsigned int numOfReaders = 0;

	numOfDevices = SkyeTek_DiscoverDevices(&devices);

	if (numOfDevices == 0)
	{
		qDebug() << "No Devices";
		return;
	}
	else
	{
		qDebug() << "Num of Devices: " << numOfDevices;
		for (int i = 0; i < numOfDevices; i++)
		{
			qDebug() << "Device " << i << ": " << devices[0]->friendly;
		}
	}

	numOfReaders = SkyeTek_DiscoverReaders(devices, numOfDevices, &readers);

	if (numOfReaders == 0)
	{
		SkyeTek_FreeDevices(devices, numOfDevices);
		return;
	}
	else
	{
		qDebug() << "Reader manufacturer: " << numOfReaders;
	}

	SKYETEK_STATUS st = SKYETEK_FAILURE;
	while (st == SKYETEK_FAILURE)
	{
		st = SkyeTek_SelectTags(readers[0], AUTO_DETECT, SelectLoopCallback, 0, 1, NULL);
	}

	SkyeTek_FreeDevices(devices, numOfDevices);
	SkyeTek_FreeReaders(readers, numOfReaders);
	qDebug() << "Freeing Devices";
}

