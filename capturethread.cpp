#include <QDebug>
#include "capturethread.h"

captureThread::captureThread(QObject *parent) :
    QThread(parent)
{
    m_scMin1 = 0;
    m_scMin2 = 0;
    m_scMin3 = 175;

    m_scMax1 = 256;
    m_scMax2 = 256;
    m_scMax3 = 256;
}

void captureThread::setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
    int fontface = cv::FONT_HERSHEY_SIMPLEX;
    double scale = 0.4;
    int thickness = 1;
    int baseline = 0;

    cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
    cv::Rect r = cv::boundingRect(contour);

    cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
    cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
    cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

void captureThread::setRange (int scMin1, int scMin2, int scMin3, int scMax1, int scMax2, int scMax3)
{
    m_scMin1 = scMin1;
    m_scMin2 = scMin2;
    m_scMin3 = scMin3;

    m_scMax1 = scMax1;
    m_scMax2 = scMax2;
    m_scMax3 = scMax3;
}

void captureThread::run()
{
    int max_brightness = 256;

    capWebcam.open(-1);
    if (capWebcam.isOpened() == false)
        qDebug()<< "Error opening camera";

    //capWebcam.set(CV_CAP_PROP_FRAME_WIDTH,400);
    //capWebcam.set(CV_CAP_PROP_FRAME_HEIGHT,300);

    while (1)
    {
         // Get one frame
        for (int n=0;n<4;n++)
        {
            capWebcam.read (matOriginal);
            if (matOriginal.empty()==true)
                return;
        }

        IplImage* image=cvCloneImage(&(IplImage)matOriginal);

        int brightness = 0;
        int i, j;
        int r = 0, g = 0, b = 0;
        int steps = 0;

        for(i = 0; i < image->width; i++)
        {
            for(j = 0; j < image->height; j++)
            {
                CvScalar pixel = cvGet2D(image, j, i);
                r = pixel.val[2];
                g = pixel.val[1];
                b = pixel.val[0];

                brightness += 0.299 * r + 0.587 * g + 0.114 * b;
                steps++;
            }
        }

        int taux = brightness/((image->width) * (image->height));
        int lum = taux;
        if(lum > max_brightness) lum = max_brightness;
        if(lum < 0)  lum = 0;

        //qDebug() << lum;

        cv::inRange(matOriginal, cv::Scalar(m_scMin1,m_scMin2, m_scMin3), cv::Scalar(m_scMax1,m_scMax2,m_scMax3), matProcessed);
        cv::GaussianBlur(matProcessed, matProcessed, cv::Size(5,5),10);
        cv::HoughCircles(matProcessed, vecCircles, CV_HOUGH_GRADIENT, 1, matProcessed.rows, 100, 40, 0, 0);

        int h = image->height;
        int w = image->width;
        int y = 240;
        int x = 200;
        cv::rectangle( matOriginal,
                       cvPoint(x-50,y-50),
                       cvPoint(x+50,y+50),
                       CV_RGB(255,255,255), 1, 8
                     );

        cv::circle (matOriginal,cv::Point (x,y),3,cv::Scalar(0,255,0),CV_FILLED);

        for (itrCircles = vecCircles.begin(); itrCircles != vecCircles.end(); itrCircles++)
        {
            cv::circle(matOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), 3, cv::Scalar(0,255,0), CV_FILLED);
            cv::circle(matOriginal, cv::Point((int)(*itrCircles)[0], (int)(*itrCircles)[1]), (int)(*itrCircles)[2], cv::Scalar(0,0,255),3);
            qDebug() << (int)(*itrCircles)[0] <<  (int)(*itrCircles)[1] << (int)(*itrCircles)[2];
        }
        cv::cvtColor (matOriginal, matOriginal, CV_BGR2RGB);

        m_qimOriginal  = QImage ((uchar *)matOriginal.data, matOriginal.cols, matOriginal.rows, matOriginal.step, QImage::Format_RGB888);
        m_qimProcessed = QImage ((uchar *)matProcessed.data, matProcessed.cols, matProcessed.rows, matProcessed.step, QImage::Format_Indexed8);
        emit imageReady(m_qimOriginal,m_qimProcessed);

        cvReleaseImage (&image);
    }
}
