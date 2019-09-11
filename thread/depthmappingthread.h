#ifndef DEPTHMAPPINGTHREAD_H
#define DEPTHMAPPINGTHREAD_H

#include <QObject>
#include <QThread>
#include <highaccuracytimer.h>

class DepthMappingThread : public QThread
{
    Q_OBJECT
    ~DepthMappingThread();
    HighAccuracyTimer timer;
public:
    explicit DepthMappingThread();

signals:

public slots:

protected:
    void run();
};

#endif // DEPTHMAPPINGTHREAD_H
