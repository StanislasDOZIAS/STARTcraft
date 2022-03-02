#include "MicroGestion.h"

bool MicroGestion::buildBuilding(BWAPI::UnitType building, BWAPI::TilePosition desiredPos, std::list<Squad*>& mySquads, BWAPI::Unit builder){
    

    if (builder == nullptr) {
        builder = getBuilder(mySquads);
        if (builder == nullptr) {
            return false;
        }
    }

    int maxBuildRange = 64;
    bool buildOnCreep = building.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(building, desiredPos, maxBuildRange, buildOnCreep);

    std::cout << "Build Pos : " << buildPos<<std::endl;

    if (builder->build(building, buildPos)) {
        (*myUnits).builder = builder;
        (*myUnits).unitBuilding[building] = 1;
        (*myUnits).blocked_minerals += building.mineralPrice();
        (*myUnits).blocked_gas += building.gasPrice();
        (*myUnits).building_frame_count = 0;
        (*myUnits).building_in_progress = building;
        std::cout << "building : " << building << std::endl;
        return true;
    }
    else {
        return false;
    }
}

BWAPI::Unit MicroGestion::getBuilder(std::list<Squad*>& mySquads) {
    BWAPI::Unit builder = nullptr;

    for (Squad* squad : mySquads) {
        if (squad->get_type() == 1 && squad->get_Action() == 1) {
            builder = squad->remove_UnitType(BWAPI::UnitTypes::Zerg_Drone);
            break;
        }
    }
    return builder;
}