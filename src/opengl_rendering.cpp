
//#define GL_DEBUG

#ifdef GL_DEBUG
GLuint debug_attrib(u32 program, char* str, char* file, int line)
{
    Debug("File: %s, Line: %d\n", file, line);
    return (GLuint)glGetAttribLocation(program, str);
}

void debug_attrib_array(u32 location, char* file, int line)
{
    Debug("File: %s, Line: %d\n", file, line);
    glEnableVertexAttribArray(location);
}

// Macro redefinition, but used for debug
#define glEnableVertexAttribArray(loc) debug_attrib_array(loc, __FILE__, __LINE__)
#define glGetAttribLocation(shader, str) debug_attrib(shader, str, __FILE__, __LINE__)
#endif


static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %d - %s\n", error, description);
}

static void show_mouse_cursor(RenderState& render_state, b32 show)
{
    if (show)
    {
        glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
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
    if (Type == GL_DEBUG_TYPE_ERROR)
    {
        Debug("GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s, source = %x, id = %ud, length %ud= \n",
              (Type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
              Type, Severity, Message, Source, Id, Length);
    }
}

void frame_buffer_size_callback(GLFWwindow *window, int width, int height)
{
    RenderState* render_state = (RenderState*)glfwGetWindowUserPointer(window);
    glfwSetWindowAspectRatio(window, 16, 9);
    glViewport(0, 0, width, height);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    memcpy(render_state->viewport, viewport, sizeof(GLint) * 4);
    
    render_state->window_width = width;
    render_state->window_height = height;
    
    // Change resolution of frame_buffer texture
    //@Incomplete: This should be done with lower resolutions and just be upscaled maybe? We need fixed resolutions
    glBindTexture(GL_TEXTURE_2D, render_state->texture_color_buffer);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, (GLsizei)render_state->scale_from_width, (GLsizei)render_state->scale_from_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->texture_color_buffer, 0);
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
    glfwDestroyWindow(render_state.window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static b32 should_close_window(RenderState& render_state)
{
    return glfwWindowShouldClose(render_state.window);
}

static GLint shader_compilation_error_checking(MemoryArena* arena,const char* shader_name, GLuint shader)
{
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
        auto temp_mem = begin_temporary_memory(arena);
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);
        
        // The maxLength includes the NULL character
        GLchar* error_log = push_size(arena, max_length, GLchar);
        
        glGetShaderInfoLog(shader, max_length, &max_length, error_log);
        
        Debug("SHADER Compilation error - %s\n", shader_name);
        Debug("%s", error_log);
        
        glDeleteShader(shader); // Don't leak the shader.
        end_temporary_memory(temp_mem);
    }
    return is_compiled;
}

static GLuint load_extra_shader(MemoryArena* arena, ShaderData& shader_data, RenderState& render_state)
{
    // @Incomplete: vertexAttribPointers?
    Shader* shader = &render_state.extra_shaders[render_state.extra_shader_index++];
    shader->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shader->vertex_shader, 1, &shader_data.vertex_shader_content, NULL);
    glCompileShader(shader->vertex_shader);
    
    if (!shader_compilation_error_checking(arena, concat(shader_data.name, ".vert", arena), shader->vertex_shader))
    {
        shader->program = 0;
        return GL_FALSE;
    }
    
    shader->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    glShaderSource(shader->fragment_shader, 1, &shader_data.fragment_shader_content, NULL);
    glCompileShader(shader->fragment_shader);
    
    if (!shader_compilation_error_checking(arena, concat(shader_data.name, ".frag", arena), shader->fragment_shader))
    {
        shader->program = 0;
        return GL_FALSE;
    }
    
    shader->program = glCreateProgram();
    
    glAttachShader(shader->program, shader->vertex_shader);
    glAttachShader(shader->program, shader->fragment_shader);
    glLinkProgram(shader->program);
    
    return GL_TRUE;
}

static GLuint load_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    auto temp_mem = begin_temporary_memory(perm_arena);
    shd->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    char* vertex_string = concat(file_path, ".vert", perm_arena);
    GLchar *vertex_text = load_shader_from_file(vertex_string, perm_arena);
    
    if (vertex_text)
    {
        glShaderSource(shd->vertex_shader, 1, &vertex_text, NULL);
        glCompileShader(shd->vertex_shader);
        
        if (!shader_compilation_error_checking(perm_arena, file_path, shd->vertex_shader))
        {
            end_temporary_memory(temp_mem);
            shd->program = 0;
            return GL_FALSE;
        }
        
        shd->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        char* fragment_string = concat(file_path, ".frag", perm_arena);
        GLchar *fragment_text = load_shader_from_file(fragment_string, perm_arena);
        
        glShaderSource(shd->fragment_shader, 1, &fragment_text, NULL);
        glCompileShader(shd->fragment_shader);
        
        if (!shader_compilation_error_checking(perm_arena, file_path, shd->fragment_shader))
        {
            end_temporary_memory(temp_mem);
            shd->program = 0;
            return GL_FALSE;
        }
        
        shd->program = glCreateProgram();
        
        glAttachShader(shd->program, shd->vertex_shader);
        glAttachShader(shd->program, shd->fragment_shader);
        glLinkProgram(shd->program);
        
        end_temporary_memory(temp_mem);
        return GL_TRUE;
    }
    end_temporary_memory(temp_mem);
    return GL_FALSE;
}

static GLuint load_vertex_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    auto temp_mem = begin_temporary_memory(perm_arena);
    shd->program = glCreateProgram();
    
    shd->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    char* vertex_string = concat(file_path, ".vert", perm_arena);
    GLchar *vertex_text = load_shader_from_file(vertex_string, perm_arena);
    glShaderSource(shd->vertex_shader, 1, &vertex_text, NULL);
    glCompileShader(shd->vertex_shader);
    
    if (!shader_compilation_error_checking(perm_arena, file_path, shd->vertex_shader))
    {
        end_temporary_memory(temp_mem);
        shd->program = 0;
        return GL_FALSE;
    }
    
    glAttachShader(shd->program, shd->vertex_shader);
    glAttachShader(shd->program, shd->fragment_shader);
    
    glLinkProgram(shd->program);
    
    end_temporary_memory(temp_mem);
    return GL_TRUE;
}

static GLuint load_fragment_shader(const char* file_path, Shader *shd, MemoryArena* perm_arena)
{
    shd->program = glCreateProgram();
    shd->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    
    auto temp_mem = begin_temporary_memory(perm_arena);
    char* fragment_string = concat(file_path, ".frag", perm_arena);
    
    GLchar *fragment_text = load_shader_from_file(fragment_string, perm_arena);
    glShaderSource(shd->fragment_shader, 1, &fragment_text, NULL);
    end_temporary_memory(temp_mem);
    
    glCompileShader(shd->fragment_shader);
    
    
    if (!shader_compilation_error_checking(perm_arena, file_path, shd->fragment_shader))
    {
        shd->program = 0;
        return GL_FALSE;
    }
    glAttachShader(shd->program, shd->vertex_shader);
    glAttachShader(shd->program, shd->fragment_shader);
    glLinkProgram(shd->program);
    
    return GL_TRUE;
}

static void use_shader(Shader *shader)
{
    glUseProgram(shader->program);
}

static void register_buffers(RenderState& render_state, GLfloat* vertex_buffer, long vertex_buffer_size, GLuint* index_buffer, i32 index_buffer_count, long index_buffer_size, b32 has_normals, b32 has_uvs, b32 skinned, i32 buffer_handle = -1)
{
    Buffer* buffer = &render_state.buffers[buffer_handle == -1 ? render_state.buffer_count : buffer_handle];
    
    buffer->vertex_buffer_size = (GLint)vertex_buffer_size;
    buffer->index_buffer_count = (GLint)index_buffer_count;
    
    if (buffer->vao == 0)
        glGenVertexArrays(1, &buffer->vao);
    
    glBindVertexArray(buffer->vao);
    
    if (buffer->vbo == 0)
        glGenBuffers(1, &buffer->vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);
    
    i32 bone_info_size = skinned ? 8 : 0;
    
    if (has_normals && has_uvs)
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
    else if (has_normals)
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
    else if (has_uvs)
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
    
    glGenBuffers(1, &buffer->ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, index_buffer, GL_STATIC_DRAW);
    
    if (buffer_handle == -1)
        render_state.buffer_count++;
    
    glBindVertexArray(0);
}

static void register_vertex_buffer(RenderState& render_state, GLfloat* buffer_data, i32 size, ShaderType shader_type, MemoryArena* perm_arena, i32 buffer_handle = -1)
{
    Buffer* buffer = &render_state.buffers[buffer_handle == -1 ? render_state.buffer_count : buffer_handle];
    
    buffer->vertex_buffer_size = size;
    buffer->index_buffer_size = 0;
    buffer->ibo = 0;
    
    if (buffer->vao == 0)
        glGenVertexArrays(1, &buffer->vao);
    
    glBindVertexArray(buffer->vao);
    
    if (buffer->vbo == 0)
        glGenBuffers(1, &buffer->vbo);
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)(sizeof(GLfloat) * size), buffer_data, GL_STATIC_DRAW);
    
    if (!render_state.shaders[shader_type].loaded)
    {
        load_shader(shader_paths[shader_type], &render_state.shaders[shader_type], perm_arena);
    }
    else
        use_shader(&render_state.shaders[shader_type]);
    
    auto position_location = (GLuint)glGetAttribLocation(render_state.tile_shader.program, "pos");
    auto texcoord_location = (GLuint)glGetAttribLocation(render_state.tile_shader.program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    if (buffer_handle == -1)
        render_state.buffer_count++;
}

static void render_setup(RenderState *render_state, MemoryArena* perm_arena)
{
    // @Cleanup: Not sure if a fallback is a good way of dealing with this
    if (render_state->scale_from_width == 0 || render_state->scale_from_height == 0)
    {
        render_state->scale_from_width = render_state->window_width;
        render_state->scale_from_height = render_state->window_height;
    }
    
    render_state->font_count = 0;
    
    // Framebuffer 1
    glGenFramebuffers(1, &render_state->frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, render_state->frame_buffer);
    
    glGenTextures(1, &render_state->texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, render_state->texture_color_buffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, render_state->scale_from_width, render_state->scale_from_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->texture_color_buffer, 0
        );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Debug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    GLuint depth_buffer;
    glGenRenderbuffers(1, &depth_buffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, render_state->scale_from_width, render_state->scale_from_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);
    
    glGenFramebuffers(1, &render_state->lighting_frame_buffer);
    glBindFramebuffer(GL_FRAMEBUFFER, render_state->lighting_frame_buffer);
    
    glGenTextures(1, &render_state->lighting_texture_color_buffer);
    glBindTexture(GL_TEXTURE_2D, render_state->lighting_texture_color_buffer);
    
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, render_state->scale_from_width, render_state->scale_from_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL
        );
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->lighting_texture_color_buffer, 0);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Debug("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
    
    // FrameBuffer vao
    glGenVertexArrays(1, &render_state->frame_buffer_vao);
    glBindVertexArray(render_state->frame_buffer_vao);
    glGenBuffers(1, &render_state->frame_buffer_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->frame_buffer_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->sprite_quad_vertices_size, render_state->frame_buffer_vertices, GL_DYNAMIC_DRAW);
    
    render_state->frame_buffer_shader.type = SHADER_FRAME_BUFFER;
    
    load_shader(shader_paths[SHADER_FRAME_BUFFER], &render_state->frame_buffer_shader, perm_arena);
    
    auto pos_loc = (GLuint)glGetAttribLocation(render_state->frame_buffer_shader.program, "pos");
    auto tex_loc = (GLuint)glGetAttribLocation(render_state->frame_buffer_shader.program, "texcoord");
    
    glEnableVertexAttribArray(pos_loc);
    glVertexAttribPointer(pos_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(tex_loc);
    glVertexAttribPointer(tex_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    
    render_state->frame_buffer_tex0_loc = (GLuint)glGetUniformLocation(render_state->frame_buffer_shader.program, "tex");
    render_state->frame_buffer_tex1_loc = (GLuint)glGetUniformLocation(render_state->frame_buffer_shader.program, "lightingTex");
    
    glGenBuffers(1, &render_state->quad_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state->quad_indices), render_state->quad_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    //Sprite
    glGenVertexArrays(1, &render_state->sprite_vao);
    glBindVertexArray(render_state->sprite_vao);
    glGenBuffers(1, &render_state->sprite_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->sprite_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->sprite_quad_vertices_size, render_state->sprite_quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    
    render_state->texture_shader.type = SHADER_TEXTURE;
    load_shader(shader_paths[SHADER_TEXTURE], &render_state->texture_shader, perm_arena);
    
    auto position_location = (GLuint)glGetAttribLocation(render_state->texture_shader.program, "pos");
    auto texcoord_location = (GLuint)glGetAttribLocation(render_state->texture_shader.program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    //Animation
    //Sprite
    glGenVertexArrays(1, &render_state->sprite_sheet_vao);
    glBindVertexArray(render_state->sprite_sheet_vao);
    glGenBuffers(1, &render_state->sprite_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->sprite_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->sprite_quad_vertices_size, render_state->sprite_quad_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    
    render_state->texture_shader.type = SHADER_SPRITESHEET;
    load_shader(shader_paths[SHADER_SPRITESHEET], &render_state->spritesheet_shader, perm_arena);
    
    position_location = (GLuint)glGetAttribLocation(render_state->spritesheet_shader.program, "pos");
    texcoord_location = (GLuint)glGetAttribLocation(render_state->spritesheet_shader.program, "texcoord");
    
    glEnableVertexAttribArray(position_location);
    glVertexAttribPointer(position_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location);
    glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //tile
    glGenVertexArrays(1, &render_state->tile_vao);
    glBindVertexArray(render_state->tile_vao);
    glGenBuffers(1, &render_state->tile_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->tile_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->tile_quad_vertices_size, render_state->tile_quad_vertices, GL_STATIC_DRAW);
    
    render_state->tile_shader.type = SHADER_TILE;
    load_shader(shader_paths[SHADER_TILE], &render_state->tile_shader, perm_arena);
    render_state->tile_shader.loaded = true;
    
    auto position_location2 = (GLuint)glGetAttribLocation(render_state->tile_shader.program, "pos");
    auto texcoord_location2 = (GLuint)glGetAttribLocation(render_state->tile_shader.program, "texcoord");
    
    glEnableVertexAttribArray(position_location2);
    glVertexAttribPointer(position_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location2);
    glVertexAttribPointer(texcoord_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glBindVertexArray(0);
    
    //console
    glGenVertexArrays(1, &render_state->rect_vao);
    glBindVertexArray(render_state->rect_vao);
    glGenBuffers(1, &render_state->normal_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->normal_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->normal_quad_vertices_size, render_state->normal_quad_vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    
    render_state->rect_shader.type = SHADER_RECT;
    load_shader(shader_paths[SHADER_RECT], &render_state->rect_shader, perm_arena);
    
    auto position_location3 = (GLuint)glGetAttribLocation(render_state->rect_shader.program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &render_state->texture_rect_vao);
    glBindVertexArray(render_state->texture_rect_vao);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->sprite_quad_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    
    render_state->texture_rect_shader.type = SHADER_TEXTURE_RECT;
    load_shader(shader_paths[SHADER_TEXTURE_RECT], &render_state->texture_rect_shader, perm_arena);
    
    position_location2 = (GLuint)glGetAttribLocation(render_state->texture_rect_shader.program, "pos");
    texcoord_location2 = (GLuint)glGetAttribLocation(render_state->texture_rect_shader.program, "texcoord");
    
    glEnableVertexAttribArray(position_location2);
    glVertexAttribPointer(position_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(texcoord_location2);
    glVertexAttribPointer(texcoord_location2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    //wireframe
    glGenVertexArrays(1, &render_state->wireframe_vao);
    glBindVertexArray(render_state->wireframe_vao);
    glGenBuffers(1, &render_state->wireframe_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->wireframe_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->wireframe_quad_vertices_size, render_state->wireframe_quad_vertices, GL_DYNAMIC_DRAW);
    
    render_state->rect_shader.type = SHADER_WIREFRAME;
    load_shader(shader_paths[SHADER_WIREFRAME], &render_state->wireframe_shader, perm_arena);
    
    position_location3 = (GLuint)glGetAttribLocation(render_state->wireframe_shader.program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    glGenVertexArrays(1, &render_state->wireframe_cube_vao);
    glBindVertexArray(render_state->wireframe_cube_vao);
    glGenBuffers(1, &render_state->wireframe_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->wireframe_cube_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 32, render_state->wireframe_cube_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glGenBuffers(1, &render_state->cube_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->cube_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * CUBE_INDICES, render_state->wireframe_cube_indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    
    //
    
    glGenVertexArrays(1, &render_state->isometric_vao);
    glBindVertexArray(render_state->isometric_vao);
    glGenBuffers(1, &render_state->isometric_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->isometric_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state->wireframe_quad_vertices_size, render_state->isometric_quad_vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->quad_index_buffer);
    
    render_state->rect_shader.type = SHADER_WIREFRAME;
    load_shader(shader_paths[SHADER_WIREFRAME], &render_state->wireframe_shader, perm_arena);
    
    position_location3 = (GLuint)glGetAttribLocation(render_state->wireframe_shader.program, "pos");
    glEnableVertexAttribArray(position_location3);
    glVertexAttribPointer(position_location3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    glBindVertexArray(0);
    
    // Lines
    load_shader(shader_paths[SHADER_LINE], &render_state->line_shader, perm_arena);
    glGenVertexArrays(1, &render_state->line_vao);
    glBindVertexArray(render_state->line_vao);
    glGenBuffers(1, &render_state->line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state->line_vbo);
    
    glGenBuffers(1, &render_state->line_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state->line_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * LINE_INDICES, render_state->line_indices, GL_STATIC_DRAW);
    
    glBindVertexArray(0);
    
    //glGenVertexArrays(1, &RenderState->Primitivevao);
    //glBindVertexArray(RenderState->Primitivevao);
    //glGenBuffers(1, &RenderState->Primitivevbo);
    //glBindBuffer(GL_ARRAY_BUFFER, RenderState->Primitivevbo);
    
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    //glBindVertexArray(0);
    
    
    
    // Passthrough
    load_shader(shader_paths[SHADER_PASS_THROUGH], &render_state->passthrough_shader, perm_arena);
    glGenVertexArrays(1, &render_state->passthrough_vao);
    glBindVertexArray(render_state->passthrough_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(r32), 0);
    glBindVertexArray(0);
    
    //font
    render_state->standard_font_shader.type = SHADER_STANDARD_FONT;
    load_shader(shader_paths[SHADER_STANDARD_FONT], &render_state->standard_font_shader, perm_arena);
    
    // Light sources
    render_state->light_source_shader.type = SHADER_LIGHT_SOURCE;
    load_shader(shader_paths[SHADER_LIGHT_SOURCE], &render_state->light_source_shader, perm_arena);
    
    render_state->simple_model_shader.type = SHADER_SIMPLE_MODEL;
    load_shader(shader_paths[SHADER_SIMPLE_MODEL], &render_state->simple_model_shader, perm_arena);
    
    render_state->spotlight_data.num_lights = 0;
    glGenBuffers(1, &render_state->spotlight_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, render_state->spotlight_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotlightData), &render_state->spotlight_data, GL_DYNAMIC_DRAW);
    
    u32 block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "spotlights");
    
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    render_state->directional_light_data.num_lights = 0;
    glGenBuffers(1, &render_state->directional_light_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, render_state->directional_light_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightData), &render_state->directional_light_data, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "directionalLights");
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    render_state->point_light_data.num_lights = 0;
    glGenBuffers(1, &render_state->point_light_ubo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, render_state->point_light_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightData), &render_state->point_light_data, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "pointLights");
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void update_lighting_data(const RenderState& render_state)
{
    glBindBuffer(GL_UNIFORM_BUFFER, render_state.spotlight_ubo);
    GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if (p)
    {
        memcpy(p, &render_state.spotlight_data, sizeof(SpotlightData));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    
    glBindBuffer(GL_UNIFORM_BUFFER, render_state.directional_light_ubo);
    p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if (p)
    {
        memcpy(p, &render_state.directional_light_data, sizeof(DirectionalLightData));
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    
    glBindBuffer(GL_UNIFORM_BUFFER, render_state.point_light_ubo);
    p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
    if (p)
    {
        memcpy(p, &render_state.point_light_data, sizeof(PointLightData));
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
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, (GLvoid*)data.image_data);
    
    texture->texture_handle = texture_handle;
    stbi_image_free(data.image_data);
    
    return GL_TRUE;
}

static void load_textures(RenderState& render_state, Renderer& renderer)
{
    for (i32 index = render_state.texture_index; index < renderer.texture_count; index++)
    {
        load_texture(renderer.texture_data[index], &render_state.texture_array[render_state.texture_index++]);
    }
}

static void load_extra_shaders(RenderState& render_state, Renderer& renderer)
{
    for (i32 index = render_state.extra_shader_index; index < renderer.shader_count; index++)
    {
        load_extra_shader(&render_state.arena, renderer.shader_data[index], render_state);
    }
}

static void create_open_gl_window(RenderState& render_state, WindowMode window_mode, const char* title, i32 width, i32 height)
{
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    i32 screen_width = width;
    i32 screen_height = height;
    
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    render_state.window_mode = window_mode;
    render_state.window_title = push_string(&render_state.arena, strlen(title) + 1);
    strcpy(render_state.window_title, title);
    
    if (window_mode == FM_BORDERLESS)
    {
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        screen_width = mode->width;
        screen_height = mode->height;
    }
    
    if (window_mode == FM_WINDOWED)
    {
        monitor = NULL;
    }
    
    render_state.window = glfwCreateWindow(screen_width, screen_height, render_state.window_title, monitor,
                                           NULL);
    
    //center window on screen (windowed?)
    
    if (window_mode == FM_WINDOWED)
    {
        int frame_buffer_width, frame_buffer_height;
        
        glfwGetFramebufferSize(render_state.window, &frame_buffer_width, &frame_buffer_height);
        glfwSetWindowPos(render_state.window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
    }
}

static void initialize_opengl(RenderState& render_state, Renderer& renderer, ConfigData* config_data, MemoryArena* perm_arena)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);
    
    glfwSetErrorCallback(error_callback);
    
    render_state.scale_from_width = config_data->scale_from_width;
    render_state.scale_from_height = config_data->scale_from_height;
    
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
    
    render_state.contrast = config_data->contrast;
    render_state.brightness = config_data->brightness;
    
    create_open_gl_window(render_state, config_data->fullscreen, config_data->title, config_data->screen_width, config_data->screen_height);
    renderer.window_mode = render_state.window_mode;
    
    if (!render_state.window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    
    glfwSetFramebufferSizeCallback(render_state.window, frame_buffer_size_callback);
    
    glfwMakeContextCurrent(render_state.window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    
    glfwSwapInterval(0);
    
    glfwGetFramebufferSize(render_state.window, &render_state.window_width, &render_state.window_height);
    glViewport(0, 0, render_state.window_width, render_state.window_height);
    
    // Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)message_callback, 0);
    
    render_state.screen_width = config_data->screen_width;
    render_state.screen_height = config_data->screen_height;
    if (render_state.screen_width != 0)
    {
        render_state.dpi_scale = render_state.window_width / render_state.screen_width;
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
    
    glfwSetWindowUserPointer(render_state.window, &render_state);
    glfwSetKeyCallback(render_state.window, key_callback);
    glfwSetCharCallback(render_state.window, character_callback);
    glfwSetCursorPosCallback(render_state.window, cursor_position_callback);
    glfwSetMouseButtonCallback(render_state.window, mouse_button_callback);
    glfwSetScrollCallback(render_state.window, scroll_callback);
    
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    memcpy(render_state.viewport, viewport, sizeof(GLint) * 4);
    
    renderer.window_width = render_state.screen_width;
    renderer.window_height = render_state.screen_height;
    
    memcpy(renderer.viewport, render_state.viewport, sizeof(i32) * 4);
    renderer.viewport[0] /= render_state.dpi_scale;
    renderer.viewport[1] /= render_state.dpi_scale;
    renderer.viewport[2] /= render_state.dpi_scale;
    renderer.viewport[3] /= render_state.dpi_scale;
    
    printf("Viewport %d %d %d %d\n", renderer.viewport[0], renderer.viewport[1], renderer.viewport[2], renderer.viewport[3]);
    
    controller_present();
    
    renderer.should_close = false;
    render_setup(&render_state, perm_arena);
}

static void reload_vertex_shader(ShaderType type, RenderState* render_state, MemoryArena* perm_arena)
{
    glDeleteProgram(render_state->shaders[type].program);
    glDeleteShader(render_state->shaders[type].vertex_shader);
    load_vertex_shader(shader_paths[type], &render_state->shaders[type], perm_arena);
}

static void reload_fragment_shader(ShaderType type, RenderState* render_state, MemoryArena* perm_arena)
{
    glDeleteProgram(render_state->shaders[type].program);
    glDeleteShader(render_state->shaders[type].fragment_shader);
    load_fragment_shader(shader_paths[type], &render_state->shaders[type], perm_arena);
    
    render_state->spotlight_data.num_lights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, render_state->spotlight_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SpotlightData), &render_state->spotlight_data, GL_DYNAMIC_DRAW);
    
    u32 block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "spotlights");
    
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 0);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    render_state->directional_light_data.num_lights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, render_state->directional_light_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(DirectionalLightData), &render_state->directional_light_data, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "directionalLights");
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 1);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    
    
    render_state->point_light_data.num_lights = 0;
    glBindBufferBase(GL_UNIFORM_BUFFER, 2, render_state->point_light_ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(PointLightData), &render_state->point_light_data, GL_DYNAMIC_DRAW);
    
    block_index = glGetUniformBlockIndex(render_state->simple_model_shader.program, "pointLights");
    glUniformBlockBinding(render_state->simple_model_shader.program, block_index, 2);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void reload_assets(RenderState& render_state, AssetManager* asset_manager, MemoryArena* perm_arena)
{
    for (int i = 0; i < SHADER_COUNT; i++)
    {
        if (asset_manager->dirty_vertex_shader_indices[i] == 1)
        {
            Debug("Reloading vertex shader type: %s\n", shader_enum_to_str((ShaderType)i));
            reload_vertex_shader((ShaderType)i, &render_state, perm_arena);
            asset_manager->dirty_vertex_shader_indices[i] = 0;
        }
        
        if (asset_manager->dirty_fragment_shader_indices[i] == 1)
        {
            Debug("Reloading fragment shader type: %s\n", shader_enum_to_str((ShaderType)i));
            reload_fragment_shader((ShaderType)i, &render_state, perm_arena);
            asset_manager->dirty_fragment_shader_indices[i] = 0;
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

static void set_vec2_uniform(GLuint shader_handle, const char *uniform_name, math::Vec2 value)
{
    glUniform2f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y);
}

void set_vec3_uniform(GLuint shader_handle, const char *uniform_name, math::Vec3 value)
{
    glUniform3f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z);
}

static void set_vec4_uniform(GLuint shader_handle, const char *uniform_name, math::Vec4 value)
{
    glUniform4f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z, value.w);
}

static void set_mat4_uniform(GLuint shader_handle, const char *uniform_name, math::Mat4 v)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_handle, uniform_name), 1, GL_TRUE, &v[0][0]);
}

void set_vec4_array_uniform(GLuint shader_handle, const char *uniform_name, math::Vec4* value, u32 length)
{
    glUniform4fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat*)&value[0]);
}

void set_float_array_uniform(GLuint shader_handle, const char *uniform_name, r32* value, u32 length)
{
    glUniform1fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat*)&value[0]);
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static void render_line(RenderState& render_state, math::Vec4 color, math::Vec3 start, math::Vec3 end, math::Mat4 projection_matrix = math::Mat4(), math::Mat4 view_matrix = math::Mat4(), r32 line_width = 1.0f, b32 is_ui = false)
{
    if (is_ui)
    {
        start.x *= render_state.scale_x;
        start.x -= 1;
        start.y *= render_state.scale_y;
        start.y -= 1;
        end.x *= render_state.scale_x;
        end.x -= 1;
        end.y *= render_state.scale_y;
        end.y -= 1;
        start.z = 0.0f;
        end.z = 0.0f;
    }
    
    auto& shader = render_state.line_shader;
    use_shader(&shader);
    
    glBindVertexArray(render_state.line_vao);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.line_vbo);
    
    auto width = 0.005f * line_width;
    
    // ONLY FOR 2D!!!
    auto dx = end.x - start.x;
    auto dy = end.y - start.y;
    auto normal = math::normalize(math::Vec2(-dy, dx));
    
    // Double vertices
    // 1.0f and -1.0f are Miters
    
    
    GLfloat points[24] = {
        start.x, start.y, start.z, normal.x, normal.y, -1.0f,
        start.x, start.y, start.z, normal.x, normal.y, 1.0f,
        end.x, end.y, end.z, normal.x, normal.y, -1.0f,
        end.x, end.y, end.z, normal.x, normal.y, 1.0f };
    
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), &points[0], GL_DYNAMIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0); // pos
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat))); // miter
    
    auto m = math::Mat4(1.0f);
    
    set_mat4_uniform(shader.program, "model", m);
    set_mat4_uniform(shader.program, "view", view_matrix);
    set_mat4_uniform(shader.program, "projection", projection_matrix);
    set_vec4_uniform(shader.program, "color", color);
    set_float_uniform(shader.program, "thickness", width);
    set_int_uniform(shader.program, "isUI", is_ui);
    
    //glDrawArrays(GL_TRIANGLES, 0, 4);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.line_ebo);
    glDrawElements(GL_TRIANGLES, LINE_INDICES, GL_UNSIGNED_INT, BUFFER_OFFSET(0));
    glBindVertexArray(0);
}

/*
static void RenderLine(render_state& RenderState, math::Vec4 Color, math::Vec3 Start, math::Vec3 End, math::m4 projection_matrix = math::m4(), math::m4 view_matrix = math::m4(), r32 LineWidth = 1.0f, b32 IsUI = false)
{
if(IsUI)
{
Start.x *= RenderState.scale_x;
Start.x -= 1;
Start.y *= RenderState.scale_y;
Start.y -= 1;
End.x *= RenderState.scale_x;
End.x -= 1;
End.y *= RenderState.scale_y;
End.y -= 1;
Start.z = 0.0f;
End.z = 0.0f;
}

glLineWidth(LineWidth);
glBindBuffer(GL_ARRAY_BUFFER, RenderState.Primitivevbo);

GLfloat Points[6] = {Start.x, Start.y, Start.z, End.x, End.y, End.z};
glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(GLfloat), &Points[0], GL_DYNAMIC_DRAW);

glBindVertexArray(RenderState.Primitivevao);

auto& Shader = RenderState.PassthroughShader;
UseShader(&Shader);

SetMat4Uniform(Shader.program, "Projection", projection_matrix);
SetMat4Uniform(Shader.program, "View", view_matrix);

SetMat4Uniform(Shader.program, "Model", math::m4(1.0f));

SetVec4Uniform(Shader.program, "Color", Color);
SetIntUniform(Shader.program, "IsUI", (i32)IsUI);

glDrawArrays(GL_LINES, 0, 6);
glLineWidth(1.0f);
}
*/
// NOTE(Niels): Possible future use but buggy
void render_circle(RenderState& render_state, math::Vec4 color, r32 center_x, r32 center_y, r32 radius, b32 is_ui = true, math::Mat4 projection_matrix = math::Mat4(), math::Mat4 view_matrix = math::Mat4())
{
    if (is_ui)
    {
        center_x *= render_state.scale_x;
        center_x -= 1;
        center_y *= render_state.scale_y;
        center_y -= 1;
        radius *= render_state.scale_x;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, render_state.primitive_vbo);
    
    GLfloat points[720];
    
    i32 point_index = 0;
    
    for (i32 index = 0; index < 360; index++)
    {
        r32 radians = (index * PI) / 180.0f;
        points[point_index++] = math::cos(radians * radius);
        points[point_index++] = math::sin(radians * radius);
    }
    
    glBufferData(GL_ARRAY_BUFFER, 720 * sizeof(GLfloat), &points[0], GL_DYNAMIC_DRAW);
    
    auto& shader = render_state.rect_shader;
    use_shader(&shader);
    
    //draw upper part
    math::Mat4 model(1.0f);
    
    if (!is_ui)
    {
        set_mat4_uniform(shader.program, "Projection", projection_matrix);
        set_mat4_uniform(shader.program, "View", view_matrix);
    }
    
    set_float_uniform(shader.program, "isUI", (r32)is_ui);
    set_mat4_uniform(shader.program, "M", model);
    set_vec4_uniform(shader.program, "color", color);
    
    glDrawArrays(GL_LINE_LOOP, 0, 720);
}

static void render_quad(RenderMode mode, RenderState& render_state, math::Vec4 color, math::Vec3 position, b32 flipped, math::Vec3 size, math::Vec3 rotation, b32 with_origin, math::Vec2 origin, i32 shader_handle, ShaderAttribute* shader_attributes, i32 shader_attribute_count, b32 is_ui = true, i32 texture_handle = 0, b32 for_animation = false, math::Vec2 texture_size = math::Vec2(), math::Vec2i frame_size = math::Vec2i(), math::Vec2 texture_offset = math::Vec2(), math::Mat4 projection_matrix = math::Mat4(), math::Mat4 view_matrix = math::Mat4())
{
    if (is_ui)
    {
        position.x *= render_state.scale_x;
        position.x -= 1;
        position.y *= render_state.scale_y;
        position.y -= 1;
    }
    
    switch (mode)
    {
        case RENDER_FILL:
        {
            auto shader = render_state.rect_shader;
            
            if (texture_handle > 0)
            {
                glBindVertexArray(render_state.texture_rect_vao);
            }
            else
            {
                glBindVertexArray(render_state.rect_vao);
            }
            
            math::Vec2i pixel_size;
            
            math::Vec3 scale = size;
            
            if (texture_handle > 0)
            {
                if (for_animation)
                {
                    pixel_size = frame_size;
                    size = math::Vec3((size.x * frame_size.x) / render_state.pixels_per_unit, (size.y * frame_size.y) / render_state.pixels_per_unit, 0);
                }
                else
                {
                    if (frame_size.x != 0 && frame_size.y != 0)
                    {
                        pixel_size = math::Vec2i(frame_size.x, frame_size.y);
                        if (is_ui)
                        {
                            size = math::Vec3(size.x * frame_size.x, size.y * frame_size.y, 0);
                        }
                        else
                        {
                            size = math::Vec3((size.x * frame_size.x) / render_state.pixels_per_unit, (size.y * frame_size.y) / render_state.pixels_per_unit, 0);
                        }
                    }
                    else
                    {
                        pixel_size = math::Vec2i(texture_size.x, texture_size.y);
                        if (is_ui)
                        {
                            size = math::Vec3(size.x * texture_size.x, size.y * texture_size.y, 0);
                        }
                        else
                        {
                            size = math::Vec3((size.x * texture_size.x) / render_state.pixels_per_unit, (size.y * texture_size.y) / render_state.pixels_per_unit, 0);
                        }
                    }
                }
                
                if (render_state.bound_texture != (GLuint)texture_handle)
                {
                    glBindTexture(GL_TEXTURE_2D, (GLuint)texture_handle);
                }
                
                if (for_animation || (texture_offset.x >= 0.0f && texture_offset.y >= 0.0f))
                    shader = render_state.spritesheet_shader;
                else
                    shader = render_state.texture_rect_shader;
                
                render_state.bound_texture = (GLuint)texture_handle;
            }
            
            if (is_ui)
            {
                size.x *= render_state.scale_x;
                size.y *= render_state.scale_y;
            }
            
            if (render_state.current_extra_shader != -1)
            {
                shader = render_state.extra_shaders[render_state.current_extra_shader];
            }
            
            if (shader_handle != -1)
            {
                shader = render_state.extra_shaders[shader_handle];
            }
            
            use_shader(&shader);
            
            math::Mat4 model(1.0f);
            
            if (flipped)
            {
                size.x *= -1;
            }
            
            model = math::scale(model, size);
            model = math::translate(model, math::Vec3(size.x / -2.0f, size.y / -size.y, 0.0f));
            
            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;
            
            auto orientation = math::Quat();
            orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
            orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
            orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));
            
            model = to_matrix(orientation) * model;
            model = math::translate(model, math::Vec3(size.x / 2.0f, size.y / size.y, 0.0f));
            
            if (with_origin)
            {
                if (flipped)
                {
                    position.x -= ((pixel_size.x - origin.x) / render_state.pixels_per_unit) * scale.x;
                    position.y -= origin.y / render_state.pixels_per_unit;
                }
                else
                {
                    position.x -= origin.x / render_state.pixels_per_unit;
                    position.y -= origin.y / render_state.pixels_per_unit;
                }
            }
            else
            {
                position -= size / 2.0f;
            }
            
            if (flipped)
            {
                model = math::translate(model, math::Vec3(-size.x, 0.0f, 0.0f));
            }
            
            model = math::translate(model, position);
            
            if (!is_ui)
            {
                set_mat4_uniform(shader.program, "Projection", projection_matrix);
                set_mat4_uniform(shader.program, "View", view_matrix);
            }
            
            set_float_uniform(shader.program, "isUI", (r32)is_ui);
            set_mat4_uniform(shader.program, "M", model);
            set_vec4_uniform(shader.program, "color", color);
            
            if (texture_offset.x >= 0.0f && texture_offset.y >= 0.0f)
            {
                set_vec2_uniform(shader.program, "textureOffset", texture_offset);
                set_vec2_uniform(shader.program, "textureSize", texture_size);
                set_vec2_uniform(shader.program, "frameSize", math::Vec2((r32)frame_size.x, (r32)frame_size.y));
            }
            
            if (for_animation)
            {
                
                set_vec2_uniform(shader.program, "textureSize", texture_size);
                set_vec2_uniform(shader.program, "frameSize", math::Vec2((r32)frame_size.x, (r32)frame_size.y));
            }
            
            if (render_state.current_extra_shader != -1 || shader_handle != -1)
            {
                //shader_attribute* Attributes = ShaderHandle != -1 ? ShaderAttributes : RenderState.ShaderAttributes;
                i32 attribute_count = shader_handle != -1 ? shader_attribute_count : render_state.shader_attribute_count;
                
                for (i32 index = 0; index < attribute_count; index++)
                {
                    ShaderAttribute& attribute = shader_attributes[index];
                    switch (attribute.type)
                    {
                        case ATTRIBUTE_FLOAT:
                        {
                            set_float_uniform(shader.program, attribute.name, attribute.float_var);
                        }
                        break;
                        case ATTRIBUTE_FLOAT2:
                        {
                            set_vec2_uniform(shader.program, attribute.name, attribute.float2_var);
                        }
                        break;
                        case ATTRIBUTE_FLOAT3:
                        {
                            set_vec3_uniform(shader.program, attribute.name, attribute.float3_var);
                        }
                        break;
                        case ATTRIBUTE_FLOAT4:
                        {
                            set_vec4_uniform(shader.program, attribute.name, attribute.float4_var);
                        }
                        break;
                        case ATTRIBUTE_INTEGER:
                        {
                            set_int_uniform(shader.program, attribute.name, attribute.integer_var);
                        }
                        break;
                        case ATTRIBUTE_BOOLEAN:
                        {
                            set_int_uniform(shader.program, attribute.name, attribute.boolean_var);
                        }
                        break;
                        case ATTRIBUTE_MATRIX4:
                        {
                            set_mat4_uniform(shader.program, attribute.name, attribute.matrix4_var);
                        }
                        break;
                    }
                }
            }
            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
        break;
        case RENDER_OUTLINE:
        {
            math::Mat4 model(1.0f);
            if (is_ui)
            {
                size.x *= render_state.scale_x;
                size.y *= render_state.scale_y;
            }
            
            model = math::scale(model, size);
            
            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;
            
            auto orientation = math::Quat();
            orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
            orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
            orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));
            
            model = math::to_matrix(orientation) * model;
            model = math::translate(model, position);
            
            glBindVertexArray(render_state.wireframe_vao);
            
            auto shader = render_state.rect_shader;
            use_shader(&shader);
            
            if (!is_ui)
            {
                set_mat4_uniform(shader.program, "Projection", projection_matrix);
                set_mat4_uniform(shader.program, "View", view_matrix);
            }
            set_float_uniform(shader.program, "isUI", (r32)is_ui);
            set_mat4_uniform(shader.program, "M", model);
            set_vec4_uniform(shader.program, "color", color);
            
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        break;
    }
    glBindVertexArray(0);
}


static r32 get_text_width(const char *text, TrueTypeFont &font)
{
    r32 width = 0.0f;
    r32 placeholder_y = 0.0f;
    
    for(u32 i = 0; i < strlen(text); i++)
    {
        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                            text[i]- font.first_char, &width, &placeholder_y, &quad, 1);
        
        i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
        width += (r32)kerning * font.scale;
    }
    
    return width;
}

//rendering methods
static void render_text(RenderState &render_state, TrueTypeFont &font, const math::Vec4& color, const char* text, r32 x, r32 y, math::Mat4 view_matrix, math::Mat4 projection_matrix, r32 scale = 1.0f,
                        Alignment alignment = ALIGNMENT_LEFT, b32 align_center_y = true)
{
    glBindVertexArray(font.vao);
    auto shader = render_state.shaders[SHADER_STANDARD_FONT];
    use_shader(&shader);
    
    set_vec4_uniform(shader.program, "color", color);
    set_vec4_uniform(shader.program, "alphaColor", math::Rgba(1, 1, 1, 1));
    set_mat4_uniform(shader.program, "projectionMatrix", projection_matrix);
    
    glBindTexture(GL_TEXTURE_2D, font.texture);
    render_state.bound_texture = font.texture;
    
    auto temp_mem = begin_temporary_memory(&render_state.arena);
    
    CharacterData* coords = push_array(&render_state.arena, 6 * strlen(text), CharacterData);
    
    i32 n = 0;
    
    if(alignment == ALIGNMENT_CENTER)
    {
        x -= get_text_width(text, font) / 2.0f;
    }
    
    // first we have to reverse the initial y to support stb_truetype where y+ is down
    y = render_state.window_height - y;
    
    for(u32 i = 0; i < strlen(text); i++)
    {
        stbtt_aligned_quad quad;
        stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                            text[i]- font.first_char, &x, &y, &quad, 1);
        
        r32 x_min = quad.x0;
        r32 x_max = quad.x1;
        r32 y_min = render_state.window_height - quad.y0;//(quad.y0 + font.baseline);
        r32 y_max = render_state.window_height - quad.y1;//(quad.y1 + font.baseline);
        
        coords[n++] = { x_max, y_max, quad.s1, quad.t1 };
        coords[n++] = { x_max, y_min, quad.s1, quad.t0 };
        coords[n++] = { x_min, y_min, quad.s0, quad.t0 };
        coords[n++] = { x_min, y_max, quad.s0, quad.t1 };
        coords[n++] = { x_max, y_max, quad.s1, quad.t1 };
        coords[n++] = { x_min, y_min, quad.s0, quad.t0 };
        
        i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
        x += (r32)kerning * font.scale;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(6 * strlen(text) * sizeof(CharacterData)), coords, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, n);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    end_temporary_memory(temp_mem);
}

static void render_wireframe_cube(const RenderCommand& command, RenderState& render_state, math::Mat4 projection = math::Mat4(1.0f), math::Mat4 view = math::Mat4(1.0f))
{
    glBindVertexArray(render_state.wireframe_cube_vao);
    
    glLineWidth(command.wireframe_cube.line_width);
    auto shader = render_state.passthrough_shader;
    use_shader(&shader);
    
    math::Mat4 model = math::Mat4(1.0f);
    model = math::scale(model, command.scale);
    model = math::rotate(model, command.orientation);
    model = math::translate(model, command.position);
    
    set_mat4_uniform(shader.program, "Model", model);
    set_mat4_uniform(shader.program, "Projection", projection);
    set_mat4_uniform(shader.program, "View", view);
    set_vec4_uniform(shader.program, "Color", command.wireframe_cube.color);
    
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(GLuint)));
    
    glLineWidth(1.0f);
    glBindVertexArray(0);
}

static void render_line(const RenderCommand& command, RenderState& render_state, math::Mat4 projection, math::Mat4 view)
{
    render_line(render_state, command.line.color, command.line.point1, command.line.point2, projection, view, command.line.line_width, command.is_ui);
}

static void render_text(const RenderCommand& command, RenderState& render_state, math::Mat4 view_matrix, math::Mat4 projection_matrix)
{
    TrueTypeFont font = font = render_state.true_type_fonts[command.text.font_handle];
    
    render_text(render_state, font, command.text.color, command.text.text, command.text.position.x, command.text.position.y, view_matrix, projection_matrix, command.text.scale, command.text.alignment);
}

static void render_quad(const RenderCommand& command, RenderState& render_state, math::Mat4 projection, math::Mat4 view)
{
    if (command.is_ui)
    {
        auto handle = command.quad.texture_handle != -1 ? render_state.texture_array[command.quad.texture_handle].texture_handle : 0;
        render_quad(command.quad.outlined ? RENDER_OUTLINE : RENDER_FILL,
                    render_state,
                    command.quad.color,
                    command.position,
                    command.quad.flipped,
                    command.scale,
                    command.rotation,
                    command.with_origin,
                    command.origin,
                    command.shader_handle,
                    command.shader_attributes,
                    command.shader_attribute_count,
                    command.is_ui,
                    (i32)handle,
                    command.quad.for_animation,
                    command.quad.texture_size,
                    command.quad.frame_size,
                    command.quad.texture_offset);
    }
    else
    {
        auto handle = command.quad.texture_handle != -1 ? render_state.texture_array[command.quad.texture_handle].texture_handle : 0;
        render_quad(command.quad.outlined ? RENDER_OUTLINE : RENDER_FILL,
                    render_state,
                    command.quad.color,
                    command.position,
                    command.quad.flipped,
                    command.scale,
                    command.rotation,
                    command.with_origin,
                    command.origin,
                    command.shader_handle,
                    command.shader_attributes,
                    command.shader_attribute_count,
                    command.is_ui,
                    (i32)handle,
                    command.quad.for_animation,
                    command.quad.texture_size,
                    command.quad.frame_size,
                    command.quad.texture_offset,
                    projection,
                    view);
    }
}

static void render_model(const RenderCommand& command, RenderState& render_state, math::Mat4 projection, math::Mat4 view)
{
    Buffer buffer = render_state.buffers[command.model.buffer_handle];
    glBindVertexArray(buffer.vao);
    
    for (i32 mesh_index = 0; mesh_index < command.model.mesh_count; mesh_index++)
    {
        MeshData mesh_data = command.model.meshes[mesh_index];
        Material material = command.model.materials[mesh_data.material_index];
        
        if (material.diffuse_texture.has_data)
        {
            Texture texture = render_state.texture_array[material.diffuse_texture.texture_handle];
            if (render_state.bound_texture != texture.texture_handle)
            {
                glBindTexture(GL_TEXTURE_2D, texture.texture_handle);
                render_state.bound_texture = texture.texture_handle;
            }
        }
        
        Shader shader = {};
        
        if (command.model.type == MODEL_SKINNED)
        {
            shader = render_state.simple_model_shader;
            use_shader(&shader);
            
            for (i32 index = 0; index < command.model.bone_count; index++)
            {
                char s_buffer[20];
                sprintf(s_buffer, "bones[%d]", index);
                set_mat4_uniform(shader.program, s_buffer, command.model.bone_transforms[index]);
            }
        }
        else
        {
            // @Incomplete: We need a shader that isn't using the bone data
        }
        
        math::Mat4 model(1.0f);
        model = math::scale(model, command.scale);
        model = math::rotate(model, command.orientation);
        model = math::translate(model, command.position);
        
        math::Mat4 normal_matrix = math::transpose(math::inverse(view * model));
        
        set_mat4_uniform(shader.program, "normalMatrix", normal_matrix);
        set_mat4_uniform(shader.program, "projection", projection);
        set_mat4_uniform(shader.program, "view", view);
        set_mat4_uniform(shader.program, "model", model);
        set_vec4_uniform(shader.program, "color", math::Rgba(1.0f, 1.0f, 1.0f, 1.0f));
        set_int_uniform(shader.program, "hasUVs", material.diffuse_texture.has_data);
        
        glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_INT, (void*)0);
        glBindVertexArray(0);
    }
}

static void render_buffer(const RenderCommand& command, RenderState& render_state, math::Mat4 projection, math::Mat4 view)
{
    Buffer buffer = render_state.buffers[command.buffer.buffer_handle];
    
    glBindVertexArray(buffer.vao);
    u32 texture_handle = command.buffer.texture_handle != -1 ? render_state.texture_array[command.buffer.texture_handle].texture_handle : 0;
    
    if (texture_handle != 0 && render_state.bound_texture != texture_handle)
    {
        glBindTexture(GL_TEXTURE_2D, texture_handle);
        render_state.bound_texture = texture_handle;
    }
    
    math::Vec3 position = command.position;
    math::Vec3 size = command.scale;
    
    if (command.is_ui)
    {
        position.x *= render_state.scale_x;
        position.x -= 1;
        position.y *= render_state.scale_y;
        position.y -= 1;
        
        size.x *= render_state.scale_x;
        size.y *= render_state.scale_y;
    }
    
    auto shader = render_state.tile_shader;
    use_shader(&shader);
    
    math::Mat4 model(1.0f);
    model = math::scale(model, size);
    model = math::translate(model, position);
    
    //Model = math::YRotate(Command.Rotation.y) * Model;
    //Model = math::XRotate(Command.Rotation.x) * Model;
    //Model = math::ZRotate(Command.Rotation.z) * Model;
    
    set_float_uniform(shader.program, "isUI", (r32)command.is_ui);
    set_mat4_uniform(shader.program, "Projection", projection);
    set_mat4_uniform(shader.program, "View", view);
    set_mat4_uniform(shader.program, "Model", model);
    set_vec4_uniform(shader.program, "Color", command.color);
    
    glDrawArrays(
        GL_TRIANGLES, 0, buffer.vertex_buffer_size / 3);
    glBindVertexArray(0);
}

void stbtt_load_font(RenderState &render_state, char *path, i32 size)
{
    TrueTypeFont &font = render_state.true_type_fonts[render_state.font_count++];
    font.atlas_width = 1024;
    font.atlas_height = 1024;
    font.oversample_x = 1;
    font.oversample_y = 1;
    font.first_char = ' ';
    font.char_count = '~' - ' ';
    font.size = size;
    
    unsigned char *ttf_buffer = push_array(&render_state.arena, (1<<20), unsigned char);
    
    auto temp_memory = begin_temporary_memory(&render_state.arena);
    
    unsigned char *temp_bitmap = push_array(&render_state.arena, 1024 * 1024, unsigned char);
    
    fread(ttf_buffer, 1, 1<<20, fopen(path, "rb"));
    
    stbtt_InitFont(&font.info, ttf_buffer, 0); 
    font.scale = stbtt_ScaleForPixelHeight(&font.info, 15);
    stbtt_GetFontVMetrics(&font.info, &font.ascent, 0, 0);
    font.baseline = (i32)(font.ascent * font.scale);
    
    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, temp_bitmap, font.atlas_width, font.atlas_height, 0, 1, 0))
        printf("Failed to initialize font");
    
    stbtt_PackSetOversampling(&context, font.oversample_x, font.oversample_y);
    if (!stbtt_PackFontRange(&context, ttf_buffer, (i32)font.scale, (r32)font.size, font.first_char, font.char_count, font.char_data))
        printf("Failed to pack font");
    
    stbtt_PackEnd(&context);
    
    glGenTextures(1, &font.texture);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, (GLsizei)font.atlas_width, (GLsizei)font.atlas_height, 0, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
    
    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glGenVertexArrays(1, &font.vao);
    glBindVertexArray(font.vao);
    
    glGenBuffers(1, &font.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, font.vbo);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    
    end_temporary_memory(temp_memory);
}

static void load_font(RenderState& render_state, char* path, i32 size)
{
    //initialize_free_type_font(path, size, render_state.ft_library, &render_state.fonts[render_state.font_count++]);
    stbtt_load_font(render_state, path, size);
}


static void register_buffers(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena)
{
    for (i32 index = render_state.buffer_count; index < renderer.buffer_count; index++)
    {
        BufferData data = renderer.buffers[index];
        
        if (data.index_buffer_count == 0)
        {
            register_vertex_buffer(render_state, data.vertex_buffer, (i32)data.vertex_buffer_size, data.shader_type, perm_arena, data.existing_handle);
        }
        else
        {
            register_buffers(render_state, data.vertex_buffer, data.vertex_buffer_size, data.index_buffer, data.index_buffer_count, data.index_buffer_size, data.has_normals, data.has_uvs, data.skinned, data.existing_handle);
            
        }
    }
    
    for (i32 index = 0; index < renderer.updated_buffer_handle_count; index++)
    {
        BufferData data = renderer.buffers[renderer.updated_buffer_handles[index]];
        if (data.index_buffer_count == 0)
        {
            register_vertex_buffer(render_state, data.vertex_buffer, (i32)data.vertex_buffer_size, data.shader_type, perm_arena, data.existing_handle);
        }
        else
        {
            register_buffers(render_state, data.vertex_buffer, data.vertex_buffer_size, data.index_buffer, data.index_buffer_count, data.index_buffer_size, data.has_normals, data.has_uvs, data.skinned, data.existing_handle);
            
        }
    }
    renderer.updated_buffer_handle_count = 0;
}

static void render_commands(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena)
{
    for (i32 index = render_state.font_count; index < renderer.font_count; index++)
    {
        FontData data = renderer.fonts[index];
        load_font(render_state, data.path, data.size);
    }
    
    auto& camera = renderer.cameras[renderer.current_camera_handle];
    auto& v = camera.view_matrix;
    
    for (i32 index = 0; index < renderer.light_command_count; index++)
    {
        const RenderCommand& command = *((RenderCommand*)renderer.light_commands.current_block->base + index);
        
        switch (command.type)
        {
            case RENDER_COMMAND_SPOTLIGHT:
            {
                Spotlight& spotlight = render_state.spotlight_data.spotlights[render_state.spotlight_data.num_lights++];
                
                auto pos = v * math::Vec4(command.position, 1.0f);
                
                spotlight.position[0] = pos.x;
                spotlight.position[1] = pos.y;
                spotlight.position[2] = pos.z;
                spotlight.position[3] = 1;
                
                spotlight.direction[0] = command.spotlight.direction.x;
                spotlight.direction[1] = command.spotlight.direction.y;
                spotlight.direction[2] = command.spotlight.direction.z;
                spotlight.direction[3] = 0;
                
                spotlight.cut_off = command.spotlight.cut_off;
                spotlight.outer_cut_off = command.spotlight.outer_cut_off;
                
                spotlight.ambient[0] = command.spotlight.ambient.x;
                spotlight.ambient[1] = command.spotlight.ambient.y;
                spotlight.ambient[2] = command.spotlight.ambient.z;
                spotlight.ambient[3] = 1.0f;
                
                spotlight.diffuse[0] = command.spotlight.diffuse.x;
                spotlight.diffuse[1] = command.spotlight.diffuse.y;
                spotlight.diffuse[2] = command.spotlight.diffuse.z;
                spotlight.diffuse[3] = 1.0f;
                spotlight.specular[0] = command.spotlight.specular.x;
                spotlight.specular[1] = command.spotlight.specular.y;
                spotlight.specular[2] = command.spotlight.specular.z;
                spotlight.specular[3] = 1.0f;
                
                spotlight.constant = command.spotlight.constant;
                spotlight.linear = command.spotlight.linear;
                spotlight.quadratic = command.spotlight.quadratic;
            }
            break;
            case RENDER_COMMAND_DIRECTIONAL_LIGHT:
            {
                DirectionalLight& directional_light = render_state.directional_light_data.directional_lights[render_state.directional_light_data.num_lights++];
                
                directional_light.direction[0] = command.directional_light.direction.x;
                directional_light.direction[1] = command.directional_light.direction.y;
                directional_light.direction[2] = command.directional_light.direction.z;
                directional_light.direction[3] = 0;
                
                directional_light.ambient[0] = command.directional_light.ambient.x;
                directional_light.ambient[1] = command.directional_light.ambient.y;
                directional_light.ambient[2] = command.directional_light.ambient.z;
                directional_light.ambient[3] = 1;
                directional_light.diffuse[0] = command.directional_light.diffuse.x;
                directional_light.diffuse[1] = command.directional_light.diffuse.y;
                directional_light.diffuse[2] = command.directional_light.diffuse.z;
                directional_light.diffuse[3] = 1;
                directional_light.specular[0] = command.directional_light.specular.x;
                directional_light.specular[1] = command.directional_light.specular.y;
                directional_light.specular[2] = command.directional_light.specular.z;
                directional_light.specular[3] = 1;
            }
            break;
            case RENDER_COMMAND_POINT_LIGHT:
            {
                PointLight& point_light = render_state.point_light_data.point_lights[render_state.point_light_data.num_lights++];
                
                auto pos = v * math::Vec4(command.position, 1.0f);
                
                point_light.position[0] = pos.x;
                point_light.position[1] = pos.y;
                point_light.position[2] = pos.z;
                point_light.position[3] = 1.0f;
                
                point_light.ambient[0] = command.point_light.ambient.x;
                point_light.ambient[1] = command.point_light.ambient.y;
                point_light.ambient[2] = command.point_light.ambient.z;
                
                point_light.diffuse[0] = command.point_light.diffuse.x;
                point_light.diffuse[1] = command.point_light.diffuse.y;
                point_light.diffuse[2] = command.point_light.diffuse.z;
                
                point_light.specular[0] = command.point_light.specular.x;
                point_light.specular[1] = command.point_light.specular.y;
                point_light.specular[2] = command.point_light.specular.z;
                
                point_light.constant = command.point_light.constant;
                point_light.linear = command.point_light.linear;
                point_light.quadratic = command.point_light.quadratic;
            }
            break;
            default:
            break;
        }
    }
    
    //update_lighting_data(render_state);
    renderer.light_command_count = 0;
    render_state.spotlight_data.num_lights = 0;
    render_state.directional_light_data.num_lights = 0;
    render_state.point_light_data.num_lights = 0;
    clear(&renderer.light_commands);
    
    glEnable(GL_DEPTH_TEST);
    for (i32 index = 0; index < renderer.command_count; index++)
    {
        const RenderCommand& command = *((RenderCommand*)renderer.commands.current_block->base + index);
        
        switch (command.type)
        {
            case RENDER_COMMAND_LINE:
            {
                render_line(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_TEXT:
            {
                render_text(command, render_state, camera.view_matrix, camera.projection_matrix);
            }
            break;
            case RENDER_COMMAND_QUAD:
            {
                render_quad(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_SPRITE:
            {
                //RenderSprite(Command, render_state, Renderer, Camera.projection_matrix, Camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_MODEL:
            {
                render_model(command, render_state, camera.projection_matrix, camera.view_matrix);
                
            }
            break;
            case RENDER_COMMAND_BUFFER:
            {
                render_buffer(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_WIREFRAME_CUBE:
            {
                render_wireframe_cube(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_DEPTH_TEST:
            {
                if (command.depth_test.on)
                {
                    glEnable(GL_DEPTH_TEST);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }
            }
            break;
            case RENDER_COMMAND_SHADER_START:
            {
                render_state.current_extra_shader = command.shader.handle;
                render_state.shader_attributes = command.shader.attributes;
                render_state.shader_attribute_count = command.shader.attribute_count;
            }
            break;
            case RENDER_COMMAND_SHADER_END:
            {
                render_state.current_extra_shader = -1;
                render_state.shader_attributes = 0;
                render_state.shader_attribute_count = 0;
            }
            break;
            default:
            break;
        }
    }
    
    renderer.command_count = 0;
    clear(&renderer.commands);
    
    glDisable(GL_DEPTH_TEST);
    
    for (i32 index = 0; index < renderer.ui_command_count; index++)
    {
        const RenderCommand& command = *((RenderCommand*)renderer.ui_commands.current_block->base + index);
        
        switch (command.type)
        {
            case RENDER_COMMAND_LINE:
            {
                render_line(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_TEXT:
            {
                render_text(command, render_state, camera.view_matrix, camera.projection_matrix);
            }
            break;
            case RENDER_COMMAND_QUAD:
            {
                render_quad(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_SPRITE:
            {
                //RenderSprite(Command, render_state, Renderer, Camera.projection_matrix, Camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_MODEL:
            {
                render_model(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_BUFFER:
            {
                render_buffer(command, render_state, camera.projection_matrix, camera.view_matrix);
            }
            break;
            case RENDER_COMMAND_DEPTH_TEST:
            {
                // @Incomplete: Do we need depth test commands for UI stuff?
            }
            break;
            case RENDER_COMMAND_SHADER_START:
            {
                render_state.current_extra_shader = command.shader.handle;
                render_state.shader_attributes = command.shader.attributes;
                render_state.shader_attribute_count = command.shader.attribute_count;
            }
            break;
            case RENDER_COMMAND_SHADER_END:
            {
                render_state.current_extra_shader = -1;
                render_state.shader_attributes = 0;
                render_state.shader_attribute_count = 0;
            }
            break;
            default:
            break;
        }
    }
    
    renderer.ui_command_count = 0;
    clear(&renderer.commands);
    
}

static void render(RenderState& render_state, Renderer& renderer, MemoryArena* perm_arena, r64 delta_time)
{
    if (renderer.window_mode != render_state.window_mode)
    {
        glfwDestroyWindow(render_state.window);
        create_open_gl_window(render_state, renderer.window_mode, render_state.window_title, render_state.window_width, render_state.window_height);
        glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSetFramebufferSizeCallback(render_state.window, frame_buffer_size_callback);
        
        glfwMakeContextCurrent(render_state.window);
        
        glfwSwapInterval(0);
        
        glfwGetFramebufferSize(render_state.window, &render_state.window_width, &render_state.window_height);
        glViewport(0, 0, render_state.window_width, render_state.window_height);
        glDisable(GL_DITHER);
        glLineWidth(2.0f);
        glEnable(GL_LINE_SMOOTH);
        
        glDisable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        
        glDepthFunc(GL_LESS);
        
        glfwSetWindowUserPointer(render_state.window, &render_state);
        glfwSetKeyCallback(render_state.window, key_callback);
        glfwSetCharCallback(render_state.window, character_callback);
        glfwSetCursorPosCallback(render_state.window, cursor_position_callback);
        glfwSetMouseButtonCallback(render_state.window, mouse_button_callback);
        glfwSetScrollCallback(render_state.window, scroll_callback);
        
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        
        memcpy(render_state.viewport, viewport, sizeof(GLint) * 4);
    }
    
    load_extra_shaders(render_state, renderer);
    
    load_textures(render_state, renderer);
    
    render_state.current_extra_shader = -1;
    render_state.shader_attribute_count = 0;
    
    auto& camera = renderer.cameras[renderer.current_camera_handle];
    camera.viewport_width = render_state.scale_from_width;
    camera.viewport_height = render_state.scale_from_height;
    
    render_state.scale_x = 2.0f / render_state.screen_width;
    render_state.scale_y = 2.0f / render_state.screen_height;
    render_state.pixels_per_unit = renderer.pixels_per_unit;
    renderer.scale_x = render_state.scale_x;
    renderer.scale_y = render_state.scale_y;
    
    register_buffers(render_state, renderer, perm_arena);
    
    if ((renderer.frame_lock != 0 && render_state.frame_delta <= 0.0) || renderer.frame_lock == 0)
    {
        renderer.fps = 1.0 / render_state.total_delta;
        renderer.current_frame++;
        renderer.fps_sum += renderer.fps;
        
        if (renderer.current_frame == 60)
        {
            renderer.current_frame = 0;
            renderer.average_fps = renderer.fps_sum / 60.0f;
            renderer.fps_sum = 0.0;
        }
        
        glBindFramebuffer(GL_FRAMEBUFFER, render_state.frame_buffer);
        
        glBindTexture(GL_TEXTURE_2D, render_state.texture_color_buffer);
        
        glEnable(GL_DEPTH_TEST);
        
        glDepthFunc(GL_LESS);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glClearColor(renderer.clear_color.r, renderer.clear_color.g, renderer.clear_color.b, renderer.clear_color.a);
        
        render_commands(render_state, renderer, perm_arena);
        render_state.bound_texture = 0;
        
        // We have to reset the bound texture to nothing, since we're about to bind other textures
        // Second pass
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glClearColor(renderer.clear_color.r, renderer.clear_color.g, renderer.clear_color.b, renderer.clear_color.a);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        glBindVertexArray(render_state.frame_buffer_vao);
        
        use_shader(&render_state.frame_buffer_shader);
        
        set_float_uniform(render_state.frame_buffer_shader.program, "contrast", render_state.contrast);
        set_float_uniform(render_state.frame_buffer_shader.program, "brightness", render_state.brightness);
        set_int_uniform(render_state.frame_buffer_shader.program, "ignoreLight", true); // @Incomplete: Lighting
        set_mat4_uniform(render_state.frame_buffer_shader.program, "P", camera.projection_matrix);
        set_mat4_uniform(render_state.frame_buffer_shader.program, "V", camera.view_matrix);
        set_vec2_uniform(render_state.frame_buffer_shader.program, "screenSize", math::Vec2((r32)render_state.window_width, (r32)render_state.window_height));
        
        glUniform1i((GLint)render_state.frame_buffer_tex0_loc, 0);
        
        glUniform1i((GLint)render_state.frame_buffer_tex1_loc, 1);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, render_state.texture_color_buffer);
        glActiveTexture(GL_TEXTURE1);
        /*glBindTexture(GL_TEXTURE_2D, RenderState.LightingTextureColorBuffer);
        RenderState.BoundTexture = RenderState.LightingTextureColorBuffer;
        */
        //Enable this if we don't do gamma correction in frame_buffer shader
        //glEnable(GL_FRAMEBUFFER_SRGB);
        
        glDrawElements(GL_TRIANGLES, sizeof(render_state.quad_indices), GL_UNSIGNED_INT, (void*)0);
        
        glActiveTexture(GL_TEXTURE0);
        
        glfwSwapBuffers(render_state.window);
        
        if (renderer.frame_lock != 0)
        {
            render_state.total_delta = 0.0;
            render_state.frame_delta += 1.0 / renderer.frame_lock;
        }
        else
        {
            render_state.total_delta = delta_time;
        }
    }
    else
    {
        
        clear(&renderer.light_commands);
        renderer.light_command_count = 0;
        clear(&renderer.commands);
        renderer.command_count = 0;
        clear(&renderer.ui_commands);
        renderer.ui_command_count = 0;
    }
    
    render_state.frame_delta -= delta_time;
    render_state.total_delta += delta_time;
}
