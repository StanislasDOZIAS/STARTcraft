#pragma once
#include <BWAPI.h>
#include "Squad.h"
#include "Scenario.h"
#include "StarterBot.h"
#include "MicroGestion.h"
#include "BWEM/mapImpl.h"

struct UnitCount;

extern UnitCount* myUnits;

class Squad;

namespace Actions {
	void morphFromLarva();

	void Economy(std::list<Squad*>& mySquads);

	void buildHatchery(std::list<Squad*>& mySquads);

	void buildTechTree(std::list<Squad*>& mySquads);

	void baseArmy(std::list<Squad*>& mySquads);
}

void buildAdditionalSupply();

void enlistUnit(Squad* squad, std::list<Squad*>& mySquads);

int transfer_squadType(Squad* origin_Squad, Squad* destination_Squad, BWAPI::UnitType Type, int number);

void transfer_squad(Squad* origin_Squad, Squad* destination_Squad, BWAPI::Unit unit);

Squad* getSquadUnit(BWAPI::Unit unit, std::list<Squad*>& mySquads);

Squad* getSquad(int Squad_type, int ActionId, std::list<Squad*>& mySquads);