#include "Squad.h"

Squad::Squad() {
	Units.resize(0);
	type = 0;
	Action = 0;
	std::memset(unitOwned, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitWanted, 0, 4 * int(BWAPI::UnitTypes::Unknown));
}

Squad::Squad(int Squad_type){
	Units.resize(0);
	type = 0;
	Action = 0;
	std::memset(unitOwned, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitWanted, 0, 4 * int(BWAPI::UnitTypes::Unknown));
}


BWAPI::Unit Squad::remove_UnitType(BWAPI::UnitType& Type) {
	for (BWAPI::Unit unit : Units) {
		if (unit->getType() == Type && (unit->exists())){
			Units.remove(unit);
			return unit;
		}
	}
	return nullptr;
}

void Squad::remove_Unit(BWAPI::Unit unit) {
	for (BWAPI::Unit u : Units) {
		if (unit->getID()==u->getID()) {
			Units.remove(u);
		}
	}
}


void Squad::add_Unit(BWAPI::Unit& Unit){
	if (Unit->getType() != BWAPI::UnitTypes::Zerg_Larva) {
		Units.push_back(Unit);
	}
}

void Squad::move(BWAPI::Position position){
	for (BWAPI::Unit unit : Units) {
		unit->move(position);
	}
}

std::list<BWAPI::Unit>& Squad::get_Units(){
	return Units;
}


int Squad::get_type() {
	return type;
}

int Squad::get_Action() {
	return Action;
}

int* Squad::getUnitOwned() {
	return unitOwned;
}

int* Squad::getUnitMorphing() {
	return unitMorphing;
}

int* Squad::getUnitWanted() {
	return unitWanted;
}


void Squad::changeAction(int ActionId) {
	Action = ActionId;
}

void Squad::countSquadUnits() {
	std::memset(unitOwned, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::list<BWAPI::Unit> to_remove;
	for (BWAPI::Unit unit : Units) {
		if (unit->exists()) {
			// We begin with morphing units
			if ((unit->getType() == BWAPI::UnitTypes::Zerg_Egg) || (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg)) {
				unitMorphing[unit->getBuildType()] += 1;
			}
			else {
				unitOwned[unit->getType()] += 1;
			}
		}
		else {
			to_remove.push_back(unit);
		}
	}
	for(BWAPI::Unit unit : to_remove) {
		Units.remove(unit);
	}
}

WorkerSquad::WorkerSquad(int size){
	Units.resize(0);
	type = 1;
	unitWanted[BWAPI::UnitTypes::Zerg_Drone] = size;
}


ArmySquad::ArmySquad(int* armyWanted) {
	Units.resize(0);
	type = 2;
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		unitWanted[unittype] = armyWanted[unittype];
	}
}