#ifndef SIGNUPPAGE_H
#define SIGNUPPAGE_H

#include <QDialog>
#include "databasetools.h"
namespace Ui {
class signuppage;
}

class signuppage : public QDialog
{
    Q_OBJECT

public:
    explicit signuppage(QWidget *parent = nullptr);
    ~signuppage();

signals:
    void emailEntered(const QString& email);
    void signupSuccessful();

private:
    Ui::signuppage *ui;
    DatabaseTools dbTools;
private slots:
    void sign_up();
    void handleEmailSubmission();
};



#endif // SIGNUPPAGE_H
