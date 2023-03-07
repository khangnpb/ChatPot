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
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle("Chatbot");

        text_edit_ = new QTextEdit;
        line_edit_ = new QLineEdit;
        send_button_ = new QPushButton("Send");

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(text_edit_);
        layout->addWidget(line_edit_);
        layout->addWidget(send_button_);

        QWidget *central_widget = new QWidget;
        central_widget->setLayout(layout);
        setCentralWidget(central_widget);

        QObject::connect(send_button_, &QPushButton::clicked, this, &MainWindow::send_message);

        api_key_ = "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";
        chatbot_id_ = "text-babbage-001";
        url_ = "https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions";
    }

private slots:

    void send_message()
    {
        QString user_input = line_edit_->text();
        add_message("You", user_input);

        QString response = generate_text(user_input, 50, 1, 0.5);

        add_message("Chatbot", response);
        line_edit_->clear();
    }

    void add_message(const QString &name, const QString &message)
    {
        QString formatted_message = QString("<b>%1:</b> %2").arg(name).arg(message);
        text_edit_->append(formatted_message);
    }

    QByteArray post_data(const QString &text)
    {
        QJsonObject json;
        json["text"] = text;
        json["model"] = "text-davinci-002";
        QJsonDocument json_doc(json);
        return json_doc.toJson();
    }

private:
    QTextEdit *text_edit_;
    QLineEdit *line_edit_;
    QPushButton *send_button_;
    QString api_key_;
    QString chatbot_id_;
    QString url_;

    QString generate_text(QString prompt, int max_tokens, int n, float temperature)
       {
           QNetworkAccessManager manager;
           QNetworkRequest request(QUrl("https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions"));
           request.setRawHeader("Authorization", ("Bearer " + api_key_).toUtf8());
           request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

           QJsonObject prompt_obj;
           prompt_obj["text"] = prompt;
           prompt_obj["temperature"] = temperature;
           prompt_obj["max_tokens"] = max_tokens;
           prompt_obj["top_p"] = 1;
           prompt_obj["frequency_penalty"] = 0.5;
           prompt_obj["n"] = n;

           QJsonObject data_obj;
           data_obj["prompt"] = prompt_obj;
           data_obj["stop"] = "\n";

           QJsonDocument doc(data_obj);
           QByteArray post_data = doc.toJson(QJsonDocument::Compact);

           QNetworkReply* reply = manager.post(request, post_data);

           while (!reply->isFinished()) {
               qApp->processEvents();
           }

           if (reply->error() != QNetworkReply::NoError) {
               qDebug() << "Reply:" << QString(reply->readAll());
               qDebug() << "Error:" << reply->errorString();
               return "Error";
           }

           QString response = QString::fromUtf8(reply->readAll());
           QJsonObject response_obj = QJsonDocument::fromJson(response.toUtf8()).object();
           QString text = response_obj.value("choices").toArray()[0].toObject().value("text").toString();

           delete reply;
           return text;
       }
};
#endif // MAINWINDOW_H
