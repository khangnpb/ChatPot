#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QtNetwork>
#include <QJsonObject>
#include <QJsonDocument>
#include <QProcess>
#include <QMessageBox>
#include <QStringList>
#include <QThread>
#include <QDebug>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    Ui::MainWindow *ui;
    QProcess *python_process;

    QString api_key_;
    QString chatbot_id_;
    QString url_;
    QByteArray ouput{};
    volatile bool isResponding;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void add_message(const QString &name, const QString &message);
    QByteArray post_data(const QString &text);
    void closeEvent(QCloseEvent *event);
    void delay();
    QString generate_text(QString prompt, int max_tokens, int n, float temperature);

private slots:
    void send_message();
    void receive_response();
};



#endif // MAINWINDOW_H
