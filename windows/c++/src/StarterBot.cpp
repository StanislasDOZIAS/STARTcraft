#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

StarterBot::StarterBot()
{
    
}

// 0 : The building isn't here
// 1 : The building is constructing
// 2 : The builindg is constructed

int got_Spawning_pool = 0;
int got_Extractor = 0;
int got_Lair = 0;
int got_Hydra_Den = 0;
int got_Evolution_Chamber = 0;
int got_Queens_Nest = 0;


// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(5);
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
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

    // Build Tech building

    buildTechBuilding();

    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

void StarterBot::sendWorkersToGaz(BWAPI::Unit Extractor)
{
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    int c = 0;
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker()&&c<3)
        {
        unit->rightClick(Extractor);
        c += 1;
        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything

    if (unusedSupply >= 2) { return; }
    // Otherwise, we are going to build a supply provider

    // For Zerg player the supply providers come from larva

    bool ovie_making = false;
    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Egg && u->getBuildType() == BWAPI::UnitTypes::Zerg_Overlord) {
            ovie_making = true;
        }
    }
    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Larva && u->canTrain(BWAPI::UnitTypes::Zerg_Overlord) && ovie_making == false) {
            u->train(BWAPI::UnitTypes::Zerg_Overlord);
            ovie_making = true;
        }
    }
}


bool StarterBot::buildBuilding(BWAPI::UnitType building)
{
    BWAPI::UnitType builderType = building.whatBuilds().first;
    BWAPI::Unit builder = Tools::GetUnitOfType(builderType);

    BWAPI::TilePosition desiredPos = BWAPI::Broodwar->self()->getStartLocation();

    int maxBuildRange = 64;
    bool buildOnCreep = building.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(building, desiredPos, maxBuildRange, buildOnCreep);
    return builder->build(building, buildPos);
}


void StarterBot::buildTechBuilding()
{
     // We will follow the following Tech tree Building :
     // Spawning pool, Vespin geyser extractor -> Lair,Hydralisk's Den,Evolution Chamber -> Queen's Nest


    if ((got_Spawning_pool == 0) && (BWAPI::Broodwar->self()->minerals()>=200) && buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)) {
            got_Spawning_pool = 1;
    }

    if ((got_Spawning_pool>0) && (got_Extractor==0) && (BWAPI::Broodwar->self()->minerals() >= 100) && buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)){
        got_Extractor = 1;
    }

    if ((got_Spawning_pool==2) && (BWAPI::Broodwar->self()->minerals() >= 150) && (BWAPI::Broodwar->self()->gas() >= 100) &&(got_Lair==0)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                if (Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Lair) != nullptr) {
                    got_Lair = 1;
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
    if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
        sendWorkersToGaz(unit);
        got_Spawning_pool = 2;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
        sendWorkersToGaz(unit);
        got_Extractor = 2;
    }
    
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