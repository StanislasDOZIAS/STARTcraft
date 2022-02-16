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

    if (builder->build(building, buildPos)) {
        (*myUnits).unitBuilding[building] = 1;
        (*myUnits).blocked_minerals += building.mineralPrice();
        (*myUnits).blocked_gas += building.gasPrice();
        (*myUnits).building_frame_count = 0;
        (*myUnits).building_in_progress = building;
        return true;
    }
    else {
        return false;
    }
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