#include "videowidget.h"

#include <QPainter>
#include <QImage>
#include "xffmpeg.h"
#include <QDebug>

VideoWidget::VideoWidget(QWidget *parent) :
    QOpenGLWidget(parent)
{

}

void VideoWidget::showImage()
{
    //startTimer(20);
    this->update();
}


void VideoWidget::paintEvent(QPaintEvent *e)
{
#if 1
    static QImage *image = NULL;
    static uint mWidth = 0;
    static uint mHeight = 0;
   // qDebug() << "mWidth: " << mWidth << "mHeight: " << mHeight;
    if(mWidth != this->width() || mHeight != this->height())
    {
        if(image)
        {
            delete image->bits();
            delete image;
            image = NULL;
        }
    }
    mWidth = this->width();
    mHeight = this->height();
    if(NULL == image)
    {
        uchar *buf = new uchar[width() * height() * 4];
            image = new QImage(buf, width(), height(), QImage::Format_ARGB32);
            //QImage 可能创建一个空的字符会引发段错误
     //   image = new QImage(mWidth,mHeight,QImage::Format_ARGB32);

       // uchar *buf = new uchar[mWidth*mHeight * 4];
       // image = new QImage(buf, mWidth, mHeight, QImage::Format_ARGB32);
    }
#if 0
    //qDebug() << "av_packet_unref qian";
    AVPacket pkt = XFFmpeg::Get()->Read();
    if (pkt.stream_index != XFFmpeg::Get()->mVideoStream)
    {
            av_packet_unref(&pkt);
            return;
    }
    qDebug() << "av_packet_unref";
    if (pkt.size == 0) return;
    AVFrame *yuv = XFFmpeg::Get()->Decode(&pkt);
    av_packet_unref(&pkt);
    if (yuv == NULL) return;

#endif
    XFFmpeg::Get()->ToRGB((char*)image->bits(),mWidth,mHeight);

    QPainter painter(this);
   // painter.begin(this);
    painter.drawImage(QPoint(0,0),*image);
   // painter.end();

#endif
}

void VideoWidget::timerEvent(QTimerEvent *e)
{

    this->update();
}
/*
void VideoWidget::resizeGL(int w, int h)
{
     glViewport( 0, 0, (GLint)w, (GLint)h );
     glMatrixMode( GL_PROJECTION );
     glLoadIdentity();
     //gluPerspective( 45.0, (GLfloat)width/(GLfloat)height, 0.1, 100.0 );
     glMatrixMode( GL_MODELVIEW );
     glLoadIdentity();
}
*/
