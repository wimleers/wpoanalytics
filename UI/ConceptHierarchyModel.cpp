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


#include "ConceptHierarchyModel.h"

ConceptHierarchyModel::ConceptHierarchyModel() {
    this->reset();
}

void ConceptHierarchyModel::update(Analytics::ItemIDNameHash itemIDNameHash) {
    if (itemIDNameHash.size() <= this->itemsAlreadyProcessed)
        return;

    Analytics::ItemName item, parent, child;
    QStringList parts;
    for (int id = this->itemsAlreadyProcessed; id < itemIDNameHash.size(); id++) {
        item = itemIDNameHash[(Analytics::ItemID) id];
        parts = item.split(':', QString::SkipEmptyParts);

        // Update the concept hierarchy model.
        parent = parts[0];
        // Root level.
        if (!this->hash.contains(parent)) {
            QStandardItem * modelItem = new QStandardItem(parent);
            modelItem->setData(parent.toUpper(), Qt::UserRole); // For sorting.

            // Store in hierarchy.
            this->hash.insert(parent, modelItem);
            QStandardItem * root = this->invisibleRootItem();
            root->appendRow(modelItem);
        }
        // Subsequent levels.
        for (int p = 1; p < parts.size(); p++) {
            child = parent + ':' + parts[p];
            if (!this->hash.contains(child)) {
                QStandardItem * modelItem = new QStandardItem(parts[p]);
                modelItem->setData(parts[p].toUpper(), Qt::UserRole); // For sorting.

                // Store in hierarchy.
                this->hash.insert(child, modelItem);
                QStandardItem * parentModelItem = this->hash[parent];
                parentModelItem->appendRow(modelItem);
            }
            parent = child;
        }
    }

    // Remember the last item processed.
    this->itemsAlreadyProcessed = itemIDNameHash.size();

    // Sort the model case-insensitively.
    this->setSortRole(Qt::UserRole);
    this->sort(0, Qt::AscendingOrder);
}

void ConceptHierarchyModel::reset() {
    this->itemsAlreadyProcessed = 0;
    this->hash.clear();
}
