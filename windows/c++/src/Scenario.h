#pragma once
#include <BWAPI.h>
#include "Tools.h"
#include "StarterBot.h"
#include "Squad.h"

class Squad;

// on regroupe tous les compteurs d'unités dans une structure pour faciltier l'échange d'information
struct UnitCount {
    // Track the Buildings
    // 0 : The building isn't here
    // 1 : The building is going to start
    // 2 : The building is constructing
    // 3 : The builindg is constructed
    int unitBuilding[BWAPI::UnitTypes::Unknown];

    int number_Hatchery;

    // The upgrade we have

    int tech[BWAPI::TechTypes::Unknown];
    int upgrades[BWAPI::UpgradeTypes::Unknown];

    // The Unit we want

    int unitWanted[BWAPI::UnitTypes::Unknown];



    // The Combat Units we owne

    int unitOwned[BWAPI::UnitTypes::Unknown];

    int unitMorphing[BWAPI::UnitTypes::Unknown];

    int supplyAvailable;

    int blocked_minerals;
    int blocked_gas;

    int UsedSupply;


    // The only larva and hydra we need per frame
    BWAPI::Unit larva;
    BWAPI::Unit hydra;
    
    // The waiting frame to detect failed building
    int building_frame_count;
    int max_frame_building;
    BWAPI::UnitType building_in_progress;

    UnitCount();
};

extern UnitCount* myUnits;

int* Scenario(BWAPI::GameWrapper& Broodwar, std::list<Squad>& mySquads);

void countUnits(BWAPI::GameWrapper& Broodwar);