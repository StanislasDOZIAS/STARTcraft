#include "Scenario.h"

UnitCount* myUnits = new UnitCount();

int* Scenario(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits, std::list<Squad>& mySquads){
    countUnits(Broodwar);
	return nullptr;
}


void countUnits(BWAPI::GameWrapper& Broodwar)
{
    const BWAPI::Unitset& myListUnits = BWAPI::Broodwar->self()->getUnits();

    // Reset the counters
    int temp_hatchery = (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Hatchery)];
    memset((*myUnits).unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    memset((*myUnits).unitMorphing, 0, 4*int(BWAPI::UnitTypes::Unknown));
    (*myUnits).supplyAvailable = 0;

    // Detect failed building

    if (((*myUnits).building_in_progress != BWAPI::UnitTypes::Unknown ) && ((*myUnits).building_frame_count > (*myUnits).max_frame_building)) {
        (*myUnits).blocked_minerals -= (*myUnits).building_in_progress.mineralPrice();
        (*myUnits).blocked_gas -= (*myUnits).building_in_progress.gasPrice();
        if ((*myUnits).building_in_progress == BWAPI::UnitTypes::Zerg_Hatchery) {
            temp_hatchery = 0;
        }
        else {
            (*myUnits).unitBuilding[(*myUnits).building_in_progress] = 0;
        }
        (*myUnits).building_in_progress = BWAPI::UnitTypes::Unknown;
    }

    for (BWAPI::Unit unit : myListUnits)
    {
        // Buildings
        if (unit->getType().isBuilding()) {
            if (unit->getType() != BWAPI::UnitTypes::Zerg_Hatchery) {
                if (unit->isBeingConstructed()) {
                    if ((*myUnits).unitBuilding[unit->getType()] == 1) {
                        (*myUnits).blocked_minerals -= unit->getType().mineralPrice();
                        (*myUnits).blocked_gas -= unit->getType().gasPrice();
                        (*myUnits).building_in_progress = BWAPI::UnitTypes::Unknown;
                    }
                    (*myUnits).unitBuilding[unit->getType()] = 2;
                }
                else {
                    (*myUnits).unitBuilding[unit->getType()] = 3;
                }
            }
            else if ((temp_hatchery == 1) && (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery) && (unit->isBeingConstructed()))
            {
                temp_hatchery = 0;
                (*myUnits).blocked_minerals -= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
                (*myUnits).number_Hatchery += 1;
                (*myUnits).building_in_progress = BWAPI::UnitTypes::Unknown;
            }
        }

        if ((unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && unit->isCompleted()) || unit->getType() == BWAPI::UnitTypes::Zerg_Hive || unit->getType() == BWAPI::UnitTypes::Zerg_Lair)
        {
            (*myUnits).supplyAvailable += 2;
        }

        // "True" Units

        else {
            // We begin with morphing units
            if ((unit->getType() == BWAPI::UnitTypes::Zerg_Egg) || (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg)) {
                (*myUnits).unitMorphing[unit->getBuildType()] += 1;
            }
            else {
                (*myUnits).unitOwned[unit->getType()] += 1;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva) {
                (*myUnits).larva = unit;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
                (*myUnits).hydra = unit;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
                (*myUnits).supplyAvailable += 16;
            }


            if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
                (*myUnits).hydra = unit;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
                if (unit->isIdle()) {
                    unit->burrow();
                }
            }
        }
    }
    
    (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Hatchery)] = temp_hatchery;
}

void nextLarvaMorph(BWAPI::GameWrapper& Broodwar) {
    if ((*myUnits).unitOwned[int(BWAPI::UnitTypes::Zerg_Drone)] + (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Drone)] < (*myUnits).unitWanted[int(BWAPI::UnitTypes::Zerg_Drone)]) {
        (*myUnits).nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Drone;
    }

    else if (((*myUnits).unitBuilding[int(BWAPI::UnitTypes::Zerg_Hydralisk_Den)] == 3) &&
        ((*myUnits).unitOwned[int(BWAPI::UnitTypes::Zerg_Hydralisk)] + (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Hydralisk)] < (*myUnits).unitWanted[int(BWAPI::UnitTypes::Zerg_Hydralisk)])) {
        (*myUnits).nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Hydralisk;
    }

    else if (((*myUnits).unitBuilding[int(BWAPI::UnitTypes::Zerg_Spawning_Pool)] == 3) &&
        ((*myUnits).unitOwned[int(BWAPI::UnitTypes::Zerg_Zergling)] + (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Zergling)] < (*myUnits).unitWanted[int(BWAPI::UnitTypes::Zerg_Zergling)])) {
        (*myUnits).nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Zergling;
    }

    else {
        (*myUnits).nextUnitFromLarva = BWAPI::UnitTypes::Unknown;
    }
}

UnitCount::UnitCount(){

    building_frame_count = 0;
    max_frame_building = 24 * 10;
    building_in_progress = BWAPI::UnitTypes::Unknown;
    number_Hatchery = 1;
    blocked_minerals = 0;
    blocked_gas = 0;
    larva = nullptr;
    hydra = nullptr;
    nextUnitFromLarva = BWAPI::UnitTypes::Unknown;

    std::memset(unitBuilding, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitMorphing, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(tech, 0, 4*int(BWAPI::TechTypes::Unknown));
    std::memset(upgrades, 0, 4*int(BWAPI::UpgradeTypes::Unknown));

    std::memset(unitWanted, 0, 4*int(BWAPI::UnitTypes::Unknown));
    unitWanted[BWAPI::UnitTypes::Zerg_Drone] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 0;
    unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 50;
    unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 5;
}