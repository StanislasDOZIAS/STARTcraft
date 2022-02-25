#pragma once
#include <iostream>
#include <vector>
#include <BWAPI.h>
#include "StarterBot.h"

struct UnitCount;

extern UnitCount* myUnits;


class Squad{
public:
	Squad();
	Squad(int Squad_type);
	int get_type();
	int get_Action();

	void add_Unit(BWAPI::Unit Unit);
	BWAPI::Unit Squad::remove_UnitType(BWAPI::UnitType& Type);
	void Squad::remove_Unit(BWAPI::Unit unit);
	void move(BWAPI::Position postion);
	void changeAction(int ActionId);
	std::vector<BWAPI::Unit>& get_Units();
	void countSquadUnits();

	int type;
	int Action;
	int unitWanted[int(BWAPI::UnitTypes::Unknown)];
	int unitOwned[int(BWAPI::UnitTypes::Unknown)];
	int unitMorphing[int(BWAPI::UnitTypes::Unknown)];

	std::vector<BWAPI::Unit> Units;
};

class WorkerSquad : public Squad {
public:
	WorkerSquad::WorkerSquad(int size);
};

class ArmySquad : public Squad {
public:
	ArmySquad(int unitWanted[int(BWAPI::UnitTypes::Unknown)]);
};