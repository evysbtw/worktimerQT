#include "infopage.h"
#include "ui_infopage.h"
#include "databasetools.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSqlQueryModel>
#include <QTableView>
#include "worktimerquerymodel.h"


infopage::infopage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::infopage),
    queryModel(new WorkTimerQueryModel(this))
{
    ui->setupUi(this);
    connect(ui->goBackButton, SIGNAL(clicked()), this, SLOT(go_back()));

    QSqlDatabase db = DatabaseTools::database();
    if (!db.open()) {
        qWarning() << "Failed to connect to database:" << db.lastError().text();
    }

    QSettings loginSettings("Worktimer");
    QString email = loginSettings.value("email").toString();

    static QRegularExpression nonAlphanumericRegex("[^a-zA-Z0-9_]", QRegularExpression::CaseInsensitiveOption);

    queryModel->setQuery(QString("SELECT date, status, start_time, end_time, required_time, duration, balance, total_balance FROM `%1`").arg(email.remove(nonAlphanumericRegex)));


    if (queryModel->lastError().isValid()) {
        qWarning() << "Failed to execute query:" << queryModel->lastError().text();
    }


    queryModel->setHeaderData(0, Qt::Horizontal, "Date");
    queryModel->setHeaderData(1, Qt::Horizontal, "Status");
    queryModel->setHeaderData(2, Qt::Horizontal, "Start Time");
    queryModel->setHeaderData(3, Qt::Horizontal, "End Time");
    queryModel->setHeaderData(4, Qt::Horizontal, "Required Time");
    queryModel->setHeaderData(5, Qt::Horizontal, "Duration");
    queryModel->setHeaderData(6, Qt::Horizontal, "Balance");
    queryModel->setHeaderData(7, Qt::Horizontal, "Total Balance");

//    QFont font = ui->infoTable->font();
//    font.setPointSize(10);  // Set the desired font size
//    ui->infoTable->setFont(font);
    ui->infoTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->infoTable->setModel(queryModel);
    ui->infoTable->show();
}

void infopage::go_back() {
    this->close();
    emit goBackSignal();
}

infopage::~infopage() {
    delete ui;
}
