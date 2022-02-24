#pragma once
#include <BWAPI.h>
#include "Squad.h"
#include "Scenario.h"
#include "StarterBot.h"
#include "MicroGestion.h"

struct UnitCount;

extern UnitCount* myUnits;

class Squad;

namespace Actions {
	void buildAdditionalSupply();

	void morphFromLarva();

	void Economy(std::list<Squad*>& mySquads);

	void Building_tree(std::list<Squad*>& mySquads);

	void BaseArmy(std::list<Squad*>& mySquads);
}

bool unitInSquad(BWAPI::Unit& unit, std::list<Squad*>& mySquads);

void getUnit(BWAPI::UnitType type, std::list<Squad*>& mySquads, BWAPI::Unit& unity);

int transfer_squad(Squad& origin_Squad, Squad& destination_Squad, BWAPI::UnitType Type, int number);

Squad* getSquadUnit(BWAPI::Unit& unit, std::list<Squad*>& mySquads);

Squad* getSquad(int Squad_type, int ActionId, std::list<Squad*>& mySquads, int size = 0);

void enlistUnit(Squad* squad, std::list<Squad*>& mySquads);