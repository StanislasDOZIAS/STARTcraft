#pragma once

#include "MapTools.h"

#include <BWAPI.h>

class StarterBot
{
    MapTools m_mapTools;

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
	void countUnits();
	void sendWorkersToGaz(BWAPI::Unit);
    bool trainAdditionalWorkers();
    bool buildAdditionalSupply();
	bool builAdditionalUnits();
	void morphFromCombatUnit();
	bool buildBuilding(BWAPI::UnitType building);
	void buildAdditionalHatch();
	void buildTechBuilding();
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
};