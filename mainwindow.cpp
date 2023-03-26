#include "mainwindow.h"
#include <QDebug>
#include <QTextImageFormat>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), python_process(new QProcess(this))
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/image/ai.png"));
    setWindowTitle("QtBot");

    ui->text_edit_->setReadOnly(true);
    ui->send_button->setIcon(QIcon(":/image/send.png"));
    ui->send_button->setLayoutDirection(Qt::RightToLeft);
    ui->clean_button->setIcon(QIcon(":/image/clean.png"));

//    QString path = QDir::currentPath() + "/chatgpt.py";
    QString path =  QCoreApplication :: applicationDirPath ()  + "/chatgpt.py"; //sử dụng cách này sẽ deploy được một file độc lập
//    QMessageBox::warning(this, "Crashed!",path);

    QStringList arguments;

//    arguments << "C:/Users/tuankiet/Desktop/ChatPot/Python/chatgpt.py";
    arguments << path;

    python_process->start("python", arguments);
    python_process->setProcessChannelMode(QProcess::MergedChannels);

    if (python_process->state() == QProcess::NotRunning)
    {
        QMessageBox::warning(this, "Crashed!", "Your process is not running");
    }

    QObject::connect(python_process,  &QProcess::readyReadStandardOutput, this, &receive_response);
    QObject::connect(ui->send_button, &QPushButton::clicked,              this, &MainWindow::send_message);
    QObject::connect(ui->line_edit,   &QLineEdit::returnPressed,          this, &MainWindow::send_message);

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

    QObject::connect(ui->send_button, &QPushButton::clicked, this, &MainWindow::send_message);
    QObject::connect(ui->line_edit, &QLineEdit::returnPressed, this, &MainWindow::send_message);
}


void MainWindow::add_message(const QString &name, const QString &message)
{
QString cheminImage;
if (name == "You") cheminImage = ":/image/User2.png";
else cheminImage = ":/image/ai.png";
QString texteFinal = ui->text_edit_->toHtml() + "<img src = \""+ cheminImage +"\" alt = \"\" width = '35' height = '35' \>";
    ui->text_edit_->setHtml(texteFinal);

    QString formatted_message = QString("%0").arg(message);
    ui->text_edit_->append(formatted_message);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QApplication::exit();
}

void MainWindow::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


MainWindow::~MainWindow()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}

