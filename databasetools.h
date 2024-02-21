#ifndef DATABASETOOLS_H
#define DATABASETOOLS_H

#include <QSqlDatabase>
#include <QString>
#include <string>
#include <QtSql>
#include <QDebug>

using namespace std;

class DatabaseTools {
public:
    static QSqlDatabase& database();
    static void closeDatabase();
    void setStart(QDate *date, QTime *time, QString *email, QString status, int *requiredTime);
    void setStop(QDate *date, QTime *time, QString *email, QTime *id);
//    void setPause(QDate date, QTime time, QString email, QString year, QString status);
//    void setUnPause(QDate date, QTime time, QString email, QString year, QTime id);
//    void setBalances(QString *email, QTime *id, QTime *balance, QTime *totalBalance);
    void signUp(QString email, QString password);
    void createUserTableIfNotExists(QString email);
    int calculateDuration(QString *email, QTime *startTime);
    int calculateBalance(QString *email, int *duration, int *requiredTime, QDate *date);
    int calculateTotalBalance(QString *email, QTime *startTime, int *balance, QDate *date, int *duration, QString status);
//    void setDuration(int duration, QString email, QString year, QTime startTime);
//    void setBalance(int balance, QString email, QString year, QTime startTime);
//    void setTotalBalance(int totalBalance, QString email, QString year, QTime startTime);
    void setDurationBalanceTotal(int *duration, int *balance, int *totalBalance, QString *email, QTime *startTime);
};

#endif // DATABASETOOLS_H
