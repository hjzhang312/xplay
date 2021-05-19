/********************************************************************************
** Form generated from reading UI file 'xplay.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XPLAY_H
#define UI_XPLAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>
#include <myslider.h>
#include "videowidget.h"

QT_BEGIN_NAMESPACE

class Ui_xplay
{
public:
    VideoWidget *openGLWidget;
    QPushButton *openBtn;
    QLabel *totaltime;
    QLabel *sp;
    QLabel *curtime;
    mySlider *horizontalSlider;

    void setupUi(QWidget *xplay)
    {
        if (xplay->objectName().isEmpty())
            xplay->setObjectName(QStringLiteral("xplay"));
        xplay->resize(800, 600);
        openGLWidget = new VideoWidget(xplay);
        openGLWidget->setObjectName(QStringLiteral("openGLWidget"));
        openGLWidget->setEnabled(true);
        openGLWidget->setGeometry(QRect(9, 9, 800, 600));
        openBtn = new QPushButton(xplay);
        openBtn->setObjectName(QStringLiteral("openBtn"));
        openBtn->setGeometry(QRect(460, 540, 99, 41));
        totaltime = new QLabel(xplay);
        totaltime->setObjectName(QStringLiteral("totaltime"));
        totaltime->setGeometry(QRect(50, 536, 67, 31));
        sp = new QLabel(xplay);
        sp->setObjectName(QStringLiteral("sp"));
        sp->setGeometry(QRect(110, 542, 16, 17));
        curtime = new QLabel(xplay);
        curtime->setObjectName(QStringLiteral("curtime"));
        curtime->setGeometry(QRect(120, 542, 67, 17));
        horizontalSlider = new mySlider(xplay);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setGeometry(QRect(40, 480, 681, 29));
        horizontalSlider->setOrientation(Qt::Horizontal);

        retranslateUi(xplay);

        QMetaObject::connectSlotsByName(xplay);
    } // setupUi

    void retranslateUi(QWidget *xplay)
    {
        xplay->setWindowTitle(QApplication::translate("xplay", "xplay", 0));
        openBtn->setText(QApplication::translate("xplay", "open", 0));
        totaltime->setText(QApplication::translate("xplay", "00:00:00", 0));
        sp->setText(QApplication::translate("xplay", "/", 0));
        curtime->setText(QApplication::translate("xplay", "00:00:00", 0));
    } // retranslateUi

};

namespace Ui {
    class xplay: public Ui_xplay {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XPLAY_H
