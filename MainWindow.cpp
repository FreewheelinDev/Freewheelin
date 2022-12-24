#include <iostream>

#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QDirIterator>
#include <QPixmap>
#include <QTime>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>
#include <thread>
#include <QThread>
#include <QPainter>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "EditDurationBox.h"

using namespace std;


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    // remove the status/menu bar to adapt the tablet
    this->setStatusBar(nullptr);
    this->setMenuBar(nullptr);
    this->setWindowTitle("FreeWheelin Studio");

    // set the minimum size of the window
    this->setMinimumSize(1024, 768);  // 1024x768 is the size of ipad mini(7.9 inch)

    centralWidget()->setMouseTracking(true);
    this->setMouseTracking(true);

    // initPic for welcome page
    setInitPic();

    // initialize the work space
    initWorkSpace();


    // set the height of the top bar, the bottom bar fixed; set background color
    ui->topBar->setFixedHeight(TOP_BAR_HEIGHT);
    ui->bottomBar->setFixedHeight(BOTTOM_BAR_HEIGHT);
    ui->topBar->setStyleSheet("background-color:#232323;");
    // top bar lower border-line as black

    ui->bottomBar->setStyleSheet("background-color:#232323");


    // initialize the setting button
    initSettingButton(); // set a svg setting button instead latter


    // ///////////// play related buttons ///////////////

//    ui->videoListButton->setStyleSheet("background-color:#454545;");
    // set border raidus and color #45454545
    ui->videoListButton->setIcon(QIcon(":/res/image/toggle-list.png"));
    ui->videoListButton->setIconSize(QSize(12, 12));
    ui->videoListButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                       "border-radius: 5px; background-color:#454545;margin-right: 3px;width:36px;height:21px;");


    ui->previousVideoButton->setIcon(QIcon(":/res/image/prev-video.png"));
    ui->previousVideoButton->setIconSize(QSize(12, 12));
    ui->previousVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                           "border-radius: 5px; background-color:#454545;margin-right: 3px;width:36px;height:21px;");


    // set the bg img for play button
    ui->playVideoButton->setIcon(QIcon(":/res/image/play.png"));
    // set the icon size
    ui->playVideoButton->setIconSize(QSize(12, 12));
    ui->playVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                       "border-radius: 5px; background-color:#505050;margin-right: 3px;width:36px;height:21px;");

    ui->nextVideoButton->setIcon(QIcon(":/res/image/next-video.png"));
    ui->nextVideoButton->setIconSize(QSize(12, 12));
    ui->nextVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                       "border-radius: 5px; background-color:#454545;margin-right: 3px;width:36px;height:21px;");


    // volume button is a square
    ui->volumeButton->setIcon(QIcon(":/res/image/volume-on.png"));
    ui->volumeButton->setIconSize(QSize(12, 12));
    ui->volumeButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                    "border-radius: 5px; background-color:#454545;margin-right: 3px;width: 20px;height: 20px;");


    ui->fullScreenButton->setIcon(QIcon(":/res/image/full-screen.png"));
    ui->fullScreenButton->setIconSize(QSize(12, 12));
    ui->fullScreenButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                        "border-radius: 5px; background-color:#454545;margin-left: 5px;width: 20px;height: 20px;");

    ui->speedButton->setStyleSheet("QPushButton{"
                                   "color: #a2a2a2; border-color:none;"
                                   "border-radius: 5px; background-color:#454545;margin-right: 3px; min-width: 38px; min-height: 20px;}"
                                   "QPushButton::menu-indicator{image: none;}");



    // ///////////// play related buttons ///////////////
    ui->terminateVideoButton->setIcon(QIcon(":/res/image/terminate_play.png"));
    ui->terminateVideoButton->setIconSize(QSize(8, 8));
    ui->terminateVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                            "border-radius: 5px; background-color:#454545;margin-right: 10px;width:21px;height:21px;");
    // set the border radius of the video list button



    // video progress slider
    ui->videoProgressSlider->setRange(0, 100);
    ui->videoProgressSlider->setValue(0);

    // Adobe PR style slider
    ui->videoProgressSlider->setStyleSheet(
            "QSlider::groove:horizontal {"
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
            "}"
    );

    // volume slider
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);

    // speed list
    auto *speedList = new QMenu(ui->speedButton);
    // style sheet
    speedList->setStyleSheet("QMenu{background-color:#454545; color:#a2a2a2; border:1px solid #454545;}"
                             "QMenu::item:selected{background-color:#696969; color:#a2a2a2; cursor: pointer;}");
    speedList->addAction("0.5x", this, [=]() {
        ui->speedButton->setText(" 0.5x");
        player->setPlaybackRate(0.5);
    });
    speedList->addAction("1.0x", this, [=]() {
        ui->speedButton->setText(" 1.0x");
        player->setPlaybackRate(1.0);
    });
    speedList->addAction("1.5x", this, [=]() {
        ui->speedButton->setText(" 1.5x");
        player->setPlaybackRate(1.5);
    });
    speedList->addAction("2.0x", this, [=]() {
        ui->speedButton->setText(" 2.0x");
        player->setPlaybackRate(2.0);
    });

    // speed button
    ui->speedButton->setCursor(Qt::PointingHandCursor);
    ui->speedButton->setMenu(speedList);

    // video progress text
    ui->videoProgressText->setText("00:00");

    // set the work space label
    ui->workSpaceLabelWidget->setFixedHeight(TOP_BAR_HEIGHT);
    ui->workSpaceLabel->setStyleSheet("color: white;");

    // set the export button's widget
    ui->exportButtonWidget->setFixedHeight(BOTTOM_BAR_HEIGHT);

    // initialize the video list widget
    videoListWidget = new QWidget(this);
    videoListWidget->setStyleSheet("background-color: #1D1D1D;");
    videoListWidget->setGeometry(0, this->height() - BOTTOM_BAR_HEIGHT - videoListWidget->height(),
                                 this->width() - ui->workPlace->width(), VIDEO_LIST_WIDGET);
    videoListWidget->hide();
    // add a widget to left side of the video list widget
    videoListWidgetLeftWidget = new QWidget(videoListWidget);
    videoListWidgetLeftWidget->setStyleSheet("background-color: #232323;");
    videoListWidgetLeftWidget->setGeometry(0, 0, 40, videoListWidget->height());
    // add 3 buttons to the left widget vertically
    auto *cycleVideoButton = new QPushButton(videoListWidgetLeftWidget);
    cycleVideoButton->setGeometry(10, 15, 20, 20);

    // set icon
    cycleVideoButton->setIcon(QIcon(":/res/image/cycle.png"));
    cycleVideoButton->setIconSize(QSize(14, 14));
    cycleVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                    "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px; background-color: #565656;}"
                                    "QPushButton:hover{background-color:#565656;}"
                                    "QPushButton:pressed{background-color:#454545;}");

    auto *randomVideoButton = new QPushButton(videoListWidgetLeftWidget);
    randomVideoButton->setGeometry(10, 50, 20, 20);
    // set the icon and style
    randomVideoButton->setIcon(QIcon(":/res/image/shuffle.png"));
    randomVideoButton->setIconSize(QSize(14, 14));

    randomVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                     "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px;}"
                                     "QPushButton:hover{background-color:#565656;}"
                                     "QPushButton:pressed{background-color:#454545;}");


    auto *clearAllButton = new QPushButton(videoListWidgetLeftWidget);

    clearAllButton->setGeometry(10, 85, 20, 20);

    clearAllButton->setIcon(QIcon(":/res/image/clear-list.png"));
    clearAllButton->setIconSize(QSize(14, 14));
    // the icon initally is just with icon,
    // hovering or press will show a bg color
    clearAllButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                  "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px;}"
                                  "QPushButton:hover{background-color:#565656;}"
                                  "QPushButton:pressed{background-color:#454545;}");

    // add a scroll area to the right side of the video list widget
    videoListWidgetScrollArea = new QScrollArea(videoListWidget);
    videoListWidgetScrollArea->setStyleSheet("background-color: #1D1D1D;");
    videoListWidgetScrollArea->setGeometry(40, 0, videoListWidget->width() - 40, videoListWidget->height());

    // toggle the video list widget's state
    connect(ui->videoListButton, &QPushButton::clicked, this, &MainWindow::toggleVideoListWidgetState);

    // init the player
    player = new ThePlayer;
    player->setParent(ui->playArea);
    player->mTimer = new QTimer(this);
    player->mTimer->setInterval(100);
    player->mTimer->start();
    connect(player->mTimer, &QTimer::timeout, this, [=]() {
        if (player->state() == QMediaPlayer::PlayingState) {
            ui->videoProgressSlider->setValue(player->position() * 100 / player->duration());
        }
        // set progress text
        QTime time(0, 0, 0);
        time = time.addMSecs(player->position());
        ui->videoProgressText->setText(time.toString("mm:ss"));
    });
    player->setVideoOutput(ui->playArea);

    playlist = new QMediaPlaylist(player);
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    // random play
    connect(randomVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->setPlaybackMode(QMediaPlaylist::Random);
        cycleVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                        "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px;}"
                                        "QPushButton:hover{background-color:#565656;}"
                                        "QPushButton:pressed{background-color:#454545;}");
        randomVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                         "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px; background-color: #565656;}"
                                         "QPushButton:hover{background-color:#565656;}"
                                         "QPushButton:pressed{background-color:#454545;}");
    });
    // cycle play
    connect(cycleVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
        cycleVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                         "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px; background-color: #565656;}"
                                         "QPushButton:hover{background-color:#565656;}"
                                         "QPushButton:pressed{background-color:#454545;}");
        randomVideoButton->setStyleSheet("QPushButton{border-style:solid; border-width:1px; border-color:none;"
                                         "border-radius: 5px; ;margin-right: 3px;width: 30 px;height: 30px;}"
                                         "QPushButton:hover{background-color:#565656;}"
                                         "QPushButton:pressed{background-color:#454545;}");

    });

    // set the initial volume
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setMaximumWidth(120);
    ui->volumeSliderWidget->setMaximumWidth(120);
    ui->volumeSlider->setValue(50);

    ui->volumeSlider->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
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
            "}"
    );
    player->setVolume(ui->volumeSlider->value());

    // set the volume of the video
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [=](int value) {
        player->setVolume(value);
        if (value == 0) {
            ui->volumeButton->setIcon(QIcon(":/res/image/volume-off.png"));
        } else {
            ui->volumeButton->setIcon(QIcon(":/res/image/volume-on.png"));
        }
    });

    // edit the duration of the video
    connect(ui->editDurationButton, &QPushButton::clicked, this, &MainWindow::showEditDurationBox);

    // the fileDialog
    auto *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setOption(QFileDialog::ShowDirsOnly);

    auto *videoListScrollAreaWidget = new QWidget(videoListWidgetScrollArea);
    videoListScrollAreaWidget->setStyleSheet("background-color:#1D1D1D;");
    // set font color
    videoListScrollAreaWidget->setStyleSheet("color: #A2A2A2;");
    auto *videoButtonsLayout = new QHBoxLayout(videoListScrollAreaWidget);
    videoListScrollAreaWidget->setLayout(videoButtonsLayout);
    videoListScrollAreaWidget->setFixedHeight(120);
    videoListWidgetScrollArea->setWidget(videoListScrollAreaWidget);

    // set the style of the scroll bar invisible
    videoListWidgetScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    videoListWidgetScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // open folder button clicked
    connect(openFileButton, &QPushButton::clicked, this, [=]() {
        if (fileDialog->exec()) {
            player->setVideoOutput(ui->playArea);
            QString path = fileDialog->selectedFiles()[0];
            string pathString = path.toStdString();
            cout << pathString << endl;
            outVideoList = getVideoList(pathString);
            vector<QUrl *> outVideoUrlList;
            for (auto &i: outVideoList) {
                outVideoUrlList.push_back(i->url);
            }
            playlist->clear();
            for (auto &i: outVideoUrlList) {
                playlist->addMedia(*i);
            }
            // cout the playlist
            player->setPlaylist(playlist);
            playlist->setCurrentIndex(0);


            for (int i = 0; i < outVideoList.size(); i++) {
                videoButtonsLayout->addWidget(outVideoList[i]);
                connect(outVideoList[i], &TheButton::clicked, this, [=]() {

                    playlist->setCurrentIndex(i);
                    player->stop();
                    ui->topBarText->setText(outVideoList[i]->title->text());

                    QTimer::singleShot(50, this, [=]() {
                        player->play();
                    });

                    TheButton::index = playlist->currentIndex();
                    // disable the button
//                    for(int i = 0; i < outVideoList.size(); i++) {
//                        outVideoList[i]->setEnabled(false);
//                        QTimer::singleShot(51, this, [=](){
//                            outVideoList[i]->setEnabled(true);
//                        });
//                    }
                    QThread::msleep(50);
                    // hide the initPic
                    this->initPic->hide();
                });
                videoListScrollAreaWidget->setMinimumWidth(111 * (i + 1));
                videoListScrollAreaWidget->setMaximumWidth(111 * (i + 1));
            }

            QTimer::singleShot(50, this, [=]() {
                player->play();
            });

            // show the video list
            videoListWidget->show();
        }
    });

    // clear video list button clicked
    connect(clearAllButton, &QPushButton::clicked, this, [=]() {
        if (outVideoList.empty()) {
            auto *msgBox = new QMessageBox(this);
            msgBox->setText("No video in the list");
            msgBox->setWindowTitle("Tip");
            msgBox->setStandardButtons(QMessageBox::Ok);
            msgBox->move(this->x() + (this->width()) / 2 - msgBox->width() / 2 - 187,
                         (this->y() + TOP_BAR_HEIGHT) + (this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT) / 2 -
                         msgBox->height() / 2);
            msgBox->exec();

            return;
        }


        // a model dialog to ensure the user's operation
        QMessageBox::StandardButton reply;
        auto *messageBox = new QMessageBox(this);
        messageBox->setWindowTitle("Clear Video List");
        messageBox->setText("Are you sure to clear the video list?");
        messageBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        messageBox->setDefaultButton(QMessageBox::No);
        messageBox->move(this->x() + (this->width()) / 2 - messageBox->width() / 2 - 240,
                         (this->y() + TOP_BAR_HEIGHT) + (this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT) / 2 -
                         messageBox->height() / 2);
        reply = static_cast<QMessageBox::StandardButton>(messageBox->exec());
        if (reply == QMessageBox::Yes) {
            playlist->clear();
            outVideoList.clear();
            // clear widget in the scroll area widget
            QLayoutItem *child;
            while ((child = videoButtonsLayout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }
            videoListScrollAreaWidget->setMinimumWidth(0);
            videoListScrollAreaWidget->setMaximumWidth(0);
            TheButton::index = -1;
            ui->topBarText->setText("Freewheelin");

            // show the initPic
            this->initPic->show();

            // set the video progress bar to 0
            ui->videoProgressSlider->setValue(0);
        } else {
            return;
        }
    });


    // ////////////////workspace styling //////////////////////////
    // bg #616161 text #A2A2A2 border-radius 5px

    // bg #616161 text #A2A2A2 border-radius 5px apple style width 80px,height 23px
    ui->addTextureButton->setStyleSheet("QPushButton{"
                                        "background-color:#616161;"
                                        "border-radius:5px;"
                                        "color:#DDDDDD;"
                                        "width:68px;"
                                        "height:17px;"
                                        "}"
                                        "QPushButton:hover{"
                                        "background-color:#616161;"
                                        "border-radius:5px;"
                                        "color:#DDDDDD;"
                                        "width:68px;"
                                        "height:17px;"
                                        "}"
                                        "QPushButton:pressed{"
                                        "background-color:#616161;"
                                        "border-radius:5px;"
                                        "color:#DDDDDD;"
                                        "width:68px;"
                                        "height:17px;"
                                        "}"
    );
    ui->addTextureButton->setCursor(Qt::PointingHandCursor);

    ui->lumetriPresetButton->setStyleSheet(
            "QPushButton{"
            "background-color:#616161;"
            "border-radius:5px;"
            "color:#DDDDDD;"
            "width:68px;"
            "height:17px;"
            "}"
            "QPushButton:hover{"
            "background-color:#616161;"
            "border-radius:5px;"
            "color:#DDDDDD;"
            "width:68px;"
            "height:17px;"
            "}"
            "QPushButton:pressed{"
            "background-color:#616161;"
            "border-radius:5px;"
            "color:#DDDDDD;"
            "width:68px;"
            "height:17px;"
            "}"

    );
    ui->lumetriPresetButton->setCursor(Qt::PointingHandCursor);

    ui->videoEffectsButton->setStyleSheet("QPushButton{"
                                          "background-color:#616161;"
                                          "border-radius:5px;"
                                          "color:#DDDDDD;"
                                          "width:68px;"
                                          "height:17px;"
                                          "}"
                                          "QPushButton:hover{"
                                          "background-color:#616161;"
                                          "border-radius:5px;"
                                          "color:#DDDDDD;"
                                          "width:68px;"
                                          "height:17px;"
                                          "}"
                                          "QPushButton:pressed{"
                                          "background-color:#616161;"
                                          "border-radius:5px;"
                                          "color:#DDDDDD;"
                                          "width:68px;"
                                          "height:17px;"
                                          "}"
    );
    ui->videoEffectsButton->setCursor(Qt::PointingHandCursor);
    // bg #616161 text #A2A2A2 border-radius 5px apple style width 80px,height 23px
    ui->videoTransitionButton->setStyleSheet("QPushButton{"
                                             "background-color:#616161;"
                                             "border-radius:5px;"
                                             "color:#DDDDDD;"
                                             "width:68px;"
                                             "height:17px;"

                                             "}"
                                             "QPushButton:hover{"
                                             "background-color:#616161;"
                                             "border-radius:5px;"
                                             "color:#DDDDDD;"
                                             "width:68px;"
                                             "height:17px;"

                                             "}"
                                             "QPushButton:pressed{"
                                             "background-color:#616161;"
                                             "border-radius:5px;"
                                             "color:#DDDDDD;"
                                             "width:68px;"
                                             "height:17px;"

                                             "}"
    );
    ui->videoTransitionButton->setCursor(Qt::PointingHandCursor);
    ui->videoEffectsButton->setCursor(Qt::PointingHandCursor);
    // the same style as the volume slider however the sub page color is more bright


    // ////////////// AlL SLIDER STYLE SETTER  ////////////////
    // ////////////// LONG REPEATED CODE NOTICE ////////////////
    ui->horizontalSlider->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_2->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_2->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_3->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_3->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_4->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_4->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_5->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_5->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_6->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_6->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_7->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_7->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_8->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_8->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    ui->horizontalSlider_9->setCursor(Qt::PointingHandCursor);
    ui->horizontalSlider_9->setStyleSheet(
            "QSlider::groove:horizontal {"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "background: #3D3E3E;"
            "}"
            "QSlider::handle:horizontal {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "width: 2px;"
            "margin-top: -5px;"
            "margin-bottom: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:horizontal {"
            "background:#AFB1B3;"
            "height: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );
    // ////////////// AlL SLIDER STYLE SETTER END ////////////////


    // ////////////////workspace styling end //////////////////////////

    ui->ExportpushButton->setStyleSheet(
            "QPushButton{"
            "color:#A2A2A2;"
            "background-color:#535353;"
            "border-radius:5px;"
            "width:68px;"
            "height:17px;"
            "}"
            "QPushButton:hover{"
            "background-color:#535353;"
            "border-radius:5px;"

            "width:68px;"
            "height:17px;"
            "}"
            "QPushButton:pressed{"
            "background-color:#535353;"
            "border-radius:5px;"

            "width:68px;"
            "height:17px;"
            "}"
    );

    // set label image and size with 100X100 high resolution using smooth transformation
    //    ui->label_5->setPixmap(QPixmap(":/res/image/test.png").scaled(80,80,Qt::KeepAspectRatio,Qt::SmoothTransformation));
    // resolve the prob
    // set the image, and use qpainter to avoid the problem of image not clear
    QPixmap pixmap(":/res/image/test.png");
    QPixmap fitpixmap = pixmap.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap finalPixmap(80, 80);
    finalPixmap.fill(Qt::transparent);
    QPainter painter(&finalPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, 80, 80, fitpixmap);
    ui->label_5->setPixmap(finalPixmap);
    // set the image, and use qpainter to avoid the problem of image not clear
    // for label_6 set the same image
    QPixmap pixmap1(":/res/image/test.png");
    QPixmap fitpixmap1 = pixmap1.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap finalPixmap1(80, 80);
    finalPixmap1.fill(Qt::transparent);
    QPainter painter1(&finalPixmap1);
    painter1.setRenderHint(QPainter::Antialiasing);
    painter1.setRenderHint(QPainter::SmoothPixmapTransform);
    painter1.drawPixmap(0, 0, 80, 80, fitpixmap1);
    ui->label_7->setPixmap(finalPixmap1);
    // set the image, and use qpainter to avoid the problem of image not clear
    // for label_11 set the same image
    QPixmap pixmap2(":/res/image/cool-area.png");
    QPixmap fitpixmap2 = pixmap2.scaled(228, 130, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap finalPixmap2(228, 130);
    finalPixmap2.fill(Qt::transparent);
    QPainter painter2(&finalPixmap2);
    painter2.setRenderHint(QPainter::Antialiasing);
    painter2.setRenderHint(QPainter::SmoothPixmapTransform);
    painter2.drawPixmap(0, 0, 228, 130, fitpixmap2);
    ui->label_11->setPixmap(finalPixmap2);
    // set labl_17 bg-img
    ui->label_17->setPixmap(
            QPixmap(":/res/image/extractor.svg").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // set label_18 bg-img
    ui->label_18->setPixmap(
            QPixmap(":/res/image/extractor.svg").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // set label_19 bg-img
    ui->label_16->setPixmap(
            QPixmap(":/res/image/extractor.svg").scaled(15, 15, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // using qpainter to set the image and avoid the problem of image not clear
    QPixmap pixmap3(":/res/image/color-span.png");
    QPixmap fitpixmap3 = pixmap3.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap finalPixmap3(80, 80);
    finalPixmap3.fill(Qt::transparent);
    QPainter painter3(&finalPixmap3);
    painter3.setRenderHint(QPainter::Antialiasing);
    painter3.setRenderHint(QPainter::SmoothPixmapTransform);
    painter3.drawPixmap(0, 0, 80, 80, fitpixmap3);
    ui->label_19->setPixmap(finalPixmap3);

    // mimic the volume slider to present a vertical slider instead of horizontal slider
    ui->verticalSlider->setStyleSheet(
            "QSlider::groove:vertical {"

            "background: #3D3E3E;"
            "width: 4px;"
            "border-radius: 2px;"
            "}"
            "QSlider::handle:vertical {"
            "background:#E7E5E8;"
            "border: 1px solid #5c5c5c;"
            "height: 2px;"
            "margin-left: -5px;"
            "margin-right: -5px;"
            "border-radius: 1px;"
            "}"
            "QSlider::sub-page:vertical {"
            "background:#AFB1B3;"
            "width: 4px;"
            "border:1px;"
            "border-radius: 2px;"
            "}"
    );


// ////////////////////  STATIC STYLE SET END  /////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////
// /////////////////////////////////////////////////////////////////////////////////////////

// //////////////////   SIGNAL & SLOT BEGIN      /////////////////////////
    // when the pause button is clicked, the video will be paused
    connect(ui->playVideoButton, &QPushButton::clicked, this, [=]() {
        if (player->state() == QMediaPlayer::PlayingState) {
            player->pause();
//             set the pause icon
//            ui->playVideoButton->setIcon(QIcon(":/res/image/pause.png"));
//            ui->playVideoButton->setIconSize(QSize(12, 12));
//            ui->playVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
//                                               "border-radius: 5px; background-color:#505050;margin-right: 3px;width:36px;height:21px;");
        } else {
            if (playlist->currentIndex() != -1) {
                this->initPic->hide();
            }
            player->play();
//            ui->playVideoButton->setIcon(QIcon(":/res/image/play.png"));
//            ui->playVideoButton->setIconSize(QSize(12, 12));
//            ui->playVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
//                                               "border-radius: 5px; background-color:#505050;margin-right: 3px;width:36px;height:21px;");
        }
    });

    // if the video is playing, the pause icon will be shown
    connect(player, &QMediaPlayer::stateChanged, this, [=](QMediaPlayer::State state) {
        if (state == QMediaPlayer::PlayingState) {
            ui->playVideoButton->setIcon(QIcon(":/res/image/pause.png"));
            ui->playVideoButton->setIconSize(QSize(12, 12));
            ui->playVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                               "border-radius: 5px; background-color:#505050;margin-right: 3px;width:36px;height:21px;");
        } else {
            ui->playVideoButton->setIcon(QIcon(":/res/image/play.png"));
            ui->playVideoButton->setIconSize(QSize(12, 12));
            ui->playVideoButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                               "border-radius: 5px; background-color:#505050;margin-right: 3px;width:36px;height:21px;");
        }
    });

    // once the TheButton::index is changed, the video will be played
    connect(playlist, &QMediaPlaylist::currentIndexChanged, this, [=](int index) {
        if (index != -1) {
            this->initPic->hide();
        }
        if (TheButton::index != -1 && TheButton::index != index) {
            outVideoList[TheButton::index]->title->setStyleSheet("background-color: rgb(29, 29, 29); color: #a2a2a2;");
            outVideoList[TheButton::index]->duration->setStyleSheet("background-color: rgb(29, 29, 29); color: #a2a2a2;");
        }
        TheButton::index = index;
        outVideoList[index]->duration->setStyleSheet("background-color: rgb(69, 69, 69); color: #a2a2a2; border-bottom-left-radius: 5px; border-bottom-right-radius: 5px;");
        outVideoList[index]->title->setStyleSheet("background-color: rgb(69, 69, 69); color: #a2a2a2; border-top-left-radius: 5px; border-top-right-radius: 5px;");
    });

    // when the terminate button is clicked, the video will be terminated
    connect(ui->terminateVideoButton, &QPushButton::clicked, this, [=]() {
        player->stop();
        player->pause();
        ui->videoProgressSlider->setValue(0);

        // disable the bottom bar
        ui->bottomBar->setEnabled(false);
        QTimer::singleShot(50, this, [=]() {
            ui->bottomBar->setEnabled(true);
        });
    });

    // next video button clicked
    connect(ui->nextVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->next();
        player->stop();
        // wait for the resource to be released
        QTimer::singleShot(50, this, [=]() {
            player->play();
        });
        TheButton::index = playlist->currentIndex();
        // disable the bottom bar
        ui->bottomBar->setEnabled(false);
        QTimer::singleShot(50, this, [=]() {
            ui->bottomBar->setEnabled(true);
        });
    });

    // previous video button clicked
    connect(ui->previousVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->previous();
        player->stop();
        QTimer::singleShot(50, this, [=]() {
            player->play();
        });
        TheButton::index = playlist->currentIndex();
        // disable the bottom bar
        ui->bottomBar->setEnabled(false);
        QTimer::singleShot(50, this, [=]() {
            ui->bottomBar->setEnabled(true);
        });
    });

    connect(ui->videoProgressSlider, &QSlider::valueChanged, this, [=](float value) {
        if (value > 99) {
            playlist->next();
            player->stop();

            QTimer::singleShot(50, this, [=](){
                player->play();
            });
        }
    });

    // when the video progress slider is moved, the video will be moved to the position
    connect(ui->videoProgressSlider, &QSlider::sliderMoved, this, [=](int value) {
        player->mTimer->stop();
        player->setPosition(ui->videoProgressSlider->value() * player->duration() / 100);
    });
    connect(ui->videoProgressSlider, &QSlider::sliderReleased, this, [=]() {
        player->mTimer->start();
    });
    connect(ui->videoProgressSlider, &QSlider::sliderPressed, this, [=]() {
        player->setPosition(ui->videoProgressSlider->value() * player->duration() / 100);
    });

    // when the volume button is clicked, the volume will be muted
    connect(ui->volumeButton, &QPushButton::clicked, this, [=]() {
        if (player->isMuted()) {
            player->setMuted(false);
            ui->volumeButton->setIcon(QIcon(":/res/image/volume-on.png"));
            ui->volumeSlider->setValue(lastVolume);
        } else {
            lastVolume = ui->volumeSlider->value();
            player->setMuted(true);
            ui->volumeButton->setIcon(QIcon(":/res/image/volume-off.png"));
            ui->volumeSlider->setValue(0);
        }
    });

    // full screen button clicked, the player widget will be full screen
    connect(ui->fullScreenButton, &QPushButton::clicked, this, [=]() {
        if (ui->playPlace->isFullScreen()) {
            ui->fullScreenButton->setIcon(QIcon(":/res/image/full-screen.png"));
            ui->fullScreenButton->setIconSize(QSize(12, 12));
            ui->fullScreenButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                                "border-radius: 5px; background-color:#454545;margin-left: 5px;width: 20px;height: 20px;");
            ui->playPlace->setWindowFlags(Qt::SubWindow);
            initPic->setParent(this);
            videoListWidget->setParent(this);
            ui->playPlace->showNormal();
            ui->topBar->show();
            this->show();
            videoListWidget->setGeometry(0, this->height() - BOTTOM_BAR_HEIGHT - videoListWidget->height(),
                                         this->width() - ui->workPlace->width(), VIDEO_LIST_WIDGET);
            videoListWidgetScrollArea->setGeometry(40, 0, this->width() - WORKPLACE_WIDTH - 40,
                                                   videoListWidget->height());
            initPic->setGeometry(0, TOP_BAR_HEIGHT, this->width() - WORKPLACE_WIDTH,
                                 this->height() - BOTTOM_BAR_HEIGHT - TOP_BAR_HEIGHT);
            initLabel->setGeometry(initPic->width() / 2 - 300, initPic->height() / 2 - 300 - 50, 600, 600);
            openFileButton->setGeometry(initPic->width() / 2 - 75, initPic->height() / 2 + 50, 150, 35);
        } else {
            if (TheButton::index != -1) {
                ui->fullScreenButton->setIcon(QIcon(":/res/image/shrink-to-normal.png"));
                ui->fullScreenButton->setIconSize(QSize(12, 12));
                ui->fullScreenButton->setStyleSheet("border-style:solid; border-width:1px; border-color:none;"
                                                    "border-radius: 5px; background-color:#454545;margin-left: 5px;width: 20px;height: 20px;");
                ui->playPlace->setWindowFlags(Qt::Window);
                initPic->setParent(ui->playPlace);
                videoListWidget->setParent(ui->playPlace);
                ui->playPlace->showFullScreen();
                videoListWidget->setGeometry(0, ui->playPlace->height() - BOTTOM_BAR_HEIGHT - 120,
                                             ui->playPlace->width(), VIDEO_LIST_WIDGET);
                videoListWidgetScrollArea->setGeometry(40, 0, ui->playPlace->width() - 40, VIDEO_LIST_WIDGET);
                initPic->setGeometry(0, 0, ui->playPlace->width(), ui->playPlace->height() - BOTTOM_BAR_HEIGHT);
                initLabel->setGeometry(initPic->width() / 2 - 300, initPic->height() / 2 - 300 - 50, 600, 600);
                openFileButton->setGeometry(initPic->width() / 2 - 75, initPic->height() / 2 + 50, 150, 35);
                ui->topBar->hide();
                this->hide();
            } else {
                // show tip message
                auto *msgBox = new QMessageBox(this);
                msgBox->setWindowTitle("Tip");
                msgBox->setText("Please open a video first!");
                msgBox->setStandardButtons(QMessageBox::Ok);
                msgBox->move(this->x() + (this->width()) / 2 - msgBox->width() / 2 - 200,
                             (this->y() + TOP_BAR_HEIGHT) + (this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT) / 2 -
                             msgBox->height() / 2);
                msgBox->exec();
            }
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    // set the width of video list widget
    int width = this->width() - ui->workPlace->width();
    videoListWidget->setGeometry(0, this->height() - BOTTOM_BAR_HEIGHT - videoListWidget->height(), width,
                                 videoListWidget->height());
    // set the width of scroll area
    videoListWidgetScrollArea->setGeometry(40, 0, width - 40, videoListWidget->height());

    // set the initPic
    initPic->setGeometry(0, TOP_BAR_HEIGHT, this->width() - WORKPLACE_WIDTH,
                         this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT);
    initLabel->setGeometry(initPic->width() / 2 - 300, initPic->height() / 2 - 300 - 50, 600, 600);
    openFileButton->setGeometry(initPic->width() / 2 - 75, initPic->height() / 2 + 50, 150, 35);
}

void MainWindow::toggleVideoListWidgetState() {
    if (videoListWidget->isHidden()) {
        videoListWidget->show();
    } else {
        videoListWidget->hide();
    }
}

std::vector<TheButton *> MainWindow::getVideoList(std::string loc) {
//    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    auto *out = new std::vector<TheButton *>();
    QDir dir(QString::fromStdString(loc));
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV")) { // mac/linux
#endif

                QString thumb = f.left(f.length() - 4) + ".png";
                if (QFile(thumb).exists()) { // if a png thumbnail exists
                    auto *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QUrl *iconUrl = new QUrl(thumb);
                        QUrl *url = new QUrl(QUrl::fromLocalFile(f)); // convert the file location to a generic url
                        // get the title of the video
                        QString title = f.mid(f.lastIndexOf('/') + 1);
                        title = title.left(title.length());
                        auto *label = new QLabel(title);
                        // get the duration of the video
                        auto *playerTmp = new QMediaPlayer();
                        playerTmp->setMedia(*url);
                        playerTmp->play();
                        // wait for the video to load
                        while (playerTmp->duration() == 0) {
                            QCoreApplication::processEvents();
                        }
                        playerTmp->pause();
                        QString duration = QTime(0, 0, 0).addMSecs(playerTmp->duration()).toString("mm:ss");
                        auto *durationLabel = new QLabel(duration);
                        out->push_back(new TheButton(videoListWidgetScrollArea, url, &sprite, label, durationLabel));

                        delete playerTmp;
                    } else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
                } else
                    qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
            }
    }

    return *out;
}


void MainWindow::showEditDurationBox() {
    if (TheButton::index == -1) {
        // a new message box
        auto *messageBox = new QMessageBox(ui->playArea);
        messageBox->setParent(ui->playArea);
        messageBox->setWindowTitle("Warning");
        messageBox->setText("Please select a video first.");
        // set the font color
        messageBox->setStyleSheet("QLabel{color:#A2A2A2;}");
        messageBox->setStandardButtons(QMessageBox::Ok);
        // set the OK button style
        messageBox->button(QMessageBox::Ok)->setStyleSheet("QPushButton{"
                                                           "background-color:#616161;"
                                                           "border-radius:5px;"
                                                           "color:#DDDDDD;"
                                                           "width:68px;"
                                                           "height:17px;"
                                                           "}"
                                                           "QPushButton:hover{"
                                                           "background-color:rgb(61, 62, 62);"
                                                           "border-radius:5px;"
                                                           "color:#DDDDDD;"
                                                           "width:68px;"
                                                           "height:17px;"
                                                           "}"
                                                           "QPushButton:pressed{"
                                                           "background-color:rgb(61, 62, 62);"
                                                           "border-radius:5px;"
                                                           "color:#DDDDDD;"
                                                           "width:68px;"
                                                           "height:17px;"
                                                           "}"
        );

        // relative move
        messageBox->move(this->x() + (this->width()) / 2 - messageBox->width() / 2 - 200,
                         (this->y() + TOP_BAR_HEIGHT) + (this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT) / 2 -
                         messageBox->height() / 2);
        messageBox->exec();
    } else {
        // pause the video
        player->pause();
        // get the current video's duration
        int duration = player->duration();
        // convert the duration to time: 00:00
        QTime time(0, 0, 0);
        time = time.addMSecs(duration);
        auto durationText = time.toString("mm:ss");
        auto *editDurationBox = new EditDurationBox(QString(durationText), this);
        editDurationBox->move(this->x() + (this->width()) / 2 - editDurationBox->width() / 2 - WORKPLACE_WIDTH / 2,
                              (this->y() + TOP_BAR_HEIGHT) + (this->height() - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT) / 2 -
                              editDurationBox->height() / 2);
        editDurationBox->exec();
    }
}

void MainWindow::initSettingButton() {
    // set the setting button's relative position
    settingButton = new QPushButton(ui->topBar);
    settingButton->setGeometry(5, 5, 20, 20);
    settingButton->setCursor(Qt::PointingHandCursor);
    settingButton->setToolTip("Settings");
    settingButton->setToolTipDuration(1000);


    // menu list
    auto *menu = new QMenu(settingButton);
    // style sheet
    menu->setStyleSheet("QMenu{background-color:#454545; color:#a2a2a2; border:1px solid #454545;}"
                             "QMenu::item:selected{background-color:#696969; color:#a2a2a2; cursor: pointer;}");
    auto *languageMenu = new QMenu("Set Language", menu);

    languageMenu->addAction("English(US)");
    languageMenu->addSeparator();
    languageMenu->addAction("English(UK)");
    languageMenu->addSeparator();
    languageMenu->addAction("Chinese(Simplified)");
    languageMenu->addSeparator();
    languageMenu->addAction("Chinese(Traditional)");
    languageMenu->addSeparator();
    languageMenu->addAction("Japanese");
    languageMenu->addSeparator();
    languageMenu->addAction("French");
    languageMenu->addSeparator();
    auto *moreOptionMenu = new QMenu("More Options", menu);
    languageMenu->addMenu(moreOptionMenu);
    menu->addMenu(languageMenu);
    menu->addSeparator();
    auto *subTitlesMenu = new QMenu("Add Subtitles", menu);
    subTitlesMenu->addAction("Add Subtitle File...");
    subTitlesMenu->addSeparator();
    auto *textSizeMenu = new QMenu("Text Size", subTitlesMenu);
    textSizeMenu->addAction("Smaller");
    textSizeMenu->addSeparator();
    textSizeMenu->addAction("Small");
    textSizeMenu->addSeparator();
    textSizeMenu->addAction("Normal");
    textSizeMenu->addSeparator();
    textSizeMenu->addAction("Large");
    textSizeMenu->addSeparator();
    textSizeMenu->addAction("Larger");
    subTitlesMenu->addMenu(textSizeMenu);
    subTitlesMenu->addSeparator();
    auto *textColorMenu = new QMenu("Text Color", subTitlesMenu);
    textColorMenu->addAction("Black");
    textColorMenu->addSeparator();
    textColorMenu->addAction("Gray");
    textColorMenu->addSeparator();
    textColorMenu->addAction("Silver");
    textColorMenu->addSeparator();
    textColorMenu->addAction("White");
    textColorMenu->addSeparator();
    textColorMenu->addAction("Maroon");
    textColorMenu->addSeparator();
    textColorMenu->addAction("Red");
    subTitlesMenu->addMenu(textColorMenu);
    subTitlesMenu->addSeparator();
    auto *outlineThicknessMenu = new QMenu("Outline Thickness", subTitlesMenu);
    outlineThicknessMenu->addAction("None");
    outlineThicknessMenu->addSeparator();
    outlineThicknessMenu->addAction("Thin");
    outlineThicknessMenu->addSeparator();
    outlineThicknessMenu->addAction("Normal");
    outlineThicknessMenu->addSeparator();
    outlineThicknessMenu->addAction("Thick");
    subTitlesMenu->addMenu(outlineThicknessMenu);
    subTitlesMenu->addSeparator();
    auto *backgroundColorMenu = new QMenu("Background Color", subTitlesMenu);
    backgroundColorMenu->addAction("Black");
    backgroundColorMenu->addSeparator();
    backgroundColorMenu->addAction("Gray");
    backgroundColorMenu->addSeparator();
    backgroundColorMenu->addAction("Silver");
    backgroundColorMenu->addSeparator();
    backgroundColorMenu->addAction("White");
    backgroundColorMenu->addSeparator();
    backgroundColorMenu->addAction("Maroon");
    backgroundColorMenu->addSeparator();
    backgroundColorMenu->addAction("Red");
    subTitlesMenu->addMenu(backgroundColorMenu);
    menu->addMenu(subTitlesMenu);

    menu->addSeparator();
    auto *colorEnhancementMenu = new QMenu("Color Enhancement", menu);
    colorEnhancementMenu->addAction("Increase Contrast");
    colorEnhancementMenu->addSeparator();
    colorEnhancementMenu->addAction("Differentiate Without Color");
    colorEnhancementMenu->addSeparator();
    colorEnhancementMenu->addAction("Classic Invert");
    colorEnhancementMenu->addSeparator();
    colorEnhancementMenu->addAction("Reduce White Point");
    colorEnhancementMenu->addSeparator();
    auto *colorFiltersMenu = new QMenu("Color Filters", colorEnhancementMenu);
    colorFiltersMenu->addAction("Red/Green Filter");
    colorFiltersMenu->addSeparator();
    colorFiltersMenu->addAction("Green/Red Filter");
    colorFiltersMenu->addSeparator();
    colorFiltersMenu->addAction("Blue/Yellow Filter");
    colorFiltersMenu->addSeparator();
    colorEnhancementMenu->addMenu(colorFiltersMenu);
    menu->addMenu(colorEnhancementMenu);
    menu->addSeparator();
    auto *dictationMenu = new QMenu("Dictation", menu);
    dictationMenu->addAction("Live Caption");
    dictationMenu->addSeparator();
    dictationMenu->addAction("Mono Audio");
    dictationMenu->addSeparator();
    dictationMenu->addAction("Hearing Aid Compatibility");
    menu->addMenu(dictationMenu);
    menu->addSeparator();
    menu->addAction("Search");
    settingButton->setMenu(menu);

    settingButton->setIconSize(QSize(20, 20));
    // set the icon
    settingButton->setIcon(QIcon(":res/image/more.png"));
    // set the style
    settingButton->setStyleSheet("QPushButton{"
                                 "background-color:#616161;"
                                 "border-radius:5px;"
                                 "color:#DDDDDD;"
                                 "width:68px;"
                                 "height:17px;"
                                 "}"
                                 "QPushButton:hover{"
                                 "background-color:#616161;"
                                 "border-radius:5px;"
                                 "color:#DDDDDD;"
                                 "width:68px;"
                                 "height:17px;"
                                 "}"
                                 "QPushButton:pressed{"
                                 "background-color:#616161;"
                                 "border-radius:5px;"
                                 "color:#DDDDDD;"
                                 "width:68px;"
                                 "height:17px;"
                                 "}"
                                 "QPushButton:menu-indicator{"
                                 "image:none;"
                                 "}");
}

void MainWindow::setInitPic() {
    // set the initial picture when the program starts
    initPic = new QWidget(this);
    initPic->setStyleSheet("background-color: #1a1a1a;");
    initLabel = new QLabel(initPic);
    initLabel->setPixmap(QPixmap(":/res/image/banner-area.png"));
    // resize the img to 160X160

    openFileButton = new QPushButton(initPic);
    openFileButton->setText("Open File");

//     set the bg as #1373E6, and the radius 22px
//     after hover, change the color as #0F66d0
//     adobe style
    openFileButton->setStyleSheet("QPushButton{"
                                  "border: none; background-color: #1373E6; color: white; height: 30px; border-radius: 5px;"
                                  "}"
                                  "QPushButton:hover{"
                                  "background-color: #0F66d0;"
                                  "}");


    openFileButton->setCursor(Qt::PointingHandCursor);
    initLabel->setScaledContents(true);
    initPic->setGeometry(0, TOP_BAR_HEIGHT, this->width() - WORKPLACE_WIDTH,
                         this->height() - BOTTOM_BAR_HEIGHT - TOP_BAR_HEIGHT);
    initLabel->setGeometry(initPic->width() / 2 - 300, initPic->height() / 2 - 300 - 50, 600, 600);
    openFileButton->setGeometry(initPic->width() / 2 - 75, initPic->height() / 2 + 50, 150, 35);
}

void MainWindow::initWorkSpace() {
    // set the width of work space fixed

    ui->workPlace->setFixedWidth(WORKPLACE_WIDTH);
    // set the background color of work space
    ui->workPlace->setStyleSheet("background-color: #232323;");

    ui->editDurationButton->setCursor(Qt::PointingHandCursor);
//    ui->editDurationButton->setStyleSheet(
//            "QPushButton{border: none; background-color: #ac7979; color: #A2A2A2; height: 25px; border-radius: 5px;}"
//            "QPushButton:hover{background-color: #1d6f1d;}"
//            "QPushButton:pressed{background-color: #1d6f1d;}");
// adobe style
    ui->editDurationButton->setStyleSheet(
            "QPushButton{border: none; background-color: #1373E6; color: white; height: 25px; border-radius: 5px;}"
            "QPushButton:hover{background-color: #0F66d0;}"
            "QPushButton:pressed{background-color: #0F66d0;}");
    // when cursor is hover, set it to a hand
    ui->editDurationButton->setText("Edit Duration");

    // set the style of the scroll bar
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                                       "border: none;"
                                                       "background: #232323;"
                                                       "width: 10px;"
                                                       "margin: 0px 0px 0px 0px;"
                                                       "}"
                                                       "QScrollBar::handle:vertical {"
                                                       "background: #3d3d3d;"
                                                       "min-height: 20px;"
                                                       "border-radius: 5px;"
                                                       "}"
                                                       "QScrollBar::add-line:vertical {"
                                                       "height: 0px;"
                                                       "subcontrol-position: bottom;"
                                                       "subcontrol-origin: margin;"
                                                       "}"
                                                       "QScrollBar::sub-line:vertical {"
                                                       "height: 0 px;"
                                                       "subcontrol-position: top;"
                                                       "subcontrol-origin: margin;"
                                                       "}"
                                                       "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {"
                                                       "background: none;"
                                                       "}");
}
