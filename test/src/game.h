#ifndef GAME_H
#define GAME_H

#include "platform.h"
#include "shared.h"
#include "rendering.h"
#include "sound.h"
#include "keycontroller.h"

struct game_state
{
	memory_arena TotalArena;
	b32 IsInitialized;
};

#endif