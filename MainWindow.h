

#ifndef FREEWHEELIN_MAINWINDOW_H
#define FREEWHEELIN_MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QScrollArea>
#include <QMediaPlaylist>
#include <QScrollBar>

#include "ThePlayer.h"

#define TOP_BAR_HEIGHT 30
#define BOTTOM_BAR_HEIGHT 40
#define WORKPLACE_WIDTH 280
#define VIDEO_LIST_WIDGET 122

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;

    QWidget *videoListWidget;
    QWidget *videoListWidgetLeftWidget;
    QScrollArea *videoListWidgetScrollArea;

    QMediaPlaylist *playlist;

    ThePlayer* player;

    std::vector<TheButton *> outVideoList = std::vector<TheButton *>();

    std::vector<TheButton *> getVideoList(std::string loc);

    QPushButton *settingButton;

    QWidget *initPic;

    QLabel *initLabel;

    QPushButton *openFileButton;

    int lastVolume = 0;

    void setInitPic(); // set all the pics

    void initSettingButton(); // set the setting button

    void initWorkSpace(); // init the workplace

private slots:
    void toggleVideoListWidgetState();

    void showEditDurationBox();

};


#endif //FREEWHEELIN_MAINWINDOW_H
