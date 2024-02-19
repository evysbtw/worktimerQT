#ifndef WORKTIMERQUERYMODEL_H
#define WORKTIMERQUERYMODEL_H

#include <QSqlQueryModel>


class WorkTimerQueryModel : public QSqlQueryModel
{
    Q_OBJECT
public:
    WorkTimerQueryModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &item, int role) const override;
};

#endif // WORKTIMERQUERYMODEL_H
