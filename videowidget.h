#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QOpenGLWidget>

class VideoWidget : public QOpenGLWidget
{
public:
    VideoWidget(QWidget *parent = 0);
    void showImage();

protected:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    //void resizeGL(int w,int h);
};

#endif // VIDEOWIDGET_H
