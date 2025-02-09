#include "Scenario.h"
#include "Squad.h"
#include "Actions.h"

UnitCount* myUnits = new UnitCount();

// Call all the other functions
void Scenario(BWAPI::GameWrapper& Broodwar, std::list<Squad*>& mySquads){
    countUnits(Broodwar, mySquads);

    changeUnitWanted(Broodwar, mySquads);

    nextLarvaMorph(BWAPI::Broodwar);
}

//Count all the unit and update myUnits
void countUnits(BWAPI::GameWrapper& Broodwar, std::list<Squad*>& mySquads)
{
    const BWAPI::Unitset& myListUnits = BWAPI::Broodwar->self()->getUnits();

    // Reset the counters
    memset(myUnits->unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    memset(myUnits->unitMorphing, 0, 4*int(BWAPI::UnitTypes::Unknown));
    myUnits->supplyAvailable = 0;
    myUnits->number_Hatchery = 0;

    // Detect failed building
    if ((myUnits->building_in_progress != BWAPI::UnitTypes::Unknown ) &&
        (myUnits->building_frame_count > myUnits->max_frame_building)) {

        myUnits->blocked_minerals -= myUnits->building_in_progress.mineralPrice();
        myUnits->blocked_gas -= myUnits->building_in_progress.gasPrice();
        myUnits->unitBuilding[myUnits->building_in_progress] = 0;
        myUnits->building_in_progress = BWAPI::UnitTypes::Unknown;
        for (Squad* squad : mySquads) {
            if (squad->get_type() == 1 && squad->get_Action() == 1) {
                squad->add_Unit(myUnits->builder);
                break;
            }
        }
        myUnits->builder = nullptr;
    }

    for (BWAPI::Unit unit : myListUnits)
    {
        // Buildings
        if (unit->getType().isBuilding()) {
            if (unit->isBeingConstructed()) {
                if (myUnits->unitBuilding[unit->getType()] == 1) {
                    myUnits->blocked_minerals -= unit->getType().mineralPrice();
                    myUnits->blocked_gas -= unit->getType().gasPrice();
                    myUnits->building_in_progress = BWAPI::UnitTypes::Unknown;
                    myUnits->builder = nullptr;
                }
                myUnits->unitBuilding[unit->getType()] = 2;
            }
            else {
                myUnits->unitOwned[unit->getType()] += 1;
            }
        
            if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery || unit->getType() == BWAPI::UnitTypes::Zerg_Hive || unit->getType() == BWAPI::UnitTypes::Zerg_Lair)
            {
                myUnits->supplyAvailable += 2;
                if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && !unit->isCompleted()) {
                    myUnits->supplyAvailable -= 2;
                }
                myUnits->number_Hatchery += 1;
            }

        }
        // "True" Units

        else{
            // We begin with morphing units
            if ((unit->getType() == BWAPI::UnitTypes::Zerg_Egg) || (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg)) {
                myUnits->unitMorphing[unit->getBuildType()] += 1;
            }
            else {
                myUnits->unitOwned[unit->getType()] += 1;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva) {
                myUnits->larva = unit;
            }


            if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
                myUnits->supplyAvailable += 16;
            }
        }
    }
}

// Update unitWanted if needed
void changeUnitWanted(BWAPI::GameWrapper& Broodwar, std::list<Squad*>& mySquads) {
    if (myUnits->unitOwned[BWAPI::UnitTypes::Zerg_Lurker] + myUnits->unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] >= 2) {
        myUnits->unitWanted[BWAPI::UnitTypes::Zerg_Drone] = 30;
        myUnits->unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 50;
        myUnits->unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 20;

        ArmySquad* Army = static_cast<ArmySquad*>(getSquad(2, 2, mySquads));
        (*Army).unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 50;
        (*Army).unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 20;
    }
}

// Choose the next larva morphing (without caring about supply)
void nextLarvaMorph(BWAPI::GameWrapper& Broodwar) {

    if ((myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Spawning_Pool)] >= 1) && 
        (myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Zergling)] + 2 * myUnits->unitMorphing[int(BWAPI::UnitTypes::Zerg_Zergling)] < myUnits->unitWanted[int(BWAPI::UnitTypes::Zerg_Zergling)]) &&
        (myUnits->unitWanted[int(BWAPI::UnitTypes::Zerg_Drone)] < 30) && (myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Drone)] >= 5)) {
        myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Zergling;
    }
    
    else if (myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Drone)] + myUnits->unitMorphing[int(BWAPI::UnitTypes::Zerg_Drone)] < myUnits->unitWanted[int(BWAPI::UnitTypes::Zerg_Drone)]) {
        myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Drone;
    }

    else if ((myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Hydralisk_Den)] >= 1) &&
        (myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Hydralisk)] + myUnits->unitMorphing[int(BWAPI::UnitTypes::Zerg_Hydralisk)] < myUnits->unitWanted[int(BWAPI::UnitTypes::Zerg_Hydralisk)])) {
        myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Hydralisk;
    }

    else if ((myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Spawning_Pool)] >= 1) &&
        (myUnits->unitOwned[int(BWAPI::UnitTypes::Zerg_Zergling)] + 2*myUnits->unitMorphing[int(BWAPI::UnitTypes::Zerg_Zergling)] < myUnits->unitWanted[int(BWAPI::UnitTypes::Zerg_Zergling)])) {
        myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Zergling;
    }

    else {
        if (myUnits->unitWanted[BWAPI::UnitTypes::Zerg_Drone] < 30) {
            myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Drone;
        }
        else {
            myUnits->nextUnitFromLarva = BWAPI::UnitTypes::Zerg_Hydralisk;
        }
        
    }
}

UnitCount::UnitCount(){

    building_frame_count = 0;
    max_frame_building = 24 * 20;
    building_in_progress = BWAPI::UnitTypes::Unknown;
    number_Hatchery = 1;
    secondBaseBuilder = nullptr;


    blocked_minerals = 0;
    blocked_gas = 0;


    larva = nullptr;
    nextUnitFromLarva = BWAPI::UnitTypes::Unknown;

    BWAPI::TilePosition secondBasePos = BWAPI::TilePositions::Origin;
    bool foundSecondBasePos = false;
    builder = nullptr;

    first_extractor = nullptr;
    second_extractor = nullptr;
    secondBase = nullptr;


    std::memset(unitBuilding, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitOwned, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(unitMorphing, 0, 4*int(BWAPI::UnitTypes::Unknown));
    std::memset(tech, 0, 4*int(BWAPI::TechTypes::Unknown));
    std::memset(upgrades, 0, 4*int(BWAPI::UpgradeTypes::Unknown));

    std::memset(unitWanted, 0, 4*int(BWAPI::UnitTypes::Unknown));
    unitWanted[BWAPI::UnitTypes::Zerg_Drone] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 10;
    unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 10;
    unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 4;
}