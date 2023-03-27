#include "mainwindow.h"
//#include <QDebug>


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
    QStringList arguments;
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

    //để có thể lưu lịch sử trò chuyện
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(save_conversation()));

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


void MainWindow::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//void MainWindow::closeEvent(QCloseEvent *event)
//{
//    QApplication::exit();
//}

MainWindow::~MainWindow()
{
    if (ui)
    {
        delete ui;
        ui = nullptr;
    }
}


void MainWindow::on_clean_button_clicked()
{
    ui->text_edit_->clear();
}

void MainWindow::save_conversation()
{
    if (ui->text_edit_->toPlainText().isEmpty()) return;

    QString file_name = QFileDialog::getSaveFileName(this, "Save this conversation", QCoreApplication :: applicationDirPath (), "(*.html)");
    if (file_name == "") return;

    QFile file (file_name, this);

    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this, "Error", "Cannot save the file " + file_name);
        file_name = "";
        file.close();
        return;
    }

    QTextStream out (&file); //cần tìm hiểu dòng này
    QString text = ui->text_edit_->toHtml();
    out << text;
    file.flush();
    file.close();
}

