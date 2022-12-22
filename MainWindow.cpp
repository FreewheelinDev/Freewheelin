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
    ui->topBar->setStyleSheet("background-color: rgb(206, 208, 209);");
    ui->bottomBar->setStyleSheet("background-color: rgb(137, 147, 153)");

    // initialize the setting button
    initSettingButton();

    // video progress slider
    ui->videoProgressSlider->setRange(0, 100);
    ui->videoProgressSlider->setValue(0);
    ui->videoProgressSlider->setStyleSheet("QSlider::groove:horizontal {"
                                          "border: 1px solid #bbb;"
                                          "background: white;"
                                          "height: 8px;"
                                          "border-radius: 4px;"
                                          "}"
                                          "QSlider::handle:horizontal {"
                                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                          "stop:0 #eee, stop:1 #ccc);"
                                          "border: 1px solid #777;"
                                          "width: 18px;"
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
                                          "QSlider::sub-page:horizontal {"
                                          "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                                          "stop:0 #66e, stop:1 #bbf);"
                                          "border: 1px solid #777;"
                                          "height: 8px;"
                                          "border-radius: 4px;"
                                          "}"
                                          "QSlider::add-page:horizontal {"
                                          "background: #fff;"
                                          "border: 1px solid #777;"
                                          "height: 8px;"
                                          "border-radius: 4px;"
                                          "}"
                                          "QSlider::add-page:horizontal:disabled {"
                                          "background: #eee;"
                                          "border-color: #999;"
                                          "}"
                                          "QSlider::sub-page:horizontal:disabled {"
                                          "background: #eee;"
                                          "border-color: #999;"
                                          "}"
                                          "QSlider::handle:horizontal:disabled {"
                                          "background: #eee;"
                                          "border: 1px solid #aaa;"
                                          "border-radius: 4px;"
                                          "}"
    );

    // volume slider
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setValue(50);

    // speed list
    auto *speedList = new QMenu(ui->speedButton);
    auto *actionSpeed1 = new QAction("0.5x", this);
    auto *actionSpeed2 = new QAction("1.0x", this);
    auto *actionSpeed3 = new QAction("1.5x", this);
    auto *actionSpeed4 = new QAction("2.0x", this);
    speedList->addAction(actionSpeed1);
    speedList->addAction(actionSpeed2);
    speedList->addAction(actionSpeed3);
    speedList->addAction(actionSpeed4);

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
    videoListWidget->setStyleSheet("background-color: #fffff4;");
    videoListWidget->setGeometry(0, this->height()-BOTTOM_BAR_HEIGHT-videoListWidget->height(), this->width()-ui->workPlace->width(), VIDEO_LIST_WIDGET);
    videoListWidget->hide();
    // add a widget to left side of the video list widget
    videoListWidgetLeftWidget = new QWidget(videoListWidget);
    videoListWidgetLeftWidget->setStyleSheet("background-color: #909090;");
    videoListWidgetLeftWidget->setGeometry(0, 0, 40, videoListWidget->height());
    // add 3 buttons to the left widget vertically
    auto *cycleVideoButton = new QPushButton(videoListWidgetLeftWidget);
    cycleVideoButton->setGeometry(10, 15, 20, 20);
    auto *randomVideoButton = new QPushButton(videoListWidgetLeftWidget);
    randomVideoButton->setGeometry(10, 50, 20, 20);
    auto *clearAllButton = new QPushButton(videoListWidgetLeftWidget);
    clearAllButton->setGeometry(10, 85, 20, 20);
    // add a scroll area to the right side of the video list widget
    videoListWidgetScrollArea = new QScrollArea(videoListWidget);
    videoListWidgetScrollArea->setStyleSheet("background-color: white;");
    videoListWidgetScrollArea->setGeometry(40, 0, videoListWidget->width()-40, videoListWidget->height());

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
            ui->videoProgressSlider->setValue(player->position()*100/player->duration());
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
    });
    // cycle play
    connect(cycleVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->setPlaybackMode(QMediaPlaylist::Loop);
    });

    // set the initial volume
    ui->volumeSlider->setRange(0, 100);
    ui->volumeSlider->setMaximumWidth(120);
    ui->volumeSliderWidget->setMaximumWidth(120);
    ui->volumeSlider->setValue(50);
    // set the style sheet of volume slider with macOS style
    ui->volumeSlider->setStyleSheet("QSlider::groove:horizontal {"
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
    player->setVolume(ui->volumeSlider->value());

    // set the volume of the video
    connect(ui->volumeSlider, &QSlider::valueChanged, this, [=](int value) {
        player->setVolume(value);
    });

    // edit the duration of the video
    connect(ui->editDurationButton, &QPushButton::clicked, this, &MainWindow::showEditDurationBox);

    // the fileDialog
    auto *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->setOption(QFileDialog::ShowDirsOnly);

    auto *videoListScrollAreaWidget = new QWidget(videoListWidgetScrollArea);
    videoListScrollAreaWidget->setStyleSheet("background-color: white;");
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
            vector<QUrl*> outVideoUrlList;
            for (auto &i : outVideoList) {
                outVideoUrlList.push_back(i->url);
            }
            playlist->clear();
            for (auto &i : outVideoUrlList) {
                playlist->addMedia(*i);
            }
            // cout the playlist
            player->setPlaylist(playlist);
            playlist->setCurrentIndex(0);

            for (int i = 0; i < outVideoList.size(); i++) {
                videoButtonsLayout->addWidget(outVideoList[i]);
                connect(outVideoList[i], &TheButton::clicked, this, [=]() {
                    playlist->setCurrentIndex(i);
                    TheButton::index = i;
                    ui->topBarText->setText(outVideoList[i]->title->text());
                    player->play();
                    // hide the initPic
                    this->initPic->hide();
                });
                videoListScrollAreaWidget->setMinimumWidth(111 * (i + 1));
                videoListScrollAreaWidget->setMaximumWidth(111 * (i + 1));
            }

            // show the video list
            videoListWidget->show();
        }
    });

    // clear video list button clicked
    connect(clearAllButton, &QPushButton::clicked, this, [=]() {
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
    });

    // when the pause button is clicked, the video will be paused
    connect(ui->playVideoButton, &QPushButton::clicked, this, [=]() {
        if (player->state() == QMediaPlayer::PlayingState) {
            player->pause();
        } else {
            if (playlist->currentIndex() != -1) {
                this->initPic->hide();
            }
            player->play();
        }
    });

    // when the terminate button is clicked, the video will be terminated
    connect(ui->terminateVideoButton, &QPushButton::clicked, this, [=]() {
        player->stop();
        player->pause();
        ui->videoProgressSlider->setValue(0);
    });

    // next video button clicked
    connect(ui->nextVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->next();
    });

    // previous video button clicked
    connect(ui->previousVideoButton, &QPushButton::clicked, this, [=]() {
        playlist->previous();
    });

    // when the video progress slider is moved, the video will be moved to the position
    connect(ui->videoProgressSlider, &QSlider::sliderMoved, this, [=](int value) {
        player->mTimer->stop();
        player->setPosition(ui->videoProgressSlider->value()*player->duration()/100);
    });
    connect(ui->videoProgressSlider, &QSlider::sliderReleased, this, [=]() {
        player->mTimer->start();
    });
    connect(ui->videoProgressSlider, &QSlider::sliderPressed, this, [=]() {
        player->setPosition(ui->videoProgressSlider->value()*player->duration()/100);
    });

    // different speed button clicked
    connect(actionSpeed1, &QAction::triggered, this, [=]() {
        player->setPlaybackRate(0.5);
    });
    connect(actionSpeed2, &QAction::triggered, this, [=]() {
        player->setPlaybackRate(1);
    });
    connect(actionSpeed3, &QAction::triggered, this, [=]() {
        player->setPlaybackRate(1.5);
    });
    connect(actionSpeed4, &QAction::triggered, this, [=]() {
        player->setPlaybackRate(2);
    });

    // full screen button clicked, the player widget will be full screen
    connect(ui->fullScreenButton, &QPushButton::clicked, this, [=]() {
        if (ui->playPlace->isFullScreen()) {
            ui->playPlace->setWindowFlags(Qt::SubWindow);
            initPic->setParent(this);
            videoListWidget->setParent(this);
            ui->playPlace->showNormal();
            ui->topBar->show();
            this->show();
            videoListWidget->setGeometry(0, this->height()-BOTTOM_BAR_HEIGHT-videoListWidget->height(), this->width()-ui->workPlace->width(), VIDEO_LIST_WIDGET);
            videoListWidgetScrollArea->setGeometry(40, 0, this->width()-WORKPLACE_WIDTH-40, videoListWidget->height());
            initPic->setGeometry(0, TOP_BAR_HEIGHT, this->width()-WORKPLACE_WIDTH, this->height()-BOTTOM_BAR_HEIGHT-TOP_BAR_HEIGHT);
            initLabel->setGeometry(initPic->width()/2-300, initPic->height()/2-300-50, 600,600);
            openFileButton->setGeometry(initPic->width()/2-75, initPic->height()/2+50, 150, 35);
        } else {
            ui->playPlace->setWindowFlags(Qt::Window);
            initPic->setParent(ui->playPlace);
            videoListWidget->setParent(ui->playPlace);
            ui->playPlace->showFullScreen();
            videoListWidget->setGeometry(0, ui->playPlace->height()-BOTTOM_BAR_HEIGHT-120, ui->playPlace->width(), VIDEO_LIST_WIDGET);
            videoListWidgetScrollArea->setGeometry(40, 0, ui->playPlace->width()-40, VIDEO_LIST_WIDGET);
            initPic->setGeometry(0, 0, ui->playPlace->width(), ui->playPlace->height()-BOTTOM_BAR_HEIGHT);
            initLabel->setGeometry(initPic->width()/2-300, initPic->height()/2-300-50, 600,600);
            openFileButton->setGeometry(initPic->width()/2-75, initPic->height()/2+50, 150, 35);
            ui->topBar->hide();
            this->hide();
        }
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);

    // set the width of video list widget
    int width = this->width()-ui->workPlace->width();
    videoListWidget->setGeometry(0, this->height()-BOTTOM_BAR_HEIGHT-videoListWidget->height(), width, videoListWidget->height());
    // set the width of scroll area
    videoListWidgetScrollArea->setGeometry(40, 0, width-40, videoListWidget->height());

    // set the initPic
    initPic->setGeometry(0, TOP_BAR_HEIGHT, this->width()-WORKPLACE_WIDTH, this->height()-TOP_BAR_HEIGHT-BOTTOM_BAR_HEIGHT);
    initLabel->setGeometry(initPic->width()/2-300, initPic->height()/2-300-50, 600,600);
    openFileButton->setGeometry(initPic->width()/2-75, initPic->height()/2+50, 150, 35);
}

void MainWindow::toggleVideoListWidgetState() {
    if (videoListWidget->isHidden()) {
        videoListWidget->show();
    } else {
        videoListWidget->hide();
    }
}

std::vector<TheButton*> MainWindow::getVideoList (std::string loc) {
//    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    auto *out = new std::vector<TheButton*>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

        if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

                QString thumb = f.left( f .length() - 4) +".png";
                if (QFile(thumb).exists()) { // if a png thumbnail exists
                    auto *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QUrl* iconUrl = new QUrl(thumb);
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        // get the title of the video
                        QString title = f.mid(f.lastIndexOf('/') + 1);
                        title = title.left(title.length());
                        auto* label = new QLabel(title);
                        // get the duration of the video
                        auto* playerTmp = new QMediaPlayer();
                        playerTmp->setMedia(*url);
                        playerTmp->play();
                        // wait for the video to load
                        while (playerTmp->duration() == 0) {
                            QCoreApplication::processEvents();
                        }
                        playerTmp->pause();
                        QString duration = QTime(0, 0, 0).addMSecs(playerTmp->duration()).toString("mm:ss");
                        auto* durationLabel = new QLabel(duration);
                        out->push_back(new TheButton(videoListWidgetScrollArea, url, &sprite, label, durationLabel));

                        delete playerTmp;
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
                }
                else
                    qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
            }
    }

    return *out;
}


void MainWindow::showEditDurationBox() {
    if (TheButton::index == -1) {
//            QMessageBox::warning(this, "Warning", "Please select a video first.");
        // a new message box
        auto *messageBox = new QMessageBox(ui->playArea);
        messageBox->setParent(ui->playArea);
        messageBox->setWindowTitle("Warning");
        messageBox->setText("Please select a video first.");
        // relative move
        messageBox->move(this->x()+(this->width())/2-messageBox->width()/2-200, (this->y()+TOP_BAR_HEIGHT)+(this->height()-TOP_BAR_HEIGHT-BOTTOM_BAR_HEIGHT)/2-messageBox->height()/2);
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
        editDurationBox->move(this->x()+(this->width())/2-editDurationBox->width()/2-WORKPLACE_WIDTH/2, (this->y()+TOP_BAR_HEIGHT)+(this->height()-TOP_BAR_HEIGHT-BOTTOM_BAR_HEIGHT)/2-editDurationBox->height()/2);
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
}

void MainWindow::setInitPic() {
    // set the initial picture when the program starts
    initPic = new QWidget(this);
    initPic->setStyleSheet("background-color: #ececec;");
    initLabel = new QLabel(initPic);
    initLabel->setPixmap(QPixmap(":/res/image/home_tag.png"));
    openFileButton = new QPushButton(initPic);
    openFileButton->setText("Open File");
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
    ui->editDurationButton->setStyleSheet("QPushButton{border: none; background-color: #ac7979; color: #ffffff; height: 25px; border-radius: 5px;}"
                                          "QPushButton:hover{background-color: #1d6f1d;}"
                                          "QPushButton:pressed{background-color: #1d6f1d;}");
    ui->editDurationButton->setText("Edit Duration");

    // set the style of the scroll bar
    ui->scrollArea->verticalScrollBar()->setStyleSheet("QScrollBar:vertical {"
                                                      "width: 10px;"
                                                      "background: #232323;"
                                                      "margin: 0px 0px 0px 0px;"
                                                      "padding-top: 0px;"
                                                      "padding-bottom: 0px;"
                                                      "}"
                                                      "QScrollBar::handle:vertical {"
                                                      "background: #ac7979;"
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

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);

    qDebug() << "mouse move";
}
