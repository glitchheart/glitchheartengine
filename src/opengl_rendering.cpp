
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

static void delete_instance_buffer(Buffer *buffer, RenderState *render_state, Renderer *renderer)
{
    glDeleteBuffers(1, &buffer->vbo);
}

static void create_instance_buffer(Buffer *buffer, size_t buffer_size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer *renderer)
{
    *buffer = {};

    if (buffer->vbo == 0)
    {
        glGenBuffers(1, &buffer->vbo);
    }

    GLenum usage = get_usage(buffer_usage);

    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer_size, nullptr, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
            Texture* texture = nullptr;
            i32 handle = 0;
            
            if (framebuffer.tex_color_buffer_handles[i] != 0)
            {
                texture = renderer.render.textures[framebuffer.tex_color_buffer_handles[i] - 1];
                glDeleteTextures(1, &texture->handle);
                handle = framebuffer.tex_color_buffer_handles[i];
            }
            else
            {
                texture = renderer.render.textures[renderer.render.texture_count++];
                handle = renderer.render.texture_count;
            }
            assert(handle != 0);

            glGenTextures(1, &texture->handle);

            if (attachment.flags & rendering::ColorAttachmentFlags::MULTISAMPLED)
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->handle);

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

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texture->handle, NULL);
                attachment.ms_texture = {handle};
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, texture->handle);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
                }
                else
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->handle, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                if(attachment.flags & rendering::ColorAttachmentFlags::CLAMP_TO_EDGE)
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
                
                attachment.texture = {handle};
            }

            texture->width = width;
            texture->height = height;
            framebuffer.tex_color_buffer_handles[i] = handle;
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

// @Incomplete: Revisit this for pausing rendering
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
        renderer->framebuffer_width = width;
        renderer->framebuffer_height = height;

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        memcpy(render_state->viewport, viewport, sizeof(GLint) * 4);

        for (i32 i = 0; i < renderer->render.framebuffer_count; i++)
        {
            rendering::FramebufferInfo &info = renderer->render.framebuffers[i];
            Framebuffer &framebuffer = render_state->v2.framebuffers[i];
            reload_framebuffer(info, framebuffer, *render_state, *renderer, width, height);
        }

        rendering::FramebufferInfo &ui_framebuffer = renderer->render.framebuffers[renderer->render.ui.pass.framebuffer.handle - 1];
        renderer->render.ui.pass.camera.projection_matrix = math::ortho(0.0f, (r32)ui_framebuffer.width, 0.0f,
                                               (r32)ui_framebuffer.height, -500.0f, 500.0f);

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

    create_shadow_map(render_state->shadow_map_buffer, 2048, 2048);

    setup_quad(*render_state, render_state->perm_arena);
    
    render_state->total_delta = 0.0f;
    render_state->frame_delta = 0.0f;

    render_state->gl_buffers = push_array(render_state->perm_arena, global_max_custom_buffers, Buffer);
}

static void load_new_shaders(RenderState &render_state, Renderer &renderer)
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

static void create_framebuffer(rendering::FramebufferInfo &info, RenderState *render_state, Renderer *renderer)
{
    Framebuffer &framebuffer = render_state->v2.framebuffers[render_state->v2.framebuffer_count++];
    create_new_framebuffer(info, framebuffer, *render_state, *renderer);
}

static void load_texture(Texture* texture, TextureFiltering filtering, TextureWrap wrap, TextureFormat format, i32 width, i32 height, unsigned char* image_data, RenderState* render_state, Renderer* renderer)
{
    if (texture->handle == 0)
    {
        glGenTextures(1, &texture->handle);
    }

    glBindTexture(GL_TEXTURE_2D, texture->handle);

    if (wrap == REPEAT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    if (filtering == LINEAR)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (filtering == NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    //enable alpha for textures
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLenum gl_format = GL_RGBA;

    switch (format)
    {
    case TextureFormat::RGBA:
    {
        gl_format = GL_RGBA;
    }
    break;
    case TextureFormat::RGB:
    {
        gl_format = GL_RGB;
    }
    break;
    case TextureFormat::RED:
    {
        gl_format = GL_RED;
    }
    break;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    texture->width = width;
    texture->height = height;
    
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, gl_format,
                 GL_UNSIGNED_BYTE, (GLvoid *)image_data);
}

static math::Vec2i get_texture_size(Texture* texture)
{
    return math::Vec2i(texture->width, texture->height);
}

static void initialize_opengl(RenderState &render_state, Renderer &renderer, r32 contrast, r32 brightness, WindowMode window_mode, i32 screen_width, i32 screen_height, const char *title, MemoryArena *perm_arena, b32 *do_save_config)
{
    renderer.api_functions.render_state = &render_state;
    renderer.api_functions.get_texture_size = &get_texture_size;
    renderer.api_functions.load_texture = &load_texture;
    renderer.api_functions.create_framebuffer = &create_framebuffer;
    renderer.api_functions.create_instance_buffer = &create_instance_buffer;
    renderer.api_functions.delete_instance_buffer = &delete_instance_buffer;

    auto recreate_window = render_state.window != nullptr;

    if (!recreate_window)
    {
        if (!glfwInit())
        {
            log_error("Could not initialize glfw");
            exit(EXIT_FAILURE);
        }
    }


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

    glfwSwapInterval(1);

    glfwGetFramebufferSize(render_state.window, &render_state.framebuffer_width, &render_state.framebuffer_height);
    glViewport(0, 0, render_state.framebuffer_width, render_state.framebuffer_height);

#if !defined(__APPLE__)
    //Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)message_callback, 0);
#endif
    glDisable(GL_DITHER);

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
        if(uniform_value.texture.handle == 0)
            return;
        
        Texture *texture = renderer.render.textures[uniform_value.texture.handle - 1];
        set_int_uniform(program, location, *texture_count);
        set_texture_uniform(program, texture->handle, *texture_count);
        (*texture_count)++;
    }
    break;
    case rendering::ValueType::MS_TEXTURE:
    {
        if(uniform_value.texture.handle == 0)
            return;
        
        Texture *texture = renderer.render.textures[uniform_value.ms_texture.handle - 1];
        set_int_uniform(program, location, *texture_count);
        set_ms_texture_uniform(program, texture->handle, *texture_count);
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
        case rendering::UniformMappingType::POINT_LIGHT_COUNT:
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
        case rendering::UniformMappingType::CAMERA_UP:
        {
            set_vec3_uniform(gl_shader.program, location, math::up(camera.view_matrix));
        }
        break;
        case rendering::UniformMappingType::CAMERA_RIGHT:
        {
            set_vec3_uniform(gl_shader.program, location, math::right(camera.view_matrix));
        }
        break;
        case rendering::UniformMappingType::CAMERA_FORWARD:
        {
            set_vec3_uniform(gl_shader.program, location, math::forward(camera.view_matrix));
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
            buf_ptr = renderer.render.instancing.float_buffers[handle];
            size = sizeof(r32);
            count = renderer.render.instancing.float_buffer_counts[handle];

            Buffer *buffer = renderer.render.instancing.internal_float_buffers[handle];
            
            glEnableVertexAttribArray(array_num);
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT2:
        {
            num_values = 2;
            buf_ptr = renderer.render.instancing.float2_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float2_buffer_counts[handle];

            Buffer *buffer = renderer.render.instancing.internal_float2_buffers[handle];

            glEnableVertexAttribArray(array_num);
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT3:
        {
            num_values = 3;
            buf_ptr = renderer.render.instancing.float3_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float3_buffer_counts[handle];

            Buffer *buffer = renderer.render.instancing.internal_float3_buffers[handle];

            glEnableVertexAttribArray(array_num);
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT4:
        {
            num_values = 4;
            buf_ptr = renderer.render.instancing.float4_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer.render.instancing.float4_buffer_counts[handle];

            Buffer *buffer = renderer.render.instancing.internal_float4_buffers[handle];
            
            glEnableVertexAttribArray(array_num);
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
         
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::MAT4:
        {
            num_values = 16;
            buf_ptr = renderer.render.instancing.mat4_buffers[handle];
            size = sizeof(math::Mat4);
            count = renderer.render.instancing.mat4_buffer_counts[handle];

            Buffer *buffer = renderer.render.instancing.internal_mat4_buffers[handle];
            GLsizei vec4_size = sizeof(math::Vec4);

            glEnableVertexAttribArray(array_num);
            glEnableVertexAttribArray(array_num + 1);
            glEnableVertexAttribArray(array_num + 2);
            glEnableVertexAttribArray(array_num + 3);

            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
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

        if (command.clip)
        {
            glEnable(GL_SCISSOR_TEST);
            math::Rect clip_rect = command.clip_rect;
            glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
        }

        rendering::CharacterBufferHandle char_buf_handle = command.buffer;
        rendering::CharacterData *coords = pass.ui.coords[char_buf_handle.handle];

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

        if (command.clip)
        {
            glEnable(GL_SCISSOR_TEST);
            math::Rect clip_rect = command.clip_rect;
            glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
        }

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

    for (i32 i = 0; i < MAX_INSTANCE_BUFFERS; i++)
    {
        renderer.render.instancing.float_buffer_counts[i] = 0;
        renderer.render.instancing.float2_buffer_counts[i] = 0;
        renderer.render.instancing.float3_buffer_counts[i] = 0;
        renderer.render.instancing.float4_buffer_counts[i] = 0;
        renderer.render.instancing.mat4_buffer_counts[i] = 0;
    }
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
    check_window_mode_and_size(render_state, renderer, save_config);
    load_new_shaders(render_state, renderer);
    reload_shaders(render_state, renderer);

    // @Speed: Do we have to clear this every frame?
    clear(&renderer.shader_arena);

    b32 should_render = renderer.window_width != 0;

    register_new_buffers(render_state, renderer);
    //register_framebuffers(render_state, renderer);

    if (should_render)
    {
        // Render through all passes
        render_shadows(render_state, renderer, render_state.shadow_map_buffer);
        render_all_passes(render_state, renderer);
        render_post_processing_passes(render_state, renderer);
        render_ui_pass(render_state, renderer);

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
        
        render_state.frame_delta -= delta_time;
        render_state.total_delta += delta_time;
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
