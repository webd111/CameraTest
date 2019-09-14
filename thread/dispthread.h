#ifndef DISPTHREAD_H
#define DISPTHREAD_H

#include <QObject>

class DispThread : public QObject
{
    Q_OBJECT
public:
    explicit DispThread(QObject *parent = nullptr);

signals:

public slots:
};

#endif // DISPTHREAD_H