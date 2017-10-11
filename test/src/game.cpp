#include "game.h"
#include "gmap.cpp"
#include "rendering.cpp"
#include "keycontroller.cpp"
#include "sound.cpp"
platform_api Platform;

extern "C" UPDATE(Update) 
{ 
	Platform = GameMemory->PlatformAPI;

	game_state* GameState = GameMemory->GameState;

	if(!GameState)
	{
	GameState = GameMemory->GameState = BootstrapPushStruct(game_state, TotalArena);
	GameState->IsInitialized = false;
	}

	Assert(GameState);

	if(!GameState->IsInitialized || !GameMemory->IsInitialized)
	{
	// Write your initialization code here 
	GameState->IsInitialized = true;
	GameMemory->IsInitialized = true;
	}
}