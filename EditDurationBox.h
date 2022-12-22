

#ifndef FREEWHEELIN_EDITDURATIONBOX_H
#define FREEWHEELIN_EDITDURATIONBOX_H

#include <QDialog>


class EditDurationBox : public QDialog {
Q_OBJECT
public:
    explicit EditDurationBox(const QString& duration, QWidget *parent = nullptr);

    ~EditDurationBox() override;

private slots:
    void saveClicked();

    void cancelClicked();
};


#endif //FREEWHEELIN_EDITDURATIONBOX_H
