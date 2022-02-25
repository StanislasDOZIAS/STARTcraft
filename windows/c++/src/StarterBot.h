#pragma once

#include "MapTools.h"
#include "Squad.h"
#include "Scenario.h"
#include "Actions.h"
#include <BWAPI.h>


//forward declaration needed for compilation
struct UnitCount;
extern UnitCount* myUnits;

class StarterBot{
    MapTools m_mapTools;

public:

    StarterBot();
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
	//UnitCount* myUnits;
	std::list<Squad*> mySquads;
};