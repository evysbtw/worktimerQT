#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <string>
#include <QMainWindow>
#include "databasetools.h"
#include "loginpage.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    std::string working_status;
    DatabaseTools dbTools;
    LoginPage *loginPage;
    bool loginCompleted;
    QTime startedTime;
    QTime pausedTime;
    int requiredTime = 28800;
    QTimer *timer;

private slots:
    void start_session();
    void end_session();
    void paused_session();
    void unpaused_session();
    void show_infopage();
    std::string get_time();
//    QString getYear(QDate fullDate);
//    void recievedEmail(const QString email);
    void showEvent(QShowEvent *event);
    void logOut();

public slots:
    void loginSuccesful();
//    void backToMainWindow();
    void showTime();

};
#endif // MAINWINDOW_H
