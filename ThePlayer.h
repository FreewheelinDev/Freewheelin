
#ifndef FREEWHEELIN_THEPLAYER_H
#define FREEWHEELIN_THEPLAYER_H

#include <QApplication>
#include <QMediaPlayer>
#include "TheButton.h"
#include <vector>
#include <QTimer>

#include "TheButton.h"

class ThePlayer : public QMediaPlayer {

Q_OBJECT

private:
    std::vector<TheButton*>* buttons{};
    long updateCount = 0;

public:
    QTimer* mTimer;

    ThePlayer() : QMediaPlayer(nullptr) {}

    ~ThePlayer() override = default;

};


#endif //FREEWHEELIN_THEPLAYER_H
