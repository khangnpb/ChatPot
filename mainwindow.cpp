#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), python_process(new QProcess(this))
{
    ui->setupUi(this);

    setWindowTitle("Chatbot");

    ui->text_edit_->setReadOnly(true);
//    ui->send_button_->resize();
    ui->send_button->setIcon(QIcon(":/image/send.png"));
    ui->send_button->setLayoutDirection(Qt::RightToLeft);
    ui->clean_button->setIcon(QIcon(":/image/clean.png"));

    QObject::connect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);


    QStringList arguments;
    arguments << "C:/Users/tuankiet/Desktop/ChatPot/Python/chatgpt.py";
    python_process->start("python", arguments);
    python_process->setProcessChannelMode(QProcess::MergedChannels);

    QObject::connect(&process, &QProcess::readyReadStandardOutput,
                     [&process, &output](){
                            output = process.readAllStandardOutput();
                        });

    if (process.state() == QProcess::NotRunning)
    {
        QMessageBox::warning(this, "error", "your process is not running");
    }
//    if (!process.waitForStarted())
//    {
//        QMessageBox::warning(this, "error", "your process is crashed");
//    }



//    python_process->write("hello", qstrlen("hello"));

//    python_process->waitForFinished(-1);
//    QByteArray output = python_process->readAllStandardOutput();
//    qDebug() << output;

//    // Write some input to the script
//    python_process->write("hello", qstrlen("hello"));

//    // Wait for the script to finish
//    if (!python_process->waitForFinished())
//    {
//        qDebug() << "Error: Could not finish process 2";
//        //QMessageBox::warning(this, "error", "your process is not running");
//    }

//    python_process->waitForFinished(-1);
//    // Read the final output from the script
//    output = python_process->readAllStandardOutput();
//    qDebug() << output;


    //đoạn test xem đọc có được không
//    add_message("You", "hellol");
//    python_process->write("hello", qstrlen("hello"));
//    python_process->waitForFinished(-1);
//    QString response ;//= python_process->readChannel();
//    add_message("Chatbot", response); //đưa tin nhắn của chatbot lên



    api_key_ = "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";
    chatbot_id_ = "davinci";
    url_ = "https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions";
}

MainWindow::~MainWindow()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}


void MainWindow::send_message()
{
    QString user_input = ui->line_edit->text();
    if (user_input == "") return;

    QObject::disconnect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::disconnect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);

    ui->line_edit->clear();

    add_message("You", user_input);

    auto data = user_input.toLocal8Bit().data();
    python_process->write(data, qstrlen(data));
    python_process->waitForFinished(-1);
    QString response = python_process->readAllStandardOutput();


//    QString response = generate_text(user_input, 50, 1, 0.5);

    add_message("Chatbot", response); //đưa tin nhắn của chatbot lên

    QObject::connect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);
}

void MainWindow::add_message(const QString &name, const QString &message)
{
    QString formatted_message = QString("<b>%1:</b> %2").arg(name).arg(message);
    ui->text_edit_->append(formatted_message);
}

QByteArray MainWindow::post_data(const QString &text)
{
    QJsonObject json;
    json["prompt"] = text;
    json["model"] = "gpt-3.5-turbo";
    QJsonDocument json_doc(json);
    return json_doc.toJson();
}

//prompt là input của mình đưa cho chatgpt, đưa chuỗi "prompt" vào rồi đọc từ chatgpt
//max_tokens là số lượng ký tự tối đa mà chatgpt trả lời
QString MainWindow::generate_text(QString prompt, int max_tokens, int n, float temperature)
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions"));
    request.setRawHeader("Authorization", ("Bearer " + api_key_).toUtf8());
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject prompt_obj;
    prompt_obj["prompt"] = prompt;
    prompt_obj["temperature"] = temperature;
    prompt_obj["max_tokens"] = max_tokens;
    prompt_obj["top_p"] = 1;
    prompt_obj["frequency_penalty"] = 0.5;
    prompt_obj["n"] = n;

    QJsonObject data_obj;
    data_obj["prompt"] = prompt;
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
