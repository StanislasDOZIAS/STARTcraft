#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <iostream>

StarterBot::StarterBot() {
}

// Called when the bot starts!
void StarterBot::onStart()
{
    myUnits = new UnitCount();

    // Set our BWAPI options here    
    BWAPI::Broodwar->setLocalSpeed(0);
    BWAPI::Broodwar->setFrameSkip(0);

    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";

    delete myUnits;
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

    //int* Actions = Scenario(BWAPI::Broodwar, myUnits, this);

    // Count our units
    //countUnits();
    countUnits(BWAPI::Broodwar);
    Actions::Building_tree(mySquads);
    Actions::Economy(mySquads);
    Actions::BaseArmy(mySquads);

    // Build Tech building
    //buildTechBuilding();


    // Build Additional Hatcheries
    //buildAdditionalHatch();

    // To morph from combat unit
    morphFromCombatUnit();

    attackStartLocations();

    // To morph from larva we need to have a larva
    if ((*myUnits).larva != nullptr) {
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


void StarterBot::sendWorkersToGaz(BWAPI::Unit Extractor)
{
    const BWAPI::Unitset& myListUnits = BWAPI::Broodwar->self()->getUnits();
    int c = 0;
    for (auto& unit : myListUnits) {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && c < 4)
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

    if (((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Drone] + (*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Drone] < (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Drone]) &&
        (BWAPI::Broodwar->self()->minerals() >= workerType.mineralPrice() + (*myUnits).blocked_minerals)) {
        if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Drone)) {
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
    if (((*myUnits).supplyAvailable - supply_Used < 4) && ((*myUnits).supplyAvailable < 400)) {
        // We don't authorize multiple overlords if supply_used <= 34 (i.e. 17)
        if (supply_Used <= 34) {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + (*myUnits).blocked_minerals) && ((*myUnits).larva != nullptr) && ((*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] == 0)) {
                if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                    return true;
                }
            }
        }
        // We authorize 1 moprhing overlord if supply_used > 34 (i.e. 17)
        else {
            if ((BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() + (*myUnits).blocked_minerals) && ((*myUnits).larva != nullptr) && ((*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Overlord] <= 1)) {
                if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
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
    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Hydralisk_Den] == 3) && ((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] + (*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Hydralisk] < (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk.gasPrice() + (*myUnits).blocked_gas))
    {
        if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Hydralisk)) {
            return true;
        }
    }

    // Build zergling
    if (((*myUnits).unitBuilding[BWAPI::UnitTypes::Zerg_Spawning_Pool] == 3) && ((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Zergling] + (*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Zergling] < (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Zergling]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Zergling.mineralPrice() + (*myUnits).blocked_minerals))
    {
        if ((*myUnits).larva->train(BWAPI::UnitTypes::Zerg_Zergling)) {
            return true;
        }
    }

    return false;
}

void StarterBot::morphFromCombatUnit()
{
    // Morph lurkers
    if (((*myUnits).tech[BWAPI::TechTypes::Lurker_Aspect] == 1) && ((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Lurker] + (*myUnits).unitMorphing[BWAPI::UnitTypes::Zerg_Lurker] < (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Lurker]) &&
        (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lurker.mineralPrice() + (*myUnits).blocked_minerals) && (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lurker.gasPrice() + (*myUnits).blocked_gas)) {
        if ((*myUnits).hydra != nullptr) {
            (*myUnits).hydra->morph(BWAPI::UnitTypes::Zerg_Lurker);
        }
    }
}


/*
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
    return builder->build(building, buildPos);
}
/*
/*
void StarterBot::buildAdditionalHatch()
{
    if ((got_Hatchery == 0) &&  (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice()*number_Hatchery + blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Hatchery)) {
        got_Hatchery = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
    }
    if (got_Hatchery == 1){
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery && u->isBeingConstructed()) {
                got_Hatchery = 0;
                blocked_minerals -= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice();
                number_Hatchery += 1;
            }
        }
    }
}*/
/*
void StarterBot::buildTechBuilding()
{
     // We will follow the following Tech tree Building :
     // Spawning pool, Vespin geyser extractor -> Lair, Hydralisk's Den, Evolution Chamber -> Queen's Nest
    if ((got_Spawning_pool == 0) && (number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() + blocked_minerals) &&
        (droneOwned + droneMorphing >=11) && buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)){
        got_Spawning_pool = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    }
    if ((got_Extractor == 0) && (got_Spawning_pool > 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() + blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)){
        got_Extractor = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Extractor.mineralPrice();
    }
    if ((got_Lair == 0) && (got_Spawning_pool == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                got_Lair = 2;
            }
        }
    }
    if ((got_Hydra_Den == 0) && (got_Lair > 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() + blocked_gas) && buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den)) {
        got_Hydra_Den = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice();
        blocked_gas += BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice();
    }
    if ((got_Hydra_Den == 3) && (got_Lair == 3) && (number_Hatchery >= 2) &&  (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() + blocked_gas)){
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->research(BWAPI::TechTypes::Lurker_Aspect)) {
                    lurker_aspect = 1;
                }
            }
        }
    }
    if ((got_Hydra_Den == 3) && (lurker_aspect == 1) && (number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Grooved_Spines.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Grooved_Spines.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()){
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den){
                if (u->upgrade(BWAPI::UpgradeTypes::Grooved_Spines)) {
                    grooved_spines = 1;
                }
            }
        }
    }
    if ((got_Hydra_Den == 3) && (grooved_spines == 1) && (number_Hatchery >= 2) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Muscular_Augments.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Muscular_Augments.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                if (u->upgrade(BWAPI::UpgradeTypes::Muscular_Augments)) {
                    muscular_augments = 1;
                }
            }
        }
    }
    if ((got_Evolution_Chamber == 0) && (grooved_spines == 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice() + blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Evolution_Chamber)) {
        got_Evolution_Chamber = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Evolution_Chamber.mineralPrice();
    }
    if ((got_Evolution_Chamber == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.mineralPrice() + blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UpgradeTypes::Zerg_Missile_Attacks.gasPrice() + blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Evolution_Chamber){
                if (u->upgrade(BWAPI::UpgradeTypes::Zerg_Missile_Attacks)) {
                    ground_dist_damage = 1;
                }
            }
        }
    }
}
*/
void StarterBot::attackStartLocations() {
    if (((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Zergling] >= (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Zergling]) && ((*myUnits).unitOwned[BWAPI::UnitTypes::Zerg_Hydralisk] >= (*myUnits).unitWanted[BWAPI::UnitTypes::Zerg_Hydralisk])) {
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
        (*myUnits).unitBuilding[unit->getType()] = 0;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair) {
        (*myUnits).number_Hatchery -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
        (*myUnits).number_Hatchery -= 1;
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
    if ((unit->getPlayer() == BWAPI::Broodwar->self()) && (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)) {
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