#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <string>
#include <QMainWindow>
#include "databasetools.h"
#include "infopage.h"
#include "loginpage.h"
#include <QStackedWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStackedWidget *stackedWidget, QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QString working_status;
    DatabaseTools dbTools;
    LoginPage *loginPage;
    infopage *infoPage;
    bool loginCompleted;
    QTime startedTime;
    QTime pausedTime;
    int requiredTime = 28800;
    QTimer *timer;
    QStackedWidget *stackedWidget;

private slots:
    void start_session();
    void end_session();
    void paused_session();
    void unpaused_session(const QString& mode);

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
    void aboutToQuitHandler();

};
#endif // MAINWINDOW_H
