static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    glfwSetWindowAspectRatio(Window, 16, 9);
    glViewport(0, 0, Width, Height);
}

inline static real64 GetTime()
{
    return glfwGetTime();
}

inline static void PollEvents()
{
    glfwPollEvents();
}

static void CloseWindow(game_state* GameState)
{
    glfwDestroyWindow(GameState->RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static bool32 ShouldCloseWindow(render_state* RenderState)
{
    return glfwWindowShouldClose(RenderState->Window); 
}

static GLint ShaderCompilationErrorChecking(const char* ShaderName, GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        GLchar* ErrorLog = (GLchar*)malloc(MaxLength);
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, ErrorLog);
        
        printf("SHADER Compilation error - %s\n", ShaderName);
        
        for(uint32 ErrorIndex = 0; ErrorIndex < MaxLength; ErrorIndex++)
        {
            printf("%d\n", ErrorLog[ErrorIndex]);
        }
        
        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const char* FilePath, shader *Shd)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString);
    
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag");
    GLchar *FragmentText = LoadShaderFromFile(FragmentString);
    
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    Shd->Program = glCreateProgram();
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadVertexShader(const char* FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString);
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadFragmentShader(const char* FilePath, shader *Shd)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag");
    GLchar *FragmentText = LoadShaderFromFile(FragmentString);
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static void UseShader(shader *Shader)
{
    glUseProgram(Shader->Program);
}

static void InitializeFreeTypeFont(char* FontPath, int FontSize, FT_Library Library, render_font* Font, shader* Shader)
{
    if(FT_New_Face(Library, FontPath, 0, &Font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }
    
    FT_Set_Pixel_Sizes(Font->Face, 0, FontSize);
    
    FT_Select_Charmap(Font->Face , ft_encoding_unicode);
    
    //Find the atlas width and height
    FT_GlyphSlot G = Font->Face->glyph;
    
    unsigned int W = 0;
    unsigned int H = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        
        W += G->bitmap.width;
        H = Max(H, G->bitmap.rows);
    }
    
    Font->AtlasWidth = W;
    Font->AtlasHeight = H;
    Font->GlyphWidth = (GLfloat)G->bitmap.width;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &Font->Texture);
    glBindTexture(GL_TEXTURE_2D, Font->Texture);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, Font->AtlasWidth, Font->AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    unsigned int X = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, X, 0, G->bitmap.width, G->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
        Font->CharacterInfo[i].AX = (real32)(G->advance.x >> 6);
        Font->CharacterInfo[i].AY = (real32)(G->advance.y >> 6);
        
        Font->CharacterInfo[i].BW = (real32)G->bitmap.width;
        Font->CharacterInfo[i].BH = (real32)G->bitmap.rows;
        
        Font->CharacterInfo[i].BL = (real32)G->bitmap_left;
        Font->CharacterInfo[i].BT = (real32)G->bitmap_top;
        
        Font->CharacterInfo[i].TX = (real32)X / Font->AtlasWidth;
        
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
    LoadShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader);
    
    auto PositionLocation = glGetAttribLocation(RenderState->TextureShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState->TextureShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //Animation
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteSheetVAO);
    glBindVertexArray(RenderState->SpriteSheetVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TextureShader.Type = Shader_SpriteSheetShader;
    LoadShader(ShaderPaths[Shader_SpriteSheetShader], &RenderState->SpriteSheetShader);
    
    PositionLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->SpriteSheetShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //error sprite
    glGenVertexArrays(1, &RenderState->SpriteErrorVAO);
    glBindVertexArray(RenderState->SpriteErrorVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->ErrorShaderSprite.Type = Shader_ErrorSprite;
    LoadShader(ShaderPaths[Shader_ErrorSprite], &RenderState->ErrorShaderSprite);
    
    PositionLocation = glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    
    //ui sprite
    glGenVertexArrays(1, &RenderState->UISpriteVAO);
    glBindVertexArray(RenderState->UISpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->UISpriteShader.Type = Shader_UISprite;
    LoadShader(ShaderPaths[Shader_UISprite], &RenderState->UISpriteShader);
    
    PositionLocation = glGetAttribLocation(RenderState->UISpriteShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->UISpriteShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //ui error shader
    glGenVertexArrays(1, &RenderState->UIErrorVAO);
    glBindVertexArray(RenderState->UIErrorVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_DYNAMIC_DRAW);
    
    //error shader
    RenderState->ErrorShaderUI.Type = Shader_ErrorUI;
    LoadShader(ShaderPaths[Shader_ErrorUI], &RenderState->ErrorShaderUI);
    
    PositionLocation = glGetAttribLocation(RenderState->ErrorShaderUI.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->ErrorShaderUI.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->TileShader.Type = Shader_Tile;
    LoadShader(ShaderPaths[Shader_Tile], &RenderState->TileShader);
    
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
    glGenBuffers(1, &RenderState->NormalQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->NormalQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->NormalQuadVerticesSize, RenderState->NormalQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->ConsoleShader.Type = Shader_Console;
    LoadShader(ShaderPaths[Shader_Console], &RenderState->ConsoleShader);
    
    auto PositionLocation3 = glGetAttribLocation(RenderState->ConsoleShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //wireframe
    glGenVertexArrays(1, &RenderState->WireframeVAO);
    glBindVertexArray(RenderState->WireframeVAO);
    glGenBuffers(1, &RenderState->WireframeQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->WireframeQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->WireframeQuadVerticesSize, RenderState->WireframeQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->ConsoleShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader);
    
    PositionLocation3 = glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //astar tile
    glGenVertexArrays(1, &RenderState->AStarPathVAO);
    glBindVertexArray(RenderState->AStarPathVAO);
    glGenBuffers(1, &RenderState->AStarPathQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->AStarPathQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->AStarPathQuadVerticesSize,
                 RenderState->AStarPathQuadVertices,GL_DYNAMIC_DRAW);
    
    LoadShader(ShaderPaths[Shader_AStarPath], &RenderState->AStarPathShader);
    
    PositionLocation3 = glGetAttribLocation(RenderState->AStarPathShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    
    glBindVertexArray(0);
    
    
    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader);
    
    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 18, RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
    
    RenderState->MenuFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 40, RenderState->FTLibrary, &RenderState->MenuFont, &RenderState->StandardFontShader);
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
    unsigned char* Image = stbi_load(FilePath, &Width, &Height, 0, STBI_rgb_alpha);
    
    if (!Image)
        return GL_FALSE;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Width, Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, Image);
    
    stbi_image_free(Image);
    
    return TextureHandle;
}

static void LoadTextures(render_state* RenderState)
{
    for(uint32 TextureIndex = 0; TextureIndex < Texture_Count; TextureIndex++)
    {
        RenderState->Textures[TextureIndex] = LoadTexture(TexturePaths[TextureIndex]);
    }
}

static void InitializeOpenGL(game_state* GameState, render_state* RenderState, config_data* ConfigData)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    RenderState->Window = glfwCreateWindow(ConfigData->ScreenWidth, ConfigData->ScreenHeight, Concat(Concat(ConfigData->Title, " "), ConfigData->Version), ConfigData->Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);
    
    if (!RenderState->Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    //center window on screen
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int Width, Height;
    
    glfwGetFramebufferSize(RenderState->Window, &Width, &Height);
    glfwSetWindowPos(RenderState->Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(RenderState->Window, FramebufferSizeCallback);
    
    glfwMakeContextCurrent(RenderState->Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(RenderState->Window, &RenderState->WindowWidth, &RenderState->WindowHeight);
    glViewport(0, 0, RenderState->WindowWidth, RenderState->WindowHeight);
    glDisable(GL_DITHER);
    printf("%s\n", glGetString(GL_VERSION));
    
    glfwSetWindowUserPointer(RenderState->Window, GameState);
    glfwSetKeyCallback(RenderState->Window, KeyCallback);
    glfwSetCharCallback(RenderState->Window, CharacterCallback);
    glfwSetCursorPosCallback(RenderState->Window, CursorPositionCallback);
    glfwSetMouseButtonCallback(RenderState->Window, MouseButtonCallback);
    glfwSetScrollCallback(RenderState->Window, ScrollCallback);
    //glfwSetInputMode(RenderState->Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    memcpy(RenderState->Viewport, Viewport, sizeof(GLint) * 4);
    
    int Present = glfwJoystickPresent(GLFW_JOYSTICK_1);
    if(Present)
    {
        GameState->InputController.ControllerPresent = true;
        const char* Name = glfwGetJoystickName(GLFW_JOYSTICK_1);
        
        if(strstr(Name, "Xbox") != 0)
        {
            GameState->InputController.ControllerType = Controller_Xbox;
        }
        else if(strstr(Name, "PS4") != 0 || strstr(Name, "Wireless") != 0)
        {
            GameState->InputController.ControllerType = Controller_PS4;
        }
        
        printf("type %d\n", GameState->InputController.ControllerType);
    }
    LoadTextures(RenderState);
    RenderSetup(RenderState);
    
    GameState->HealthBar = {};
    GameState->HealthBar.Position = glm::vec2(RenderState->WindowWidth / 2, RenderState->WindowHeight - 50);
    GameState->HealthBar.RenderInfo.Size = glm::vec3(2, 1, 1);
    GameState->HealthBar.RenderInfo.TextureHandle = RenderState->Textures[Texture_HealthFull];
}

static void ReloadVertexShader(Shader_Type Type, render_state* RenderState)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].VertexShader);
    LoadVertexShader(ShaderPaths[Type], &RenderState->Shaders[Type]);
}

static void ReloadFragmentShader(Shader_Type Type, render_state* RenderState)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].FragmentShader);
    LoadFragmentShader(ShaderPaths[Type], &RenderState->Shaders[Type]);
}

static void ReloadAssets(asset_manager *AssetManager, game_state* GameState)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(AssetManager->DirtyVertexShaderIndices[i] == 1)
        {
            printf("Reloading vertex shader type: %d\n", i);
            ReloadVertexShader((Shader_Type)i, &GameState->RenderState);
            AssetManager->DirtyVertexShaderIndices[i] = 0;
        }
        
        if(AssetManager->DirtyFragmentShaderIndices[i] == 1)
        {
            printf("Reloading fragment shader type: %d\n", i);
            ReloadFragmentShader((Shader_Type)i, &GameState->RenderState);
            AssetManager->DirtyFragmentShaderIndices[i] = 0;
        }
    }
    
    if(AssetManager->DirtyTileset == 1)
    {
        GameState->CurrentLevel.Tilemap.RenderEntity.TextureHandle = LoadTexture("../assets/textures/tiles.png");
        AssetManager->DirtyTileset = 0;
    }
}

static void SetFloatAttribute(GLuint ShaderHandle, const char* UniformName, real32 Value)
{
    glUniform1f(glGetUniformLocation(ShaderHandle, UniformName), Value);
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

//TODO(Daniel) there's a weird bug when rendering special characters. The cursor just slowly jumps up for every character pressed
static void RenderRect(Render_Mode Mode, render_state* RenderState, glm::vec4 Color, real32 X, real32 Y, real32 Width, real32 Height)
{
    X *= RenderState->ScaleX;
    X -= 1;
    Y *= RenderState->ScaleY;
    Y -= 1;
    Width *= RenderState->ScaleX;
    Height *= RenderState->ScaleY;
    
    switch(Mode)
    {
        case Render_Fill:
        {
            glBindVertexArray(RenderState->ConsoleVAO); //TODO(Daniel) Create a vertex array buffer + object for simple rect / just change name
            
            auto Shader = RenderState->ConsoleShader;
            UseShader(&Shader);
            
            //draw upper part
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Attribute(Shader.Program, "color", Color);
            
            glDrawArrays(GL_QUADS, 0, 4);
        }
        break;
        case Render_Outline:
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(X, Y, 0));
            Model = glm::scale(Model, glm::vec3(Width, Height, 1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->Shaders[Shader_Wireframe];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "MVP", Model);
            
            SetVec4Attribute(Shader.Program, "color", glm::vec4(1, 1, 1, 1));
            
            glDrawArrays(GL_LINE_STRIP, 0, 6);
            glBindVertexArray(0);
        }
        break;
    }
}

static void MeasureText(const render_font* Font, const char* Text, float* Width, float* Height)
{
    int Count, C;
    
    if (!Text) 
    {
        return;
    }
    
    if (Width) 
    {
        /* Width of a text rectangle is a sum advances for every glyph in a string */
        *Width = 0.0f;
        
        for(Count = 0; Count < strlen(Text); ++Count) 
        {
            C= Text[Count];
            *Width += Font->CharacterInfo[C].AX;
        }
    }
    
    if (Height)
    {
        *Height = 0.0f;
        
        for(Count = 0; Count < strlen(Text); ++Count) 
        {
            C = Text[Count];
            
            if (*Height < Font->CharacterInfo[C].AY) 
            {
                *Height = Font->CharacterInfo[C].AY;
            }
        }
    }
}

//rendering methods
static void RenderText(render_state* RenderState, const render_font& Font, const glm::vec4& Color, const char* Text, real32 X, real32 Y, real32 Scale, Alignment Alignment = Alignment_Left) 
{
    X *= RenderState->ScaleX;
    X -= 1;
    Y *= RenderState->ScaleY;
    Y -= 1;
    
    glBindVertexArray(Font.VAO);
    auto Shader = RenderState->Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Attribute(Shader.Program, "color", Color);
    SetVec4Attribute(Shader.Program, "alphaColor", Font.AlphaColor);
    
    if (RenderState->BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState->BoundTexture = Font.Texture;
    }
    
    point* Coords = (point*)malloc(sizeof(point) * 6 * strlen(Text));
    
    int N = 0;
    
    switch(Alignment)
    {
        case Alignment_Left:
        break;
        case Alignment_Right:
        break;
        case Alignment_Center:
        {
            real32 Width;
            real32 Height;
            MeasureText(&Font, Text, &Width, &Height);
            X -= RenderState->ScaleX * Width / 2;
        }
        break;
    }
    
    for(const char *P = Text; *P; P++) 
    { 
        real32 W = Font.CharacterInfo[*P].BW * RenderState->ScaleX;
        real32 H = Font.CharacterInfo[*P].BH * RenderState->ScaleY;
        
        real32 X2 =  X + Font.CharacterInfo[*P ].BL * RenderState->ScaleX;
        real32 Y2 = -Y - Font.CharacterInfo[*P ].BT * RenderState->ScaleY;
        
        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*P].AX * RenderState->ScaleX;
        Y += Font.CharacterInfo[*P].AY * RenderState->ScaleY;
        
        /* Skip glyphs that have no pixels */
        if(!W || !H)
            continue;
        
        Coords[N++] = { X2, -Y2, Font.CharacterInfo[*P].TX, 0 };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, 0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth,  0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2 - H, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, 6 * strlen(Text) * sizeof(point), Coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, N);
    free(Coords);
    glBindVertexArray(0);
}

static void RenderConsole(render_state* RenderState, console* Console)
{
    glBindVertexArray(RenderState->ConsoleVAO);
    
    real32 PercentAnimated = 1.0f + 1.0f - (real32)Console->CurrentTime / (real32)Console->TimeToAnimate;
    
    //draw upper part
    RenderRect(Render_Fill, RenderState, glm::vec4(0.0f, 0.4f, 0.3f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.23f);
    
    //draw lower bar
    RenderRect(Render_Fill, RenderState, glm::vec4(0.0f, 0.2f, 0.2f, 0.6f), 0.0f, (real32)RenderState->WindowHeight * 0.77f * PercentAnimated, (real32)RenderState->WindowWidth, 20);  
    
    GLfloat TimeValue = (real32)glfwGetTime();
    GLfloat AlphaValue = (real32)((sin(TimeValue * 4) / 2) + 0.5f);
    real32 Width;
    real32 Height;
    MeasureText(&RenderState->InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderRect(Render_Fill, RenderState, glm::vec4(AlphaValue, 1, AlphaValue, 1), 5 / 1920.0f * (real32)RenderState->WindowWidth + Width, RenderState->WindowHeight * 0.77f * PercentAnimated, 10, 20);
    
    RenderText(RenderState, RenderState->InconsolataFont, glm::vec4(0, 0.8, 0, 1),  &Console->Buffer[0], 5 / 1920.0f * (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.775f * PercentAnimated, 1);
    
    int index = 0;
    
    glm::vec4 Color;
    
    for(int Index = 0; Index < HISTORY_BUFFER_LINES; Index++)
    {
        if(Index % 2 != 0)
            Color = glm::vec4(0, 1, 0, 1);
        else
            Color = glm::vec4(1, 1, 1, 1);
        
        RenderText(RenderState, RenderState->InconsolataFont, Color, &Console->HistoryBuffer[Index][0], 5 / 1920.0f * (real32)RenderState->WindowWidth, (real32)RenderState->WindowHeight * 0.78f * PercentAnimated + (Index + 1) * 20 * PercentAnimated, 1);
    }
}

static void RenderColliderWireframe(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Active)
    {
        glm::mat4 Model(1.0f);
        
        Model = glm::translate(Model, glm::vec3(Entity->CollisionAABB.Center.x - Entity->CollisionAABB.Extents.x, Entity->CollisionAABB.Center.y - Entity->CollisionAABB.Extents.y, 0.0f));
        Model = glm::scale(Model, glm::vec3(Entity->CollisionAABB.Extents.x * 2, Entity->CollisionAABB.Extents.y * 2,1));
        
        glBindVertexArray(RenderState->WireframeVAO);
        
        auto Shader = RenderState->Shaders[Shader_Wireframe];
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        glm::vec4 color;
        
        if(Entity->IsColliding)
        {
            color = glm::vec4(1.0,0.0,0.0,1.0);
        }
        else 
        {
            color = glm::vec4(0.0,1.0,0.0,1.0);
        }
        
        SetVec4Attribute(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
        
        if(Entity->HitTrigger)
        {
            glm::mat4 Model(1.0f);
            
            Model = glm::translate(Model, glm::vec3(Entity->Position.x + Entity->Center.x * Entity->Scale.x - Entity->HitTrigger->Extents.x, Entity->Position.y + Entity->Center.y * Entity->Scale.y - Entity->HitTrigger->Extents.y, 0.0f));
            Model = glm::scale(Model, glm::vec3(Entity->HitTrigger->Extents.x * 2, Entity->HitTrigger->Extents.y * 2,1));
            
            glBindVertexArray(RenderState->WireframeVAO);
            
            auto Shader = RenderState->Shaders[Shader_Wireframe];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Entity->HitTrigger->IsColliding)
            {
                color = glm::vec4(1.0,0.0,0.0,1.0);
            }
            else 
            {
                color = glm::vec4(0.0,1.0,0.0,1.0);
            }
            
            SetVec4Attribute(Shader.Program, "color", color);
            
            glDrawArrays(GL_LINE_STRIP, 0, 6);
            glBindVertexArray(0);
        }
    }
}


static void RenderWireframe(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    if(Entity->Active)
    {
        glm::mat4 Model(1.0f);
        
        Model = glm::translate(Model, glm::vec3(Entity->Position.x, Entity->Position.y, 0.0f));
        Model = glm::scale(Model, glm::vec3(Entity->Scale.x, Entity->Scale.y, 1));
        
        glBindVertexArray(RenderState->WireframeVAO);
        
        auto Shader = RenderState->Shaders[Shader_Wireframe];
        UseShader(&Shader);
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        glm::vec4 color(0.0,1.0,0.0,1.0);
        
        SetVec4Attribute(Shader.Program, "color", color);
        
        glDrawArrays(GL_LINE_STRIP, 0, 6);
        glBindVertexArray(0);
    }
}

static void RenderAStarPath(render_state* RenderState, entity* Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    
    if(Entity->Enemy.AStarPath) 
    {
        glBindVertexArray(RenderState->AStarPathVAO);
        for(uint32 PathIndex = 0; PathIndex < Entity->Enemy.AStarPathLength; PathIndex++)
        {
            glm::mat4 Model(1.0f);
            Model = glm::translate(Model, glm::vec3(Entity->Enemy.AStarPath[PathIndex].x, Entity->Enemy.AStarPath[PathIndex].y, 0.0f));
            Model = glm::scale(Model, glm::vec3(1, 1, 1));
            
            auto Shader = RenderState->Shaders[Shader_AStarPath];
            UseShader(&Shader);
            
            SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
            SetMat4Uniform(Shader.Program, "View", View);
            SetMat4Uniform(Shader.Program, "Model", Model);
            glm::vec4 color;
            
            if(Entity->Enemy.PathIndex == PathIndex)
            {
                color = glm::vec4(0.0,1.0,0.0,0.4);
            }
            else 
            {
                color = glm::vec4(0.0,0.0,1.0,0.4);
            }
            
            SetVec4Attribute(Shader.Program, "color", color);
            glDrawArrays(GL_QUADS, 0, 6);
            
            
            
        }
        glBindVertexArray(0);
    }
}

static void RenderEntity(render_state *RenderState, entity &Entity, glm::mat4 ProjectionMatrix, glm::mat4 View)
{ 
    render_entity* RenderEntity = &RenderState->RenderEntities[Entity.RenderEntityHandle];
    auto Shader = RenderState->Shaders[RenderEntity->ShaderIndex];
    
    if(RenderEntity->Rendered && Entity.Active)
    {
        glm::mat4 Model(1.0f);
        Model = glm::translate(Model, glm::vec3(Entity.Position.x, Entity.Position.y, 0.0f));
        Model = glm::translate(Model, glm::vec3(1, 1, 0.0f));
        Model = glm::rotate(Model, Entity.Rotation.z, glm::vec3(0, 0, 1)); 
        Model = glm::translate(Model, glm::vec3(-1, -1, 0.0f));
        
        glm::vec3 Scale;
        
        if(Entity.IsFlipped)
        {
            Scale = glm::vec3(-Entity.Scale.x, Entity.Scale.y, Entity.Scale.z);
            Model = glm::translate(Model, glm::vec3(Entity.Scale.x, 0, 0));
        }
        else
            Scale = Entity.Scale;
        
        Model = glm::scale(Model, Scale);
        
        if(Entity.Type == Entity_Enemy)
        {
            if(Entity.Enemy.AIState == AI_Hit)
                RenderEntity->Color = glm::vec4(1, 0, 0, 1);
            else
                RenderEntity->Color = glm::vec4(0, 1, 0, 1); //@Cleanup: This is just placeholder before we get a real enemy sprite that is different from the player
        }
        
        if(Entity.CurrentAnimation) 
        {
            if(Entity.Type == Entity_Barrel)
                printf("asdasd\n");
            animation* Animation = Entity.CurrentAnimation;
            
            if (RenderState->BoundTexture != Animation->TextureHandle) //never bind the same texture if it's already bound
            {
                glBindTexture(GL_TEXTURE_2D, Animation->TextureHandle);
                RenderState->BoundTexture = Animation->TextureHandle;
            }
            
            if(Shader.Program == 0)
            {
                Shader = RenderState->ErrorShaderSprite;
                glBindVertexArray(RenderState->SpriteErrorVAO);
            }
            else
            {
                //Shader = RenderState->SpriteSheetShader;
                glBindVertexArray(RenderState->SpriteSheetVAO);
            }
            UseShader(&Shader);
            auto Frame = Animation->Frames[Entity.AnimationInfo.FrameIndex];
            SetVec2Attribute(Shader.Program,"textureOffset", glm::vec2(Frame.X,Frame.Y));
            SetVec4Attribute(Shader.Program, "color", RenderEntity->Color);
            SetVec2Attribute(Shader.Program,"sheetSize",
                             glm::vec2(Animation->Columns, Animation->Rows));
        } 
        else 
        {
            if (RenderState->BoundTexture != RenderEntity->TextureHandle) //never bind the same texture if it's already bound
            {
                glBindTexture(GL_TEXTURE_2D, RenderEntity->TextureHandle);
                RenderState->BoundTexture = RenderEntity->TextureHandle;
            }
            
            if(Shader.Program == 0)
            {
                Shader = RenderState->ErrorShaderSprite;
                glBindVertexArray(RenderState->SpriteErrorVAO);
            }
            else
            {
                glBindVertexArray(RenderState->SpriteVAO);
            }
            UseShader(&Shader);
        }
        
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", View);
        SetMat4Uniform(Shader.Program, "Model", Model);
        
        glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
        glDrawArrays(GL_QUADS, 0, 4);
        glBindVertexArray(0);
    }
    
    if(RenderState->RenderColliders && !Entity.IsKinematic)
        RenderColliderWireframe(RenderState, &Entity, ProjectionMatrix, View);
    
    if(RenderState->RenderPaths && Entity.Enemy.AStarPath)
        RenderAStarPath(RenderState,&Entity,ProjectionMatrix,View);
    
    if(RenderState->RenderFPS)
    {
        char FPS[32];
        sprintf(FPS, "%4.0f",RenderState->FPS);
        RenderText(RenderState, RenderState->InconsolataFont, glm::vec4(1, 1, 1, 1), FPS, RenderState->WindowWidth / 2.0f, 20.0f, 1.0f, Alignment_Center);
    }
}

static void RenderRoom(render_state* RenderState, const room& Room, glm::mat4 ProjectionMatrix, glm::mat4 View, int StartX, int StartY, int EndX, int EndY)
{
    glBindVertexArray(RenderState->TileVAO);
    
    if (RenderState->BoundTexture != Room.RenderEntity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Room.RenderEntity.TextureHandle);
        RenderState->BoundTexture = Room.RenderEntity.TextureHandle;
    }
    
    auto Shader = RenderState->Shaders[Room.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    real32 Scale = 1.0f;
    
    for (uint32 i = 0; i < Room.Width; i++)
    {
        for (uint32 j = 0; j < Room.Height; j++)
        {
            if(Room.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(i * Scale, j * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                
                SetVec2Attribute(Shader.Program, "textureOffset", Room.Data[i][j].TextureOffset);
                
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", View);
                SetMat4Uniform(Shader.Program, "Model", Model);
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    
    
    glBindVertexArray(0);
}


static void RenderTilemap(render_state* RenderState, const tilemap& Tilemap, glm::mat4 ProjectionMatrix, glm::mat4 View)
{
    glBindVertexArray(RenderState->TileVAO);
    
    if (RenderState->BoundTexture != Tilemap.RenderEntity.TextureHandle) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Tilemap.RenderEntity.TextureHandle);
        RenderState->BoundTexture = Tilemap.RenderEntity.TextureHandle;
    }
    
    auto Shader = RenderState->Shaders[Tilemap.RenderEntity.ShaderIndex];
    UseShader(&Shader);
    
    real32 Scale = 1.0f;
    
    for (uint32 i = 0; i < Tilemap.Width; i++)
    {
        for (uint32 j = 0; j < Tilemap.Height; j++)
        {
            if(Tilemap.Data[i][j].Type != Tile_None)
            {
                glm::mat4 Model(1.0f);
                Model = glm::translate(Model, glm::vec3(i * Scale, j * Scale, 0.0f));
                Model = glm::scale(Model, glm::vec3(Scale, Scale, 1.0f));
                
                SetVec2Attribute(Shader.Program, "textureOffset", Tilemap.Data[i][j].TextureOffset);
                
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", View);
                SetMat4Uniform(Shader.Program, "Model", Model);
                
                glDrawArrays(GL_QUADS, 0, 4);
            }
        }
    }
    
    
    glBindVertexArray(0);
}

int CompareFunction(const void* a, const void* b)
{
    render_entity APtr = *(render_entity*)a;
    render_entity BPtr = *(render_entity*)b;
    
    collision_AABB BoxA = APtr.Entity->CollisionAABB;
    collision_AABB BoxB = BPtr.Entity->CollisionAABB;
    
    if(BoxA.Center.y - BoxA.Extents.y > BoxB.Center.y - BoxB.Extents.y)
        return 1;
    if(BoxA.Center.y  - BoxA.Extents.y < BoxB.Center.y - BoxB.Extents.y)
        return -1;
    return 0;
}

static void RenderInGameMode(game_state* GameState)
{
    RenderTilemap(&GameState->RenderState, GameState->CurrentLevel.Tilemap, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    
    //@Fix this is buggy
    qsort(GameState->RenderState.RenderEntities, GameState->RenderState.RenderEntityCount, sizeof(render_entity), CompareFunction);
    
    
    for(uint32 Index = 0; Index < GameState->RenderState.RenderEntityCount; Index++) 
    {
        render_entity* Render = &GameState->RenderState.RenderEntities[Index];
        
        Render->Entity->RenderEntityHandle = Index;
        
        RenderEntity(&GameState->RenderState, *Render->Entity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
    }
}

static void RenderGame(game_state* GameState)
{
    switch(GameState->GameMode)
    {
        case Mode_MainMenu:
        {
            glm::vec4 TextColor;
            
            for(uint32 Index = 0; Index < GameState->MainMenu.OptionCount; Index++)
            {
                if((int32)Index == GameState->MainMenu.SelectedIndex)
                {
                    RenderRect(Render_Fill, &GameState->RenderState, glm::vec4(1, 1, 1, 1), (real32)GameState->RenderState.WindowWidth / 2 - 200, (real32)GameState->RenderState.WindowHeight / 2 - 10 - 40 * Index, 400, 40);
                    TextColor = glm::vec4(0, 0, 0, 1);
                }
                else
                {
                    TextColor = glm::vec4(1, 1, 1, 1);
                }
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, TextColor, GameState->MainMenu.Options[Index], (real32)GameState->RenderState.WindowWidth / 2, (real32)GameState->RenderState.WindowHeight / 2 - 40 * Index, 1, Alignment_Center);
                
            }
        }
        break;
        case Mode_InGame:
        {
            RenderInGameMode(GameState);
            if(GameState->Paused)
                RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(0.5, 1, 1, 1), "PAUSED", (real32)GameState->RenderState.WindowWidth / 2, 40, 1, Alignment_Center);
        }
        break;
        case Mode_Editor:
        {
            RenderInGameMode(GameState);
            if(GameState->EditorState.SelectedEntity)
                RenderWireframe(&GameState->RenderState, GameState->EditorState.SelectedEntity, GameState->Camera.ProjectionMatrix, GameState->Camera.ViewMatrix);
            RenderText(&GameState->RenderState, GameState->RenderState.MenuFont, glm::vec4(1, 1, 1, 1), "Editor", (real32)GameState->RenderState.WindowWidth / 2, GameState->RenderState.WindowHeight - 100, 1, Alignment_Center);
        }
        break;
    }
}

static void RenderEditorUI(game_state* GameState, const editor_ui& EditorUI, render_state* RenderState)
{
    switch(GameState->EditorUI.State)
    {
        case State_Selection:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
            RenderRect(Render_Outline, &GameState->RenderState, glm::vec4(0.2f, 0.2f, 0.2f, 1), -0.2f, -0.3f, 0.4f, 0.6f);
            
            for(int Index = 0; Index < MENU_OPTIONS_COUNT; Index++)
            {
                RenderText(&GameState->RenderState, GameState->RenderState.InconsolataFont, glm::vec4(1, 1, 1, 1), GameState->EditorUI.MenuOptions[Index], -0.21f, 0 + (MENU_OPTIONS_COUNT - (Index)) * 0.06f, 1);
            }
        }
        break;
        case State_Animations:
        {
        }
        break;
        case State_Collision:
        {
        }
        break;
        case State_EntityList:
        {
            glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            RenderGame(GameState);
            RenderRect(Render_Fill, 
                       RenderState, 
                       glm::vec4(0, 0.4f, 0.3f, 0.6f), 
                       1.0f, 
                       -2.0f + (real32)RenderState->WindowHeight / 2.0f - 20.0f * (1.0f + GameState->EntityCount) / 2.0f, 
                       120.0f, 
                       20.0f * (1.0f + GameState->EntityCount));
            
            for(int i = -1; i < GameState->EntityCount; i++)
            {
                char* EntityName = i == -1 ? "Entities:" : GameState->Entities[i].Name;
                
                if(!EntityName)
                    EntityName = "NO_NAME";
                
                glm::vec4 Color;
                
                if(i == -1 || i == (int32)GameState->EditorUI.SelectedIndex)
                {
                    if(i != -1)
                    {
                        Color = glm::vec4(0, 0, 0, 1);
                        RenderRect(Render_Fill, 
                                   RenderState, 
                                   glm::vec4(1, 1, 1, 1), 
                                   1.0f,
                                   -2.0f + (real32)RenderState->WindowHeight / 2.0f - 20.0f * i, 
                                   120.0f, 
                                   20.0f); 
                    }
                    else
                        Color = glm::vec4(1, 1, 1, 1);
                }
                else
                {
                    Color = glm::vec4(1, 1, 1, 1);
                }
                
                RenderText(RenderState, RenderState->InconsolataFont, Color, EntityName, 2.0f, (real32)RenderState->WindowHeight / 2.0f - 20.0f * i, 1.0f);
            }
        }
        break;
    }
}

static void RenderPlayerUI(health_bar* HealthBar, render_state* RenderState)
{
    real32 X = HealthBar->Position.x * RenderState->ScaleX;
    X -= 1;
    real32 Y = HealthBar->Position.y * RenderState->ScaleY;
    Y -= 1;
    
    auto Shader = RenderState->UISpriteShader;
    
    if(Shader.Program == 0)
    {
        Shader = RenderState->ErrorShaderUI;
        glBindVertexArray(RenderState->UIErrorVAO);
    }
    else
    {
        glBindVertexArray(RenderState->UISpriteVAO);
        glBindTexture(GL_TEXTURE_2D, HealthBar->RenderInfo.TextureHandle);
    }
    
    UseShader(&Shader);
    
    glm::mat4 Model(1.0f);
    Model = glm::translate(Model, glm::vec3(X, Y, 0));
    Model = glm::scale(Model, glm::vec3(0.1, 0.075, 1));
    
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Attribute(Shader.Program, "color", glm::vec4(1, 1, 1, 1)); 
    
    glDrawArrays(GL_QUADS, 0, 4);
    glBindVertexArray(0);
}

static void Render(game_state* GameState)
{
    GameState->RenderState.ScaleX = 2.0f / GameState->RenderState.WindowWidth;
    GameState->RenderState.ScaleY = 2.0f / GameState->RenderState.WindowHeight;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 1.0f);
    
    if(GameState->EditorUI.On)
    {
        RenderEditorUI(GameState, GameState->EditorUI, &GameState->RenderState);
    }
    else
    {
        //glfwSetInputMode(GameState->RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        RenderGame(GameState);
    }
    
    RenderPlayerUI(&GameState->HealthBar, &GameState->RenderState);
    
    if(GameState->Console.CurrentTime > 0)
        RenderConsole(&GameState->RenderState, &GameState->Console);
    
    glfwSwapBuffers(GameState->RenderState.Window);
}
