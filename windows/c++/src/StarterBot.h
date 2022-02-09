#pragma once

#include "MapTools.h"
#include "Squad.h"
#include "Scenario.h"
#include "Actions.h"
#include <BWAPI.h>


//forward declaration needed for compilation
struct UnitCount;

class StarterBot{
    MapTools m_mapTools;

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
	//void countUnits();
	void sendWorkersToGaz(BWAPI::Unit);
    bool trainAdditionalWorkers();
    bool buildAdditionalSupply();
	bool builAdditionalUnits();
	void morphFromCombatUnit();
	//bool buildBuilding(BWAPI::UnitType);
	//void buildAdditionalHatch();
	//void buildTechBuilding();
	void attackStartLocations();
	void drawDebugInformation();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);


	Squad& getSquad(int Squad_type, int ActionId, int size = 0);
	BWAPI::Unit& getUnit(BWAPI::UnitType type);
	void enlistUnit(Squad& squad);
	bool unitInSquad(BWAPI::Unit unit);

private:
	UnitCount* myUnits;
	std::list<Squad*> mySquads;
};