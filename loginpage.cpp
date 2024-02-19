#include "loginpage.h"
#include "ui_loginpage.h"
#include "databasetools.h"
#include "signuppage.h"


LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);

    signupPage = new signuppage(this);
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(logging_in()));
    connect(ui->signup_button, SIGNAL(clicked()), this, SLOT(to_signup_page()));
    connect(signupPage, &signuppage::signupSuccessful, this, &LoginPage::loginSubmitted);
    ui->password_editline->setEchoMode(QLineEdit::Password);
}

LoginPage::~LoginPage()
{
    delete ui;
}


void LoginPage::logging_in() {
    QSqlDatabase db = DatabaseTools::database();
    QString email = ui->email_editline->text();
    QString password = ui->password_editline->text();

    if (db.open()) {
        qDebug() << "Connection successful";
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE email = :email");
        query.bindValue(":email", email);
        if (query.exec() && query.next()) {

            QString storedPassword = query.value("password").toString();
            if (password == storedPassword) {
                qDebug() << "Login successful";

                QSettings loginSettings("Worktimer");
                loginSettings.setValue("email", email);
                loginSettings.setValue("password", password);

                emit loginSubmitted();
                this->close();
            } else {
                qDebug() << "Login failed: Incorrect password";
            }
        } else {
            qDebug() << "Login failed: User not found";
        }
    }
    else{
        qDebug() << "Connection failed";
    }
}




void LoginPage::to_signup_page() {
    signupPage->show();
    this->hide();
}

