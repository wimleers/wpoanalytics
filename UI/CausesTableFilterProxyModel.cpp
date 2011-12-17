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


#include "CausesTableFilterProxyModel.h"


//------------------------------------------------------------------------------
// Public methods.

CausesTableFilterProxyModel::CausesTableFilterProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{
}

void CausesTableFilterProxyModel::setEpisodesColumn(int col) {
    this->episodesColumn = col;
    this->invalidateFilter();
}

void CausesTableFilterProxyModel::setCircumstancesColumn(int col) {
    this->circumstancesColumn = col;
    this->invalidateFilter();
}

void CausesTableFilterProxyModel::setConsequentsColumn(int col) {
    this->consequentsColumn = col;
    this->invalidateFilter();
}

void CausesTableFilterProxyModel::setEpisodeFilter(const QString & filter) {
    this->episodesFilter = QRegExp(filter, Qt::CaseInsensitive, QRegExp::FixedString);
    this->invalidateFilter();
}

void CausesTableFilterProxyModel::setCircumstancesFilter(const QStringList & filter) {
    this->circumstancesFilter.clear();
    foreach (const QString & f, filter)
        this->circumstancesFilter.append(QRegExp(f, Qt::CaseInsensitive, QRegExp::Wildcard));
    this->invalidateFilter();
}

void CausesTableFilterProxyModel::setConsequentsFilter(const QStringList & filter) {
    this->consequentsFilter.clear();
    foreach (const QString & f, filter)
        this->consequentsFilter.append(QRegExp(f, Qt::CaseInsensitive, QRegExp::Wildcard));
    this->invalidateFilter();
}



//------------------------------------------------------------------------------
// Protected methods.

bool CausesTableFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex & sourceParent) const {
    bool episodesColumnMatches = false;
    bool circumstancesColumnMatches = false;
    bool consequentsColumnMatches = false;
    QModelIndex index;

    // Episodes filter.
    index = this->sourceModel()->index(sourceRow, this->episodesColumn, sourceParent);
    episodesColumnMatches = this->sourceModel()->data(index).toString().contains(this->episodesFilter);

    // Circumstances filter.
    index = this->sourceModel()->index(sourceRow, this->circumstancesColumn, sourceParent);
    if (!this->circumstancesFilter.isEmpty()) {
        foreach (const QRegExp & regexp, this->circumstancesFilter) {
            circumstancesColumnMatches = this->sourceModel()->data(index).toString().contains(regexp);
            if (!circumstancesColumnMatches)
                break;
        }
    }
    else {
        // No circumstances filter, hence this column *always* matches.
        circumstancesColumnMatches = true;
    }

    // Consequents filter.
    index = this->sourceModel()->index(sourceRow, this->consequentsColumn, sourceParent);
    if (!this->consequentsFilter.isEmpty()) {
        foreach (const QRegExp & regexp, this->consequentsFilter) {
            consequentsColumnMatches = this->sourceModel()->data(index).toString().contains(regexp);
            if (!consequentsColumnMatches)
                break;
        }
    }
    else {
        // No consequents filter, hence this column *always* matches.
        consequentsColumnMatches = true;
    }

    return episodesColumnMatches && (circumstancesColumnMatches || consequentsColumnMatches);
}
