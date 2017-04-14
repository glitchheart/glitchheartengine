#include "game.h"
#include "world.cpp"
#include "entity.cpp"
#include "keycontroller.cpp"

extern "C" UPDATE(Update)
{
    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(GameState->Camera.ViewportWidth), static_cast<GLfloat>(GameState->Camera.ViewportHeight), 0.0f, -1.0f, 1.0f);
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-GameState->Player.Position.x + GameState->Camera.ViewportWidth / 2, -GameState->Player.Position.y + GameState->Camera.ViewportHeight / 2, 0));

    //player movement
    if (IsKeyDown(GLFW_KEY_A,GameState))
    {
	GameState->Player.Position.x += -GameState->Player.player.WalkingSpeed * DeltaTime;
    }
    else if (IsKeyDown(GLFW_KEY_D,GameState))
	GameState->Player.Position.x += GameState->Player.player.WalkingSpeed * DeltaTime;

    if (IsKeyDown(GLFW_KEY_W,GameState))
    {
	GameState->Player.Position.y += -GameState->Player.player.WalkingSpeed * DeltaTime;
    }
    else if (IsKeyDown(GLFW_KEY_S,GameState))
	GameState->Player.Position.y += GameState->Player.player.WalkingSpeed * DeltaTime;

    //player rotation
    //NOTE(THIS IS FOR NIELS)

    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0), GameState->Camera.ViewMatrix, GameState->Camera.ProjectionMatrix, glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    auto direction = glm::vec2(pos.x, pos.y) - GameState->Player.Position;
    direction = glm::normalize(direction);
    float degrees = atan2(direction.y, direction.x);

    GameState->Player.Rotation = glm::vec3(0, 0, degrees);
}

BOOL WINAPI DllMain(
    _In_ HINSTANCE hinstDLL,
    _In_ DWORD fdwReason,
    _In_ LPVOID lpvReserved)
{
    return (TRUE);
}