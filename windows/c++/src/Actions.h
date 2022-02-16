#pragma once
#include <BWAPI.h>
#include "Squad.h"
#include "Scenario.h"
#include "StarterBot.h"
#include "MicroGestion.h"

struct UnitCount;

class Squad;

namespace Actions {
	void CreateNewBase(BWAPI::Position position, Squad& squad);

	void Building_tree(std::list<Squad*>& mySquads);

	void Economy(std::list<Squad*>& mySquads);

	void BaseArmy(std::list<Squad*>& mySquads);
}

void getUnit(BWAPI::UnitType type, std::list<Squad*>& mySquads, BWAPI::Unit& unity);

bool unitInSquad(BWAPI::Unit& unit, std::list<Squad*>& mySquads);

Squad* getSquad(int Squad_type, int ActionId, std::list<Squad*>& mySquads, int size = 0);

void enlistUnit(Squad* squad, std::list<Squad*>& mySquads);