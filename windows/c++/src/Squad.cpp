#include "Squad.h"

Squad::Squad() {
	Units.resize(0);
	Squad_size = 0;
	type = 0;
	Action = 0;
	droneWanted = 0;
	zerglingWanted = 0;
	hydraWanted = 0;
	lurkerWanted = 0;
	droneOwned = 0;
	zerglingOwned = 0;
	hydraOwned = 0;
	lurkerOwned = 0;
}

Squad::Squad(int Squad_type, int size) {
	Units.resize(0);
	Squad_size = 0;
	type = 0;
	Action = 0;
	droneWanted = 0;
	zerglingWanted = 0;
	hydraWanted = 0;
	lurkerWanted = 0;
	droneOwned = 0;
	zerglingOwned = 0;
	hydraOwned = 0;
	lurkerOwned = 0;
}

int Squad::get_Squad_size()
{
	return Squad_size;
}

BWAPI::Unit Squad::remove_Unit(BWAPI::UnitType& Type) {
	for (BWAPI::Unit unit : Units) {
		if (unit->getType() == Type && (unit->exists())){
			Units.remove(unit);
			Squad_size -= 1;
			return unit;
		}
	}
	return nullptr;
}


void Squad::add_Unit(BWAPI::Unit& Unit){
	if (Unit->getType() != BWAPI::UnitTypes::Zerg_Larva) {
		Units.push_back(Unit);
		Squad_size += 1;
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

int Squad::get_droneOwned() {
	return droneOwned;
}

int Squad::get_zerglingOwned() {
	return zerglingOwned;
}

int Squad::get_lurkerOwned() {
	return lurkerOwned;
}

int Squad::get_hydraOwned() {
	return hydraOwned;
}

int Squad::get_droneWanted() {
	return droneWanted;
}

int Squad::get_zerglingWanted() {
	return zerglingWanted;
}

int Squad::get_lurkerWanted() {
	return lurkerWanted;
}

int Squad::get_hydraWanted() {
	return hydraWanted;
}

void Squad::changeAction(int ActionId) {
	Action = ActionId;
}

void Squad::countSquadUnits() {
	droneOwned = 0;
	zerglingOwned = 0;
	hydraOwned = 0;
	lurkerOwned = 0;
	for (BWAPI::Unit unit : Units) {
		if (unit->exists()) {
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Drone) {
				droneOwned += 1;
			}
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
				zerglingOwned += 1;
			}
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
				hydraOwned += 1;
			}
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
				lurkerOwned += 1;
			}
		}
		}
}

WorkerSquad::WorkerSquad(int size){
	Units.resize(0);
	type = 1;
	droneWanted = size;
}


ZerglingSquad::ZerglingSquad(int size) {
	Units.resize(0);
	type = 2;
	zerglingWanted = size;
}