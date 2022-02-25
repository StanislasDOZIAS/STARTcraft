#include "Squad.h"

Squad::Squad() {
	type = 0;
	Action = 0;
	std::memset(unitOwned, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitWanted, 0, 4 * int(BWAPI::UnitTypes::Unknown));
}

Squad::Squad(int Squad_type){
	type = 0;
	Action = 0;
	std::memset(unitOwned, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitMorphing, 0, 4 * int(BWAPI::UnitTypes::Unknown));
	std::memset(unitWanted, 0, 4 * int(BWAPI::UnitTypes::Unknown));
}


BWAPI::Unit Squad::remove_UnitType(BWAPI::UnitType& Type) {
	for (BWAPI::Unit unit : Units) {
		if (unit->getType() == Type && (unit->exists())){
			remove(Units.begin(), Units.end(), unit);
			Units.resize(Units.size() - 1);
			return unit;
		}
	}
	return nullptr;
}

void Squad::remove_Unit(BWAPI::Unit unit) {
	remove(Units.begin(), Units.end(), unit);
	Units.resize(Units.size() - 1);
}


void Squad::add_Unit(BWAPI::Unit unit){
	if (unit->getType() != BWAPI::UnitTypes::Zerg_Larva) {
		Units.push_back(unit);
	}
}

void Squad::move(BWAPI::Position position){
	for (BWAPI::Unit unit : Units) {
		unit->move(position);
	}
}

std::vector<BWAPI::Unit>& Squad::get_Units(){
	return Units;
}


int Squad::get_type() {
	return type;
}

int Squad::get_Action() {
	return Action;
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
		remove(Units.begin(), Units.end(), unit);
	}
}

WorkerSquad::WorkerSquad(int size){
	type = 1;
	Action = 0;
	unitWanted[BWAPI::UnitTypes::Zerg_Drone] = size;
}


ArmySquad::ArmySquad(int* armyWanted) {
	type = 2;
	Action = 0;
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		unitWanted[unittype] = armyWanted[unittype];
	}
}