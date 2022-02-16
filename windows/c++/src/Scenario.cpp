#include "Scenario.h"

int* Scenario(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits, std::list<Squad>& mySquads){
    countUnits(Broodwar, myUnits);
	return nullptr;
}


void countUnits(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits)
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myRealUnits = Broodwar->self()->getUnits();

    // Reset the units

    myUnits.droneOwned = 0;
    myUnits.zerglingOwned = 0;
    myUnits.hydraOwned = 0;
    myUnits.lurkerOwned = 0;
    myUnits.mutaliskOwned = 0;
    myUnits.ultraliskOwned = 0;

    myUnits.droneMorphing = 0;
    myUnits.zerglingMorphing = 0;
    myUnits.hydraMorphing = 0;
    myUnits.lurkerMorphing = 0;
    myUnits.mutaliskMorphing = 0;
    myUnits.ultraliskMorphing = 0;

    myUnits.ovieMorphing = 0;
    myUnits.supplyAvailable = 0;


    for (BWAPI::Unit unit : myRealUnits)
    {
        // Buildings
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
            if (unit->isBeingConstructed()) {
                if (myUnits.got_Spawning_pool == 1) {
                    myUnits.blocked_minerals -= unit->getType().mineralPrice();
                }
                myUnits.got_Spawning_pool = 2;
            }
            else {
                myUnits.got_Spawning_pool = 3;
            }
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
            if (unit->isBeingConstructed()) {
                if (myUnits.got_Extractor == 1) {
                    myUnits.blocked_minerals -= unit->getType().mineralPrice();
                }
                myUnits.got_Extractor = 2;
            }
            else {
                myUnits.got_Extractor = 3;
            }
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair) {
            myUnits.supplyAvailable += 2;
            if (unit->isBeingConstructed()) {
                if (myUnits.got_Lair == 1) {
                    myUnits.blocked_minerals -= unit->getType().mineralPrice();
                    myUnits.blocked_gas -= unit->getType().gasPrice();
                }
                myUnits.got_Lair = 2;
            }
            else {
                myUnits.got_Lair = 3;
            }
        }

        if ((unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && unit->isCompleted()) || unit->getType() == BWAPI::UnitTypes::Zerg_Hive)
        {
            myUnits.supplyAvailable += 2;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
            if (unit->isBeingConstructed()) {
                if (myUnits.got_Hydra_Den == 1) {
                    myUnits.blocked_minerals -= unit->getType().mineralPrice();
                    myUnits.blocked_gas -= unit->getType().gasPrice();
                }
                myUnits.got_Hydra_Den = 2;
            }
            else {
                myUnits.got_Hydra_Den = 3;
            }
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
            if (unit->isBeingConstructed()) {
                if (myUnits.got_Evolution_Chamber == 1) {
                    myUnits.blocked_minerals -= unit->getType().mineralPrice();
                    myUnits.blocked_gas -= unit->getType().gasPrice();
                }
                myUnits.got_Evolution_Chamber = 2;
            }
            else {
                myUnits.got_Evolution_Chamber = 3;
            }
        }


        // Units

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva) {
            myUnits.larva = unit;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Drone) {
            myUnits.droneOwned += 1;
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
            myUnits.supplyAvailable += 16;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
            myUnits.zerglingOwned += 1;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
            myUnits.hydraOwned += 1;
            myUnits.hydra = unit;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
            myUnits.lurkerOwned += 1;
            if (unit->isIdle()) {
                unit->burrow();
            }
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg) {
            myUnits.lurkerMorphing += 1;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk) {
            myUnits.mutaliskOwned += 1;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Ultralisk) {
            myUnits.ultraliskOwned += 1;
        }

        if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg) {
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Drone) {
                myUnits.droneMorphing += 1;
            }
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Zergling) {
                myUnits.zerglingMorphing += 1;
            }
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
                myUnits.hydraMorphing += 1;
            }
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Mutalisk) {
                myUnits.mutaliskMorphing += 1;
            }
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Ultralisk) {
                myUnits.ultraliskMorphing += 1;
            }
            if (unit->getBuildType() == BWAPI::UnitTypes::Zerg_Overlord) {
                myUnits.ovieMorphing += 1;
            }
        }

    }

    myUnits.UsedSupply = Broodwar->self()->supplyUsed();
}

UnitCount::UnitCount(){
    got_Spawning_pool = 0;
    got_Extractor = 0;
    got_Lair = 0;
    got_Hydra_Den = 0;
    got_Evolution_Chamber = 0;
    got_Queens_Nest = 0;
    got_Hatchery = 0;

    number_Hatchery = 0;

    //The upgrade we have

    lurker_aspect = 0;
    grooved_spines = 0;
    muscular_augments = 0;

    ground_armor = 0;
    ground_dist_damage = 0;
    ground_melee_damage = 0;

    air_armor = 0;
    air_damage = 0;

    //The Unit we want

    droneWanted = 0;
    zerglingWanted = 0;
    hydraWanted = 0;
    lurkerWanted = 0;

    //The Combat Units we owne

    droneOwned = 0;
    zerglingOwned = 0;
    hydraOwned = 0;
    lurkerOwned = 0;
    mutaliskOwned = 0;
    ultraliskOwned = 0;

    droneMorphing = 0;
    zerglingMorphing = 0;
    hydraMorphing = 0;
    lurkerMorphing = 0;
    mutaliskMorphing = 0;
    ultraliskMorphing = 0;

    ovieMorphing = 0;
    supplyAvailable = 0;
    blocked_minerals = 0;
    blocked_gas = 0;

    // The only larva and hydra we need per frame
    larva = nullptr;
    hydra = nullptr;
}
