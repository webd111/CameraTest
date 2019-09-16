#ifndef DISPTHREAD_H
#define DISPTHREAD_H

#include <QThread>
#include <QQueue>

#include <opencv/myopencv.h>
#include <myvariable.h>

using namespace cv;

class DispThread : public QThread
{
    Q_OBJECT

public:
    explicit DispThread();
    ~DispThread();

    QQueue<Mat> img_queue;      // RGB图像
    QQueue<Mat> imgd_queue;     // 深度图图像
    QMutex m_queue;

protected:
    void run();

signals:

public slots:
};

#endif // DISPTHREAD_H
