#pragma once
#include <BWAPI.h>
#include "Tools.h"
#include "StarterBot.h"
#include "Squad.h"

class Squad;

// on regroupe tous les compteurs d'unités dans une structure pour faciltier l'échange d'information
struct UnitCount {
    int got_Spawning_pool;
    int got_Extractor;
    int got_Lair;
    int got_Hydra_Den;
    int got_Evolution_Chamber;
    int got_Queens_Nest;
    int got_Hatchery;

    int number_Hatchery;

    // The upgrade we have

    int lurker_aspect;
    int grooved_spines;
    int muscular_augments;

    int ground_armor;
    int ground_dist_damage;
    int ground_melee_damage;

    int air_armor;
    int air_damage;

    // The Unit we want

    int droneWanted;
    int zerglingWanted;
    int hydraWanted;
    int lurkerWanted;


    // The Combat Units we owne

    int droneOwned;
    int zerglingOwned;
    int hydraOwned;
    int lurkerOwned;
    int mutaliskOwned;
    int ultraliskOwned;

    int droneMorphing;
    int zerglingMorphing;
    int hydraMorphing;
    int lurkerMorphing;
    int mutaliskMorphing;
    int ultraliskMorphing;

    int ovieMorphing;
    int supplyAvailable;

    int blocked_minerals;
    int blocked_gas;

    int UsedSupply;
    // The only larva and hydra we need per frame
    BWAPI::Unit larva;
    BWAPI::Unit hydra;
    
    UnitCount();
};

int* Scenario(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits, std::list<Squad>& mySquads);

void countUnits(BWAPI::GameWrapper& Broodwar, UnitCount& myUnits);