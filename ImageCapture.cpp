#include "ImageCapture.h"
#include <QImage>
#include <QFile>
#include <QTimer>
#include <QString>
#include <QDebug>

//using namespace cv;

ImageCapture::ImageCapture( QObject * parent )
	:QObject(parent)
	,_timer(NULL)
	,_cvCap(NULL)
	,_processImage(false)
    ,_trackBall(true)
    ,_pStrStorage(0)
    ,m_iMinHue(1)
    ,m_iMinSaturation(151)
    ,m_iMinValue(29)
    ,m_iMaxHue(13)
    ,m_iMaxSaturation(256)
    ,m_iMaxValue(256)
{
	this->_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(timerTick()));
}

ImageCapture::~ImageCapture(){
	this->stopCapture();
}

void ImageCapture::stopCapture(){
	this->_timer->stop();
	cvReleaseCapture(&_cvCap);
}

void ImageCapture::setMinHue(int hue)
{
    m_iMinHue=hue;
}
void ImageCapture::setMinSaturation(int saturation)
{
    m_iMinSaturation=saturation;
}
void ImageCapture::setMinValue(int value)
{
    m_iMinValue=value;
}
void ImageCapture::setMaxHue(int hue)
{
    m_iMaxHue=hue;
}
void ImageCapture::setMaxSaturation(int saturation)
{
    m_iMaxSaturation=saturation;
}
void ImageCapture::setMaxValue(int value)
{
    m_iMaxValue=value;
}

// if you pass cameraIndex=-1 (default), OpenCV will popup a simple
// camera selection widget
void ImageCapture::captureFromCamera( int cameraIndex ){
    this->stopCapture();
    this->_cvCap = cvCaptureFromCAM(cameraIndex);
	if( _cvCap ){
        this->_timer->setInterval(200);
		this->_timer->start();
	} else{
        emit error( tr("Invalid camera index: %1").arg(cameraIndex) );
    }
}

void ImageCapture::timerTick()
{
    float *p_fltXYRadius;

	IplImage * image = cvQueryFrame(this->_cvCap);
    if( image )
    {
        if (_processImage)
        {
            if (_trackBall)
            {
                const CvSize s = cvSize(image->width,image->height);
                IplImage * p_imgHSV = cvCreateImage(s, image->depth,3);
                IplImage * p_imgProcessed = cvCreateImage(s, image->depth, 1);
                cvCvtColor(image,p_imgHSV,CV_BGR2HSV);
                cvInRangeS(p_imgHSV,
                           cvScalar(m_iMinHue, m_iMinSaturation, m_iMinValue),
                           cvScalar(m_iMaxHue, m_iMaxSaturation, m_iMaxValue),
                           p_imgProcessed);
                _pStrStorage = cvCreateMemStorage(0);

                cvSmooth(p_imgProcessed,
                         p_imgProcessed,
                         CV_GAUSSIAN,
                         7,
                         7);

                _pSeqCircles = cvHoughCircles(p_imgProcessed,
                                              _pStrStorage,
                                              CV_HOUGH_GRADIENT,
                                              2,
                                              p_imgProcessed->height/4,
                                              100,
                                              90,
                                              10,
                                              100);

                for (int i=0;i<_pSeqCircles->total;i++)
                {
                    p_fltXYRadius=(float*)cvGetSeqElem(_pSeqCircles,i);

                    qDebug() << cvRound(p_fltXYRadius[0]) << cvRound(p_fltXYRadius[1]);

                    cvCircle(image,
                             cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
                            5,
                            CV_RGB(0,255,0),
                            CV_FILLED);

                    cvCircle(image,
                             cvPoint(cvRound(p_fltXYRadius[0]),cvRound(p_fltXYRadius[1])),
                            cvRound(p_fltXYRadius[2]),
                            CV_RGB(255,0,0),
                            3);
                }
                emit imageReal( convert(image) );
                emit imageCaptured( convert(p_imgProcessed ));

                cvReleaseImage(&p_imgHSV);
                cvReleaseImage(&p_imgProcessed);
                cvReleaseMemStorage(&_pStrStorage);
            }
        }
        else
        {
            emit imageCaptured( convert(image) );
        }
    }
}

void ImageCapture::enableProcessing(bool on){
	_processImage = on;
}

void ImageCapture::trackBall(bool bTrack)
{
   _trackBall = bTrack;
}

QImage*  ImageCapture::convert(IplImage *iplImg)
{
    int h = iplImg->height;
    int w = iplImg->width;
    int channels = iplImg->nChannels;
    QImage *qimg = new QImage(w, h, QImage::Format_ARGB32);
    char *data = iplImg->imageData;

    for (int y = 0; y < h; y++, data += iplImg->widthStep)
    {
        for (int x = 0; x < w; x++)
        {
            char r, g, b, a = 0;
            if (channels == 1)
            {
                r = data[x * channels];
                g = data[x * channels];
                b = data[x * channels];
            }
            else if (channels == 3 || channels == 4)
            {
                r = data[x * channels + 2];
                g = data[x * channels + 1];
                b = data[x * channels];
            }

            if (channels == 4)
            {
                a = data[x * channels + 3];
                qimg->setPixel(x, y, qRgba(r, g, b, a));
            }
            else
            {
                qimg->setPixel(x, y, qRgb(r, g, b));
            }
        }
    }
    return qimg;
}
