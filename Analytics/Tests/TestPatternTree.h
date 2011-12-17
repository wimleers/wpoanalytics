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


#ifndef TESTPATTERNTREE_H
#define TESTPATTERNTREE_H

#include <QtTest/QtTest>
#include "../PatternTree.h"

using namespace Analytics;

class TestPatternTree : public QObject {
    Q_OBJECT

private slots:
    void basic();
    void additionsRemainInSync();
    void getFrequentItemsetsForRange();
    void getTotalSupportForRange();

private:
    PatternTree * buildBasicPatternTree();
    static const TTWDefinition getTTWDefinition() {
        QMap<char, uint> granularitiesDefault;
        granularitiesDefault.insert('Q', 4);
        granularitiesDefault.insert('H', 24);
        granularitiesDefault.insert('D', 31);
        granularitiesDefault.insert('M', 12);
        granularitiesDefault.insert('Y', 1);
        TTWDefinition defaultTTWDefinition(3600,
                                           granularitiesDefault,
                                           QList<char>() << 'Q' << 'H' << 'D' << 'M' << 'Y');
        return defaultTTWDefinition;
    }
};

#endif // TESTPATTERNTREE_H
