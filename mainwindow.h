#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

#include "qlibusb.h"
#include "capturethread.h"

#define USB_BUFFER_LEN 	32

#define VELOCIDAD_H		0
#define VELOCIDAD_L		1
#define MOTOR_LEFT_H	2
#define MOTOR_LEFT_L	3
#define MOTOR_RIGHT_H	4
#define MOTOR_RIGHT_L	5
#define BPID_H			6
#define BPID_L			7

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void resetRobot();
    void getUsb();
    void errorUsb();
    void setSpeed (int speed1, int speed2);


private:
    Ui::MainWindow *ui;
    QlibUsb         m_libUsb;
    QTimer         *m_pTimer;
    quint8         *m_pUsbBufferTx;
    quint8         *m_pUsbBufferRx;
    double          m_x;
    double          m_y;
    double          range_y_min;
    double          range_y_max;
    captureThread  *m_pCaptureThrd;

/*    cv::VideoCapture capWebcam;
    cv::Mat          matOriginal;
    cv::Mat          matProcessed;

    QImage          qimOriginal;
    QImage          qimProcessed;*/

    QTimer          *m_pTimer3;


private slots:

    void on_timeout();
    void on_drawFrames(QImage, QImage);
    void on_button_up_pressed();
    void on_button_up_released();
    void on_sliderValueChanged();

};

#endif // MAINWINDOW_H
