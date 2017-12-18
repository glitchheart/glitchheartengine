static void ErrorCallback(int Error, const char *Description)
{
    fprintf(stderr, "Error: %d - %s\n", Error, Description);
}

static void ShowMouseCursor(render_state& RenderState, b32 Show)
{
    if(Show)
    {
        glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
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
        GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)RenderState->ScaleFromWidth, (GLsizei)RenderState->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->TextureColorBuffer, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
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

[[noreturn]]
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

static GLint ShaderCompilationErrorChecking(const char* ShaderName, GLuint Shader)
{
    GLint IsCompiled = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &IsCompiled);
    if (!IsCompiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(Shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        GLchar* ErrorLog = PushTempSize(MaxLength, GLchar);
        
        glGetShaderInfoLog(Shader, MaxLength, &MaxLength, ErrorLog);
        
        DEBUG_PRINT("SHADER Compilation error - %s\n", ShaderName);
        DEBUG_PRINT("%s", ErrorLog);
        
        glDeleteShader(Shader); // Don't leak the shader.
    }
    return IsCompiled;
}

static GLuint LoadExtraShader(shader_data& ShaderData, render_state& RenderState)
{
    // @Incomplete: vertexAttribPointers?
    shader* Shader = &RenderState.ExtraShaders[RenderState.ExtraShaderIndex++];
    Shader->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(Shader->VertexShader, 1, &ShaderData.VertexShaderContent, NULL);
    glCompileShader(Shader->VertexShader);
    
    if (!ShaderCompilationErrorChecking("", Shader->VertexShader))
    {
        Shader->Program = 0;
        return GL_FALSE;
    }
    
    Shader->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(Shader->FragmentShader, 1, &ShaderData.FragmentShaderContent, NULL);
    glCompileShader(Shader->FragmentShader);
    
    if (!ShaderCompilationErrorChecking("", Shader->FragmentShader))
    {
        Shader->Program = 0;
        return GL_FALSE;
    }
    
    Shader->Program = glCreateProgram();
    
    glAttachShader(Shader->Program, Shader->VertexShader);
    glAttachShader(Shader->Program, Shader->FragmentShader);
    glLinkProgram(Shader->Program);
    
    return GL_TRUE;
}

static GLuint LoadShader(const char* FilePath, shader *Shd, memory_arena* PermArena)
{
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath, ".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString, PermArena);
    
    if(VertexText)
    {
        glShaderSource(Shd->VertexShader, 1, &VertexText, NULL);
        glCompileShader(Shd->VertexShader);
        
        if (!ShaderCompilationErrorChecking(FilePath, Shd->VertexShader))
        {
            Shd->Program = 0;
            return GL_FALSE;
        }
        
        Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        char* FragmentString = Concat(FilePath, ".frag");
        GLchar *FragmentText = LoadShaderFromFile(FragmentString, PermArena);
        
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
    return GL_FALSE;
}

static GLuint LoadVertexShader(const char* FilePath, shader *Shd, memory_arena* PermArena)
{
    Shd->Program = glCreateProgram();
    
    Shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* VertexString = Concat(FilePath,".vert");
    GLchar *VertexText = LoadShaderFromFile(VertexString, PermArena);
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

static GLuint LoadFragmentShader(const char* FilePath, shader *Shd, memory_arena* PermArena)
{
    Shd->Program = glCreateProgram();
    
    Shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* FragmentString = Concat(FilePath,".frag");
    GLchar *FragmentText = LoadShaderFromFile(FragmentString, PermArena);
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

static void InitializeFreeTypeFont(char* FontPath, int FontSize, FT_Library Library, render_font* Font)
{
    if(FT_New_Face(Library, FontPath, 0, &Font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }
    
    FT_Set_Pixel_Sizes(Font->Face, 0, (FT_UInt)FontSize);
    
    FT_Select_Charmap(Font->Face , ft_encoding_unicode);
    
    //Find the atlas width and height
    FT_GlyphSlot G = Font->Face->glyph;
    
    unsigned int W = 0;
    unsigned int H = 0;
    
    for(int I = 0; I < 255; I++)
    {
        if(FT_Load_Char(Font->Face, (FT_ULong)I, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Loading character %c failed!\n", I);
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)Font->AtlasWidth, (GLsizei)Font->AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    unsigned int X = 0;
    
    for(int I = 0; I < 255; I++) 
    {
        if(FT_Load_Char(Font->Face, (FT_ULong)I, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, (GLsizei)X, 0, (GLsizei)G->bitmap.width, (GLsizei)G->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
        Font->CharacterInfo[I].AX = (r32)(G->advance.x >> 6);
        Font->CharacterInfo[I].AY = (r32)(G->advance.y >> 6);
        
        Font->CharacterInfo[I].BW = (r32)G->bitmap.width;
        Font->CharacterInfo[I].BH = (r32)G->bitmap.rows;
        
        Font->CharacterInfo[I].BL = (r32)G->bitmap_left;
        Font->CharacterInfo[I].BT = (r32)G->bitmap_top;
        
        Font->CharacterInfo[I].TX = (r32)X / Font->AtlasWidth;
        
        X += G->bitmap.width;
    }
    
    glGenVertexArrays(1, &Font->VAO);
    glBindVertexArray(Font->VAO);
    
    glGenBuffers(1, &Font->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, Font->VBO);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void RegisterBuffers(render_state& RenderState, GLfloat* VertexBuffer, long VertexBufferSize, GLuint* IndexBuffer, i32 IndexBufferCount, long IndexBufferSize, b32 HasNormals, b32 HasUVs, b32 Skinned, i32 BufferHandle = -1)
{
    buffer* Buffer = &RenderState.Buffers[BufferHandle == -1 ? RenderState.BufferCount : BufferHandle];
    
    Buffer->VertexBufferSize = (GLint)VertexBufferSize;
    Buffer->IndexBufferCount = (GLint)IndexBufferCount;
    
    if(Buffer->VAO == 0)
        glGenVertexArrays(1, &Buffer->VAO);
    
    glBindVertexArray(Buffer->VAO);
    
    if(Buffer->VBO == 0)
        glGenBuffers(1, &Buffer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, Buffer->VBO);
    glBufferData(GL_ARRAY_BUFFER, VertexBufferSize, VertexBuffer, GL_STATIC_DRAW);
    
    i32 BoneInfoSize = Skinned ? 8 : 0;
    
    if(HasNormals && HasUVs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(8 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(12 * sizeof(GLfloat)));
    }
    else if(HasNormals)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((6 + BoneInfoSize) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)((6 + BoneInfoSize) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone count
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(7 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(11 * sizeof(GLfloat)));
    }
    else if(HasUVs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((5 + BoneInfoSize) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (GLsizei)((5 + BoneInfoSize) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone count
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(5 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(10 * sizeof(GLfloat)));
    }
    else
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)(3 + BoneInfoSize * sizeof(GLfloat)), 0);
        
        // Bone count
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(4 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + BoneInfoSize) * sizeof(GLfloat)), (void*)(8 * sizeof(GLfloat)));
    }
    
    glGenBuffers(1, &Buffer->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Buffer->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferSize, IndexBuffer, GL_STATIC_DRAW);
    
    if(BufferHandle == -1)
        RenderState.BufferCount++;
    
    glBindVertexArray(0);
}

static void RegisterVertexBuffer(render_state& RenderState, GLfloat* BufferData, i32 Size, Shader_Type ShaderType, memory_arena* PermArena, i32 BufferHandle = -1)
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)(sizeof(GLfloat) * Size), BufferData, GL_STATIC_DRAW);
    
    if(!RenderState.Shaders[ShaderType].Loaded)
    {
        LoadShader(ShaderPaths[ShaderType], &RenderState.Shaders[ShaderType], PermArena);
    }
    else
        UseShader(&RenderState.Shaders[ShaderType]);
    
    auto PositionLocation = (GLuint)glGetAttribLocation(RenderState.TileShader.Program, "pos");
    auto TexcoordLocation = (GLuint)glGetAttribLocation(RenderState.TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PositionLocation);
    glVertexAttribPointer(PositionLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexcoordLocation);
    glVertexAttribPointer(TexcoordLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if(BufferHandle == -1)
        RenderState.BufferCount++;
}

static void RenderSetup(render_state *RenderState, memory_arena* PermArena)
{
    // @Cleanup: Not sure if a fallback is a good way of dealing with this
    if(RenderState->ScaleFromWidth == 0 || RenderState->ScaleFromHeight == 0)
    {
        RenderState->ScaleFromWidth = RenderState->WindowWidth;
        RenderState->ScaleFromHeight = RenderState->WindowHeight;
    }
    
    if(FT_Init_FreeType(&RenderState->FTLibrary)) 
    {
        fprintf(stderr, "Could not init freetype library\n");
    }
    
    RenderState->FontCount = 0;
    
    // Framebuffer 1
    glGenFramebuffers(1, &RenderState->FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->FrameBuffer);
    
    glGenTextures(1, &RenderState->TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->TextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->ScaleFromWidth, RenderState->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, RenderState->TextureColorBuffer, 0
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        DEBUG_PRINT("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    GLuint DepthBuffer;
    glGenRenderbuffers(1, &DepthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, RenderState->ScaleFromWidth, RenderState->ScaleFromHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DepthBuffer);
    
    glGenFramebuffers(1, &RenderState->LightingFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState->LightingFrameBuffer);
    
    glGenTextures(1, &RenderState->LightingTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, RenderState->LightingTextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, RenderState->ScaleFromWidth, RenderState->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->FrameBufferVertices, GL_DYNAMIC_DRAW);
    
    RenderState->FrameBufferShader.Type = Shader_FrameBuffer;
    
    LoadShader(ShaderPaths[Shader_FrameBuffer], &RenderState->FrameBufferShader, PermArena);
    
    auto PosLoc = (GLuint)glGetAttribLocation(RenderState->FrameBufferShader.Program, "pos");
    auto TexLoc = (GLuint)glGetAttribLocation(RenderState->FrameBufferShader.Program, "texcoord");
    
    glEnableVertexAttribArray(PosLoc);
    glVertexAttribPointer(PosLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(TexLoc);
    glVertexAttribPointer(TexLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    RenderState->FrameBufferTex0Loc = (GLuint)glGetUniformLocation(RenderState->FrameBufferShader.Program, "tex");
    RenderState->FrameBufferTex1Loc = (GLuint)glGetUniformLocation(RenderState->FrameBufferShader.Program, "lightingTex");
    
    glGenBuffers(1, &RenderState->QuadIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RenderState->QuadIndices), RenderState->QuadIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    //Sprite
    glGenVertexArrays(1, &RenderState->SpriteVAO);
    glBindVertexArray(RenderState->SpriteVAO);
    glGenBuffers(1, &RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureShader.Type = Shader_Texture;
    LoadShader(ShaderPaths[Shader_Texture], &RenderState->TextureShader, PermArena);
    
    auto PositionLocation = (GLuint)glGetAttribLocation(RenderState->TextureShader.Program, "pos");
    auto TexcoordLocation = (GLuint)glGetAttribLocation(RenderState->TextureShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureShader.Type = Shader_Spritesheet;
    LoadShader(ShaderPaths[Shader_Spritesheet], &RenderState->SpritesheetShader, PermArena);
    
    PositionLocation = (GLuint)glGetAttribLocation(RenderState->SpritesheetShader.Program, "pos");
    TexcoordLocation = (GLuint)glGetAttribLocation(RenderState->SpritesheetShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->ErrorShaderSprite.Type = Shader_ErrorSprite;
    LoadShader(ShaderPaths[Shader_ErrorSprite], &RenderState->ErrorShaderSprite, PermArena);
    
    PositionLocation = (GLuint)glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "pos");
    TexcoordLocation = (GLuint)glGetAttribLocation(RenderState->ErrorShaderSprite.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    
    RenderState->UISpriteShader.Type = Shader_UISprite;
    LoadShader(ShaderPaths[Shader_UISprite], &RenderState->UISpriteShader, PermArena);
    
    PositionLocation = (GLuint)glGetAttribLocation(RenderState->UISpriteShader.Program, "pos");
    TexcoordLocation = (GLuint)glGetAttribLocation(RenderState->UISpriteShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->SpriteQuadVerticesSize, RenderState->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //error shader
    RenderState->ErrorShaderUI.Type = Shader_ErrorUI;
    LoadShader(ShaderPaths[Shader_ErrorUI], &RenderState->ErrorShaderUI, PermArena);
    
    PositionLocation = (GLuint)glGetAttribLocation(RenderState->ErrorShaderUI.Program, "pos");
    TexcoordLocation = (GLuint)glGetAttribLocation(RenderState->ErrorShaderUI.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->TileQuadVerticesSize, RenderState->TileQuadVertices, GL_STATIC_DRAW);
    
    RenderState->TileShader.Type = Shader_Tile;
    LoadShader(ShaderPaths[Shader_Tile], &RenderState->TileShader, PermArena);
    RenderState->TileShader.Loaded = true;
    
    auto PositionLocation2 = (GLuint)glGetAttribLocation(RenderState->TileShader.Program, "pos");
    auto TexcoordLocation2 = (GLuint)glGetAttribLocation(RenderState->TileShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->NormalQuadVerticesSize, RenderState->NormalQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->RectShader.Type = Shader_Rect;
    LoadShader(ShaderPaths[Shader_Rect], &RenderState->RectShader, PermArena);
    
    auto PositionLocation3 = (GLuint)glGetAttribLocation(RenderState->RectShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &RenderState->TextureRectVAO);
    glBindVertexArray(RenderState->TextureRectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->SpriteQuadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->TextureRectShader.Type = Shader_TextureRect;
    LoadShader(ShaderPaths[Shader_TextureRect], &RenderState->TextureRectShader, PermArena);
    
    PositionLocation2 = (GLuint)glGetAttribLocation(RenderState->TextureRectShader.Program, "pos");
    TexcoordLocation2 = (GLuint)glGetAttribLocation(RenderState->TextureRectShader.Program, "texcoord");
    
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
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->WireframeQuadVerticesSize, RenderState->WireframeQuadVertices, GL_DYNAMIC_DRAW);
    
    RenderState->RectShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader, PermArena);
    
    PositionLocation3 = (GLuint)glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    //
    
    glGenVertexArrays(1, &RenderState->WireframeCubeVAO);
    glBindVertexArray(RenderState->WireframeCubeVAO);
    glGenBuffers(1, &RenderState->WireframeCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->WireframeCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, RenderState->WireframeCubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &RenderState->CubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->CubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * CUBE_INDICES, RenderState->WireframeCubeIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //
    
    glGenVertexArrays(1, &RenderState->IsometricVAO);
    glBindVertexArray(RenderState->IsometricVAO);
    glGenBuffers(1, &RenderState->IsometricQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->IsometricQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)RenderState->WireframeQuadVerticesSize, RenderState->IsometricQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, RenderState->QuadIndexBuffer);
    
    RenderState->RectShader.Type = Shader_Wireframe;
    LoadShader(ShaderPaths[Shader_Wireframe], &RenderState->WireframeShader, PermArena);
    
    PositionLocation3 = (GLuint)glGetAttribLocation(RenderState->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(PositionLocation3);
    glVertexAttribPointer(PositionLocation3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    // Lines
    glGenVertexArrays(1, &RenderState->PrimitiveVAO);
    glBindVertexArray(RenderState->PrimitiveVAO);
    glGenBuffers(1, &RenderState->PrimitiveVBO);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState->PrimitiveVBO);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glBindVertexArray(0);
    
    // Passthrough
    LoadShader(ShaderPaths[Shader_Passthrough], &RenderState->PassthroughShader, PermArena);
    glGenVertexArrays(1, &RenderState->PassthroughVAO);
    glBindVertexArray(RenderState->PassthroughVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(r32), 0);
    glBindVertexArray(0);
    
    //font
    RenderState->StandardFontShader.Type = Shader_StandardFont;
    LoadShader(ShaderPaths[Shader_StandardFont], &RenderState->StandardFontShader, PermArena);
    
    // Light sources
    RenderState->LightSourceShader.Type = Shader_LightSource;
    LoadShader(ShaderPaths[Shader_LightSource], &RenderState->LightSourceShader, PermArena);
    
    RenderState->SimpleModelShader.Type = Shader_SimpleModel;
    LoadShader(ShaderPaths[Shader_SimpleModel], &RenderState->SimpleModelShader, PermArena);
    
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
    if(P)
    {
        memcpy(P, &RenderState.SpotlightData, sizeof(spotlight_data));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.DirectionalLightUBO);
    P = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if(P)
    {
        memcpy(P, &RenderState.DirectionalLightData, sizeof(directional_light_data));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.PointLightUBO);
    P = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if(P)
    {
        memcpy(P, &RenderState.PointLightData, sizeof(point_light_data));
    }
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

static void LoadExtraShaders(render_state& RenderState, renderer& Renderer)
{
    for(i32 Index = RenderState.ExtraShaderIndex; Index < Renderer.ShaderCount; Index++)
    {
        LoadExtraShader(Renderer.ShaderData[Index], RenderState);
    }
}

static void CreateOpenGLWindow(render_state& RenderState, Window_Mode WindowMode, const char* Title, i32 Width, i32 Height)
{
    GLFWmonitor* Monitor = glfwGetPrimaryMonitor();
    i32 ScreenWidth = Width;
    i32 ScreenHeight = Height;
    
    const GLFWvidmode* Mode = glfwGetVideoMode(Monitor);
    RenderState.WindowMode = WindowMode;
    RenderState.WindowTitle = PushString(&RenderState.Arena, Title);
    
    if(WindowMode == FM_Borderless)
    {
        glfwWindowHint(GLFW_RED_BITS, Mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, Mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, Mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, Mode->refreshRate);
        ScreenWidth = Mode->width;
        ScreenHeight = Mode->height;
    }
    
    if(WindowMode == FM_Windowed)
    {
        Monitor = NULL;
    }
    
    RenderState.Window = glfwCreateWindow(ScreenWidth, ScreenHeight, Title, Monitor, 
                                          NULL);
    
    //center window on screen (windowed?)
    
    if(WindowMode == FM_Windowed)
    {
        int FrameBufferWidth, FrameBufferHeight;
        
        glfwGetFramebufferSize(RenderState.Window, &FrameBufferWidth, &FrameBufferHeight);
        glfwSetWindowPos(RenderState.Window, Mode->width / 2 - Width / 2, Mode->height / 2 - Height / 2);
    }
}

static void InitializeOpenGL(render_state& RenderState, renderer& Renderer, config_data* ConfigData, memory_arena* PermArena)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwSetErrorCallback(ErrorCallback);
    
    RenderState.ScaleFromWidth = ConfigData->ScaleFromWidth;
    RenderState.ScaleFromHeight = ConfigData->ScaleFromHeight;
    
    //@Incomplete: Figure something out here. Ask for compatible version etc
#ifdef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif __linux
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    RenderState.Contrast = ConfigData->Contrast;
    RenderState.Brightness = ConfigData->Brightness;
    
    CreateOpenGLWindow(RenderState, ConfigData->Fullscreen, Concat(Concat(ConfigData->Title, " "), ConfigData->Version), ConfigData->ScreenWidth, ConfigData->ScreenHeight);
    Renderer.WindowMode = RenderState.WindowMode;
    
    if (!RenderState.Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    glfwSetFramebufferSizeCallback(RenderState.Window, FramebufferSizeCallback);
    
    glfwMakeContextCurrent(RenderState.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(RenderState.Window, &RenderState.WindowWidth, &RenderState.WindowHeight);
    glViewport(0, 0, RenderState.WindowWidth, RenderState.WindowHeight);
    glDisable(GL_DITHER);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    glDepthFunc(GL_LESS);
    
    DEBUG_PRINT("%s\n", glGetString(GL_VERSION));
    DEBUG_PRINT("Shading language supported: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    DEBUG_PRINT("Glad Version: %d.%d\n", GLVersion.major, GLVersion.minor);
    
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
    
    Renderer.ShouldClose = false;
    RenderSetup(&RenderState, PermArena);
}

static void ReloadVertexShader(Shader_Type Type, render_state* RenderState, memory_arena* PermArena)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].VertexShader);
    LoadVertexShader(ShaderPaths[Type], &RenderState->Shaders[Type], PermArena);
}

static void ReloadFragmentShader(Shader_Type Type, render_state* RenderState, memory_arena* PermArena)
{
    glDeleteProgram(RenderState->Shaders[Type].Program);
    glDeleteShader(RenderState->Shaders[Type].FragmentShader);
    LoadFragmentShader(ShaderPaths[Type], &RenderState->Shaders[Type], PermArena);
    
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

static void ReloadAssets(render_state& RenderState, asset_manager* AssetManager, memory_arena* PermArena)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(AssetManager->DirtyVertexShaderIndices[i] == 1)
        {
            DEBUG_PRINT("Reloading vertex shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            ReloadVertexShader((Shader_Type)i, &RenderState, PermArena);
            AssetManager->DirtyVertexShaderIndices[i] = 0;
        }
        
        if(AssetManager->DirtyFragmentShaderIndices[i] == 1)
        {
            DEBUG_PRINT("Reloading fragment shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            ReloadFragmentShader((Shader_Type)i, &RenderState, PermArena);
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

void SetVec4ArrayUniform(GLuint ShaderHandle, const char *UniformName, math::v4* Value, u32 Length)
{
    glUniform4fv(glGetUniformLocation(ShaderHandle, UniformName), (GLsizei)Length, (GLfloat*)&Value[0]);
}

void SetFloatArrayUniform(GLuint ShaderHandle, const char *UniformName, r32* Value, u32 Length)
{
    glUniform1fv(glGetUniformLocation(ShaderHandle, UniformName), (GLsizei)Length, (GLfloat*)&Value[0]);
}

static void RenderLine(render_state& RenderState, math::v4 Color, math::v3 Start, math::v3 End, math::m4 ProjectionMatrix = math::m4(), math::m4 ViewMatrix = math::m4(), r32 LineWidth = 1.0f, b32 IsUI = false)
{
    if(IsUI)
    {
        Start.x *= RenderState.ScaleX;
        Start.x -= 1;
        Start.y *= RenderState.ScaleY;
        Start.y -= 1;
        End.x *= RenderState.ScaleX;
        End.x -= 1;
        End.y *= RenderState.ScaleY;
        End.y -= 1;
        Start.z = 0.0f;
        End.z = 0.0f;
    }
    
    glLineWidth(LineWidth);
    glBindBuffer(GL_ARRAY_BUFFER, RenderState.PrimitiveVBO);
    
    GLfloat Points[6] = {Start.x, Start.y, Start.z, End.x, End.y, End.z};
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);
    
    glBindVertexArray(RenderState.PrimitiveVAO);
    
    auto& Shader = RenderState.PassthroughShader;
    UseShader(&Shader);
    
    SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
    SetMat4Uniform(Shader.Program, "View", ViewMatrix);
    
    SetMat4Uniform(Shader.Program, "Model", math::m4(1.0f));
    
    SetVec4Uniform(Shader.Program, "Color", Color);
    
    glDrawArrays(GL_LINES, 0, 6);
    glLineWidth(1.0f);
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
        Points[PointIndex++] = math::Cos(Radians * Radius);
        Points[PointIndex++] = math::Sin(Radians * Radius);
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

static void RenderQuad(Render_Mode Mode, render_state& RenderState, math::v4 Color, math::v3 Position, math::v3 Size, math::quat Orientation, i32 ShaderHandle, shader_attribute* ShaderAttributes, i32 ShaderAttributeCount, b32 IsUI = true, i32 TextureHandle = 0, b32 ForAnimation = false, math::v2 TextureSize = math::v2(), math::v2i FrameSize = math::v2i(), math::v2 TextureOffset = math::v2(), math::m4 ProjectionMatrix = math::m4(), math::m4 ViewMatrix = math::m4())
{
    if(IsUI)
    {
        Position.x *= RenderState.ScaleX;
        Position.x -= 1;
        Position.y *= RenderState.ScaleY;
        Position.y -= 1;
        
        Size.x *= RenderState.ScaleX;
        Size.y *= RenderState.ScaleY;
    }
    
    switch(Mode)
    {
        case Render_Fill:
        {
            auto Shader = RenderState.RectShader;
            
            if(TextureHandle > 0)
            {
                glBindVertexArray(RenderState.TextureRectVAO);
            }
            else
            {
                glBindVertexArray(RenderState.RectVAO);
            }
            
            if(TextureHandle > 0)
            {
                glBindTexture(GL_TEXTURE_2D, (GLuint)TextureHandle);
                
                if(ForAnimation || (TextureOffset.X >= 0.0f && TextureOffset.Y >= 0.0f))
                    Shader = RenderState.SpritesheetShader;
                else
                    Shader = RenderState.TextureRectShader;
                
                RenderState.BoundTexture = (GLuint)TextureHandle;
            }
            
            if(RenderState.CurrentExtraShader != -1)
            {
                Shader = RenderState.ExtraShaders[RenderState.CurrentExtraShader];
            }
            
            if(ShaderHandle != -1)
            {
                Shader = RenderState.ExtraShaders[ShaderHandle];
            }
            
            UseShader(&Shader);
            
            //draw upper part
            math::m4 Model(1.0f);
            Model = math::Scale(Model, Size);
            
            // @Incomplete: We should be using an origin point for this
            //Model = math::Translate(Model, math::v3(-Size.x / 2.0f, -Size.y / 2.0f, -Size.z / 2.0f));
            
            Model = ToMatrix(Orientation) * Model;
            
            // @Incomplete: We should be using an origin point for this
            //Model = math::Translate(Model, math::v3(Size.x / 2.0f, Size.y / 2.0f, Size.z / 2.0f));
            
            Model = math::Translate(Model, Position);
            
            if(!IsUI)
            {
                SetMat4Uniform(Shader.Program, "Projection", ProjectionMatrix);
                SetMat4Uniform(Shader.Program, "View", ViewMatrix);
            }
            
            SetFloatUniform(Shader.Program, "isUI", (r32)IsUI);
            SetMat4Uniform(Shader.Program, "M", Model);
            SetVec4Uniform(Shader.Program, "color", Color);
            
            if(TextureOffset.X >= 0.0f && TextureOffset.Y >= 0.0f)
            {
                SetVec2Uniform(Shader.Program, "textureOffset", TextureOffset);
                SetVec2Uniform(Shader.Program, "textureSize", TextureSize);
                SetVec2Uniform(Shader.Program, "frameSize", math::v2((r32)FrameSize.x, (r32)FrameSize.y));
            }
            
            if(ForAnimation)
            {
                
                SetVec2Uniform(Shader.Program, "textureSize", TextureSize);
                SetVec2Uniform(Shader.Program, "frameSize", math::v2((r32)FrameSize.x, (r32)FrameSize.y));
            }
            
            if(RenderState.CurrentExtraShader != -1 || ShaderHandle != -1)
            {
                //shader_attribute* Attributes = ShaderHandle != -1 ? ShaderAttributes : RenderState.ShaderAttributes;
                i32 AttributeCount = ShaderHandle != -1 ? ShaderAttributeCount : RenderState.ShaderAttributeCount;
                
                for(i32 Index = 0; Index < AttributeCount; Index++)
                {
                    shader_attribute& Attribute = ShaderAttributes[Index];
                    switch(Attribute.Type)
                    {
                        case Attribute_Float:
                        {
                            SetFloatUniform(Shader.Program, Attribute.Name, Attribute.FloatVar);
                        }
                        break;
                        case Attribute_Float2:
                        {
                            SetVec2Uniform(Shader.Program, Attribute.Name, Attribute.Float2Var);
                        }
                        break;
                        case Attribute_Float3:
                        {
                            SetVec3Uniform(Shader.Program, Attribute.Name, Attribute.Float3Var);
                        }
                        break;
                        case Attribute_Float4:
                        {
                            SetVec4Uniform(Shader.Program, Attribute.Name, Attribute.Float4Var);
                        }
                        break;
                        case Attribute_Integer:
                        {
                            SetIntUniform(Shader.Program, Attribute.Name, Attribute.IntegerVar);
                        }
                        break;
                        case Attribute_Boolean:
                        {
                            SetIntUniform(Shader.Program, Attribute.Name, Attribute.BooleanVar);
                        }
                        break;
                        case Attribute_Matrix4:
                        {
                            SetMat4Uniform(Shader.Program, Attribute.Name, Attribute.Matrix4Var);
                        }
                        break;
                    }
                }
            }
            
            glDrawElements(GL_TRIANGLES, sizeof(RenderState.QuadIndices), GL_UNSIGNED_INT, (void*)0);
        }
        break;
        case Render_Outline:
        {
            math::m4 Model(1.0f);
            
            Model = math::Scale(Model, Size);
            Model = math::Translate(Model, Position);
            Model = math::ToMatrix(Orientation) * Model;
            
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
    size_t Count;
    
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
                       Alignment Alignment = Alignment_Left,  b32 AlignCenterY = true) 
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
    
    point* Coords = PushTempArray(6 * strlen(Text), point);
    
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
        if(!(i32)Font.CharacterInfo[*P].BW || !(i32)Font.CharacterInfo[*P].BH)
            continue;
        
        Coords[N++] = { X2, -Y2, Font.CharacterInfo[*P].TX, 0 };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, 0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth,  0 };
        Coords[N++] = { X2, -Y2 - H, Font.CharacterInfo[*P].TX, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
        Coords[N++] = { X2 + W, -Y2 - H, Font.CharacterInfo[*P].TX + Font.CharacterInfo[*P].BW / Font.AtlasWidth, Font.CharacterInfo[*P].BH / Font.AtlasHeight };
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(6 * strlen(Text) * sizeof(point)), Coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, N);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void RenderWireframeCube(const render_command& Command, render_state& RenderState, math::m4 Projection = math::m4(1.0f), math::m4 View = math::m4(1.0f))
{
    glBindVertexArray(RenderState.WireframeCubeVAO);
    
    glLineWidth(Command.WireframeCube.LineWidth);
    auto Shader = RenderState.PassthroughShader;
    UseShader(&Shader);
    
    math::m4 Model = math::m4(1.0f);
    Model = math::Scale(Model, Command.Scale);
    Model = math::Rotate(Model, Command.Orientation);
    Model = math::Translate(Model, Command.Position);
    
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetMat4Uniform(Shader.Program, "Projection", Projection);
    SetMat4Uniform(Shader.Program, "View", View);
    SetVec4Uniform(Shader.Program, "Color", Command.WireframeCube.Color);
    
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(GLuint)));
    
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

//@Deprecated: Uses old-style rendering
/*
void RenderConsole(render_state& RenderState, console* Console)
{
    glBindVertexArray(RenderState.RectVAO);
    
    r32 PercentAnimated = 1.0f + 1.0f - (r32)Console->CurrentTime / (r32)Console->TimeToAnimate;
    
    //draw upper part
    RenderQuad(Render_Fill, RenderState, math::v4(0.0f, 0.4f, 0.3f, 0.6f), math::v3(0.0f, (r32)RenderState.WindowHeight * 0.77f * PercentAnimated, 0.0f), math::v3((r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.23f, 0.0f), math::v3(), -1, 0, 0);
    
    //draw lower bar
    RenderQuad(Render_Fill, RenderState, math::v4(0.0f, 0.2f, 0.2f, 0.6f), math::v3(0.0f, (r32)RenderState.WindowHeight * 0.77f * PercentAnimated, 0.0f), math::v3((r32)RenderState.WindowWidth, 0.0f, 0.0f), math::v3(), -1, 0L, 0);  
    
    GLfloat TimeValue = (r32)glfwGetTime();
    GLfloat AlphaValue = (r32)((sin(TimeValue * 4) / 2) + 0.5f);
    r32 Width;
    r32 Height;
    MeasureText(RenderState.Fonts[0], &Console->Buffer[0], &Width, &Height);
    
    //draw cursor
    RenderQuad(Render_Fill, RenderState, math::v4(AlphaValue, 1, AlphaValue, 1), math::v3(5 / 1920.0f * (r32)RenderState.WindowWidth + Width, RenderState.WindowHeight * 0.77f * PercentAnimated, 0.0f), math::v3(10, 20, 0.0f), math::v3(), -1, 0, 0);
    
    RenderText(RenderState, RenderState.Fonts[0], math::v4(0, 0.8, 0, 1),  &Console->Buffer[0],  5 / 1920.0f * (r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.775f * PercentAnimated);
    
    math::v4 Color;
    
    for(i32 Index = 0; Index < HISTORY_BUFFER_LINES; Index++)
    {
        if(Index % 2 != 0)
            Color = math::v4(0, 1, 0, 1);
        else
            Color = math::v4(1, 1, 1, 1);
            
        RenderText(RenderState, RenderState.Fonts[0], Color, &Console->HistoryBuffer[Index][0], 5 / 1920.0f * (r32)RenderState.WindowWidth, (r32)RenderState.WindowHeight * 0.78f * PercentAnimated + (Index + 1) * 20 * PercentAnimated);
    }
}*/

static void RenderLine(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    RenderLine(RenderState, Command.Line.Color, Command.Line.Point1, Command.Line.Point2, Projection, View, Command.Line.LineWidth);
}

static void RenderText(const render_command& Command, render_state& RenderState)
{
    render_font RenderFont;
    RenderFont = RenderState.Fonts[Command.Text.FontHandle];
    RenderText(RenderState, RenderFont, Command.Text.Color, Command.Text.Text, Command.Text.Position.x, Command.Text.Position.y, Command.Text.Alignment);
}

static void RenderQuad(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    if(Command.IsUI)
    {
        auto Handle = Command.Quad.TextureHandle != -1 ? RenderState.TextureArray[Command.Quad.TextureHandle].TextureHandle : 0;
        RenderQuad(Command.Quad.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Quad.Color, 
                   Command.Position,
                   Command.Scale,
                   Command.Orientation,
                   Command.ShaderHandle,
                   Command.ShaderAttributes,
                   Command.ShaderAttributeCount,
                   Command.IsUI,
                   (i32)Handle,
                   Command.Quad.ForAnimation,
                   Command.Quad.TextureSize,
                   Command.Quad.FrameSize,
                   Command.Quad.TextureOffset);
    }
    else
    {
        auto Handle = Command.Quad.TextureHandle != -1 ? RenderState.TextureArray[Command.Quad.TextureHandle].TextureHandle : 0;
        RenderQuad(Command.Quad.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Quad.Color, 
                   Command.Position,
                   Command.Scale,
                   Command.Orientation,
                   Command.ShaderHandle,
                   Command.ShaderAttributes,
                   Command.ShaderAttributeCount,
                   Command.IsUI,
                   (i32)Handle,
                   Command.Quad.ForAnimation,
                   Command.Quad.TextureSize,
                   Command.Quad.FrameSize,
                   Command.Quad.TextureOffset,
                   Projection, 
                   View);
    }
}

static void RenderModel(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    buffer Buffer = RenderState.Buffers[Command.Model.BufferHandle];
    glBindVertexArray(Buffer.VAO);
    
    for(i32 MeshIndex = 0; MeshIndex < Command.Model.MeshCount; MeshIndex++)
    {
        mesh_data MeshData = Command.Model.Meshes[MeshIndex];
        material Material = Command.Model.Materials[MeshData.MaterialIndex];
        
        if(Material.DiffuseTexture.HasData)
        {
            texture Texture = RenderState.TextureArray[Material.DiffuseTexture.TextureHandle];
            if(RenderState.BoundTexture != Texture.TextureHandle)
            {
                glBindTexture(GL_TEXTURE_2D, Texture.TextureHandle);
                RenderState.BoundTexture = Texture.TextureHandle;
            }
        }
        
        shader Shader = {};
        
        if(Command.Model.Type == Model_Skinned)
        {
            Shader = RenderState.SimpleModelShader;
            UseShader(&Shader);
            
            for(i32 Index = 0; Index < Command.Model.BoneCount; Index++)
            {
                char SBuffer[20];
                sprintf(SBuffer, "bones[%d]", Index);
                SetMat4Uniform(Shader.Program, SBuffer, Command.Model.BoneTransforms[Index]);
            }
        }
        else
        {
            // @Incomplete: We need a shader that isn't using the bone data
        }
        
        math::m4 Model(1.0f);
        Model = math::Scale(Model, Command.Scale);
        Model = math::Rotate(Model, Command.Orientation);
        Model = math::Translate(Model, Command.Position);
        
        math::m4 NormalMatrix = math::Transpose(math::Inverse(View * Model));
        
        SetMat4Uniform(Shader.Program, "normalMatrix", NormalMatrix);
        SetMat4Uniform(Shader.Program, "projection", Projection);
        SetMat4Uniform(Shader.Program, "view", View);
        SetMat4Uniform(Shader.Program, "model", Model);
        SetVec4Uniform(Shader.Program, "color", math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        SetIntUniform(Shader.Program, "hasUVs", Material.DiffuseTexture.HasData);
        
        glDrawElements(GL_TRIANGLES, Buffer.IndexBufferCount, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}

static void RenderBuffer(const render_command& Command, render_state& RenderState, math::m4 Projection, math::m4 View)
{
    buffer Buffer = RenderState.Buffers[Command.Buffer.BufferHandle];
    
    glBindVertexArray(Buffer.VAO);
    u32 TextureHandle = Command.Buffer.TextureHandle != -1 ? RenderState.TextureArray[Command.Buffer.TextureHandle].TextureHandle : 0;
    
    if (TextureHandle != 0 && RenderState.BoundTexture != TextureHandle)
    {
        glBindTexture(GL_TEXTURE_2D, TextureHandle);
        RenderState.BoundTexture = TextureHandle;
    }
    
    math::v3 Position = Command.Position;
    math::v3 Size = Command.Scale;
    
    if(Command.IsUI)
    {
        Position.x *= RenderState.ScaleX;
        Position.x -= 1;
        Position.y *= RenderState.ScaleY;
        Position.y -= 1;
        
        Size.x *= RenderState.ScaleX;
        Size.y *= RenderState.ScaleY;
    }
    
    
    auto Shader = RenderState.TileShader;
    UseShader(&Shader);
    
    math::m4 Model(1.0f);
    Model = math::Scale(Model, Size);
    Model = math::Translate(Model, Position);
    
    //Model = math::YRotate(Command.Rotation.y) * Model;
    //Model = math::XRotate(Command.Rotation.x) * Model;
    //Model = math::ZRotate(Command.Rotation.z) * Model;
    
    SetFloatUniform(Shader.Program, "isUI", (r32)Command.IsUI);
    SetMat4Uniform(Shader.Program, "Projection", Projection);
    SetMat4Uniform(Shader.Program, "View", View);
    SetMat4Uniform(Shader.Program, "Model", Model);
    SetVec4Uniform(Shader.Program, "Color", math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
    
    glDrawArrays(
        GL_TRIANGLES, 0, Buffer.VertexBufferSize / 3);
    glBindVertexArray(0);
}

static void LoadFont(render_state& RenderState, char* Path, i32 Size)
{
    InitializeFreeTypeFont(Path, Size, RenderState.FTLibrary, &RenderState.Fonts[RenderState.FontCount++]);
}


static void RenderCommands(render_state& RenderState, renderer& Renderer, memory_arena* PermArena)
{
    for(i32 Index = RenderState.BufferCount; Index < Renderer.BufferCount; Index++)
    {
        buffer_data Data = Renderer.Buffers[Index];
        
        if(Data.IndexBufferCount == 0)
        {
            RegisterVertexBuffer(RenderState, Data.VertexBuffer, (i32)Data.VertexBufferSize, Data.ShaderType, PermArena, Data.ExistingHandle);
        }
        else
        {
            RegisterBuffers(RenderState, Data.VertexBuffer, Data.VertexBufferSize, Data.IndexBuffer, Data.IndexBufferCount, Data.IndexBufferSize, Data.HasNormals, Data.HasUVs, Data.Skinned, Data.ExistingHandle);
            
        }
    }
    
    for(i32 Index = 0; Index < Renderer.UpdatedBufferHandleCount; Index++)
    {
        buffer_data Data = Renderer.Buffers[Renderer.UpdatedBufferHandles[Index]];
        if(Data.IndexBufferCount == 0)
        {
            RegisterVertexBuffer(RenderState, Data.VertexBuffer, (i32)Data.VertexBufferSize, Data.ShaderType, PermArena, Data.ExistingHandle);
        }
        else
        {
            RegisterBuffers(RenderState, Data.VertexBuffer, Data.VertexBufferSize, Data.IndexBuffer, Data.IndexBufferCount, Data.IndexBufferSize, Data.HasNormals, Data.HasUVs, Data.Skinned, Data.ExistingHandle);
            
        }
    }
    Renderer.UpdatedBufferHandleCount = 0;
    
    for(i32 Index = RenderState.FontCount; Index < Renderer.FontCount; Index++)
    {
        font_data Data = Renderer.Fonts[Index];
        
        LoadFont(RenderState, Data.Path, Data.Size);
    }
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    auto& V = Camera.ViewMatrix;
    
    for(i32 Index = 0; Index < Renderer.LightCommandCount; Index++)
    {
        const render_command& Command =*((render_command*)Renderer.LightCommands.CurrentBlock->Base + Index);
        
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
            default:
            break;
        }
    }
    
    UpdateLightingData(RenderState);
    Renderer.LightCommandCount = 0;
    RenderState.SpotlightData.NumLights = 0;
    RenderState.DirectionalLightData.NumLights = 0;
    RenderState.PointLightData.NumLights = 0;
    Clear(&Renderer.LightCommands);
    
    glEnable(GL_DEPTH_TEST);
    for(i32 Index = 0; Index < Renderer.CommandCount; Index++)
    {
        const render_command& Command = *((render_command*)Renderer.Commands.CurrentBlock->Base + Index);
        
        switch(Command.Type)
        {
            case RenderCommand_Line:
            {
                RenderLine(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Text:
            {
                RenderText(Command, RenderState);
            }
            break;
            case RenderCommand_Quad:
            {
                RenderQuad(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Sprite:
            {
                //RenderSprite(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Model:
            {
                RenderModel(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
                
            }
            break;
            case RenderCommand_Buffer:
            {
                RenderBuffer(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_WireframeCube:
            {
                RenderWireframeCube(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_DepthTest:
            {
                if(Command.DepthTest.On)
                {
                    glEnable(GL_DEPTH_TEST);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            break;
            case RenderCommand_ShaderStart:
            {
                RenderState.CurrentExtraShader = Command.Shader.Handle;
                RenderState.ShaderAttributes = Command.Shader.Attributes;
                RenderState.ShaderAttributeCount = Command.Shader.AttributeCount;
            }
            break;
            case RenderCommand_ShaderEnd:
            {
                RenderState.CurrentExtraShader = -1;
                RenderState.ShaderAttributes = 0;
                RenderState.ShaderAttributeCount = 0;
            }
            break;
            default:
            break;
        }
    }
    
    Renderer.CommandCount = 0;
    Clear(&Renderer.Commands);
    
    glDisable(GL_DEPTH_TEST);
    
    for(i32 Index = 0; Index < Renderer.UICommandCount; Index++)
    {
        const render_command& Command = *((render_command*)Renderer.UICommands.CurrentBlock->Base + Index);
        
        switch(Command.Type)
        {
            case RenderCommand_Line:
            {
                RenderLine(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Text:
            {
                RenderText(Command, RenderState);
            }
            break;
            case RenderCommand_Quad:
            {
                RenderQuad(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Sprite:
            {
                //RenderSprite(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Model:
            {
                RenderModel(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Buffer:
            {
                RenderBuffer(Command, RenderState, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_DepthTest:
            {
                // @Incomplete: Do we need depth test commands for UI stuff?
            }
            break;
            case RenderCommand_ShaderStart:
            {
                RenderState.CurrentExtraShader = Command.Shader.Handle;
                RenderState.ShaderAttributes = Command.Shader.Attributes;
                RenderState.ShaderAttributeCount = Command.Shader.AttributeCount;
            }
            break;
            case RenderCommand_ShaderEnd:
            {
                RenderState.CurrentExtraShader = -1;
                RenderState.ShaderAttributes = 0;
                RenderState.ShaderAttributeCount = 0;
            }
            break;
            default:
            break;
        }
    }
    
    Renderer.UICommandCount = 0;
    Clear(&Renderer.UICommands);
    
}

static void Render(render_state& RenderState, renderer& Renderer, memory_arena* PermArena)
{
    if(Renderer.WindowMode != RenderState.WindowMode)
    {
        glfwDestroyWindow(RenderState.Window);
        CreateOpenGLWindow(RenderState, Renderer.WindowMode, RenderState.WindowTitle, RenderState.WindowWidth, RenderState.WindowHeight);
        glfwSetInputMode(RenderState.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetFramebufferSizeCallback(RenderState.Window, FramebufferSizeCallback);
        
        glfwMakeContextCurrent(RenderState.Window);
        
        glfwSwapInterval(0);
        
        glfwGetFramebufferSize(RenderState.Window, &RenderState.WindowWidth, &RenderState.WindowHeight);
        glViewport(0, 0, RenderState.WindowWidth, RenderState.WindowHeight);
        glDisable(GL_DITHER);
        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glDepthFunc(GL_LESS);
        
        glfwSetWindowUserPointer(RenderState.Window, &RenderState);
        glfwSetKeyCallback(RenderState.Window, KeyCallback);
        glfwSetCharCallback(RenderState.Window, CharacterCallback);
        glfwSetCursorPosCallback(RenderState.Window, CursorPositionCallback);
        glfwSetMouseButtonCallback(RenderState.Window, MouseButtonCallback);
        glfwSetScrollCallback(RenderState.Window, ScrollCallback);
        
        GLint Viewport[4];
        glGetIntegerv(GL_VIEWPORT, Viewport);
        
        memcpy(RenderState.Viewport, Viewport, sizeof(GLint) * 4);
    }
    
    LoadExtraShaders(RenderState, Renderer);
    
    LoadTextures(RenderState, Renderer);
    
    RenderState.CurrentExtraShader = -1;
    RenderState.ShaderAttributeCount = 0;
    
    auto& Camera = Renderer.Cameras[Renderer.CurrentCameraHandle];
    Camera.ViewportWidth = RenderState.ScaleFromWidth;
    Camera.ViewportHeight = RenderState.ScaleFromHeight;
    Renderer.ViewportWidth = RenderState.ScaleFromWidth;
    Renderer.ViewportHeight = RenderState.ScaleFromHeight;
    
    RenderState.ScaleX = 2.0f / RenderState.WindowWidth;
    RenderState.ScaleY = 2.0f / RenderState.WindowHeight;
    
    glBindFramebuffer(GL_FRAMEBUFFER, RenderState.FrameBuffer);
    
    glBindTexture(GL_TEXTURE_2D, RenderState.TextureColorBuffer);
    
    glEnable(GL_DEPTH_TEST);
    
    glDepthFunc(GL_LESS);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glClearColor(Renderer.ClearColor.r, Renderer.ClearColor.g, Renderer.ClearColor.b, Renderer.ClearColor.a);
    
    //RenderGame(GameState);
    RenderCommands(RenderState, Renderer, PermArena);
    // Second pass
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glClearColor(Renderer.ClearColor.r, Renderer.ClearColor.g, Renderer.ClearColor.b, Renderer.ClearColor.a);
    
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
    
    glUniform1i((GLint)RenderState.FrameBufferTex0Loc, 0);
    
    glUniform1i((GLint)RenderState.FrameBufferTex1Loc, 1);
    glActiveTexture(GL_TEXTURE0);
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
