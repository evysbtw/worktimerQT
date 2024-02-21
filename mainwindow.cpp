#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "databasetools.h"
#include "loginpage.h"
#include "infopage.h"
#include <ctime>
#include <string>
#include <QtSql>
#include <QDebug>
#include <QEvent>
#include <QSettings>
#include <QRegularExpression>


using namespace std;

MainWindow::MainWindow(QStackedWidget *stackedWidget, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    stackedWidget(stackedWidget)
{
    ui->setupUi(this);

    setFixedSize(800, 600);

    loginPage = new LoginPage(stackedWidget);
    stackedWidget->addWidget(loginPage);

    connect(ui->start_button, SIGNAL(clicked()), this, SLOT(start_session()));
    connect(ui->end_button, SIGNAL(clicked()), this, SLOT(end_session()));
    connect(ui->pause_button, SIGNAL(clicked()), this, SLOT(paused_session()));
    connect(ui->unpause_button, &QPushButton::clicked, this, [this]() {unpaused_session("Default"); });
    connect(ui->logOutButton, SIGNAL(clicked()), this, SLOT(logOut()));
    connect(ui->info_button, SIGNAL(clicked()), this, SLOT(show_infopage()));

    connect(loginPage, &LoginPage::loginSubmitted, this, &MainWindow::loginSuccesful);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::aboutToQuitHandler);

    ui->start_button->setEnabled(true);
    ui->end_button->setEnabled(false);
    ui->pause_button->setEnabled(false);
    ui->unpause_button->setEnabled(false);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

}

MainWindow::~MainWindow()
{
    delete ui;
}



QString sanitizeString(const QString &input) {
    static QRegularExpression nonAlphanumericRegex("[^a-zA-Z0-9_]", QRegularExpression::CaseInsensitiveOption);
    QString sanitizedString = input;
    sanitizedString.remove(nonAlphanumericRegex);
    return sanitizedString;
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();
    ui->id_label->setText(QString("ID: %1").arg(email));

    QTime time = QTime::currentTime();
    QString text_time = time.toString("hh : mm : ss");
    ui->time_label->setText(text_time);

    if (email.isEmpty() && !loginCompleted) {
        qDebug() << "Hiding Main window...";
        QTimer::singleShot(0, this, [this](){
            stackedWidget->setCurrentIndex(1);
        });
    }
}

void MainWindow::start_session() {
    std::string date = get_time();
    QString qStringDate = QString::fromStdString(date);

    this->working_status = "Active";

    QDate qDate = QDate::fromString(qStringDate.left(10), "yyyy-MM-dd");
    QTime qTime = QTime::fromString(qStringDate.mid(11), "hh:mm:ss");

    this->startedTime = qTime;
    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();
    dbTools.createUserTableIfNotExists(email);
    dbTools.setStart(qDate, qTime, email, "Working", requiredTime);
    qDebug() << requiredTime;

    ui->start_button->setEnabled(false);
    ui->end_button->setEnabled(true);
    ui->pause_button->setEnabled(true);
    ui->unpause_button->setEnabled(false);

    ui->status_label->setText("Status: Active");
}

void MainWindow::end_session() {
    std::string date = get_time();
    this->working_status = "Offline";

    QString qStringDate = QString::fromStdString(date);
    QDate qDate = QDate::fromString(qStringDate.left(10), "yyyy-MM-dd");
    QTime qTime = QTime::fromString(qStringDate.mid(11), "hh:mm:ss");

    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();
    QTime id = this->startedTime;

    dbTools.setStop(qDate, qTime, email, id);

    ui->start_button->setEnabled(true);
    ui->end_button->setEnabled(false);
    ui->pause_button->setEnabled(false);
    ui->unpause_button->setEnabled(false);

    ui->status_label->setText("Status: Offline");

    int duration = dbTools.calculateDuration(email, id);
    int balance = dbTools.calculateBalance(email, duration, requiredTime, qDate);
    int totalBalance = dbTools.calculateTotalBalance(email, id, balance, qDate, duration, "Working");

    dbTools.setDurationBalanceTotal(duration, balance, totalBalance, email, id);

}

void MainWindow::paused_session() {
    std::string date = get_time();
    QString qStringDate = QString::fromStdString(date);

    this->working_status = "Paused";

    QDate qDate = QDate::fromString(qStringDate.left(10), "yyyy-MM-dd");
    QTime qTime = QTime::fromString(qStringDate.mid(11), "hh:mm:ss");
    this->pausedTime = qTime;
    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();
    dbTools.setStop(qDate, qTime, email, this->startedTime);

    ui->status_label->setText("Status: Paused");
    int duration = dbTools.calculateDuration(email, this->startedTime);
    int balance = dbTools.calculateBalance(email, duration, requiredTime, qDate);
    int totalBalance = dbTools.calculateTotalBalance(email, this->startedTime, balance, qDate, duration, "Working");

    dbTools.setDurationBalanceTotal(duration, balance, totalBalance, email, this->startedTime);

    ui->start_button->setEnabled(false);
    ui->end_button->setEnabled(false);
    ui->pause_button->setEnabled(false);
    ui->unpause_button->setEnabled(true);

    dbTools.setStart(qDate, qTime, email, "Pause", 0);
}

void MainWindow::unpaused_session(const QString& mode) {
    std::string date = get_time();
    this->working_status = "Active";

    QString qStringDate = QString::fromStdString(date);
    QDate qDate = QDate::fromString(qStringDate.left(10), "yyyy-MM-dd");
    QTime qTime = QTime::fromString(qStringDate.mid(11), "hh:mm:ss");

    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();

    dbTools.setStop(qDate, qTime, email, this->pausedTime);

    this->startedTime = qTime;

    ui->status_label->setText("Status: Active");

    int duration = dbTools.calculateDuration(email, this->pausedTime);
    int balance = 0;
    int totalBalance = dbTools.calculateTotalBalance(email, this->pausedTime, balance, qDate, duration, "Pause");

    dbTools.setDurationBalanceTotal(duration, balance, totalBalance, email, this->pausedTime);

    ui->start_button->setEnabled(false);
    ui->end_button->setEnabled(true);
    ui->pause_button->setEnabled(true);
    ui->unpause_button->setEnabled(false);

    if (mode == "Default") {
        dbTools.setStart(qDate, qTime, email, "Working", requiredTime);
    }
}

void MainWindow::show_infopage() {
    infopage *infoPage = new infopage;
    infoPage->show();
    this->hide();

    connect(infoPage, &infopage::goBackSignal, this, [=]() {
        this->show();
    });
}

QSqlDatabase& DatabaseTools::database () {
    static QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("worktimer");
    db.setUserName("root");
    db.setPassword("mohasql");
    return db;
}


void MainWindow::loginSuccesful() {
    loginCompleted = true;
    this->show();
}

void DatabaseTools::setStart(QDate date, QTime time, QString email, QString status, int requiredTime) {
    QSqlDatabase db = DatabaseTools::database();
    QString sanitizedEmail = sanitizeString(email);
    if (db.open()) {
        qDebug() << "setStart: Connected to the database";
        QSqlQuery query;
        QString insertQuery;

        insertQuery = QString("INSERT INTO `%1` (date, start_time, status, required_time) VALUES (:date, :time, :status, :requiredTime)").arg(sanitizedEmail);

        query.prepare(insertQuery);
        query.bindValue(":date", date.toString(Qt::ISODate));
        query.bindValue(":time", time.toString("HH:mm:ss"));
        query.bindValue(":status", status);
        query.bindValue(":requiredTime", requiredTime);

        if (query.exec()) {
            qDebug() << "StartTime inserted successfully";
        } else {
            qDebug() << "Insertion error: " << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "setStart: Database connection error: " << db.lastError().text();
    }
}

void DatabaseTools::setStop(QDate date, QTime time, QString email, QTime id) {
    QSqlDatabase db = DatabaseTools::database();
    QString sanitizedEmail = sanitizeString(email);
    if (db.open()) {
        qDebug() << "setStop: Connected to the database";
        QSqlQuery query;
        QString insertQuery;

        insertQuery = QString("UPDATE `%1` SET end_time = :time WHERE start_time = :starttime").arg(sanitizedEmail);

        query.prepare(insertQuery);
        query.bindValue(":time", time.toString("HH:mm:ss"));
        query.bindValue(":starttime", id);

        if (query.exec()) {
            qDebug() << "endTime inserted successfully";
        } else {
            qDebug() << "setStop: Insertion error: " << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "setStop: Database connection error: " << db.lastError().text();
    }
}


std::string MainWindow::get_time() {
    std::time_t currentTime = std::time(nullptr);

    std::tm *localTime = std::localtime(&currentTime);

    std::string formattedDateTime;
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localTime);

    return buffer;
}


void MainWindow::logOut() {
    QSettings loginSettings("Worktimer");
    loginSettings.clear();
    loginCompleted = false;

    if (working_status == "Active") {
        end_session();
    }
    if (working_status == "Paused") {
        unpaused_session("QuitHandlingMode");
    }

    QString email = loginSettings.value("email").toString();
    if (email.isEmpty() && !loginCompleted) {
        qDebug() << "Hiding Main window...";
        QTimer::singleShot(0, this, [this](){
            this->hide();
            loginPage->show();
        });
    }
    QSqlDatabase db = DatabaseTools::database();
    db.close();

}


int DatabaseTools::calculateDuration(QString email, QTime startTime) {
    QString sanitizedEmail = sanitizeString(email);

    QSqlDatabase db = DatabaseTools::database();
    if (db.open()) {
        qDebug() << "CalculateDuration: Connected to the database";
        QSqlQuery query;

        QString insertQuery = QString("SELECT end_time FROM `%1` WHERE start_time = :startTime").arg(sanitizedEmail);
        query.prepare(insertQuery);
        query.bindValue(":startTime", startTime);


        if (query.exec()) {
            if (query.next()) {
                QTime endTime = query.value(0).toTime();
                int duration = startTime.secsTo(endTime);
                return duration;
            }
        }
        else {
            qDebug() << "Duration query not executed" << query.lastError().text();

            return 0;
        }
    }
    else {
        qDebug() << "CalculateDuration: Connection failed";
        return 0;
    }
    return 0;
}

int DatabaseTools::calculateBalance(QString email, int duration, int requiredTime, QDate date) {
    QSqlDatabase db = DatabaseTools::database();
    QString sanitizedEmail = sanitizeString(email);
    if (db.open()) {
        QSqlQuery query;

        QString idQuery = QString("SELECT id FROM `%1` WHERE date = :date AND status = :status ORDER BY id DESC").arg(sanitizedEmail);
        query.prepare(idQuery);
        query.bindValue(":date", date);
        query.bindValue(":status", "Working");

        qDebug() << "Db opened, calculating balance..";

        if (query.exec()) {
            if (query.size() > 1) {
                qDebug() << "Query size > 1, updating balance..";
                int id;
                int i = 0;
                while (query.next() && i < 2) {
                    id = query.value(0).toInt();
                    i++;
                }
                QString balanceQuery = QString("SELECT balance FROM `%1` WHERE id = :id").arg(sanitizedEmail);
                query.prepare(balanceQuery);
                query.bindValue(":id", id);
                if (query.exec()) {
                    if (query.next()) {
                        int balance = query.value(0).toInt();
                        qDebug() << "Todays balance: " << balance;
                        int newbalance = balance + duration;
                        qDebug() << "New balance: " << newbalance;
                        return newbalance;
                    }
                }
            }
            else {
                qDebug() << "Query size = 1, updating balance..";
                int balance = duration - requiredTime;
                return balance;
            }
        }
    }
    else {
        return 0;
    }
    return 0;
}

int DatabaseTools::calculateTotalBalance(QString email, QTime startTime, int balance, QDate date, int duration, QString status) {
    QString sanitizedEmail = sanitizeString(email);

    QSqlDatabase db = DatabaseTools::database();
    QSqlQuery query;
    if (db.open()) {
        qDebug() << "CalculateTotalBalance: Connected to the database";

        QString idQuery = QString("SELECT id FROM `%1` WHERE date = :date ORDER BY id DESC").arg(sanitizedEmail);
        query.prepare(idQuery);
        query.bindValue(":date", date);


        if (query.exec()) {
            if (query.size() > 1) {
                int id;
                int i = 0;
                while (query.next() && i < 2) {
                    id = query.value(0).toInt();
                    i++;
                }
                QString balanceQuery = QString("SELECT total_balance FROM `%1` WHERE id = :id").arg(sanitizedEmail);
                query.prepare(balanceQuery);
                query.bindValue(":id", id);

                if (query.exec()) {
                    if (query.next()) {
                        int previousTotalBalance = query.value(0).toInt();
                        if (status == "Working") {
                            int actualTotalBalance = previousTotalBalance + duration;
                            return actualTotalBalance;
                        }
                        else {
                            qDebug() << "Returning previous total_balance";
                            return previousTotalBalance;
                        }

                    }
                }
            }
            else {
                if (query.next()) {
                    int id = query.value(0).toInt();
                    if (id > 1) {
                        int previousTotalBalanceId = id - 1;

                        QString totalBalanceQuery = QString("SELECT total_balance FROM %1 WHERE id = :id").arg(sanitizedEmail);
                        query.prepare(totalBalanceQuery);
                        query.bindValue(":id", previousTotalBalanceId);

                        if (query.exec()) {
                            if (query.next()) {
                                int previousTotalBalance = query.value(0).toInt();
                                int totalBalance = previousTotalBalance + balance;
                                return totalBalance;
                            }
                        }
                    }
                    else {
                        return balance;
                    }
                }
            }
        }
    }
}

void DatabaseTools::setDurationBalanceTotal(int duration, int balance, int totalBalance, QString email, QTime startTime) {
    QSqlDatabase db = DatabaseTools::database();
    QString sanitizedEmail = sanitizeString(email);

    if (db.open()) {
        qDebug() << "setDurationTotalBalance: Connected to the database";
        QSqlQuery query;
        QString insertQuery;

        insertQuery = QString("UPDATE `%1` SET duration = :duration, balance = :balance, total_balance = :total_balance WHERE start_time = :start_time").arg(sanitizedEmail);

        query.prepare(insertQuery);
        query.bindValue(":duration", duration);
        query.bindValue(":balance", balance);
        query.bindValue(":total_balance", totalBalance);
        query.bindValue(":start_time", startTime);

        if (query.exec()) {
            qDebug() << "Data inserted successfully";
        } else {
            qDebug() << "Insertion error: " << query.lastError().text();
        }
        db.close();
    } else {
        qDebug() << "setDurationTotalBalance: Database connection error: " << db.lastError().text();
    }
}

void MainWindow::showTime(){
    QTime time = QTime::currentTime();
    QString text_time = time.toString("hh : mm : ss");
    ui->time_label->setText(text_time);
}

void MainWindow::aboutToQuitHandler() {
    if (working_status == "Active") {
        end_session();
    }
    if (working_status == "Paused") {
        unpaused_session("QuitHandlingMode");
    }

    QSqlDatabase db = DatabaseTools::database();
    db.close();
}





