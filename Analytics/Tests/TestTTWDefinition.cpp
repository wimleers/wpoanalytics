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


#include "TestTTWDefinition.h"

void TestTTWDefinition::serialization_data() {
    QTest::addColumn<uint>("secPerWindow");
    QTest::addColumn<MapCharUint>("granularities");
    QTest::addColumn<QList<char> >("order");
    QTest::addColumn<QString>("expected");

    // Default.
    QMap<char, uint> granularitiesDefault;
    granularitiesDefault.insert('Q', 4);
    granularitiesDefault.insert('H', 24);
    granularitiesDefault.insert('D', 31);
    granularitiesDefault.insert('M', 12);
    granularitiesDefault.insert('Y', 1);
    QTest::newRow("default")
            << (uint) 900
            << granularitiesDefault
            << (QList<char>() << 'Q' << 'H' << 'D' << 'M' << 'Y')
            << QString("900:QQQQHHHHHHHHHHHHHHHHHHHHHHHHDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDMMMMMMMMMMMMY");

    // 30 days.
    QMap<char, uint> granularities30Days;
    granularities30Days.insert('H', 24);
    granularities30Days.insert('D', 30);
    QTest::newRow("30 days")
            << (uint) 3600
            << granularities30Days
            << (QList<char>() << 'H' << 'D')
            << QString("3600:HHHHHHHHHHHHHHHHHHHHHHHHDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD");

    // 48 hours.
    QMap<char, uint> granularities48Hours;
    granularities48Hours.insert('H', 48);
    QTest::newRow("48 hours")
            << (uint) 3600
            << granularities48Hours
            << (QList<char>() << 'H')
            << QString("3600:HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH");

}

void TestTTWDefinition::serialization() {
    QFETCH(uint, secPerWindow);
    QFETCH(MapCharUint, granularities);
    QFETCH(QList<char>, order);
    QFETCH(QString, expected);

    TTWDefinition ttwDef(secPerWindow, granularities, order);

    // Verify expected serialization.
    QString serialized = ttwDef.serialize();
    QCOMPARE(serialized, expected);

    // Verify deserialization == serialization.
    TTWDefinition deserialized;
    deserialized.deserialize(serialized);
    QCOMPARE(deserialized, ttwDef);
}
