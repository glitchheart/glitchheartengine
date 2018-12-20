
//#define GL_DEBUG

// #ifdef GL_DEBUG
// GLuint debug_attrib(u32 program, char* str, char* file, int line)
// {
//     debug("File: %s, Line: %d\n", file, line);
//     return (GLuint)glGetAttribLocation(program, str);
// }

// void debug_attrib_array(u32 location, char* file, int line)
// {
//     debug("File: %s, Line: %d\n", file, line);
//     glEnableVertexAttribArray(location);
// }

// // Macro redefinition, but used for debug
// #define glEnableVertexAttribArray(loc) debug_attrib_array(loc, __FILE__, __LINE__)
// #define glGetAttribLocation(shader, str) debug_attrib(shader, str, __FILE__, __LINE__)
// #endif

static void bind_vertex_array(GLuint vao, RenderState &render_state)
{
    if (vao != render_state.current_state.vao)
    {
        render_state.current_state.vao = vao;
        glBindVertexArray(vao);
    }
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %d - %s\n", error, description);
}

static void show_mouse_cursor(RenderState &render_state, b32 show)
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

#define error_gl() _error_gl(__LINE__, __FILE__)
void _error_gl(i32 line, const char *file)
{
    GLenum err = glGetError();
    switch (err)
    {
    case GL_INVALID_ENUM:
    {
        log_error("OpenGL Error: GL_INVALID_ENUM on line %d in file %s", line, file);
    }
    break;
    case GL_INVALID_VALUE:
    {
        log_error("OpenGL Error: GL_INVALID_VALUE on line %d in file %s", line, file);
    }
    break;
    case GL_INVALID_OPERATION:
    {
        log_error("OpenGL Error: GL_INVALID_OPERATION on line %d in file %s", line, file);
    }
    break;
    case GL_STACK_OVERFLOW:
    {
        log_error("OpenGL Error: GL_STACK_OVERFLOW on line %d in file %s", line, file);
    }
    break;
    case GL_OUT_OF_MEMORY:
    {
        log_error("OpenGL Error: GL_OUT_OF_MEMORY on line %d in file %s", line, file);
    }
    break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
    {
        log_error("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION on line %d in file %s", line, file);
    }
    break;
    case GL_CONTEXT_LOST:
    {
        log_error("OpenGL Error: GL_CONTEXT_LOST on line %d in file %sx", line, file);
    }
    break;
    case GL_TABLE_TOO_LARGE:
    {
        log_error("OpenGL Error: GL_TABLE_TOO_LARGE on line %d in file %s", line, file);
    }
    break;
    }
}

void message_callback(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar *message,
                      const void *user_param)
{
    (void)user_param; // Silence unused warning

    char *src_str;

    if (type == GL_DEBUG_TYPE_ERROR)
    {
        switch (source)
        {
        case GL_DEBUG_SOURCE_API:
            src_str = "API";
            break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            src_str = "WINDOW SYSTEM";
            break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            src_str = "SHADER COMPILER";
            break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
            src_str = "THIRD PARTY";
            break;

        case GL_DEBUG_SOURCE_APPLICATION:
            src_str = "APPLICATION";
            break;

        case GL_DEBUG_SOURCE_OTHER:
            src_str = "UNKNOWN";
            break;

        default:
            src_str = "UNKNOWN";
            break;
        }

        log_error("OpenGL error: %s type = 0x%x, severity = 0x%x, message = %s, source = %s, id = %ud, length %ud=",
                  (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
                  type, severity, message, src_str, id, length);
    }
}

static void vertex_attrib_pointer(GLuint loc, i32 count, GLenum type, u32 stride, void *offset)
{
    glEnableVertexAttribArray(loc);
    glVertexAttribPointer(loc, count, type, GL_FALSE, (GLsizei)stride, offset);
}

inline static r64 get_time()
{
    return glfwGetTime();
}

inline static void poll_events()
{
    glfwPollEvents();
}

[[noreturn]] static void close_window(RenderState &render_state)
{
    glfwDestroyWindow(render_state.window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static b32 should_close_window(RenderState &render_state)
{
    return glfwWindowShouldClose(render_state.window);
}

static GLint compile_shader(MemoryArena *arena, const char *shader_name, GLuint shader)
{
    glCompileShader(shader);
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
        TemporaryMemory temp_mem = begin_temporary_memory(arena);
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        GLchar *error_log = nullptr;
        GLchar error_log_static[512];

        if (max_length > 512)
        {
            error_log = push_size(arena, max_length, GLchar);
        }
        else
        {
            error_log = error_log_static;
        }

        glGetShaderInfoLog(shader, max_length, &max_length, error_log);

        log_error("Shader compilation error - %s\n", shader_name);
        log_error("%s", error_log);

        glDeleteShader(shader);

        end_temporary_memory(temp_mem);
    }

    return is_compiled;
}

static GLint link_program(MemoryArena *arena, const char *program_name, GLuint program)
{
    GLint is_linked = 0;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    if (!is_linked)
    {
        TemporaryMemory temp_mem = begin_temporary_memory(arena);
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        GLchar *error_log = nullptr;
        GLchar error_log_static[512];

        if (max_length > 512)
        {
            error_log = push_size(arena, max_length, GLchar);
        }
        else
        {
            error_log = error_log_static;
        }

        glGetProgramInfoLog(program, max_length, &max_length, error_log);

        log_error("Program linking error - %s\n", program_name);
        log_error("%s", error_log);
    }
    return is_linked;
}

static GLint shader_compilation_error_checking(MemoryArena *arena, const char *shader_name, GLuint shader)
{
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
        TemporaryMemory temp_mem = begin_temporary_memory(arena);
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        // The max_length includes the NULL character
        GLchar *error_log = push_size(arena, max_length, GLchar);

        glGetShaderInfoLog(shader, max_length, &max_length, error_log);

        log_error("SHADER Compilation error - %s", shader_name);
        log_error("%s", error_log);

        glDeleteShader(shader); // Don't leak the shader.
        end_temporary_memory(temp_mem);
    }
    return is_compiled;
}

static GLint shader_link_error_checking(MemoryArena *arena, const char *program_name, GLuint program)
{
    GLint is_linked = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (!is_linked)
    {
        TemporaryMemory temp_mem = begin_temporary_memory(arena);
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        // The maxLength includes the NULL character
        GLchar *error_log = push_size(arena, max_length, GLchar);

        glGetProgramInfoLog(program, max_length, &max_length, error_log);

        log_error("SHADER Linking error - %s", program_name);
        log_error("%s", error_log);

        glDeleteProgram(program); // Don't leak the program.
        end_temporary_memory(temp_mem);
    }
    return is_linked;
}

static void set_all_uniform_locations(rendering::Shader &shader, ShaderGL &gl_shader)
{
    gl_shader.location_count = 0;

    for (i32 i = 0; i < shader.uniform_count; i++)
    {
        rendering::Uniform &uniform = shader.uniforms[i];

        if (uniform.is_array)
        {
            if (uniform.type == rendering::ValueType::STRUCTURE)
            {
                rendering::Structure &structure = shader.structures[uniform.structure_index];

                for (i32 i = 0; i < uniform.array_size; i++)
                {
                    for (i32 j = 0; j < structure.uniform_count; j++)
                    {
                        rendering::Uniform struct_uni = structure.uniforms[j];
                        char char_buf[256];
                        sprintf(char_buf, "%s[%d].%s", uniform.name, i, struct_uni.name);
                        GLint location = glGetUniformLocation(gl_shader.program, char_buf);
                        gl_shader.uniform_locations[gl_shader.location_count++] = location;
                    }
                }
            }
            else
            {
                for (i32 i = 0; i < uniform.array_size; i++)
                {
                    char char_buf[256];
                    sprintf(char_buf, "%s[%d]", uniform.name, i);

                    GLint location = glGetUniformLocation(gl_shader.program, char_buf);
                    gl_shader.uniform_locations[gl_shader.location_count++] = location;
                }
            }
        }
        else
        {
            GLint location = glGetUniformLocation(gl_shader.program, uniform.name);
            gl_shader.uniform_locations[gl_shader.location_count++] = location;
        }
    }
}

static GLuint load_shader(Renderer &renderer, rendering::Shader &shader, ShaderGL &gl_shader)
{
    char *vert_shader = shader.vert_shader;
    gl_shader.vert_program = glCreateShader(GL_VERTEX_SHADER);

    // @Incomplete: Think about common preamble stuff like #version 330 core and stuff
    glShaderSource(gl_shader.vert_program, 1, (GLchar **)&vert_shader, nullptr);

    if (!compile_shader(&renderer.shader_arena, shader.path, gl_shader.vert_program))
    {
        log_error("Failed compilation of vertex shader: %s", shader.path);
        gl_shader.vert_program = 0;
        return GL_FALSE;
    }

    char *frag_shader = shader.frag_shader;
    gl_shader.frag_program = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(gl_shader.frag_program, 1, (GLchar **)&frag_shader, nullptr);

    if (!compile_shader(&renderer.shader_arena, shader.path, gl_shader.frag_program))
    {
        log_error("Failed compilation of fragment shader: %s", shader.path);
        gl_shader.frag_program = 0;
        return GL_FALSE;
    }

    gl_shader.program = glCreateProgram();

    glAttachShader(gl_shader.program, gl_shader.vert_program);
    glAttachShader(gl_shader.program, gl_shader.frag_program);

    if (!link_program(&renderer.shader_arena, shader.path, gl_shader.program))
    {
        log_error("Failed linking of program: %s", shader.path);
        gl_shader.program = 0;
        gl_shader.frag_program = 0;
        gl_shader.vert_program = 0;
        return GL_FALSE;
    }

    set_all_uniform_locations(shader, gl_shader);

    return GL_TRUE;
}

static void delete_shader_program(ShaderGL &shader)
{
    glDeleteProgram(shader.program);
    glDeleteShader(shader.vert_program);
    glDeleteShader(shader.frag_program);
    shader.program = 0;
    shader.vert_program = 0;
    shader.frag_program = 0;
}

static void reload_shaders(RenderState &render_state, Renderer &renderer)
{
    if (renderer.render.shaders_to_reload_count > 0)
    {
        i32 shaders_to_reload[8];
        i32 count = renderer.render.shaders_to_reload_count;
        memcpy(shaders_to_reload, renderer.render.shaders_to_reload, sizeof(i32) * count);
        renderer.render.shaders_to_reload_count = 0;

        for (i32 i = 0; i < count; i++)
        {
            i32 index = shaders_to_reload[i];
            rendering::Shader &shader = renderer.render.shaders[index];
            ShaderGL &gl_shader = render_state.gl_shaders[index];

            delete_shader_program(gl_shader);
            clear(&shader.arena);
            rendering::load_shader(renderer, shader);
            rendering::update_materials_with_shader(renderer, shader);
            load_shader(renderer, shader, gl_shader);
            printf("Reloaded shader: %s\n", shader.path);
        }
    }
}

static GLuint load_shader(const char *file_path, Shader *shd, MemoryArena *arena)
{
    TemporaryMemory temp_mem = begin_temporary_memory(arena);
    shd->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    char *vertex_string = concat(file_path, ".vert", arena);
    GLchar *vertex_text = load_shader_from_file(vertex_string, arena);

    if (vertex_text)
    {
        glShaderSource(shd->vertex_shader, 1, (GLchar **)&vertex_text, nullptr);
        glCompileShader(shd->vertex_shader);

        if (!shader_compilation_error_checking(arena, file_path, shd->vertex_shader))
        {
            log_error("failed compilation of vertex shader: %s", file_path);
            end_temporary_memory(temp_mem);
            shd->program = 0;
            return GL_FALSE;
        }

        shd->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        char *fragment_string = concat(file_path, ".frag", arena);
        GLchar *fragment_text = load_shader_from_file(fragment_string, arena);

        glShaderSource(shd->fragment_shader, 1, (GLchar **)&fragment_text, nullptr);
        glCompileShader(shd->fragment_shader);

        if (!shader_compilation_error_checking(arena, file_path, shd->fragment_shader))
        {
            log_error("failed compilation of fragment shader: %s", file_path);
            end_temporary_memory(temp_mem);
            shd->program = 0;
            return GL_FALSE;
        }

        char *geometry_string = concat(file_path, ".geom", arena);
        GLchar *geometry_text = load_shader_from_file(geometry_string, arena, IGNORE_ERROR);

        if (geometry_text)
        {
            shd->geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(shd->geometry_shader, 1, &geometry_text, nullptr);
            glCompileShader(shd->geometry_shader);

            if (!shader_compilation_error_checking(arena, file_path, shd->geometry_shader))
            {
                log_error("failed compilation of geometry shader: %s", file_path);
                end_temporary_memory(temp_mem);
                shd->program = 0;
                return GL_FALSE;
            }
        }

        shd->program = glCreateProgram();

        if (geometry_text)
        {
            glAttachShader(shd->program, shd->geometry_shader);
        }

        glAttachShader(shd->program, shd->vertex_shader);
        glAttachShader(shd->program, shd->fragment_shader);
        glLinkProgram(shd->program);

        end_temporary_memory(temp_mem);
        return GL_TRUE;
    }
    end_temporary_memory(temp_mem);
    return GL_FALSE;
}

static GLuint load_vertex_shader(const char *file_path, Shader *shd, MemoryArena *arena)
{
    TemporaryMemory temp_mem = begin_temporary_memory(arena);
    shd->program = glCreateProgram();

    shd->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    char *vertex_string = concat(file_path, ".vert", arena);
    GLchar *vertex_text = load_shader_from_file(vertex_string, arena);
    glShaderSource(shd->vertex_shader, 1, &vertex_text, nullptr);
    glCompileShader(shd->vertex_shader);

    if (!shader_compilation_error_checking(arena, file_path, shd->vertex_shader))
    {
        log_error("failed compilation of vertex shader: %s", file_path);
        end_temporary_memory(temp_mem);
        shd->program = 0;
        return GL_FALSE;
    }

    glAttachShader(shd->program, shd->geometry_shader);

    glAttachShader(shd->program, shd->vertex_shader);

    glAttachShader(shd->program, shd->fragment_shader);

    glLinkProgram(shd->program);

    end_temporary_memory(temp_mem);
    return GL_TRUE;
}

static GLuint load_fragment_shader(const char *file_path, Shader *shd, MemoryArena *arena)
{
    shd->program = glCreateProgram();
    shd->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

    TemporaryMemory temp_mem = begin_temporary_memory(arena);
    char *fragment_string = concat(file_path, ".frag", arena);

    GLchar *fragment_text = load_shader_from_file(fragment_string, arena);
    glShaderSource(shd->fragment_shader, 1, &fragment_text, nullptr);
    end_temporary_memory(temp_mem);

    glCompileShader(shd->fragment_shader);

    if (!shader_compilation_error_checking(arena, file_path, shd->fragment_shader))
    {
        log_error("failed compilation of fragment shader: %s", file_path);
        end_temporary_memory(temp_mem);
        shd->program = 0;
        return GL_FALSE;
    }

    glAttachShader(shd->program, shd->geometry_shader);
    glAttachShader(shd->program, shd->vertex_shader);
    glAttachShader(shd->program, shd->fragment_shader);
    glLinkProgram(shd->program);

    return GL_TRUE;
}

static GLuint load_geometry_shader(const char *file_path, Shader *shd, MemoryArena *arena)
{
    shd->program = glCreateProgram();
    shd->geometry_shader = glCreateShader(GL_GEOMETRY_SHADER);

    TemporaryMemory temp_mem = begin_temporary_memory(arena);
    char *geometry_string = concat(file_path, ".geom", arena);

    GLchar *geometry_text = load_shader_from_file(geometry_string, arena);
    glShaderSource(shd->geometry_shader, 1, &geometry_text, nullptr);
    end_temporary_memory(temp_mem);

    glCompileShader(shd->geometry_shader);

    if (!shader_compilation_error_checking(arena, file_path, shd->geometry_shader))
    {
        log_error("failed compilation of geometry shader: %s", file_path);
        end_temporary_memory(temp_mem);
        shd->program = 0;
        return GL_FALSE;
    }

    glAttachShader(shd->program, shd->geometry_shader);
    glAttachShader(shd->program, shd->vertex_shader);
    glAttachShader(shd->program, shd->fragment_shader);

    glLinkProgram(shd->program);

    end_temporary_memory(temp_mem);
    return GL_TRUE;
}

static void use_shader(const Shader shader)
{
    glUseProgram(shader.program);
}

static void register_buffers(RenderState &render_state, GLfloat *vertex_buffer, i32 vertex_buffer_size, GLushort *index_buffer, i32 index_buffer_count, i32 index_buffer_size, b32 has_normals, b32 has_uvs, b32 skinned, i32 buffer_handle = -1)
{
    Buffer *buffer = &render_state.buffers[buffer_handle == -1 ? render_state.buffer_count : buffer_handle];
    *buffer = {};
    buffer->vertex_buffer_size = vertex_buffer_size;
    buffer->index_buffer_count = index_buffer_count;

    if (buffer->vao == 0)
        glGenVertexArrays(1, &buffer->vao);

    bind_vertex_array(buffer->vao, render_state);

    if (buffer->vbo == 0)
        glGenBuffers(1, &buffer->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, vertex_buffer, GL_STATIC_DRAW);

    i32 bone_info_size = skinned ? 8 : 0;

    if (skinned)
    {
        if (has_normals && has_uvs)
        {
            vertex_attrib_pointer(0, 3, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), nullptr);

            vertex_attrib_pointer(1, 3, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

            vertex_attrib_pointer(2, 2, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));

            // Bone indices
            vertex_attrib_pointer(3, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(8 * sizeof(GLfloat)));

            // Weights
            vertex_attrib_pointer(4, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(12 * sizeof(GLfloat)));
        }
        else if (has_normals)
        {
            vertex_attrib_pointer(0, 3, GL_FLOAT, ((6 + bone_info_size) * sizeof(GLfloat)), nullptr);

            vertex_attrib_pointer(1, 3, GL_FLOAT, ((6 + bone_info_size) * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

            // Bone count
            vertex_attrib_pointer(2, 1, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));

            // Bone indices
            vertex_attrib_pointer(3, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(7 * sizeof(GLfloat)));

            // Weights
            vertex_attrib_pointer(4, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(11 * sizeof(GLfloat)));
        }
        else if (has_uvs)
        {
            vertex_attrib_pointer(0, 3, GL_FLOAT, ((5 + bone_info_size) * sizeof(GLfloat)), nullptr);

            vertex_attrib_pointer(1, 2, GL_FLOAT, ((5 + bone_info_size) * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

            // Bone count
            vertex_attrib_pointer(2, 1, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(5 * sizeof(GLfloat)));

            // Bone indices
            vertex_attrib_pointer(3, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));

            // Weights
            vertex_attrib_pointer(4, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(10 * sizeof(GLfloat)));
        }
        else
        {
            vertex_attrib_pointer(0, 3, GL_FLOAT, (3 + bone_info_size * sizeof(GLfloat)), nullptr);

            // Bone count
            vertex_attrib_pointer(1, 1, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

            // Bone indices
            vertex_attrib_pointer(2, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(4 * sizeof(GLfloat)));

            // Weights
            vertex_attrib_pointer(3, 4, GL_FLOAT, ((8 + bone_info_size) * sizeof(GLfloat)), (void *)(8 * sizeof(GLfloat)));
        }
    }
    else
    {
        vertex_attrib_pointer(0, 3, GL_FLOAT, (8 * sizeof(GLfloat)), nullptr);
        vertex_attrib_pointer(1, 3, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));
        vertex_attrib_pointer(2, 2, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));
    }

    if (buffer->index_buffer_count > 0)
    {
        glGenBuffers(1, &buffer->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, index_buffer, GL_STATIC_DRAW);
    }

    if (buffer_handle == -1)
        render_state.buffer_count++;
}

static void register_instance_buffer(RenderState &render_state, BufferData &buffer_data, i32 buffer_handle = -1)
{
    Buffer *buffer = &render_state.buffers[buffer_handle == -1 ? render_state.buffer_count : buffer_handle];
    *buffer = {};

    if (buffer->vao == 0)
    {
        glGenVertexArrays(1, &buffer->vao);
    }

    bind_vertex_array(buffer->vao, render_state);

    // @Incomplete: Particles
    if (buffer->vbo == 0)
    {
        glGenBuffers(1, &buffer->vbo);
    }

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer_data.instance_buffer_size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (buffer_handle == -1)
        render_state.buffer_count++;
}

GLenum get_usage(rendering::BufferUsage buffer_usage)
{
    GLenum usage = GL_DYNAMIC_DRAW;

    // @Incomplete: Copy/Read?
    switch (buffer_usage)
    {
    case rendering::BufferUsage::DYNAMIC:
        usage = GL_DYNAMIC_DRAW;
        break;
    case rendering::BufferUsage::STATIC:
        usage = GL_STATIC_DRAW;
        break;
    case rendering::BufferUsage::STREAM:
        usage = GL_STREAM_DRAW;
        break;
    }

    return usage;
}

static void delete_all_instance_buffers(RenderState *render_state, Renderer *renderer)
{
    for (i32 i = 0; i < render_state->v2.instance_buffer_count; i++)
    {
        Buffer &buffer = render_state->v2.instance_buffers[i];
        glDeleteBuffers(1, &buffer.vbo);
    }
    render_state->v2.instance_buffer_count = 0;
}

static void delete_instance_buffer(rendering::InternalBufferHandle handle, RenderState *render_state, Renderer *renderer)
{
    Buffer &buffer = render_state->v2.instance_buffers[handle.handle];
    glDeleteBuffers(1, &buffer.vbo);
    if (handle.handle < render_state->v2.instance_buffer_count - 1)
    {
        render_state->v2.instance_buffers[handle.handle] = render_state->v2.instance_buffers[render_state->v2.instance_buffer_count - 1];
    }

    render_state->v2.instance_buffer_count--;
}

static rendering::InternalBufferHandle new_create_instance_buffer(size_t buffer_size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer *renderer)
{
    Buffer &buffer = render_state->v2.instance_buffers[render_state->v2.instance_buffer_count];
    buffer = {};

    if (buffer.vbo == 0)
    {
        glGenBuffers(1, &buffer.vbo);
    }

    GLenum usage = get_usage(buffer_usage);

    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer_size, nullptr, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return {render_state->v2.instance_buffer_count++};
}

static void register_vertex_buffer(RenderState &render_state, GLfloat *buffer_data, i32 size, ShaderType shader_type, MemoryArena *perm_arena, i32 buffer_handle = -1)
{
    Buffer *buffer = &render_state.buffers[buffer_handle == -1 ? render_state.buffer_count : buffer_handle];
    *buffer = {};
    buffer->vertex_buffer_size = size;
    buffer->index_buffer_size = 0;
    buffer->ibo = 0;

    if (buffer->vao == 0)
        glGenVertexArrays(1, &buffer->vao);

    bind_vertex_array(buffer->vao, render_state);

    if (buffer->vbo == 0)
        glGenBuffers(1, &buffer->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizei)(sizeof(GLfloat) * size), buffer_data, GL_STATIC_DRAW);

    if (!render_state.shaders[shader_type].loaded)
    {
        load_shader(shader_paths[shader_type], &render_state.shaders[shader_type], perm_arena);
    }
    else
        use_shader(render_state.shaders[shader_type]);

    GLuint position_location = (GLuint)glGetAttribLocation(render_state.texture_quad_shader.program, "pos");
    GLuint texcoord_location = (GLuint)glGetAttribLocation(render_state.texture_quad_shader.program, "texcoord");

    vertex_attrib_pointer(position_location, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(texcoord_location, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (buffer_handle == -1)
        render_state.buffer_count++;
}

static void create_framebuffer_color_attachment(RenderState &render_state, Renderer &renderer, rendering::FramebufferInfo &info, Framebuffer &framebuffer, i32 width, i32 height)
{
    framebuffer.tex_color_buffer_count = info.color_attachments.count;

    for (i32 i = 0; i < info.color_attachments.count; i++)
    {
        rendering::ColorAttachment &attachment = info.color_attachments.attachments[i];
        if (attachment.type == rendering::ColorAttachmentType::RENDER_BUFFER)
        {
            if (framebuffer.tex_color_buffer_handles[i] != 0)
            {
                glDeleteRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
            }

            if (attachment.flags & rendering::ColorAttachmentFlags::MULTISAMPLED)
            {
                if (framebuffer.tex_color_buffer_handles[i] != 0)
                {
                    glDeleteRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
                }

                glGenRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
                glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.tex_color_buffer_handles[i]);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, GL_RGBA16F, width, height);
                }
                else
                {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, GL_RGBA8, width, height);
                }

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER,
                                          framebuffer.tex_color_buffer_handles[i]);
            }
            else
            {
                glGenRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
                glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.tex_color_buffer_handles[i]);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, width, height);
                }
                else
                {
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, width, height);
                }

                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER,
                                          framebuffer.tex_color_buffer_handles[i]);
            }
        }
        else
        {
            if (framebuffer.tex_color_buffer_handles[i] != 0)
            {
                glDeleteTextures(1, &framebuffer.tex_color_buffer_handles[i]);
            }

            Texture texture;
            glGenTextures(1, &texture.texture_handle);

            i32 handle = render_state.texture_index + 1;

            if (attachment.flags & rendering::ColorAttachmentFlags::MULTISAMPLED)
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture.texture_handle);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, attachment.samples, GL_RGBA16F, width, height, GL_TRUE);
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                }
                else
                {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, attachment.samples, GL_RGBA, width, height, GL_TRUE);
                    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texture.texture_handle, NULL);
                attachment.ms_texture = {handle};
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
                }
                else
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture.texture_handle, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                attachment.texture = {handle};
            }

            framebuffer.tex_color_buffer_handles[i] = texture.texture_handle;
            render_state.texture_array[render_state.texture_index] = texture;
            renderer.texture_data[renderer.texture_count].handle = handle - 1;
            renderer.texture_count++;
            render_state.texture_index++;
        }
    }
}

static void create_framebuffer_render_buffer_attachment(rendering::FramebufferInfo &info, Framebuffer &framebuffer, i32 width, i32 height)
{
    if (framebuffer.depth_buffer_handle != 0)
    {
        glDeleteRenderbuffers(1, &framebuffer.depth_buffer_handle);
    }

    glGenRenderbuffers(1, &framebuffer.depth_buffer_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth_buffer_handle);

    if (info.depth_attachment.flags & rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED)
    {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, info.depth_attachment.samples, GL_DEPTH_COMPONENT, width, height);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth_buffer_handle);
}

// @Incomplete: We should probably have a good way to link one or multiple light sources to this
static void create_shadow_map(Framebuffer &framebuffer, i32 width, i32 height)
{
    framebuffer.shadow_map.width = width;
    framebuffer.shadow_map.height = height;

    glGenFramebuffers(1, &framebuffer.buffer_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

    glGenTextures(1, &framebuffer.shadow_map_handle);
    glBindTexture(GL_TEXTURE_2D, framebuffer.shadow_map_handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Prevent shadows outside of the shadow map
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, framebuffer.shadow_map_handle, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        debug("Error: Shadow map incomplete\n");
        error_gl();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static void create_new_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer &renderer)
{
    framebuffer.width = info.width;
    framebuffer.height = info.height;

    glGenFramebuffers(1, &framebuffer.buffer_handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

    u32 color_buffer_count = info.color_attachments.count;

    u32 *attachments = (u32 *)malloc(sizeof(u32) * color_buffer_count);

    for (u32 i = 0; i < color_buffer_count; i++)
    {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    if (info.color_attachments.enabled)
        create_framebuffer_color_attachment(render_state, renderer, info, framebuffer, info.width, info.height);

    if (info.depth_attachment.enabled)
        create_framebuffer_render_buffer_attachment(info, framebuffer, info.width, info.height);

    glDrawBuffers(color_buffer_count, attachments);

    free(attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        log_error("Error: Framebuffer incomplete");
        error_gl();
    }
}

static void reload_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer &renderer, i32 width, i32 height)
{
    switch (info.size_ratio)
    {
    case 0:
        return;
        break;
    default:
        info.width = width * info.size_ratio;
        info.height = height * info.size_ratio;
        break;
    }
    // @Note(Daniel): Right now this call is identical to create_new_framebuffer, but we might want to do more here later
    create_new_framebuffer(info, framebuffer, render_state, renderer);
}

static void create_framebuffer(RenderState &render_state, Framebuffer &framebuffer, i32 width, i32 height, Shader &shader, MemoryArena *arena, r32 *vertices, u32 vertices_size, u32 *indices, u32 indices_size, b32 multisampled, i32 samples = 0, b32 hdr = false, i32 color_buffer_count = 1)
{
    if (framebuffer.buffer_handle == 0)
    {
        glGenFramebuffers(1, &framebuffer.buffer_handle);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

    u32 *attachments = (u32 *)malloc(sizeof(u32) * color_buffer_count);

    for (i32 i = 0; i < color_buffer_count; i++)
    {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }

    //    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    //create_framebuffer_color_attachment(render_state, renderer, framebuffer, width, height, multisampled, samples, hdr, color_buffer_count);
    //create_framebuffer_render_buffer_attachment(framebuffer, width, height, multisampled, samples);

    glDrawBuffers(color_buffer_count, attachments);

    free(attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        log_error("Error: Framebuffer incomplete");
        error_gl();
    }

    // FrameBuffer vao
    glGenVertexArrays(1, &framebuffer.vao);
    bind_vertex_array(framebuffer.vao, render_state);
    glGenBuffers(1, &framebuffer.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, framebuffer.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_size, vertices, GL_DYNAMIC_DRAW);

    shader.type = SHADER_FRAME_BUFFER;

    // @Incomplete: This should not be loaded more than once!
    load_shader(shader_paths[SHADER_FRAME_BUFFER], &shader, render_state.perm_arena);

    auto pos_loc = (GLuint)glGetAttribLocation(shader.program, "pos");
    auto tex_loc = (GLuint)glGetAttribLocation(shader.program, "texcoord");

    vertex_attrib_pointer(pos_loc, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(tex_loc, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    render_state.framebuffers[render_state.current_framebuffer].tex0_loc = (GLuint)glGetUniformLocation(shader.program, "tex");

    glGenBuffers(1, &framebuffer.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, framebuffer.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_size, indices, GL_STATIC_DRAW);
    bind_vertex_array(0, render_state);
}

void window_iconify_callback(GLFWwindow *window, i32 iconified)
{
    RenderState *render_state = (RenderState *)glfwGetWindowUserPointer(window);

    if (iconified)
    {
        render_state->paused = true;
    }
    else
    {
        render_state->paused = false;
    }
}

static void reload_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer &renderer, i32 width, i32 height);

void frame_buffer_size_callback(GLFWwindow *window, int width, int height)
{
    if (width != 0 && height != 0)
    {

        RenderState *render_state = rendering_state.render_state;
        Renderer *renderer = rendering_state.renderer;

        glViewport(0, 0, width, height);
        render_state->framebuffer_width = width;
        render_state->framebuffer_height = height;

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        memcpy(render_state->viewport, viewport, sizeof(GLint) * 4);

        for (i32 i = 0; i < renderer->render.framebuffer_count; i++)
        {
            rendering::FramebufferInfo &info = renderer->render.framebuffers[i];
            Framebuffer &framebuffer = render_state->v2.framebuffers[i];
            reload_framebuffer(info, framebuffer, *render_state, *renderer, width, height);
        }

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);

        glfwSetWindowPos(render_state->window, mode->width / 2 - width / 2, mode->height / 2 - height / 2);
    }
}

static void setup_quad(RenderState &render_state, MemoryArena *arena)
{
    //Quad EBO
    glGenBuffers(1, &render_state.quad_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.quad_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state.quad_indices), render_state.quad_indices, GL_STATIC_DRAW);

    //Quad VBO/VAO
    glGenVertexArrays(1, &render_state.quad_vao);
    bind_vertex_array(render_state.quad_vao, render_state);
    glGenBuffers(1, &render_state.quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.quad_vertices_size, render_state.quad_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.quad_index_buffer);

    render_state.quad_shader.type = SHADER_QUAD;
    load_shader(shader_paths[SHADER_QUAD], &render_state.quad_shader, arena);

    vertex_attrib_pointer(0, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    glBindVertexArray(0);

    // Framebuffer quad
    glGenVertexArrays(1, &render_state.framebuffer_quad_vao);
    bind_vertex_array(render_state.framebuffer_quad_vao, render_state);
    glGenBuffers(1, &render_state.framebuffer_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.framebuffer_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.framebuffer_quad_vertices_size, render_state.framebuffer_quad_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.quad_index_buffer);

    vertex_attrib_pointer(0, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    bind_vertex_array(0, render_state);

    // Textured Quad

    glGenVertexArrays(1, &render_state.texture_quad_vao);
    bind_vertex_array(render_state.texture_quad_vao, render_state);
    glGenBuffers(1, &render_state.texture_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.texture_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.texture_quad_vertices_size, render_state.texture_quad_vertices, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &render_state.texture_quad_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.texture_quad_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state.quad_indices), render_state.quad_indices, GL_STATIC_DRAW);

    render_state.texture_quad_shader.type = SHADER_TEXTURE_QUAD;
    load_shader(shader_paths[SHADER_TEXTURE_QUAD], &render_state.texture_quad_shader, arena);

    vertex_attrib_pointer(0, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    bind_vertex_array(0, render_state);

    glGenVertexArrays(1, &render_state.rounded_quad_vao);
    bind_vertex_array(render_state.rounded_quad_vao, render_state);
    glGenBuffers(1, &render_state.rounded_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.rounded_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.rounded_quad_vertices_size, render_state.rounded_quad_vertices, GL_DYNAMIC_DRAW);
    glGenBuffers(1, &render_state.rounded_quad_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.rounded_quad_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state.quad_indices), render_state.quad_indices, GL_STATIC_DRAW);

    render_state.rounded_quad_shader.type = SHADER_ROUNDED_QUAD;
    load_shader(shader_paths[SHADER_ROUNDED_QUAD], &render_state.rounded_quad_shader, arena);

    vertex_attrib_pointer(0, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    bind_vertex_array(0, render_state);
}

static void setup_billboard(RenderState &render_state, MemoryArena *arena)
{
    //Quad EBO
    glGenBuffers(1, &render_state.billboard_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.billboard_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(render_state.quad_indices), render_state.quad_indices, GL_STATIC_DRAW);

    //Quad VBO/VAO
    glGenVertexArrays(1, &render_state.billboard_vao);
    bind_vertex_array(render_state.billboard_vao, render_state);
    glGenBuffers(1, &render_state.billboard_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.billboard_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.billboard_vertices_size, render_state.billboard_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.billboard_ibo);

    render_state.quad_shader.type = SHADER_QUAD;
    load_shader(shader_paths[SHADER_QUAD], &render_state.quad_shader, arena);

    auto position_location = (GLuint)glGetAttribLocation(render_state.quad_shader.program, "pos");
    vertex_attrib_pointer(position_location, 3, GL_FLOAT, 3 * sizeof(float), nullptr);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);

    bind_vertex_array(0, render_state);
}

static void setup_lines(RenderState &render_state, MemoryArena *arena)
{
    load_shader(shader_paths[SHADER_LINE], &render_state.line_shader, arena);
    glGenVertexArrays(1, &render_state.line_vao);
    bind_vertex_array(render_state.line_vao, render_state);
    glGenBuffers(1, &render_state.line_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.line_vbo);

    glGenBuffers(1, &render_state.line_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.line_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * LINE_INDICES, render_state.line_indices, GL_STATIC_DRAW);

    bind_vertex_array(0, render_state);
}

static void create_standard_cursors(RenderState &render_state)
{
    render_state.cursors[CURSOR_ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    render_state.cursors[CURSOR_IBEAM] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    render_state.cursors[CURSOR_CROSSHAIR] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
    render_state.cursors[CURSOR_HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    render_state.cursors[CURSOR_HRESIZE] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    render_state.cursors[CURSOR_VRESIZE] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
}

static void render_setup(RenderState *render_state, MemoryArena *perm_arena)
{
    render_state->perm_arena = perm_arena;

    glfwGetFramebufferSize(render_state->window, &render_state->framebuffer_width, &render_state->framebuffer_height);

    //    create_framebuffer(*render_state, render_state->framebuffers[render_state->current_framebuffer], render_state->framebuffer_width, render_state->framebuffer_height, render_state->frame_buffer_shader, &render_state->framebuffer_arena, render_state->framebuffer_quad_vertices,
    //render_state->framebuffer_quad_vertices_size,render_state->quad_indices, sizeof(render_state->quad_indices), false, 1, true, 2);

    // @Note: Bloom stuff

    glGenFramebuffers(3, render_state->bloom.ping_pong_fbo);
    glGenTextures(3, render_state->bloom.ping_pong_buffer);

    for (i32 i = 0; i < 3; i++)
    {
        glViewport(0, 0, render_state->framebuffer_width, render_state->framebuffer_height);
        glBindFramebuffer(GL_FRAMEBUFFER, render_state->bloom.ping_pong_fbo[i]);
        glBindTexture(GL_TEXTURE_2D, render_state->bloom.ping_pong_buffer[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, render_state->framebuffer_width, render_state->framebuffer_height, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, render_state->bloom.ping_pong_buffer[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            debug("Error: Bloom FBO incomplete\n");
            error_gl();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // END BLOOM

    render_state->depth_shader.type = SHADER_DEPTH;
    render_state->depth_instanced_shader.type = SHADER_DEPTH_INSTANCED;

    load_shader(shader_paths[SHADER_DEPTH], &render_state->depth_shader, render_state->perm_arena);
    load_shader(shader_paths[SHADER_DEPTH_INSTANCED], &render_state->depth_instanced_shader, render_state->perm_arena);

    create_shadow_map(render_state->shadow_map_buffer, 2048, 2048);

    setup_billboard(*render_state, render_state->perm_arena);
    setup_quad(*render_state, render_state->perm_arena);
    setup_lines(*render_state, render_state->perm_arena);
    
    render_state->mesh_shader.type = SHADER_MESH;
    load_shader(shader_paths[SHADER_MESH], &render_state->mesh_shader, render_state->perm_arena);

    render_state->mesh_instanced_shader.type = SHADER_MESH_INSTANCED;
    load_shader(shader_paths[SHADER_MESH_INSTANCED], &render_state->mesh_instanced_shader, render_state->perm_arena);

    render_state->particle_shader.type = SHADER_PARTICLES;
    load_shader(shader_paths[SHADER_PARTICLES], &render_state->particle_shader, render_state->perm_arena);

    render_state->total_delta = 0.0f;
    render_state->frame_delta = 0.0f;

    render_state->buffers = push_array(render_state->perm_arena, global_max_custom_buffers, Buffer);
    render_state->gl_buffers = push_array(render_state->perm_arena, global_max_custom_buffers, Buffer);
}

static GLuint load_texture(TextureData &data, Texture *texture, b32 free_buffer = true)
{
    if (texture->texture_handle == 0)
    {
        glGenTextures(1, &texture->texture_handle);
    }

    glBindTexture(GL_TEXTURE_2D, texture->texture_handle);

    if (data.wrap == REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (data.filtering == LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (data.filtering == NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    //enable alpha for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum format = GL_RGBA;

    switch (data.format)
    {
    case TextureFormat::RGBA:
    {
        format = GL_RGBA;
    }
    break;
    case TextureFormat::RGB:
    {
        format = GL_RGB;
    }
    break;
    case TextureFormat::RED:
    {
        format = GL_RED;
    }
    break;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, data.width, data.height, 0, format,
                 GL_UNSIGNED_BYTE, (GLvoid *)data.image_data);

    if (free_buffer)
    {
        stbi_image_free(data.image_data);
        data.image_data = nullptr;
    }

    return GL_TRUE;
}

static void load_textures(RenderState &render_state, Renderer &renderer)
{
    for (i32 index = render_state.texture_index; index < renderer.texture_count; index++)
    {
        load_texture(renderer.texture_data[index], &render_state.texture_array[render_state.texture_index++]);
    }
}

static void load_extra_shaders(RenderState &render_state, Renderer &renderer)
{
    // @Note: Load the "new" shader system shaders
    for (i32 index = render_state.gl_shader_count; index < renderer.render.shader_count; index++)
    {
        rendering::Shader &shader = renderer.render.shaders[index];
        ShaderGL &gl_shader = render_state.gl_shaders[index];
        gl_shader.location_count = 0;

        if (shader.loaded)
        {
            load_shader(renderer, shader, gl_shader);
        }
    }

    // @Note: Even if loading a shader fails, we don't want to continue to compile it
    render_state.gl_shader_count = renderer.render.shader_count;
}

static const GLFWvidmode *create_open_gl_window(RenderState &render_state, WindowMode window_mode, const char *title, i32 width, i32 height)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    i32 screen_width = width;
    i32 screen_height = height;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    render_state.window_mode = window_mode;
    render_state.window_title = push_string(&render_state.string_arena, strlen(title) + 1);
    strcpy(render_state.window_title, title);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int widthMM, heightMM;
    glfwGetMonitorPhysicalSize(glfwGetPrimaryMonitor(), &widthMM, &heightMM);
    render_state.screen_dpi = mode->width / (widthMM / 25.4);
    render_state.density_factor = 160.0f / (r32)render_state.screen_dpi;

    render_state.refresh_rate = mode->refreshRate;

    if (window_mode == FM_BORDERLESS)
    {
        screen_width = mode->width;
        screen_height = mode->height;
    }

    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    //glfwWindowHint(GLFW_SAMPLES, 8);

    debug_log("refresh rate %d", mode->refreshRate);

    if (window_mode == FM_WINDOWED)
    {
        monitor = nullptr;
    }

    auto old_window = render_state.window;

    render_state.window = glfwCreateWindow(screen_width, screen_height, render_state.window_title, monitor,
                                           nullptr);
    render_state.framebuffers[render_state.current_framebuffer].buffer_handle = 0;

    if (old_window)
    {
        glfwDestroyWindow(old_window);
    }

    //center window on screen (windowed?)
    if (window_mode == FM_WINDOWED)
    {
        int frame_buffer_width, frame_buffer_height;

        glfwGetFramebufferSize(render_state.window, &frame_buffer_width, &frame_buffer_height);
        glfwSetWindowPos(render_state.window, mode->width / 2 - screen_width / 2, mode->height / 2 - screen_height / 2);
    }

    return monitor ? mode : nullptr;
}

static void new_create_framebuffer(rendering::FramebufferInfo &info, RenderState *render_state, Renderer *renderer)
{
    Framebuffer &framebuffer = render_state->v2.framebuffers[render_state->v2.framebuffer_count++];
    create_new_framebuffer(info, framebuffer, *render_state, *renderer);
}

static void new_load_texture(TextureData &texture_data, RenderState *render_state, Renderer *renderer, b32 free_buffer)
{
    i32 index = texture_data.handle;

    if (index == 0)
    {
        index = render_state->texture_index++;
    }
    load_texture(texture_data, &render_state->texture_array[render_state->texture_index++], free_buffer);
}

static void initialize_opengl(RenderState &render_state, Renderer &renderer, r32 contrast, r32 brightness, WindowMode window_mode, i32 screen_width, i32 screen_height, const char *title, MemoryArena *perm_arena, b32 *do_save_config)
{
    renderer.api_functions.render_state = &render_state;
    renderer.api_functions.load_texture = &new_load_texture;
    renderer.api_functions.create_framebuffer = &new_create_framebuffer;
    renderer.api_functions.create_instance_buffer = &new_create_instance_buffer;
    renderer.api_functions.delete_instance_buffer = &delete_instance_buffer;
    renderer.api_functions.delete_all_instance_buffers = &delete_all_instance_buffers;

    auto recreate_window = render_state.window != nullptr;

    if (!recreate_window)
    {
        if (!glfwInit())
        {
            log_error("Could not initialize glfw");
            exit(EXIT_FAILURE);
        }
    }

    render_state.framebuffers[render_state.current_framebuffer].buffer_handle = 0;
    render_state.paused = false;

    glfwSetErrorCallback(error_callback);

    //@Incomplete: Figure something out here. Ask for compatible version etc
#ifdef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
#elif __linux
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif __APPLE__
    // @Note: Apple only __really__ supports OpenGL Core 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    render_state.contrast = contrast;
    render_state.brightness = brightness;

    if (screen_width == 0 || screen_height == 0)
    {
        const GLFWvidmode *original_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        screen_height = original_mode->width;
        screen_width = original_mode->height;
        *do_save_config = true;
    }

    auto mode = create_open_gl_window(render_state, window_mode, title, screen_width, screen_height);
    renderer.window_mode = render_state.window_mode;

    if (mode && renderer.window_mode == FM_BORDERLESS)
    {
        renderer.window_width = mode->width;
        renderer.window_height = mode->height;
    }
    else
    {
        renderer.window_width = screen_width;
        renderer.window_height = screen_height;
    }

    if (!render_state.window)
    {
        // @Note: If no window has been created, try and see if 3.3 works
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        create_open_gl_window(render_state, window_mode, title, screen_width, screen_height);
        renderer.window_mode = render_state.window_mode;

        if (!render_state.window)
        {
            log_error("Could not create window");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }

    glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    glfwSetFramebufferSizeCallback(render_state.window, frame_buffer_size_callback);
    glfwSetWindowIconifyCallback(render_state.window, window_iconify_callback);

    glfwMakeContextCurrent(render_state.window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSwapInterval(0);

    glfwGetFramebufferSize(render_state.window, &render_state.framebuffer_width, &render_state.framebuffer_height);
    glViewport(0, 0, render_state.framebuffer_width, render_state.framebuffer_height);

#if !defined(__APPLE__)
    //Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)message_callback, 0);
#endif
    glEnable(GL_MULTISAMPLE);
    glDisable(GL_DITHER);
    glLineWidth(2.0f);
    glEnable(GL_LINE_SMOOTH);

    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glDepthFunc(GL_LESS);

    log("%s", glGetString(GL_VERSION));
    log("Shading language supported: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    log("Glad Version: %d.%d", GLVersion.major, GLVersion.minor);

    rendering_state.render_state = &render_state;
    rendering_state.renderer = &renderer;

    glfwSetWindowUserPointer(render_state.window, &rendering_state);
    glfwSetKeyCallback(render_state.window, key_callback);
    glfwSetCharCallback(render_state.window, character_callback);
    glfwSetCursorPosCallback(render_state.window, cursor_position_callback);
    glfwSetMouseButtonCallback(render_state.window, mouse_button_callback);
    glfwSetScrollCallback(render_state.window, scroll_callback);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    memcpy(render_state.viewport, viewport, sizeof(GLint) * 4);
    memcpy(renderer.viewport, render_state.viewport, sizeof(i32) * 4);

    controller_present();

    renderer.should_close = false;

    render_setup(&render_state, perm_arena);

    if (!recreate_window)
    {
        renderer.shadow_map_matrices.depth_projection_matrix = math::ortho(-10, 10, -10, 10, 1, 7.5f);
        renderer.shadow_map_matrices.depth_view_matrix = math::look_at_with_target(math::Vec3(-2.0f, 2.0f, -1.0f), math::Vec3(0, 0, 0));
        renderer.shadow_map_matrices.depth_bias_matrix = math::Mat4(
            0.5, 0.0, 0.0, 0.0,
            0.0, 0.5, 0.0, 0.0,
            0.0, 0.0, 0.5, 0.0,
            0.5, 0.5, 0.5, 1.0);
    }

    render_state.paused = false;

    create_standard_cursors(render_state);

    renderer.framebuffer_width = render_state.framebuffer_width;
    renderer.framebuffer_height = render_state.framebuffer_height;

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    i32 video_mode_count;
    auto video_modes = glfwGetVideoModes(monitor, &video_mode_count);

    if (!renderer.available_resolutions)
    {
        renderer.available_resolutions = push_array(render_state.perm_arena, video_mode_count, Resolution);

        for (i32 video_mode_index = 0; video_mode_index < video_mode_count; video_mode_index++)
        {
            auto vm = video_modes[video_mode_index];

            auto skip = false;

            for (i32 i = 0; i < renderer.available_resolutions_count; i++)
            {
                if (renderer.available_resolutions[i].width == vm.width && renderer.available_resolutions[i].height == vm.height)
                {
                    skip = true;
                }
            }

            if (!skip)
            {
                renderer.available_resolutions[renderer.available_resolutions_count++] = {vm.width, vm.height};

                if (renderer.window_width == vm.width && renderer.window_height == vm.height)
                {
                    renderer.current_resolution_index = renderer.available_resolutions_count - 1;
                    //auto res = renderer.available_resolutions[renderer.current_resolution_index];
                    renderer.resolution = {vm.width, vm.height};
                }
            }
        }

        // @Incomplete: Replace with own sort? Don't like using qsort here :(
        qsort(renderer.available_resolutions, (size_t)renderer.available_resolutions_count, sizeof(Resolution), [](const void *a, const void *b) {
            auto r_1 = (const Resolution *)a;
            auto r_2 = (const Resolution *)b;

            auto width_diff = r_1->width - r_2->width;
            auto height_diff = r_1->height - r_2->height;

            if (width_diff == 0)
            {
                return height_diff;
            }
            else
            {
                return (r_1->width - r_2->width);
            }
        });
    }
    else
    {
        for (i32 res_index = 0; res_index < renderer.available_resolutions_count; res_index++)
        {
            auto resolution = renderer.available_resolutions[res_index];
            if (renderer.window_width == resolution.width && renderer.window_height == resolution.height)
            {
                renderer.current_resolution_index = res_index;
                break;
            }
        }
    }
}

static void initialize_opengl(RenderState &render_state, Renderer &renderer, ConfigData *config_data, MemoryArena *perm_arena, b32 *do_save_config)
{
    initialize_opengl(render_state, renderer, config_data->contrast, config_data->brightness, config_data->window_mode, config_data->screen_width, config_data->screen_height, config_data->title, perm_arena, do_save_config);
}

static void delete_shaders(RenderState &render_state)
{
    for (i32 i = 0; i < SHADER_COUNT; i++)
    {
        auto type = (ShaderType)i;
        glDeleteProgram(render_state.shaders[type].program);
        glDeleteShader(render_state.shaders[type].geometry_shader);
        glDeleteShader(render_state.shaders[type].vertex_shader);
        glDeleteShader(render_state.shaders[type].fragment_shader);
    }
}

static void reload_vertex_shader(ShaderType type, RenderState *render_state, MemoryArena *arena)
{
    glDeleteProgram(render_state->shaders[type].program);
    glDeleteShader(render_state->shaders[type].vertex_shader);
    load_vertex_shader(shader_paths[type], &render_state->shaders[type], arena);
}

static void reload_fragment_shader(ShaderType type, RenderState *render_state, MemoryArena *arena)
{
    glDeleteProgram(render_state->shaders[type].program);
    glDeleteShader(render_state->shaders[type].fragment_shader);
    load_fragment_shader(shader_paths[type], &render_state->shaders[type], arena);
}

static void reload_geometry_shader(ShaderType type, RenderState *render_state, MemoryArena *arena)
{
    glDeleteProgram(render_state->shaders[type].program);
    glDeleteShader(render_state->shaders[type].geometry_shader);
    load_geometry_shader(shader_paths[type], &render_state->shaders[type], arena);
}

static void reload_assets(RenderState &render_state, AssetManager *asset_manager, MemoryArena *arena)
{
    for (int i = 0; i < SHADER_COUNT; i++)
    {
        if (asset_manager->dirty_vertex_shader_indices[i] == 1)
        {
            debug("Reloading vertex shader type: %s\n", shader_enum_to_str((ShaderType)i));
            reload_vertex_shader((ShaderType)i, &render_state, arena);
            asset_manager->dirty_vertex_shader_indices[i] = 0;
        }

        if (asset_manager->dirty_fragment_shader_indices[i] == 1)
        {
            debug("Reloading fragment shader type: %s\n", shader_enum_to_str((ShaderType)i));
            reload_fragment_shader((ShaderType)i, &render_state, arena);
            asset_manager->dirty_fragment_shader_indices[i] = 0;
        }

        if (asset_manager->dirty_geometry_shader_indices[i] == 1)
        {
            debug("Reloading geometry shader type: %s\n", shader_enum_to_str((ShaderType)i));
            reload_geometry_shader((ShaderType)i, &render_state, arena);
            asset_manager->dirty_geometry_shader_indices[i] = 0;
        }
    }
}

static void set_float_uniform(GLuint shader_handle, const char *uniform_name, r32 value)
{
    glUniform1f(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_float_uniform(GLuint shader_handle, GLuint location, r32 value)
{
    glUniform1f(location, value);
}

static void set_int_uniform(GLuint shader_handle, const char *uniform_name, i32 value)
{
    glUniform1i(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_int_uniform(GLuint shader_handle, GLuint location, i32 value)
{
    glUniform1i(location, value);
}

static void set_bool_uniform(GLuint shader_handle, const char *uniform_name, b32 value)
{
    glUniform1i(glGetUniformLocation(shader_handle, uniform_name), value);
}

static void set_bool_uniform(GLuint shader_handle, GLuint location, b32 value)
{
    glUniform1i(location, value);
}

static void set_vec2_uniform(GLuint shader_handle, const char *uniform_name, math::Vec2 value)
{
    glUniform2f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y);
}

static void set_vec2_uniform(GLuint shader_handle, GLuint location, math::Vec2 value)
{
    glUniform2f(location, value.x, value.y);
}

void set_vec3_uniform(GLuint shader_handle, const char *uniform_name, math::Vec3 value)
{
    glUniform3f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z);
}

void set_vec3_uniform(GLuint shader_handle, GLuint location, math::Vec3 value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

static void set_vec4_uniform(GLuint shader_handle, const char *uniform_name, math::Vec4 value)
{
    glUniform4f(glGetUniformLocation(shader_handle, uniform_name), value.x, value.y, value.z, value.w);
}

static void set_vec4_uniform(GLuint shader_handle, GLuint location, math::Vec4 value)
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

static void set_mat4_uniform(GLuint shader_handle, const char *uniform_name, math::Mat4 v)
{
    glUniformMatrix4fv(glGetUniformLocation(shader_handle, uniform_name), 1, GL_TRUE, &v[0][0]);
}

static void set_mat4_uniform(GLuint shader_handle, GLint location, math::Mat4 v)
{
    glUniformMatrix4fv(location, 1, GL_TRUE, &v[0][0]);
}

void set_vec4_array_uniform(GLuint shader_handle, const char *uniform_name, math::Vec4 *value, u32 length)
{
    glUniform4fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat *)&value[0]);
}

void set_float_array_uniform(GLuint shader_handle, const char *uniform_name, r32 *value, u32 length)
{
    glUniform1fv(glGetUniformLocation(shader_handle, uniform_name), (GLsizei)length, (GLfloat *)&value[0]);
}

static void set_texture_uniform(GLuint shader_handle, GLuint texture, i32 index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, texture);
}

static void set_ms_texture_uniform(GLuint shader_handle, GLuint texture, i32 index)
{
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
}

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

static void prepare_shader(const Shader shader, ShaderAttribute *attributes, i32 shader_attribute_count)
{
    use_shader(shader);

    for (i32 index = 0; index < shader_attribute_count; index++)
    {
        ShaderAttribute &attribute = attributes[index];
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

static void render_mesh(const RenderCommand &render_command, Renderer &renderer, RenderState &render_state, math::Mat4 projection_matrix, math::Mat4 view_matrix, b32 for_shadow_map, ShadowMapMatrices *shadow_map_matrices = nullptr)
{
    i32 _internal_buffer_handle = renderer._internal_buffer_handles[render_command.mesh.buffer_handle - 1];

    Buffer buffer = render_state.buffers[_internal_buffer_handle];
    bind_vertex_array(buffer.vao, render_state);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
    Shader shader = render_state.mesh_shader;

    if (for_shadow_map)
    {
        shader = render_state.depth_shader;
        use_shader(shader);
    }
    else
    {
        use_shader(shader);
    }

    vertex_attrib_pointer(0, 3, GL_FLOAT, (8 * sizeof(GLfloat)), nullptr);
    vertex_attrib_pointer(1, 3, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));
    vertex_attrib_pointer(2, 2, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));

    math::Mat4 model_matrix(1.0f);
    model_matrix = math::scale(model_matrix, render_command.scale);

    math::Vec3 rotation = render_command.rotation;

    auto orientation = math::Quat();
    orientation = math::rotate(orientation, rotation.x, math::Vec3(1.0f, 0.0f, 0.0f));
    orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, 1.0f, 0.0f));
    orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, 1.0f));

    model_matrix = to_matrix(orientation) * model_matrix;

    model_matrix = math::translate(model_matrix, render_command.position);

    set_mat4_uniform(shader.program, "projectionMatrix", projection_matrix);
    set_mat4_uniform(shader.program, "viewMatrix", view_matrix);
    set_mat4_uniform(shader.program, "modelMatrix", model_matrix);

    if (!for_shadow_map)
    {
        glUniform1i(glGetUniformLocation(shader.program, "diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(shader.program, "specularTexture"), 1);
        glUniform1i(glGetUniformLocation(shader.program, "ambientTexture"), 2);
        glUniform1i(glGetUniformLocation(shader.program, "specularIntensityTexture"), 3);
        glUniform1i(glGetUniformLocation(shader.program, "shadowMap"), 4);

        if (render_command.mesh.diffuse_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh.diffuse_texture - 1].handle];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "hasTexture", true);
        }
        else
            set_bool_uniform(shader.program, "hasTexture", false);

        if (render_command.mesh.specular_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh.specular_texture - 1].handle];

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "hasSpecular", true);
        }
        else
        {
            set_bool_uniform(shader.program, "hasSpecular", false);
        }

        if (render_command.mesh.ambient_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh.ambient_texture - 1].handle];

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "hasAmbient", true);
        }
        else
        {
            set_bool_uniform(shader.program, "hasAmbient", false);
        }

        glActiveTexture(GL_TEXTURE3);
        if (render_command.mesh.specular_intensity_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh.specular_intensity_texture - 1].handle];

            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "hasSpecularIntensity", true);
        }
        else
        {
            set_bool_uniform(shader.program, "hasSpecularIntensity", false);
        }

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, render_state.shadow_map_buffer.shadow_map_handle);

        set_bool_uniform(shader.program, "receivesShadows", render_command.receives_shadows);
        set_mat4_uniform(shader.program, "depthModelMatrix", shadow_map_matrices->depth_model_matrix);
        set_mat4_uniform(shader.program, "depthBiasMatrix", shadow_map_matrices->depth_bias_matrix);
        set_mat4_uniform(shader.program, "depthViewMatrix", shadow_map_matrices->depth_view_matrix);
        set_mat4_uniform(shader.program, "depthProjectionMatrix", shadow_map_matrices->depth_projection_matrix);

        set_vec3_uniform(shader.program, "lightPosWorld", math::Vec3(0, 20, -10));

        set_vec3_uniform(shader.program, "lightSpecular", math::Vec3(1.0f));
        set_vec3_uniform(shader.program, "lightColor", math::Vec3(1.0f));
        set_vec3_uniform(shader.program, "lightAmbient", math::Vec3(0.2f));
        set_vec3_uniform(shader.program, "diffuseColor", render_command.mesh.diffuse_color.xyz);
        set_vec3_uniform(shader.program, "specularColor", render_command.mesh.specular_color.xyz);
        set_float_uniform(shader.program, "specularExponent", render_command.mesh.specular_exponent);

        set_vec3_uniform(shader.program, "ambientColor", render_command.mesh.ambient_color.xyz);

        switch (render_command.mesh.wireframe_type)
        {
        case WT_NONE:
        {
            set_bool_uniform(shader.program, "drawWireframe", false);
            set_bool_uniform(shader.program, "drawMesh", true);
        }
        break;
        case WT_WITH_MESH:
        {
            set_vec4_uniform(shader.program, "wireframeColor", render_command.mesh.wireframe_color);
            set_bool_uniform(shader.program, "drawWireframe", true);
            set_bool_uniform(shader.program, "drawMesh", true);
        }
        break;
        case WT_WITHOUT_MESH:
        {
            set_vec4_uniform(shader.program, "wireframeColor", render_command.mesh.wireframe_color);
            set_bool_uniform(shader.program, "drawWireframe", true);
            set_bool_uniform(shader.program, "drawMesh", false);
        }
        break;
        }

        set_float_uniform(shader.program, "lightPower", 1.0f);
    }

    // @Incomplete: We want this to be without anything but the vertex positions.
    // The depth shader shouldn't assume a buffer with anything else in it, so we
    // have to find a way to do this efficiently.
    if (buffer.index_buffer_count == 0)
    {
        glDrawArrays(
            GL_TRIANGLES, 0, buffer.vertex_count);
    }
    else
    {
        glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr);
    }

    glActiveTexture(GL_TEXTURE0);
}

static void render_mesh_instanced(const RenderCommand &render_command, Renderer &renderer, RenderState &render_state, math::Mat4 projection_matrix, math::Mat4 view_matrix, b32 for_shadow_map, ShadowMapMatrices *shadow_map_matrices = nullptr)
{
    i32 _internal_buffer_handle = renderer._internal_buffer_handles[render_command.mesh_instanced.buffer_handle - 1];

    i32 _internal_offset_buffer_handle = renderer._internal_buffer_handles[render_command.mesh_instanced.instance_offset_buffer_handle - 1];
    i32 _internal_color_buffer_handle = renderer._internal_buffer_handles[render_command.mesh_instanced.instance_color_buffer_handle - 1];
    i32 _internal_rotation_buffer_handle = renderer._internal_buffer_handles[render_command.mesh_instanced.instance_rotation_buffer_handle - 1];
    i32 _internal_scale_buffer_handle = renderer._internal_buffer_handles[render_command.mesh_instanced.instance_scale_buffer_handle - 1];

    Buffer buffer = render_state.buffers[_internal_buffer_handle];
    Buffer offset_instance_buffer = render_state.buffers[_internal_offset_buffer_handle];
    Buffer color_instance_buffer = render_state.buffers[_internal_color_buffer_handle];
    Buffer rotation_instance_buffer = render_state.buffers[_internal_rotation_buffer_handle];
    Buffer scale_instance_buffer = render_state.buffers[_internal_scale_buffer_handle];

    bind_vertex_array(buffer.vao, render_state);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);

    Shader shader = render_state.mesh_instanced_shader;

    if (for_shadow_map)
    {
        shader = render_state.depth_instanced_shader;
        use_shader(shader);
    }
    else
    {
        use_shader(shader);
    }

    glEnableVertexAttribArray(0);
    vertex_attrib_pointer(0, 3, GL_FLOAT, (8 * sizeof(GLfloat)), nullptr);

    glEnableVertexAttribArray(1);
    vertex_attrib_pointer(1, 3, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    vertex_attrib_pointer(2, 2, GL_FLOAT, (8 * sizeof(GLfloat)), (void *)(6 * sizeof(GLfloat)));

    // offset
    glEnableVertexAttribArray(3);
    glBindBuffer(GL_ARRAY_BUFFER, offset_instance_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(math::Vec3) * render_command.mesh_instanced.offset_count), render_command.mesh_instanced.offsets);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void *)nullptr);
    glVertexAttribDivisor(3, 1);

    // color
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, color_instance_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(math::Rgba) * render_command.mesh_instanced.offset_count), render_command.mesh_instanced.colors);

    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, (void *)nullptr);
    glVertexAttribDivisor(4, 1);

    // rotation
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, rotation_instance_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(math::Vec3) * render_command.mesh_instanced.offset_count), render_command.mesh_instanced.rotations);

    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, (void *)nullptr);
    glVertexAttribDivisor(5, 1);

    // scale
    glEnableVertexAttribArray(6);
    glBindBuffer(GL_ARRAY_BUFFER, scale_instance_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(sizeof(math::Vec3) * render_command.mesh_instanced.offset_count), render_command.mesh_instanced.scalings);

    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, (void *)nullptr);
    glVertexAttribDivisor(6, 1);

    math::Mat4 model_matrix(1.0f);
    model_matrix = math::scale(model_matrix, render_command.scale);

    math::Vec3 rotation = render_command.rotation;
    auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
    auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
    auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;

    math::Quat orientation = math::Quat();
    orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
    orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
    orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));

    model_matrix = to_matrix(orientation) * model_matrix;

    model_matrix = math::translate(model_matrix, render_command.position);

    set_mat4_uniform(shader.program, "projectionMatrix", projection_matrix);
    set_mat4_uniform(shader.program, "viewMatrix", view_matrix);
    set_mat4_uniform(shader.program, "modelMatrix", model_matrix);

    if (!for_shadow_map)
    {
        glUniform1i(glGetUniformLocation(shader.program, "material.diffuseTexture"), 0);
        glUniform1i(glGetUniformLocation(shader.program, "material.specularTexture"), 1);
        glUniform1i(glGetUniformLocation(shader.program, "material.ambientTexture"), 2);
        glUniform1i(glGetUniformLocation(shader.program, "material.specularIntensityTexture"), 3);
        glUniform1i(glGetUniformLocation(shader.program, "shadowMap"), 4);

        if (render_command.mesh_instanced.diffuse_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh_instanced.diffuse_texture - 1].handle];

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "material.hasTexture", true);
        }
        else
            set_bool_uniform(shader.program, "material.hasTexture", false);

        if (render_command.mesh_instanced.specular_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh_instanced.specular_texture - 1].handle];

            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "material.hasSpecular", true);
        }
        else
        {
            set_bool_uniform(shader.program, "material.hasSpecular", false);
        }

        if (render_command.mesh_instanced.ambient_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh_instanced.ambient_texture - 1].handle];

            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "material.hasAmbient", true);
        }
        else
        {
            set_bool_uniform(shader.program, "material.hasAmbient", false);
        }

        if (render_command.mesh_instanced.specular_intensity_texture != 0)
        {
            auto texture = render_state.texture_array[renderer.texture_data[render_command.mesh_instanced.specular_intensity_texture - 1].handle];

            glActiveTexture(GL_TEXTURE3);
            glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

            set_bool_uniform(shader.program, "material.hasSpecularIntensity", true);
        }
        else
        {
            set_bool_uniform(shader.program, "material.hasSpecularIntensity", false);
        }

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, render_state.shadow_map_buffer.shadow_map_handle);

        set_bool_uniform(shader.program, "receivesShadows", render_command.receives_shadows);
        set_mat4_uniform(shader.program, "depthModelMatrix", shadow_map_matrices->depth_model_matrix);
        set_mat4_uniform(shader.program, "depthBiasMatrix", shadow_map_matrices->depth_bias_matrix);
        set_mat4_uniform(shader.program, "depthViewMatrix", shadow_map_matrices->depth_view_matrix);
        set_mat4_uniform(shader.program, "depthProjectionMatrix", shadow_map_matrices->depth_projection_matrix);

        set_vec3_uniform(shader.program, "material.diffuseColor", render_command.mesh_instanced.diffuse_color.xyz);
        set_vec3_uniform(shader.program, "material.specularColor", render_command.mesh_instanced.specular_color.xyz);
        set_float_uniform(shader.program, "material.specularExponent", render_command.mesh_instanced.specular_exponent);

        set_vec3_uniform(shader.program, "material.ambientColor", render_command.mesh_instanced.ambient_color.xyz);

        set_bool_uniform(shader.program, "material.translucency.hasTranslucency", false);
        set_float_uniform(shader.program, "material.translucency.distortion", 0.059f);
        set_float_uniform(shader.program, "material.translucency.power", 9.8f);
        set_float_uniform(shader.program, "material.translucency.scale", 0.5f);
        set_vec3_uniform(shader.program, "material.translucency.subColor", math::Vec3(1.0f));
        set_float_uniform(shader.program, "material.dissolve", render_command.mesh_instanced.dissolve);

        switch (render_command.mesh_instanced.wireframe_type)
        {
        case WT_NONE:
        {
            set_bool_uniform(shader.program, "drawWireframe", false);
            set_bool_uniform(shader.program, "drawMesh", true);
        }
        break;
        case WT_WITH_MESH:
        {
            set_vec4_uniform(shader.program, "wireframeColor", render_command.mesh_instanced.wireframe_color);
            set_bool_uniform(shader.program, "drawWireframe", true);
            set_bool_uniform(shader.program, "drawMesh", true);
        }
        break;
        case WT_WITHOUT_MESH:
        {
            set_vec4_uniform(shader.program, "wireframeColor", render_command.mesh_instanced.wireframe_color);
            set_bool_uniform(shader.program, "drawWireframe", true);
            set_bool_uniform(shader.program, "drawMesh", false);
        }
        break;
        }
    }

    glDrawElementsInstanced(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr, render_command.mesh_instanced.offset_count);

    glActiveTexture(GL_TEXTURE0);
}

static void render_particles(RenderCommand &render_command, Renderer &renderer, RenderState &render_state, math::Mat4 projection_matrix, math::Mat4 view_matrix)
{
    for (i32 i = 0; i < renderer.particles._tagged_removed_count; i++)
    {
        if (renderer.particles._tagged_removed[i] == render_command.particles.handle)
        {
            return;
        }
    }

    if (render_command.particles.blend_mode == CBM_ONE)
    {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    glDepthMask(GL_FALSE);

    i32 _internal_offset_handle = renderer._internal_buffer_handles[render_command.particles.offset_buffer_handle - 1];
    i32 _internal_color_handle = renderer._internal_buffer_handles[render_command.particles.color_buffer_handle - 1];
    i32 _internal_size_handle = renderer._internal_buffer_handles[render_command.particles.size_buffer_handle - 1];
    i32 _internal_angle_handle = renderer._internal_buffer_handles[render_command.particles.angle_buffer_handle - 1];

    if (_internal_offset_handle == -1 || _internal_color_handle == -1 || _internal_size_handle == -1 || _internal_angle_handle == -1)
    {
        return;
    }

    Buffer offset_buffer = render_state.buffers[_internal_offset_handle];
    Buffer color_buffer = render_state.buffers[_internal_color_handle];
    Buffer size_buffer = render_state.buffers[_internal_size_handle];
    Buffer angle_buffer = render_state.buffers[_internal_angle_handle];

    bind_vertex_array(render_state.billboard_vao, render_state);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.billboard_vbo);
    Shader shader = render_state.particle_shader;

    use_shader(shader);

    vertex_attrib_pointer(0, 3, GL_FLOAT, (5 * sizeof(GLfloat)), nullptr);

    vertex_attrib_pointer(1, 2, GL_FLOAT, (5 * sizeof(GLfloat)), (void *)(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, offset_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)sizeof(math::Vec3) * render_command.particles.particle_count, render_command.particles.offsets);
    vertex_attrib_pointer(2, 3, GL_FLOAT, (3 * sizeof(GLfloat)), (void *)(0 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)sizeof(math::Vec4) * render_command.particles.particle_count, render_command.particles.colors);
    vertex_attrib_pointer(3, 4, GL_FLOAT, (4 * sizeof(GLfloat)), (void *)(0 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, size_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)sizeof(math::Vec2) * render_command.particles.particle_count, render_command.particles.sizes);
    vertex_attrib_pointer(4, 2, GL_FLOAT, 2 * sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, angle_buffer.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizei)sizeof(r32) * render_command.particles.particle_count, render_command.particles.angles);
    vertex_attrib_pointer(5, 1, GL_FLOAT, sizeof(GLfloat), (void *)(0 * sizeof(GLfloat)));

    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 0);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);

    static GLint pLoc = glGetUniformLocation(shader.program, "projectionMatrix");
    static GLint vLoc = glGetUniformLocation(shader.program, "viewMatrix");
    static GLint crLoc = glGetUniformLocation(shader.program, "cameraRight");
    static GLint cuLoc = glGetUniformLocation(shader.program, "cameraUp");

    set_mat4_uniform(shader.program, pLoc, projection_matrix);
    set_mat4_uniform(shader.program, vLoc, view_matrix);

    set_vec3_uniform(shader.program, crLoc, math::Vec3(view_matrix[0][0], view_matrix[1][0], view_matrix[2][0]));
    set_vec3_uniform(shader.program, cuLoc, math::Vec3(view_matrix[0][1], view_matrix[1][1], view_matrix[2][1]));

    static GLint wLoc = glGetUniformLocation(shader.program, "withTexture");

    // Check for texture
    if (render_command.particles.diffuse_texture != 0)
    {
        auto texture = render_state.texture_array[renderer.texture_data[render_command.particles.diffuse_texture - 1].handle];

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture.texture_handle);

        set_bool_uniform(shader.program, wLoc, true);
    }
    else
        set_bool_uniform(shader.program, wLoc, false);

    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)nullptr, render_command.particles.particle_count);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void render_buffer(const RenderCommand &command, RenderState &render_state, Renderer &renderer, math::Mat4 projection, math::Mat4 view)
{
    i32 _internal_handle = renderer._internal_buffer_handles[command.buffer.buffer_handle - 1];
    Buffer buffer = render_state.buffers[_internal_handle];

    bind_vertex_array(buffer.vao, render_state);
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

    auto shader = render_state.texture_quad_shader;
    use_shader(shader);

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
    bind_vertex_array(0, render_state);
}

static void unregister_buffers(RenderState &render_state, Renderer &renderer)
{
    GLuint vbos[global_max_custom_buffers];
    i32 vbo_count = 0;

    GLuint ibos[global_max_custom_buffers];
    i32 ibo_count = 0;

    GLuint vaos[global_max_custom_buffers];
    i32 vao_count = 0;

    for (i32 index = 0; index < renderer.removed_buffer_handle_count; index++)
    {
        i32 external_handle = renderer.removed_buffer_handles[index];
        i32 removed_buffer_handle = renderer._internal_buffer_handles[external_handle - 1];

        BufferData &data = renderer.buffers[removed_buffer_handle];

        vaos[vao_count++] = render_state.buffers[removed_buffer_handle].vao;
        vbos[vbo_count++] = render_state.buffers[removed_buffer_handle].vbo;
        render_state.buffers[removed_buffer_handle].vao = 0;
        render_state.buffers[removed_buffer_handle].vbo = 0;

        if (data.index_buffer_size != 0)
        {
            ibos[vbo_count++] = render_state.buffers[removed_buffer_handle].ibo;
            render_state.buffers[removed_buffer_handle].ibo = 0;
        }

        data = {};
    }

    glDeleteBuffers(vbo_count, vbos);
    glDeleteVertexArrays(vao_count, vaos);
    glDeleteBuffers(ibo_count, ibos);

    for (i32 index = 0; index < renderer.removed_buffer_handle_count; index++)
    {
        i32 external_handle = renderer.removed_buffer_handles[index];
        i32 removed_buffer_handle = renderer._internal_buffer_handles[external_handle - 1];

        render_state.buffers[removed_buffer_handle] = render_state.buffers[render_state.buffer_count - 1];
        render_state.buffer_count--;

        renderer.buffers[removed_buffer_handle] = renderer.buffers[renderer.buffer_count - 1];
        renderer._internal_buffer_handles[external_handle - 1] = -1;

        for (i32 internal_index = 0; internal_index < global_max_custom_buffers; internal_index++)
        {
            if (renderer._internal_buffer_handles[internal_index] == renderer.buffer_count - 1)
            {
                renderer._internal_buffer_handles[internal_index] = removed_buffer_handle;
                break;
            }
        }

        renderer.buffer_count--;
    }
    renderer.removed_buffer_handle_count = 0;
}

static void register_buffers(RenderState &render_state, Renderer &renderer)
{

    for (i32 index = render_state.buffer_count; index < renderer.buffer_count; index++)
    {
        BufferData data = renderer.buffers[index];

        if (data.for_instancing)
        {
            register_instance_buffer(render_state, data);
        }
        else if (data.index_buffer_count == 0)
        {
            register_vertex_buffer(render_state, data.vertex_buffer, (i32)data.vertex_buffer_size, data.shader_type, render_state.perm_arena, data.existing_handle);
        }
        else
        {
            register_buffers(render_state, data.vertex_buffer, data.vertex_buffer_size, data.index_buffer, data.index_buffer_count, data.index_buffer_size, data.has_normals, data.has_uvs, data.skinned, data.existing_handle);
        }
    }

    for (i32 index = 0; index < renderer.updated_buffer_handle_count; index++)
    {
        // @Note:(Niels): Get the internal handle for the updated buffer that we have set
        i32 external_handle = renderer.updated_buffer_handles[index];
        i32 updated_buffer_handle = renderer._internal_buffer_handles[external_handle - 1];

        BufferData data = renderer.buffers[updated_buffer_handle];
        if (data.for_instancing)
        {
            register_instance_buffer(render_state, data, data.existing_handle);
        }
        else if (data.index_buffer_count == 0)
        {
            register_vertex_buffer(render_state, data.vertex_buffer, (i32)data.vertex_buffer_size, data.shader_type, render_state.perm_arena, data.existing_handle);
        }
        else
        {
            register_buffers(render_state, data.vertex_buffer, data.vertex_buffer_size, data.index_buffer, data.index_buffer_count, data.index_buffer_size, data.has_normals, data.has_uvs, data.skinned, data.existing_handle);
        }
    }

    renderer.updated_buffer_handle_count = 0;
}

// @Incomplete: Ignores if the register info has new vertex attributes
static void update_buffer(Buffer &buffer, rendering::RegisterBufferInfo info, RenderState &render_state)
{
    GLenum usage = GL_DYNAMIC_DRAW;

    // @Incomplete: Copy/Read?
    switch (info.usage)
    {
    case rendering::BufferUsage::DYNAMIC:
        usage = GL_DYNAMIC_DRAW;
        break;
    case rendering::BufferUsage::STATIC:
        usage = GL_STATIC_DRAW;
        break;
    case rendering::BufferUsage::STREAM:
        usage = GL_STREAM_DRAW;
        break;
    }

    bind_vertex_array(buffer.vao, render_state);
    buffer.vertex_count = info.data.vertex_count;
    buffer.vertex_buffer_size = info.data.vertex_buffer_size;

    glBufferData(GL_ARRAY_BUFFER, info.data.vertex_buffer_size, info.data.vertex_buffer, usage);

    if (info.data.index_buffer_count > 0)
    {
        buffer.index_buffer_count = info.data.index_buffer_count;
        buffer.index_buffer_size = info.data.index_buffer_size;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.data.index_buffer_size, info.data.index_buffer, usage);
    }

    bind_vertex_array(0, render_state);
}

static void register_buffer(Buffer &buffer, rendering::RegisterBufferInfo info, RenderState &render_state)
{
    buffer.vertex_buffer_size = info.data.vertex_buffer_size;
    buffer.index_buffer_count = info.data.index_buffer_count;
    buffer.vertex_count = info.data.vertex_count;

    glGenVertexArrays(1, &buffer.vao);
    bind_vertex_array(buffer.vao, render_state);

    glGenBuffers(1, &buffer.vbo);

    glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);

    GLenum usage = get_usage(info.usage);

    glBufferData(GL_ARRAY_BUFFER, info.data.vertex_buffer_size, info.data.vertex_buffer, usage);

    size_t offset = 0;

    for (i32 i = 0; i < info.vertex_attribute_count; i++)
    {
        rendering::VertexAttribute &attrib = info.vertex_attributes[i];

        GLenum type = GL_FLOAT;
        i32 count = 1;
        size_t type_size = 0;

        switch (attrib.type)
        {
        case rendering::ValueType::FLOAT:
        {
            type = GL_FLOAT;
            count = 1;
            type_size = 1 * sizeof(GLfloat);
        }
        break;
        case rendering::ValueType::FLOAT2:
        {
            type = GL_FLOAT;
            count = 2;
            type_size = 2 * sizeof(GLfloat);
        }
        break;
        case rendering::ValueType::FLOAT3:
        {
            type = GL_FLOAT;
            count = 3;
            type_size = 3 * sizeof(GLfloat);
        }
        break;
        case rendering::ValueType::FLOAT4:
        {
            type = GL_FLOAT;
            count = 4;
            type_size = 4 * sizeof(GLfloat);
        }
        break;
        case rendering::ValueType::INTEGER:
        {
            type = GL_INT;
            count = 1;
            type_size = 1 * sizeof(GLint);
        }
        break;
        case rendering::ValueType::BOOL:
        {
            type = GL_INT;
            count = 1;
            type_size = 1 * sizeof(GLint);
        }
        break;
        case rendering::ValueType::MAT4:
        {
            type = GL_FLOAT;
            count = 16;
            type_size = 16 * sizeof(GLfloat);
        }
        break;
        case rendering::ValueType::TEXTURE:
        case rendering::ValueType::INVALID:
        {
            assert(false);
        }
        break;
        }

        vertex_attrib_pointer(i, count, type, (u32)info.stride, (void *)offset);

        offset += type_size;
    }

    if (info.data.index_buffer_count > 0)
    {
        glGenBuffers(1, &buffer.ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.data.index_buffer_size, info.data.index_buffer, usage);
    }

    bind_vertex_array(0, render_state);
}

static void register_framebuffers(RenderState &render_state, Renderer &renderer)
{
    for (i32 index = render_state.v2.framebuffer_count; index < renderer.render.framebuffer_count; index++)
    {
        rendering::FramebufferInfo &info = renderer.render.framebuffers[index];
        Framebuffer &framebuffer = render_state.v2.framebuffers[index];
        create_new_framebuffer(info, framebuffer, render_state, renderer);

        for (i32 i = 0; i < info.pending_textures.count; i++)
        {
            rendering::RenderPass &pass = renderer.render.post_processing_passes[info.pending_textures.pass_handles[i].handle - 1];
            char *name = info.pending_textures.uniform_names[i];
            rendering::ColorAttachment &attachment = info.color_attachments.attachments[info.pending_textures.color_attachment_indices[i]];

            for (i32 j = 0; j < pass.post_processing.uniform_value_count; j++)
            {
                rendering::UniformValue &uv = pass.post_processing.uniform_values[j];
                if (strcmp(uv.name, name) == 0)
                {
                    uv.texture = attachment.texture;
                    break;
                }
            }
        }

        info.pending_textures.count = 0;
    }
    render_state.v2.framebuffer_count = renderer.render.framebuffer_count;
}

//@Cleanup: Remove new
static void register_new_buffers(RenderState &render_state, Renderer &renderer)
{
    for (i32 index = render_state.gl_buffer_count; index < renderer.render.buffer_count; index++)
    {
        rendering::RegisterBufferInfo &info = renderer.render.buffers[index];
        Buffer &gl_buffer = render_state.gl_buffers[index];
        register_buffer(gl_buffer, info, render_state);
    }

    render_state.gl_buffer_count = renderer.render.buffer_count;
}

static void update_new_buffers(RenderState &render_state, Renderer &renderer)
{
    for (i32 index = 0; renderer.render.updated_buffer_handle_count; index++)
    {
        i32 handle = renderer.render.updated_buffer_handles[index];
        rendering::RegisterBufferInfo &info = renderer.render.buffers[handle];
        Buffer &gl_buffer = render_state.gl_buffers[handle];
        update_buffer(gl_buffer, info, render_state);
    }

    renderer.updated_buffer_handle_count = 0;
}

static void set_uniform(RenderState &render_state, Renderer &renderer, GLuint program, rendering::UniformValue &uniform_value, GLuint location, i32 *texture_count = nullptr)
{
    switch (uniform_value.uniform.type)
    {
    case rendering::ValueType::FLOAT:
    {
        set_float_uniform(program, location, uniform_value.float_val);
    }
    break;
    case rendering::ValueType::FLOAT2:
    {
        set_vec2_uniform(program, location, uniform_value.float2_val);
    }
    break;
    case rendering::ValueType::FLOAT3:
    {
        set_vec3_uniform(program, location, uniform_value.float3_val);
    }
    break;
    case rendering::ValueType::FLOAT4:
    {
        set_vec4_uniform(program, location, uniform_value.float4_val);
    }
    break;
    case rendering::ValueType::INTEGER:
    {
        set_int_uniform(program, location, uniform_value.integer_val);
    }
    break;
    case rendering::ValueType::BOOL:
    {
        set_bool_uniform(program, location, uniform_value.boolean_val);
    }
    break;
    case rendering::ValueType::MAT4:
    {
        set_mat4_uniform(program, location, uniform_value.mat4_val);
    }
    break;
    case rendering::ValueType::TEXTURE:
    {
        Texture texture = render_state.texture_array[renderer.texture_data[uniform_value.texture.handle - 1].handle];
        set_int_uniform(program, location, *texture_count);
        set_texture_uniform(program, texture.texture_handle, *texture_count);
        (*texture_count)++;
    }
    break;
    case rendering::ValueType::MS_TEXTURE:
    {
        Texture texture = render_state.texture_array[renderer.texture_data[uniform_value.ms_texture.handle - 1].handle];
        set_int_uniform(program, location, *texture_count);
        set_ms_texture_uniform(program, texture.texture_handle, *texture_count);
        (*texture_count)++;
    }
    break;
    case rendering::ValueType::INVALID:
        assert(false);
        break;
    }
}

static void set_uniform(rendering::Transform transform, const rendering::RenderPass &render_pass, rendering::UniformValue &uniform_value, ShaderGL &gl_shader, const Camera &camera, i32 *texture_count, RenderState &render_state, Renderer &renderer)
{
    rendering::Uniform &uniform = uniform_value.uniform;

    GLuint location = gl_shader.uniform_locations[uniform_value.uniform.location_index];
    if (location != -1)
    {
        switch (uniform.mapping_type)
        {
        case rendering::UniformMappingType::NONE:
        case rendering::UniformMappingType::DIFFUSE_TEX:
        case rendering::UniformMappingType::DIFFUSE_COLOR:
        case rendering::UniformMappingType::SPECULAR_TEX:
        case rendering::UniformMappingType::SPECULAR_COLOR:
        case rendering::UniformMappingType::SPECULAR_EXPONENT:
        case rendering::UniformMappingType::AMBIENT_COLOR:
        case rendering::UniformMappingType::AMBIENT_TEX:
        case rendering::UniformMappingType::DIRECTIONAL_LIGHT_COUNT:
        case rendering::UniformMappingType::POINT_LIGHT_POSITION:
        case rendering::UniformMappingType::DIRECTIONAL_LIGHT_DIRECTION:
        case rendering::UniformMappingType::DIRECTIONAL_LIGHT_AMBIENT:
        case rendering::UniformMappingType::DIRECTIONAL_LIGHT_DIFFUSE:
        case rendering::UniformMappingType::DIRECTIONAL_LIGHT_SPECULAR:
        case rendering::UniformMappingType::POINT_LIGHT_AMBIENT:
        case rendering::UniformMappingType::POINT_LIGHT_DIFFUSE:
        case rendering::UniformMappingType::POINT_LIGHT_SPECULAR:
        case rendering::UniformMappingType::POINT_LIGHT_CONSTANT:
        case rendering::UniformMappingType::POINT_LIGHT_LINEAR:
        case rendering::UniformMappingType::POINT_LIGHT_QUADRATIC:
        {
            set_uniform(render_state, renderer, gl_shader.program, uniform_value, location, texture_count);
        }
        break;
        case rendering::UniformMappingType::CLIPPING_PLANE:
        {
            set_vec4_uniform(gl_shader.program, location, render_pass.clipping_planes.plane);
        }
        break;
        case rendering::UniformMappingType::LIGHT_SPACE_MATRIX:
        {
            set_mat4_uniform(gl_shader.program, location, renderer.render.light_space_matrix);
        }
        break;
        case rendering::UniformMappingType::SHADOW_MAP:
        {
            set_int_uniform(gl_shader.program, location, *texture_count);
            glActiveTexture(GL_TEXTURE0 + *texture_count++);
            glBindTexture(GL_TEXTURE_2D, render_state.shadow_map_buffer.shadow_map_handle);
        }
        break;
        case rendering::UniformMappingType::SHADOW_VIEW_POSITION:
        {
            set_vec3_uniform(gl_shader.program, location, renderer.render.shadow_view_position);
        }
        break;
        case rendering::UniformMappingType::MODEL:
        {
            math::Mat4 model_matrix(1.0f);
            model_matrix = math::scale(model_matrix, transform.scale);

            math::Vec3 rotation = transform.rotation;
            auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
            auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
            auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;

            math::Quat orientation = math::Quat();
            orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
            orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
            orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));

            model_matrix = to_matrix(orientation) * model_matrix;

            model_matrix = math::translate(model_matrix, transform.position);

            set_mat4_uniform(gl_shader.program, location, model_matrix);
        }
        break;
        case rendering::UniformMappingType::VIEW:
        {
            set_mat4_uniform(gl_shader.program, location, camera.view_matrix);
        }
        break;
        case rendering::UniformMappingType::PROJECTION:
        {
            set_mat4_uniform(gl_shader.program, location, camera.projection_matrix);
        }
        break;
        case rendering::UniformMappingType::CAMERA_POSITION:
        {
            set_vec3_uniform(gl_shader.program, location, camera.position);
        }
        break;
        }
    }
}

static void setup_instanced_vertex_attribute_buffers(rendering::VertexAttributeInstanced *instanced_vertex_attributes, i32 attr_count, rendering::Shader &shader_info, RenderState &render_state, Renderer &renderer)
{
    for (i32 i = 0; i < attr_count; i++)
    {
        rendering::VertexAttributeInstanced &vertex_attribute = instanced_vertex_attributes[i];

        i32 array_num = shader_info.vertex_attribute_count + i;
        rendering::InternalBufferHandle buffer_handle = {-1};
        size_t size = 0;
        i32 count = 0;
        void *buf_ptr = nullptr;
        i32 num_values = 0;

        i32 handle = vertex_attribute.instance_buffer_handle.handle - 1;

        switch (vertex_attribute.attribute.type)
        {
        case rendering::ValueType::FLOAT:
        {
            num_values = 1;
            buffer_handle = renderer.render.instancing.float_buffer_handles[handle];
            buf_ptr = renderer.render.instancing.float_buffers[handle];
            size = sizeof(r32);
            count = renderer.render.instancing.float_buffer_counts[handle];

            assert(buffer_handle.handle >= 0);
            Buffer &buffer = render_state.v2.instance_buffers[buffer_handle.handle];
            glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
            glEnableVertexAttribArray(array_num);
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT2:
        {
            num_values = 2;
            buffer_handle = renderer.render.instancing.float2_buffer_handles[handle];
            buf_ptr = renderer.render.instancing.float2_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float2_buffer_counts[handle];

            assert(buffer_handle.handle >= 0);
            Buffer &buffer = render_state.v2.instance_buffers[buffer_handle.handle];
            glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
            glEnableVertexAttribArray(array_num);
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT3:
        {
            num_values = 3;
            buffer_handle = renderer.render.instancing.float3_buffer_handles[handle];
            buf_ptr = renderer.render.instancing.float3_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float3_buffer_counts[handle];

            assert(buffer_handle.handle >= 0);
            Buffer &new_buffer = render_state.v2.instance_buffers[buffer_handle.handle];

            glEnableVertexAttribArray(array_num);
            glBindBuffer(GL_ARRAY_BUFFER, new_buffer.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT4:
        {
            num_values = 4;
            buffer_handle = renderer.render.instancing.float4_buffer_handles[handle];
            buf_ptr = renderer.render.instancing.float4_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float4_buffer_counts[handle];

            assert(buffer_handle.handle >= 0);
            Buffer &buffer = render_state.v2.instance_buffers[buffer_handle.handle];
            glBindBuffer(GL_ARRAY_BUFFER, buffer.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
            glEnableVertexAttribArray(array_num);
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::MAT4:
        {
            num_values = 16;
            buffer_handle = renderer.render.instancing.mat4_buffer_handles[handle];
            buf_ptr = renderer.render.instancing.mat4_buffers[handle];
            size = sizeof(math::Mat4);
            count = renderer.render.instancing.mat4_buffer_counts[handle];

            assert(buffer_handle.handle >= 0);
            Buffer &new_buffer = render_state.v2.instance_buffers[buffer_handle.handle];

            GLsizei vec4_size = sizeof(math::Vec4);

            glEnableVertexAttribArray(array_num);
            glEnableVertexAttribArray(array_num + 1);
            glEnableVertexAttribArray(array_num + 2);
            glEnableVertexAttribArray(array_num + 3);

            glBindBuffer(GL_ARRAY_BUFFER, new_buffer.vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);

            glVertexAttribPointer(array_num, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void *)0);
            glVertexAttribPointer(array_num + 1, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void *)(vec4_size));
            glVertexAttribPointer(array_num + 2, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void *)(2 * vec4_size));
            glVertexAttribPointer(array_num + 3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void *)(3 * vec4_size));

            glVertexAttribDivisor(array_num, 1);
            glVertexAttribDivisor(array_num + 1, 1);
            glVertexAttribDivisor(array_num + 2, 1);
            glVertexAttribDivisor(array_num + 3, 1);
        }
        break;
        }
    }
}

static Buffer &get_internal_buffer(Renderer &renderer, RenderState &render_state, rendering::BufferHandle buffer)
{
    i32 handle = renderer.render._internal_buffer_handles[buffer.handle - 1];
    Buffer &gl_buffer = render_state.gl_buffers[handle];
    return gl_buffer;
}

static void render_buffer(rendering::Transform transform, rendering::BufferHandle& buffer_handle, const rendering::RenderPass &render_pass, RenderState &render_state, Renderer &renderer, rendering::Material& material, const Camera &camera, i32 count = 0, ShaderGL *shader = nullptr)
{
    Buffer& buffer = get_internal_buffer(renderer, render_state, buffer_handle);
    
    bind_vertex_array(buffer.vao, render_state);

    ShaderGL gl_shader;
    rendering::Shader &shader_info = renderer.render.shaders[material.shader.handle];

    // If we specified a custom shader, use it
    if (shader)
    {
        gl_shader = *shader;
    }
    else
    {
        gl_shader = render_state.gl_shaders[shader_info.index];
    }

    b32 fallback = false;

    if (!gl_shader.program) // Use fallback if it exists
    {
        fallback = true;

        gl_shader = render_state.gl_shaders[renderer.render.fallback_shader.handle];

        if (!gl_shader.program)
            return;

        if (render_state.current_state.shader_program != gl_shader.program)
        {
            glUseProgram(gl_shader.program);
            render_state.current_state.shader_program = gl_shader.program;
        }
    }
    else
    {
        if (render_state.current_state.shader_program != gl_shader.program)
        {
            glUseProgram(gl_shader.program);
            render_state.current_state.shader_program = gl_shader.program;
        }
    }

    if (fallback)
    {
        if (buffer.ibo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
            glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, buffer.vertex_count / 3);
        }
        return;
    }

    i32 texture_count = 0;

    for (i32 i = 0; i < material.uniform_value_count; i++)
    {
        rendering::UniformValue &uniform_value = material.uniform_values[i];
        rendering::Uniform &uniform = uniform_value.uniform;

        if (uniform.is_array)
        {
            rendering::UniformArray &array = material.arrays[uniform_value.array_index];
            for (i32 j = 0; j < array.entry_count; j++)
            {
                rendering::UniformEntry &entry = array.entries[j];
                for (i32 k = 0; k < entry.value_count; k++)
                {
                    rendering::UniformValue &value = entry.values[k];
                    set_uniform(transform, render_pass, value, gl_shader, camera, &texture_count, render_state, renderer);

                }
            }
        }
        else
        {
            set_uniform(transform, render_pass, uniform_value, gl_shader, camera, &texture_count, render_state, renderer);
        }
    }

    if (material.instanced_vertex_attribute_count > 0)
    {
        setup_instanced_vertex_attribute_buffers(material.instanced_vertex_attributes, material.instanced_vertex_attribute_count, shader_info, render_state, renderer);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
        glDrawElementsInstanced(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr, count);
    }
    else
    {
        if (buffer.ibo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
            glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, buffer.vertex_count / 3);
        }
    }
}

static void render_shadow_buffer(rendering::ShadowCommand &shadow_command, RenderState &render_state, Renderer &renderer)
{
    i32 handle = renderer.render._internal_buffer_handles[shadow_command.buffer.handle - 1];
    Buffer &buffer = render_state.gl_buffers[handle];

    bind_vertex_array(buffer.vao, render_state);

    ShaderGL gl_shader = render_state.gl_shaders[renderer.render.shadow_map_shader.handle];

    if (render_state.current_state.shader_program != gl_shader.program)
    {
        glUseProgram(gl_shader.program);
        render_state.current_state.shader_program = gl_shader.program;
    }

    rendering::Transform transform = shadow_command.transform;

    math::Mat4 model_matrix(1.0f);
    model_matrix = math::scale(model_matrix, transform.scale);

    math::Vec3 rotation = transform.rotation;
    auto x_axis = rotation.x > 0.0f ? 1.0f : 0.0f;
    auto y_axis = rotation.y > 0.0f ? 1.0f : 0.0f;
    auto z_axis = rotation.z > 0.0f ? 1.0f : 0.0f;

    math::Quat orientation = math::Quat();
    orientation = math::rotate(orientation, rotation.x, math::Vec3(x_axis, 0.0f, 0.0f));
    orientation = math::rotate(orientation, rotation.y, math::Vec3(0.0f, y_axis, 0.0f));
    orientation = math::rotate(orientation, rotation.z, math::Vec3(0.0f, 0.0f, z_axis));

    model_matrix = to_matrix(orientation) * model_matrix;
    model_matrix = math::translate(model_matrix, transform.position);

    static GLint lsLoc = glGetUniformLocation(gl_shader.program, "lightSpaceMatrix");
    static GLint mLoc = glGetUniformLocation(gl_shader.program, "model");

    set_mat4_uniform(gl_shader.program, lsLoc, renderer.render.light_space_matrix);
    set_mat4_uniform(gl_shader.program, mLoc, model_matrix);

    if (buffer.ibo)
    {
        glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, buffer.vertex_count);
    }

    bind_vertex_array(0, render_state);
}

static void render_instanced_shadow_buffer(rendering::ShadowCommand &shadow_command, RenderState &render_state, Renderer &renderer)
{
    i32 handle = renderer.render._internal_buffer_handles[shadow_command.buffer.handle - 1];
    Buffer &buffer = render_state.gl_buffers[handle];

    bind_vertex_array(buffer.vao, render_state);

    ShaderGL gl_shader = render_state.gl_shaders[renderer.render.shadow_map_shader.handle];
    rendering::Shader &shader_info = renderer.render.shaders[renderer.render.shadow_map_shader.handle];

    glUseProgram(gl_shader.program);

    setup_instanced_vertex_attribute_buffers(shadow_command.instanced_vertex_attributes, shadow_command.instanced_vertex_attribute_count, shader_info, render_state, renderer);

    static GLint lsLoc = glGetUniformLocation(gl_shader.program, "lightSpaceMatrix");
    set_mat4_uniform(gl_shader.program, lsLoc, renderer.render.light_space_matrix);

    if (buffer.ibo)
    {
        glDrawElementsInstanced(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr, shadow_command.count);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, buffer.vertex_count);
    }

    bind_vertex_array(0, render_state);
}

static void render_shadows(RenderState &render_state, Renderer &renderer, Framebuffer &framebuffer)
{
    glCullFace(GL_FRONT); // KILL PETER PAN!
    glViewport(0, 0, framebuffer.shadow_map.width, framebuffer.shadow_map.height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);
    glClear(GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    for (i32 i = 0; i < renderer.render.shadow_command_count; i++)
    {
        rendering::ShadowCommand &shadow_command = renderer.render.shadow_commands[i];
        if (shadow_command.count > 1)
            render_instanced_shadow_buffer(shadow_command, render_state, renderer);
        else
            render_shadow_buffer(shadow_command, render_state, renderer);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);

    renderer.render.shadow_command_count = 0;
}

static void render_ui_pass(RenderState &render_state, Renderer &renderer)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    rendering::RenderPass &pass = renderer.render.ui.pass;

    Framebuffer &framebuffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];

    glViewport(0, 0, framebuffer.width, framebuffer.height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

    Buffer &font_buffer = get_internal_buffer(renderer, render_state, renderer.render.ui.font_buffer);

    i32 internal_handle = renderer.render._internal_buffer_handles[renderer.render.ui.font_buffer.handle - 1];

    rendering::RegisterBufferInfo &info = renderer.render.buffers[internal_handle];

    bind_vertex_array(font_buffer.vao, render_state);
    glBindBuffer(GL_ARRAY_BUFFER, font_buffer.vbo);

    for (i32 i = 0; i < pass.ui.text_command_count; i++)
    {
        rendering::TextRenderCommand &command = pass.ui.text_commands[i];

        // if (command.clip)
        // {
        //     glEnable(GL_SCISSOR_TEST);
        //     math::Rect clip_rect = command.clip_rect;
        //     glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
        // }

        rendering::CharacterBufferHandle char_buf_handle = command.buffer;
        rendering::CharacterData *coords = pass.ui.coords[char_buf_handle.handle];

        TrueTypeFontInfo tt_font = renderer.tt_font_infos[command.font.handle];

        // @Incomplete: Reload fonts

        info.data.vertex_buffer_size = (i32)(6 * command.text_length * sizeof(rendering::CharacterData));
        info.data.vertex_count = (i32)(6 * command.text_length * 3);
        info.data.vertex_buffer = (r32 *)coords;

        update_buffer(font_buffer, info, render_state);

        render_buffer({}, renderer.render.ui.font_buffer, pass, render_state, renderer, command.material, pass.camera, 0 ,&render_state.gl_shaders[command.shader_handle.handle]);

        if (command.clip)
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    for (i32 i = 0; i < pass.ui.render_command_count; i++)
    {
        rendering::UIRenderCommand &command = pass.ui.render_commands[i];

        // if (command.clip)
        // {
        //     glEnable(GL_SCISSOR_TEST);
        //     math::Rect clip_rect = command.clip_rect;
        //     glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
        // }

        render_buffer(command.transform, command.buffer, pass, render_state, renderer, command.material, pass.camera, 0, &render_state.gl_shaders[command.shader_handle.handle]);

        if (command.clip)
        {
            glDisable(GL_SCISSOR_TEST);
        }
    }

    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    pass.ui.text_command_count = 0;
    pass.ui.render_command_count = 0;
}

static void render_all_passes(RenderState &render_state, Renderer &renderer)
{
    bind_vertex_array(0, render_state);

    // @Incomplete: Create a better way for enabling/disabling the clipping planes
    // Check if we have clipping planes
    glEnable(GL_CLIP_PLANE0);

    // Go backwards through the array to enable easy render pass adding
    for (i32 pass_index = renderer.render.pass_count - 1; pass_index >= 0; pass_index--)
    {
        rendering::RenderPass &pass = renderer.render.passes[pass_index];

        Framebuffer &framebuffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];

        glViewport(0, 0, framebuffer.width, framebuffer.height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

        // @Incomplete: Not all framebuffers should have depth testing or clear both bits
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(renderer.clear_color.r, renderer.clear_color.g, renderer.clear_color.b, renderer.clear_color.a);

        for (i32 i = 0; i < pass.commands.render_command_count; i++)
        {
            rendering::RenderCommand &command = pass.commands.render_commands[i];

            rendering::Material &material = get_material_instance(command.material, renderer);

            render_buffer(command.transform, command.buffer, pass, render_state, renderer, material, pass.camera, command.count, &render_state.gl_shaders[command.pass.shader_handle.handle]);
        }

        pass.commands.render_command_count = 0;
    }

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);

    for (i32 i = 0; i < renderer.render.instancing.float3_buffer_count; i++)
    {
        renderer.render.instancing.float3_buffer_counts[i] = 0;
    }

    for (i32 i = 0; i < renderer.render.instancing.mat4_buffer_count; i++)
    {
        renderer.render.instancing.mat4_buffer_counts[i] = 0;
    }
}

static void render_new_commands(RenderState &render_state, Renderer &renderer)
{
    glEnable(GL_DEPTH_TEST);

    for (i32 i = 0; i < renderer.render.render_command_count; i++)
    {
        //rendering::RenderCommand &command = renderer.render.render_commands[i];
        //render_buffer(command, , render_state, renderer, renderer.camera);
    }

    glActiveTexture(GL_TEXTURE0);

    renderer.render.render_command_count = 0;
}

static void render_commands(RenderState &render_state, Renderer &renderer)
{
    //glEnable(GL_DEPTH_TEST);

    for (i32 index = 0; index < renderer.command_count; index++)
    {
        RenderCommand &command = renderer.commands[index];

        switch (command.type)
        {
            //         case RENDER_COMMAND_LINE:
            //         {
            //             render_line(command, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix);
            //         }
            //         break;
            //         case RENDER_COMMAND_TEXT:
            //         {
            //             render_text(command, render_state, renderer, renderer.camera.view_matrix, renderer.camera.projection_matrix);
            //         }
            //         break;
            //         case RENDER_COMMAND_3D_TEXT:
            //         {
            //             render_3d_text(command, render_state, renderer, renderer.camera.view_matrix, renderer.camera.projection_matrix);
            //         }
            //         break;
            //         case RENDER_COMMAND_QUAD:
            //         {
            //             render_quad(command, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix);
            //         }
            //         break;
            //         case RENDER_COMMAND_MODEL:
            //         {
            //             //render_model(command, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix);

            //         }
            //         break;
            //         case RENDER_COMMAND_MESH:
            //         {
            //             render_mesh(command, renderer, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix, false, &renderer.shadow_map_matrices);

            //         }
            //         break;
        case RENDER_COMMAND_PARTICLES:
        {
            render_particles(command, renderer, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix);
        }
        break;
            //         case RENDER_COMMAND_MESH_INSTANCED:
            //         {
            //             render_mesh_instanced(command, renderer, render_state, renderer.camera.projection_matrix, renderer.camera.view_matrix, false, &renderer.shadow_map_matrices);
            //         }
            //         break;
            //         case RENDER_COMMAND_BUFFER:
            //         {
            //             render_buffer(command, render_state, renderer, renderer.camera.projection_matrix, renderer.camera.view_matrix);
            //         }
            //         break;
            //         case RENDER_COMMAND_DEPTH_TEST:
            //         {
            //             if (command.depth_test.on)
            //             {
            //                 glEnable(GL_DEPTH_TEST);
            //             }
            //             else
            //             {
            //                 glDisable(GL_DEPTH_TEST);
            //             }
            //         }
            //         break;
            //         case RENDER_COMMAND_CURSOR:
            //         {
            //             glfwSetCursor(render_state.window, render_state.cursors[command.cursor.type]);
            //         }
            //         break;
            //         default:
            //         break;
        }
    }

    renderer.command_count = 0;
    //clear(&renderer.commands);

    //glDisable(GL_DEPTH_TEST);

    //clear(&renderer.ui_commands);
}

static void swap_buffers(RenderState &render_state)
{
    glfwSwapBuffers(render_state.window);
}

static void check_window_mode_and_size(RenderState &render_state, Renderer &renderer, b32 *save_config)
{
    if (renderer.window_width != render_state.window_width || renderer.window_height != render_state.window_height || renderer.window_mode != render_state.window_mode)
    {
        render_state.window_width = renderer.window_width;
        render_state.window_height = renderer.window_height;
        *save_config = true;

        if (renderer.window_mode != render_state.window_mode)
        {
            const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            if (renderer.window_mode == FM_BORDERLESS)
            {
                glfwSetWindowMonitor(render_state.window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
                renderer.window_width = mode->width;
                renderer.window_height = mode->height;

                for (i32 res_index = 0; res_index < renderer.available_resolutions_count; res_index++)
                {
                    auto res = renderer.available_resolutions[res_index];
                    if (res.width == renderer.window_width && res.height == renderer.window_height)
                    {
                        renderer.current_resolution_index = res_index;
                        break;
                    }
                }
            }
            else
            {
                glfwSetWindowMonitor(render_state.window, nullptr, mode->width / 2 - renderer.window_width / 2, mode->height / 2 - renderer.window_height / 2, renderer.window_width, renderer.window_height, 0);
                glfwSetWindowSize(render_state.window, render_state.window_width, render_state.window_height);

                for (i32 res_index = 0; res_index < renderer.available_resolutions_count; res_index++)
                {
                    auto res = renderer.available_resolutions[res_index];
                    if (res.width == renderer.window_width && res.height == renderer.window_height)
                    {
                        renderer.current_resolution_index = res_index;
                        break;
                    }
                }
            }

            render_state.window_mode = renderer.window_mode;
        }
        else
        {
            glfwSetWindowSize(render_state.window, render_state.window_width, render_state.window_height);
        }
    }
}

static void render_bloom(RenderState &render_state, Renderer &renderer)
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    b32 horizontal = true;
    b32 first_iteration = true;
    i32 amount = renderer.render.bloom.amount;

    Framebuffer final_buffer = render_state.v2.framebuffers[0];

    bind_vertex_array(render_state.framebuffer_quad_vao, render_state);
    ShaderGL gl_shader = render_state.gl_shaders[renderer.render.blur_shader.handle];
    glUseProgram(gl_shader.program);

    for (i32 i = 0; i < amount; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, render_state.bloom.ping_pong_fbo[horizontal]);

        glActiveTexture(GL_TEXTURE0);

        GLuint tex = first_iteration ? final_buffer.tex_color_buffer_handles[1] : render_state.bloom.ping_pong_buffer[!horizontal];

        glBindTexture(GL_TEXTURE_2D, tex);
        set_int_uniform(gl_shader.program, "image", 0);
        set_int_uniform(gl_shader.program, "horizontal", horizontal);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)nullptr);
        horizontal = !horizontal;

        if (first_iteration)
            first_iteration = false;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, render_state.bloom.ping_pong_fbo[2]);

    ShaderGL bloom_shader = render_state.gl_shaders[renderer.render.bloom_shader.handle];
    glUseProgram(bloom_shader.program);

    set_int_uniform(bloom_shader.program, "scene", 0);
    set_int_uniform(bloom_shader.program, "bloomBlur", 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_state.framebuffers[render_state.current_framebuffer].tex_color_buffer_handles[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, render_state.bloom.ping_pong_buffer[!horizontal]);

    set_int_uniform(bloom_shader.program, "bloom", renderer.render.bloom.active);

    set_float_uniform(bloom_shader.program, "exposure", renderer.render.bloom.exposure);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)nullptr);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    bind_vertex_array(0, render_state);
}

/*

    Framebuffer final_buffer = render_state.v2.framebuffers[renderer.render.final_framebuffer.handle - 1];
    
    glDisable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, render_state.v2.framebuffers[0].buffer_handle);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, final_buffer.buffer_handle);

    glBlitFramebuffer(0, 0, final_buffer.width, final_buffer.height, 0, 0, final_buffer.width, final_buffer.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
*/

static void render_post_processing_passes(RenderState &render_state, Renderer &renderer)
{
    glDisable(GL_DEPTH_TEST);

    bind_vertex_array(render_state.framebuffer_quad_vao, render_state);

    Framebuffer final_buffer = render_state.v2.framebuffers[renderer.render.final_framebuffer.handle - 1];

    for (i32 pass_index = 0; pass_index < renderer.render.post_processing_pass_count; pass_index++)
    {
        rendering::RenderPass &pass = renderer.render.post_processing_passes[pass_index];
        Framebuffer pass_buffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];

        // if(pass_index == renderer.render.post_processing_pass_count - 1)
        // {
        //     glBindFramebuffer(GL_FRAMEBUFFER, final_buffer.buffer_handle);
        // }
        // else
        // {
        glBindFramebuffer(GL_FRAMEBUFFER, pass_buffer.buffer_handle);
        // }

        ShaderGL shader = render_state.gl_shaders[pass.post_processing.shader_handle.handle];

        glUseProgram(shader.program);

        i32 texture_count = 0;

        for (i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            set_uniform({}, pass, pass.post_processing.uniform_values[i], shader, {}, &texture_count, render_state, renderer);
        }

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)nullptr);
    }

    bind_vertex_array(0, render_state);

    // glBindFramebuffer(GL_FRAMEBUFFER, final_buffer.buffer_handle);

    // bind_vertex_array(render_state.framebuffer_quad_vao, render_state);
    // ShaderGL hdr_shader = render_state.gl_shaders[renderer.render.hdr_shader.handle];

    // glUseProgram(hdr_shader.program);

    // set_int_uniform(hdr_shader.program, "scene", 0);
    // set_int_uniform(hdr_shader.program, "width", final_buffer.width);
    // set_int_uniform(hdr_shader.program, "height", final_buffer.height);

    // set_float_uniform(hdr_shader.program, "exposure", renderer.render.hdr.exposure);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, render_state.v2.framebuffers[0].tex_color_buffer_handles[0]);
    // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)nullptr);

    // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
}

static void render(RenderState &render_state, Renderer &renderer, r64 delta_time, b32 *save_config)
{
    if (render_state.paused)
    {
        renderer.command_count = 0;
        return;
    }

    check_window_mode_and_size(render_state, renderer, save_config);
    load_extra_shaders(render_state, renderer);
    reload_shaders(render_state, renderer);
    // @Speed: Do we have to clear this every frame?
    clear(&renderer.shader_arena);

    render_state.scale_x = 2.0f / render_state.framebuffer_width;
    render_state.scale_y = 2.0f / render_state.framebuffer_height;

    renderer.scale_x = render_state.scale_x;
    renderer.scale_y = render_state.scale_y;

    render_state.pixels_per_unit = renderer.pixels_per_unit;

    b32 should_render = renderer.window_width != 0;

    renderer.ui_projection_matrix = math::ortho(0.0f, (r32)renderer.framebuffer_width, 0.0f, (r32)renderer.framebuffer_height, -500.0f, 500.0f);

    //load_textures(render_state, renderer);
    register_buffers(render_state, renderer);
    register_new_buffers(render_state, renderer);
    //register_framebuffers(render_state, renderer);

    if (should_render)
    {
        // Render through all passes
        render_shadows(render_state, renderer, render_state.shadow_map_buffer);
        render_all_passes(render_state, renderer);
        render_commands(render_state, renderer);
        render_post_processing_passes(render_state, renderer);
        render_ui_pass(render_state, renderer);

        renderer.particles._tagged_removed_count = 0;

        render_state.bound_texture = 0;

        Framebuffer &final_framebuffer = render_state.v2.framebuffers[renderer.render.final_framebuffer.handle - 1];
        renderer.framebuffer_width = render_state.framebuffer_width;
        renderer.framebuffer_height = render_state.framebuffer_height;

        // Blit the final framebuffer to screen
        glBindFramebuffer(GL_READ_FRAMEBUFFER, final_framebuffer.buffer_handle);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glDrawBuffer(GL_BACK);

        i32 width = render_state.framebuffer_width;
        i32 height = render_state.framebuffer_height;

        glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // @Incomplete: Remove when we remember to always check this in all render calls
        render_state.current_state.shader_program = 0;

        /////// NOTHING WITH FRAMEBUFFERS ///////

        if (renderer.frame_lock != 0)
        {
            render_state.total_delta = 0.0;
            render_state.frame_delta += 1.0 / renderer.frame_lock;
        }
        else
        {
            render_state.total_delta = delta_time;
        }

        renderer.command_count = 0;

        render_state.frame_delta -= delta_time;
        render_state.total_delta += delta_time;

        unregister_buffers(render_state, renderer);
    }
}

static void mojave_workaround(RenderState &render_state)
{
    // MacOS Mojave workaround
    i32 x = 0;
    i32 y = 0;
    glfwGetWindowPos(render_state.window, &x, &y);
    glfwSetWindowPos(render_state.window, x + 1, y);
    glfwSetWindowPos(render_state.window, x - 1, y);
}
