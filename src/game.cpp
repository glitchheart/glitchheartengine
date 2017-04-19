#include "game.h"
#include "keycontroller.cpp"
#include "sound.cpp"

//CONSOLE STUFF TODO(Daniel) MOOOOOOOOOOOOOOOOOOVE
void ExecuteCommand(game_state *GameState)
{

    if (strcmp(" ", GameState->Console.Buffer) != 0) //NOTE(Daniel) if the command isn't an empty string
    {
        char *Result = &GameState->Console.Buffer[0];

        if (strcmp(GameState->Console.Buffer, "exit") == 0)
        {
            //TODO(niels): Need to find a way to call this from here
            //             This should probably be in platform code anyway?
            //             Doesn't really make sense to have it in game code
            CleanupSound(GameState);
            glfwDestroyWindow(GameState->RenderState.Window);
            glfwTerminate();
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(GameState->Console.Buffer, "build") == 0)
        {
            Result = "Building...";
            system("..\\build.bat"); //TODO(Daniel) separate thread
        }
        else if (strstr(GameState->Console.Buffer, "zoom") != NULL)
        {
            //NOTE(Daniel) copy the string before splitting it. The call to strtok manipulates it.
            char ResultCopy[40];
            strcpy(&ResultCopy[0], Result);

            char *Pointer;
            char *StrZoomAmount;

            Pointer = strtok(&ResultCopy[0], " "); //skip only spaces

            int Count = 0;

            while (Pointer != NULL && Count < 1)
            {
                if (Count == 0)
                    StrZoomAmount = strtok(NULL, " ");
                else
                    strtok(NULL, " ");
                Count++;
            }
            real32 ZoomAmount = (real32)strtod(StrZoomAmount, NULL);
            GameState->Camera.Zoom = ZoomAmount;

            Result = CombineStrings("Zoom set to ", StrZoomAmount);
        }
        else
        {
            Result = CombineStrings(Result, ": Command not found");
        }

        //NOTE(Daniel) Copy the command into the history buffer
        for (int i = HISTORY_BUFFER_LINES - 1; i > 0; i--)
        {
            sprintf(GameState->Console.HistoryBuffer[i], GameState->Console.HistoryBuffer[i - 1]);
        }

        sprintf(GameState->Console.HistoryBuffer[0], Result);

        for (int i = 0; i <= GameState->Console.BufferIndex; i++)
            GameState->Console.Buffer[i] = '\0';

        GameState->Console.BufferIndex = 0;
    }    
}

extern "C" UPDATE(Update)
{
    glfwGetFramebufferSize(GameState->RenderState.Window, &GameState->RenderState.WindowWidth, &GameState->RenderState.WindowHeight);

    if (GetKeyDown(GLFW_KEY_TAB, GameState))
    {
        GameState->Console.Open = !GameState->Console.Open;
    }

    if (GetKeyDown(GLFW_KEY_BACKSPACE, GameState) && GameState->Console.Open)
    {
        if (GameState->Console.BufferIndex > 0)
            GameState->Console.Buffer[--GameState->Console.BufferIndex] = '\0';
    }

    if (GetKeyDown(GLFW_KEY_ENTER, GameState))
    {
        if (GameState->Console.Open)
        {
            ExecuteCommand(GameState);
        }
        else
        {
            PlaySoundEffectOnce(GameState, &GameState->SoundManager.Track01);
        }
    }

    if (!GameState->Console.Open)
    {
        //player movement
        if (GetKey(GLFW_KEY_A, GameState))
        {
            GameState->Player.Position.x += -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(GLFW_KEY_D, GameState))
        {
            GameState->Player.Position.x += GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }

        if (GetKey(GLFW_KEY_W, GameState))
        {
            GameState->Player.Position.y += -GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
        else if (GetKey(GLFW_KEY_S, GameState))
        {
            GameState->Player.Position.y += GameState->Player.player.WalkingSpeed * (real32)DeltaTime;
        }
    }

    auto pos = glm::unProject(glm::vec3(GameState->InputController.MouseX, GameState->RenderState.Viewport[3] - GameState->InputController.MouseY, 0), GameState->Camera.ViewMatrix, GameState->Camera.ProjectionMatrix, glm::vec4(0, 0, GameState->RenderState.Viewport[2], GameState->RenderState.Viewport[3]));
    auto direction = glm::vec2(pos.x, pos.y) - GameState->Player.Position;
    direction = glm::normalize(direction);
    float degrees = atan2(direction.y, direction.x);

    GameState->Player.Rotation = glm::vec3(0, 0, degrees);

    GameState->Camera.ProjectionMatrix = glm::ortho(0.0f, static_cast<GLfloat>(GameState->Camera.ViewportWidth / GameState->Camera.Zoom), static_cast<GLfloat>(GameState->Camera.ViewportHeight / GameState->Camera.Zoom), 0.0f, -1.0f, 1.0f);
    GameState->Camera.ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(-GameState->Player.Position.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2, -GameState->Player.Position.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2, 0));
}
