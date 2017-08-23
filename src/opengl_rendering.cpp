static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %s\n", Description);
}

void FramebufferSizeCallback(GLFWwindow *Window, int Width, int Height)
{
    render_state* RenderState = (render_state*)glfwGetWindowUserPointer(Window);
    
    glfwSetWindowAspectRatio(Window, 16, 9);
    glViewport(0, 0, Width, Height);
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    memcpy(RenderState->Viewport, Viewport, sizeof(GLint) * 4);
    
    RenderState->WindowWidth = Width;
    RenderState->WindowHeight = Height;
    
    // Change resolution of framebuffer texture
    //@Incomplete: This should be done with lower resolutions and just be upscaled maybe? We need fixed resolutions
    glBindTexture(GL_TEXTURE_2D, RenderState->TextureColorBuffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->WindowWidth, RenderState->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->TextureColorBuffer, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline static r64 GetTime()
{
    return glfwGetTime();
}

inline static void PollEvents()
{
    glfwPollEvents();
}

static void CloseWindow(render_state& RenderState)
{
    glfwDestroyWindow(RenderState.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static b32 ShouldCloseWindow(render_state& RenderState)
{
    return glfwWindowShouldClose(RenderState.Window); 
}

static GLint ShaderCompilationErrorChecking(const char* ShaderName, GLuint Shader, memory_arena* TempArena)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        GLchar* ErrorLog = PushSize(TempArena, MaxLength, GLchar);
        
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, ErrorLog);
        
        DEBUG_PRINT("SHADER Compilation error - %s\n", ShaderName);
        DEBUG_PRINT(ErrorLog);
        
        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadShader(const char* FilePath, shader *Shd, memory_arena* TempArena)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert", TempArena);
    GLchar *VertexText = LoadShaderFromFile(VertexString, TempArena);
    
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader, TempArena))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag", TempArena);
    GLchar *FragmentText = LoadShaderFromFile(FragmentString, TempArena);
    
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader, TempArena))
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

static GLuint LoadVertexShader(const char* FilePath, shader *Shd, memory_arena* TempArena)
{
    Shd->Program = glCreateProgram();
    
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert", TempArena);
    GLchar *VertexText = LoadShaderFromFile(VertexString, TempArena);
    glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
    glCompileShader(Shd->VertexShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader, TempArena))
    {
        Shd->Program = 0;
        return GL_FALSE;
    }
    
    glAttachShader(Shd->Program, Shd->VertexShader);
    glAttachShader(Shd->Program, Shd->FragmentShader);
    
    glLinkProgram(Shd->Program);
    
    return GL_TRUE;
}

static GLuint LoadFragmentShader(const char* FilePath, shader *Shd, memory_arena* TempArena)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag", TempArena);
    GLchar *FragmentText = LoadShaderFromFile(FragmentString, TempArena);
    glShaderSource(Shd->FragmentShader, 1, &FragmentText, NULL);
    glCompileShader(Shd->FragmentShader);
    
    if (!ShaderCompilationErrorChecking(FilePath, Shd->FragmentShader, TempArena))
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
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned int X = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(Font->Face, i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, X, 0, G->bitmap.width, G->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
        Font->CharacterInfo[i].AX = (r32)(G->advance.x >> 6);
        Font->CharacterInfo[i].AY = (r32)(G->advance.y >> 6);
        
        Font->CharacterInfo[i].BW = (r32)G->bitmap.width;
        Font->CharacterInfo[i].BH = (r32)G->bitmap.rows;
        
        Font->CharacterInfo[i].BL = (r32)G->bitmap_left;
        Font->CharacterInfo[i].BT = (r32)G->bitmap_top;
        
        Font->CharacterInfo[i].TX = (r32)X / Font->AtlasWidth;
        
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

static void RegisterBuffers(render_state& RenderState, GLfloat* VertexBuffer, i32 VertexBufferSize, GLuint* IndexBuffer, i32 IndexBufferSize, b32 HasNormals, b32 HasUVs, i32 BufferHandle = -1)
{
    buffer* Buffer = &RenderState.Buffers[BufferHandle == -1 ? RenderState.BufferCount : BufferHandle];
    
    Buffer->VertexBufferSize = VertexBufferSize;
    Buffer->IndexBufferSize = IndexBufferSize;
    
    if(Buffer->VAO == 0)
        glGenVertexArrays(1, &Buffer->VAO);
    
    glBindVertexArray(Buffer->VAO);
    
    if(Buffer->VBO == 0)
        glGenBuffers(1, &Buffer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * VertexBufferSize, VertexBuffer, GL_STATIC_DRAW);
    
    if(HasNormals && HasUVs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    }
    else if(HasNormals)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    }
    else if(HasUVs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    }
    else
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    }
    
    glGenBuffers(1, &Buffer->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndexBufferSize, IndexBuffer, GL_STATIC_DRAW);
    
    if(BufferHandle == -1)
        RenderState.BufferCount++;
    
    glBindVertexArray(0);
}

static void RegisterVertexBuffer(render_state& RenderState, GLfloat* BufferData, i32 Size, Shader_Type ShaderType, memory_arena* TempArena, i32 BufferHandle = -1)
{
    buffer* Buffer = &RenderState.Buffers[BufferHandle == -1 ? RenderState.BufferCount : BufferHandle];
    
    Buffer->VertexBufferSize = Size;
    Buffer->IndexBufferSize = 0;
    Buffer->IBO = 0;
    
    if(Buffer->VAO == 0)
        glGenVertexArrays(1, &Buffer->VAO);
    
    glBindVertexArray(Buffer->VAO);
    
    if(Buffer->VBO == 0)
        glGenBuffers(1, &Buffer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffer->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * Size, BufferData, GL_STATIC_DRAW);
    
    if(!RenderState.Shaders[ShaderType].Loaded)
    {
        LoadShader(ShaderPaths[ShaderType], &RenderState.Shaders[ShaderType], TempArena);
    }
    else
        UseShader(&RenderState.Shaders[ShaderType]);
    
    auto PositionLocation = glGetAttribLocation(RenderState.TileShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState.TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if(BufferHandle == -1)
        RenderState.BufferCount++;
}

static void RenderSetup(render_state *RenderState, memory_arena* TempArena)
{
    if(FT_Init_FreeType(&RenderState->FTLibrary)) 
    {
        fprintf(stderr, "Could not init freetype library\n");
    }
    
    // Framebuffer
    glGenFramebuffers(1, &RenderState->FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->FrameBuffer);
    
    glGenTextures(1, &RenderState->TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->TextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->WindowWidth, RenderState->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->TextureColorBuffer, 0
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DEBUG_PRINT("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    GLuint DepthBuffer;
    glGenRenderbuffers(1, &DepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RenderState->WindowWidth, RenderState->WindowHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);
    
    glGenFramebuffers(1, &RenderState->LightingFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->LightingFrameBuffer);
    
    glGenTextures(1, &RenderState->LightingTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->LightingTextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->WindowWidth, RenderState->WindowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->LightingTextureColorBuffer, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DEBUG_PRINT("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    // FrameBuffer VAO
    glGenVertexArrays(1, &RenderState->FrameBufferVAO);
    glBindVertexArray(RenderState->FrameBufferVAO);
    glGenBuffers(1, &RenderState->FrameBufferVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->FrameBufferVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->FrameBufferVertices, GL_DYNAMIC_DRAW);
    
    RenderState->FrameBufferShader.Type = Shader_FrameBuffer;
    
    LoadShader(ShaderPaths[Shader_FrameBuffer], &RenderState->FrameBufferShader, TempArena);
    
    auto PosLoc = glGetAttribLocation(RenderState->FrameBufferShader.Program, "pos");
    auto TexLoc = glGetAttribLocation(RenderState->FrameBufferShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PosLoc);
    glVertexAttribPointer(PosLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexLoc);
    glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    //auto TexLoc = glGetUniformLocation(GameState->RenderState.FrameBufferShader.Program, "tex");
    //glUniform1i(TexLoc, 0);
    //TexLoc = glGetUniformLocation(GameState->RenderState.FrameBufferShader.Program, "lightingTex");
    //glUniform1i(TexLoc, 1);
    
    RenderState->FrameBufferTex0Loc = glGetUniformLocation(RenderState->FrameBufferShader.Program, "tex");
    RenderState->FrameBufferTex1Loc = glGetUniformLocation(RenderState->FrameBufferShader.Program, "lightingTex");
    
    glGenBuffers(1, &RenderState->QuadIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RenderState->QuadIndices), RenderState->QuadIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader, TempArena);
    
    auto PositionLocation = glGetAttribLocation(RenderState->TextureShader.Program, "pos");
    auto TexcoordLocation = glGetAttribLocation(RenderState->TextureShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    //Animation
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteSheetVAO);
    glBindVertexArray(RenderState->SpriteSheetVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureShader.Type = Shader_Spritesheet;
    LoadShader(ShaderPaths[Shader_Spritesheet], &RenderState->SpritesheetShader, TempArena);
    
    PositionLocation = glGetAttribLocation(RenderState->SpritesheetShader.Program, "pos");
    TexcoordLocation = glGetAttribLocation(RenderState->SpritesheetShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->ErrorShaderSprite.Type = Shader_ErrorSprite;
    LoadShader(ShaderPaths[Shader_ErrorSprite], &RenderState->ErrorShaderSprite, TempArena);
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->UISpriteShader.Type = Shader_UISprite;
    LoadShader(ShaderPaths[Shader_UISprite], &RenderState->UISpriteShader, TempArena);
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //error shader
    RenderState->ErrorShaderUI.Type = Shader_ErrorUI;
    LoadShader(ShaderPaths[Shader_ErrorUI], &RenderState->ErrorShaderUI, TempArena);
    
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
    glBufferData(GL_ARRAY_BUFFER, RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_STATIC_DRAW);
    
    RenderState->TileShader.Type = Shader_Tile;
    LoadShader(ShaderPaths[Shader_Tile], &RenderState->TileShader, TempArena);
    
    auto PositionLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "pos");
    auto TexcoordLocation2 = glGetAttribLocation(RenderState->TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation2);
    glVertexAttribPointer(PositionLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation2);
    glVertexAttribPointer(TexcoordLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //console
    glGenVertexArrays(1, &RenderState->RectVAO);
    glBindVertexArray(RenderState->RectVAO);
    glGenBuffers(1, &RenderState->NormalQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->NormalQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->NormalQuadVerticesSize, RenderState->NormalQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->RectShader.Type = Shader_Rect;
    LoadShader(ShaderPaths[Shader_Rect], &RenderState->RectShader, TempArena);
    
    auto PositionLocation3 = glGetAttribLocation(RenderState->RectShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &RenderState->TextureRectVAO);
    glBindVertexArray(RenderState->TextureRectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureRectShader.Type = Shader_TextureRect;
    LoadShader(ShaderPaths[Shader_TextureRect], &RenderState->TextureRectShader, TempArena);
    
    PositionLocation2 = glGetAttribLocation(RenderState->TextureRectShader.Program, "pos");
    TexcoordLocation2 = glGetAttribLocation(RenderState->TextureRectShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation2);
    glVertexAttribPointer(PositionLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation2);
    glVertexAttribPointer(TexcoordLocation2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    //wireframe
    glGenVertexArrays(1, &RenderState->WireframeVAO);
    glBindVertexArray(RenderState->WireframeVAO);
    glGenBuffers(1, &RenderState->WireframeQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->WireframeQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->WireframeQuadVerticesSize, RenderState->WireframeQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->RectShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader, TempArena);
    
    PositionLocation3 = glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &RenderState->IsometricVAO);
    glBindVertexArray(RenderState->IsometricVAO);
    glGenBuffers(1, &RenderState->IsometricQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->IsometricQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, RenderState->WireframeQuadVerticesSize, RenderState->IsometricQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->RectShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader, TempArena);
    
    PositionLocation3 = glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    // Lines
    glGenBuffers(1, &RenderState->PrimitiveVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->PrimitiveVBO);
    
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
    
    LoadShader(ShaderPaths[Shader_AStarPath], &RenderState->AStarPathShader, TempArena);
    
    PositionLocation3 = glGetAttribLocation(RenderState->AStarPathShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader, TempArena);
    
    RenderState->RobotoFont = {};
    InitializeFreeTypeFont("../assets/fonts/roboto/Roboto-Regular.ttf", 20, RenderState->FTLibrary, &RenderState->RobotoFont, &RenderState->StandardFontShader);
    
    RenderState->InconsolataFont = {};
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 18, RenderState->FTLibrary, &RenderState->InconsolataFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 12, RenderState->FTLibrary, &RenderState->SmallerInconsolataFont, &RenderState->StandardFontShader);
    
    InitializeFreeTypeFont("../assets/fonts/inconsolata/Inconsolata-Regular.ttf", 40, RenderState->FTLibrary, &RenderState->MenuFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/roboto/Roboto-Regular.ttf", 30, RenderState->FTLibrary, &RenderState->ButtonFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/rubber-biscuit/RUBBBB__.ttf", 50, RenderState->FTLibrary, &RenderState->TitleFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/pixelmix.ttf", 18, RenderState->FTLibrary, &RenderState->DamageFont, &RenderState->StandardFontShader);
    InitializeFreeTypeFont("../assets/fonts/Arialic-Hollow.ttf", 18, RenderState->FTLibrary, &RenderState->OutlineFont, &RenderState->StandardFontShader);
    
    // Light sources
    RenderState->LightSourceShader.Type = Shader_LightSource;
    LoadShader(ShaderPaths[Shader_LightSource], &RenderState->LightSourceShader, TempArena);
    
    RenderState->SimpleModelShader.Type = Shader_SimpleModel;
    LoadShader(ShaderPaths[Shader_SimpleModel], &RenderState->SimpleModelShader, TempArena);
    
    RenderState->SpotlightData.NumLights = 0;
    glGenBuffers(1, &RenderState->SpotlightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, RenderState->SpotlightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(spotlight_data), &RenderState->SpotlightData, GL_DYNAMIC_DRAW);
    
    u32 BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "spotlights");
    
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    RenderState->DirectionalLightData.NumLights = 0;
    glGenBuffers(1, &RenderState->DirectionalLightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, RenderState->DirectionalLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(directional_light_data), &RenderState->DirectionalLightData, GL_DYNAMIC_DRAW);
    
    BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "directionalLights");
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    RenderState->PointLightData.NumLights = 0;
    glGenBuffers(1, &RenderState->PointLightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, RenderState->PointLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(point_light_data), &RenderState->PointLightData, GL_DYNAMIC_DRAW);
    
    BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "pointLights");
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void UpdateLightingData(const render_state& RenderState)
{
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.SpotlightUBO);
    GLvoid* P = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(P, &RenderState.SpotlightData, sizeof(spotlight_data));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.DirectionalLightUBO);
    P = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(P, &RenderState.DirectionalLightData, sizeof(directional_light_data));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.PointLightUBO);
    P = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    memcpy(P, &RenderState.PointLightData, sizeof(point_light_data));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

static GLuint LoadTexture(texture_data& Data, texture* Texture)
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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Data.Width, Data.Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, (GLvoid*) Data.ImageData);
    
    Texture->TextureHandle = TextureHandle;
    stbi_image_free(Data.ImageData);
    
    return GL_TRUE;
}

static void LoadTextures(render_state& RenderState, renderer& Renderer)
{
    for(i32 Index = RenderState.TextureIndex; Index < Renderer.TextureCount; Index++)
    {
        LoadTexture(Renderer.TextureData[Index], &RenderState.TextureArray[RenderState.TextureIndex++]);
    }
}

static void InitializeOpenGL(render_state& RenderState, renderer& Renderer, config_data* ConfigData, memory_arena* TempArena)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    RenderState.Window = glfwCreateWindow(ConfigData->ScreenWidth, ConfigData->ScreenHeight, Concat(Concat(ConfigData->Title, " ", TempArena), ConfigData->Version, TempArena),  ConfigData->Fullscreen ? glfwGetPrimaryMonitor() : NULL, 
                                          NULL);
    RenderState.Contrast = ConfigData->Contrast;
    RenderState.Brightness = ConfigData->Brightness;
    
    if (!RenderState.Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    //center window on screen
    const GLFWvidmode *Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int Width, Height;
    
    glfwGetFramebufferSize(RenderState.Window, &Width, &Height);
    glfwSetWindowPos(RenderState.Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    glfwSetWindowAspectRatio(RenderState.Window, 16, 9);
    
    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(RenderState.Window, FramebufferSizeCallback);
    
    glfwMakeContextCurrent(RenderState.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(RenderState.Window, &RenderState.WindowWidth, &RenderState.WindowHeight);
    glViewport(0, 0, RenderState.WindowWidth, RenderState.WindowHeight);
    glDisable(GL_DITHER);
    glLineWidth(2.0f);
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    DEBUG_PRINT("%s\n", glGetString(GL_VERSION));
    
    glfwSetWindowUserPointer(RenderState.Window, &RenderState);
    glfwSetKeyCallback(RenderState.Window, KeyCallback);
    glfwSetCharCallback(RenderState.Window, CharacterCallback);
    glfwSetCursorPosCallback(RenderState.Window, CursorPositionCallback);
    glfwSetMouseButtonCallback(RenderState.Window, MouseButtonCallback);
    glfwSetScrollCallback(RenderState.Window, ScrollCallback);
    
    GLint Viewport[4];
    glGetIntegerv(GL_VIEWPORT, Viewport);
    memcpy(RenderState.Viewport, Viewport, sizeof(GLint) * 4);
    
    ControllerPresent();
    
    RenderSetup(&RenderState, TempArena);
}

static void ReloadVertexShader(Shader_Type Type, render_state* RenderState, memory_arena* TempArena)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].VertexShader);
    LoadVertexShader(ShaderPaths[Type], &RenderState->Shaders[Type], TempArena);
}

static void ReloadFragmentShader(Shader_Type Type, render_state* RenderState, memory_arena* TempArena)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].FragmentShader);
    LoadFragmentShader(ShaderPaths[Type], &RenderState->Shaders[Type], TempArena);
    
    RenderState->SpotlightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, RenderState->SpotlightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(spotlight_data), &RenderState->SpotlightData, GL_DYNAMIC_DRAW);
    
    u32 BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "spotlights");
    
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    RenderState->DirectionalLightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, RenderState->DirectionalLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(directional_light_data), &RenderState->DirectionalLightData, GL_DYNAMIC_DRAW);
    
    BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "directionalLights");
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    RenderState->PointLightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, RenderState->PointLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(point_light_data), &RenderState->PointLightData, GL_DYNAMIC_DRAW);
    
    BlockIndex = glGetUniformBlockIndex(RenderState->SimpleModelShader.Program, "pointLights");
    glUniformBlockBinding(RenderState->SimpleModelShader.Program, BlockIndex, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void ReloadAssets(render_state& RenderState, asset_manager* AssetManager, memory_arena* TempArena)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(AssetManager->DirtyVertexShaderIndices[i] == 1)
        {
            DEBUG_PRINT("Reloading vertex shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            ReloadVertexShader((Shader_Type)i, &RenderState, TempArena);
            AssetManager->DirtyVertexShaderIndices[i] = 0;
        }
        
        if(AssetManager->DirtyFragmentShaderIndices[i] == 1)
        {
            DEBUG_PRINT("Reloading fragment shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            ReloadFragmentShader((Shader_Type)i, &RenderState, TempArena);
            AssetManager->DirtyFragmentShaderIndices[i] = 0;
        }
    }
}

static void SetFloatUniform(GLuint ShaderHandle, const char* UniformName, r32 Value)
{
    glUniform1f(glGetUniformLocation(ShaderHandle, UniformName), Value);
}

static void SetIntUniform(GLuint ShaderHandle, const char* UniformName, i32 Value)
{
    glUniform1i(glGetUniformLocation(ShaderHandle, UniformName), Value);
}

static void SetVec2Uniform(GLuint ShaderHandle, const char *UniformName, math::v2 Value)
{
    glUniform2f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y);
}

void SetVec3Uniform(GLuint ShaderHandle, const char *UniformName, math::v3 Value)
{
    glUniform3f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z);
}

static void SetVec4Uniform(GLuint ShaderHandle, const char *UniformName, math::v4 Value)
{
    glUniform4f(glGetUniformLocation(ShaderHandle, UniformName), Value.x, Value.y, Value.z, Value.w);
}

static void SetMat4Uniform(GLuint ShaderHandle, const char *UniformName, math::m4 V)
{
    glUniformMatrix4fv(glGetUniformLocation(ShaderHandle, UniformName), 1, GL_TRUE, &V[0][0]);
}

static void SetVec4ArrayUniform(GLuint ShaderHandle, const char *UniformName, math::v4* Value, u32 Length)
{
    glUniform4fv(glGetUniformLocation(ShaderHandle, UniformName), Length, (GLfloat*)&Value[0]);
}

static void SetFloatArrayUniform(GLuint ShaderHandle, const char *UniformName, r32* Value, u32 Length)
{
    glUniform1fv(glGetUniformLocation(ShaderHandle, UniformName), Length, (GLfloat*)&Value[0]);
}

static void RenderLine(render_state& RenderState, math::v4 Color, r32 X1, r32 Y1, r32 X2, r32 Y2, b32 IsUI = true, math::m4 ProjectionMatrix = math::m4(), math::m4 ViewMatrix = math::m4())
{
    if(IsUI)
    {
        X1 *= RenderState.ScaleX;
        X1 -= 1;
        Y1 *= RenderState.ScaleY;
        Y1 -= 1;
        X2 *= RenderState.ScaleX;
        X2 -= 1;
        Y2 *= RenderState.ScaleY;
        Y2 -= 1;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.PrimitiveVBO);
    
    GLfloat Points[4] = { X1, Y1, X2, Y2 };
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);
    
    auto& Shader = RenderState.RectShader;
    UseShader(&Shader);
    
    //draw upper part
    math::m4 Model(1.0f);
    
    if(!IsUI)
    {
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    }
    
    SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    
    glDrawArrays(GL_LINES, 0, 4);
}

// NOTE(Niels): Possible future use but buggy
void RenderCircle(render_state& RenderState, math::v4 Color, r32 CenterX, r32 CenterY, r32 Radius, b32 IsUI = true, math::m4 ProjectionMatrix = math::m4(), math::m4 ViewMatrix = math::m4())
{
    if(IsUI)
    {
        CenterX *= RenderState.ScaleX;
        CenterX -= 1;
        CenterY *= RenderState.ScaleY;
        CenterY -= 1;
        Radius *= RenderState.ScaleX;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.PrimitiveVBO);
    
    GLfloat Points[720];
    
    i32 PointIndex = 0;
    
    for(i32 Index = 0; Index < 360; Index++)
    {
        r32 Radians = (Index * PI) / 180.0f;
        Points[PointIndex++] = cos(Radians * Radius);
        Points[PointIndex++] = sin(Radians * Radius);
    }
    
    glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);
    
    auto& Shader = RenderState.RectShader;
    UseShader(&Shader);
    
    //draw upper part
    math::m4 Model(1.0f);
    
    if(!IsUI)
    {
        SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
        SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    }
    
    SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
    SetMat4Uniform(Shader.Program, "M", Model);
    SetVec4Uniform(Shader.Program, "color", Color);
    
    glDrawArrays(GL_LINE_LOOP, 0, 720);
}

static void RenderRect(Render_Mode Mode, render_state& RenderState, math::v4 Color, r32 X, r32 Y, r32 Width, r32 Height, u32 TextureHandle = 0, b32 IsUI = true, math::m4 ProjectionMatrix = math::m4(), math::m4 ViewMatrix = math::m4())
{
    if(IsUI)
    {
        X *= RenderState.ScaleX;
        X -= 1;
        Y *= RenderState.ScaleY;
        Y -= 1;
        
        Width *= RenderState.ScaleX;
        Height *= RenderState.ScaleY;
    }
    
    switch(Mode)
    {
        case Render_Fill:
        {
            auto Shader = RenderState.RectShader;
            
            if(TextureHandle != 0)
            {
                glBindVertexArray(RenderState.TextureRectVAO);
            }
            else
            {
                glBindVertexArray(RenderState.RectVAO);
            }
            
            if(TextureHandle != 0)
            {
                glBindTexture(GL_TEXTURE_2D, TextureHandle);
                Shader = RenderState.TextureRectShader;
                RenderState.BoundTexture = TextureHandle;
            } 
            
            UseShader(&Shader);
            
            //draw upper part
            math::m4 Model(1.0f);
            Model = math::Translate(Model, math::v3(X, Y, 0));
            Model = math::Scale(Model, math::v3(Width, Height, 1));
            
            if(!IsUI)
            {
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", ViewMatrix);
            }
            
            SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            glDrawElements(GL_TRIANGLES, sizeof(RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0);
        }
        break;
        case Render_Outline:
        {
            math::m4 Model(1.0f);
            Model = math::Translate(Model, math::v3(X, Y, 0));
            Model = math::Scale(Model, math::v3(Width, Height, 1));
            
            glBindVertexArray(RenderState.WireframeVAO);
            
            auto Shader = RenderState.RectShader;
            UseShader(&Shader);
            
            if(!IsUI)
            {
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", ViewMatrix);
            }
            
            SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        break;
    }
    glBindVertexArray(0);
}

static void MeasureText(const render_font& Font, const char* Text, float* Width, float* Height)
{
    int Count;
    
    if (!Text) 
    {
        return;
    }
    
    *Width = 0.0f;
    *Height = 0.0f;
    
    for(Count = 0; Count < strlen(Text); ++Count) 
    {
        *Width += Font.CharacterInfo[Text[Count]].AX;
        
        if(*Height < Font.CharacterInfo[Text[Count]].BH) 
        {
            *Height = Font.CharacterInfo[Text[Count]].BH;
        }
    }
}

//rendering methods
static void RenderText(render_state& RenderState, const render_font& Font, const math::v4& Color, const char* Text, r32 X, r32 Y,
                       memory_arena* TempArena, Alignment Alignment = Alignment_Left,  b32 AlignCenterY = true) 
{
    glBindVertexArray(Font.VAO);
    auto Shader = RenderState.Shaders[Shader_StandardFont];
    UseShader(&Shader);
    SetVec4Uniform(Shader.Program, "color", Color);
    SetVec4Uniform(Shader.Program, "alphaColor", Font.AlphaColor);
    
    if (RenderState.BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState.BoundTexture = Font.Texture;
    }
    
    point* Coords = PushArray(TempArena, 6 * strlen(Text), point);
    
    int N = 0;
    
    switch(Alignment)
    {
        case Alignment_Left:
        break;
        case Alignment_Right:
        break;
        case Alignment_Center:
        {
            r32 Width;
            r32 Height;
            
            MeasureText(Font, Text, &Width, &Height);
            
            X -= Width / 2.0f;
            if(AlignCenterY)
                Y -= Height / 2.0f;
        }
        break;
    }
    
    X = (r32)(i32)X;
    Y = (r32)(i32)Y;
    
    if((i32)X % 2 != 0)
        X-= 1.0f;
    if((i32)Y % 2 != 0)
        Y -= 1.0f;
    
    X *= RenderState.ScaleX;
    X -= 1.0f;
    Y *= RenderState.ScaleY;
    Y -= 1.0f;
    
    for(const char *P = Text; *P; P++) 
    { 
        r32 W = Font.CharacterInfo[*P].BW * RenderState.ScaleX;
        r32 H = Font.CharacterInfo[*P].BH * RenderState.ScaleY;
        
        r32 X2 =  X + Font.CharacterInfo[*P ].BL * RenderState.ScaleX;
        r32 Y2 = -Y - Font.CharacterInfo[*P ].BT * RenderState.ScaleY;
        
        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*P].AX * RenderState.ScaleX;
        Y += Font.CharacterInfo[*P].AY * RenderState.ScaleY;
        
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
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


static void RenderConsole(render_state& RenderState, console* Console, memory_arena* TempArena)
{
    glBindVertexArray(RenderState.RectVAO);
    
    r32 PercentAnimated = 1.0f + 1.0f - (r32)Console->CurrentTime / (r32)Console->TimeToAnimate;
    
    //draw upper part
    RenderRect(Render_Fill, RenderState, math::v4(0.0f, 0.4f, 0.3f, 0.6f), 0.0f, (r32)RenderState.WindowHeight * 0.77f * PercentAnimated, (r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.23f);
    
    //draw lower bar
    RenderRect(Render_Fill, RenderState, math::v4(0.0f, 0.2f, 0.2f, 0.6f), 0.0f, (r32)RenderState.WindowHeight * 0.77f * PercentAnimated, (r32)RenderState.WindowWidth, 20);  
    
    GLfloat TimeValue = (r32)glfwGetTime();
    GLfloat AlphaValue = (r32)((sin(TimeValue * 4) / 2) + 0.5f);
    r32 Width;
    r32 Height;
    MeasureText(RenderState.InconsolataFont, &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderRect(Render_Fill, RenderState, math::v4(AlphaValue, 1, AlphaValue, 1), 5 / 1920.0f * (r32)RenderState.WindowWidth + Width, RenderState.WindowHeight * 0.77f * PercentAnimated, 10, 20);
    
    RenderText(RenderState, RenderState.InconsolataFont, math::v4(0, 0.8, 0, 1),  &Console->Buffer[0],  5 / 1920.0f * (r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.775f * PercentAnimated, TempArena);
    
    int index = 0;
    
    math::v4 Color;
    
    for(int Index = 0; Index < HISTORY_BUFFER_LINES; Index++)
    {
        if(Index % 2 != 0)
            Color = math::v4(0, 1, 0, 1);
        else
            Color = math::v4(1, 1, 1, 1);
        
        RenderText(RenderState, RenderState.InconsolataFont, Color, &Console->HistoryBuffer[Index][0], 5 / 1920.0f * (r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.78f * PercentAnimated + (Index + 1) * 20 * PercentAnimated, TempArena);
    }
}

static void RenderLine(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    
}

static void RenderText(const render_command& Command, render_state& RenderState, memory_arena* TempArena)
{
    // @Incomplete: Need to set a font
    RenderText(RenderState, RenderState.InconsolataFont, Command.Text.Color, Command.Text.Text, Command.Text.Position.x, Command.Text.Position.y, TempArena);
}

static void RenderRect(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    if(Command.IsUI)
    {
        RenderRect(Command.Rect.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Rect.Color, 
                   Command.Rect.Position.x, 
                   Command.Rect.Position.y, 
                   Command.Rect.Size.x, 
                   Command.Rect.Size.y, 0, true);
    }
    else
    {
        RenderRect(Command.Rect.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Rect.Color, 
                   Command.Rect.Position.x, 
                   Command.Rect.Position.y,  
                   Command.Rect.Size.x, 
                   Command.Rect.Size.y, 
                   0,
                   Command.IsUI,
                   Projection, 
                   View);
    }
}

static void RenderSprite(const render_command& Command, render_state& RenderState, renderer& Renderer,  math::m4 Projection, math::m4 View)
{
    texture_data* TextureData = Renderer.TextureMap[Command.Sprite.TextureName];
    
    const texture& Texture = RenderState.TextureArray[TextureData->Handle];
    
    if (RenderState.BoundTexture != Texture.TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Texture.TextureHandle);
        RenderState.BoundTexture = Texture.TextureHandle;
    }
    
    shader Shader = RenderState.SpritesheetShader;
    
    if(Shader.Program == 0)
    {
        Shader = RenderState.ErrorShaderSprite;
        glBindVertexArray(RenderState.SpriteErrorVAO);
    }
    else
    {
        glBindVertexArray(RenderState.SpriteSheetVAO);
    }
    
    UseShader(&Shader);
    
    math::m4 Model(1.0f);
    Model = math::Scale(Model, math::v3(Command.Sprite.Scale.x, Command.Sprite.Scale.y, Command.Sprite.Scale.z));
    
    Model = math::YRotate(Command.Rotation.y) * Model;
    Model = math::XRotate(Command.Rotation.x) * Model;
    Model = math::ZRotate(Command.Rotation.z) * Model;
    
    Model = math::Translate(Model, math::v3(Command.Sprite.Position.x, Command.Sprite.Position.y, Command.Sprite.Position.z));
    
    SetVec4Uniform(Shader.Program, "spriteColor", Command.Sprite.Color);
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetVec2Uniform(Shader.Program,"textureOffset", Command.Sprite.TextureOffset);
    SetFloatUniform(Shader.Program, "frameWidth", Command.Sprite.Frame.x);
    SetFloatUniform(Shader.Program, "frameHeight", Command.Sprite.Frame.y);
    SetVec2Uniform(Shader.Program,"textureSize",
                   math::v2(TextureData->Width, TextureData->Height));
    
    SetFloatUniform(Shader.Program, "time", (r32)GetTime());
    SetMat4Uniform(Shader.Program, "Projection", Projection);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    
    glDrawElements(GL_TRIANGLES, sizeof(RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

static void RenderModel(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    for(i32 HandleIndex = 0; HandleIndex < Command.Model.HandleCount; HandleIndex++)
    {
        mesh_render_data RenderData = Command.Model.RenderData[HandleIndex];
        
        buffer Buffer = RenderState.Buffers[RenderData.BufferHandle];
        
        glBindVertexArray(Buffer.VAO);
        
        if(RenderData.Material.HasTexture)
        {
            texture Texture = RenderState.TextureArray[RenderData.Material.TextureHandle];
            if(RenderState.BoundTexture != Texture.TextureHandle)
            {
                glBindTexture(GL_TEXTURE_2D, Texture.TextureHandle);
                RenderState.BoundTexture = Texture.TextureHandle;
            }
        }
        
        // @Incomplete: Missing shader switching
        auto Shader = RenderState.SimpleModelShader;
        UseShader(&Shader);
        
        math::m4 Model(1.0f);
        Model = math::Scale(Model, Command.Scale);
        
        Model = math::YRotate(Command.Rotation.y) * Model;
        Model = math::XRotate(Command.Rotation.x) * Model;
        Model = math::ZRotate(Command.Rotation.z) * Model;
        
        Model = math::Translate(Model, Command.Position);
        
        math::m4 NormalMatrix = math::Transpose(math::Inverse(View * Model));
        
        SetMat4Uniform(Shader.Program, "normalMatrix", NormalMatrix);
        SetMat4Uniform(Shader.Program, "projection", Projection);
        SetMat4Uniform(Shader.Program, "view", View);
        SetMat4Uniform(Shader.Program, "model", Model);
        SetVec4Uniform(Shader.Program, "color", math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        SetIntUniform(Shader.Program, "hasUVs", RenderData.Material.HasTexture);
        
        glDrawElements(GL_TRIANGLES, Buffer.IndexBufferSize, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}

static void RenderBuffer(const render_command& Command, render_state& RenderState, renderer& Renderer,  math::m4 Projection, math::m4 View)
{
    buffer Buffer = RenderState.Buffers[Command.Buffer.BufferHandle];
    
    glBindVertexArray(Buffer.VAO);
    
    texture_data* TextureData = Renderer.TextureMap[Command.Buffer.TextureName];
    
    const texture& Texture = RenderState.TextureArray[TextureData->Handle];
    
    if (RenderState.BoundTexture != Texture.TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, Texture.TextureHandle);
        RenderState.BoundTexture = Texture.TextureHandle;
    }
    
    auto Shader = RenderState.TileShader;
    UseShader(&Shader);
    
    math::m4 Model(1.0f);
    Model = math::Scale(Model, math::v3(1, 1, 1.0f));
    
    Model = math::YRotate(Command.Rotation.y) * Model;
    Model = math::XRotate(Command.Rotation.x) * Model;
    Model = math::ZRotate(Command.Rotation.z) * Model;
    
    SetFloatUniform(Shader.Program, "isUI", 0);
    SetMat4Uniform(Shader.Program, "Projection", Projection);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetVec4Uniform(Shader.Program, "Color", math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
    
    glDrawArrays(GL_QUADS, 0, Buffer.VertexBufferSize / 4);
    glBindVertexArray(0);
}

static void RenderCommands(render_state& RenderState, renderer& Renderer, memory_arena* TempArena)
{
    for(i32 Index = RenderState.BufferCount; Index < Renderer.BufferCount; Index++)
    {
        buffer_data Data = Renderer.Buffers[Index];
        
        if(Data.IndexBufferSize == 0)
        {
            RegisterVertexBuffer(RenderState, Data.VertexBuffer, Data.VertexBufferSize, Data.ShaderType, TempArena, Data.ExistingHandle);
        }
        else
        {
            RegisterBuffers(RenderState, Data.VertexBuffer, Data.VertexBufferSize, Data.IndexBuffer, Data.IndexBufferSize, Data.HasNormals, Data.HasUVs, Data.ExistingHandle);
            
        }
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    auto& V = Camera.ViewMatrix;
    
    for(i32 Index = 0; Index < Renderer.LightCommandCount; Index++)
    {
        const render_command& Command = *((render_command*)Renderer.LightCommands.Base + Index);
        
        switch(Command.Type)
        {
            case RenderCommand_Spotlight:
            {
                spotlight& Spotlight = RenderState.SpotlightData.Spotlights[RenderState.SpotlightData.NumLights++];
                
                auto Pos = V * math::v4(Command.Position, 1.0f);
                
                Spotlight.Position[0] = Pos.x;
                Spotlight.Position[1] = Pos.y;
                Spotlight.Position[2] = Pos.z;
                Spotlight.Position[3] = 1;
                
                Spotlight.Direction[0] = Command.Spotlight.Direction.x;
                Spotlight.Direction[1] = Command.Spotlight.Direction.y;
                Spotlight.Direction[2] = Command.Spotlight.Direction.z;
                Spotlight.Direction[3] = 0;
                
                Spotlight.CutOff = Command.Spotlight.CutOff;
                Spotlight.OuterCutOff = Command.Spotlight.OuterCutOff;
                
                Spotlight.Ambient[0] = Command.Spotlight.Ambient.x;
                Spotlight.Ambient[1] = Command.Spotlight.Ambient.y;
                Spotlight.Ambient[2] = Command.Spotlight.Ambient.z;
                Spotlight.Ambient[3] = 1.0f;
                
                Spotlight.Diffuse[0] = Command.Spotlight.Diffuse.x;
                Spotlight.Diffuse[1] = Command.Spotlight.Diffuse.y;
                Spotlight.Diffuse[2] = Command.Spotlight.Diffuse.z;
                Spotlight.Diffuse[3] = 1.0f;
                Spotlight.Specular[0] = Command.Spotlight.Specular.x;
                Spotlight.Specular[1] = Command.Spotlight.Specular.y;
                Spotlight.Specular[2] = Command.Spotlight.Specular.z;
                Spotlight.Specular[3] = 1.0f;
                
                Spotlight.Constant = Command.Spotlight.Constant;
                Spotlight.Linear = Command.Spotlight.Linear;
                Spotlight.Quadratic = Command.Spotlight.Quadratic;
            }
            break;
            case RenderCommand_DirectionalLight:
            {
                directional_light& DirectionalLight = RenderState.DirectionalLightData.DirectionalLights[RenderState.DirectionalLightData.NumLights++];
                
                DirectionalLight.Direction[0] = Command.DirectionalLight.Direction.x;
                DirectionalLight.Direction[1] = Command.DirectionalLight.Direction.y;
                DirectionalLight.Direction[2] = Command.DirectionalLight.Direction.z;
                DirectionalLight.Direction[3] = 0;
                
                DirectionalLight.Ambient[0] = Command.DirectionalLight.Ambient.x;
                DirectionalLight.Ambient[1] = Command.DirectionalLight.Ambient.y;
                DirectionalLight.Ambient[2] = Command.DirectionalLight.Ambient.z;
                DirectionalLight.Ambient[3] = 1;
                DirectionalLight.Diffuse[0] = Command.DirectionalLight.Diffuse.x;
                DirectionalLight.Diffuse[1] = Command.DirectionalLight.Diffuse.y;
                DirectionalLight.Diffuse[2] = Command.DirectionalLight.Diffuse.z;
                DirectionalLight.Diffuse[3] = 1;
                DirectionalLight.Specular[0] = Command.DirectionalLight.Specular.x;
                DirectionalLight.Specular[1] = Command.DirectionalLight.Specular.y;
                DirectionalLight.Specular[2] = Command.DirectionalLight.Specular.z;
                DirectionalLight.Specular[3] = 1;
            }
            break;
            case RenderCommand_PointLight:
            {
                point_light& PointLight = RenderState.PointLightData.PointLights[RenderState.PointLightData.NumLights++];
                
                auto Pos = V * math::v4(Command.Position, 1.0f);
                
                PointLight.Position[0] = Pos.x;
                PointLight.Position[1] = Pos.y;
                PointLight.Position[2] = Pos.z;
                PointLight.Position[3] = 1.0f;
                
                PointLight.Ambient[0] = Command.PointLight.Ambient.x;
                PointLight.Ambient[1] = Command.PointLight.Ambient.y;
                PointLight.Ambient[2] = Command.PointLight.Ambient.z;
                
                PointLight.Diffuse[0] = Command.PointLight.Diffuse.x;
                PointLight.Diffuse[1] = Command.PointLight.Diffuse.y;
                PointLight.Diffuse[2] = Command.PointLight.Diffuse.z;
                
                PointLight.Specular[0] = Command.PointLight.Specular.x;
                PointLight.Specular[1] = Command.PointLight.Specular.y;
                PointLight.Specular[2] = Command.PointLight.Specular.z;
                
                PointLight.Constant = Command.PointLight.Constant;
                PointLight.Linear = Command.PointLight.Linear;
                PointLight.Quadratic = Command.PointLight.Quadratic;
            }
            break;
        }
    }
    
    UpdateLightingData(RenderState);
    Renderer.LightCommandCount = 0;
    Reset(&Renderer.LightCommands);
    
    for(i32 Index = 0; Index < Renderer.CommandCount; Index++)
    {
        const render_command& Command = *((render_command*)Renderer.Buffer.Base + Index);
        
        switch(Command.Type)
        {
            case RenderCommand_Line:
            {
                RenderLine(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Text:
            {
                RenderText(Command, RenderState, TempArena);
            }
            break;
            case RenderCommand_Rect:
            {
                RenderRect(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Sprite:
            {
                RenderSprite(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Model:
            {
                RenderModel(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Buffer:
            {
                RenderBuffer(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
        }
    }
    
    Renderer.CommandCount = 0;
    Reset(&Renderer.Buffer);
    
    RenderState.SpotlightData.NumLights = 0;
    RenderState.DirectionalLightData.NumLights = 0;
    RenderState.PointLightData.NumLights = 0;
}

static void Render(render_state& RenderState, renderer& Renderer, memory_arena* TempArena)
{
    LoadTextures(RenderState, Renderer);
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    Camera.ViewportWidth = RenderState.WindowWidth;
    Camera.ViewportHeight = RenderState.WindowHeight;
    
    RenderState.ScaleX = 2.0f / RenderState.WindowWidth;
    RenderState.ScaleY = 2.0f / RenderState.WindowHeight;
    
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState.FrameBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState.TextureColorBuffer);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glClearColor(0.5, 0.5, 0.5, 1.0f);
    
    //RenderGame(GameState);
    RenderCommands(RenderState, Renderer, TempArena);
    
    // Second pass
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.5, 0.5, 0.5, 1.0f);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(RenderState.FrameBufferVAO);
    UseShader(&RenderState.FrameBufferShader);
    
    SetFloatUniform(RenderState.FrameBufferShader.Program, "contrast", RenderState.Contrast);
    SetFloatUniform(RenderState.FrameBufferShader.Program, "brightness", RenderState.Brightness);
    SetIntUniform(RenderState.FrameBufferShader.Program, "ignoreLight",  true); // @Incomplete: Lighting
    SetMat4Uniform(RenderState.FrameBufferShader.Program,"P", Camera.ProjectionMatrix);
    SetMat4Uniform(RenderState.FrameBufferShader.Program,"V", Camera.ViewMatrix);
    SetVec2Uniform(RenderState.FrameBufferShader.Program, "screenSize", math::v2((r32)RenderState.WindowWidth,(r32)RenderState.WindowHeight));
    
    glUniform1i(RenderState.FrameBufferTex0Loc, 0);
    glUniform1i(RenderState.FrameBufferTex1Loc, 1);
    
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, RenderState.TextureColorBuffer);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, RenderState.LightingTextureColorBuffer);
    
    RenderState.BoundTexture = RenderState.LightingTextureColorBuffer;
    
    //Enable this if we don't do gamma correction in framebuffer shader
    //glEnable(GL_FRAMEBUFFER_SRGB);
    
    glDrawElements(GL_TRIANGLES, sizeof(RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0); 
    glActiveTexture(GL_TEXTURE0);
    
    glfwSwapBuffers(RenderState.Window);
    }