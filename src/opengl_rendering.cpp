
//#define GL_DEBUG

#ifdef GL_DEBUG
GLuint DebugAttrib(u32 Program, char* Str, char* File, int Line)
{
    Debug("File: %s, Line: %d\n", File, Line);
    return (GLuint)glGetAttribLocation(Program, Str);
}

void DebugAttribArray(u32 Location, char* File, int Line)
{
    Debug("File: %s, Line: %d\n", File, Line);
    glEnableVertexAttribArray(Location);
}


// Macro redefinition, but used for debug
#define glEnableVertexAttribArray(Loc) DebugAttribArray(Loc, __FILE__, __LINE__)
#define glGetAttribLocation(Shader, Str) DebugAttrib(Shader, Str, __FILE__, __LINE__)
#endif


static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %d - %s\n", error, description);
}

static void show_mouse_cursor(RenderState& render_state, b32 show)
{
    if(show)
    {
        glfwSetInputMode(render_state.Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(render_state.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void message_callback(GLenum Source,
                     GLenum Type,
                     GLuint Id,
                     GLenum Severity,
                     GLsizei Length,
                     const GLchar* Message,
                     const void* user_param)
{
    (void)user_param; // Silence unused warning
    if(Type == GL_DEBUG_TYPE_ERROR)
    {
        Debug("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s, source = %x, id = %ud, length %ud= \n",
              (Type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
              Type, Severity, Message, Source, Id, Length);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    RenderState* render_state = (RenderState*)glfwGetWindowUserPointer(window);
    glfwSetWindowAspectRatio(window, 16, 9);
    glViewport(0, 0, width, height);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    memcpy(render_state->Viewport, viewport, sizeof(GLint) * 4);
    
    render_state->WindowWidth = width;
    render_state->WindowHeight = height;
    
    // Change resolution of framebuffer texture
    //@Incomplete: This should be done with lower resolutions and just be upscaled maybe? We need fixed resolutions
    glBindTexture(GL_TEXTURE_2D, render_state->TextureColorBuffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)render_state->ScaleFromWidth, (GLsizei)render_state->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->TextureColorBuffer, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

inline static r64 get_time()
{
    return glfwGetTime();
}

inline static void poll_events()
{
    glfwPollEvents();
}

[[noreturn]]
static void close_window(RenderState& render_state)
{
    glfwDestroyWindow(render_state.Window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static b32 should_close_window(RenderState& render_state)
{
    return glfwWindowShouldClose(render_state.Window); 
}

static GLint shader_compilation_error_checking(const char* ShaderName, GLuint shader)
{
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
        GLint MaxLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &MaxLength);
        
        // The maxLength includes the NULL character
        GLchar* ErrorLog = PushTempSize(MaxLength, GLchar);
        
        glGetShaderInfoLog(shader, MaxLength, &MaxLength, ErrorLog);
        
        Debug("SHADER Compilation error - %s\n", ShaderName);
        Debug("%s", ErrorLog);
        
        glDeleteShader(shader); // Don't leak the shader.
    }
    return is_compiled;
}

static GLuint load_extra_shader(ShaderData& shader_data, RenderState& render_state)
{
    // @Incomplete: vertexAttribPointers?
    Shader* shader = &render_state.ExtraShaders[render_state.ExtraShaderIndex++];
    shader->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->VertexShader, 1, &shader_data.VertexShaderContent, NULL);
    glCompileShader(shader->VertexShader);
    
    if (!shader_compilation_error_checking(concat(shader_data.Name, ".vert"), shader->VertexShader))
    {
        shader->Program = 0;
        return GL_FALSE;
    }
    
    shader->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(shader->FragmentShader, 1, &shader_data.FragmentShaderContent, NULL);
    glCompileShader(shader->FragmentShader);
    
    if (!shader_compilation_error_checking(concat(shader_data.Name, ".frag"), shader->FragmentShader))
    {
        shader->Program = 0;
        return GL_FALSE;
    }
    
    shader->Program = glCreateProgram();
    
    glAttachShader(shader->Program, shader->VertexShader);
    glAttachShader(shader->Program, shader->FragmentShader);
    glLinkProgram(shader->Program);
    
    return GL_TRUE;
}

static GLuint load_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* vertex_string = concat(file_path, ".vert");
    GLchar *vertex_text = load_shader_from_file(vertex_string, perm_arena);
    
    if(vertex_text)
    {
        glShaderSource(shd->VertexShader, 1, &vertex_text, NULL);
        glCompileShader(shd->VertexShader);
        
        if (!shader_compilation_error_checking(file_path, shd->VertexShader))
        {
            shd->Program = 0;
            return GL_FALSE;
        }
        
        shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        char* fragment_string = concat(file_path, ".frag");
        GLchar *fragment_text = load_shader_from_file(fragment_string, perm_arena);
        
        glShaderSource(shd->FragmentShader, 1, &fragment_text, NULL);
        glCompileShader(shd->FragmentShader);
        
        if (!shader_compilation_error_checking(file_path, shd->FragmentShader))
        {
            shd->Program = 0;
            return GL_FALSE;
        }
        
        shd->Program = glCreateProgram();
        
        glAttachShader(shd->Program, shd->VertexShader);
        glAttachShader(shd->Program, shd->FragmentShader);
        glLinkProgram(shd->Program);
        
        return GL_TRUE;
    }
    return GL_FALSE;
}

static GLuint load_vertex_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    shd->Program = glCreateProgram();
    
    shd->VertexShader = glCreateShader(GL_VERTEX_SHADER);
    char* vertex_string = concat(file_path,".vert");
    GLchar *vertex_text = load_shader_from_file(vertex_string, perm_arena);
    glShaderSource(shd->VertexShader, 1, &vertex_text, NULL);
    glCompileShader(shd->VertexShader);
    
    if (!shader_compilation_error_checking(file_path, shd->VertexShader))
    {
        shd->Program = 0;
        return GL_FALSE;
    }
    
    glAttachShader(shd->Program, shd->VertexShader);
    glAttachShader(shd->Program, shd->FragmentShader);
    
    glLinkProgram(shd->Program);
    
    return GL_TRUE;
}

static GLuint load_fragment_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    shd->Program = glCreateProgram();
    
    shd->FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    char* fragment_string = concat(file_path,".frag");
    GLchar *fragment_text = load_shader_from_file(fragment_string, perm_arena);
    glShaderSource(shd->FragmentShader, 1, &fragment_text, NULL);
    glCompileShader(shd->FragmentShader);
    
    if (!shader_compilation_error_checking(file_path, shd->FragmentShader))
    {
        shd->Program = 0;
        return GL_FALSE;
    }
    glAttachShader(shd->Program, shd->VertexShader);
    glAttachShader(shd->Program, shd->FragmentShader);
    glLinkProgram(shd->Program);
    
    return GL_TRUE;
}

static void use_shader(Shader *shader)
{
    glUseProgram(shader->Program);
}

static void initialize_free_type_font(char* font_path, int font_size, FT_Library library, RenderFont* font)
{
    if(FT_New_Face(library, font_path, 0, &font->Face)) 
    {
        fprintf(stderr, "Could not open font\n");
    }
    
    FT_Set_Pixel_Sizes(font->Face, 0, (FT_UInt)font_size);
    
    FT_Select_Charmap(font->Face , ft_encoding_unicode);
    
    //Find the atlas width and height
    FT_GlyphSlot G = font->Face->glyph;
    
    unsigned int w = 0;
    unsigned int H = 0;
    
    for(int i = 0; i < 255; i++)
    {
        if(FT_Load_Char(font->Face, (FT_ULong)i, FT_LOAD_RENDER))
        {
            fprintf(stderr, "Loading character %c failed!\n", i);
            continue;
        }
        
        w += G->bitmap.width;
        H = Max(H, G->bitmap.rows);
    }
    
    font->AtlasWidth = w;
    font->AtlasHeight = H;
    font->GlyphWidth = (GLfloat)G->bitmap.width;
    
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &font->Texture);
    glBindTexture(GL_TEXTURE_2D, font->Texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (GLsizei)font->AtlasWidth, (GLsizei)font->AtlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned int x = 0;
    
    for(int i = 0; i < 255; i++) 
    {
        if(FT_Load_Char(font->Face, (FT_ULong)i, FT_LOAD_RENDER))
            continue;
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, (GLsizei)x, 0, (GLsizei)G->bitmap.width, (GLsizei)G->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, G->bitmap.buffer);
        
        font->CharacterInfo[i].AX = (r32)(G->advance.x >> 6);
        font->CharacterInfo[i].AY = (r32)(G->advance.y >> 6);
        
        font->CharacterInfo[i].BW = (r32)G->bitmap.width;
        font->CharacterInfo[i].BH = (r32)G->bitmap.rows;
        
        font->CharacterInfo[i].BL = (r32)G->bitmap_left;
        font->CharacterInfo[i].BT = (r32)G->bitmap_top;
        
        font->CharacterInfo[i].TX = (r32)x / font->AtlasWidth;
        
        x += G->bitmap.width;
    }
    
    glGenVertexArrays(1, &font->VAO);
    glBindVertexArray(font->VAO);
    
    glGenBuffers(1, &font->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, font->VBO);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

static void register_buffers(RenderState& render_state, GLfloat* vertex_buffer, long vertex_buffer_size, GLuint* index_buffer, i32 index_buffer_count, long index_buffer_size, b32 has_normals, b32 has_u_vs, b32 skinned, i32 buffer_handle = -1)
{
    Buffer* buffer = &render_state.Buffers[buffer_handle == -1 ? render_state.BufferCount : buffer_handle];
    
    buffer->VertexBufferSize = (GLint)vertex_buffer_size;
    buffer->IndexBufferCount = (GLint)index_buffer_count;
    
    if(buffer->VAO == 0)
        glGenVertexArrays(1, &buffer->VAO);
    
    glBindVertexArray(buffer->VAO);
    
    if(buffer->VBO == 0)
        glGenBuffers(1, &buffer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
    
    i32 bone_info_size = skinned ? 8 : 0;
    
    if(has_normals && has_u_vs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(8 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(12 * sizeof(GLfloat)));
    }
    else if(has_normals)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((6 + bone_info_size) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, (GLsizei)((6 + bone_info_size) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone count
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(7 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(11 * sizeof(GLfloat)));
    }
    else if(has_u_vs)
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)((5 + bone_info_size) * sizeof(GLfloat)), 0);
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (GLsizei)((5 + bone_info_size) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone count
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(5 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(6 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(10 * sizeof(GLfloat)));
    }
    else
    {
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (GLsizei)(3 + bone_info_size * sizeof(GLfloat)), 0);
        
        // Bone count
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(3 * sizeof(GLfloat)));
        
        // Bone indices
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(4 * sizeof(GLfloat)));
        
        // Weights
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, (GLsizei)((8 + bone_info_size) * sizeof(GLfloat)), (void*)(8 * sizeof(GLfloat)));
    }
    
    glGenBuffers(1, &buffer->IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, index_buffer, GL_STATIC_DRAW);
    
    if(buffer_handle == -1)
        render_state.BufferCount++;
    
    glBindVertexArray(0);
}

static void register_vertex_buffer(RenderState& render_state, GLfloat* buffer_data, i32 size, Shader_Type shader_type, MemoryArena* perm_arena, i32 buffer_handle = -1)
{
    Buffer* buffer = &render_state.Buffers[buffer_handle == -1 ? render_state.BufferCount : buffer_handle];
    
    buffer->VertexBufferSize = size;
    buffer->IndexBufferSize = 0;
    buffer->IBO = 0;
    
    if(buffer->VAO == 0)
        glGenVertexArrays(1, &buffer->VAO);
    
    glBindVertexArray(buffer->VAO);
    
    if(buffer->VBO == 0)
        glGenBuffers(1, &buffer->VBO);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)(sizeof(GLfloat) * size), buffer_data, GL_STATIC_DRAW);
    
    if(!render_state.Shaders[shader_type].Loaded)
    {
        load_shader(ShaderPaths[shader_type], &render_state.Shaders[shader_type], perm_arena);
    }
    else
        use_shader(&render_state.Shaders[shader_type]);
    
    auto position_location = (GLuint)glGetAttribLocation(render_state.TileShader.Program, "pos");
    auto texcoord_location = (GLuint)glGetAttribLocation(render_state.TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if(buffer_handle == -1)
        render_state.BufferCount++;
}

static void render_setup(RenderState *render_state, MemoryArena* perm_arena)
{
    // @Cleanup: Not sure if a fallback is a good way of dealing with this
    if(render_state->ScaleFromWidth == 0 || render_state->ScaleFromHeight == 0)
    {
        render_state->ScaleFromWidth = render_state->WindowWidth;
        render_state->ScaleFromHeight = render_state->WindowHeight;
    }
    
    if(FT_Init_FreeType(&render_state->FTLibrary)) 
    {
        fprintf(stderr, "Could not init freetype library\n");
    }
    
    render_state->FontCount = 0;
    
    // Framebuffer 1
    glGenFramebuffers(1, &render_state->FrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, render_state->FrameBuffer);
    
    glGenTextures(1, &render_state->TextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, render_state->TextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, render_state->ScaleFromWidth, render_state->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->TextureColorBuffer, 0
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Debug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_state->ScaleFromWidth, render_state->ScaleFromHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
    
    glGenFramebuffers(1, &render_state->LightingFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, render_state->LightingFrameBuffer);
    
    glGenTextures(1, &render_state->LightingTextureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, render_state->LightingTextureColorBuffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, render_state->ScaleFromWidth, render_state->ScaleFromHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->LightingTextureColorBuffer, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Debug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    // FrameBuffer VAO
    glGenVertexArrays(1, &render_state->FrameBufferVAO);
    glBindVertexArray(render_state->FrameBufferVAO);
    glGenBuffers(1, &render_state->FrameBufferVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->FrameBufferVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->SpriteQuadVerticesSize, render_state->FrameBufferVertices, GL_DYNAMIC_DRAW);
    
    render_state->FrameBufferShader.Type = Shader_FrameBuffer;
    
    load_shader(ShaderPaths[Shader_FrameBuffer], &render_state->FrameBufferShader, perm_arena);
    
    auto pos_loc = (GLuint)glGetAttribLocation(render_state->FrameBufferShader.Program, "pos");
    auto tex_loc = (GLuint)glGetAttribLocation(render_state->FrameBufferShader.Program, "texcoord");
    
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(tex_loc);
    glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    render_state->FrameBufferTex0Loc = (GLuint)glGetUniformLocation(render_state->FrameBufferShader.Program, "tex");
    render_state->FrameBufferTex1Loc = (GLuint)glGetUniformLocation(render_state->FrameBufferShader.Program, "lightingTex");
    
    glGenBuffers(1, &render_state->QuadIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state->QuadIndices), render_state->QuadIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    //Sprite
    glGenVertexArrays(1, &render_state->SpriteVAO);
    glBindVertexArray(render_state->SpriteVAO);
    glGenBuffers(1, &render_state->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->SpriteQuadVerticesSize, render_state->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    
    render_state->TextureShader.Type = Shader_Texture;
    load_shader(ShaderPaths[Shader_Texture], &render_state->TextureShader, perm_arena);
    
    auto position_location = (GLuint)glGetAttribLocation(render_state->TextureShader.Program, "pos");
    auto texcoord_location = (GLuint)glGetAttribLocation(render_state->TextureShader.Program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    //Animation
    //Sprite
    glGenVertexArrays(1, &render_state->SpriteSheetVAO);
    glBindVertexArray(render_state->SpriteSheetVAO);
    glGenBuffers(1, &render_state->SpriteQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->SpriteQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->SpriteQuadVerticesSize, render_state->SpriteQuadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    
    render_state->TextureShader.Type = Shader_Spritesheet;
    load_shader(ShaderPaths[Shader_Spritesheet], &render_state->SpritesheetShader, perm_arena);
    
    position_location = (GLuint)glGetAttribLocation(render_state->SpritesheetShader.Program, "pos");
    texcoord_location = (GLuint)glGetAttribLocation(render_state->SpritesheetShader.Program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //error sprite
    /*glGenVertexArrays(1, &RenderState->SpriteErrorVAO);
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
    */
    //ui sprite
    /*glGenVertexArrays(1, &RenderState->UISpriteVAO);
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
    */
    //tile
    glGenVertexArrays(1, &render_state->TileVAO);
    glBindVertexArray(render_state->TileVAO);
    glGenBuffers(1, &render_state->TileQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->TileQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->TileQuadVerticesSize, render_state->TileQuadVertices, GL_STATIC_DRAW);
    
    render_state->TileShader.Type = Shader_Tile;
    load_shader(ShaderPaths[Shader_Tile], &render_state->TileShader, perm_arena);
    render_state->TileShader.Loaded = true;
    
    auto position_location2 = (GLuint)glGetAttribLocation(render_state->TileShader.Program, "pos");
    auto texcoord_location2 = (GLuint)glGetAttribLocation(render_state->TileShader.Program, "texcoord");
    
    glEnableVertexAttribArray(position_location2);
    glVertexAttribPointer(position_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location2);
    glVertexAttribPointer(texcoord_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //console
    glGenVertexArrays(1, &render_state->RectVAO);
    glBindVertexArray(render_state->RectVAO);
    glGenBuffers(1, &render_state->NormalQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->NormalQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->NormalQuadVerticesSize, render_state->NormalQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    
    render_state->RectShader.Type = Shader_Rect;
    load_shader(ShaderPaths[Shader_Rect], &render_state->RectShader, perm_arena);
    
    auto position_location3 = (GLuint)glGetAttribLocation(render_state->RectShader.Program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &render_state->TextureRectVAO);
    glBindVertexArray(render_state->TextureRectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->SpriteQuadVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    
    render_state->TextureRectShader.Type = Shader_TextureRect;
    load_shader(ShaderPaths[Shader_TextureRect], &render_state->TextureRectShader, perm_arena);
    
    position_location2 = (GLuint)glGetAttribLocation(render_state->TextureRectShader.Program, "pos");
    texcoord_location2 = (GLuint)glGetAttribLocation(render_state->TextureRectShader.Program, "texcoord");
    
    glEnableVertexAttribArray(position_location2);
    glVertexAttribPointer(position_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location2);
    glVertexAttribPointer(texcoord_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    //wireframe
    glGenVertexArrays(1, &render_state->WireframeVAO);
    glBindVertexArray(render_state->WireframeVAO);
    glGenBuffers(1, &render_state->WireframeQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->WireframeQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->WireframeQuadVerticesSize, render_state->WireframeQuadVertices, GL_DYNAMIC_DRAW);
    
    render_state->RectShader.Type = Shader_Wireframe;
    load_shader(ShaderPaths[Shader_Wireframe], &render_state->WireframeShader, perm_arena);
    
    position_location3 = (GLuint)glGetAttribLocation(render_state->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &render_state->WireframeCubeVAO);
    glBindVertexArray(render_state->WireframeCubeVAO);
    glGenBuffers(1, &render_state->WireframeCubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->WireframeCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, render_state->WireframeCubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &render_state->CubeIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->CubeIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * CUBE_INDICES, render_state->WireframeCubeIndices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //
    
    glGenVertexArrays(1, &render_state->IsometricVAO);
    glBindVertexArray(render_state->IsometricVAO);
    glGenBuffers(1, &render_state->IsometricQuadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->IsometricQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->WireframeQuadVerticesSize, render_state->IsometricQuadVertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->QuadIndexBuffer);
    
    render_state->RectShader.Type = Shader_Wireframe;
    load_shader(ShaderPaths[Shader_Wireframe], &render_state->WireframeShader, perm_arena);
    
    position_location3 = (GLuint)glGetAttribLocation(render_state->WireframeShader.Program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    // Lines
    load_shader(ShaderPaths[Shader_Line], &render_state->LineShader, perm_arena);
    glGenVertexArrays(1, &render_state->LineVAO);
    glBindVertexArray(render_state->LineVAO);
    glGenBuffers(1, &render_state->LineVBO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->LineVBO);
    
    glGenBuffers(1, &render_state->LineEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->LineEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * LINE_INDICES, render_state->LineIndices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    /*glGenVertexArrays(1, &RenderState->PrimitiveVAO);
        glBindVertexArray(RenderState->PrimitiveVAO);
        glGenBuffers(1, &RenderState->PrimitiveVBO);
        glBindBuffer(GL_ARRAY_BUFFER, RenderState->PrimitiveVBO);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glBindVertexArray(0);
        */
    
    
    // Passthrough
    load_shader(ShaderPaths[Shader_Passthrough], &render_state->PassthroughShader, perm_arena);
    glGenVertexArrays(1, &render_state->PassthroughVAO);
    glBindVertexArray(render_state->PassthroughVAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(r32), 0);
    glBindVertexArray(0);
    
    //font
    render_state->StandardFontShader.Type = Shader_StandardFont;
    load_shader(ShaderPaths[Shader_StandardFont], &render_state->StandardFontShader, perm_arena);
    
    // Light sources
    render_state->LightSourceShader.Type = Shader_LightSource;
    load_shader(ShaderPaths[Shader_LightSource], &render_state->LightSourceShader, perm_arena);
    
    render_state->SimpleModelShader.Type = Shader_SimpleModel;
    load_shader(ShaderPaths[Shader_SimpleModel], &render_state->SimpleModelShader, perm_arena);
    
    render_state->SpotlightData.NumLights = 0;
    glGenBuffers(1, &render_state->SpotlightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, render_state->SpotlightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotlightData), &render_state->SpotlightData, GL_DYNAMIC_DRAW);
    
    u32 block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "spotlights");
    
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    render_state->DirectionalLightData.NumLights = 0;
    glGenBuffers(1, &render_state->DirectionalLightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, render_state->DirectionalLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightData), &render_state->DirectionalLightData, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "directionalLights");
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    render_state->PointLightData.NumLights = 0;
    glGenBuffers(1, &render_state->PointLightUBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, render_state->PointLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightData), &render_state->PointLightData, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "pointLights");
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void update_lighting_data(const render_state& render_state)
{
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.SpotlightUBO);
    GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if(p)
    {
        memcpy(p, &RenderState.SpotlightData, sizeof(SpotlightData));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.DirectionalLightUBO);
    p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if(p)
    {
        memcpy(p, &RenderState.DirectionalLightData, sizeof(DirectionalLightData));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindBuffer(GL_UNIFORM_BUFFER, RenderState.PointLightUBO);
    p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if(p)
    {
        memcpy(p, &RenderState.PointLightData, sizeof(PointLightData));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
}

static GLuint load_texture(texture_data& data, Texture* texture)
{ 
    GLuint texture_handle;
    
    glGenTextures(1, &texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture_handle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    //enable alpha for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.Width, data.Height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, (GLvoid*) data.ImageData);
    
    texture->TextureHandle = texture_handle;
    stbi_image_free(data.ImageData);
    
    return GL_TRUE;
}

static void load_textures(RenderState& render_state, Renderer& renderer)
{
    for(i32 index = render_state.TextureIndex; index < renderer.TextureCount; index++)
    {
        load_texture(renderer.TextureData[index], &render_state.TextureArray[render_state.TextureIndex++]);
    }
}

static void load_extra_shaders(RenderState& render_state, Renderer& renderer)
{
    for(i32 index = render_state.ExtraShaderIndex; index < renderer.ShaderCount; index++)
    {
        load_extra_shader(renderer.ShaderData[index], render_state);
    }
}

static void create_open_gl_window(RenderState& render_state, WindowMode window_mode, const char* title, i32 width, i32 height)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    i32 screen_width = width;
    i32 screen_height = height;
    
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    render_state.WindowMode = window_mode;
    render_state.WindowTitle = push_string(&render_state.Arena, strlen(title) + 1);
    strcpy(render_state.WindowTitle, title);
    
    if(window_mode == FM_Borderless)
    {
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        screen_width = mode->width;
        screen_height = mode->height;
    }
    
    if(window_mode == FM_Windowed)
    {
        monitor = NULL;
    }
    
    render_state.Window = glfwCreateWindow(screen_width, screen_height, render_state.WindowTitle, monitor, 
                                          NULL);
    
    //center window on screen (windowed?)
    
    if(window_mode == FM_Windowed)
    {
        int frame_buffer_width, frame_buffer_height;
        
        glfwGetFramebufferSize(render_state.Window, &frame_buffer_width, &frame_buffer_height);
        glfwSetWindowPos(render_state.Window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
    }
}

static void initialize_open_gl(RenderState& render_state, Renderer& renderer, ConfigData* config_data, MemoryArena* perm_arena)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwSetErrorCallback(error_callback);
    
    render_state.ScaleFromWidth = config_data->ScaleFromWidth;
    render_state.ScaleFromHeight = config_data->ScaleFromHeight;
    
    //@Incomplete: Figure something out here. Ask for compatible version etc
#ifdef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif __linux
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#elif __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    render_state.Contrast = config_data->Contrast;
    render_state.Brightness = config_data->Brightness;
    
    create_open_gl_window(render_state, config_data->Fullscreen, config_data->Title, config_data->ScreenWidth, config_data->ScreenHeight);
    renderer.WindowMode = render_state.WindowMode;
    
    if (!render_state.Window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetInputMode(render_state.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    glfwSetFramebufferSizeCallback(render_state.Window, framebuffer_size_callback);
    
    glfwMakeContextCurrent(render_state.Window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(render_state.Window, &render_state.WindowWidth, &render_state.WindowHeight);
    glViewport(0, 0, render_state.WindowWidth, render_state.WindowHeight);
    
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC) message_callback, 0);
    
    render_state.ScreenWidth = config_data->ScreenWidth;
    render_state.ScreenHeight = config_data->ScreenHeight;
    if(render_state.ScreenWidth != 0)
    {
        render_state.DpiScale = render_state.WindowWidth / render_state.ScreenWidth;
    }
    
    glDisable(GL_DITHER);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);
    
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    
    glDepthFunc(GL_LESS);
    
    Debug("%s\n", glGetString(GL_VERSION));
    Debug("Shading language supported: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    Debug("Glad Version: %d.%d\n", GLVersion.major, GLVersion.minor);
    
    glfwSetWindowUserPointer(render_state.Window, &render_state);
    glfwSetKeyCallback(render_state.Window, key_callback);
    glfwSetCharCallback(render_state.Window, character_callback);
    glfwSetCursorPosCallback(render_state.Window, cursor_position_callback);
    glfwSetMouseButtonCallback(render_state.Window, mouse_button_callback);
    glfwSetScrollCallback(render_state.Window, scroll_callback);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    memcpy(render_state.Viewport, viewport, sizeof(GLint) * 4);
    
    renderer.WindowWidth = render_state.ScreenWidth;
    renderer.WindowHeight = render_state.ScreenHeight;
    
    memcpy(renderer.Viewport, render_state.Viewport, sizeof(i32) * 4);
    renderer.Viewport[0] /= render_state.DpiScale;
    renderer.Viewport[1] /= render_state.DpiScale;
    renderer.Viewport[2] /= render_state.DpiScale;
    renderer.Viewport[3] /= render_state.DpiScale;
    
    printf("Viewport %d %d %d %d\n", renderer.Viewport[0], renderer.Viewport[1], renderer.Viewport[2], renderer.Viewport[3]);
    
    controller_present();
    
    renderer.ShouldClose = false;
    render_setup(&render_state, perm_arena);
}

static void reload_vertex_shader(Shader_Type type, RenderState* render_state, MemoryArena* perm_arena)
{
    glDeleteProgram(render_state->Shaders[type].Program);
    glDeleteShader(render_state->Shaders[type].VertexShader);
    load_vertex_shader(ShaderPaths[type], &render_state->Shaders[type], perm_arena);
}

static void reload_fragment_shader(Shader_Type type, RenderState* render_state, MemoryArena* perm_arena)
{
    glDeleteProgram(render_state->Shaders[type].Program);
    glDeleteShader(render_state->Shaders[type].FragmentShader);
    load_fragment_shader(ShaderPaths[type], &render_state->Shaders[type], perm_arena);
    
    render_state->SpotlightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, render_state->SpotlightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotlightData), &render_state->SpotlightData, GL_DYNAMIC_DRAW);
    
    u32 block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "spotlights");
    
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    render_state->DirectionalLightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, render_state->DirectionalLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightData), &render_state->DirectionalLightData, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "directionalLights");
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    render_state->PointLightData.NumLights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, render_state->PointLightUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightData), &render_state->PointLightData, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->SimpleModelShader.Program, "pointLights");
    glUniformBlockBinding(render_state->SimpleModelShader.Program, block_index, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void reload_assets(RenderState& render_state, AssetManager* asset_manager, MemoryArena* perm_arena)
{
    for(int i = 0; i < Shader_Count; i++)
    {
        if(asset_manager->DirtyVertexShaderIndices[i] == 1)
        {
            Debug("Reloading vertex shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            reload_vertex_shader((Shader_Type)i, &render_state, perm_arena);
            asset_manager->DirtyVertexShaderIndices[i] = 0;
        }
        
        if(asset_manager->DirtyFragmentShaderIndices[i] == 1)
        {
            Debug("Reloading fragment shader type: %s\n", ShaderEnumToStr((Shader_Type)i));
            reload_fragment_shader((Shader_Type)i, &render_state, perm_arena);
            asset_manager->DirtyFragmentShaderIndices[i] = 0;
        }
    }
}

static void set_float_uniform(GLuint shader_handle, const char* uniform_name, r32 value)
{
    glUniform1f(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_int_uniform(GLuint shader_handle, const char* uniform_name, i32 value)
{
    glUniform1i(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_bool_uniform(GLuint shader_handle, const char* uniform_name, b32 value)
{
    glUniform1i(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_vec2_uniform(GLuint shader_handle, const char *uniform_name, math::v2 value)
{
    glUniform2f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y);
}

void set_vec3_uniform(GLuint shader_handle, const char *uniform_name, math::v3 value)
{
    glUniform3f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z);
}

static void set_vec4_uniform(GLuint shader_handle, const char *uniform_name, math::v4 value)
{
    glUniform4f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z, value.w);
}

static void set_mat4_uniform(GLuint shader_handle, const char *uniform_name, math::m4 v)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_handle, uniform_name), 1, GL_TRUE, &v[0][0]);
}

void set_vec4_array_uniform(GLuint shader_handle, const char *uniform_name, math::v4* value, u32 length)
{
    glUniform4fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat*)&value[0]);
}

void set_float_array_uniform(GLuint shader_handle, const char *uniform_name, r32* value, u32 length)
{
    glUniform1fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat*)&value[0]);
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static void render_line(RenderState& render_state, math::v4 color, math::v3 start, math::v3 end, math::m4 projection_matrix = math::m4(), math::m4 view_matrix = math::m4(), r32 line_width = 1.0f, b32 is_ui = false)
{
    if(is_ui)
    {
        start.x *= render_state.ScaleX;
        start.x -= 1;
        start.y *= render_state.ScaleY;
        start.y -= 1;
        end.x *= render_state.ScaleX;
        end.x -= 1;
        end.y *= render_state.ScaleY;
        end.y -= 1;
        start.z = 0.0f;
        end.z = 0.0f;
    }
    
    auto& shader = render_state.LineShader;
    use_shader(&shader);
    
    glBindVertexArray(render_state.LineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.LineVBO);
    
    auto width = 0.005f * line_width;
    
    // ONLY FOR 2D!!!
    auto dx = end.x - start.x;
    auto dy = end.y - start.y;
    auto normal =  math::normalize(math::v2(-dy, dx));
    
    // Double vertices
    // 1.0f and -1.0f are Miters
    
    
    GLfloat points[24] = {
        start.x, start.y, start.z, normal.x, normal.y, -1.0f,
        start.x, start.y, start.z, normal.x, normal.y, 1.0f,
        end.x, end.y, end.z, normal.x, normal.y, -1.0f,
        end.x, end.y, end.z, normal.x, normal.y, 1.0f};
    
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), &points[0], GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat))); // miter
    
    auto m = math::m4(1.0f);
    
    set_mat4_uniform(shader.Program, "model", m);
    set_mat4_uniform(shader.Program, "view", view_matrix);
    set_mat4_uniform(shader.Program, "projection", projection_matrix);
    set_vec4_uniform(shader.Program, "color", color);
    set_float_uniform(shader.Program, "thickness", width);
    set_int_uniform(shader.Program, "isUI", is_ui);
    
    //glDrawArrays(GL_TRIANGLES, 0, 4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.LineEBO);
    glDrawElements(GL_TRIANGLES, LINE_INDICES, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    glBindVertexArray(0);
}

/*
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
    SetIntUniform(Shader.Program, "IsUI", (i32)IsUI);
    
    glDrawArrays(GL_LINES, 0, 6);
    glLineWidth(1.0f);
}
*/
// NOTE(Niels): Possible future use but buggy
void render_circle(RenderState& render_state, math::v4 color, r32 center_x, r32 center_y, r32 radius, b32 is_ui = true, math::m4 projection_matrix = math::m4(), math::m4 view_matrix = math::m4())
{
    if(is_ui)
    {
        center_x *= render_state.ScaleX;
        center_x -= 1;
        center_y *= render_state.ScaleY;
        center_y -= 1;
        radius *= render_state.ScaleX;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, render_state.PrimitiveVBO);
    
    GLfloat points[720];
    
    i32 point_index = 0;
    
    for(i32 index = 0; index < 360; index++)
    {
        r32 radians = (index * PI) / 180.0f;
        points[point_index++] = math::cos(radians * radius);
        points[point_index++] = math::sin(radians * radius);
    }
    
    glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), &points[0], GL_DYNAMIC_DRAW);
    
    auto& shader = render_state.RectShader;
    use_shader(&shader);
    
    //draw upper part
    math::m4 model(1.0f);
    
    if(!is_ui)
    {
        set_mat4_uniform(shader.Program, "Projection", projection_matrix);
        set_mat4_uniform(shader.Program, "View", view_matrix);
    }
    
    set_float_uniform(shader.Program, "isUI", (r32)is_ui);
    set_mat4_uniform(shader.Program, "M", model);
    set_vec4_uniform(shader.Program, "color", color);
    
    glDrawArrays(GL_LINE_LOOP, 0, 720);
}

static void render_quad(RenderMode mode, RenderState& render_state, math::v4 color, math::v3 position, b32 flipped, math::v3 size, math::v3 rotation, b32 with_origin, math::v2 origin, i32 shader_handle, ShaderAttribute* shader_attributes, i32 shader_attribute_count, b32 is_ui = true, i32 texture_handle = 0, b32 for_animation = false, math::v2 texture_size = math::v2(), math::v2i frame_size = math::v2i(), math::v2 texture_offset = math::v2(), math::m4 projection_matrix = math::m4(), math::m4 view_matrix = math::m4())
{
    if(is_ui)
    {
        position.x *= render_state.ScaleX;
        position.x -= 1;
        position.y *= render_state.ScaleY;
        position.y -= 1;
    }
    
    switch(mode)
    {
        case Render_Fill:
        {
            auto shader = render_state.RectShader;
            
            if(texture_handle > 0)
            {
                glBindVertexArray(render_state.TextureRectVAO);
            }
            else
            {
                glBindVertexArray(render_state.RectVAO);
            }
            
            math::v2i pixel_size;
            
            math::v3 scale = size;
            
            if(texture_handle > 0)
            {
                if(for_animation)
                {
                    pixel_size = frame_size;
                    size = math::v3((size.x * frame_size.x) / render_state.PixelsPerUnit, (size.y * frame_size.y) / render_state.PixelsPerUnit, 0);
                }
                else
                {
                    if(frame_size.x != 0 && frame_size.y != 0)
                    {
                        pixel_size = math::v2i(frame_size.x, frame_size.y);
                        if(is_ui)
                        {
                            size = math::v3(size.x * frame_size.x, size.y * frame_size.y, 0);
                        }
                        else
                        {
                            size = math::v3((size.x * frame_size.x) / render_state.PixelsPerUnit, (size.y * frame_size.y) / render_state.PixelsPerUnit, 0);
                        }
                    }
                    else
                    {
                        pixel_size = math::v2i(texture_size.x, texture_size.y);
                        if(is_ui)
                        {
                            size = math::v3(size.x * texture_size.x, size.y * texture_size.y, 0);
                        }
                        else
                        {
                            size = math::v3((size.x * texture_size.x) / render_state.PixelsPerUnit, (size.y * texture_size.y) / render_state.PixelsPerUnit, 0);
                        }
                    }
                }
                
                if(render_state.BoundTexture != texture_handle)
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)texture_handle);
                }
                
                if(for_animation || (texture_offset.X >= 0.0f && texture_offset.Y >= 0.0f))
                    shader = render_state.SpritesheetShader;
                else
                    shader = render_state.TextureRectShader;
                
                render_state.BoundTexture = (GLuint)texture_handle;
            }
            
            if(is_ui)
            {
                size.x *= render_state.ScaleX;
                size.y *= render_state.ScaleY;
            }
            
            if(render_state.CurrentExtraShader != -1)
            {
                shader = render_state.ExtraShaders[render_state.CurrentExtraShader];
            }
            
            if(shader_handle != -1)
            {
                shader = render_state.ExtraShaders[shader_handle];
            }
            
            use_shader(&shader);
            
            math::m4 model(1.0f);
            
            if(flipped)
            {
                size.x *= -1;
            }
            
            model = math::scale(model, size);
            model = math::translate(model, math::v3(size.x / -2.0f, size.y / -size.y, 0.0f));
            
            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;
            
            auto orientation = math::quat();
            orientation = math::rotate(orientation, rotation.x, math::v3(x_axis, 0.0f, 0.0f));
            orientation = math::rotate(orientation, rotation.y, math::v3(0.0f, y_axis, 0.0f));
            orientation = math::rotate(orientation, rotation.z, math::v3(0.0f, 0.0f, z_axis));
            
            model = to_matrix(orientation) * model;
            model = math::translate(model, math::v3(size.x / 2.0f, size.y / size.y, 0.0f));
            
            if(with_origin)
            {
                if(flipped)
                {
                    position.x -= ((pixel_size.x - origin.x) / render_state.PixelsPerUnit) * scale.x;
                    position.y -= origin.y / render_state.PixelsPerUnit;
                }
                else
                {
                    position.x -= origin.x / render_state.PixelsPerUnit;
                    position.y -= origin.y / render_state.PixelsPerUnit;
                }
            }
            else
            {
                position -= size / 2.0f;
            }
            
            if(flipped)
            {
                model = math::translate(model, math::v3(-size.x, 0.0f, 0.0f));
            }
            
            model = math::translate(model, position);
            
            if(!is_ui)
            {
                set_mat4_uniform(shader.Program, "Projection", projection_matrix);
                set_mat4_uniform(shader.Program, "View", view_matrix);
            }
            
            set_float_uniform(shader.Program, "isUI", (r32)is_ui);
            set_mat4_uniform(shader.Program, "M", model);
            set_vec4_uniform(shader.Program, "color", color);
            
            if(texture_offset.X >= 0.0f && texture_offset.Y >= 0.0f)
            {
                set_vec2_uniform(shader.Program, "textureOffset", texture_offset);
                set_vec2_uniform(shader.Program, "textureSize", texture_size);
                set_vec2_uniform(shader.Program, "frameSize", math::v2((r32)frame_size.x, (r32)frame_size.y));
            }
            
            if(for_animation)
            {
                
                set_vec2_uniform(shader.Program, "textureSize", texture_size);
                set_vec2_uniform(shader.Program, "frameSize", math::v2((r32)frame_size.x, (r32)frame_size.y));
            }
            
            if(render_state.CurrentExtraShader != -1 || shader_handle != -1)
            {
                //shader_attribute* Attributes = ShaderHandle != -1 ? ShaderAttributes : RenderState.ShaderAttributes;
                i32 attribute_count = shader_handle != -1 ? shader_attribute_count : render_state.ShaderAttributeCount;
                
                for(i32 index = 0; index < attribute_count; index++)
                {
                    ShaderAttribute& attribute = shader_attributes[index];
                    switch(attribute.Type)
                    {
                        case Attribute_Float:
                        {
                            set_float_uniform(shader.Program, attribute.Name, attribute.FloatVar);
                        }
                        break;
                        case Attribute_Float2:
                        {
                            set_vec2_uniform(shader.Program, attribute.Name, attribute.Float2Var);
                        }
                        break;
                        case Attribute_Float3:
                        {
                            set_vec3_uniform(shader.Program, attribute.Name, attribute.Float3Var);
                        }
                        break;
                        case Attribute_Float4:
                        {
                            set_vec4_uniform(shader.Program, attribute.Name, attribute.Float4Var);
                        }
                        break;
                        case Attribute_Integer:
                        {
                            set_int_uniform(shader.Program, attribute.Name, attribute.IntegerVar);
                        }
                        break;
                        case Attribute_Boolean:
                        {
                            set_int_uniform(shader.Program, attribute.Name, attribute.BooleanVar);
                        }
                        break;
                        case Attribute_Matrix4:
                        {
                            set_mat4_uniform(shader.Program, attribute.Name, attribute.Matrix4Var);
                        }
                        break;
                    }
                }
            }
            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
        break;
        case Render_Outline:
        {
            math::m4 model(1.0f);
            if(is_ui)
            {
                size.x *= render_state.ScaleX;
                size.y *= render_state.ScaleY;
            }
            
            model = math::scale(model, size);
            
            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;
            
            auto orientation = math::quat();
            orientation = math::rotate(orientation, rotation.x, math::v3(x_axis, 0.0f, 0.0f));
            orientation = math::rotate(orientation, rotation.y, math::v3(0.0f, y_axis, 0.0f));
            orientation = math::rotate(orientation, rotation.z, math::v3(0.0f, 0.0f, z_axis));
            
            model = math::to_matrix(orientation) * model;
            model = math::translate(model, position);
            
            glBindVertexArray(render_state.WireframeVAO);
            
            auto shader = render_state.RectShader;
            use_shader(&shader);
            
            if(!is_ui)
            {
                set_mat4_uniform(shader.Program, "Projection", projection_matrix);
                set_mat4_uniform(shader.Program, "View", view_matrix);
            }
            set_float_uniform(shader.Program, "isUI", (r32)is_ui);
            set_mat4_uniform(shader.Program, "M", model);
            set_vec4_uniform(shader.Program, "color", color);
            
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        break;
    }
    glBindVertexArray(0);
}

static void measure_text(const render_font& font, const char* text, float* width, float* height)
{
    size_t count;
    
    if (!text) 
    {
        return;
    }
    
    *width = 0.0f;
    *height = 0.0f;
    
    for(count = 0; count < strlen(text); ++count) 
    {
        *Width += Font.CharacterInfo[Text[count]].AX;
        
        if(*Height < Font.CharacterInfo[Text[count]].BH) 
        {
            *Height = Font.CharacterInfo[Text[count]].BH;
        }
    }
}

//rendering methods
static void render_text(RenderState& render_state, const render_font& font, const math::v4& color, const char* Text, r32 x, r32 y, r32 scale = 1.0f,
                       Alignment alignment = Alignment_Left,  b32 align_center_y = true) 
{
    glBindVertexArray(Font.VAO);
    auto shader = render_state.Shaders[Shader_StandardFont];
    use_shader(&shader);
    
    set_vec4_uniform(shader.Program, "color", color);
    set_vec4_uniform(shader.Program, "alphaColor", Font.AlphaColor);
    
    if (RenderState.BoundTexture != Font.Texture) //never bind the same texture if it's already bound
    {
        glBindTexture(GL_TEXTURE_2D, Font.Texture);
        RenderState.BoundTexture = Font.Texture;
    }
    
    point* coords = PushTempArray(6 * strlen(Text), point);
    
    int n = 0;
    
    switch(alignment)
    {
        case Alignment_Left:
        break;
        case Alignment_Right:
        break;
        case Alignment_Center:
        {
            r32 width;
            r32 height;
            
            measure_text(Font, Text, &width, &height);
            
            width *= scale;
            height *= scale;
            
            x -= width / 2.0f;
            if(align_center_y)
                y -= height / 2.0f;
        }
        break;
    }
    
    x = (r32)x;
    y = (r32)y;
    
    x *= render_state.ScaleX;
    x -= 1.0f;
    y *= render_state.ScaleY;
    y -= 1.0f;
    
    r32 epsilon = 0.0001f;
    
    for(const char *p = Text; *p; p++) 
    { 
        r32 w = Font.CharacterInfo[*p].BW * RenderState.ScaleX * Scale;
        r32 h = Font.CharacterInfo[*p].BH * RenderState.ScaleY * Scale;
        
        r32 x2 = X + Font.CharacterInfo[*p ].BL * RenderState.ScaleX * Scale;
        r32 y2 = -Y - Font.CharacterInfo[*p ].BT * RenderState.ScaleY * Scale;
        
        /* Advance the cursor to the start of the next character */
        X += Font.CharacterInfo[*p].AX * RenderState.ScaleX * Scale;
        Y += Font.CharacterInfo[*p].AY * RenderState.ScaleY * Scale;
        
        /* Skip glyphs that have no pixels */
        if(!(i32)Font.CharacterInfo[*p].BW || !(i32)Font.CharacterInfo[*p].BH)
            continue;
        
        coords[n++] = { x2 + epsilon, -y2 + epsilon, (r32)Font.CharacterInfo[*p].TX + epsilon, epsilon };
        coords[n++] = { x2 + w - epsilon, -y2 + epsilon, (r32)Font.CharacterInfo[*p].TX - epsilon + (r32)Font.CharacterInfo[*p].BW / (r32)Font.AtlasWidth, epsilon };
        coords[n++] = { x2 + epsilon, -y2 - h - epsilon, (r32)Font.CharacterInfo[*p].TX + epsilon, (r32)Font.CharacterInfo[*p].BH / (r32)Font.AtlasHeight - epsilon };
        coords[n++] = { x2 + w - epsilon, -y2 + epsilon, (r32)Font.CharacterInfo[*p].TX - epsilon + (r32)Font.CharacterInfo[*p].BW / (r32)Font.AtlasWidth,  epsilon };
        coords[n++] = { x2 + epsilon, -y2 - h - epsilon, (r32)Font.CharacterInfo[*p].TX + epsilon, (r32)Font.CharacterInfo[*p].BH / (r32)Font.AtlasHeight - epsilon };
        coords[n++] = { x2 + w - epsilon, -y2 - h, (r32)Font.CharacterInfo[*p].TX - epsilon + Font.CharacterInfo[*p].BW / (r32)Font.AtlasWidth, (r32)Font.CharacterInfo[*p].BH / (r32)Font.AtlasHeight - epsilon };
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, Font.VBO);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(6 * strlen(Text) * sizeof(point)), coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, n);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

static void render_wireframe_cube(const render_command& command, RenderState& render_state, math::m4 projection = math::m4(1.0f), math::m4 view = math::m4(1.0f))
{
    glBindVertexArray(render_state.WireframeCubeVAO);
    
    glLineWidth(Command.WireframeCube.LineWidth);
    auto shader = render_state.PassthroughShader;
    use_shader(&shader);
    
    math::m4 model = math::m4(1.0f);
    model = math::scale(model, Command.Scale);
    model = math::rotate(model, Command.Orientation);
    model = math::translate(model, Command.Position);
    
    set_mat4_uniform(shader.Program, "Model", model);
    set_mat4_uniform(shader.Program, "Projection", projection);
    set_mat4_uniform(shader.Program, "View", view);
    set_vec4_uniform(shader.Program, "Color", Command.WireframeCube.Color);
    
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(GLuint)));
    
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

static void render_line(const render_command& Command, RenderState& RenderState, math::m4 Projection, math::m4 View)
{
    render_line(RenderState, Command.Line.Color, Command.Line.Point1, Command.Line.Point2, Projection, View, Command.Line.LineWidth, Command.IsUI);
}

static void render_text(const render_command& Command, RenderState& RenderState)
{
    RenderFont render_font;
    render_font = RenderState.Fonts[Command.Text.FontHandle];
    // @Incomplete: Y-centering
    render_text(RenderState, render_font, Command.Text.Color, Command.Text.Text, Command.Text.Position.x, Command.Text.Position.y, Command.Text.Scale, Command.Text.Alignment);
}

static void render_quad(const render_command& Command, RenderState& RenderState, math::m4 Projection, math::m4 View)
{
    if(Command.IsUI)
    {
        auto handle = Command.Quad.TextureHandle != -1 ? RenderState.TextureArray[Command.Quad.TextureHandle].TextureHandle : 0;
        render_quad(Command.Quad.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Quad.Color, 
                   Command.Position,
                   Command.Quad.Flipped,
                   Command.Scale,
                   Command.Rotation,
                   Command.WithOrigin,
                   Command.Origin,
                   Command.ShaderHandle,
                   Command.ShaderAttributes,
                   Command.ShaderAttributeCount,
                   Command.IsUI,
                   (i32)handle,
                   Command.Quad.ForAnimation,
                   Command.Quad.TextureSize,
                   Command.Quad.FrameSize,
                   Command.Quad.TextureOffset);
    }
    else
    {
        auto handle = Command.Quad.TextureHandle != -1 ? RenderState.TextureArray[Command.Quad.TextureHandle].TextureHandle : 0;
        render_quad(Command.Quad.Outlined ? Render_Outline : Render_Fill, 
                   RenderState, 
                   Command.Quad.Color, 
                   Command.Position,
                   Command.Quad.Flipped,
                   Command.Scale,
                   Command.Rotation,
                   Command.WithOrigin,
                   Command.Origin,
                   Command.ShaderHandle,
                   Command.ShaderAttributes,
                   Command.ShaderAttributeCount,
                   Command.IsUI,
                   (i32)handle,
                   Command.Quad.ForAnimation,
                   Command.Quad.TextureSize,
                   Command.Quad.FrameSize,
                   Command.Quad.TextureOffset,
                   Projection, 
                   View);
    }
}

static void render_model(const render_command& command, RenderState& render_state, math::m4 projection, math::m4 view)
{
    Buffer buffer = RenderState.Buffers[Command.Model.BufferHandle];
    glBindVertexArray(buffer.VAO);
    
    for(i32 mesh_index = 0; mesh_index < Command.Model.MeshCount; mesh_index++)
    {
        MeshData mesh_data = Command.Model.Meshes[mesh_index];
        Material material = Command.Model.Materials[mesh_data.MaterialIndex];
        
        if(material.DiffuseTexture.HasData)
        {
            Texture texture = render_state.TextureArray[material.DiffuseTexture.TextureHandle];
            if(render_state.BoundTexture != texture.TextureHandle)
            {
                glBindTexture(GL_TEXTURE_2D, texture.TextureHandle);
                render_state.BoundTexture = texture.TextureHandle;
            }
        }
        
        Shader shader = {};
        
        if(Command.Model.Type == Model_Skinned)
        {
            shader = render_state.SimpleModelShader;
            use_shader(&shader);
            
            for(i32 index = 0; index < Command.Model.BoneCount; index++)
            {
                char s_buffer[20];
                sprintf(s_buffer, "bones[%d]", index);
                set_mat4_uniform(shader.Program, s_buffer, Command.Model.BoneTransforms[index]);
            }
        }
        else
        {
            // @Incomplete: We need a shader that isn't using the bone data
        }
        
        math::m4 model(1.0f);
        model = math::scale(model, Command.Scale);
        model = math::rotate(model, Command.Orientation);
        model = math::translate(model, Command.Position);
        
        math::m4 normal_matrix = math::transpose(math::inverse(view * model));
        
        set_mat4_uniform(shader.Program, "normalMatrix", normal_matrix);
        set_mat4_uniform(shader.Program, "projection", projection);
        set_mat4_uniform(shader.Program, "view", view);
        set_mat4_uniform(shader.Program, "model", model);
        set_vec4_uniform(shader.Program, "color", math::rgba(1.0f, 1.0f, 1.0f, 1.0f));
        set_int_uniform(shader.Program, "hasUVs", material.DiffuseTexture.HasData);
        
        glDrawElements(GL_TRIANGLES, buffer.IndexBufferCount, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}

static void render_buffer(const render_command& command, RenderState& render_state, math::m4 projection, math::m4 view)
{
    Buffer buffer = RenderState.Buffers[Command.Buffer.BufferHandle];
    
    glBindVertexArray(buffer.VAO);
    u32 texture_handle = Command.Buffer.TextureHandle != -1 ? RenderState.TextureArray[Command.Buffer.TextureHandle].TextureHandle : 0;
    
    if (texture_handle != 0 && render_state.BoundTexture != texture_handle)
    {
        glBindTexture(GL_TEXTURE_2D, texture_handle);
        render_state.BoundTexture = texture_handle;
    }
    
    math::v3 position = Command.Position;
    math::v3 size = Command.Scale;
    
    if(Command.IsUI)
    {
        position.x *= render_state.ScaleX;
        position.x -= 1;
        position.y *= render_state.ScaleY;
        position.y -= 1;
        
        size.x *= render_state.ScaleX;
        size.y *= render_state.ScaleY;
    }
    
    
    auto shader = render_state.TileShader;
    use_shader(&shader);
    
    math::m4 model(1.0f);
    model = math::scale(model, size);
    model = math::translate(model, position);
    
    //Model = math::YRotate(Command.Rotation.y) * Model;
    //Model = math::XRotate(Command.Rotation.x) * Model;
    //Model = math::ZRotate(Command.Rotation.z) * Model;
    
    set_float_uniform(shader.Program, "isUI", (r32)Command.IsUI);
    set_mat4_uniform(shader.Program, "Projection", projection);
    set_mat4_uniform(shader.Program, "View", view);
    set_mat4_uniform(shader.Program, "Model", model);
    set_vec4_uniform(shader.Program, "Color", Command.Color);
    
    glDrawArrays(
        GL_TRIANGLES, 0, buffer.VertexBufferSize / 3);
    glBindVertexArray(0);
}

static void load_font(RenderState& render_state, char* path, i32 size)
{
    initialize_free_type_font(path, size, render_state.FTLibrary, &render_state.Fonts[render_state.FontCount++]);
}


static void register_buffers(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena)
{
    for(i32 index = render_state.BufferCount; index < renderer.BufferCount; index++)
    {
        BufferData data = renderer.Buffers[index];
        
        if(data.IndexBufferCount == 0)
        {
            register_vertex_buffer(render_state, data.VertexBuffer, (i32)data.VertexBufferSize, data.ShaderType, perm_arena, data.ExistingHandle);
        }
        else
        {
            register_buffers(render_state, data.VertexBuffer, data.VertexBufferSize, data.IndexBuffer, data.IndexBufferCount, data.IndexBufferSize, data.HasNormals, data.HasUVs, data.Skinned, data.ExistingHandle);
            
        }
    }
    
    for(i32 index = 0; index < renderer.UpdatedBufferHandleCount; index++)
    {
        BufferData data = renderer.Buffers[renderer.UpdatedBufferHandles[index]];
        if(data.IndexBufferCount == 0)
        {
            register_vertex_buffer(render_state, data.VertexBuffer, (i32)data.VertexBufferSize, data.ShaderType, perm_arena, data.ExistingHandle);
        }
        else
        {
            register_buffers(render_state, data.VertexBuffer, data.VertexBufferSize, data.IndexBuffer, data.IndexBufferCount, data.IndexBufferSize, data.HasNormals, data.HasUVs, data.Skinned, data.ExistingHandle);
            
        }
    }
    renderer.UpdatedBufferHandleCount = 0;
}

static void render_commands(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena)
{
    for(i32 index = render_state.FontCount; index < renderer.FontCount; index++)
    {
        FontData data = renderer.Fonts[index];
        
        load_font(render_state, data.Path, data.Size);
    }
    
    auto& camera = renderer.Cameras[renderer.CurrentCameraHandle];
    auto& v = camera.ViewMatrix;
    
    for(i32 index = 0; index < renderer.LightCommandCount; index++)
    {
        const render_command& command =*((RenderCommand*)Renderer.LightCommands.CurrentBlock->Base + index);
        
        switch(command.Type)
        {
            case RenderCommand_Spotlight:
            {
                Spotlight& spotlight = RenderState.SpotlightData.Spotlights[RenderState.SpotlightData.NumLights++];
                
                auto pos = v * math::v4(command.Position, 1.0f);
                
                spotlight.Position[0] = pos.x;
                spotlight.Position[1] = pos.y;
                spotlight.Position[2] = pos.z;
                spotlight.Position[3] = 1;
                
                spotlight.Direction[0] = command.Spotlight.Direction.x;
                spotlight.Direction[1] = command.Spotlight.Direction.y;
                spotlight.Direction[2] = command.Spotlight.Direction.z;
                spotlight.Direction[3] = 0;
                
                spotlight.CutOff = command.Spotlight.CutOff;
                spotlight.OuterCutOff = command.Spotlight.OuterCutOff;
                
                spotlight.Ambient[0] = command.Spotlight.Ambient.x;
                spotlight.Ambient[1] = command.Spotlight.Ambient.y;
                spotlight.Ambient[2] = command.Spotlight.Ambient.z;
                spotlight.Ambient[3] = 1.0f;
                
                spotlight.Diffuse[0] = command.Spotlight.Diffuse.x;
                spotlight.Diffuse[1] = command.Spotlight.Diffuse.y;
                spotlight.Diffuse[2] = command.Spotlight.Diffuse.z;
                spotlight.Diffuse[3] = 1.0f;
                spotlight.Specular[0] = command.Spotlight.Specular.x;
                spotlight.Specular[1] = command.Spotlight.Specular.y;
                spotlight.Specular[2] = command.Spotlight.Specular.z;
                spotlight.Specular[3] = 1.0f;
                
                spotlight.Constant = command.Spotlight.Constant;
                spotlight.Linear = command.Spotlight.Linear;
                spotlight.Quadratic = command.Spotlight.Quadratic;
            }
            break;
            case RenderCommand_DirectionalLight:
            {
                DirectionalLight& directional_light = RenderState.DirectionalLightData.DirectionalLights[RenderState.DirectionalLightData.NumLights++];
                
                directional_light.Direction[0] = command.DirectionalLight.Direction.x;
                directional_light.Direction[1] = command.DirectionalLight.Direction.y;
                directional_light.Direction[2] = command.DirectionalLight.Direction.z;
                directional_light.Direction[3] = 0;
                
                directional_light.Ambient[0] = command.DirectionalLight.Ambient.x;
                directional_light.Ambient[1] = command.DirectionalLight.Ambient.y;
                directional_light.Ambient[2] = command.DirectionalLight.Ambient.z;
                directional_light.Ambient[3] = 1;
                directional_light.Diffuse[0] = command.DirectionalLight.Diffuse.x;
                directional_light.Diffuse[1] = command.DirectionalLight.Diffuse.y;
                directional_light.Diffuse[2] = command.DirectionalLight.Diffuse.z;
                directional_light.Diffuse[3] = 1;
                directional_light.Specular[0] = command.DirectionalLight.Specular.x;
                directional_light.Specular[1] = command.DirectionalLight.Specular.y;
                directional_light.Specular[2] = command.DirectionalLight.Specular.z;
                directional_light.Specular[3] = 1;
            }
            break;
            case RenderCommand_PointLight:
            {
                PointLight& point_light = RenderState.PointLightData.PointLights[RenderState.PointLightData.NumLights++];
                
                auto pos = v * math::v4(command.Position, 1.0f);
                
                point_light.Position[0] = pos.x;
                point_light.Position[1] = pos.y;
                point_light.Position[2] = pos.z;
                point_light.Position[3] = 1.0f;
                
                point_light.Ambient[0] = command.PointLight.Ambient.x;
                point_light.Ambient[1] = command.PointLight.Ambient.y;
                point_light.Ambient[2] = command.PointLight.Ambient.z;
                
                point_light.Diffuse[0] = command.PointLight.Diffuse.x;
                point_light.Diffuse[1] = command.PointLight.Diffuse.y;
                point_light.Diffuse[2] = command.PointLight.Diffuse.z;
                
                point_light.Specular[0] = command.PointLight.Specular.x;
                point_light.Specular[1] = command.PointLight.Specular.y;
                point_light.Specular[2] = command.PointLight.Specular.z;
                
                point_light.Constant = command.PointLight.Constant;
                point_light.Linear = command.PointLight.Linear;
                point_light.Quadratic = command.PointLight.Quadratic;
            }
            break;
            default:
            break;
        }
    }
    
    update_lighting_data(RenderState);
    renderer.LightCommandCount = 0;
    render_state.SpotlightData.NumLights = 0;
    render_state.DirectionalLightData.NumLights = 0;
    render_state.PointLightData.NumLights = 0;
    clear(&renderer.LightCommands);
    
    glEnable(GL_DEPTH_TEST);
    for(i32 index = 0; index < renderer.CommandCount; index++)
    {
        const render_command& command = *((RenderCommand*)Renderer.Commands.CurrentBlock->Base + index);
        
        switch(command.Type)
        {
            case RenderCommand_Line:
            {
                render_line(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_Text:
            {
                render_text(command, RenderState);
            }
            break;
            case RenderCommand_Quad:
            {
                render_quad(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_Sprite:
            {
                //RenderSprite(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Model:
            {
                render_model(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
                
            }
            break;
            case RenderCommand_Buffer:
            {
                render_buffer(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_WireframeCube:
            {
                render_wireframe_cube(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_DepthTest:
            {
                if(command.DepthTest.On)
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
                RenderState.CurrentExtraShader = command.Shader.Handle;
                RenderState.ShaderAttributes = command.Shader.Attributes;
                RenderState.ShaderAttributeCount = command.Shader.AttributeCount;
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
    
    renderer.CommandCount = 0;
    clear(&renderer.Commands);
    
    glDisable(GL_DEPTH_TEST);
    
    for(i32 index = 0; index < renderer.UICommandCount; index++)
    {
        const render_command& command = *((RenderCommand*)Renderer.UICommands.CurrentBlock->Base + index);
        
        switch(command.Type)
        {
            case RenderCommand_Line:
            {
                render_line(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_Text:
            {
                render_text(command, RenderState);
            }
            break;
            case RenderCommand_Quad:
            {
                render_quad(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_Sprite:
            {
                //RenderSprite(Command, RenderState, Renderer, Camera.ProjectionMatrix, Camera.ViewMatrix);
            }
            break;
            case RenderCommand_Model:
            {
                render_model(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_Buffer:
            {
                render_buffer(command, RenderState, camera.ProjectionMatrix, camera.ViewMatrix);
            }
            break;
            case RenderCommand_DepthTest:
            {
                // @Incomplete: Do we need depth test commands for UI stuff?
            }
            break;
            case RenderCommand_ShaderStart:
            {
                RenderState.CurrentExtraShader = command.Shader.Handle;
                RenderState.ShaderAttributes = command.Shader.Attributes;
                RenderState.ShaderAttributeCount = command.Shader.AttributeCount;
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
    
    renderer.UICommandCount = 0;
    clear(&renderer.UICommands);
    
}

static void render(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena, r64 delta_time)
{
    if(renderer.WindowMode != render_state.WindowMode)
    {
        glfwDestroyWindow(render_state.Window);
        create_open_gl_window(render_state, renderer.WindowMode, render_state.WindowTitle, render_state.WindowWidth, render_state.WindowHeight);
        glfwSetInputMode(render_state.Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetFramebufferSizeCallback(render_state.Window, framebuffer_size_callback);
        
        glfwMakeContextCurrent(render_state.Window);
        
        glfwSwapInterval(0);
        
        glfwGetFramebufferSize(render_state.Window, &render_state.WindowWidth, &render_state.WindowHeight);
        glViewport(0, 0, render_state.WindowWidth, render_state.WindowHeight);
        glDisable(GL_DITHER);
        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glDepthFunc(GL_LESS);
        
        glfwSetWindowUserPointer(render_state.Window, &render_state);
        glfwSetKeyCallback(render_state.Window, key_callback);
        glfwSetCharCallback(render_state.Window, character_callback);
        glfwSetCursorPosCallback(render_state.Window, cursor_position_callback);
        glfwSetMouseButtonCallback(render_state.Window, mouse_button_callback);
        glfwSetScrollCallback(render_state.Window, scroll_callback);
        
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        memcpy(render_state.Viewport, viewport, sizeof(GLint) * 4);
    }
    
    load_extra_shaders(render_state, renderer);
    
    load_textures(render_state, renderer);
    
    render_state.CurrentExtraShader = -1;
    render_state.ShaderAttributeCount = 0;
    
    auto& camera = renderer.Cameras[renderer.CurrentCameraHandle];
    camera.ViewportWidth = render_state.ScaleFromWidth;
    camera.ViewportHeight = render_state.ScaleFromHeight;
    
    render_state.ScaleX = 2.0f / render_state.ScreenWidth;
    render_state.ScaleY = 2.0f / render_state.ScreenHeight;
    render_state.PixelsPerUnit = renderer.PixelsPerUnit;
    renderer.ScaleX = render_state.ScaleX;
    renderer.ScaleY = render_state.ScaleY;
    
    register_buffers(render_state, renderer, perm_arena);
    
    if((renderer.FrameLock != 0 && render_state.FrameDelta <= 0.0) || renderer.FrameLock == 0)
    {
        renderer.FPS = 1.0 / render_state.TotalDelta;
        renderer.CurrentFrame++;
        renderer.FPSSum += renderer.FPS;
        
        if(renderer.CurrentFrame == 60)
        {
            renderer.CurrentFrame = 0;
            renderer.AverageFPS = renderer.FPSSum / 60.0f;
            renderer.FPSSum = 0.0;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, render_state.FrameBuffer);
        
        glBindTexture(GL_TEXTURE_2D, render_state.TextureColorBuffer);
        
        glEnable(GL_DEPTH_TEST);
        
        glDepthFunc(GL_LESS);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glClearColor(renderer.ClearColor.r, renderer.ClearColor.g, renderer.ClearColor.b, renderer.ClearColor.a);
        
        render_commands(render_state, renderer, perm_arena);
        render_state.BoundTexture = -1; 
        
        // We have to reset the bound texture to nothing, since we're about to bind other textures
        // Second pass
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glClearColor(renderer.ClearColor.r, renderer.ClearColor.g, renderer.ClearColor.b, renderer.ClearColor.a);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindVertexArray(render_state.FrameBufferVAO);
        
        use_shader(&render_state.FrameBufferShader);
        
        set_float_uniform(render_state.FrameBufferShader.Program, "contrast", render_state.Contrast);
        set_float_uniform(render_state.FrameBufferShader.Program, "brightness", render_state.Brightness);
        set_int_uniform(render_state.FrameBufferShader.Program, "ignoreLight",  true); // @Incomplete: Lighting
        set_mat4_uniform(render_state.FrameBufferShader.Program,"P", camera.ProjectionMatrix);
        set_mat4_uniform(render_state.FrameBufferShader.Program,"V", camera.ViewMatrix);
        set_vec2_uniform(render_state.FrameBufferShader.Program, "screenSize", math::v2((r32)render_state.WindowWidth,(r32)render_state.WindowHeight));
        
        glUniform1i((GLint)render_state.FrameBufferTex0Loc, 0);
        
        glUniform1i((GLint)render_state.FrameBufferTex1Loc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, render_state.TextureColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        /*glBindTexture(GL_TEXTURE_2D, RenderState.LightingTextureColorBuffer);
        RenderState.BoundTexture = RenderState.LightingTextureColorBuffer;
        */
        //Enable this if we don't do gamma correction in framebuffer shader
        //glEnable(GL_FRAMEBUFFER_SRGB);
        
        glDrawElements(GL_TRIANGLES, sizeof(render_state.QuadIndices), GL_UNSIGNED_INT, (void*)0); 
        
        glActiveTexture(GL_TEXTURE0);
        
        glfwSwapBuffers(render_state.Window);
        
        if(renderer.FrameLock != 0)
        {
            render_state.TotalDelta = 0.0;
            render_state.FrameDelta += 1.0 / renderer.FrameLock;
        }
        else
        {
            render_state.TotalDelta = delta_time;
        }
    }
    else
    {
        
        clear(&renderer.LightCommands);
        renderer.LightCommandCount = 0;
        clear(&renderer.Commands);
        renderer.CommandCount = 0;
        clear(&renderer.UICommands);
        renderer.UICommandCount = 0;
    }
    
    render_state.FrameDelta -= delta_time;
    render_state.TotalDelta += delta_time;
}
