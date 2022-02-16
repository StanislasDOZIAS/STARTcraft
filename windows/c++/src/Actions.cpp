#include "Actions.h"


void Actions::CreateNewBase(BWAPI::Position position, Squad& squad) {
	squad.move(position);
	for (BWAPI::Unit unit : squad.get_Units()) {
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
			unit->build(BWAPI::UnitTypes::Zerg_Hatchery, BWAPI::TilePosition(position));
		}
	}
}

void Actions::ExpendBase_1(UnitCount& myUnits, std::list<Squad*>& mySquads) {
    /*
    BWAPI::Unit builder = nullptr;
    int ActionId = 1;
    Squad *gaz;
    Squad *mineral;
    mineral = getSquad(1, ActionId, mySquads, myUnits, 15);
    mineral->countSquadUnits();
    if (myUnits.supplyAvailable < myUnits.UsedSupply- 4) {
        std::cout << "we need overlords ! " << std::endl;
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Larva, mySquads, myUnits, u);
        u->morph(BWAPI::UnitTypes::Zerg_Overlord);
    }
    //enlistUnit(mineral, mySquads, myUnits);
    BWAPI::Unit Extractor;
    getUnit(BWAPI::UnitTypes::Zerg_Extractor, mySquads, myUnits, Extractor);
    for (BWAPI::Unit u : mineral->get_Units()) {
        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(u, BWAPI::Broodwar->getMinerals());

        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral && u->isIdle()) { u->rightClick(closestMineral); }
    }

    if (Extractor != nullptr) {
        if (Extractor->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
            gaz = getSquad(1, 2, mySquads, myUnits, 4);
            //std::cout << "gaz : " << gaz->get_Units().size() << std::endl;
            for (BWAPI::Unit u : gaz->get_Units()) {
                if (u->isIdle() || u->isGatheringMinerals()) { u->rightClick(Extractor); }
            }
        }
    }

    if ((myUnits.got_Hatchery == 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice() * myUnits.number_Hatchery + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hatchery)) {
        myUnits.got_Hatchery = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
    }

    if (myUnits.got_Hatchery == 1) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery && u->isBeingConstructed()) {
                myUnits.got_Hatchery = 0;
                myUnits.blocked_minerals -= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
                myUnits.number_Hatchery += 1;
            }
        }
    }
    */

    // We will follow the following Tech tree Building :
// Spawning pool, Vespin geyser extractor -> Lair, Hydralisk's Den, Evolution Chamber -> Queen's Nest
    /*

    if ((myUnits.got_Spawning_pool == 0) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + myUnits.blocked_minerals) &&
        (myUnits.droneOwned + myUnits.droneMorphing >= 11) && MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)) {
        myUnits.got_Spawning_pool = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    }


    if ((myUnits.got_Extractor == 0) && (myUnits.got_Spawning_pool > 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)) {
        myUnits.got_Extractor = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Extractor.mineralPrice();
    }


    if ((myUnits.got_Lair == 0) && (myUnits.got_Spawning_pool == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                myUnits.got_Lair = 2;
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 0) && (myUnits.got_Lair > 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + myUnits.blocked_gas) && MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den)) {
        myUnits.got_Hydra_Den = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice();
        myUnits.blocked_gas += BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice();
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.got_Lair == 3) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->research(BWAPI::TechTypes::Lurker_Aspect)) {
                    myUnits.lurker_aspect = 1;
                }
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.lurker_aspect == 1) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    myUnits.grooved_spines = 1;
                }
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.grooved_spines == 1) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    myUnits.muscular_augments = 1;
                }
            }
        }
    }

    if ((myUnits.got_Evolution_Chamber == 0) && (myUnits.grooved_spines == 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice() + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Evolution_Chamber)) {
        myUnits.got_Evolution_Chamber = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice();
    }


    if ((myUnits.got_Evolution_Chamber == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)) {
                    myUnits.ground_dist_damage = 1;
                }
            }
        }
    }
    */
}

void Actions::BaseArmy(UnitCount& myUnits, std::list<Squad*>& mySquads) {
    Squad* Zerglings;
    int ActionId = 2;
    Zerglings = getSquad(2, ActionId, mySquads, myUnits, 80);
}


void Actions::Building_tree(UnitCount& myUnits, std::list<Squad*>& mySquads) {

    if ((myUnits.got_Hatchery == 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice() * myUnits.number_Hatchery + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hatchery)) {
        myUnits.got_Hatchery = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
    }

    if (myUnits.got_Hatchery == 1) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery && u->isBeingConstructed()) {
                myUnits.got_Hatchery = 0;
                myUnits.blocked_minerals -= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
                myUnits.number_Hatchery += 1;
            }
        }
    }

    if ((myUnits.got_Spawning_pool == 0) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + myUnits.blocked_minerals) &&
        (myUnits.droneOwned + myUnits.droneMorphing >= 11) && MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)) {
        myUnits.got_Spawning_pool = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    }


    if ((myUnits.got_Extractor == 0) && (myUnits.got_Spawning_pool > 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)) {
        myUnits.got_Extractor = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Extractor.mineralPrice();
    }


    if ((myUnits.got_Lair == 0) && (myUnits.got_Spawning_pool == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                myUnits.got_Lair = 2;
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 0) && (myUnits.got_Lair > 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + myUnits.blocked_gas) && MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den)) {
        myUnits.got_Hydra_Den = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice();
        myUnits.blocked_gas += BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice();
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.got_Lair == 3) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->research(BWAPI::TechTypes::Lurker_Aspect)) {
                    myUnits.lurker_aspect = 1;
                }
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.lurker_aspect == 1) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    myUnits.grooved_spines = 1;
                }
            }
        }
    }

    if ((myUnits.got_Hydra_Den == 3) && (myUnits.grooved_spines == 1) && (myUnits.number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    myUnits.muscular_augments = 1;
                }
            }
        }
    }

    if ((myUnits.got_Evolution_Chamber == 0) && (myUnits.grooved_spines == 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice() + myUnits.blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Evolution_Chamber)) {
        myUnits.got_Evolution_Chamber = 1;
        myUnits.blocked_minerals += BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice();
    }


    if ((myUnits.got_Evolution_Chamber == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.mineralPrice() + myUnits.blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.gasPrice() + myUnits.blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)) {
                    myUnits.ground_dist_damage = 1;
                }
            }
        }
    }
}

void Actions::Economy(UnitCount& myUnits, std::list<Squad*>& mySquads) {
    //BWAPI::Unit builder = nullptr;
    int ActionId = 1;
    Squad* gaz;
    Squad* mineral;
    mineral = getSquad(1, ActionId, mySquads, myUnits, 15);
    mineral->countSquadUnits();
    if (myUnits.supplyAvailable < myUnits.UsedSupply - 4) {
        std::cout << "we need overlords ! " << std::endl;
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Larva, mySquads, myUnits, u);
        u->morph(BWAPI::UnitTypes::Zerg_Overlord);
    }
    //enlistUnit(mineral, mySquads, myUnits);
    BWAPI::Unit Extractor;
    getUnit(BWAPI::UnitTypes::Zerg_Extractor, mySquads, myUnits, Extractor);
    for (BWAPI::Unit u : mineral->get_Units()) {
        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(u, BWAPI::Broodwar->getMinerals());

        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral && u->isIdle()) { u->rightClick(closestMineral); }
    }

    if (Extractor != nullptr) {
        if (Extractor->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
            gaz = getSquad(1, 2, mySquads, myUnits, 4);
            //std::cout << "gaz : " << gaz->get_Units().size() << std::endl;
            for (BWAPI::Unit u : gaz->get_Units()) {
                if (u->isIdle() || u->isGatheringMinerals()) { u->rightClick(Extractor); }
            }
        }
    }
}

//Renvoie une unité libre (ds aucune squad)
void getUnit(BWAPI::UnitType type, std::list<Squad*>& mySquads, UnitCount& myUnits, BWAPI::Unit& unity) {
    bool found_unit = false;
    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == type && !unitInSquad(u, mySquads)) {
            unity = u;
            found_unit = true;
            //return u;
        }
    }
    if (!found_unit) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && !unitInSquad(u, mySquads)) {
                unity = u;
                found_unit = true;
            }
        }
    }
    if (!found_unit) {
        unity = nullptr;
    }
    //return myUnits.larva;
}



bool unitInSquad(BWAPI::Unit& unit, std::list<Squad*>& mySquads) {
    bool inSquad = false;
    for (Squad* squad : mySquads) {
        for (BWAPI::Unit u2 : squad->get_Units()) {
            if (u2->getID() == unit->getID()) {
                inSquad = true;
            }
        }
    }
    return inSquad;
}

Squad* getSquad(int Squad_type, int ActionId,std::list<Squad*>& mySquads, UnitCount& myUnits, int size) {
    for (Squad* squad : mySquads) {
        if (squad->get_type() == Squad_type && squad->get_Action() == ActionId) {
            enlistUnit(squad, mySquads, myUnits);
            return squad;
        }
    }

    static Squad* newSquad;
    if (Squad_type == 1) {
        newSquad = new WorkerSquad(size);
    }
    else if (Squad_type == 2) {
        newSquad = new ZerglingSquad(size);
    }
    else {
        newSquad = new Squad(1, 1);
    }
    newSquad->changeAction(ActionId);
    enlistUnit(newSquad, mySquads, myUnits);
    mySquads.push_back(newSquad);
    return newSquad;
}

void enlistUnit(Squad* squad, std::list<Squad*>& mySquads, UnitCount& myUnits) {
    squad->countSquadUnits();
    if (squad->get_droneWanted() > squad->get_droneOwned()) {
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Drone, mySquads, myUnits, u);
        if (u != nullptr) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Drone) {
                squad->add_Unit(u);
            }
            else if (u->getType() == BWAPI::UnitTypes::Zerg_Larva) {
                u->morph(BWAPI::UnitTypes::Zerg_Drone);
            }
        }
    }

    if (squad->get_zerglingWanted() > squad->get_zerglingOwned()) {
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Zergling, mySquads, myUnits, u);
        if (u != nullptr) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
                squad->add_Unit(u);
            }
            else if (u->getType() == BWAPI::UnitTypes::Zerg_Larva) {
                u->morph(BWAPI::UnitTypes::Zerg_Zergling);
            }
        }
    }
}