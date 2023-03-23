#ifndef FIRST_H
#define FIRST_H

#include <QDialog>
#include "mainwindow.h"
namespace Ui {
class First;
}

class First : public QDialog
{
    Q_OBJECT

public:
    explicit First(QWidget *parent = nullptr);
    ~First();

private slots:
    void on_pushButton_clicked();

private:
    Ui::First *ui;
    MainWindow *mainwindow;
};

#endif // FIRST_H
