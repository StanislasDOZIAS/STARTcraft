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


BWAPI::Unit Squad::remove_UnitType(BWAPI::UnitType Type) {
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

void Squad::attack(BWAPI::Position target) {

	double slower = 10000;
	BWAPI::UnitType slowType = BWAPI::UnitTypes::Zerg_Hydralisk;
	double temp = 0;
	int min_range = 10000;
	BWAPI::Unit nearest_slow_unit = nullptr;
	for (BWAPI::Unit u : Units) {
		if ((u->getType() == slowType) && u->getDistance(target) < min_range) {
			nearest_slow_unit = u;
			min_range = u->getDistance(target);
		}
	}


	for (BWAPI::Unit unit : Units) {
		if (MicroGestion::detectEnnemieClose(unit)) {
			for (BWAPI::Unit u : Units) {
				if (u->isIdle() && u->canAttack()) {
					u->attack(target, false);
				}
				if (u->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
					if (MicroGestion::detectEnnemieClose(u)) {
						if (u->isMoving() || (!u->isBurrowed() && u->isIdle())) {
							u->burrow();
						}
					}
					else{
						if (u->isBurrowed()) {
							u->unburrow();
						}
						else {
							u->move(target, false);
						}
					}
				}
				if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord){
					if (nearest_slow_unit != nullptr) {
						unit->move(nearest_slow_unit->getPosition(), false);
					}
					else {
						unit->move(target, false);
					}
				}

			}
			return;
		}
	}


	if (nearest_slow_unit != nullptr) {
		for (BWAPI::Unit unit : Units) {
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
				if (unit->isBurrowed()) {
					unit->unburrow();
				}
				else {
					unit->move(target, false);
				}
			}
			else if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
				unit->move(nearest_slow_unit->getPosition(), false);
			}
			else if (unit->getType() != slowType) {
				unit->attack(nearest_slow_unit->getPosition(), false);
			}
			else {
				unit->attack(target, false);
			}
		}
	}
}

void Squad::unlimitedUnitWanted() {
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		if (unitWanted[unittype]>0) {
			unitWanted[unittype] = 200;
		}
	}
	unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 10;
	unitWanted[BWAPI::UnitTypes::Zerg_Overlord] = 1;
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
		if (unit->exists() && !(unit->getType().isBuilding())) {
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
		remove_Unit(unit);
	}
}

WorkerSquad::WorkerSquad(int size){
	type = 1;
	Action = 0; //1 if minerals, 2 if gas
	unitWanted[BWAPI::UnitTypes::Zerg_Drone] = size;
}


ArmySquad::ArmySquad(int* armyWanted) {
	type = 2;
	Action = 0;
	alreadyAttacking = false;
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		unitWanted[unittype] = armyWanted[unittype];
	}
	unitWanted[int(BWAPI::UnitTypes::Zerg_Overlord)] = 1;
}