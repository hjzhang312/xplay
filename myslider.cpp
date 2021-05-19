#include "myslider.h"
#include <QDebug>

mySlider::mySlider(QWidget *parent) :
    QSlider(parent)
{

}

void mySlider::mousePressEvent(QMouseEvent *e)
{
   // QSlider::mousePressEvent(event);
              double pos = e->pos().x() / (double)width();
              int value = pos * (maximum() - minimum()) + minimum();
              setValue(value);
              qDebug() << "mousePro: " << value;

    //int value = ((float)e->pos().x() / (float)this->width())*(this->maximum() +1);
    //this->setValue(value);
    QSlider::mousePressEvent(e);
}

