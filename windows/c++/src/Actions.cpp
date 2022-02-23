#include "Actions.h"

// Build more supply if we are going to run out soon
void Actions::buildAdditionalSupply()
{
    if ((*myUnits).larva == nullptr) {
        return;
    }

    const int supply_Used = BWAPI::Broodwar->self()->supplyUsed();
    // If we have a sufficient amount of supply, we don't need to do anything
    if (((*myUnits).supplyAvailable - supply_Used < 4) && ((*myUnits).supplyAvailable < 400)) {
        // We don't authorize multiple overlords if supply_used <= 34 (i.e. 17)
        if (supply_Used <= 34) {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + (*myUnits).blocked_minerals) && ((*myUnits).larva != nullptr) && ((*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] == 0)) {
                if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                    (*myUnits).larva = nullptr;
                }
            }
        }
        // We authorize 1 morphing overlord if supply_used > 34 (i.e. 17)
        else {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + (*myUnits).blocked_minerals) && ((*myUnits).larva != nullptr) && ((*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] <= 1)) {
                if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                    (*myUnits).larva = nullptr;
                }
            }
        }
    }
}

void Actions::Economy(std::list<Squad*>& mySquads) {
    //BWAPI::Unit builder = nullptr;
    int ActionId = 1;
    Squad* gaz;
    Squad* mineral;
    mineral = getSquad(1, ActionId, mySquads, 15);
    mineral->countSquadUnits();

    BWAPI::Unit Extractor;
    getUnit(BWAPI::UnitTypes::Zerg_Extractor, mySquads, Extractor);
    for (BWAPI::Unit u : mineral->get_Units()) {
        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(u, BWAPI::Broodwar->getMinerals());

        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral && u->isIdle()) { u->rightClick(closestMineral); }
    }

    if (Extractor != nullptr) {
        if ((Extractor->getType() == BWAPI::UnitTypes::Zerg_Extractor) && (Extractor->isCompleted())) {
            gaz = getSquad(1, 2, mySquads, 4);
            if (gaz->get_Squad_size() < 4) {
                std::cout << transfer_squad(*mineral, *gaz, BWAPI::UnitTypes::Zerg_Drone, 4 - gaz->get_Squad_size());
            }
            //std::cout << "gaz : " << gaz->get_Units().size() << std::endl;
            for (BWAPI::Unit u : gaz->get_Units()) {
                if (u->isIdle() || u->isGatheringMinerals()) { u->rightClick(Extractor); }
            }
        }
    }
}


void Actions::BaseArmy(std::list<Squad*>& mySquads) {
    Squad* Zerglings;
    int ActionId = 2;
    Zerglings = getSquad(2, ActionId, mySquads, 80);
}


void Actions::Building_tree(std::list<Squad*>& mySquads) {

    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();
    if (((*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] == 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice() * (*myUnits).number_Hatchery + (*myUnits).blocked_minerals)) {
        if (MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hatchery, desiredPos)) {
            (*myUnits).unitMorphing[int(BWAPI::UnitTypes::Zerg_Hatchery)] = 1;
        }
    }

     // We will follow the following Tech tree Building :
     // Vespin geyser extractor, Spawning pool -> Lair, Hydralisk's Den, Evolution Chamber -> Queen's Nest

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] == 0) && ((*myUnits).number_Hatchery >= 2) && ((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Drone] + (*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Drone] >= 12) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() + (*myUnits).blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Extractor, desiredPos)) {
    }


    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 0)  && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] > 1) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + (*myUnits).blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool, desiredPos)){
    }


    //std::cout << ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 0) << "   " << ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) << "    " << (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + (*myUnits).blocked_minerals) << "    " << (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + (*myUnits).blocked_gas) << std::endl;
    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 0) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                std::cout << BWAPI::Broodwar->self()->minerals() << "   " << BWAPI::Broodwar->self()->gas() << std::endl;
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                std::cout << u->getType() << std::endl;
                (*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] = 2;
                break;
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 0) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] > 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + (*myUnits).blocked_gas) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den, desiredPos)) {
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + (*myUnits).blocked_gas)){
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->research(BWAPI::TechTypes::Lurker_Aspect)) {
                    (*myUnits).tech[BWAPI::TechTypes::Lurker_Aspect] = 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).tech[BWAPI::TechTypes::Lurker_Aspect] == 1) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()){
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den){
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] = 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 1) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Muscular_Augments] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + (*myUnits).blocked_minerals) &&(BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Muscular_Augments] = 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 0) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Muscular_Augments] == 1) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice() + (*myUnits).blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Evolution_Chamber, desiredPos)) {
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Metabolic_Boost] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Metabolic_Boost.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Metabolic_Boost.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
                if (u->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Metabolic_Boost] += 1;
                }
            }
        }
    }


    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Missile_Attacks] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber){
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Missile_Attacks] += 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Carapace] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Carapace.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Carapace.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Carapace] += 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Melee_Attacks] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Melee_Attacks.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Melee_Attacks.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Zerg_Melee_Attacks] += 1;
                }
            }
        }
    }
}



//Return a free unit (in no squad)
void getUnit(BWAPI::UnitType type, std::list<Squad*>& mySquads, BWAPI::Unit& unity) {
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
    //return (*myUnits).larva;
}


int transfer_squad(Squad& origin_Squad, Squad& destination_Squad, BWAPI::UnitType Type, int number) {
    int res = 0;
    while (res < number) {
        BWAPI::Unit transfered_unit = origin_Squad.remove_Unit(Type);
        if (transfered_unit != nullptr) {
            destination_Squad.add_Unit(transfered_unit);
            res += 1;
        }
        else {
            return res;
        }
    }
    return res;
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

Squad* getSquad(int Squad_type, int ActionId, std::list<Squad*>& mySquads, int size) {
    for (Squad* squad : mySquads) {
        if (squad->get_type() == Squad_type && squad->get_Action() == ActionId) {
            enlistUnit(squad, mySquads);
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
    enlistUnit(newSquad, mySquads);
    mySquads.push_back(newSquad);
    return newSquad;
}




// create the unit wanted for the squad

void enlistUnit(Squad* squad, std::list<Squad*>& mySquads) {
    squad->countSquadUnits();
    if (squad->get_droneWanted() > squad->get_droneOwned()) {
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Drone, mySquads, u);
        if (u != nullptr) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Drone) {
                squad->add_Unit(u);
            }
            else if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && BWAPI::Broodwar->self()->minerals() > BWAPI::UnitTypes::Zerg_Drone.mineralPrice() + (*myUnits).blocked_minerals) {
                u->morph(BWAPI::UnitTypes::Zerg_Drone);
            }
        }
    }

    if (squad->get_zerglingWanted() > squad->get_zerglingOwned()) {
        BWAPI::Unit u;
        getUnit(BWAPI::UnitTypes::Zerg_Zergling, mySquads, u);
        if (u != nullptr) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
                squad->add_Unit(u);
            }
            else if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && BWAPI::Broodwar->self()->minerals() > BWAPI::UnitTypes::Zerg_Drone.mineralPrice() + (*myUnits).blocked_minerals && BWAPI::Broodwar->self()->gas() > BWAPI::UnitTypes::Zerg_Drone.gasPrice() + (*myUnits).blocked_gas) {
                u->morph(BWAPI::UnitTypes::Zerg_Zergling);
            }
        }
    }
}