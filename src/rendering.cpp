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
           
        }
        break;
        case Shader_StandardFont:
        {
            // auto TexcoordLocation = glGetAttribLocation(Shader->Program, "coord");
            // glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
            // glEnableVertexAttribArray(0);
        }
        break;
        default:
        {
            
        }
        break;
    }

    glUseProgram(Shader->Program);
}

static void InitializeFreeTypeFont(FT_Library Library, render_font* Font, shader* Shader)
{
    if(FT_New_Face(Library, "./assets/fonts/inconsolata/Inconsolata-Regular.ttf", 0, &Font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }

    FT_Set_Pixel_Sizes(Font->Face, 0, 30);

    //Find the atlas width and height
    FT_GlyphSlot G = Font->Face->glyph;

    unsigned int W = 0;
    unsigned int H = 0;

    for(int i = 32; i < 128; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }

        W += G->bitmap.width;
        H = std::max(H, G->bitmap.rows);
    }

    Font->AtlasWidth = W;
    Font->AtlasHeight = H;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &Font->Texture);
    glBindTexture(GL_TEXTURE_2D, Font->Texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, Font->AtlasWidth, Font->AtlasHeight, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    uint32 X = 0;

    for(int i = 32; i < 128; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, X, 0, G->bitmap.width, G->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE, G->bitmap.buffer);

        Font->CharacterInfo[i].AX = G->advance.x >> 6;
        Font->CharacterInfo[i].AY = G->advance.y >> 6;

        Font->CharacterInfo[i].BW = G->bitmap.width;
        Font->CharacterInfo[i].BH = G->bitmap.rows;

        Font->CharacterInfo[i].BL = G->bitmap_left;
        Font->CharacterInfo[i].BT = G->bitmap_top;

        Font->CharacterInfo[i].TX = (float)X / Font->AtlasWidth;

        X += G->bitmap.width;
    }

    glGenVertexArrays(1, &Font->VAO);
    glBindVertexArray(Font->VAO);

    glGenBuffers(1, &Font->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Font->VBO);
    auto TexcoordLocation = glGetAttribLocation(Shader->Program, "coord");
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void RenderSetup(render_state *RenderState)
{
    if(FT_Init_FreeType(&RenderState->FTLibrary)) 
    {
        fprintf(stderr, "Could not init freetype library\n");
    }

    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);

    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader("./assets/shaders/textureshader", &RenderState->TextureShader);

    auto PositionLocation = glGetAttribLocation(RenderState->TextureShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState->TextureShader.Program, "texcoord");

    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    //tile
    glGenVertexArrays(1, &RenderState->TileVAO);
    glBindVertexArray(RenderState->TileVAO);
    glGenBuffers(1, &RenderState->TileQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->TileQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TileShader.Type = Shader_Tile;
    LoadShader("./assets/shaders/tileshader", &RenderState->TileShader);

    auto PositionLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "pos");
    auto TexcoordLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "texcoord");

    glEnableVertexAttribArray(PositionLocation2);
    glVertexAttribPointer(PositionLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation2);
    glVertexAttribPointer(TexcoordLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);

    //console
    glGenVertexArrays(1, &RenderState->ConsoleVAO);
    glBindVertexArray(RenderState->ConsoleVAO);
    glGenBuffers(1, &RenderState->ConsoleQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->ConsoleShader.Type = Shader_Console;
    LoadShader("./assets/shaders/consoleshader", &RenderState->ConsoleShader);

    auto PositionLocation3 = glGetAttribLocation(RenderState->ConsoleShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    glBindVertexArray(0);

    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader("./assets/shaders/standardfontshader", &RenderState->StandardFontShader);

    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont(RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
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
    
    glDeleteShader(RenderState->StandardFontShader.VertexShader);
    glDeleteShader(RenderState->StandardFontShader.FragmentShader);
    glDeleteProgram(RenderState->StandardFontShader.Program);
    
    LoadShader("./assets/shaders/textureshader", &RenderState->TextureShader);
    LoadShader("./assets/shaders/tileshader", &RenderState->TileShader);
    LoadShader("./assets/shaders/consoleshader", &RenderState->ConsoleShader);
    LoadShader("./assets/shaders/standardfontshader", &RenderState->StandardFontShader);
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
        case Shader_StandardFont:
            glDeleteProgram(RenderState->StandardFontShader.Program);
            glDeleteShader(RenderState->StandardFontShader.VertexShader);
            LoadVertexShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader);
            break;
    }
}

static void ReloadFragmentShader(uint32 Index, render_state* RenderState)
{
    std::cout << "RELOAD SHADER\n" << std::endl;
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
        case Shader_StandardFont:
            glDeleteProgram(RenderState->StandardFontShader.Program);
            glDeleteShader(RenderState->StandardFontShader.FragmentShader);
            LoadFragmentShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader);
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

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const char *Text, float X, float Y, float SX, float SY) 
{
    auto Shader = RenderState->Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Attribute(Shader.Program, "color", Font.Color);

    if (RenderState->BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState->BoundTexture = Font.Texture;
    }

    struct point
    {
        GLfloat X;
        GLfloat Y;
        GLfloat S;
        GLfloat T;
    } Coords[6 * strlen(Text)];

    int N = 0;

    for(const char *P = Text; *P; P++) 
    { 
        float X2 =  X + Font.CharacterInfo[*P ].BL * SX;
        float Y2 = -Y - Font.CharacterInfo[*P ].BT * SY;
        float W = Font.CharacterInfo[*P].BW * SX;
        float H = Font.CharacterInfo[*P].BH * SY;

        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*P].AX * SX;
        Y += Font.CharacterInfo[*P].AY * SY;

        /* Skip glyphs that have no pixels */
        if(!W || !H)
            continue;
            
        Coords[N++] = (point){ X2, -Y2, Font.CharacterInfo[*P].TX, 0 };
        Coords[N++] = (point){ X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, 0 };
        Coords[N++] = (point){ X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = (point){ X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth,  0 };
        Coords[N++] = (point){ X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = (point){ X2 + W, -Y2 - H, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
    }

    glBindVertexArray(Font.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof Coords, Coords, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, N);
}

static void RenderConsole(render_state* RenderState, console* Console, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    auto Shader = RenderState->Shaders[Shader_Console];

    UseShader(&Shader);
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(-1, 0.5, 0));
    Model = glm::scale(Model, glm::vec3(2, 0.5, 1));
    SetMat4Uniform(Shader.Program, "M", Model);

    glBindVertexArray(RenderState->ConsoleVAO); //TODO(Daniel) Create a vertex array buffer + object for console
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->ConsoleQuadVBO);
    glDrawArrays(GL_QUADS, 0, 4);

    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    float SX = 2.0f / Mode->width;
    float SY = 2.0f / Mode->height;

    RenderText(RenderState, RenderState->InconsolataFont, ">", -1 + 8 * SX, 1.02 - 50 * SY, SX, SY);
    RenderText(RenderState, RenderState->InconsolataFont, &Console->Buffer[0], -0.98 + 8 * SX, 1.02 - 50 * SY, SX, SY);
}

static void RenderEntity(render_state *RenderState, const entity &entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{ 
    auto Shader = RenderState->Shaders[entity.ShaderIndex];
    UseShader(&Shader);

    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(entity.Position.x, entity.Position.y, 0.0f));
    Model = glm::translate(Model, glm::vec3(1, 1, 0.0f)); 
    Model = glm::rotate(Model, entity.Rotation.z + 1.56f, glm::vec3(0, 0, 1)); //NOTE(Daniel) 1.56 is approximately 90 degrees in radians
    Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f)); 
    Model = glm::scale(Model, entity.Scale);
    glm::mat4 MVP = ProjectionMatrix * View * Model;
    SetMat4Uniform(Shader.Program, "MVP", MVP);

    if (RenderState->BoundTexture != entity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, entity.TextureHandle);
        RenderState->BoundTexture = entity.TextureHandle;
    }

    glBindVertexArray(RenderState->SpriteVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);

    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void RenderTileChunk(render_state* RenderState, const tile_chunk &TileChunk, shader* Shader, GLuint TilesetTextureHandle, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    real32 Scale = 1.0f;

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

                SetVec2Attribute(Shader->Program, "textureOffset", TileChunk.Data[i][j].TextureOffset);
                SetMat4Uniform(Shader->Program, "MVP", MVP);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    glBindVertexArray(0);
}

static void RenderTilemap(render_state *RenderState, const tilemap_data &TilemapData, GLuint TilesetTextureHandle, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    if (RenderState->BoundTexture != TilesetTextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, TilesetTextureHandle);
        RenderState->BoundTexture = TilesetTextureHandle;
    }

    glBindVertexArray(RenderState->TileVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);

    auto Shader = RenderState->TileShader;
    UseShader(&Shader);

    for(int i = 0; i < TILEMAP_SIZE; i++)
    {
        for(int j = 0; j < TILEMAP_SIZE; j++)
        {
            RenderTileChunk(RenderState, TilemapData.Chunks[i][j], &Shader, TilesetTextureHandle, ProjectionMatrix, View, StartX, StartY, EndX, EndY);
        }
    }
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