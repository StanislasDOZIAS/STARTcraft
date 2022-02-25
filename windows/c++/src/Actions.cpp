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

void Actions::morphFromLarva() {
    if (
        ((*myUnits).larva != nullptr) && ((*myUnits).nextUnitFromLarva != BWAPI::UnitTypes::Unknown) &&
        (BWAPI::Broodwar->self()->minerals() >= (*myUnits).nextUnitFromLarva.mineralPrice() + (*myUnits).blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= (*myUnits).nextUnitFromLarva.gasPrice() + (*myUnits).blocked_minerals) &&
        (*myUnits).larva->train((*myUnits).nextUnitFromLarva)
        ){
        (*myUnits).larva = nullptr;
    }
}

void Actions::Economy(std::list<Squad*>& mySquads) {
    //BWAPI::Unit builder = nullptr;
    Squad* gas;
    Squad* mineral;
    mineral = getSquad(1, 1, mySquads); //first 1 : squad of type worker, second 1 : squad which gather minerals
    if (mineral == nullptr) {
        mineral = new WorkerSquad((*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Drone]);
        mineral->changeAction(1);
        enlistUnit(mineral, mySquads);
        mySquads.push_back(mineral);
    }

    mineral->countSquadUnits();
    for (BWAPI::Unit unit : mineral->get_Units()) {
        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral && unit->isIdle()) { unit->rightClick(closestMineral); }
    }

    if ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] == 3) {
        BWAPI::Unit Extractor = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Extractor);
        gas = getSquad(1, 2, mySquads); //1 : squad of type worker, 2 : squad which gather minerals
        if (gas == nullptr) {
            gas = new WorkerSquad(4);
            gas->changeAction(2);
            enlistUnit(gas, mySquads);
            mySquads.push_back(gas);
        }
        gas->countSquadUnits();
        if (gas->get_Units().size() < 4) {
            transfer_squadType(mineral, gas, BWAPI::UnitTypes::Zerg_Drone, 4 - gas->get_Units().size());
        }
        for (BWAPI::Unit unit : gas->get_Units()) {
            if (unit->isIdle() || unit->isGatheringMinerals()) { unit->rightClick(Extractor); }
        }
    }
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


    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 0) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] > 1) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + (*myUnits).blocked_minerals) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool, desiredPos)) {
    }


    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 0) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                if (u->morph(BWAPI::UnitTypes::Zerg_Lair)) {
                    (*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] = 2;
                    break;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 0) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] > 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + (*myUnits).blocked_gas) &&
        MicroGestion::buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den, desiredPos)) {
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + (*myUnits).blocked_gas)) {
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
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] = 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 1) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Muscular_Augments] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + (*myUnits).blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    (*myUnits).upgrades[BWAPI::UpgradeTypes::Muscular_Augments] = 1;
                }
            }
        }
    }

    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 0) && ((*myUnits).upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 1) &&
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
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
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

void Actions::BaseArmy(std::list<Squad*>& mySquads, int* armyWanted) {
    Squad* Army;
    int ActionId = 2;
    Army = getSquad(2, ActionId, mySquads);
    if (Army == nullptr) {
        Army = new ArmySquad(armyWanted);
        Army->changeAction(ActionId);
        enlistUnit(Army, mySquads);
        mySquads.push_back(Army);
    }
}




//Usefull functions

// Say if a unit is in a squad
bool unitInSquad(BWAPI::Unit unit, std::list<Squad*>& mySquads) {
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

// transfer a unit from a squad to another
int transfer_squadType(Squad* origin_Squad, Squad* destination_Squad, BWAPI::UnitType Type, int number) {
    int res = 0;
    while (res < number) {
        BWAPI::Unit transfered_unit = origin_Squad->remove_UnitType(Type);
        if (transfered_unit != nullptr) {
            destination_Squad->add_Unit(transfered_unit);
            res += 1;
        }
        else {
            return res;
        }
    }
    return res;
}

void transfer_squad(Squad* origin_Squad, Squad* destination_Squad, BWAPI::Unit unit) {
    origin_Squad->remove_Unit(unit);
    destination_Squad->add_Unit(unit);
}


// Return the squad of a unit
Squad* getSquadUnit(BWAPI::Unit unit, std::list<Squad*>& mySquads) {
    for (Squad* squad : mySquads) {
        for (BWAPI::Unit u2 : squad->get_Units()) {
            if (u2->getID() == unit->getID()) {
                return squad;
            }
        }
    }
    return nullptr;
}


// Get a squad which the right parameters or return nullptr
Squad* getSquad(int Squad_type, int ActionId, std::list<Squad*>& mySquads) {
    for (Squad* squad : mySquads) {
        if (squad->get_type() == Squad_type && squad->get_Action() == ActionId) {
            enlistUnit(squad, mySquads);
            return squad;
        }
    }
    return nullptr;
}




// Add the unused unit to a squad
void enlistUnit(Squad* squad, std::list<Squad*>& mySquads) {
    squad->countSquadUnits();
    for (BWAPI::Unit unit : mySquads.front()->get_Units()){
        if ((squad->unitOwned[unit->getType()] + squad->unitMorphing[unit->getType()] < squad->unitWanted[unit->getType()])){
            transfer_squad(mySquads.front(), squad, unit);
            if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg) {
                squad->unitMorphing[unit->getBuildType()] += 1;
            }
            else {
                squad->unitOwned[unit->getType()] += 1;
            }
        }
    }
}