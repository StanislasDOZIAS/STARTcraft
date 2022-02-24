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
	Squad(int Squad_type, int size);
	int get_type();
	int get_Action();
	int get_droneWanted();
	int get_zerglingWanted();
	int get_hydraWanted();
	int get_lurkerWanted();
	int get_droneOwned();
	int get_zerglingOwned();
	int get_hydraOwned();
	int get_lurkerOwned();
	void add_Unit(BWAPI::Unit& Unit);
	BWAPI::Unit Squad::remove_UnitType(BWAPI::UnitType& Type);
	void Squad::remove_Unit(BWAPI::Unit unit);
	void move(BWAPI::Position postion);
	void changeAction(int ActionId);
	std::list<BWAPI::Unit>& get_Units();
	void countSquadUnits();

protected :
	int type;
	int Action;
	int droneWanted;
	int zerglingWanted;
	int hydraWanted;
	int lurkerWanted;
	int droneOwned;
	int zerglingOwned;
	int hydraOwned;
	int lurkerOwned;
	std::list<BWAPI::Unit> Units;
};

class WorkerSquad : public Squad {
public:
	WorkerSquad(int size);
};

class ZerglingSquad : public Squad {
public :
	ZerglingSquad(int size);
};