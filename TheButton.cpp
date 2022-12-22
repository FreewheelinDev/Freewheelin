
#include <QDebug>

#include "TheButton.h"

int TheButton::index = -1;  // which one is clicked, -1 means none

TheButton::TheButton(QWidget *parent, QUrl *url, QImage *icon, QLabel *title, QLabel *duration) : QWidget(parent) {

    this->url = url;
    this->icon = icon;
    this->title = title;
    this->duration = duration;
    auto *imageWidget = new QWidget(this);
    auto *image = new QLabel(imageWidget);
    image->setPixmap(QPixmap::fromImage(*icon));
    image->setScaledContents(true);
    image->setFixedSize(101,57);
    imageWidget->setFixedSize(101,57);
    auto *layout = new QVBoxLayout(this);
//    this->title->setStyleSheet("background-color: #d0d0d1;");
    layout->addWidget(this->title);
    layout->addWidget(imageWidget);
//    this->duration->setStyleSheet("background-color: #d0d0d1;");
    layout->addWidget(this->duration);
    layout->setSpacing(0);

    // margin
    layout->setContentsMargins(0,0,0,0);


//    auto *rWidget = new QWidget(this);
//    auto *rLayout = new QVBoxLayout(rWidget);
//    rLayout->addWidget(title);
//    rLayout->addWidget(duration);
//    auto *layout = new QHBoxLayout(this);
//    layout->addWidget(imageWidget);
//    layout->addWidget(rWidget);
    this->setMinimumSize(101, 100);
    this->setMaximumSize(101, 100);

//    this->bottomWidget = new QWidget(this);
//    bottomWidget->setStyleSheet("background-color: #d0d0d1; border-radius: 15px; ");
//    bottomWidget->setGeometry(this->geometry());
//    bottomWidget->stackUnder(imageWidget);
}

void TheButton::mouseReleaseEvent(QMouseEvent *event) {
    emit clicked();
}
