static GLint ShaderCompilationErrorChecking(GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> ErrorLog(MaxLength);
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, &ErrorLog[0]);

        std::cout << "SHADER Compilation error" << std::endl;

        for (std::vector<GLchar>::const_iterator i = ErrorLog.begin(); i != ErrorLog.end(); ++i)
            std::cout << *i;

        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const std::string FilePath, shader *Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);

    if (!ShaderCompilationErrorChecking(Shd->VertexShader))
        return GL_FALSE;

    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);

    if (!ShaderCompilationErrorChecking(Shd->FragmentShader))
        return GL_FALSE;

    Shd->Program = glCreateProgram();

    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
   
    return GL_TRUE;
}

static GLuint LoadVertexShader(const std::string FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();

    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLchar *VertexText = LoadShaderFromFile(FilePath + std::string(".vert"));
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);

    if (!ShaderCompilationErrorChecking(Shd->VertexShader))
        return GL_FALSE;

    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);

    glLinkProgram(Shd->Program);
   
    return GL_TRUE;
}

static GLuint LoadFragmentShader(const std::string FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLchar *FragmentText = LoadShaderFromFile(FilePath + std::string(".frag"));
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);

    if (!ShaderCompilationErrorChecking(Shd->FragmentShader))
        return GL_FALSE;
   
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
   
    return GL_TRUE;
}

static void UseShader(shader *Shader)
{
    switch(Shader->Type)
    {
        case Shader_Console:
        {
            auto PositionLocation = glGetAttribLocation(Shader->Program, "pos");
            glEnableVertexAttribArray(PositionLocation);
            glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        }
            break;
        default:
        {
            auto PositionLocation = glGetAttribLocation(Shader->Program, "pos");
            auto TexcoordLocation = glGetAttribLocation(Shader->Program, "texcoord");

            glEnableVertexAttribArray(PositionLocation);
            glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

            glEnableVertexAttribArray(TexcoordLocation);
            glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
        }
            break;
    }

    glUseProgram(Shader->Program);
}

static void RenderSetup(render_state *RenderState)
{
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    glGenVertexArrays(1, &RenderState->TileVAO);
    glBindVertexArray(RenderState->TileVAO);
    glGenBuffers(1, &RenderState->TileQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->TileQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    glGenVertexArrays(1, &RenderState->ConsoleVAO);
    glBindVertexArray(RenderState->ConsoleVAO);
    glGenBuffers(1, &RenderState->ConsoleQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_DYNAMIC_DRAW);
    glBindVertexArray(0);

    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader("./assets/shaders/textureshader", &RenderState->TextureShader);

    RenderState->TileShader.Type = Shader_Tile;
    LoadShader("./assets/shaders/tileshader", &RenderState->TileShader);

    RenderState->ConsoleShader.Type = Shader_Console;
    LoadShader("./assets/shaders/consoleshader", &RenderState->ConsoleShader);
}

static void ReloadShaders(render_state* RenderState)
{
    glDeleteShader(RenderState->TextureShader.VertexShader);
    glDeleteShader(RenderState->TextureShader.FragmentShader);
    glDeleteProgram(RenderState->TextureShader.Program);

    glDeleteShader(RenderState->TileShader.VertexShader);
    glDeleteShader(RenderState->TileShader.FragmentShader);
    glDeleteProgram(RenderState->TileShader.Program);
    
    glDeleteShader(RenderState->ConsoleShader.VertexShader);
    glDeleteShader(RenderState->ConsoleShader.FragmentShader);
    glDeleteProgram(RenderState->ConsoleShader.Program);
    
    LoadShader("./assets/shaders/textureshader", &RenderState->TextureShader);
    LoadShader("./assets/shaders/tileshader", &RenderState->TileShader);
    LoadShader("./assets/shaders/consoleshader", &RenderState->ConsoleShader);
}

static void ReloadVertexShader(uint32 Index, render_state* RenderState)
{
    switch(Index)
    {
        case Shader_Texture:
            glDeleteProgram(RenderState->TextureShader.Program);
            glDeleteShader(RenderState->TextureShader.VertexShader);
            LoadVertexShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader);
            break;
        case Shader_Tile:
            glDeleteProgram(RenderState->TileShader.Program);
            glDeleteShader(RenderState->TileShader.VertexShader);
            LoadVertexShader(ShaderPaths[Shader_Tile], &RenderState->TileShader);
            break;
        case Shader_Console:
            glDeleteProgram(RenderState->ConsoleShader.Program);
            glDeleteShader(RenderState->ConsoleShader.VertexShader);
            LoadVertexShader(ShaderPaths[Shader_Console], &RenderState->ConsoleShader);
            break;
    }
}

static void ReloadFragmentShader(uint32 Index, render_state* RenderState)
{
    std::cout << "RELOAD SHADER" << std::endl;
    switch(Index)
    {
        case Shader_Texture:
            glDeleteProgram(RenderState->TextureShader.Program);
            glDeleteShader(RenderState->TextureShader.FragmentShader);
            LoadFragmentShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader);
            break;
        case Shader_Tile:
            glDeleteProgram(RenderState->TileShader.Program);
            glDeleteShader(RenderState->TileShader.FragmentShader);
            LoadFragmentShader(ShaderPaths[Shader_Tile], &RenderState->TileShader);
            break;
        case Shader_Console:
            glDeleteProgram(RenderState->ConsoleShader.Program);
            glDeleteShader(RenderState->ConsoleShader.FragmentShader);
            LoadFragmentShader(ShaderPaths[Shader_Console], &RenderState->ConsoleShader);
            break;
    }
}

static GLuint LoadTexture(const char *FilePath)
{
    GLuint TextureHandle;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &TextureHandle);
    glBindTexture(GL_TEXTURE_2D, TextureHandle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //enable alpha for textures
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    int Width, Height;
    unsigned char *Image = SOIL_load_image(FilePath, &Width, &Height, 0, SOIL_LOAD_RGBA);

    if (!Image)
        return GL_FALSE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, Image);

    SOIL_free_image_data(Image);

    return TextureHandle;
}

static void ReloadAssets(asset_manager *AssetManager, game_state* GameState)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(AssetManager->DirtyVertexShaderIndices[i] == 1)
        {
            ReloadVertexShader(i, &GameState->RenderState);
            AssetManager->DirtyVertexShaderIndices[i] = 0;
        }

        if(AssetManager->DirtyFragmentShaderIndices[i] == 1)
        {
            ReloadFragmentShader(i, &GameState->RenderState);
            AssetManager->DirtyFragmentShaderIndices[i] = 0;
        }
    }

    if(AssetManager->DirtyTileset == 1)
    {
       GameState->TilemapData.TileAtlasTexture = LoadTexture("./assets/textures/tiles.png");
       AssetManager->DirtyTileset = 0;
    }
}

static void SetVec2Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec2 Value)
{
    glUniform2f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y);
}

static void SetVec3Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec3 Value)
{
    glUniform3f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z);
}

static void SetVec4Attribute(GLuint ShaderHandle, const char *UniformName, glm::vec4 Value)
{
    glUniform4f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z, Value.w);
}

static void SetMat4Uniform(GLuint ShaderHandle, const char *UniformName, glm::mat4 Value)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_FALSE, &Value[0][0]);
}

static GLuint BoundVAO;

static void RenderConsole(render_state* RenderState, console* Console, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->ConsoleVAO); //TODO(Daniel) Create a vertex array buffer + object for console
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);

    glm::mat4 Model(1.0f);

    Model = glm::translate(Model, glm::vec3(-1, 0.5, 0));
    Model = glm::scale(Model, glm::vec3(2, 0.5, 1));

    auto Shader = RenderState->Shaders[Shader_Console];

    UseShader(&Shader);

    SetMat4Uniform(Shader.Program, "M", Model);
    glDrawArrays(GL_QUADS, 0, 4);

    glBindVertexArray(0);
}

static void RenderEntity(render_state *RenderState, const entity &entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->SpriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    
    if (RenderState->BoundTexture != entity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, entity.TextureHandle);
        RenderState->BoundTexture = entity.TextureHandle;
    }

    glm::mat4 Model(1.0f);

    Model = glm::translate(Model, glm::vec3(entity.Position.x, entity.Position.y, 0.0f));
    
    Model = glm::translate(Model, glm::vec3(1, 1, 0.0f)); 
    Model = glm::rotate(Model, entity.Rotation.z + 1.56f, glm::vec3(0, 0, 1)); //NOTE(Daniel) 1.56 is approximately 90 degrees in radians
    Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f)); 

    Model = glm::scale(Model, entity.Scale);

    auto Shader = RenderState->Shaders[entity.ShaderIndex];

    glm::mat4 MVP = ProjectionMatrix * View * Model;

    UseShader(&Shader);

    SetMat4Uniform(Shader.Program, "MVP", MVP);
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderTileChunk(render_state* RenderState, const tile_chunk &TileChunk,  GLuint TilesetTextureHandle, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    glBindVertexArray(RenderState->SpriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);

    real32 Scale = 1.0f;
    
    auto Shader = RenderState->TileShader;

    UseShader(&Shader);

    // for (int i = StartX; i < EndX; i++) //TODO(Daniel) this has to be added again
    // {
    //     for (int j = StartY; j < EndY; j++)
    //     {
    for (int i = 0; i < CHUNK_SIZE; i++)
    {
        for (int j = 0; j < CHUNK_SIZE; j++)
        {
            if(TileChunk.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(TileChunk.X * CHUNK_SIZE + i * Scale, TileChunk.Y * CHUNK_SIZE + j * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                glm::mat4 MVP = ProjectionMatrix * View * Model;

                SetVec2Attribute(Shader.Program, "textureOffset", TileChunk.Data[i][j].TextureOffset);
                SetMat4Uniform(Shader.Program, "MVP", MVP);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    glBindVertexArray(0);
}

static void RenderTilemap(render_state *RenderState, const tilemap_data &TilemapData, GLuint TilesetTextureHandle, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    glBindVertexArray(RenderState->TileVAO);

    if (RenderState->BoundTexture != TilesetTextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, TilesetTextureHandle);
        RenderState->BoundTexture = TilesetTextureHandle;
    }

    for(int i = 0; i < TILEMAP_SIZE; i++)
    {
        for(int j = 0; j < TILEMAP_SIZE; j++)
        {
            RenderTileChunk(RenderState, TilemapData.Chunks[i][j], TilesetTextureHandle, ProjectionMatrix, View, StartX, StartY, EndX, EndY);
        }
    }
    
    // for (int i = StartX; i < EndX; i++)
    // {
    //     for (int j = StartY; j < EndY; j++)
    //     {
    //         if(TilemapData.Data[i][j].Type != Tile_None)
    //         {
    //             glm::mat4 Model(1.0f);
    //             Model = glm::translate(Model, glm::vec3(i * scale, j * scale, 0.0f));
    //             Model = glm::scale(Model, glm::vec3(scale, scale, 1.0f));
    //             glm::mat4 MVP = ProjectionMatrix * View * Model;

    //             SetVec2Attribute(Shader.Program, "textureOffset", TilemapData.Data[i][j].TextureOffset);
    //             SetMat4Uniform(Shader.Program, "MVP", MVP);
    //             glDrawArrays(GL_QUADS, 0, 4);
    //         }
    //     }
    // }
    glBindVertexArray(0);
}

static void Render(game_state* GameState)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(99.0f / 255.0f, 155.0f / 255.0f, 255.0f / 255.0f, 1.0f);

    //find the visible chunks
    int minX = (int)std::max(0.0f, GameState->Player.Position.x - GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2);
    int minY = (int)std::max(0.0f, GameState->Player.Position.y - GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2);

    int maxX = (int)std::min((real32)TILEMAP_SIZE * CHUNK_SIZE, GameState->Player.Position.x + GameState->Camera.ViewportWidth / GameState->Camera.Zoom / 2.0f + 2);
    int maxY = (int)std::min((real32)TILEMAP_SIZE * CHUNK_SIZE, GameState->Player.Position.y + GameState->Camera.ViewportHeight / GameState->Camera.Zoom / 2.0f + 2);
    
    RenderTilemap(&GameState->RenderState, GameState->TilemapData, GameState->TilemapData.TileAtlasTexture, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix, minX, minY, maxX, maxY);

    RenderEntity(&GameState->RenderState, GameState->Player, GameState->Camera.ProjectionMatrix,  GameState->Camera.ViewMatrix);
    
    if(GameState->Console.Open)
        RenderConsole(&GameState->RenderState, &GameState->Console, GameState->Camera.ProjectionMatrix,  GameState->Camera.ViewMatrix);

    glfwSwapBuffers(GameState->RenderState.Window);
}