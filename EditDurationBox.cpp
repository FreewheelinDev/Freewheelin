#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QSlider>
#include "EditDurationBox.h"

EditDurationBox::EditDurationBox(const QString &duration, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Edit Duration");
    setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);
    setFixedSize(500, 150);
    // set the style bg as #232323
    // set the style of the text as #A2A2A2
    setStyleSheet("background-color: #232323; color: #A2A2A2;");

    auto *layout = new QVBoxLayout(this);
    auto *editArea = new QWidget(this);
    auto *editLayout = new QHBoxLayout(editArea);
    auto *label1 = new QLabel("00:00", editArea);
    auto *slider = new QSlider(Qt::Horizontal, editArea);
    slider->setRange(0, 100);
    slider->setValue(0);
    slider->setStyleSheet("QSlider::groove:horizontal {"
                          "height: 6px;"
                          "border:1px;"
                          "border-radius: 2px;"
                          "background: #3D3E3E;"

                          "}"

                          "QSlider::handle:horizontal {"
                          "background:#E7E5E8;"
                          "width: 2px;"
                          "height: 13px;"
                          "margin-top: -18px;"
                          "margin-bottom: -18px;"
                          "border-radius: 6px;"
                          "}"

                          "QSlider::sub-page:horizontal {"
                          "background:#565656;"
                          "height: 6px;"
                          "border: 1px;"
                          "border-radius: 2px;"
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
    auto *saveButton = new QPushButton("Save", buttonArea);
    auto *cancelButton = new QPushButton("Cancel", buttonArea);

    saveButton->setStyleSheet("QPushButton{"
                                "background-color:#616161;"
                                "border-radius:5px;"
                                "color:#DDDDDD;"
                                "width:68px;"
                                "height:17px;"
                                "}"
                                "QPushButton:hover{"
                                "background-color: rgb(61, 62, 62);"
                                "border-radius:5px;"
                                "color:#DDDDDD;"
                                "width:68px;"
                                "height:17px;"
                                "}");
    // set the save and cancel button style
    cancelButton->setStyleSheet("QPushButton{"
                                "background-color:#1373E6;"
                                "border-radius:5px;"
                                "color:white;"
                                "width:68px;"
                                "height:17px;"
                                "}"
                                "QPushButton:hover{"
                                "background-color:#0F66d0;"
                                "border-radius:5px;"
                                "color:#DDDDDD;"
                                "width:68px;"
                                "height:17px;"
                                "}");
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
