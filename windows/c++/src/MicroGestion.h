#pragma once
#include <BWAPI.h>

namespace MicroGestion {
    bool buildBuilding(BWAPI::UnitType building, BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation());
    BWAPI::Unit getBuilder();
}