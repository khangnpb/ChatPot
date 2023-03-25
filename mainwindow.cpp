#include "mainwindow.h"
#include <QDebug>

//#include <QColorDialog>
//#include <QPalette>
//#include <QTextCursor>
//#include <QTextEdit>
//#include <QTextDocumentFragment>
//#include <QUrl>
//#include <QTextDocument>
#include <QTextImageFormat>
//#include <QVariant>
//#include <QImage>
//#include <QImageReader>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), python_process(new QProcess(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/ai.png"));
    setWindowTitle("QtBot");

//     //1

//        QColor color = QColorDialog::getColor(Qt::white,this);
//        ui->text_edit_->setPalette(QPalette(Qt::white));

//     //1

//     //2
//        QPalette palette = ui->text_edit_->palette();

//        QColor color = QColorDialog::getColor(Qt::black,this); // in here your color pallete will open..

//        QPalette p = ui->text_edit_->palette(); // define pallete for textEdit..
//        p.setColor(QPalette::Base, Qt::white); // set color "Red" for textedit base
//        p.setColor(QPalette::Text, color); // set text color which is selected from color pallete
//        ui->text_edit_->setPalette(p); // change textedit palette
//     //2

    ui->text_edit_->setReadOnly(true);
    ui->send_button->setIcon(QIcon(":/image/send.png"));
    ui->send_button->setLayoutDirection(Qt::RightToLeft);
    ui->clean_button->setIcon(QIcon(":/image/clean.png"));

    QStringList arguments;
    arguments << "C:/Users/tuankiet/Desktop/ChatPot/Python/chatgpt.py";
    python_process->start("python", arguments);
    python_process->setProcessChannelMode(QProcess::MergedChannels);

    if (python_process->state() == QProcess::NotRunning)
    {
        QMessageBox::warning(this, "Crashed!", "Your process is not running");
    }

    QObject::connect(python_process,  &QProcess::readyReadStandardOutput, this, &receive_response);
    QObject::connect(ui->send_button, &QPushButton::clicked,              this, &MainWindow::send_message);
    QObject::connect(ui->line_edit,   &QLineEdit::returnPressed,          this, &MainWindow::send_message);
    /* khi nhấn enter, thì tín hiệu này sẽ được phát để đi vào hàm send_message
     * send_message phải handle dược tin nhắn này
     */
}

void MainWindow::receive_response()
{
    this->isResponding = false;
}

void MainWindow::send_message()
{
    QString user_input = ui->line_edit->text();
    if (user_input == "") return;

    QObject::disconnect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::disconnect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);

    ui->line_edit->clear();
    add_message("You", user_input + "\n");

    user_input += "\r\n";
    python_process->write(user_input.toUtf8());
    isResponding = true;

    bool failed = false;

    while (this->isResponding)
    {
        if (python_process->state() == QProcess::NotRunning)
        {
            failed = true;
            break;

        }
        delay(); //wait while chatbot is responding message
    }
    if (failed)
    {
        QMessageBox::warning(this, "Crashed!", "Your process is failed. Please restart!");
        QApplication::exit();
        return;
    }

    QString  response (python_process->readAllStandardOutput() );
    add_message("Chatbot", response); //đưa tin nhắn của chatbot lên
    //qDebug() << response;

    QObject::connect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);
}

////
//void text_edit_::insertImage()
//{
//    QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
//                                  ".", tr("Bitmap Files (*.bmp)\n"
//                                    "JPEG (*.jpg *jpeg)\n"
//                                    "GIF (*.gif)\n"
//                                    "PNG (*.png)\n"));
//    QUrl Uri ( QString ( "file://%1" ).arg ( file ) );
//    QImage image = QImageReader ( file ).read();

//    QTextDocument * textDocument = m_textEdit->document();
//    textDocument->addResource( QTextDocument::ImageResource, Uri, QVariant ( image ) );
//    QTextCursor cursor = m_textEdit->textCursor();
//    QTextImageFormat imageFormat;
//    imageFormat.setWidth( image.width() );
//    imageFormat.setHeight( image.height() );
//    imageFormat.setName( Uri.toString() );
//    cursor.insertImage(imageFormat);
// }
////

void MainWindow::add_message(const QString &name, const QString &message)
{

//    //
//        QTextEdit *textEditor = new QTextEdit(0);
//        QTextDocumentFragment fragment;
//        fragment = QTextDocumentFragment::fromHtml("<img src = ':/image/ai.png'>");
//        textEditor->textCursor().insertFragment(fragment);
//        textEditor->setVisible(true);
//    //

QString cheminImage;
if (name == "You") cheminImage = ":/image/User2.png";
else cheminImage = ":/image/ai.png";
//QString cheminImage = ":/image/ai.png";
QString texteFinal = ui->text_edit_->toHtml() + "<img src = \""+ cheminImage +"\" alt = \"\" width = '35' height = '35' \>";
//texteFinal.setHeight(20);
    ui->text_edit_->setHtml(texteFinal);

//    QString formatted_message = QString("<b>%1: </b> %2").arg(name).arg(message);
    QString formatted_message = QString("%0").arg(message);
    ui->text_edit_->append(formatted_message);
}

void MainWindow::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    //this->add_message("Chatbot", "waiting....");
}


MainWindow::~MainWindow()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}



/*
    api_key_ = "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";
    chatbot_id_ = "davinci";
    url_ = "https://api.openai.com/v1/engines/" + chatbot_id_ + "/completions";
*/
/*
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
*/


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
