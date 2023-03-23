#include "first.h"
#include "ui_first.h"
#include <QPixmap>

First::First(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::First)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/ai.png"));

    QPixmap pix(":/image/ai.png");
    int w = ui->label_pic->width();
    int h = ui->label_pic->height();
    ui->label_pic->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));
}

First::~First()
{
    delete ui;
}

void First::on_pushButton_clicked()
{
//    MainWindow bot;
//    bot.setModal(true);
//    bot.exec();
    hide();
    mainwindow = new MainWindow(this);
    mainwindow->show();
}

