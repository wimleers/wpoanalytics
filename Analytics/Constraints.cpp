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


#include "Constraints.h"

// TODO: Make the distinction between "standard usage" of Constraints (with
// preprocessing, so that we can match ItemIDLists very fast) versus the
// "alternative usage" of Constraints (without preprocessing, but it can work
// directly with strings; with QSet<ItemName>s).

namespace Analytics {

    const char * Constraints::ItemConstraintTypeName[2] = {
        "ItemConstraintPositive",
        "ItemConstraintNegative",
    };

    const char * Constraints::LengthConstraintTypeName[3] = {
        "LengthConstraintEquals",
        "LengthConstraintLessThan",
        "LengthConstraintGreaterThan",
    };

    const ConstraintClassification Constraints::ItemConstraintTypeClassification[2] = {
        ConstraintClassificationMonotone,
        ConstraintClassificationAntimonotone,
    };

    const ConstraintClassification Constraints::LengthConstraintTypeClassification[3] = {
        ConstraintClassificationSuccint,
        ConstraintClassificationSuccint,
        ConstraintClassificationSuccint,
    };


    //--------------------------------------------------------------------------
    // Public methods.

    Constraints::Constraints() {
        this->reset();
    }

    void Constraints::reset() {
        this->itemConstraints.clear();
        this->preprocessedItemConstraints.clear();
        this->highestPreprocessedItemID = ROOT_ITEMID;
    }

    /**
     * Add an item constraint of a given constraint type. When
     * frequent itemsets are being generated, only those will be considered
     * that match the constraints defined here.
     * Wildcards are allowed, e.g. "episode:*" will match "episode:foo",
     * "episode:bar", etc.
     *
     * @param items
     *   An set of item names.
     * @param type
     *   The constraint type.
     */
    void Constraints::addItemConstraint(const QSet<ItemName> & items, ItemConstraintType type) {
        if (items.isEmpty())
            return;

        if (!this->itemConstraints.contains(type))
            this->itemConstraints.insert(type, QVector<QSet<ItemName> >());
        this->itemConstraints[type].append(items);

        this->updatePreprocesedItemConstraintsStructure();
    }

    void Constraints::setItemConstraints(const ItemConstraintsHash & itemConstraints) {
        this->itemConstraints = itemConstraints;
        this->updatePreprocesedItemConstraintsStructure();
    }

    /**
     * Get all item IDs for a given item constraint type. Clearly, this only
     * returns item IDs after all item IDs have been preprocessed.
     *
     * @param type
     *   An item constraint type to get all item IDs for.
     * @return
     *   All item IDs for the given item constraint type.
     */
    QSet<ItemID> Constraints::getAllItemIDsForConstraintType(ItemConstraintType type) const {
        QSet<ItemID> preprocessedItemIDs;

        foreach (const QSet<ItemID> & itemIDs, this->preprocessedItemConstraints[type])
            preprocessedItemIDs.unite(itemIDs);

        return preprocessedItemIDs;
    }

    /**
     * Consider the given item ID -> name hash for use with constraints, but
     * only if its size exceeds the highest preprocessed item, or no items have
     * been preprocessed yet.
     *
     * This method is supposed to be used when all item IDs are already known.
     *
     * @param hash
     *   An item ID -> name hash.
     */
    void Constraints::preprocessItemIDNameHash(const ItemIDNameHash & hash) {
        if (this->highestPreprocessedItemID == ROOT_ITEMID || this->highestPreprocessedItemID + 1 < (uint) hash.size()) {
            this->clearPreprocessedItems();

            foreach (ItemID itemID, hash.keys()) {
                ItemName itemName = hash[itemID];
                this->preprocessItem(itemName, itemID);
            }
        }
    }

    /**
     * Consider the given item for use with constraints: store its item id in
     * an optimized data structure to allow for fast constraint checking
     * during frequent itemset generation.
     *
     * @param name
     *   An item name.
     * @param id
     *   The corresponding item ID.
     */
    void Constraints::preprocessItem(const ItemName & name, ItemID id) {
        QRegExp rx;
        rx.setPatternSyntax(QRegExp::Wildcard);

        // Store the item IDs that correspond to the wildcard item
        // constraints.
        ItemConstraintType constraintType;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            constraintType = (ItemConstraintType) i;

            if (!this->itemConstraints.contains(constraintType))
                continue;

            for (int i = 0; i < this->itemConstraints[constraintType].size(); i++) {
                foreach (ItemName item, this->itemConstraints[constraintType][i]) {
                    // Map ItemNames to ItemIDs.
                    if (item.compare(name) == 0) {
                        this->addPreprocessedItemConstraint(constraintType, i, id);
                    }
                    // Map ItemNames with wildcards in them to *all* corresponding
                    // ItemIDs.
                    else if (item.contains('*')) {
                        rx.setPattern(item);
                        if (rx.exactMatch(name))
                            this->addPreprocessedItemConstraint(constraintType, i, id);
                    }
                }
            }
        }

        // Always keep the highest preprocessed item ID.
        if (this->highestPreprocessedItemID == ROOT_ITEMID || id > this->highestPreprocessedItemID)
            this->highestPreprocessedItemID = id;
    }

    /**
     * Remove the given item id from the optimized constraint storage data
     * structure, because it is infrequent.
     *
     * @param id
     *   The item id to remove.
     */
    void Constraints::removeItem(ItemID id) {
        ItemConstraintType type;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            type = (ItemConstraintType) i;

            if (!this->preprocessedItemConstraints.contains(type))
                continue;

            for (int i = 0; i < this->itemConstraints[type].size(); i++)
                this->preprocessedItemConstraints[type][i].remove(id);
        }
    }

    /**
     * Check if the given itemset matches the defined constraints.
     *
     * @param itemset
     *   An itemset to check the constraints for.
     * @return
     *   True if the itemset matches the constraints, false otherwise.
     */
    bool Constraints::matchItemset(const ItemIDList & itemset,
                                   const QSet<ConstraintClassification> & constraintsSubset) const
    {
#ifdef CONSTRAINTS_DEBUG
        FrequentItemset fis(itemset, 0);
        fis.IDNameHash = this->itemIDNameHash;
        qDebug() << "Matching itemset" << fis << " to constraints " << this->itemConstraints;
#endif
        bool match;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            if (!constraintsSubset.isEmpty() && !constraintsSubset.contains(Constraints::ItemConstraintTypeClassification[i]))
                continue;

            ItemConstraintType type = (ItemConstraintType) i;
            for (int c = 0; c < this->preprocessedItemConstraints[type].size(); c++) {
                match = Constraints::matchItemsetHelper(itemset, type, this->preprocessedItemConstraints[type][c]);
#ifdef CONSTRAINTS_DEBUG
                qDebug() << "\t" << Constraints::ItemConstraintTypeName[type] << c << ": " << match;
#endif
                if (!match)
                    return false;
            }
        }

        return true;
    }

    /**
     * Check if the given itemset matches the defined constraints.
     *
     * @param itemset
     *   An itemset to check the constraints for.
     * @return
     *   True if the itemset matches the constraints, false otherwise.
     */
    bool Constraints::matchItemset(const QSet<ItemName> & itemset,
                                   const QSet<ConstraintClassification> & constraintsSubset) const
    {
#ifdef CONSTRAINTS_DEBUG
        qDebug() << "Matching itemset" << itemset << " to constraints " << this->itemConstraints;
#endif
        bool match;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            if (!constraintsSubset.isEmpty() && !constraintsSubset.contains(Constraints::ItemConstraintTypeClassification[i]))
                continue;

            ItemConstraintType type = (ItemConstraintType) i;
            for (int c = 0; c < this->itemConstraints[type].size(); c++) {
                match = Constraints::matchItemsetHelper(itemset, type, this->itemConstraints[type][c]);
#ifdef CONSTRAINTS_DEBUG
                qDebug() << "\t" << Constraints::ItemConstraintTypeName[type] << c << ": " << match;
#endif
                if (!match)
                    return false;
            }
        }

        return true;
    }

    /**
     * Check if a particular frequent itemset search space will be able to
     * match the defined constraints. We can do this by matching all *monotone*
     * constraints over the itemset *and* prefix paths support counts
     * simultaneously (since this itemset will be extended with portions of
     * the prefix paths).
     * Note that if we'd simply match *all* constraints, not just the monotone
     * ones, then it's possible relevant subsets are dropped.
     *
     * @param itemset
     *   An itemset to check the constraints for.
     * @param prefixPathsSupportCounts
     *   A list of support counts for the prefix paths in this search space.
     * @return
     *   True if the itemset matches the constraints, false otherwise.
     */
    bool Constraints::matchSearchSpace(const ItemIDList & frequentItemset, const QHash<ItemID, SupportCount> & prefixPathsSupportCounts) const {
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            if (Constraints::ItemConstraintTypeClassification[i] != ConstraintClassificationMonotone)
                continue;

            ItemConstraintType type = (ItemConstraintType) i;
            for (int c = 0; c < this->preprocessedItemConstraints[type].size(); c++) {
                if (!Constraints::matchSearchSpaceHelper(frequentItemset, prefixPathsSupportCounts, type, this->preprocessedItemConstraints[type][c]))
                    return false;
            }
        }

        return true;
    }


    //------------------------------------------------------------------------
    // Protected methods.

    /**
     * Helper function for Constraints::matchItemSet(ItemIDList).
     */
    bool Constraints::matchItemsetHelper(const ItemIDList & itemset, ItemConstraintType type, const QSet<ItemID> & constraintItems) {
        foreach (ItemID id, constraintItems) {
            switch (type) {

            case ItemConstraintPositive:
                if (itemset.contains(id))
                    return true;
                break;

            case ItemConstraintNegative:
                if (itemset.contains(id))
                    return false;
                break;
            }
        }

        // In case we haven't returned yet, meaning that none of the items in
        // the constraint was actually *in* this itemset.
        switch (type) {
        case ItemConstraintNegative:
            return true;

        case ItemConstraintPositive:
            return false;
        }

        // Satisfy the compiler.
        return false;
    }  

    /**
     * Helper function for Constraints::matchItemSet(QSet<ItemName>).
     */
    bool Constraints::matchItemsetHelper(const QSet<ItemName> & itemset, ItemConstraintType type, const QSet<ItemName> & constraintItems) {
        QSet<ItemName> itemsetCopy = itemset;
        bool emptyIntersection = itemsetCopy.intersect(constraintItems).isEmpty();

        // Match wildcard item constraints, if any.
        QRegExp rx;
        rx.setPatternSyntax(QRegExp::Wildcard);
        foreach (const ItemName & constraintItem, constraintItems) {
            if (constraintItem.contains('*')) {
                rx.setPattern(constraintItem);
                foreach (const QString & item, itemset) {
                    if (rx.exactMatch(item)) {
                        if (type == ItemConstraintPositive)
                            return true;
                        else if (type == ItemConstraintNegative)
                            return false;
                    }
                }
            }
        }

        if (type == ItemConstraintPositive)
            return !emptyIntersection;
        else if (type == ItemConstraintNegative)
            return emptyIntersection;

        // Satisfy the compiler.
        return false;
    }

    /**
     * Helper function for Constraints::matchSearchSpace().
     */
    bool Constraints::matchSearchSpaceHelper(const ItemIDList & frequentItemset, const QHash<ItemID, SupportCount> & prefixPathsSupportCounts, ItemConstraintType type, const QSet<ItemID> & constraintItems) {
        foreach (ItemID id, constraintItems) {
            switch (type) {
            case ItemConstraintPositive:
                if (frequentItemset.contains(id) || prefixPathsSupportCounts[id] > 0)
                    return true;
                break;

            case ItemConstraintNegative:
                if (prefixPathsSupportCounts[id] > 0)
                    return false;
                break;
            }
        }

        // In case we haven't returned yet, meaning that none of the items in
        // the constraint was actually *in* this itemset.
        switch (type) {
        case ItemConstraintNegative:
            return true;
            break;

        case ItemConstraintPositive:
            return false;
            break;
        }

        // Satisfy the compiler.
        return false;
    }

    /**
     * Make sure the preprocessedItemConstraints data structure mirrors the
     * itemConstraints datastructure, albeit it will be empty for now.
     */
    void Constraints::updatePreprocesedItemConstraintsStructure() {
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            ItemConstraintType type = (ItemConstraintType) i;

            if (!this->preprocessedItemConstraints.contains(type))
                this->preprocessedItemConstraints.insert(type, QVector<QSet<ItemID> >());

            this->preprocessedItemConstraints[type].resize(this->itemConstraints[type].size());
        }
    }

    /**
     * Store a preprocessed item constraint in the optimized constraint data
     * structure.
     *
     * @param type
     *   The item constraint type.
     * @param constraint
     *   The how manieth constraint of this item constraint type.
     * @param id
     *   The item id.
     */
    void Constraints::addPreprocessedItemConstraint(ItemConstraintType type, uint constraint, ItemID id) {
        if (!this->preprocessedItemConstraints.contains(type))
            this->preprocessedItemConstraints.insert(type, QVector<QSet<ItemID> >());
        if ((uint) this->preprocessedItemConstraints[type].size() <= constraint)
            this->preprocessedItemConstraints[type].resize(constraint + 1);
        this->preprocessedItemConstraints[type][constraint].insert(id);
    }


    //------------------------------------------------------------------------
    // Other.

#ifdef DEBUG

    QDebug operator<<(QDebug dbg, const Constraints & constraints) {
        // Item constraints.
        // Stats.
        unsigned int sum = 0;
        foreach (ItemConstraintType type, constraints.itemConstraints.keys())
            for (int c = 0; c < constraints.itemConstraints[type].size(); c++)
                sum += constraints.itemConstraints[type][c].size();
        // Display.
        dbg.nospace() << "item constraints (" << sum << "):" << endl;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            ItemConstraintType constraintType = (ItemConstraintType) i;
            dbg.nospace() << "\t" << Constraints::ItemConstraintTypeName[i] << ":" << endl;

            for (int c = 0; c < constraints.itemConstraints[constraintType].size(); c++) {
                dbg.space() << "\t\t" <<  c << ". ";
                if (constraints.itemConstraints[constraintType][c].isEmpty())
                    dbg.space() << "none";
                else
                    dbg.space() << constraints.itemConstraints[constraintType][c];
                dbg.nospace() << endl;
            }
            dbg.nospace() << endl;
        }


        // Preprocessed item constraints.
        // Stats.
        sum = 0;
        foreach (ItemConstraintType type, constraints.itemConstraints.keys())
            for (int c = 0; c < constraints.preprocessedItemConstraints[type].size(); c++)
                sum += constraints.preprocessedItemConstraints[type][c].size();
        // Display.
        dbg.nospace() << "preprocesseditem constraints (" << sum << "):" << endl;
        for (int i = ItemConstraintPositive; i <= ItemConstraintNegative; i++) {
            ItemConstraintType constraintType = (ItemConstraintType) i;
            dbg.nospace() << "\t" << Constraints::ItemConstraintTypeName[i] << ":" << endl;

            for (int c = 0; c < constraints.preprocessedItemConstraints[constraintType].size(); c++) {
                dbg.space() << "\t\t" << c << ". ";
                if (constraints.preprocessedItemConstraints[constraintType][c].isEmpty())
                    dbg.space() << "none";
                else
                    dbg.space() << constraints.preprocessedItemConstraints[constraintType][c];
                dbg.nospace() << endl;
            }
            dbg.nospace() << endl;
        }

        return dbg.maybeSpace();
    }
#endif

}
