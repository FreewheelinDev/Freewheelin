
#ifndef FREEWHEELIN_THEBUTTON_H
#define FREEWHEELIN_THEBUTTON_H

#include <QPushButton>
#include <QUrl>
#include <QLabel>
#include <QVBoxLayout>

class TheButton : public QWidget{
Q_OBJECT

public:
    static int index;

    QUrl *url; // video file to play
    QImage *icon; // icon to display
    QLabel *title; // title to display
    QLabel *duration; // duration to display
    QWidget* bottomWidget; // bottom widget to display

    TheButton(QWidget *parent, QUrl *url, QImage *icon, QLabel *title, QLabel *duration);

protected:
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void clicked();
};


#endif //FREEWHEELIN_THEBUTTON_H
