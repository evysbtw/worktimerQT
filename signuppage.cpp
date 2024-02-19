#include "signuppage.h"
#include "ui_signuppage.h"
#include "databasetools.h"
#include <QtSql>
#include <QDebug>
#include <QSettings>

signuppage::signuppage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::signuppage)
{
    ui->setupUi(this);
    connect(ui->signup_button, SIGNAL(clicked()), this, SLOT(sign_up()));
}

signuppage::~signuppage()
{
    delete ui;
}


void DatabaseTools::createUserTableIfNotExists(QString email) {
    QSqlDatabase db = DatabaseTools::database();
    static QRegularExpression nonAlphanumericRegex("[^a-zA-Z0-9_]", QRegularExpression::CaseInsensitiveOption);
    if (db.open()) {
        qDebug() << "Connected to the database";
        QSqlQuery query;

        QString sanitizedEmail = email.remove(nonAlphanumericRegex);

        QString queryString = QString("CREATE TABLE IF NOT EXISTS `%1` ("
                                      "id INT AUTO_INCREMENT PRIMARY KEY,"
                                      "date DATE,"
                                      "status TEXT,"
                                      "start_time TIME,"
                                      "end_time TIME,"
                                      "required_time INT,"
                                      "duration INT,"
                                      "balance INT,"
                                      "total_balance INT"
                                      ")").arg(sanitizedEmail);
        if (query.exec(queryString)) {
            qDebug() << "Table created successfully";
            qDebug() << sanitizedEmail;

        } else {
            qDebug() << "Error creating table:" << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "Connection failed";
    }
}


void DatabaseTools::signUp(QString email, QString password) {
    QSqlDatabase db = DatabaseTools::database();
    QSettings loginSettings("Worktimer");
    loginSettings.setValue("email", email);
    loginSettings.setValue("password", password);
    createUserTableIfNotExists(email);
    if (db.open()) {
        qDebug() << "Connected to the database";
        QSqlQuery query;
        query.prepare("INSERT INTO users (email, password)" "VALUES (:email, :password)");
        query.bindValue(":email", email);
        query.bindValue(":password", password);

        if (query.exec()) {
            qDebug() << "Data inserted successfully";
        } else {
            qDebug() << "Insertion error: " << query.lastError().text();
        }

        db.close();
    }
    else {
        qDebug() << "Database connection error: " << db.lastError().text();
    }
}

void signuppage::handleEmailSubmission() {
    QString email = ui->email_editline->text();
    emit emailEntered(email);
}

bool checkIfExists(QString email) {
    QSqlDatabase db = DatabaseTools::database();
    if (db.open()) {
        qDebug() << "Connected to the database";
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE email = :email");
        query.bindValue(":email", email);

        if (query.exec()) {
            if (query.size() > 0) {
                qDebug() << "User found, already registered.";
                return 0;
            }
            else {
                qDebug() << "User not found, OK to register";
                return 1;
            }
        } else {
            qDebug() << "Execution error";
            return 0;
        }

        db.close();
    }
    else {
        qDebug() << "Connection failed";
        return 0;
    }
}

bool matchedPasswords(QString password, QString confirm_password) {
    if (password == confirm_password) {
        qDebug() << "Passwords match";
        return 1;
    }
    else {
        return 0;
    }
}

void signuppage::sign_up() {
    QString email = ui->email_editline->text();
    QString password = ui->password_editline->text();
    QString confirm_password = ui->confirm_password->text();
    if (checkIfExists(email)) {
        if (matchedPasswords(password, confirm_password)) {
            dbTools.signUp(email, password);
            emit signupSuccessful();
            this->close();
        }
        else {
            ui->password_label->setText("Passwords do not match!");
        }
    }
    else {
        ui->email_label->setText("This email is already in use");
    }
}


