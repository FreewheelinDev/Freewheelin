#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>
#include "EditDurationBox.h"

EditDurationBox::EditDurationBox(const QString& duration, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Edit Duration");
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setFixedSize(500, 150);
    auto *layout = new QVBoxLayout(this);
    auto *editArea = new QWidget(this);
    auto *editLayout = new QHBoxLayout(editArea);
    auto *label1 = new QLabel("00:00", editArea);
    auto *slider = new QSlider(Qt::Horizontal, editArea);
    slider->setRange(0, 100);
    slider->setValue(0);
    slider->setStyleSheet("QSlider::groove:horizontal {"
                          "border: 1px solid #bbb;"
                          "background: white;"
                          "height: 10px;"
                          "border-radius: 4px;"
                          "}"
                          "QSlider::sub-page:horizontal {"
                          "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,"
                          "stop: 0 #66e, stop: 1 #bbf);"
                          "background: qlineargradient(x1: 0, y1: 0.2, x2: 1, y2: 1,"
                          "stop: 0 #bbf, stop: 1 #55f);"
                          "border: 1px solid #777;"
                          "height: 10px;"
                          "border-radius: 4px;"
                          "}"
                          "QSlider::add-page:horizontal {"
                          "background: #fff;"
                          "border: 1px solid #777;"
                          "height: 10px;"
                          "border-radius: 4px;"
                          "}"
                          "QSlider::handle:horizontal {"
                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                          "stop:0 #eee, stop:1 #ccc);"
                          "border: 1px solid #777;"
                          "width: 13px;"
                          "margin-top: -2px;"
                          "margin-bottom: -2px;"
                          "border-radius: 4px;"
                          "}"
                          "QSlider::handle:horizontal:hover {"
                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                          "stop:0 #fff, stop:1 #ddd);"
                          "border: 1px solid #444;"
                          "border-radius: 4px;"
                          "}"
                          "QSlider::sub-page:horizontal:disabled {"
                          "background: #bbb;"
                          "border-color: #999;"
                          "}"
                          "QSlider::add-page:horizontal:disabled {"
                          "background: #eee;"
                          "border-color: #999;"
                          "}");
    connect(slider, &QSlider::valueChanged, [=]() {
        // calculate the seconds
        int totalSeconds = duration.mid(0, 2).toInt() * 60 + duration.mid(3, 2).toInt();
        int seconds = totalSeconds * slider->value() / 100;
        int minutes = seconds / 60;
        seconds = seconds % 60;
        label1->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    });
    auto *label2 = new QLabel(duration, editArea);
    editLayout->addWidget(label1);
    editLayout->addWidget(slider);
    editLayout->addWidget(label2);
    auto *buttonArea = new QWidget(this);
    auto *buttonLayout = new QHBoxLayout(buttonArea);
    auto *spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
    auto *cancelButton = new QPushButton("Cancel", buttonArea);
    auto *saveButton = new QPushButton("Save", buttonArea);
    buttonLayout->addItem(spacerItem);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(saveButton);
    // add a line divider
    auto *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(editArea);
    layout->addWidget(line);
    layout->addWidget(buttonArea);
    connect(cancelButton, &QPushButton::clicked, this, &EditDurationBox::cancelClicked);
    connect(saveButton, &QPushButton::clicked, this, &EditDurationBox::saveClicked);
}

EditDurationBox::~EditDurationBox() {
    delete this;
}

void EditDurationBox::saveClicked() {
    this->close();
}

void EditDurationBox::cancelClicked() {
    this->close();
}
