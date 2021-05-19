#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class mySlider : public QSlider
{
public:
    mySlider(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *e);
};

#endif // MYSLIDER_H
