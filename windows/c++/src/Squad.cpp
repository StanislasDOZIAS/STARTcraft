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


std::vector<BWAPI::Unit>& Squad::get_Units() {
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


// Remove a unit from a defined Type
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

// Remove the given Unit
void Squad::remove_Unit(BWAPI::Unit unit) {
	remove(Units.begin(), Units.end(), unit);
	Units.resize(Units.size() - 1);
}

// Add the given Unit
void Squad::add_Unit(BWAPI::Unit unit){
	if (unit->getType() != BWAPI::UnitTypes::Zerg_Larva) {
		Units.push_back(unit);
	}
}

// Count the unit in the squad and update unitOwned and unitMorphing, in addition to remove the dead units
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
	for (BWAPI::Unit unit : to_remove) {
		remove_Unit(unit);
	}
}


// Move all the squad to the position
void Squad::move(BWAPI::Position position){
	for (BWAPI::Unit unit : Units) {
		unit->move(position);
	}
}

// Decide if we want to attack and launch the attack if needed
bool Squad::decideAttack(bool alreadyAttacking) {
	bool wantAttack = (((unitOwned[BWAPI::UnitTypes::Zerg_Zergling] >= unitWanted[BWAPI::UnitTypes::Zerg_Zergling]) &&
		(unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] >= unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk]) &&
		(unitOwned[BWAPI::UnitTypes::Zerg_Lurker] >= unitWanted[BWAPI::UnitTypes::Zerg_Lurker])));
	if (wantAttack && !alreadyAttacking) {
		unlimitedUnitWanted();
	}
	if (wantAttack || alreadyAttacking) {
		for (BWAPI::TilePosition ennemyLocation : BWAPI::Broodwar->getStartLocations()) {
			if (ennemyLocation != BWAPI::Broodwar->self()->getStartLocation()) {
				BWAPI::Position target = static_cast <BWAPI::Position>(ennemyLocation);
				attack(target);
				return true;
			}
		}
	}
	return false;
}

// Set "unlimited" unit wanted to the squad
void Squad::unlimitedUnitWanted() {
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		if (unitWanted[unittype] > 0) {
			unitWanted[unittype] = 400; // the max supply is 200 and Zergling count as 0.5, so 400 on each unit is "Unlimited"
		}
	}
	unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 10; // They come from Hydralisk and so we don't want too many of them
	unitWanted[BWAPI::UnitTypes::Zerg_Overlord] = 1; // Only one is needed to detect inivisible units
}

// Make all the squad attack a target position, managing the different move speed
void Squad::attack(BWAPI::Position target) {

	//Get the closest Hydralisk
	BWAPI::UnitType slowType = BWAPI::UnitTypes::Zerg_Hydralisk;
	int min_range = 10000;
	BWAPI::Unit nearest_slow_unit = nullptr;
	for (BWAPI::Unit u : Units) {
		if ((u->getType() == slowType) && u->getDistance(target) < min_range) {
			nearest_slow_unit = u;
			min_range = u->getDistance(target);
		}
	}


	for (BWAPI::Unit unit : Units) {
		if (MicroGestion::detectEnnemieClose(unit)) { //We detect an ennemy, everyone attack
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


	if (nearest_slow_unit != nullptr) { // We didn't detect any ennemy and we have an hydra, the other Types of unit follow the closest
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
			else if (unit->getType() != slowType && unit->canAttack()) {
				unit->attack(nearest_slow_unit->getPosition(), false);
			}
			else {
				unit->attack(target, false);
			}
		}
	}
}

// Make the unit of the squad defend the second Base
void Squad::Defend() {
	BWAPI::Position B2_pos = static_cast <BWAPI::Position>(myUnits->secondBasePos);
	BWAPI::Position centerPos = BWAPI::Positions::Origin;
	int numberPos = 0;
	for (BWAPI::TilePosition ennemyLocation : BWAPI::Broodwar->getStartLocations()) {
		centerPos += static_cast <BWAPI::Position>(ennemyLocation);
		numberPos += 1;
	}
	centerPos.x = centerPos.x / numberPos;
	centerPos.y = centerPos.y / numberPos;

	BWAPI::Position def_pos;
	def_pos.x = 0.8 * B2_pos.x + 0.2 * centerPos.x;
	def_pos.y = 0.85 * B2_pos.y + 0.15 * centerPos.y;

	// We only defend in the second base if we have enough units (to counter very fast rush with the drone of the first base)
	if (Units.size() <= 6) {
		def_pos = static_cast <BWAPI::Position>(BWAPI::Broodwar->self()->getStartLocation());
	}

	for (BWAPI::Unit unit : Units) {
		if (unit->isIdle()) {
			unit->attack(def_pos, false);
		}
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
			if (unit->isIdle()) {
				unit->burrow();
			}
		}
		if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
			unit->move(def_pos, false);
		}
	}
}

// Morph lurker from Hydralisks inside the squad
void Squad::morphLurker() {
	if ((myUnits->tech[BWAPI::TechTypes::Lurker_Aspect] == 1) && (unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] >= 1) &&
		(unitOwned[BWAPI::UnitTypes::Zerg_Lurker] + unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] < unitWanted[BWAPI::UnitTypes::Zerg_Lurker]) &&
		(BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lurker.mineralPrice() + myUnits->blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lurker.gasPrice() + myUnits->blocked_gas)) {
		for (BWAPI::Unit unit : Units) {
			if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
				if (unit->morph(BWAPI::UnitTypes::Zerg_Lurker)) {
					unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] += 1;
					break;
				}
			}
		}
	}
}


// The type of squad //

WorkerSquad::WorkerSquad(int size){
	type = 1;
	Action = 0; //1 if minerals, X2 if gas (where X is the base linked to the extractor)
	unitWanted[BWAPI::UnitTypes::Zerg_Drone] = size;
}

ArmySquad::ArmySquad(int* armyWanted) {
	type = 2;
	Action = 0;
	alreadyAttacking = false;
	for (int unittype = 0; unittype < BWAPI::UnitTypes::Unknown; ++unittype) {
		unitWanted[unittype] = armyWanted[unittype];
	}
	unitWanted[int(BWAPI::UnitTypes::Zerg_Drone)] = 0;
	unitWanted[int(BWAPI::UnitTypes::Zerg_Overlord)] = 1;
}