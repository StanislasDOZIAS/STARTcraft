#pragma once
#include <BWAPI.h>
#include "Scenario.h"
struct UnitCount;

//UnitCount* myUnits;


namespace MicroGestion {
    bool buildBuilding(BWAPI::UnitType building, BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation());
    BWAPI::Unit getBuilder();
}