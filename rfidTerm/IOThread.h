#pragma once

#include <QThread>

#include <stapi/SkyeTekAPI.h>

#include "RfidReader.h"

class IOThread : public QThread
{
	Q_OBJECT
public:
	IOThread(QObject* parent, RfidReader* reader);
	~IOThread();

private:
	bool mbRunning;

	RfidReader* mReader;

protected:
	void run();

signals:
	void ReadOnceSignal();

public slots:
	void StopRunningSlot();
};