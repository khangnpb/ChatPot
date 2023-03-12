#include "mainwindow.h"
//#include "ui_mainwindow.h"
//#include <QObject>
//#include <Python.h>

//MainWindow::~MainWindow()
//{
//    delete ui;
//}a


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("Chatbot");

    text_edit_ = new QTextEdit (this);
    line_edit_ = new QLineEdit (this);
    send_button_ = new QPushButton( "Send", this);

    text_edit_->setReadOnly(true);

    QVBoxLayout *layout = new QVBoxLayout (this);
    layout->addWidget(text_edit_);
    layout->addWidget(line_edit_);
    layout->addWidget(send_button_);

    QWidget *central_widget = new QWidget(this);
    central_widget->setLayout(layout);
    setCentralWidget(central_widget);

    QObject::connect(send_button_, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(line_edit_, &QLineEdit::returnPressed, this, &MainWindow::send_message);

    api_key_ = "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";
    chatbot_id_ = "davinci";
    url_ = "https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions";
}

void MainWindow::send_message()
{
    QObject::disconnect(send_button_, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::disconnect(line_edit_, &QLineEdit::returnPressed, this, &MainWindow::send_message);

    QString user_input = line_edit_->text();
    line_edit_->clear();

    add_message("You", user_input);

//    // Initialize Python interpreter
//       Py_Initialize();

//       // Import module and function
//       PyObject *pModule = PyImport_ImportModule("your_module_name");
//       PyObject *pFunc = PyObject_GetAttrString(pModule, "generate_text");

//       // Create arguments tuple
//       PyObject *pArgs = PyTuple_New(1);
//       PyTuple_SetItem(pArgs, 0, PyUnicode_FromString("your_prompt_text"));

//       // Call function and get result
//       PyObject *pResult = PyObject_CallObject(pFunc, pArgs);

//       // Parse result
//       const char *result = PyUnicode_AsUTF8(pResult);

//       // Cleanup
//       Py_DECREF(pModule);
//       Py_DECREF(pFunc);
//       Py_DECREF(pArgs);
//       Py_DECREF(pResult);

//       Py_Finalize();

    QString response = generate_text(user_input, 50, 1, 0.5);

    add_message("Chatbot", response); //đưa tin nhắn của chatbot lên

    QObject::connect(send_button_, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(line_edit_, &QLineEdit::returnPressed, this, &MainWindow::send_message);
}

void MainWindow::add_message(const QString &name, const QString &message)
{
    QString formatted_message = QString("<b>%1:</b> %2").arg(name).arg(message);
    text_edit_->append(formatted_message);
}

QByteArray MainWindow::post_data(const QString &text)
{
    QJsonObject json;
    json["prompt"] = text;
    json["model"] = "text-davinci-002";
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
