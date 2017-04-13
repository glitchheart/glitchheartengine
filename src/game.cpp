#include "game.h"
#include "entity.h"
#include <GLFW/glfw3.h>
#include "keycontroller.h"

static void Update(double DeltaTime, game_state* GameState)
{
	GameState->Camera.ProjectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(GameState->Camera.ViewportWidth), static_cast<GLfloat>(GameState->Camera.ViewportHeight), 0.0f, -1.0f, 1.0f);
	GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-GameState->Player.Position.x + GameState->Camera.ViewportWidth / 2, -GameState->Player.Position.y + GameState->Camera.ViewportHeight / 2, 0));  

	//player movement
	if(IsKeyDown(GLFW_KEY_A))
		GameState->Player.Position.x += -GameState->Player.player.WalkingSpeed * DeltaTime;
	else if(IsKeyDown(GLFW_KEY_D))
		GameState->Player.Position.x += GameState->Player.player.WalkingSpeed * DeltaTime;

	if(IsKeyDown(GLFW_KEY_W))
		GameState->Player.Position.y += -GameState->Player.player.WalkingSpeed * DeltaTime;
	else if(IsKeyDown(GLFW_KEY_S))
		GameState->Player.Position.y += GameState->Player.player.WalkingSpeed * DeltaTime;
	
	//player rotation
	//NOTE(THIS IS FOR NIELS)

	GLint Viewport[4];
	glGetIntegerv(GL_VIEWPORT, Viewport);
	auto pos = glm::unProject(glm::vec3(MouseX, Viewport[3] - MouseY, 0), GameState->Camera.ViewMatrix, GameState->Camera.ProjectionMatrix, glm::vec4(0, 0, Viewport[2], Viewport[3]));

	auto direction = glm::vec2(pos.x, pos.y) - GameState->Player.Position;
	direction = glm::normalize(direction);
	float degrees = atan2(direction.y, direction.x);

	GameState->Player.Rotation = glm::vec3(0, 0, degrees);
}