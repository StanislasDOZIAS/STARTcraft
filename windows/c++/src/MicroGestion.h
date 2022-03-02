#pragma once
#include <BWAPI.h>
#include "Scenario.h"
struct UnitCount;

extern UnitCount* myUnits;


namespace MicroGestion {
    bool buildBuilding(BWAPI::UnitType building, BWAPI::TilePosition desiredPos, std::list<Squad*>& mySquads, BWAPI::Unit builder = nullptr);
    BWAPI::Unit getBuilder(std::list<Squad*>& mySquads);
}