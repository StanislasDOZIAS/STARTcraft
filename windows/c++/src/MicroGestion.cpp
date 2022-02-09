#include "MicroGestion.h"

bool MicroGestion::buildBuilding(BWAPI::UnitType building, BWAPI::TilePosition desiredPos){
    BWAPI::Unit builder = getBuilder();

    if (builder == nullptr) {
        return false;
    }
    std::cout << "building : " << building << std::endl;
    int maxBuildRange = 64;
    bool buildOnCreep = building.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(building, desiredPos, maxBuildRange, buildOnCreep);
    return builder->build(building, buildPos);
}

BWAPI::Unit MicroGestion::getBuilder() {
    BWAPI::Unit builder = nullptr;

    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Drone && (u->isGatheringMinerals())) {
            builder = u;
            break;
        }
    }
    return builder;
}