#ifndef INFOPAGE_H
#define INFOPAGE_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class infopage;
}

class infopage : public QWidget
{
    Q_OBJECT

public:
    explicit infopage(QWidget *parent = nullptr);
    ~infopage();

private:
    Ui::infopage *ui;
    QSqlQueryModel *queryModel;

private slots:
    void go_back();

signals:
    void goBackSignal();
};



#endif // INFOPAGE_H


