#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QImage>
#include <QThread>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

class captureThread : public QThread
{
    Q_OBJECT
public:
    explicit        captureThread(QObject *parent = 0);
    void            run();
    void            setRange (int scMin1, int scMin2, int scMin3, int scMax1, int scMax2, int scMax3);

private:
    void            setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour);

private:
    cv::VideoCapture                 capWebcam;
    cv::Mat                          matOriginal;
    cv::Mat                          matProcessed;
    std::vector<cv::Vec3f>           vecCircles;
    std::vector<cv::Vec3f>::iterator itrCircles;
    int                              m_scMin1;
    int                              m_scMin2;
    int                              m_scMin3;
    int                              m_scMax1;
    int                              m_scMax2;
    int                              m_scMax3;

public:
    QImage                           m_qimOriginal;
    QImage                           m_qimProcessed;

signals:
    void            imageReady(QImage ,QImage);

public slots:

};

#endif // CAPTURETHREAD_H
