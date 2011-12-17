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


#ifndef COMMON_H
#define COMMON_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QMetaType>

typedef quint32 Time;
typedef QStringList RawTransaction;
// TiltedTimeWindow.
typedef int Granularity;
typedef int Bucket;

struct BatchMetadata {
    BatchMetadata() : discardedSamples(0.0) {}

    void setChunkInfo(quint32 batchID,
                      bool isLastChunk,
                      quint32 numDiscardedSamples)
    {
        this->batchID             = batchID;
        this->isLastChunk         = isLastChunk;
        this->discardedSamples = numDiscardedSamples;
    }

    // Time perspective.
    quint32 batchID;
    Time startTime;
    Time endTime;
    bool isLastChunk;

    // Stats.
    quint32 samples;
    quint32 transactions;
    quint32 items;
    double transactionsPerSample;
    double itemsPerTransaction;
    quint32 discardedSamples;
};

// Call with either RawTransaction or Config::Sample.
template <class T>
struct Batch {
    QList<T> data;
    BatchMetadata meta;
};

void registerCommonMetaTypes();

#endif // COMMON_H
