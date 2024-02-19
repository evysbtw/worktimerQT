#include "worktimerquerymodel.h"
#include <cstdlib>
#include <QDebug>

WorkTimerQueryModel::WorkTimerQueryModel(QObject *parent) : QSqlQueryModel(parent)
{

}

QVariant WorkTimerQueryModel::data(const QModelIndex &item, int role) const
{

    if ((item.column()==0 && role == Qt::DisplayRole)) {

    }

    if ((item.column() == 4 || item.column() == 5) && role == Qt::DisplayRole)
    {
        int durationInSeconds = QSqlQueryModel::data(item).toInt();
        int hours = durationInSeconds / 3600;
        int minutes = (durationInSeconds % 3600) / 60;
        int seconds = durationInSeconds % 60;


        return QString("%1:%2:%3").arg(hours, 2, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'));
    }

    else if ((item.column() == 6 || item.column() == 7) && role == Qt::DisplayRole)
    {
        int durationInSeconds = QSqlQueryModel::data(item).toInt();

        QString sign = (durationInSeconds < 0) ? "-" : "";
        durationInSeconds = abs(durationInSeconds);
//        int absoluteDuration = abs(durationInSeconds);
//        qDebug() << "Absolute duration:" << absoluteDuration;

        int hours = durationInSeconds / 3600;
        int minutes = (durationInSeconds % 3600) / 60;
        int seconds = durationInSeconds % 60;


        return QString("%4%1:%2:%3").arg(hours, 3, 10, QChar('0'))
            .arg(minutes, 2, 10, QChar('0'))
            .arg(seconds, 2, 10, QChar('0'))
            .arg(sign);
    }



    return QSqlQueryModel::data(item, role);
}
