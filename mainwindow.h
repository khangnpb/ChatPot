#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QtNetwork>
#include <QJsonObject>
#include <QJsonDocument>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    //~MainWindow();

private slots:

    void send_message();

    void add_message(const QString &name, const QString &message);

    QByteArray post_data(const QString &text);

private:

    QTextEdit *text_edit_;
    QLineEdit *line_edit_;
    QPushButton *send_button_;
    QString api_key_;
    QString chatbot_id_;
    QString url_;

    QString generate_text(QString prompt, int max_tokens, int n, float temperature);
};
#endif // MAINWINDOW_H
