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


#ifndef TESTCONSTRAINTS_H
#define TESTCONSTRAINTS_H

#include <QtTest/QtTest>
#include <QFile>
#include "../Item.h"

// #define protected public
// #define private   public
#include "../Constraints.h"
// #undef protected
// #undef private


using namespace Analytics;

class TestConstraints : public QObject {
    Q_OBJECT

private slots:
//    void initTestCase() {}
//    void cleanupTestCase() {}
//    void init();
//    void cleanup();
    void basic();
    void basic_data();
    void alternate();
    void alternate_data();
    void alternateWildcard();
    void alternateWildcard_data();
    void noConstraints();
    void edgeCases();
};


Q_DECLARE_METATYPE(QSet<ItemName>)

#endif // TESTCONSTRAINTS_H
