#include "Scenario.h"

UnitCount* myUnits = new UnitCount();

int* Scenario(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits, std::list<Squad>& mySquads){
    countUnits(Broodwar);
	return nullptr;
}


void countUnits(BWAPI::GameWrapper& Broodwar)
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myListUnits = BWAPI::Broodwar->self()->getUnits();

    // Reset the units
    int temp_hatchery = (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Hatchery)];
    memset((*myUnits).unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    memset((*myUnits).unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
    (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Lurker)] = 0;
    (*myUnits).unitOwned[int(BWAPI::UnitTypes::Zerg_Lurker)] = 0;


    //(*myUnits).number_Hatchery = 1;
    (*myUnits).supplyAvailable = 0;

    // Detect failed building

    if ((*myUnits).building_frame_count > (*myUnits).max_frame_building) {
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

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Drone) {
                // If it is an idle worker, then we want to send it somewhere
                if (unit->isIdle())
                {
                    // Get the closest mineral to this worker unit
                    BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

                    // If a valid mineral was found, right click it with the unit in order to start harvesting
                    if (closestMineral) { unit->rightClick(closestMineral); }
                }
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

UnitCount::UnitCount(){

    building_frame_count = 0;
    max_frame_building = 24 * 10;
    building_in_progress = BWAPI::UnitTypes::Unknown;
    number_Hatchery = 1;
    blocked_minerals = 0;
    blocked_gas = 0;
    larva = nullptr;
    hydra = nullptr;

    std::memset(unitBuilding, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitMorphing, 0, 4*int(BWAPI::UnitTypes::Unknown)+3);
    std::memset(tech, 0, 4*int(BWAPI::TechTypes::Unknown));
    std::memset(upgrades, 0, 4*int(BWAPI::UpgradeTypes::Unknown));

    std::memset(unitWanted, 0, 4*int(BWAPI::UnitTypes::Unknown));
    unitWanted[BWAPI::UnitTypes::Zerg_Drone] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 50;
    unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 10;
}