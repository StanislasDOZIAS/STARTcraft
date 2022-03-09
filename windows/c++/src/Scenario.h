#pragma once
#include <BWAPI.h>
#include "Tools.h"
#include "StarterBot.h"
#include "Squad.h"

class Squad;

// on regroupe tous les compteurs d'unités dans une structure pour faciltier l'échange d'information
struct UnitCount {
    // All the unit we owne (unit and buildings)
    int unitOwned[int(BWAPI::UnitTypes::Unknown)];


    // Track the Buildings construction
    // 0 : No plan for this building
    // 1 : The building is going to start
    // 2 : The building is constructing
    int unitBuilding[BWAPI::UnitTypes::Unknown];

    int number_Hatchery;

    // The upgrade we have

    int tech[int(BWAPI::TechTypes::Unknown)];
    int upgrades[int(BWAPI::UpgradeTypes::Unknown)];

    // The Unit we want

    int unitWanted[int(BWAPI::UnitTypes::Unknown)];


    // The Combat Units

    int unitMorphing[int(BWAPI::UnitTypes::Unknown)];

    int supplyAvailable;

    int blocked_minerals;
    int blocked_gas;

    int UsedSupply;


    // The only larva and hydra we need per frame and the target for larva
    BWAPI::Unit larva;
    BWAPI::UnitType nextUnitFromLarva;


    // Usefull things for buildings
    BWAPI::Unit builder;
    int building_frame_count;
    int max_frame_building;
    BWAPI::UnitType building_in_progress;

    BWAPI::TilePosition secondBasePos;
    bool foundSecondBasePos;
    BWAPI::Unit secondBaseBuilder;
    BWAPI::Unit secondBase;

    BWAPI::Unit first_extractor;
    BWAPI::Unit second_extractor;

    UnitCount();
};

extern UnitCount* myUnits;

void Scenario(BWAPI::GameWrapper& Broodwar, std::list<Squad*>& mySquads);

void countUnits(BWAPI::GameWrapper& Broodwar, std::list<Squad*>& mySquads);

void nextLarvaMorph(BWAPI::GameWrapper& Broodwar);