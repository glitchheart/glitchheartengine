#include "game.h"
#include "keycontroller.cpp"
#include "sound.cpp"

//CONSOLE STUFF TODO(Daniel) MOOOOOOOOOOOOOOOOOOVE
void ExecuteCommand(game_state *GameState)
{
    if (strcmp(GameState->Console.Buffer, "exit") == 0)
    {
        glfwDestroyWindow(GameState->RenderState.Window);
        glfwTerminate();
        exit(EXIT_SUCCESS);
    }
}

extern "C" UPDATE(Update)
{
   glfwGetFramebufferSize(GameState->RenderState.Window, &GameState->RenderState.WindowWidth, &GameState->RenderState.WindowHeight);

    if (GetKeyDown(GLFW_KEY_ENTER, GameState))
    {
        if (GameState->Console.Open)
        {
            // ExecuteCommand(GameState);
        }
        else
        {
            PlaySoundOnce(&GameState->SoundManager.LoadedSounds[THEME_MUSIC]);
        }
    }

    if (GetKeyDown(GLFW_KEY_1, GameState))
    {
        PlaySoundOnce(&GameState->SoundManager.LoadedSounds[SOUND_02]);
    }

    if (GetKeyDown(GLFW_KEY_2, GameState))
    {
        PlaySoundOnce(&GameState->SoundManager.LoadedSounds[SOUND_03]);
    }

    if (GetKeyDown(GLFW_KEY_3, GameState))
    {
        PlaySoundOnce(&GameState->SoundManager.LoadedSounds[SOUND_04]);
    }

    if (GetKeyDown(GLFW_KEY_4, GameState))
    {
        PlaySoundOnce(&GameState->SoundManager.LoadedSounds[SOUND_05]);
    }

    if (GetKeyDown(GLFW_KEY_5, GameState))
    {
        PlaySoundOnce(&GameState->SoundManager.LoadedSounds[SOUND_06]);
    }

    if (GetKeyDown(GLFW_KEY_TAB, GameState))
    {
        GameState->Console.Open = !GameState->Console.Open;
    }

    //player movement
    if (GetKey(GLFW_KEY_A, GameState))
    {
        GameState->Player.Position.x += -GameState->Player.player.WalkingSpeed * DeltaTime;
    }   
    else if (GetKey(GLFW_KEY_D, GameState))
    {
        GameState->Player.Position.x += GameState->Player.player.WalkingSpeed * DeltaTime;
    }

    if (GetKey(GLFW_KEY_W, GameState))
    {
        GameState->Player.Position.y += -GameState->Player.player.WalkingSpeed * DeltaTime;
    }
    else if (GetKey(GLFW_KEY_S, GameState))
    {
        GameState->Player.Position.y += GameState->Player.player.WalkingSpeed * DeltaTime;
    }

    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0), GameState->Camera.ViewMatrix, GameState->Camera.ProjectionMatrix, glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    auto direction = glm::vec2(pos.x, pos.y) - GameState->Player.Position;
    direction = glm::normalize(direction);
    float degrees = atan2(direction.y, direction.x);

    GameState->Player.Rotation = glm::vec3(0, 0, degrees);

    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom), static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom), 0.0f, -1.0f, 1.0f);
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-GameState->Player.Position.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2, -GameState->Player.Position.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2, 0));
}