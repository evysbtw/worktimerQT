#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include "signuppage.h"

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();

private:
    Ui::LoginPage *ui;
    signuppage *signupPage;

signals:
    void loginSubmitted();

private slots:
    void logging_in();
    void to_signup_page();
};

#endif // LOGINPAGE_H
