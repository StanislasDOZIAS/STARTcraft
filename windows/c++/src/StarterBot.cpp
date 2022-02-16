#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <iostream>

StarterBot::StarterBot()
{
}

// Track the Buildings
// 0 : The building isn't here
// 1 : The building is going to start
// 2 : The building is constructing
// 3 : The builindg is constructed
int unitBuilding[BWAPI::UnitTypes::Unknown] = { 0 };



// The Combat Units we Want
int unitWanted[BWAPI::UnitTypes::Unknown] = { 0 };


// The Combat Units we Owne
int unitOwned[BWAPI::UnitTypes::Unknown] = { 0 };

// The Combat Units which are morphing
int unitMorphing[BWAPI::UnitTypes::Unknown] = { 0 };

// The upgrade we have
int tech[BWAPI::TechTypes::Unknown] = { 0 };
int upgrades[BWAPI::UpgradeTypes::Unknown] = { 0 };



// Usefull counters

int number_Hatchery = 1;
int supplyAvailable = 0;

// The lock ressources for buidlings

int blocked_minerals = 0;
int blocked_gas = 0;

// The waiting frame to detect failed building
int building_frame_count = 0;
int max_frame_building = 24 * 10;
BWAPI::UnitType building_in_progress = BWAPI::UnitTypes::Unknown;

// The only larva and hydra we need per frame
BWAPI::Unit larva = nullptr;
BWAPI::Unit hydra = nullptr;

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(0);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    //
    // Reseting the counters
    //


    memset(unitBuilding,0,BWAPI::UnitTypes::Unknown);
    memset(unitOwned, 0, BWAPI::UnitTypes::Unknown);
    memset(unitMorphing, 0, BWAPI::UnitTypes::Unknown);
    memset(tech, 0, BWAPI::TechTypes::Unknown);
    memset(upgrades, 0, BWAPI::UpgradeTypes::Unknown);

    memset(unitWanted, 0, BWAPI::UnitTypes::Unknown);
    unitWanted[BWAPI::UnitTypes::Zerg_Drone] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Zergling] = 50;
    unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk] = 20;
    unitWanted[BWAPI::UnitTypes::Zerg_Lurker] = 10;
    

    // Usefull things

    // Number of hatches
    number_Hatchery = 1;

    // The lock ressources for buidlings
    blocked_minerals = 0;
    blocked_gas = 0;


    // The waiting frame to detect failed building
    building_frame_count = 0;


    // The only larva and hydra we need per frame
    larva = nullptr;
    hydra = nullptr;
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner) 
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();

    building_frame_count += 1;

    // Count our units
    countUnits();
    

    // Build Tech building
    buildTechBuilding();
    

    // Build Additional Hatcheries
    buildAdditionalHatch();


    // To morph from combat unit
    morphFromCombatUnit();

    attackStartLocations();

    // To morph from larva we need to have a larva
    if (larva != nullptr) {
        // Build more supply if we are going to run out soon
        if (buildAdditionalSupply()) {
            return;
        }

        // Train more workers so we can gather more income
        if (trainAdditionalWorkers()) {
            return;
        }

        // Build units
        if (builAdditionalUnits()) {
            return;
        }
    }
}




// Count all the units so thatwe don't count them many times
void StarterBot::countUnits()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();

    // Reset the units

    memset(unitOwned, 0, BWAPI::UnitTypes::Unknown);
    memset(unitMorphing, 0, BWAPI::UnitTypes::Unknown);
    unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] = 0;
    unitOwned[BWAPI::UnitTypes::Zerg_Lurker] = 0;

    supplyAvailable = 0;

    // Detect failed building

    if (building_frame_count > max_frame_building) {
        blocked_minerals -= building_in_progress.mineralPrice();
        blocked_gas -= building_in_progress.gasPrice();
        if (building_in_progress == BWAPI::UnitTypes::Zerg_Hatchery) {
            unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] = 0;
        }
        else {
            unitBuilding[building_in_progress] = 0;
        }
        building_in_progress = BWAPI::UnitTypes::Unknown;
    }

    for (BWAPI::Unit unit : myUnits)
    {
        // Buildings
        if (unit->getType().isBuilding()) {
            if (unit->getType() != BWAPI::UnitTypes::Zerg_Hatchery) {
                if (unit->isBeingConstructed()) {
                    if (unitBuilding[unit->getType()] == 1) {
                        blocked_minerals -= unit->getType().mineralPrice();
                        blocked_gas -= unit->getType().gasPrice();
                        building_in_progress = BWAPI::UnitTypes::Unknown;
                    }
                    unitBuilding[unit->getType()] = 2;

                }
                else {
                    if (unitBuilding[unit->getType()] == 2) { // it is just finished
                        if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
                            sendWorkersToGaz(unit);
                        }
                    }
                    unitBuilding[unit->getType()] = 3;
                }

            }
            else if ( (unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] == 1) && (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery) && (unit->isBeingConstructed()) )
            {
                    unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] = 0;
                    blocked_minerals -= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
                    number_Hatchery += 1;
                    building_in_progress = BWAPI::UnitTypes::Unknown;
            }
        }

        if ((unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && unit->isCompleted()) || unit->getType() == BWAPI::UnitTypes::Zerg_Hive || unit->getType() == BWAPI::UnitTypes::Zerg_Lair)
        {
            supplyAvailable += 2;
        }

        // "True" Units

        else {
            // We begin with morphing units
            if ((unit->getType() == BWAPI::UnitTypes::Zerg_Egg) || (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg)) {
                unitMorphing[unit->getBuildType()] += 1;
            }
            else {
                unitOwned[unit->getType()] += 1;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva) {
                larva = unit;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Drone) {
                // If it is an idle worker, then we want to send it somewhere
                if (unit->isIdle())
                {
                    // Get the closest mineral to this worker unit
                    BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

                    // If a valid mineral was found, right click it with the unit in order to start harvesting
                    if (closestMineral) { unit->rightClick(closestMineral); }
                }
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
                supplyAvailable += 16;
            }


            if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
                hydra = unit;
            }

            if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
                if (unit->isIdle()) {
                    unit->burrow();
                }
            }
        }
    }
}


void StarterBot::sendWorkersToGaz(BWAPI::Unit Extractor)
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    int c = 0;
    for (auto& unit : myUnits){
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isGatheringMinerals() && c<4)
        {
        unit->rightClick(Extractor);
        c += 1;
        }
    }
}

// Train more workers so we can gather more income
bool StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    
    if ((unitOwned[BWAPI::UnitTypes::Zerg_Drone] + unitMorphing[BWAPI::UnitTypes::Zerg_Drone] < unitWanted[BWAPI::UnitTypes::Zerg_Drone]) &&
        (BWAPI::Broodwar->self()->minerals() >= workerType.mineralPrice() + blocked_minerals)){
        if (larva->train(BWAPI::UnitTypes::Zerg_Drone)) {
            return true;
        }
    }

    return false;
}


// Build more supply if we are going to run out soon
bool StarterBot::buildAdditionalSupply()
{
    const int supply_Used = BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if ((supplyAvailable - supply_Used < 4) && (supplyAvailable < 400)) {
        // We don't authorize multiple overlords if supply_used <= 34 (i.e. 17)
        if (supply_Used <= 34) {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + blocked_minerals) && (larva != nullptr) && (unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] == 0)) {
                if (larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                    return true;
                }
            }
        }
        // We authorize 1 moprhing overlord if supply_used > 34 (i.e. 17)
        else {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + blocked_minerals) && (larva != nullptr) && (unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] <= 1)) {
                if (larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Train more combat units so we can fight people
bool StarterBot::builAdditionalUnits()
{
   // Build hydras
    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && (unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] + unitMorphing[BWAPI::UnitTypes::Zerg_Hydralisk] < unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk.gasPrice() + blocked_gas))
    {
        if (larva->train(BWAPI::UnitTypes::Zerg_Hydralisk)) {
            return true;
        }
    }

    // Build zergling
    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] ==3) && (unitOwned[BWAPI::UnitTypes::Zerg_Zergling] + unitMorphing[BWAPI::UnitTypes::Zerg_Zergling] < unitWanted[BWAPI::UnitTypes::Zerg_Zergling]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Zergling.mineralPrice() + blocked_minerals))
    {
        if (larva->train(BWAPI::UnitTypes::Zerg_Zergling)) {
            return true;
        }
    }

    return false;
}

void StarterBot::morphFromCombatUnit()
{
    // Morph lurkers
    if ((tech[BWAPI::TechTypes::Lurker_Aspect] == 1) && (unitOwned[BWAPI::UnitTypes::Zerg_Lurker] + unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] < unitWanted[BWAPI::UnitTypes::Zerg_Lurker]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lurker.mineralPrice() + blocked_minerals) &&(BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lurker.gasPrice() + blocked_gas)){
        if (hydra != nullptr) {
            hydra->morph(BWAPI::UnitTypes::Zerg_Lurker);
        }
    }
}

bool StarterBot::buildBuilding(BWAPI::UnitType building)
{
    BWAPI::Unit builder = nullptr;

    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Drone && (u->isGatheringMinerals())) {
            builder = u;
            break;
        }
    }

    if (builder == nullptr) {
        return false;
    }

    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();
    int maxBuildRange = 64;
    bool buildOnCreep = building.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(building, desiredPos, maxBuildRange, buildOnCreep);
    if (builder->build(building, buildPos)) {
        unitBuilding[building] = 1;
        blocked_minerals += building.mineralPrice();
        blocked_gas += building.gasPrice();
        building_frame_count = 0;
        building_in_progress = building;
        return true;
    }
    else {
        return false;
    }
}


void StarterBot::buildAdditionalHatch()
{
    if ((unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] == 0) &&  (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice()*number_Hatchery + blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Hatchery)) {
        unitMorphing[BWAPI::UnitTypes::Zerg_Hatchery] = 1;
    }

}

void StarterBot::buildTechBuilding()
{
     // We will follow the following Tech tree Building :
     // Vespin geyser extractor, Spawning pool -> Lair, Hydralisk's Den, Evolution Chamber -> Queen's Nest

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] == 0) && (number_Hatchery >= 2) && (unitOwned[BWAPI::UnitTypes::Zerg_Drone] + unitMorphing[BWAPI::UnitTypes::Zerg_Drone] >= 12) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() + blocked_minerals) && buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)) {
    }


    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 0)  && (unitBuilding[BWAPI::UnitTypes::Zerg_Extractor] > 1) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + blocked_minerals) && buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)){
    }



    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 0) && (unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                unitBuilding[BWAPI::UnitTypes::Zerg_Lair] = 2;
                break;
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 0) && (unitBuilding[BWAPI::UnitTypes::Zerg_Lair] > 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + blocked_gas) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den)) {
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && (unitBuilding[BWAPI::UnitTypes::Zerg_Lair] == 3) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + blocked_gas)){
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->research(BWAPI::TechTypes::Lurker_Aspect)) {
                    tech[BWAPI::TechTypes::Lurker_Aspect] = 1;
                }
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && (tech[BWAPI::TechTypes::Lurker_Aspect] == 1) && (upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()){
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den){
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    upgrades[BWAPI::UpgradeTypes::Grooved_Spines] = 1;
                }
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && (upgrades[BWAPI::UpgradeTypes::Grooved_Spines] == 1) && (upgrades[BWAPI::UpgradeTypes::Muscular_Augments] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + blocked_minerals) &&(BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    upgrades[BWAPI::UpgradeTypes::Muscular_Augments] = 1;
                }
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 0) && (upgrades[BWAPI::UpgradeTypes::Muscular_Augments] == 1) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice() + blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Evolution_Chamber)) {
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) && (upgrades[BWAPI::UpgradeTypes::Metabolic_Boost] == 0) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Metabolic_Boost.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Metabolic_Boost.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
                if (u->upgrade(BWAPI::UpgradeTypes::Metabolic_Boost)) {
                    upgrades[BWAPI::UpgradeTypes::Metabolic_Boost] += 1;
                }
            }
        }
    }


    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && (upgrades[BWAPI::UpgradeTypes::Zerg_Missile_Attacks] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber){
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)) {
                    upgrades[BWAPI::UpgradeTypes::Zerg_Missile_Attacks] += 1;
                }
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && (upgrades[BWAPI::UpgradeTypes::Zerg_Carapace] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Carapace.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Carapace.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Carapace)) {
                    upgrades[BWAPI::UpgradeTypes::Zerg_Carapace] += 1;
                }
            }
        }
    }

    if ((unitBuilding[BWAPI::UnitTypes::Zerg_Evolution_Chamber] == 3) && (upgrades[BWAPI::UpgradeTypes::Zerg_Melee_Attacks] < 2) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Melee_Attacks.mineralPrice() + blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Melee_Attacks.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber) {
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Melee_Attacks)) {
                    upgrades[BWAPI::UpgradeTypes::Zerg_Melee_Attacks] += 1;
                }
            }
        }
    }
}

void StarterBot::attackStartLocations() {
    if ((unitOwned[BWAPI::UnitTypes::Zerg_Zergling] >= unitWanted[BWAPI::UnitTypes::Zerg_Zergling]) && (unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] >= unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk])) {
        for (BWAPI::Unit unit : BWAPI::Broodwar->self()->getUnits()) {
            if ((unit->getType() == BWAPI::UnitTypes::Zerg_Zergling) || (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk)) {
                for (BWAPI::TilePosition ennemyLocation : BWAPI::Broodwar->getStartLocations()) {
                    if (ennemyLocation != BWAPI::Broodwar->self()->getStartLocation()) {
                        unit->attack(static_cast <BWAPI::Position>(ennemyLocation), true);
                    }
                }
            }
        }
    }
}


// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{

    // Buildings
    if (unit->getType().isBuilding()) {
        unitBuilding[unit->getType()] = 0;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair) {
        number_Hatchery -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
        number_Hatchery -= 1;
    }
}


// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{

}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)

{ 
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
    if ((unit->getPlayer()==BWAPI::Broodwar->self()) &&( unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)) {
        BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

        // If a valid mineral was found, right click it with the unit in order to start harvesting
        if (closestMineral) { unit->rightClick(closestMineral); }
    }


    // Units
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}