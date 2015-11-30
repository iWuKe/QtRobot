#include <QDebug>

#include <stdlib.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"


inline int max ( int a, int b ) { return a > b ? a : b; }
inline int min ( int a, int b ) { return a < b ? a : b; }


using namespace cv;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->plot->addGraph();
    ui->plot->addGraph();
    ui->plot->xAxis->setLabel("<-- Time -->");
    ui->plot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->plot->xAxis->setAutoTickStep(true);
    ui->plot->xAxis->setNumberFormat("gb");
    ui->plot->xAxis->setTickLabelColor(Qt::black);
    ui->plot->xAxis->axisRect()->setupFullAxesBox();

    ui->plot->graph(1)->setPen(QPen(Qt::red));


    ui->speed_1->setSkin("Tachometer");
    ui->speed_1->setNeedleOrigin(0.486, 0.466);
    ui->speed_1->setMinimum(0);
    ui->speed_1->setMaximum(32);
    ui->speed_1->setStartAngle(-130);
    ui->speed_1->setEndAngle(133);
    ui->speed_1->setValue(0);
    ui->speed_1->setShowOverlay(true);

    ui->speed_2->setSkin("Tachometer");
    ui->speed_2->setNeedleOrigin(0.486, 0.466);
    ui->speed_2->setMinimum(0);
    ui->speed_2->setMaximum(32);
    ui->speed_2->setStartAngle(-130);
    ui->speed_2->setEndAngle(133);
    ui->speed_2->setValue(0);
    ui->speed_2->setShowOverlay(true);

    ui->joystick->setEnabled(false);

    ui->sliderScMin1->setValue(0);
    ui->sliderScMin2->setValue(0);
    ui->sliderScMin3->setValue(175);

    ui->sliderScMax1->setValue (256);
    ui->sliderScMax2->setValue (256);
    ui->sliderScMax3->setValue (256);

    m_pUsbBufferTx = new quint8[USB_BUFFER_LEN];
    m_pUsbBufferRx = new quint8[USB_BUFFER_LEN];


    m_pCaptureThrd = new captureThread(this);
    connect (m_pCaptureThrd, SIGNAL (imageReady(QImage, QImage)), this, SLOT (on_drawFrames(QImage, QImage)));
    m_pCaptureThrd->start();

    ui->sliderScMin1->setValue(0);
    ui->sliderScMin2->setValue(0);
    ui->sliderScMin3->setValue(175);

    ui->sliderScMax1->setValue(114);
    ui->sliderScMax2->setValue(163);
    ui->sliderScMax3->setValue(256);

    connect (ui->sliderScMax1,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));
    connect (ui->sliderScMax2,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));
    connect (ui->sliderScMax3,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));
    connect (ui->sliderScMin1,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));
    connect (ui->sliderScMin2,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));
    connect (ui->sliderScMin3,SIGNAL (valueChanged(int)), this, SLOT(on_sliderValueChanged()));

    on_sliderValueChanged();

    m_pTimer = new QTimer (this);
    connect (m_pTimer,SIGNAL (timeout()), this, SLOT (on_timeout()));
    m_pTimer->start (1000);

/*    m_pTimer3 = new QTimer (this);
    connect (m_pTimer3,SIGNAL (timeout()),this,SLOT (on_drawFrames()),Qt::DirectConnection);
    m_pTimer3->start(20);*/


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_sliderValueChanged()
{
    int scMin1 = ui->sliderScMin1->value();
    int scMin2 = ui->sliderScMin2->value();
    int scMin3 = ui->sliderScMin3->value();

    int scMax1 = ui->sliderScMax1->value();
    int scMax2 = ui->sliderScMax2->value();
    int scMax3 = ui->sliderScMax3->value();

    m_pCaptureThrd->setRange (scMin1,scMin2,scMin3,scMax1,scMax2,scMax3);
}

void MainWindow::resetRobot()
{
    memset (m_pUsbBufferTx, 0, USB_BUFFER_LEN);
    memset (m_pUsbBufferRx, 0, USB_BUFFER_LEN);

    m_libUsb.send (m_pUsbBufferTx, USB_BUFFER_LEN);
}

void MainWindow::errorUsb()
{
    ui->led_usb->setPixmap(QPixmap (QString::fromUtf8(":/led_off")));
    m_pTimer->start(1000);
    m_libUsb.usbRelease();
    ui->joystick->setEnabled(false);

}

void MainWindow::getUsb()
{
    int velocidad=0;
    int encoder1=0;
    int encoder2=0;

    if (m_libUsb.get (m_pUsbBufferRx,USB_BUFFER_LEN))
    {
        //Error de USB
        //Posiblemente el SUB se ha desconectado.
        errorUsb();
    }
    else
    {
        velocidad = m_pUsbBufferRx[0] << 8;
        velocidad |= m_pUsbBufferRx[1];

        encoder1 = m_pUsbBufferRx[2] << 8;
        encoder1 |= m_pUsbBufferRx[3];

        encoder2 = m_pUsbBufferRx[4] << 8;
        encoder2 |= m_pUsbBufferRx[5];

        ui->speed_2->setValue(encoder1);
        ui->speed_1->setValue(encoder2);

        QCustomPlot *graphPlot = (QCustomPlot*) (ui->plot);
        range_y_min = min(range_y_min,encoder1);
        range_y_max = max(range_y_max,encoder1);
        graphPlot->graph(0)->addData(m_x, encoder1);
        graphPlot->graph(1)->addData(m_x, encoder2);

        graphPlot->xAxis->setRange(m_x+0.5, 50, Qt::AlignRight);
        graphPlot->yAxis->setRange(range_y_min,range_y_max);
        graphPlot->replot();
        m_x += 0.5;
    }
}

void MainWindow::on_timeout()
{
    if (!m_libUsb.isOpen())
    {
        if (!m_libUsb.open (0x04d8, 0x4541))
        {
            ui->led_usb->setPixmap(QPixmap (QString::fromUtf8(":/led_on")));
            resetRobot();
            m_pTimer->start(100);
            ui->joystick->setEnabled(true);
        }
    }
    else
    {
        getUsb();
    }
}

void MainWindow::on_drawFrames(QImage original, QImage processed)
{
    ui->lblOriginal->setPixmap(QPixmap::fromImage(original));
    ui->lblProcessed->setPixmap(QPixmap::fromImage(processed));
}

/*void MainWindow::on_processFrameAndUpdateGUI()
{
    capWebcam.read (matOriginal);
    if (matOriginal.empty()==true)
        return;

    //usleep(30000);

    cv::inRange(matOriginal, cv::Scalar(0,0,175), cv::Scalar(100,100,256), matProcessed);

    cv::GaussianBlur(matProcessed, matProcessed, cv::Size(9,9),1.5);
    cv::HoughCircles(matProcessed, vecCircles, CV_HOUGH_GRADIENT, 2, matProcessed.rows / 4, 100, 50, 10, 400);

    for (itrCircles = vecCircles.begin(); itrCircles != vecCircles.end(); itrCircles++)
    {
        ui->plainTextEdit->appendPlainText(QString("ball position x = ")+QString::number((*itrCircles)[0]).rightJustified(4,' ') +
                QString (", y = ") + QString::number ((*itrCircles)[1]).rightJustified(4,' ') +
                QString (", radius = ") + QString::number ((*itrCircles)[2], 'f', 3).rightJustified(7,' '));

        cv::circle(matOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
        cv::circle(matOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), (int)(*itrCircles)[2], cv::Scalar(0,0,255),3);
    }
    cv::cvtColor (matOriginal, matOriginal, CV_BGR2RGB);


    qimOriginal = QImage ((uchar *)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);
    qimProcessed = QImage ((uchar *)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_Indexed8);
}*/


void MainWindow::setSpeed (int speed1, int speed2)
{
    int bPid = 1;

    m_pUsbBufferTx[VELOCIDAD_H]=speed1>>8;
    m_pUsbBufferTx[VELOCIDAD_L]=speed1&0x00ff;

    m_pUsbBufferTx[MOTOR_LEFT_H]=speed1>>8;
    m_pUsbBufferTx[MOTOR_LEFT_L]=speed1&0x00ff;

    m_pUsbBufferTx[MOTOR_RIGHT_H]=speed2>>8;
    m_pUsbBufferTx[MOTOR_RIGHT_L]=speed2&0x00ff;

    m_pUsbBufferTx[BPID_H]=bPid>>8;
    m_pUsbBufferTx[BPID_L]=bPid&0x00ff;

    m_libUsb.send (m_pUsbBufferTx, USB_BUFFER_LEN);
}

void MainWindow::on_button_up_pressed()
{
    int speed = ui->slider->value();
    setSpeed (speed,speed);
}

void MainWindow::on_button_up_released()
{
    setSpeed (0,0);
}
