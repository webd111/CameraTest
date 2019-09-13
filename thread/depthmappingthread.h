#ifndef DEPTHMAPPINGTHREAD_H
#define DEPTHMAPPINGTHREAD_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include <highaccuracytimer.h>
#include <opencv/stereocamera.h>


class DepthMappingThread : public QThread
{
    Q_OBJECT
    ~DepthMappingThread();
    HighAccuracyTimer timer;

    StereoCamera* stereoCamera;
public:
    explicit DepthMappingThread(StereoCamera* stereoCamera);

    void end();
signals:

public slots:

protected:
    void run();
};

#endif // DEPTHMAPPINGTHREAD_H
