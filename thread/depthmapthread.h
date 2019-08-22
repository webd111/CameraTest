#ifndef DEPTHMAPTHREAD_H
#define DEPTHMAPTHREAD_H

#include <QThread>
#include "opencv/stereocamera.h"

class DepthMapThread : public QThread
{
    Q_OBJECT
    StereoCamera* stereoCamera;
    ~DepthMapThread();
public:
    DepthMapThread(StereoCamera*);
    bool IsStillRunning = true;         // run() control flag
    QTime timedebug;

signals:
    void runTimes(QVector<int>, QStringList, int);

protected:
    void run();         // override run()

};

#endif // DEPTHMAPTHREAD_H
