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
    Actions::buildAdditionalSupply();
    Actions::Building_tree(mySquads);
    Actions::Economy(mySquads);
    Actions::BaseArmy(mySquads);

    // To morph from combat unit
    morphFromCombatUnit();

    attackStartLocations();

    (*myUnits).building_frame_count += 1;
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