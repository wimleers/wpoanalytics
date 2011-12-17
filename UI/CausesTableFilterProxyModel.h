/** 
 * Copyright 2011 Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */ 


#ifndef CAUSESTABLEFILTERPROXYMODEL_H
#define CAUSESTABLEFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QModelIndex>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

class CausesTableFilterProxyModel : public QSortFilterProxyModel {

    Q_OBJECT

public:
    explicit CausesTableFilterProxyModel(QObject * parent = NULL);

    void setEpisodesColumn(int col);
    void setCircumstancesColumn(int col);
    void setConsequentsColumn(int col);

    void setEpisodeFilter(const QString & filter);
    void setCircumstancesFilter(const QStringList & filter);
    void setConsequentsFilter(const QStringList & filter);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const;

private:
    int episodesColumn;
    int circumstancesColumn;
    int consequentsColumn;
    QRegExp episodesFilter;
    QList<QRegExp> circumstancesFilter;
    QList<QRegExp> consequentsFilter;
};

#endif // CAUSESTABLEFILTERPROXYMODEL_H
