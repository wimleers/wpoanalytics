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


#include "TestFPTree.h"
#include "TestConstraints.h"
#include "TestFPGrowth.h"
#include "TestRuleMiner.h"
#include "TestTTWDefinition.h"
#include "TestTiltedTimeWindow.h"
#include "TestPatternTree.h"
#include "TestFPStream.h"

namespace Analytics {
void tests() {
    TestFPTree FPTree;
    QTest::qExec(&FPTree);

    TestConstraints constraints;
    QTest::qExec(&constraints);

    TestFPGrowth FPGrowth;
    QTest::qExec(&FPGrowth);

    TestRuleMiner ruleMiner;
    QTest::qExec(&ruleMiner);

    // FP-Stream related classes & tests.
    TestTTWDefinition ttwDefinition;
    QTest::qExec(&ttwDefinition);

    TestTiltedTimeWindow tiltedTimeWindow;
    QTest::qExec(&tiltedTimeWindow);

    TestPatternTree patternTree;
    QTest::qExec(&patternTree);

    TestFPStream FPStream;
    QTest::qExec(&FPStream);
}
}
