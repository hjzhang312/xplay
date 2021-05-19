#ifndef XPLAY_H
#define XPLAY_H

#include <QWidget>
#include <QResizeEvent>

namespace Ui {
class xplay;
}

class xplay : public QWidget
{
    Q_OBJECT

public:
    explicit xplay(QWidget *parent = 0);
    static xplay *Get()
    {
        static xplay vt;
        return &vt;
    }
    void openAVFile(const char *path);

    void showGlWindow();
    ~xplay();
public slots:
    void btnSlots();
    void mysliderRelsease();
private:
    Ui::xplay *ui;
public:
    void resizeEvent(QResizeEvent *e);
};

#endif // XPLAY_H
