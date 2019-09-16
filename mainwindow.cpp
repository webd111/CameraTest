#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
//    qDebug() << "MainWindow::MainWindow()";
    ui->setupUi(this);

    ui->labelOriginalImage1->setScaledContents(true);
    ui->labelOriginalImage2->setScaledContents(true);
    ui->labelOriginalImage3->setScaledContents(true);

    qlabel_ptr.push_back(ui->labelOriginalImage1);
    qlabel_ptr.push_back(ui->labelOriginalImage2);
    qlabel_ptr.push_back(ui->labelOriginalImage3);

    ui->comboBoxCameraInterface1->setItemText(0,USBCameraInterface);
    ui->comboBoxCameraInterface2->setItemText(0,USBCameraInterface);
    ui->comboBoxCameraInterface3->setItemText(0,USBCameraInterface);

    qDebug() << "Main Thread Id: " << QThread::currentThreadId();

    labelRunTime = new QLabel();
    labelRunTime->setMinimumSize(labelRunTime->sizeHint());
    labelRunTime->setAlignment(Qt::AlignLeft);

    labelSystemTime = new QLabel();
    labelRunTime->setMinimumSize(labelRunTime->sizeHint());
    labelRunTime->setAlignment(Qt::AlignLeft);

    MainTimer = new QTimer();
    connect(MainTimer,SIGNAL(timeout()),this,SLOT(MainTimerUpdate()));
    MainTimer->start(100);

    SystemTime = new QTime();
    SystemTime->start();

    ui->statusBar->addPermanentWidget(labelRunTime);
    ui->statusBar->addPermanentWidget(labelSystemTime);

    WidgetInit();
    AlgorithmInit();
}

MainWindow::~MainWindow()
{
    delete labelRunTime;
    delete labelSystemTime;
    delete MainTimer;
    delete SystemTime;
    delete model;
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
#ifdef WIN32
    try
    {
        SetWindowExtents(hv_WindowHandleSrc1, ui->centralWidget->y() + ui->groupBoxOriginalImage->y() + ui->labelOriginalImage1->y(),
                         ui->centralWidget->x() + ui->groupBoxOriginalImage->x() + ui->labelOriginalImage1->x(),
                         ui->labelOriginalImage1->width(), ui->labelOriginalImage1->height());
        SetWindowExtents(hv_WindowHandleSrc2, ui->centralWidget->y() + ui->groupBoxOriginalImage->y() + ui->labelOriginalImage2->y(),
                         ui->centralWidget->x() + ui->groupBoxOriginalImage->x() + ui->labelOriginalImage2->x(),
                         ui->labelOriginalImage2->width(), ui->labelOriginalImage2->height());
        SetWindowExtents(hv_WindowHandleSrc3, ui->centralWidget->y() + ui->groupBoxDeepImage->y() + ui->labelOriginalImage3->y(),
                         ui->centralWidget->x() + ui->groupBoxDeepImage->x() + ui->labelOriginalImage3->x(),
                         ui->labelOriginalImage3->width(), ui->labelOriginalImage3->height());
        SetWindowExtents(hv_WindowHandleSrc4, ui->centralWidget->y() + ui->groupBoxDeepImage->y() + ui->labelOriginalImage4->y(),
                         ui->centralWidget->x() + ui->groupBoxDeepImage->x() + ui->labelOriginalImage4->x(),
                         ui->labelOriginalImage4->width(), ui->labelOriginalImage4->height());

        DispImage(hImageSrc1, hv_WindowHandleSrc1);
        DispImage(hImageSrc2, hv_WindowHandleSrc2);
        DispImage(hImageSrc3, hv_WindowHandleSrc3);
        DispImage(hImageSrc4, hv_WindowHandleSrc4);
    }
    catch (HException &){}
#endif
}

void MainWindow::closeEvent(QCloseEvent*)
{
#ifdef WIN32
    HalconDeinit();
#endif
}

void MainWindow::HalconInit()
{
//    qDebug() << "MainWindow::HalconInit()";
    try {
        SetWindowAttr("background_color", "gray");
        OpenWindow(ui->centralWidget->y() + ui->groupBoxOriginalImage->y() + ui->labelOriginalImage1->y(),
                   ui->centralWidget->x() + ui->groupBoxOriginalImage->x() + ui->labelOriginalImage1->x(),
                   ui->labelOriginalImage1->width(), ui->labelOriginalImage1->height(),
                   Hlong(this->winId()), "", "", &hv_WindowHandleSrc1);
        HDevWindowStack::Push(hv_WindowHandleSrc1);
        SetWindowAttr("background_color", "gray");
        OpenWindow(ui->centralWidget->y() + ui->groupBoxOriginalImage->y() + ui->labelOriginalImage2->y(),
                   ui->centralWidget->x() + ui->groupBoxOriginalImage->x() + ui->labelOriginalImage2->x(),
                   ui->labelOriginalImage2->width(), ui->labelOriginalImage2->height(),
                   Hlong(this->winId()), "", "", &hv_WindowHandleSrc2);
        HDevWindowStack::Push(hv_WindowHandleSrc2);
        SetWindowAttr("background_color", "gray");
        OpenWindow(ui->centralWidget->y() + ui->groupBoxDeepImage->y() + ui->labelOriginalImage3->y(),
                   ui->centralWidget->x() + ui->groupBoxDeepImage->x() + ui->labelOriginalImage3->x(),
                   ui->labelOriginalImage3->width(), ui->labelOriginalImage3->height(),
                   Hlong(this->winId()), "", "", &hv_WindowHandleSrc3);
        HDevWindowStack::Push(hv_WindowHandleSrc3);
        SetWindowAttr("background_color", "gray");
        OpenWindow(ui->centralWidget->y() + ui->groupBoxDeepImage->y() + ui->labelOriginalImage4->y(),
                   ui->centralWidget->x() + ui->groupBoxDeepImage->x() + ui->labelOriginalImage4->x(),
                   ui->labelOriginalImage4->width(), ui->labelOriginalImage4->height(),
                   Hlong(this->winId()), "", "", &hv_WindowHandleSrc4);
        HDevWindowStack::Push(hv_WindowHandleSrc4);
    } catch (HException &exception)
    {
        fprintf(stderr, "  Error #%u in %s: %s\n", exception.ErrorCode(),
                exception.ProcName().Text(),
                exception.ErrorMessage().Text());
    }

//    qDebug() << "MainWindow::HalconInit() end";
}

void MainWindow::HalconDeinit()
{
    for (int i = 0; i < hv_WindowHandle_ptr.size(); ++i) {
        try {
            CloseWindow(*hv_WindowHandle_ptr[i]);
        } catch (HException&) {}
    }
}

void MainWindow::WidgetInit()
{
    model->setColumnCount(3);
    model->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("Camera No."));
    model->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("Time"));
    model->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("Error Message"));

    ui->tableViewException->setModel(model);
    ui->tableViewException->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);//表头信息显示居左
    ui->tableViewException->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableViewException->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableViewException->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

void MainWindow::AlgorithmInit()
{

}

void MainWindow::MainTimerUpdate()
{
    labelRunTime->setText("Run Time : " + QString::number(SystemTime->elapsed()/1000.0,'f',1) + "s");
    labelSystemTime->setText("System Time : " + QTime::currentTime().toString("HH:mm:ss"));
}

void MainWindow::imgAcqThread1Destoryed()
{
    ui->actionCamera1Read->setChecked(false);
}

void MainWindow::imgAcqThread2Destoryed()
{
    ui->actionCamera2Read->setChecked(false);
}

void MainWindow::imgAcqThread3Destoryed()
{
    ui->actionCamera3Read->setChecked(false);
}

void MainWindow::depthMappingThreadDestoryed()
{
    ui->actionDepthMapping->setChecked(false);
}

void MainWindow::on_actionCamera1Read_triggered()
{
    ui->tabWidgetParameter->setCurrentIndex(0);         //将参数tab设置为当前对应tab
    if(ui->actionCamera1Read->isChecked())
    {
        if(imgAcqThread1.isNull())    //对象未创建
        {
            int mode = 0;
            if (ui->radioButtonSingleView->isChecked())
                mode = 1;
            else if (ui->radioButtonDoubleView->isChecked())
                mode = 2;
            else if (ui->radioButtonFullView->isChecked())
                mode = 3;
            if(ui->comboBoxCameraInterface1->currentText() == USBCameraInterface ||
                    ui->comboBoxCameraInterface1->currentText() == "GigEVision2")
            {
                HCameraParams hparams;
                hparams.cameraIndex = 1;
                hparams.cameraInterface = ui->comboBoxCameraInterface1->currentText();
                hparams.device = ui->comboBoxCameraDevice1->currentText();
                hparams.port = ui->comboBoxCameraPort1->currentText().toInt();
                hparams.cameraType = ui->comboBoxCameraType1->currentText();
                hparams.isConnected = false;
                imgAcqThread1 = new ImgAcqThread(hparams, mode);
            }
            else if(ui->comboBoxCameraInterface1->currentText() == "UserDefined")
            {
                WCameraParams wparams;
                wparams.cameraIndex = 1;
                wparams.cameraInterface = ui->comboBoxCameraInterface1->currentText();
                wparams.ip_client = ui->comboBoxCameraDevice1->currentText().toStdString();
                wparams.port_client = ui->comboBoxCameraPort1->currentText().toUShort();
                wparams.ip_server = ui->comboBoxCameraType1->currentText().toStdString();
                wparams.port_server = ui->comboBoxCameraPort1_2->currentText().toUShort();
                wparams.width = ui->comboBoxImageWidth1->currentText().toInt();
                wparams.height = ui->comboBoxImageHeight1->currentText().toInt();
                wparams.channels = ui->comboBoxImageChannel1->currentText().toInt();
                wparams.data_length = ui->comboBoxDataLength1->currentText().toInt();
                imgAcqThread1 = new ImgAcqThread(wparams, mode);
            }
            connect(imgAcqThread1, &ImgAcqThread::finished, imgAcqThread1, &QObject::deleteLater);
            connect(imgAcqThread1, &QObject::destroyed, this, &MainWindow::imgAcqThread1Destoryed);
            connect(imgAcqThread1, SIGNAL(ImgCaptured(int)), this, SLOT(updateImage(int)));
            connect(imgAcqThread1, SIGNAL(ImgAcqTime(int, double)), this, SLOT(updateStatistic(int, double)), Qt::QueuedConnection);
            connect(imgAcqThread1, SIGNAL(ImgAcqException(int, QString, HException)), this, SLOT(updateException(int, QString, HException)), Qt::QueuedConnection);
            connect(imgAcqThread1, SIGNAL(ImgAcqInfo(int, QString*)), this, SLOT(updateInformation(int, QString*)), Qt::QueuedConnection);
            connect(imgAcqThread1, SIGNAL(ImgLossRate(double)), this, SLOT(updateLossRate(double)), Qt::QueuedConnection);
        }
        if(!imgAcqThread1->isRunning())
            imgAcqThread1->start();
    }
    else
    {
        if(!imgAcqThread1.isNull())
        {
            if(imgAcqThread1->isRunning())
            {
//                // All signals to and from the object are automatically disconnected. So there's no need to disconnect manually.
//                disconnect(imgAcqThread1, SIGNAL(ImgAcqTime(int, double)), this, SLOT(updateStatistic(int, double)));
//                disconnect(imgAcqThread1, SIGNAL(ImgAcqException(int, QString, HException)), this, SLOT(updateException(int, QString, HException)));
//                disconnect(imgAcqThread1, SIGNAL(ImgAcqInfo(int, QString*)), this, SLOT(updateInformation(int, QString*)));
                imgAcqThread1->end();       // This thread will quit gracefully
            }
        }
    }
}

void MainWindow::on_actionCamera2Read_triggered()
{
    ui->tabWidgetParameter->setCurrentIndex(1);         //将参数tab设置为当前对应tab
    if(ui->actionCamera2Read->isChecked())
    {
        if(imgAcqThread2.isNull())    //对象未创建
        {
            HCameraParams hparams;
            hparams.cameraIndex = 2;
            hparams.cameraInterface = ui->comboBoxCameraInterface2->currentText();
            hparams.device = ui->comboBoxCameraDevice2->currentText();
            hparams.port = ui->comboBoxCameraPort2->currentText().toInt();
            hparams.cameraType = ui->comboBoxCameraType2->currentText();
            hparams.isConnected = false;
            imgAcqThread2 = new ImgAcqThread(hparams);
            QObject::connect(imgAcqThread2, &ImgAcqThread::finished, imgAcqThread2, &QObject::deleteLater);
            connect(imgAcqThread2, &QObject::destroyed, this, &MainWindow::imgAcqThread2Destoryed);
            connect(imgAcqThread2, SIGNAL(ImgCaptured(int)), this, SLOT(updateImage(int)));
            connect(imgAcqThread2, SIGNAL(ImgAcqTime(int, double)), this, SLOT(updateStatistic(int, double)), Qt::QueuedConnection);
            connect(imgAcqThread2, SIGNAL(ImgAcqException(int, QString, HException)), this, SLOT(updateException(int, QString, HException)), Qt::QueuedConnection);
            connect(imgAcqThread2, SIGNAL(ImgAcqInfo(int, QString*)), this, SLOT(updateInformation(int, QString*)), Qt::QueuedConnection);
        }
        if(!imgAcqThread2->isRunning())
            imgAcqThread2->start();
    }
    else
    {
        if(!imgAcqThread2.isNull())
        {
            if(imgAcqThread2->isRunning())
            {
                imgAcqThread2->end();
            }
        }
    }
}

void MainWindow::on_actionCamera3Read_triggered()
{
    ui->tabWidgetParameter->setCurrentIndex(2);         //将参数tab设置为当前对应tab
    if(ui->actionCamera3Read->isChecked())
    {
        if(imgAcqThread3.isNull())    //对象未创建
        {
            HCameraParams hparams;
            hparams.cameraIndex = 3;
            hparams.cameraInterface = ui->comboBoxCameraInterface3->currentText();
            hparams.device = ui->comboBoxCameraDevice3->currentText();
            hparams.port = ui->comboBoxCameraPort3->currentText().toInt();
            hparams.cameraType = ui->comboBoxCameraType3->currentText();
            hparams.isConnected = false;
            imgAcqThread3 = new ImgAcqThread(hparams);
            QObject::connect(imgAcqThread3, &ImgAcqThread::finished, imgAcqThread3, &QObject::deleteLater);
            connect(imgAcqThread3, &QObject::destroyed, this, &MainWindow::imgAcqThread3Destoryed);
            connect(imgAcqThread3, SIGNAL(ImgCaptured(int)), this, SLOT(updateImage(int)));
            connect(imgAcqThread3, SIGNAL(ImgAcqTime(int, double)), this, SLOT(updateStatistic(int, double)), Qt::QueuedConnection);
            connect(imgAcqThread3, SIGNAL(ImgAcqException(int, QString, HException)), this, SLOT(updateException(int, QString, HException)), Qt::QueuedConnection);
            connect(imgAcqThread3, SIGNAL(ImgAcqInfo(int, QString*)), this, SLOT(updateInformation(int, QString*)), Qt::QueuedConnection);
        }
        if(!imgAcqThread3->isRunning())
            imgAcqThread3->start();
    }
    else
    {
        if(!imgAcqThread3.isNull())
        {
            if(imgAcqThread3->isRunning())
            {
                imgAcqThread3->end();
            }
        }
    }
}

void MainWindow::on_actionDepthMapping_triggered()
{
    if(ui->actionDepthMapping->isChecked())
    {
        if(depthMappingThread.isNull())    //对象未创建
        {
            depthMappingThread = new DepthMappingThread(stereoCamera);

            connect(depthMappingThread, &DepthMappingThread::finished, depthMappingThread, &QObject::deleteLater);
            connect(depthMappingThread, &QObject::destroyed, this, &MainWindow::depthMappingThreadDestoryed);
        }
        if(!depthMappingThread->isRunning())
            depthMappingThread->start();
    }
    else
    {
        if(!depthMappingThread.isNull())
        {
            if(depthMappingThread->isRunning())
            {
//                // All signals to and from the object are automatically disconnected. So there's no need to disconnect manually.
//                disconnect(depthMappingThread, SIGNAL(ImgAcqTime(int, double)), this, SLOT(updateStatistic(int, double)));
//                disconnect(depthMappingThread, SIGNAL(ImgAcqException(int, QString, HException)), this, SLOT(updateException(int, QString, HException)));
//                disconnect(depthMappingThread, SIGNAL(ImgAcqInfo(int, QString*)), this, SLOT(updateInformation(int, QString*)));
                depthMappingThread->end();       // This thread will quit gracefully
            }
        }
    }
}

void MainWindow::on_actionMatching_triggered()
{
}

void MainWindow::on_pushButtonReadParams_clicked()
{
//    QString filepath = QFileDialog::getOpenFileName(this,
//        tr("浏览"),
//        "",
//        tr("*.csv")); //选择路径
//    QFile file(filepath);
//    if(!file.open(QFile::ReadOnly|QFile::Text))
//    {
//        QMessageBox::warning(this, tr("Error"), tr("Cannot open file!"));
//        return;
//    }
//    QTextStream textStream(&file);

//    QString textLine;
//    QStringList textList;
//    double params[9][10] = {{0}};
//    textStream.readLineInto(&textLine);
////    for(int i = 0; textStream.readLineInto(&textLine); i++)
//    for(int i = 0; i < 9; i++)
//    {
//        textStream.readLineInto(&textLine);
//        textList = textLine.split(",");
//        for(int j = 0; j < 10; j++)
//        {
//            params[i][j] = textList.takeFirst().toDouble();
//        }
//    }
//    file.close();

//    updateParamsWidget();
}

void MainWindow::updateParamsWidget()
{

}

void MainWindow::on_pushButtonApplyParams_clicked()
{

}

void MainWindow::updateImage(int idx)
{
#ifdef WIN32

#elif __linux__
    QImage qimg;
    HImage img;
    QMutexLocker locker(m_ptr[idx-1]);
    img = *hImage_ptr[idx-1];
    locker.unlock();
    HImage2QImage(img, qimg);
    qlabel_ptr[idx-1]->setPixmap(QPixmap::fromImage(qimg));
//    img.WriteImage("jpg", 0, "img");
//    qDebug() << "Save flag:" << qimg.save("img.jpg");
#endif
}

void MainWindow::updateStatistic(int cameraIndex, double time)
{
    switch (cameraIndex)
    {
    case 1:
        ui->doubleSpinBoxCamera1Time->setValue(time);
        ui->doubleSpinBoxCamera1Fps->setValue(1000/time);
        break;
    case 2:
        ui->doubleSpinBoxCamera2Time->setValue(time);
        ui->doubleSpinBoxCamera2Fps->setValue(1000/time);
        break;
    case 3:
        ui->doubleSpinBoxCamera3Time->setValue(time);
        ui->doubleSpinBoxCamera3Fps->setValue(1000/time);
        break;
    default:
        break;
    }
}

void MainWindow::updateException(int cameraIndex, QString time, HException e)
{
    QStandardItem* itemIndex = new QStandardItem;
    QStandardItem* itemTime = new QStandardItem;
    QStandardItem* itemException = new QStandardItem;

    itemIndex->setText(QString::number(cameraIndex));
    itemTime->setText(time);
    itemException->setText(e.ErrorMessage().Text());

    int rows = model->rowCount();
    model->setItem(rows, 0, itemIndex);
    model->setItem(rows, 1, itemTime);
    model->setItem(rows, 2, itemException);

//    delete itemException;
    ui->tableViewException->setModel(model);

//    // They should not be deleted
//    delete itemIndex;
//    delete itemTime;
}

void MainWindow::updateException(QString time, QString e1, HException e2)
{

}

void MainWindow::updateLossRate(double _rate)
{
    ui->doubleSpinBoxCamera1LossRate->setValue(_rate*100);
}

void MainWindow::updateInformation(int cameraIndex, QString* cameraInfo)
{
    qDebug() << "MainWindow::updateInformation";
    QLabel* labelInterface = nullptr;
    QLabel* labelDevice = nullptr;
    QLabel* labelPort = nullptr;
    QLabel* labelType = nullptr;
    QLabel* labelLabelIp = nullptr;
    QLabel* labelLabelCameraIp = nullptr;
    qDebug() << cameraIndex;
    qDebug() << cameraInfo[0] << cameraInfo[1] << cameraInfo[2] << cameraInfo[3];
    switch (cameraIndex)
    {
    case 1:
        labelInterface = ui->labelInfoInterfaceShow1;
        labelDevice = ui->labelInfoDeviceShow1;
        labelPort = ui->labelInfoPortShow1;
        labelType = ui->labelInfoTypeShow1;
        labelLabelIp = ui->labelInfoPort1;
        labelLabelCameraIp = ui->labelInfoType1;
        break;
    case 2:
        labelInterface = ui->labelInfoInterfaceShow2;
        labelDevice = ui->labelInfoDeviceShow2;
        labelPort = ui->labelInfoPortShow2;
        labelType = ui->labelInfoTypeShow2;
        labelLabelIp = ui->labelInfoPort2;
        labelLabelCameraIp = ui->labelInfoType2;
        break;
    case 3:
        labelInterface = ui->labelInfoInterfaceShow3;
        labelDevice = ui->labelInfoDeviceShow3;
        labelPort = ui->labelInfoPortShow3;
        labelType = ui->labelInfoTypeShow3;
        labelLabelIp = ui->labelInfoPort3;
        labelLabelCameraIp = ui->labelInfoType3;
        break;
    default:
        break;
    }
//    qDebug() << cameraInfo[0] << cameraInfo[1] << cameraInfo[2] << cameraInfo[3];
    labelInterface->setText(cameraInfo[0]);
    labelDevice->setText(cameraInfo[1]);
    if(cameraInfo[0] == USBCameraInterface || cameraInfo[0] == "GigEVision2")
    {
        labelLabelIp->setText(QString::fromLocal8Bit("端口："));
        labelLabelIp->setAlignment(Qt::AlignCenter);
        labelLabelCameraIp->setText(QString::fromLocal8Bit("相机类型："));
        labelLabelCameraIp->setAlignment(Qt::AlignCenter);
        labelPort->setText(cameraInfo[2]);
        labelType->setText(cameraInfo[3]);
    }
    else if(cameraInfo[0] == "UserDefined")
    {
        labelLabelIp->setText(QString::fromLocal8Bit("本机IP及端口："));
        labelLabelIp->setAlignment(Qt::AlignCenter);
        labelLabelCameraIp->setText(QString::fromLocal8Bit("相机IP及端口："));
        labelLabelCameraIp->setAlignment(Qt::AlignCenter);
        labelPort->setText(cameraInfo[2]);
        labelType->setText(cameraInfo[3]);
    }
    nextInFocusChain();
}

void MainWindow::on_actionSave_triggered()
{
    HImage img,img2,img3,img4;
    QMutexLocker locker1(m_ptr[0]);
    if(hImageSrc1.IsInitialized())
        img = hImageSrc1;
    else
        return;
    locker1.unlock();
    QMutexLocker locker2(m_ptr[1]);
    if(hImageSrc2.IsInitialized())
        img2 = hImageSrc2;
    else
        return;
    locker2.unlock();
    QMutexLocker locker3(m_ptr[2]);
    if(hImageSrc3.IsInitialized())
        img3 = hImageSrc3;
    else
        return;
    locker3.unlock();
//    QMutexLocker locker4(m_ptr[3]);
//    if(hImageSrc4.IsInitialized())
//        img4 = hImageSrc4;
//    else
//        return;
//    locker4.unlock();
    char* pathA = QString(QString("C:/Users/12257/Desktop/calibration/imgA") +
                          QTime::currentTime().toString("HHmmss") + QString(".png")).toLocal8Bit().data();
    qDebug() << "pathA:" << pathA;
    img.WriteImage("png", 0, pathA);
    char* pathB = QString(QString("C:/Users/12257/Desktop/calibration/imgB") +
                          QTime::currentTime().toString("HHmmss") + QString(".png")).toLocal8Bit().data();
    qDebug() << "pathB:" << pathB;
    img2.WriteImage("png", 0, pathB);
    char* pathC = QString(QString("C:/Users/12257/Desktop/calibration/imgC") +
                          QTime::currentTime().toString("HHmmss") + QString(".png")).toLocal8Bit().data();
    qDebug() << "pathC:" << pathC;
    img3.WriteImage("png", 0, pathC);
}

void MainWindow::on_actionRefresh_triggered()
{
    try
    {
        ui->statusBar->showMessage("Searching for devices...");
        repaint();
        HalconCamera::getCameraInfo();
        QVector<QString> deviceList;
        QVector<QString> interfaceList;
        QVector<QComboBox*> comboBoxDeviceList;
        interfaceList.push_back(ui->comboBoxCameraInterface1->currentText());
        interfaceList.push_back(ui->comboBoxCameraInterface2->currentText());
        interfaceList.push_back(ui->comboBoxCameraInterface3->currentText());
        comboBoxDeviceList.push_back(ui->comboBoxCameraDevice1);
        comboBoxDeviceList.push_back(ui->comboBoxCameraDevice2);
        comboBoxDeviceList.push_back(ui->comboBoxCameraDevice3);

        for (int i = 0; i < interfaceList.size(); i++)
        {
            deviceList.clear();
            if(interfaceList.at(i) == USBCameraInterface ||
                    interfaceList.at(i) == "GigEVision2")
            {
                deviceList = HalconCamera::getAvailableDevice(interfaceList.at(i));
            }
            else if(interfaceList.at(i) == "UserDefined")
            {
                deviceList = WebCamera::getAvailableDevice(interfaceList.at(i));
            }
            comboBoxDeviceList.at(i)->clear();
            for(int j = 0; j < deviceList.size(); j++)
            {
                comboBoxDeviceList.at(i)->insertItem(j+1, deviceList[j]);

            }
        }
        ui->statusBar->showMessage("");
    }
    catch(HException&){}
}


void MainWindow::on_comboBoxCameraInterface1_currentTextChanged(const QString &arg1)
{
    if(arg1 == USBCameraInterface)
    {
        ui->comboBoxCameraPort1->setEnabled(true);
        ui->comboBoxCameraType1->setEnabled(true);
        ui->comboBoxCameraPort1_2->setEnabled(false);
        ui->comboBoxImageWidth1->setEnabled(false);
        ui->comboBoxImageHeight1->setEnabled(false);
        ui->comboBoxDataLength1->setEnabled(false);
        ui->comboBoxImageChannel1->setEnabled(false);

        ui->comboBoxCameraPort1->setEditable(false);
        ui->comboBoxCameraType1->setEditable(false);

        ui->labelCameraDevice1->setText(QString::fromLocal8Bit("设备"));
        ui->labelCameraDevice1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1->setText(QString::fromLocal8Bit("端口"));
        ui->labelCameraPort1->setAlignment(Qt::AlignCenter);
        ui->labelCameraType1->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1_2->setText(QString::fromLocal8Bit(""));
        ui->labelCameraPort1_2->setAlignment(Qt::AlignCenter);

//        ui->comboBoxCameraDevice1->clear();
//        ui->comboBoxCameraPort1->clear();
//        ui->comboBoxCameraType1->clear();
//        ui->comboBoxCameraPort1_2->clear();
    }
    else if(arg1 == "GigEVision2")
    {
        ui->comboBoxCameraPort1->setEnabled(false);
        ui->comboBoxCameraType1->setEnabled(false);
        ui->comboBoxCameraPort1_2->setEnabled(false);
        ui->comboBoxImageWidth1->setEnabled(false);
        ui->comboBoxImageHeight1->setEnabled(false);
        ui->comboBoxDataLength1->setEnabled(false);
        ui->comboBoxImageChannel1->setEnabled(false);

        ui->comboBoxCameraPort1->setEditable(false);
        ui->comboBoxCameraType1->setEditable(false);

        ui->labelCameraDevice1->setText(QString::fromLocal8Bit("设备"));
        ui->labelCameraDevice1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1->setText(QString::fromLocal8Bit("端口"));
        ui->labelCameraPort1->setAlignment(Qt::AlignCenter);
        ui->labelCameraType1->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1_2->setText(QString::fromLocal8Bit(""));
        ui->labelCameraPort1_2->setAlignment(Qt::AlignCenter);

//        ui->comboBoxCameraDevice1->clear();
//        ui->comboBoxCameraPort1->clear();
//        ui->comboBoxCameraType1->clear();
//        ui->comboBoxCameraPort1_2->clear();
    }
    else if(arg1 == "UserDefined")
    {
        ui->comboBoxCameraPort1->setEnabled(true);
        ui->comboBoxCameraType1->setEnabled(true);
        ui->comboBoxCameraPort1_2->setEnabled(true);
        ui->comboBoxImageWidth1->setEnabled(true);
        ui->comboBoxImageHeight1->setEnabled(true);
        ui->comboBoxDataLength1->setEnabled(true);
        ui->comboBoxImageChannel1->setEnabled(true);

        ui->comboBoxCameraPort1->setEditable(true);
        ui->comboBoxCameraType1->setEditable(true);

//        ui->comboBoxCameraPort1->addItem(QString("5001"));
//        ui->comboBoxCameraType1->addItem(QString("192.168.1.10"));
//        ui->comboBoxCameraPort1_2->addItem(QString("5001"));

//        qDebug() << "asdfasdf";

        ui->labelCameraDevice1->setText(QString::fromLocal8Bit("本机IP"));
        ui->labelCameraDevice1->setAlignment(Qt::AlignCenter);
        ui->labelCameraType1->setText(QString::fromLocal8Bit("相机IP"));
        ui->labelCameraType1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1->setText(QString::fromLocal8Bit("本机端口"));
        ui->labelCameraPort1->setAlignment(Qt::AlignCenter);
        ui->labelCameraPort1_2->setText(QString::fromLocal8Bit("相机端口"));
        ui->labelCameraPort1_2->setAlignment(Qt::AlignCenter);

//        ui->comboBoxCameraDevice1->clear();
//        ui->comboBoxCameraPort1->clear();
//        ui->comboBoxCameraType1->clear();
//        ui->comboBoxCameraPort1_2->clear();
    }
}

void MainWindow::on_comboBoxCameraInterface2_currentTextChanged(const QString &arg1)
{
    if(arg1 == USBCameraInterface)
    {
        ui->comboBoxCameraPort2->setEnabled(true);
        ui->comboBoxCameraType2->setEnabled(true);
        ui->comboBoxCameraPort2->setEditable(false);
        ui->comboBoxCameraType2->setEditable(false);
        ui->labelCameraType2->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType2->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice2->clear();
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
    }
    else if(arg1 == "GigEVision2")
    {
        ui->comboBoxCameraPort2->setEnabled(false);
        ui->comboBoxCameraType2->setEnabled(false);
        ui->comboBoxCameraPort2->setEditable(false);
        ui->comboBoxCameraType2->setEditable(false);
        ui->labelCameraType2->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType2->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice2->clear();
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
    }
    else if(arg1 == "UserDefined")
    {
        ui->comboBoxCameraPort2->setEnabled(true);
        ui->comboBoxCameraType2->setEnabled(true);
        ui->labelCameraType2->setText(QString::fromLocal8Bit("IP地址"));
        ui->labelCameraType2->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice2->clear();
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
        ui->comboBoxCameraPort2->setEditable(true);
        ui->comboBoxCameraType2->setEditable(true);
    }
}

void MainWindow::on_comboBoxCameraInterface3_currentTextChanged(const QString &arg1)
{
    if(arg1 == USBCameraInterface)
    {
        ui->comboBoxCameraPort3->setEnabled(true);
        ui->comboBoxCameraType3->setEnabled(true);
        ui->comboBoxCameraPort3->setEditable(false);
        ui->comboBoxCameraType3->setEditable(false);
        ui->labelCameraType3->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType3->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice3->clear();
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraType3->clear();
    }
    else if(arg1 == "GigEVision2")
    {
        ui->comboBoxCameraPort3->setEnabled(false);
        ui->comboBoxCameraType3->setEnabled(false);
        ui->comboBoxCameraPort3->setEditable(false);
        ui->comboBoxCameraType3->setEditable(false);
        ui->labelCameraType3->setText(QString::fromLocal8Bit("类型"));
        ui->labelCameraType3->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice3->clear();
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraType3->clear();
    }
    else if(arg1 == "UserDefined")
    {
        ui->comboBoxCameraPort3->setEnabled(true);
        ui->comboBoxCameraType3->setEnabled(true);
        ui->labelCameraType3->setText(QString::fromLocal8Bit("IP地址"));
        ui->labelCameraType3->setAlignment(Qt::AlignCenter);
        ui->comboBoxCameraDevice3->clear();
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraPort3->setEditable(true);
        ui->comboBoxCameraType3->clear();
        ui->comboBoxCameraType3->setEditable(true);
    }
}

void MainWindow::on_comboBoxCameraDevice1_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface1->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraPort1->clear();
        ui->comboBoxCameraType1->clear();
        QVector<int> portList = HalconCamera::getAvailablePort(text);
        for(int i = 0; i < portList.length(); i++)
        {
            ui->comboBoxCameraPort1->insertItem(i+1, QString::number(portList[i]));
        }
    }
    else if(ui->comboBoxCameraInterface1->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraPort1->clear();
        ui->comboBoxCameraType1->clear();
    }
    else if(ui->comboBoxCameraInterface1->currentText() == "UserDefined")
    {
//        ui->comboBoxCameraPort1->clear();
//        ui->comboBoxCameraType1->clear();
    }
}

void MainWindow::on_comboBoxCameraPort1_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface1->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraType1 ->clear();
        QVector<QString> typeList = HalconCamera::getAvailableCameraType(ui->comboBoxCameraDevice1->currentText(), text.toInt());
        for(int i = 0; i < typeList.length(); i++)
        {
            ui->comboBoxCameraType1->insertItem(i+1, typeList[i]);
        }
    }
    else if(ui->comboBoxCameraInterface1->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraType1 ->clear();
    }
}

void MainWindow::on_comboBoxCameraDevice2_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface2->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
        QVector<int> portList = HalconCamera::getAvailablePort(text);
        for(int i = 0; i < portList.length(); i++)
        {
            ui->comboBoxCameraPort2->insertItem(i+1, QString::number(portList[i]));
        }
    }
    else if(ui->comboBoxCameraInterface2->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
    }
    else if(ui->comboBoxCameraInterface2->currentText() == "UserDefined")
    {
        ui->comboBoxCameraPort2->clear();
        ui->comboBoxCameraType2->clear();
    }
}

void MainWindow::on_comboBoxCameraPort2_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface2->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraType2 ->clear();
        QVector<QString> typeList = HalconCamera::getAvailableCameraType(ui->comboBoxCameraDevice2->currentText(), text.toInt());
        for(int i = 0; i < typeList.length(); i++)
        {
            ui->comboBoxCameraType2->insertItem(i+1, typeList[i]);
        }
    }
    else if(ui->comboBoxCameraInterface2->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraType2 ->clear();
    }
}

void MainWindow::on_comboBoxCameraDevice3_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface3->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraType3->clear();
        QVector<int> portList = HalconCamera::getAvailablePort(text);
        for(int i = 0; i < portList.length(); i++)
        {
            ui->comboBoxCameraPort3->insertItem(i+1, QString::number(portList[i]));
        }
    }
    else if(ui->comboBoxCameraInterface3->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraType3->clear();
    }
    else if(ui->comboBoxCameraInterface3->currentText() == "UserDefined")
    {
        ui->comboBoxCameraPort3->clear();
        ui->comboBoxCameraType3->clear();
    }
}

void MainWindow::on_comboBoxCameraPort3_currentTextChanged(const QString &text1)
{
    if(ui->comboBoxCameraInterface3->currentText() == USBCameraInterface)
    {
        QString text = text1;
        ui->comboBoxCameraType3 ->clear();
        QVector<QString> typeList = HalconCamera::getAvailableCameraType(ui->comboBoxCameraDevice3->currentText(), text.toInt());
        for(int i = 0; i < typeList.length(); i++)
        {
            ui->comboBoxCameraType3->insertItem(i+1, typeList[i]);
        }
    }
    else if(ui->comboBoxCameraInterface3->currentText() == "GigEVision2")
    {
        ui->comboBoxCameraType3 ->clear();
    }
}
