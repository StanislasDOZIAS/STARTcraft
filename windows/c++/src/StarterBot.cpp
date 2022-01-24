#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

StarterBot::StarterBot()
{
}

// 0 : The building isn't here
// 1 : The building is going to start
// 2 : The building is constructing
// 3 : The builindg is constructed

int got_Spawning_pool = 0;
int got_Extractor = 0;
int got_Lair = 0;
int got_Hydra_Den = 0;
int got_Evolution_Chamber = 0;
int got_Queens_Nest = 0;
int got_Hatchery = 0;
int number_Hatchery = 1;

// The Combat Units we owne

int zerglingOwned = 0;
int hydraOwned = 0;
int lurkerOwned = 0;
int mutaliskOwned = 0;
int ultraliskOwned = 0;

int zerglingMorphing = 0;
int hydraMorphing = 0;
int lurkerMorphing = 0;
int mutaliskMorphing = 0;
int ultraliskMorphing = 0;

// The lock ressources for buidlings

int blocked_minerals = 0;
int blocked_gas = 0;


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

    // Build Tech building
    buildTechBuilding();


    // Build Tech building
    buildAdditionalHatch();


    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build units
    builAdditionalUnits();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();

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
        if (unit->getType().isWorker() && c<4)
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
    if ((workersOwned < workersWanted) && (workerType.mineralPrice() < BWAPI::Broodwar->self()->minerals()-blocked_minerals))
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        BWAPI::Unit larva = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Larva);
        if (larva != nullptr) {
            if (larva->train(BWAPI::UnitTypes::Zerg_Drone)) {
            }
        }
    }
}


// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply available (pb when an overlords hatches) and scan if some overlords are beeing morphed

    int supplyAvailable = 0;
    int ovie_Morphing = 0;

    for (BWAPI::Unit unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord)
        {
            supplyAvailable += 16;
        }
        else if (unit->getType() == BWAPI::UnitTypes::Zerg_Egg && unit->getBuildType() == BWAPI::UnitTypes::Zerg_Overlord)
        {
            ovie_Morphing += 1;
        }
        else if ((unit->getType() == BWAPI::UnitTypes::Zerg_Hatchery && unit->isCompleted()) ||
            unit->getType() == BWAPI::UnitTypes::Zerg_Lair ||
            unit->getType() == BWAPI::UnitTypes::Zerg_Hive)
        {
            supplyAvailable += 2;
        }
    }

    const int supply_Used = BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if ((supplyAvailable - supply_Used < 4)) {
        // We don't authorize multiple overlords if supply_used <= 34 (i.e. 17)
        if (supply_Used <= 34) {
            BWAPI::Unit larva = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Larva);
            if ((BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() < BWAPI::Broodwar->self()->minerals() - blocked_minerals) && (larva != nullptr) && (ovie_Morphing==0)) {
                if (larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                }
            }
        }
        // We authorize 1 moprhing overlord if supply_used > 34 (i.e. 17)
        else {
            BWAPI::Unit larva = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Larva);
            if ((BWAPI::UnitTypes::Zerg_Overlord.mineralPrice() < BWAPI::Broodwar->self()->minerals() - blocked_minerals) && (larva != nullptr) && (ovie_Morphing <= 1)) {
                if (larva->train(BWAPI::UnitTypes::Zerg_Overlord)) {
                }
            }
        }
    }
}

// Train more combat units so we can fight people
void StarterBot::builAdditionalUnits()
{


    // Morph lukers
    if ((got_Hydra_Den == 3) && (lurkerMorphing + lurkerOwned < hydraOwned) && (BWAPI::UnitTypes::Zerg_Lurker.mineralPrice() < BWAPI::Broodwar->self()->minerals() - blocked_minerals) &&
        (BWAPI::UnitTypes::Zerg_Lurker.gasPrice() < BWAPI::Broodwar->self()->gas() - blocked_gas))
    {
        BWAPI::Unit hydra = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Hydralisk);
        if (hydra != nullptr) {
            if (hydra->morph(BWAPI::UnitTypes::Zerg_Lurker)) {
                lurkerMorphing = 1;
                hydraOwned -= 1;
                for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
                    if (u->getType() == BWAPI::UnitTypes::Zerg_Lurker_Egg) {
                        lurkerMorphing += 1;
                    }
                }
            }
        }
    }


   // Build hydras
    if ((got_Hydra_Den == 3) && (hydraOwned + hydraMorphing < 20) && (BWAPI::UnitTypes::Zerg_Hydralisk.mineralPrice() < BWAPI::Broodwar->self()->minerals() - blocked_minerals) &&
        (BWAPI::UnitTypes::Zerg_Zergling.gasPrice() < BWAPI::Broodwar->self()->gas() - blocked_gas))
    {
        BWAPI::Unit larva = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Larva);
        if (larva != nullptr) {
            if (larva->train(BWAPI::UnitTypes::Zerg_Hydralisk)) {
                hydraMorphing = 1;
                for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
                    if (u->getType() == BWAPI::UnitTypes::Zerg_Egg && u->getBuildType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
                        hydraMorphing += 1;
                    }
                }
            }
        }
    }


    // Build zergling
    if ((got_Spawning_pool==3)&&(zerglingOwned + zerglingMorphing < 50) && (BWAPI::UnitTypes::Zerg_Zergling.mineralPrice() < BWAPI::Broodwar->self()->minerals() - blocked_minerals))
    {
        BWAPI::Unit larva = Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Larva);
        if (larva != nullptr) {
            if (larva->train(BWAPI::UnitTypes::Zerg_Zergling)){
                zerglingMorphing = 1;
                for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
                    if (u->getType() == BWAPI::UnitTypes::Zerg_Egg && u->getBuildType() == BWAPI::UnitTypes::Zerg_Zergling) {
                        zerglingMorphing += 1;
                    }
                }
            }
        }
    }



}


bool StarterBot::buildBuilding(BWAPI::UnitType building)
{
    BWAPI::UnitType builderType = building.whatBuilds().first;
    BWAPI::Unit builder = nullptr;

    for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
        if (u->getType() == BWAPI::UnitTypes::Zerg_Drone && !((u->isGatheringGas()))) {
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


void StarterBot::buildAdditionalHatch()
{
    if ((got_Hatchery == 0) &&  (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hatchery.mineralPrice()*number_Hatchery - blocked_minerals) &&
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
}

void StarterBot::buildTechBuilding()
{
     // We will follow the following Tech tree Building :
     // Spawning pool, Vespin geyser extractor -> Lair,Hydralisk's Den,Evolution Chamber -> Queen's Nest


    if ((got_Spawning_pool == 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice() - blocked_minerals) &&
        (Tools::CountUnitsOfType(BWAPI::UnitTypes::Zerg_Drone, BWAPI::Broodwar->self()->getUnits())>=11) && buildBuilding(BWAPI::UnitTypes::Zerg_Spawning_Pool)){
        got_Spawning_pool = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    }


    if ((got_Spawning_pool == 1) && (Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Spawning_Pool) != nullptr)) {
        got_Spawning_pool = 2;
        blocked_minerals -= BWAPI::UnitTypes::Zerg_Spawning_Pool.mineralPrice();
    }


    if ((got_Extractor == 0) && (got_Spawning_pool > 1) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice() - blocked_minerals) &&
        buildBuilding(BWAPI::UnitTypes::Zerg_Extractor)){
        got_Extractor = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Extractor.mineralPrice();
    }

    if ((got_Extractor == 1) && (Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Extractor) != nullptr)) {
        got_Extractor = 2;
        blocked_minerals -= BWAPI::UnitTypes::Zerg_Extractor.mineralPrice();
    }

    if ((got_Lair == 0) && (got_Spawning_pool == 3) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Lair.mineralPrice() - blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Lair.gasPrice()-blocked_gas)) {
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hatchery) {
                u->morph(BWAPI::UnitTypes::Zerg_Lair);
                got_Lair = 2;
            }
        }
    }

    if ((got_Hydra_Den == 0) && (got_Lair > 0) && (BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice() - blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice() - blocked_gas) && buildBuilding(BWAPI::UnitTypes::Zerg_Hydralisk_Den)) {
        got_Hydra_Den = 1;
        blocked_minerals += BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice();
        blocked_gas += BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice();
    }

    if ((got_Hydra_Den == 1) && (Tools::GetUnitOfType(BWAPI::UnitTypes::Zerg_Hydralisk_Den) != nullptr)) {
        got_Hydra_Den = 2;
        blocked_minerals -= BWAPI::UnitTypes::Zerg_Hydralisk_Den.mineralPrice();
        blocked_gas -= BWAPI::UnitTypes::Zerg_Hydralisk_Den.gasPrice();
    }

    if ((got_Hydra_Den == 3) && (got_Lair == 3) && (number_Hatchery >= 2) &&  (BWAPI::Broodwar->self()->minerals() >= BWAPI::TechTypes::Lurker_Aspect.mineralPrice() - blocked_minerals) &&
        (BWAPI::Broodwar->self()->gas() >= BWAPI::TechTypes::Lurker_Aspect.gasPrice() - blocked_gas)){
        for (BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits()) {
            if (u->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
                u->research(BWAPI::TechTypes::Lurker_Aspect);
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
    if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
        got_Spawning_pool = 0;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
        sendWorkersToGaz(unit);
        got_Extractor = 0;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair) {
        got_Lair = 0;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
        got_Hydra_Den = 0;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
        zerglingOwned -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
        hydraOwned -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
        lurkerOwned -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk) {
        mutaliskOwned -= 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Ultralisk) {
        ultraliskOwned -= 1;
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

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord) {
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Spawning_Pool) {
        got_Spawning_pool = 3;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Extractor) {
        sendWorkersToGaz(unit);
        got_Extractor = 3;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lair) {
        got_Lair = 3;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk_Den) {
        got_Hydra_Den = 3;
    }


    if (unit->getType() == BWAPI::UnitTypes::Zerg_Zergling) {
        zerglingMorphing -= 1;
        zerglingOwned += 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Hydralisk) {
        hydraMorphing -= 1;
        hydraOwned += 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Lurker) {
        lurkerMorphing -= 1;
        lurkerOwned += 1;
        unit->burrow();
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Mutalisk) {
        mutaliskMorphing -= 1;
        mutaliskOwned += 1;
    }

    if (unit->getType() == BWAPI::UnitTypes::Zerg_Ultralisk) {
        ultraliskMorphing -= 1;
        ultraliskOwned += 1;
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