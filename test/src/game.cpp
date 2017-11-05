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
        LoadSound(SoundCommands, "/home/bross/code/glitchheartgame2017/test/assets/sounds/bonfire.wav", &GameState->TestSound);
    }
    
    if(KEY_DOWN(Key_S) && GameState->TestSound != -1)
    {
        PLAY_SOUND(GameState->TestSound);
    }
    
    //DEBUG_PRINT("Sound: %d\n", GameState->TestSound);
    
    if(KEY_DOWN(Key_Escape))
    {
        Renderer.ShouldClose = true;
    }
}