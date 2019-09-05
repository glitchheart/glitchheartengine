 static void bind_vertex_array(GLuint vao, RenderState *render_state)
{
    if (vao != render_state->current_state.vao)
    {
        render_state->current_state.vao = vao;
        glBindVertexArray(vao);
    }
}

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %d - %s\n", error, description);
}

static void show_mouse_cursor(b32 show, RenderState *render_state)
{
    if (show)
    {
        glfwSetInputMode(render_state->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(render_state->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

static void debug_vao()
{
    int vab, eabb, eabbs, mva, is_on, vaabb;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vab);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &eabb);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &eabbs);

    debug(" VAO: %d\n", vab);
    debug(" IBO: %d, size: %d\n", eabb, eabbs);

    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &mva);
    for(i32 i = 0; i < mva; i++)
    {
        glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &is_on);
        if(is_on)
        {
            glGetVertexAttribiv(i, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &vaabb);
            debug(" attrib #%d: VBO=%d\n", i, vaabb);
        }
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

        printf("OpenGL error: %s type = 0x%x, severity = 0x%x, message = %s, source = %s, id = %ud, length %ud=\n",
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

[[noreturn]]
static void close_window(RenderState &render_state)
{
    glfwDestroyWindow(render_state.window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

static b32 should_close_window(RenderState &render_state)
{
    return glfwWindowShouldClose(render_state.window);
}

static GLint compile_shader(const char *shader_name, GLuint shader)
{
    glCompileShader(shader);
    GLint is_compiled = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled)
    {
        GLint max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        GLchar *error_log = nullptr;
        GLchar error_log_static[512];

        b32 allocated = false;
        
        if (max_length > 512)
        {
            error_log = (char *)malloc(max_length * sizeof(GLchar));
            allocated = true;
        }
        else
        {
            error_log = error_log_static;
        }

        glGetShaderInfoLog(shader, max_length, &max_length, error_log);

        log_error("Shader compilation error - %s\n", shader_name);
        log_error("%s", error_log);

        glDeleteShader(shader);

        if(allocated)
            free(error_log);
    }

    return is_compiled;
}

static GLint link_program(const char *program_name, GLuint program)
{
    GLint is_linked = 0;

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);

    if (!is_linked)
    {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        GLchar *error_log = nullptr;
        GLchar error_log_static[512];

        b32 allocated = false;
        
        if (max_length > 512)
        {
            error_log = (char *)malloc(max_length * sizeof(GLchar));
            allocated = true;
        }
        else
        {
            error_log = error_log_static;
        }

        glGetProgramInfoLog(program, max_length, &max_length, error_log);

        log_error("Program linking error - %s\n", program_name);
        log_error("%s", error_log);

        if(allocated)
            free(error_log);
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

                for (i32 k = 0; k < uniform.array_size; k++)
                {
                    for (i32 j = 0; j < structure.uniform_count; j++)
                    {
                        rendering::Uniform struct_uni = structure.uniforms[j];
                        char char_buf[256];
                        sprintf(char_buf, "%s[%d].%s", uniform.name, k, struct_uni.name);
                        GLint location = glGetUniformLocation(gl_shader.program, char_buf);
                        gl_shader.uniform_locations[gl_shader.location_count++] = location;
                    }
                }
            }
            else
            {
                for (i32 k = 0; k < uniform.array_size; k++)
                {
                    char char_buf[256];
                    sprintf(char_buf, "%s[%d]", uniform.name, k);

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

    for(i32 i = 0; i < shader.ubo_count; i++)
    {
        rendering::UniformBufferInfo ubo = shader.uniform_buffers[i];
        u32 block_index = glGetUniformBlockIndex(gl_shader.program, ubo.name);
        glUniformBlockBinding(gl_shader.program, block_index, (i32)ubo.mapping_type);
    }
}

static GLuint load_shader(Renderer *renderer, rendering::Shader &shader, ShaderGL &gl_shader)
{
    char *vert_shader = shader.vert_shader;
    gl_shader.vert_program = glCreateShader(GL_VERTEX_SHADER);

	if (!gl_shader.vert_program)
	{
		error_gl();
	}

    // @Incomplete: Think about common preamble stuff like #version 330 core and stuff
    glShaderSource(gl_shader.vert_program, 1, (GLchar **)&vert_shader, nullptr);

    if (!compile_shader(shader.path, gl_shader.vert_program))
    {
        log_error("Failed compilation of vertex shader: %s", shader.path);
        gl_shader.vert_program = 0;
        return GL_FALSE;
    }

    if(shader.geo_shader)
    {
        char *geo_shader = shader.geo_shader;
        gl_shader.geo_program = glCreateShader(GL_GEOMETRY_SHADER);

        glShaderSource(gl_shader.geo_program, 1, (GLchar **)&geo_shader, nullptr);

        if (!compile_shader(shader.path, gl_shader.geo_program))
        {
            log_error("Failed compilation of geometry shader: %s", shader.path);
            gl_shader.geo_program = 0;
            return GL_FALSE;
        }
    }
    
    char *frag_shader = shader.frag_shader;
    gl_shader.frag_program = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(gl_shader.frag_program, 1, (GLchar **)&frag_shader, nullptr);

    if (!compile_shader(shader.path, gl_shader.frag_program))
    {
        log_error("Failed compilation of fragment shader: %s", shader.path);
        gl_shader.frag_program = 0;
        return GL_FALSE;	
    }

    gl_shader.program = glCreateProgram();

    glAttachShader(gl_shader.program, gl_shader.vert_program);

    if(shader.geo_shader)
        glAttachShader(gl_shader.program, gl_shader.geo_program);
    
    glAttachShader(gl_shader.program, gl_shader.frag_program);

    if (!link_program(shader.path, gl_shader.program))
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

static void reload_shaders(RenderState &render_state, Renderer *renderer)
{
    if (renderer->render.shaders_to_reload_count > 0)
    {
        i32 shaders_to_reload[8];
        i32 count = renderer->render.shaders_to_reload_count;
        memcpy(shaders_to_reload, renderer->render.shaders_to_reload, sizeof(i32) * count);
        renderer->render.shaders_to_reload_count = 0;

        for (i32 i = 0; i < count; i++)
        {
            i32 index = shaders_to_reload[i];
            rendering::Shader &shader = renderer->render.shaders[index];
            ShaderGL &gl_shader = render_state.gl_shaders[index];

            delete_shader_program(gl_shader);

			MemoryArena arena = {};

            rendering::load_shader(renderer, shader, true, &arena);
            rendering::update_materials_with_shader(renderer, shader);
            load_shader(renderer, shader, gl_shader);
			
			clear(&arena);

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

    buffer->usage = get_usage(buffer_usage);
    buffer->size = buffer_size;
    
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)buffer->size, nullptr, buffer->usage);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static rendering::BufferUsage get_buffer_usage(Buffer *buffer)
{
	return buffer->usage == GL_DYNAMIC_DRAW ? rendering::BufferUsage::DYNAMIC : rendering::BufferUsage::STATIC;
}

static void delete_buffer(Buffer *buffer, RenderState *render_state, Renderer *renderer)
{
    glDeleteBuffers(1, &buffer->vbo);
}

static void create_uniform_buffer(UniformBuffer *buffer, rendering::BufferUsage buffer_usage, size_t size, Renderer* renderer)
{
    *buffer = {};
    
    if(buffer->ubo == 0)
    {
        glGenBuffers(1, &buffer->ubo);
    }

    buffer->size = math::multiple_of_number((i32)size, 4);
    buffer->memory = push_size(&renderer->ubo_arena, buffer->size, u8);
    buffer->usage = get_usage(buffer_usage);

    glBindBuffer(GL_UNIFORM_BUFFER, buffer->ubo);
    glBufferData(GL_UNIFORM_BUFFER, buffer->size, NULL, buffer->usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

static void update_uniform_buffer(UniformBuffer *buffer, rendering::UniformBufferUpdate update, Renderer *renderer)
{
    rendering::UniformBufferLayout layout = renderer->render.ubo_layouts[(i32)update.mapping_type];
        
    size_t size = rendering::generate_ubo_update_data(layout, update, buffer->memory, buffer->size, renderer);
        
    glBindBufferRange(GL_UNIFORM_BUFFER, (i32)update.mapping_type, buffer->ubo, 0, buffer->size);
    
    // glBindBuffer(GL_UNIFORM_BUFFER, buffer->ubo);

    // @Incomplete: Consider updating with offset as well
    
    glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, size, buffer->memory);
}

static void create_buffer(Buffer *buffer, rendering::RegisterBufferInfo info, RenderState *render_state, Renderer *renderer)
{
    *buffer = {};

    if(buffer->vbo == 0)
    {
        glGenBuffers(1, &buffer->vbo);
    }

    buffer->vertex_buffer_size = info.data.vertex_buffer_size;
    buffer->index_buffer_count = info.data.index_buffer_count;
    buffer->vertex_count = info.data.vertex_count;

    glGenVertexArrays(1, &buffer->vao);
    bind_vertex_array(buffer->vao, render_state);

    glGenBuffers(1, &buffer->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);

    buffer->usage = get_usage(info.usage);

    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)info.data.vertex_buffer_size, info.data.vertex_buffer, buffer->usage);

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
            type_size = sizeof(GLfloat);
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
        case rendering::ValueType::MS_TEXTURE:
        case rendering::ValueType::STRUCTURE:
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
        glGenBuffers(1, &buffer->ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, info.data.index_buffer_size, info.data.index_buffer, buffer->usage);
    }

    bind_vertex_array(0, render_state);
}

static void create_framebuffer_color_attachment(RenderState &render_state, Renderer *renderer, rendering::FramebufferInfo &info, Framebuffer &framebuffer, i32 width, i32 height)
{
    framebuffer.tex_color_buffer_count = info.color_attachments.count;

    for(i32 i = 0; i < info.color_attachments.count; i++)
    {
        rendering::ColorAttachment &attachment = info.color_attachments.attachments[i];
        
        if(attachment.type == rendering::AttachmentType::RENDER_BUFFER)
        {
            if(framebuffer.tex_color_buffer_handles[i] != 0)
            {
                glDeleteRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
            }

            if(attachment.flags & rendering::ColorAttachmentFlags::MULTISAMPLED)
            {
                glGenRenderbuffers(1, &framebuffer.tex_color_buffer_handles[i]);
                glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.tex_color_buffer_handles[i]);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, GL_RGBA16F, width, height);
                }
                else
                {
                    glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, GL_RGB, width, height);
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
                    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB, width, height);
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
                texture = renderer->render.textures[framebuffer.tex_color_buffer_handles[i] - 1];
                glDeleteTextures(1, &texture->handle);
                handle = framebuffer.tex_color_buffer_handles[i];
            }
            else
            {
                texture = renderer->render.textures[renderer->render.texture_count++];
                handle = renderer->render.texture_count;
            }
            
            assert(handle != 0);

            glGenTextures(1, &texture->handle);

            if(attachment.flags & rendering::ColorAttachmentFlags::MULTISAMPLED)
            {
                glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->handle);

                if (attachment.flags & rendering::ColorAttachmentFlags::HDR)
                {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, attachment.samples, GL_RGBA16F, width, height, GL_TRUE);
                    //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
                }
                else
                {
                    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, attachment.samples, GL_RGB, width, height, GL_TRUE);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D_MULTISAMPLE, texture->handle, NULL);
                attachment.ms_texture = { handle };
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
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
                }

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texture->handle, NULL);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                
                if(attachment.flags & rendering::ColorAttachmentFlags::CLAMP_TO_EDGE)   
                {
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                }
                
                attachment.texture = { handle };
            }

            texture->width = width;
            texture->height = height;
            framebuffer.tex_color_buffer_handles[i] = handle;
        }
    }
}

static void _create_framebuffer_depth_texture_attachment(rendering::DepthAttachment &attachment, i32 index, Framebuffer &framebuffer, i32 width, i32 height, Renderer *renderer)
{
	Texture* texture = nullptr;
	i32 handle = 0;

	if (framebuffer.depth_buffer_handles[index] != 0)
	{
		texture = renderer->render.textures[framebuffer.depth_buffer_handles[index] - 1];
		glDeleteTextures(1, &texture->handle);
		handle = framebuffer.depth_buffer_handles[index];
	}
	else
	{
		texture = renderer->render.textures[renderer->render.texture_count++];
		handle = renderer->render.texture_count;
	}

    if(attachment.flags & rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED)
    {
        glGenTextures(1, &texture->handle);
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture->handle);
        
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, attachment.samples, GL_DEPTH24_STENCIL8, width, height, GL_TRUE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        
        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, texture->handle, NULL);

        texture->width = width;
        texture->height = height;
        framebuffer.depth_buffer_handles[index] = handle;
        attachment.texture = {handle};
    }
    else
    {
        glGenTextures(1, &texture->handle);
        glBindTexture(GL_TEXTURE_2D, texture->handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);

        // Prevent shadows outside of the shadow map
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

        float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->handle, 0);

        texture->width = width;
        texture->height = height;
        framebuffer.depth_buffer_handles[index] = handle;
        attachment.texture = {handle};
    }
}

static void _create_framebuffer_depth_render_buffer_attachment(rendering::DepthAttachment &attachment, i32 index, Framebuffer &framebuffer, i32 width, i32 height)
{
    if (framebuffer.depth_buffer_handles[index] != 0)
    {
        glDeleteRenderbuffers(1, &framebuffer.depth_buffer_handles[index]);
    }
    
    glGenRenderbuffers(1, &framebuffer.depth_buffer_handles[index]);
    glBindRenderbuffer(GL_RENDERBUFFER, framebuffer.depth_buffer_handles[index]);

    if(attachment.flags & rendering::DepthAttachmentFlags::DEPTH_MULTISAMPLED)
    {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, attachment.samples, GL_DEPTH24_STENCIL8, width, height);
    }
    else
    {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    }

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebuffer.depth_buffer_handles[index]);
}

static void create_framebuffer_depth_attachment(rendering::FramebufferInfo &info, Framebuffer &framebuffer, i32 width, i32 height, Renderer *renderer)
{
    framebuffer.depth_buffer_count = info.depth_attachments.count;
    
    for (i32 i = 0; i < info.depth_attachments.count; i++)
    {
        rendering::DepthAttachment &attachment = info.depth_attachments.attachments[i];

        if(attachment.type == rendering::AttachmentType::TEXTURE)
        {
            _create_framebuffer_depth_texture_attachment(attachment, i, framebuffer, width, height, renderer);
        }
        else
        {
            _create_framebuffer_depth_render_buffer_attachment(attachment, i, framebuffer, width, height);
        }
    }
}

static void create_new_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer *renderer)
{
	framebuffer.width = info.width;
	framebuffer.height = info.height;

	if (framebuffer.buffer_handle != 0)
	{
		glDeleteFramebuffers(1, &framebuffer.buffer_handle);
	}

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

    if (info.depth_attachments.enabled)
    {
        create_framebuffer_depth_attachment(info, framebuffer, info.width, info.height, renderer);
    }

    glDrawBuffers(color_buffer_count, attachments);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    free(attachments);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        log_error("Error: Framebuffer incomplete");
        if(status == GL_FRAMEBUFFER_UNDEFINED)
        {
            log_error("FRAMEBUFFER UNDEFINED");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT)
        {
            log_error("INCOMPLETE ATTACHMENT");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT)
        {
            log_error("MISSING ATTACHMENT");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER)
        {
            log_error("INCOMPLETE DRAW BUFFER");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER)
        {
            log_error("INCOMPLETE READ BUFFER");
        }
        else if(status == GL_FRAMEBUFFER_UNSUPPORTED)
        {
            log_error("UNSUPPORTED");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        {
            log_error("INCOMPLETE MULTISAMPLE");
        }
        else if(status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS)
        {
            log_error("INCOMPLETE LAYER TARGETS");
        }
    }
}

static void _reload_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer *renderer, i32 width, i32 height)
{
    switch (info.size_ratio)
    {
    case 0:
    return;
    default:
    info.width = width * info.size_ratio;
    info.height = height * info.size_ratio;
    break;
    }
    // @Note(Daniel): Right now this call is identical to create_new_framebuffer, but we might want to do more here later
    create_new_framebuffer(info, framebuffer, render_state, renderer);
}


static void reload_framebuffer(rendering::FramebufferHandle handle, RenderState *render_state, Renderer *renderer, i32 width, i32 height)
{
    rendering::FramebufferInfo& info = renderer->render.framebuffers[handle.handle - 1];
    Framebuffer &framebuffer = render_state->v2.framebuffers[handle.handle - 1];

    if(width <= 0 || height <= 0)
    {
        return;
    }

    info.width = width;
    info.height = height;
    
    create_new_framebuffer(info, framebuffer,  *render_state, renderer);
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

static void _reload_framebuffer(rendering::FramebufferInfo &info, Framebuffer &framebuffer, RenderState &render_state, Renderer *renderer, i32 width, i32 height);

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
            _reload_framebuffer(info, framebuffer, *render_state, renderer, width, height);
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
    bind_vertex_array(render_state.framebuffer_quad_vao, &render_state);
    glGenBuffers(1, &render_state.framebuffer_quad_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, render_state.framebuffer_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)render_state.framebuffer_quad_vertices_size, render_state.framebuffer_quad_vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.quad_index_buffer);

    vertex_attrib_pointer(0, 2, GL_FLOAT, 4 * sizeof(float), nullptr);
    vertex_attrib_pointer(1, 2, GL_FLOAT, 4 * sizeof(float), (void *)(2 * sizeof(float)));

    bind_vertex_array(0, &render_state);
}

// @Cleanup: Move glfw stuff to separate header!
static void set_window_cursor(RenderState* render_state, CursorType cursor)
{
    glfwSetCursor(render_state->window, render_state->cursors[cursor]);
}

// @Cleanup: Move glfw stuff to separate header!
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

    setup_quad(*render_state, render_state->perm_arena);
    
    render_state->total_delta = 0.0f;
    render_state->frame_delta = 0.0f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
}

static void load_shader(RenderState *render_state, Renderer *renderer, rendering::Shader &shader)
{
    ShaderGL &gl_shader = render_state->gl_shaders[render_state->gl_shader_count];
    gl_shader.handle = render_state->gl_shader_count;
    
    gl_shader.location_count = 0;

	if(shader.loaded)
		load_shader(renderer, shader, gl_shader);

	render_state->gl_shader_count = renderer->render.shader_count;
}

namespace ui_rendering
{
    // DEAR IMGUI
    bool imgui_impl_opengl3_create_fonts_texture(InternalImGuiState *state)
    {
        // Build texture atlas
        ImGuiIO& io = ImGui::GetIO();
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

        // Upload texture to graphics system
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &state->font_texture);
        glBindTexture(GL_TEXTURE_2D, state->font_texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #ifdef GL_UNPACK_ROW_LENGTH
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    #endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store our identifier
        io.Fonts->TexID = (ImTextureID)(intptr_t)state->font_texture;

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);

        return true;
    }

    static bool check_shader(GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to compile %s!\n", desc);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    static bool check_program(InternalImGuiState *state, GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if ((GLboolean)status == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to link %s! (with GLSL '%s')\n", desc, state->glsl_version_string);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, NULL, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return (GLboolean)status == GL_TRUE;
    }

    bool imgui_impl_opengl3_create_device_objects(InternalImGuiState *state)
    {
        // Backup GL state
        GLint last_texture, last_array_buffer;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    #ifndef IMGUI_IMPL_OPENGL_ES2
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
    #endif

        // Parse GLSL version string
        int glsl_version = 130;
        sscanf(state->glsl_version_string, "#version %d", &glsl_version);

        const GLchar* vertex_shader =
            "#version 330 core\n"
            "layout (location = 0) in vec2 Position;\n"
            "layout (location = 1) in vec2 UV;\n"
            "layout (location = 2) in vec4 Color;\n"
            "uniform mat4 ProjMtx;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";


        const GLchar* fragment_shader =
            "#version 330 core\n"
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "uniform sampler2D Texture;\n"
            "layout (location = 0) out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        // Create shaders
        const GLchar* vertex_shader_with_version[2] = { state->glsl_version_string, vertex_shader };
        state->vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(state->vert_handle, 2, vertex_shader_with_version, NULL);
        glCompileShader(state->vert_handle);
        check_shader(state->vert_handle, "vertex shader");

        const GLchar* fragment_shader_with_version[2] = { state->glsl_version_string, fragment_shader };
        state->frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(state->frag_handle, 2, fragment_shader_with_version, NULL);
        glCompileShader(state->frag_handle);
        check_shader(state->frag_handle, "fragment shader");

        state->shader_handle = glCreateProgram();
        glAttachShader(state->shader_handle, state->vert_handle);
        glAttachShader(state->shader_handle, state->frag_handle);
        glLinkProgram(state->shader_handle);
        check_program(state, state->shader_handle, "shader program");

        state->attrib_location_tex = glGetUniformLocation(state->shader_handle, "Texture");
        state->attrib_location_proj_mtx = glGetUniformLocation(state->shader_handle, "ProjMtx");
        state->attrib_location_vtx_pos = glGetAttribLocation(state->shader_handle, "Position");
        state->attrib_location_vtx_uv = glGetAttribLocation(state->shader_handle, "UV");
        state->attrib_location_vtx_color = glGetAttribLocation(state->shader_handle, "Color");

        // Create buffers
        glGenBuffers(1, &state->vbo_handle);
        glGenBuffers(1, &state->elements_handle);

        imgui_impl_opengl3_create_fonts_texture(state);

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    #ifndef IMGUI_IMPL_OPENGL_ES2
        glBindVertexArray(last_vertex_array);
    #endif

        return true;
    }

    static void initialize_imgui(RenderState &render_state)
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        imgui_impl_glfw_init_for_opengl(render_state.window, true);
        imgui_impl_opengl3_create_device_objects(&render_state.imgui_state);
    }

    static void start_imgui_frame(InternalImGuiState *state)
    {
		if (!state->font_texture)
			imgui_impl_opengl3_create_device_objects(state);

		imgui_impl_glfw_new_frame();
		ImGui::NewFrame();
    }

    static void imgui_impl_opengl3_setup_render_state(InternalImGuiState *imgui_state, ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
    {
        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
    #ifdef GL_POLYGON_MODE
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    #endif

        // Setup viewport, orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
        const float ortho_projection[4][4] =
        {
            { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
        };

        glUseProgram(imgui_state->shader_handle);
        glUniform1i(imgui_state->attrib_location_tex, 0);
        glUniformMatrix4fv(imgui_state->attrib_location_proj_mtx, 1, GL_FALSE, &ortho_projection[0][0]);
    #ifdef GL_SAMPLER_BINDING
        glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
    #endif

        (void)vertex_array_object;
    #ifndef IMGUI_IMPL_OPENGL_ES2
        glBindVertexArray(vertex_array_object);
    #endif

        // Bind vertex/index buffers and setup attributes for ImDrawVert
        glBindBuffer(GL_ARRAY_BUFFER, imgui_state->vbo_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imgui_state->elements_handle);
        glEnableVertexAttribArray(imgui_state->attrib_location_vtx_pos);
        glEnableVertexAttribArray(imgui_state->attrib_location_vtx_uv);
        glEnableVertexAttribArray(imgui_state->attrib_location_vtx_color);
        glVertexAttribPointer(imgui_state->attrib_location_vtx_pos,   2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
        glVertexAttribPointer(imgui_state->attrib_location_vtx_uv,    2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
        glVertexAttribPointer(imgui_state->attrib_location_vtx_color, 4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));
    }

    static void draw_imgui_frame(RenderState &render_state, InternalImGuiState *imgui_state)
    {
        static math::Rgba clear_color(0.0f, 0.0f, 0.0f, 1.0f);

        ImGui::Render();
        
        int display_w, display_h;
        glfwGetFramebufferSize(render_state.window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        //glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        //glClear(GL_COLOR_BUFFER_BIT);
        ImDrawData *draw_data = ImGui::GetDrawData();

        int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Backup GL state
        GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
        glActiveTexture(GL_TEXTURE0);
        GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    #ifdef GL_SAMPLER_BINDING
        GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
    #endif
        GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    #ifndef IMGUI_IMPL_OPENGL_ES2
        GLint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
    #endif
    #ifdef GL_POLYGON_MODE
        GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
    #endif
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
        bool clip_origin_lower_left = true;
        
        #if defined(GL_CLIP_ORIGIN) && !defined(__APPLE__)
        GLenum last_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
        if (last_clip_origin == GL_UPPER_LEFT)
            clip_origin_lower_left = false;
        #endif

        // Setup desired GL state
        // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
        // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
        GLuint vertex_array_object = 0;

        glGenVertexArrays(1, &vertex_array_object);
        imgui_impl_opengl3_setup_render_state(&render_state.imgui_state, draw_data, fb_width, fb_height, vertex_array_object);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = draw_data->CmdLists[n];

            // Upload vertex/index buffers
            glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

            for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    // if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    //     ImGui_ImplOpenGL3_SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                    // else
                        pcmd->UserCallback(cmd_list, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec4 clip_rect;
                    clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                    clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                    clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                    clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                    if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                    {
                        // Apply scissor/clipping rectangle
                        if (clip_origin_lower_left)
                            glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                        else
                            glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)

                        // Bind texture, Draw
                        glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
    #if IMGUI_IMPL_OPENGL_HAS_DRAW_WITH_BASE_VERTEX
                        glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
    #else
                        glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
    #endif
                    }
                }
            }
        }

        // Destroy the temporary VAO
    glDeleteVertexArrays(1, &vertex_array_object);

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, last_sampler);
#endif
    glActiveTexture(last_active_texture);
    glBindVertexArray(last_vertex_array_object);

    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }
}


static const GLFWvidmode *create_open_gl_window(RenderState &render_state, WindowMode window_mode, const char *title, i32 width, i32 height)
{
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    i32 screen_width = width;
    i32 screen_height = height;

    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
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

    debug_log("refresh rate %d", mode->refreshRate);

    if (window_mode == FM_WINDOWED)
    {
        monitor = nullptr;
    }

    auto old_window = render_state.window;

    render_state.window = glfwCreateWindow(screen_width, screen_height, render_state.window_title, monitor,
                                           nullptr);

    // glfwSetWindowAttrib(render_state.window, GLFW_RESIZABLE, GLFW_FALSE);
    // glfwSetWindowAttrib(render_state.window, GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_FALSE);

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
    create_new_framebuffer(info, framebuffer, *render_state, renderer);
}

static void load_texture(Texture* texture, TextureFiltering filtering, TextureWrap wrap, TextureFormat format, i32 width, i32 height, unsigned char* image_data, RenderState* render_state, Renderer* renderer, TextureUsage usage = TextureUsage::STATIC)
{
    b32 existing_tex = true;
    
    if (texture->handle == 0)
    {
        existing_tex = false;
        glGenTextures(1, &texture->handle);
    }

    GLenum gl_format = GL_RGBA8;
    GLenum img_format = GL_RGBA;

    switch (format)
    {
    case TextureFormat::RGBA:
    {
        gl_format = GL_RGBA8;
        img_format = GL_RGBA;
    }
    break;
    case TextureFormat::RGB:
    {
        gl_format = GL_RGB8;
        img_format = GL_RGB;
    }
    break;
    case TextureFormat::RED:
    {
        gl_format = GL_R8;
        img_format = GL_RED;
    }
    break;
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if (filtering == NEAREST)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    
    texture->width = width;
    texture->height = height;

    i32 mip = 4;

#if defined(__APPLE__)
    glEnable(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, gl_format, width, height, 0, img_format, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
 #else

    if(usage == TextureUsage::STATIC)
    {
        if(!existing_tex)
        {
            glTexStorage2D(GL_TEXTURE_2D, mip, gl_format, width, height);
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture->width, texture->height, img_format, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexImage2D(GL_TEXTURE_2D, 0, gl_format, texture->width, texture->height, 0, img_format, GL_UNSIGNED_BYTE, (GLvoid*)image_data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mip);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -1);
}

static math::Vec2i get_texture_size(Texture* texture)
{
    return math::Vec2i(texture->width, texture->height);
}

static b32 get_mouse_lock(RenderState *render_state)
{
    return render_state->mouse_locked;
}

static void set_mouse_lock(b32 locked, RenderState *render_state)
{
    if(locked)
    {
        glfwSetInputMode(render_state->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    else
        glfwSetInputMode(render_state->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    render_state->mouse_locked = locked;
}

static Buffer &get_internal_buffer(Renderer *renderer, RenderState &render_state, rendering::BufferHandle buffer)
{
    i32 handle = renderer->render._internal_buffer_handles[buffer.handle - 1];
    Buffer *gl_buffer = renderer->render.buffers[handle];
    return *gl_buffer;
}

static void update_vertex_buffer(Buffer *buffer, r32 *data, size_t count, size_t size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer *renderer)
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
    GLenum usage = get_usage(buffer_usage);

    bind_vertex_array(buffer->vao, render_state);
    buffer->vertex_count = (i32)count;
    buffer->vertex_buffer_size = (i32)size;

    glBufferData(GL_ARRAY_BUFFER, size, data, usage);

    bind_vertex_array(0, render_state);
}

static void update_index_buffer(Buffer *buffer, u16* data, size_t count, size_t size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer* renderer)
{
    if(buffer->ibo == 0)
    {
        glGenBuffers(1, &buffer->ibo);
    }
    
    GLenum usage = get_usage(buffer_usage);
    bind_vertex_array(buffer->vao, render_state);
        
    buffer->index_buffer_count = (i32)count;
    buffer->index_buffer_size = (i32)size;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
}

static void update_buffer(Buffer *buffer, rendering::BufferType buffer_type, void* data, size_t count, size_t size, rendering::BufferUsage buffer_usage, RenderState *render_state, Renderer* renderer)
{
    switch(buffer_type)
    {
    case rendering::BufferType::VERTEX:
    {
        update_vertex_buffer(buffer, (r32*)data, count, size, buffer_usage, render_state, renderer);
    }
    break;
    case rendering::BufferType::INDEX:
    {
        update_index_buffer(buffer, (u16*)data, count, size, buffer_usage, render_state, renderer);        
    }
    break;    
    }
}

static void set_v_sync(RenderState *render_state, b32 value)
{
    glfwSwapInterval(value ? 1 : 0);
    render_state->vsync_active = value;
}

static b32 get_v_sync(RenderState *render_state)
{
    return render_state->vsync_active;
}

static void set_window_mode(RenderState* render_state, Renderer* renderer, Resolution new_resolution, WindowMode new_window_mode)
{
    b32 save = false;
    if(new_window_mode != renderer->window_mode)
    {
        save = true;
        renderer->window_mode = new_window_mode;
        
        const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        if(new_window_mode == FM_BORDERLESS)
        {
            glfwSetWindowMonitor(render_state->window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            render_state->window_width = mode->width;
            render_state->window_height = mode->height;
            renderer->window_width = mode->width;
            renderer->window_height = mode->height;

            for (i32 res_index = 0; res_index < renderer->available_resolutions_count; res_index++)
            {
                auto res = renderer->available_resolutions[res_index];
                if (res.width == renderer->window_width && res.height == renderer->window_height)
                {
                    renderer->current_resolution_index = res_index;
                    break;
                }
            }
        }
        else
        {
            render_state->window_width = new_resolution.width;
            render_state->window_height = new_resolution.height;
            renderer->window_width = new_resolution.width;
            renderer->window_height = new_resolution.height;

            glfwSetWindowMonitor(render_state->window, nullptr, mode->width / 2 - new_resolution.width / 2, mode->height / 2 - new_resolution.height / 2, new_resolution.width, new_resolution.height, 0);
            glfwSetWindowSize(render_state->window, new_resolution.width, new_resolution.height);

            for (i32 res_index = 0; res_index < renderer->available_resolutions_count; res_index++)
            {
                auto res = renderer->available_resolutions[res_index];
                if (res.width == renderer->window_width && res.height == renderer->window_height)
                {
                    renderer->current_resolution_index = res_index;
                    break;
                }
            }
        }
    }
    else if(new_resolution.width != render_state->window_width || new_resolution.height != render_state->window_height)
    {
        render_state->window_width = new_resolution.width;
        render_state->window_height = new_resolution.height;
        renderer->window_width = new_resolution.width;
        renderer->window_height = new_resolution.height;
        
        glfwSetWindowSize(render_state->window, new_resolution.width, new_resolution.height);
        save = true;
    }

    if(save)
    {
        save_config("../.config", renderer, core.sound_system);
    }
}

static void set_window_title(const char *title, Renderer *renderer)
{
    glfwSetWindowTitle(renderer->api_functions.render_state->window, title);
}

static void initialize_opengl(RenderState &render_state, Renderer *renderer, r32 contrast, r32 brightness, WindowMode window_mode, i32 screen_width, i32 screen_height, const char *title, MemoryArena *perm_arena, b32 *do_save_config, b32 vsync_active)
{
    renderer->api_functions.render_state = &render_state;
    renderer->api_functions.get_texture_size = &get_texture_size;
    renderer->api_functions.load_texture = &load_texture;
    renderer->api_functions.create_framebuffer = &create_framebuffer;
    renderer->api_functions.reload_framebuffer = &reload_framebuffer;
    renderer->api_functions.create_instance_buffer = &create_instance_buffer;
    renderer->api_functions.create_uniform_buffer = &create_uniform_buffer;
    renderer->api_functions.update_uniform_buffer = &update_uniform_buffer;
    renderer->api_functions.get_buffer_usage = &get_buffer_usage;
    renderer->api_functions.delete_instance_buffer = &delete_instance_buffer;
    renderer->api_functions.create_buffer = &create_buffer;
    renderer->api_functions.delete_buffer = &delete_buffer;
    renderer->api_functions.update_buffer = &update_buffer;
    renderer->api_functions.set_mouse_lock = &set_mouse_lock;
    renderer->api_functions.get_mouse_lock = &get_mouse_lock;
    renderer->api_functions.show_mouse_cursor = &show_mouse_cursor;
    renderer->api_functions.set_window_cursor = &set_window_cursor;
    renderer->api_functions.set_window_mode = &set_window_mode;
    renderer->api_functions.set_v_sync = &set_v_sync;
    renderer->api_functions.get_v_sync = &get_v_sync;
    renderer->api_functions.load_shader = &load_shader;
    renderer->api_functions.set_window_title = &set_window_title;

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

    //glfwSetErrorCallback(error_callback);
    
    //@Incomplete: Figure something out here. Ask for compatible version etc
#ifdef _WIN32
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif __linux
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
#elif __APPLE__
    // @Note: Apple only __really__ supports OpenGL Core 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
    //glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GL_TRUE);
#endif

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_SAMPLES, 8);
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
    renderer->window_mode = window_mode;

    if (mode && renderer->window_mode == FM_BORDERLESS)
    {
        renderer->window_width = mode->width;
        renderer->window_height = mode->height;
    }
    else
    {
        renderer->window_width = screen_width;
        renderer->window_height = screen_height;
    }

    if (!render_state.window)
    {
        // @Note: If no window has been created, try and see if 3.3 works
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
                
        create_open_gl_window(render_state, window_mode, title, screen_width, screen_height);
        renderer->window_mode = window_mode;

        if (!render_state.window)
        {
            log_error("Could not create window");
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
    }

    //glfwSetInputMode(render_state.window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


    glfwSetFramebufferSizeCallback(render_state.window, frame_buffer_size_callback);
    glfwSetWindowIconifyCallback(render_state.window, window_iconify_callback);

    glfwMakeContextCurrent(render_state.window);
    glfwSwapInterval(0);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    render_state.vsync_active = vsync_active;
    
    glfwSetWindowUserPointer(render_state.window, &rendering_state);
    glfwSetKeyCallback(render_state.window, key_callback);
    glfwSetCharCallback(render_state.window, character_callback);
    glfwSetCursorPosCallback(render_state.window, cursor_position_callback);
    glfwSetMouseButtonCallback(render_state.window, mouse_button_callback);
    glfwSetScrollCallback(render_state.window, scroll_callback);

    ui_rendering::initialize_imgui(render_state);

    glfwGetFramebufferSize(render_state.window, &render_state.framebuffer_width, &render_state.framebuffer_height);
    glViewport(0, 0, render_state.framebuffer_width, render_state.framebuffer_height);

#if !defined(__APPLE__)
    //Enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback((GLDEBUGPROC)message_callback, 0);
#endif
    glDisable(GL_DITHER);
    
    glEnable( GL_MULTISAMPLE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    log("%s", glGetString(GL_VERSION));
    log("Shading language supported: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    log("Glad Version: %d.%d", GLVersion.major, GLVersion.minor);

    rendering_state.render_state = &render_state;
    rendering_state.renderer = renderer;

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    memcpy(render_state.viewport, viewport, sizeof(GLint) * 4);
    memcpy(renderer->viewport, render_state.viewport, sizeof(i32) * 4);

    controller_present();

    renderer->should_close = false;

    render_state.current_state.vao = 0;

    render_setup(&render_state, perm_arena);
    
    render_state.paused = false;

    create_standard_cursors(render_state);

    renderer->framebuffer_width = render_state.framebuffer_width;
    renderer->framebuffer_height = render_state.framebuffer_height;

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();

    i32 video_mode_count;
    auto video_modes = glfwGetVideoModes(monitor, &video_mode_count);

    if (!renderer->available_resolutions)
    {
        renderer->available_resolutions = push_array(render_state.perm_arena, video_mode_count, Resolution);

        for (i32 video_mode_index = 0; video_mode_index < video_mode_count; video_mode_index++)
        {
            auto vm = video_modes[video_mode_index];

            auto skip = false;

            for (i32 i = 0; i < renderer->available_resolutions_count; i++)
            {
                if (renderer->available_resolutions[i].width == vm.width && renderer->available_resolutions[i].height == vm.height)
                {
                    skip = true;
                }
            }

            if (!skip)
            {
                renderer->available_resolutions[renderer->available_resolutions_count++] = {vm.width, vm.height};

                if ((renderer->window_width == vm.width && renderer->window_height == vm.height) || (renderer->window_width == vm.width && ABS(renderer->window_height - vm.height) < 50)
                || (renderer->window_height == vm.height && ABS(renderer->window_width - vm.width) < 50))
                {
                    renderer->current_resolution_index = renderer->available_resolutions_count - 1;
                    //auto res = renderer->available_resolutions[renderer->current_resolution_index];
                    renderer->resolution = {vm.width, vm.height};
                }
            }
        }

        // @Incomplete: Replace with own sort? Don't like using qsort here :(
        qsort(renderer->available_resolutions, (size_t)renderer->available_resolutions_count, sizeof(Resolution), [](const void *a, const void *b) 
        {
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
        for (i32 res_index = 0; res_index < renderer->available_resolutions_count; res_index++)
        {
            auto resolution = renderer->available_resolutions[res_index];
            if (renderer->window_width == resolution.width && renderer->window_height == resolution.height)
            {
                renderer->current_resolution_index = res_index;
                break;
            }
        }
    }
}

static void initialize_opengl(RenderState &render_state, Renderer *renderer, project::ProjectState *project_state, ConfigData *config_data, MemoryArena *perm_arena, b32 *do_save_config)
{
    initialize_opengl(render_state, renderer, config_data->contrast, config_data->brightness, config_data->window_mode, config_data->screen_width, config_data->screen_height, project_state->project_settings.name, perm_arena, do_save_config, config_data->vsync);
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

static void register_framebuffers(RenderState &render_state, Renderer *renderer)
{
    for (i32 index = render_state.v2.framebuffer_count; index < renderer->render.framebuffer_count; index++)
    {
        rendering::FramebufferInfo &info = renderer->render.framebuffers[index];
        Framebuffer &framebuffer = render_state.v2.framebuffers[index];
        create_new_framebuffer(info, framebuffer, render_state, renderer);

        for (i32 i = 0; i < info.pending_textures.count; i++)
        {
            rendering::RenderPass &pass = renderer->render.post_processing_passes[info.pending_textures.pass_handles[i].handle - 1];
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
    render_state.v2.framebuffer_count = renderer->render.framebuffer_count;
}

static void set_uniform(RenderState &render_state, Renderer *renderer, GLuint program, rendering::UniformValue &uniform_value, GLuint location, i32 *texture_count = nullptr)
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
        
        Texture *texture = renderer->render.textures[uniform_value.texture.handle - 1];
        set_int_uniform(program, location, *texture_count);
        set_texture_uniform(program, texture->handle, *texture_count);
        (*texture_count)++;
    }
    break;
    case rendering::ValueType::MS_TEXTURE:
    {
        if(uniform_value.texture.handle == 0)
            return;
        
        Texture *texture = renderer->render.textures[uniform_value.ms_texture.handle - 1];
        set_int_uniform(program, location, *texture_count);
        set_ms_texture_uniform(program, texture->handle, *texture_count);
        (*texture_count)++;
    }
    break;
    case rendering::ValueType::INVALID:
    case rendering::ValueType::STRUCTURE:
        assert(false);
        break;
    }
}

static void set_uniform(rendering::Transform transform, const rendering::RenderPass &render_pass, rendering::UniformValue &uniform_value, ShaderGL &gl_shader, const Camera &camera, i32 *texture_count, RenderState &render_state, Renderer *renderer)
{
    rendering::Uniform &uniform = uniform_value.uniform;

    GLuint location = gl_shader.uniform_locations[uniform_value.uniform.location_index];
    switch (uniform.mapping_type)
    {
    case rendering::UniformMappingType::CUSTOM:
    {
        rendering::CustomUniformMapping mapping = renderer->render.custom_mappings[uniform.custom_mapping.handle];
        switch(mapping.type)
        {
        case rendering::ValueType::FLOAT:
        {
            set_float_uniform(gl_shader.program, location, mapping.float_val);
        }
        break;
        case rendering::ValueType::FLOAT2:
        {
            set_vec2_uniform(gl_shader.program, location, mapping.float2_val);
        }
        break;
        case rendering::ValueType::FLOAT3:
        {
            set_vec3_uniform(gl_shader.program, location, mapping.float3_val);

        }
        break;
        case rendering::ValueType::FLOAT4:
        {
            set_vec4_uniform(gl_shader.program, location, mapping.float4_val);
        }
        break;
        case rendering::ValueType::INTEGER:
        {
            set_int_uniform(gl_shader.program, location, mapping.integer_val);
        }
        break;
        case rendering::ValueType::BOOL:
        {
            set_bool_uniform(gl_shader.program, location, mapping.boolean_val);
        }
        break;
        case rendering::ValueType::MAT4:
        {
            set_mat4_uniform(gl_shader.program, location, mapping.mat4_val);
        }
        break;
        case rendering::ValueType::TEXTURE:
        {
            if(mapping.texture.handle == 0)
                return;
                
            Texture *texture = renderer->render.textures[mapping.texture.handle - 1];
            set_texture_uniform(gl_shader.program, texture->handle, *texture_count);
            (*texture_count)++;
        }
        break;
        case rendering::ValueType::MS_TEXTURE:
        {
            if(mapping.ms_texture.handle == 0)
                return;
                
            Texture *texture = renderer->render.textures[mapping.ms_texture.handle - 1];
            set_ms_texture_uniform(gl_shader.program, texture->handle, *texture_count);
            (*texture_count)++;
        }
        break;
        case rendering::ValueType::STRUCTURE:
        case rendering::ValueType::INVALID:
        {
            assert(false);
        }
        break;
        }
    }
    break;
    case rendering::UniformMappingType::NONE:
    case rendering::UniformMappingType::DIFFUSE_TEX:
    case rendering::UniformMappingType::DIFFUSE_COLOR:
    case rendering::UniformMappingType::SPECULAR_TEX:
    case rendering::UniformMappingType::SPECULAR_COLOR:
    case rendering::UniformMappingType::SPECULAR_EXPONENT:
    case rendering::UniformMappingType::SPECULAR_INTENSITY_TEX:
    case rendering::UniformMappingType::AMBIENT_COLOR:
    case rendering::UniformMappingType::AMBIENT_TEX:
    case rendering::UniformMappingType::BUMP_TEX:
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
    case rendering::UniformMappingType::FRAMEBUFFER_WIDTH:
    {
        set_int_uniform(gl_shader.program, location, renderer->framebuffer_width);
    }
    break;
    case rendering::UniformMappingType::FRAMEBUFFER_HEIGHT:
    {
        set_int_uniform(gl_shader.program, location, renderer->framebuffer_height);
    }
    break;
    case rendering::UniformMappingType::TIME:
    {
        set_float_uniform(gl_shader.program, location, (r32)glfwGetTime());
    }
    break;
    case rendering::UniformMappingType::CLIPPING_PLANE:
    {
        set_vec4_uniform(gl_shader.program, location, render_pass.clipping_planes.plane);
    }
    break;
    case rendering::UniformMappingType::LIGHT_SPACE_MATRIX:
    {
        set_mat4_uniform(gl_shader.program, location, renderer->render.light_space_matrix);
    }
    break;
    case rendering::UniformMappingType::SHADOW_MAP:
    {
        set_int_uniform(gl_shader.program, location, *texture_count);
            
        rendering::TextureHandle handle = rendering::get_depth_texture_from_framebuffer(0, renderer->render.shadow_framebuffer, renderer);

        Texture *texture = renderer->render.textures[handle.handle - 1];
        set_texture_uniform(gl_shader.program, texture->handle, *texture_count);
        *texture_count += 1;
        // (*texture_count++);
    }
    break;
    case rendering::UniformMappingType::SHADOW_MAP_SIZE:
    {
        set_float_uniform(gl_shader.program, location, (r32)renderer->render.shadow_settings.size);
    }
    break;
    case rendering::UniformMappingType::SHADOW_VIEW_POSITION:
    {
        set_vec3_uniform(gl_shader.program, location, renderer->render.shadow_view_position);
    }
    break;
    case rendering::UniformMappingType::VIEWPORT_SIZE:
    {
        set_vec2_uniform(gl_shader.program, location, math::Vec2(renderer->framebuffer_width, renderer->framebuffer_height));
    }
    break;
    case rendering::UniformMappingType::MODEL:
    {
        set_mat4_uniform(gl_shader.program, location, transform.model);
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
        set_vec3_uniform(gl_shader.program, location, camera.pos);
    }
    break;
    case rendering::UniformMappingType::DISSOLVE:
    case rendering::UniformMappingType::POINT_LIGHTS:
    case rendering::UniformMappingType::DIRECTIONAL_LIGHTS:
    // @Incomplete: Handle these?
    break;
    default:
    break;
    }
}

static void setup_instanced_vertex_attribute_buffers(rendering::VertexAttributeInstanced *instanced_vertex_attributes, i32 attr_count, rendering::Shader &shader_info, RenderState &render_state, Renderer *renderer)
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
            buf_ptr = renderer->render.instancing.float_buffers[handle];
            size = sizeof(r32);
            count = renderer->render.instancing.float_buffer_counts[handle];

            Buffer *buffer = renderer->render.instancing.internal_float_buffers[handle];

            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glEnableVertexAttribArray(array_num);
            
            if(renderer->render.instancing.dirty_float_buffers[handle])
            {
                glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
                renderer->render.instancing.dirty_float_buffers[handle] = false;
            }

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT2:
        {
            num_values = 2;
            buf_ptr = renderer->render.instancing.float2_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer->render.instancing.float2_buffer_counts[handle];

            Buffer *buffer = renderer->render.instancing.internal_float2_buffers[handle];

            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glEnableVertexAttribArray(array_num);
            
            if(renderer->render.instancing.dirty_float2_buffers[handle])
            {
                glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
                renderer->render.instancing.dirty_float2_buffers[handle] = false;
            }

            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT3:
        {
            num_values = 3;
            buf_ptr = renderer->render.instancing.float3_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer->render.instancing.float3_buffer_counts[handle];

            Buffer *buffer = renderer->render.instancing.internal_float3_buffers[handle];

            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            glEnableVertexAttribArray(array_num);
            
            if(renderer->render.instancing.dirty_float3_buffers[handle])
            {
                glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
                renderer->render.instancing.dirty_float3_buffers[handle] = false;
            }
            
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::FLOAT4:
        {
            num_values = 4;
            buf_ptr = renderer->render.instancing.float4_buffers[handle];
            size = sizeof(r32) * num_values;
            count = renderer->render.instancing.float4_buffer_counts[handle];

            Buffer *buffer = renderer->render.instancing.internal_float4_buffers[handle];
            
            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            
            glEnableVertexAttribArray(array_num);
            
            if(renderer->render.instancing.dirty_float4_buffers[handle])
            {
                glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), buf_ptr);
                renderer->render.instancing.dirty_float4_buffers[handle] = false;
            }
                   
            glVertexAttribPointer(array_num, num_values, GL_FLOAT, GL_FALSE, (GLsizei)size, (void *)nullptr);
            glVertexAttribDivisor(array_num, 1);
        }
        break;
        case rendering::ValueType::MAT4:
        {
            num_values = 16;
            buf_ptr = renderer->render.instancing.mat4_buffers[handle];
            size = sizeof(math::Mat4);
            count = renderer->render.instancing.mat4_buffer_counts[handle];

            Buffer *buffer = renderer->render.instancing.internal_mat4_buffers[handle];
            GLsizei vec4_size = sizeof(math::Vec4);

            glEnableVertexAttribArray(array_num);
            glEnableVertexAttribArray(array_num + 1);
            glEnableVertexAttribArray(array_num + 2);
            glEnableVertexAttribArray(array_num + 3);

            glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
            
            if(renderer->render.instancing.dirty_mat4_buffers[handle])
            {
                glBufferData(GL_ARRAY_BUFFER, buffer->size, NULL, GL_DYNAMIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, (GLsizeiptr)(size * count), &((math::Mat4*)buf_ptr)[0]);
                renderer->render.instancing.dirty_mat4_buffers[handle] = false;
            }

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
        case rendering::ValueType::INTEGER:
        case rendering::ValueType::BOOL:
        case rendering::ValueType::TEXTURE:
        case rendering::ValueType::MS_TEXTURE:
        case rendering::ValueType::STRUCTURE:
        case rendering::ValueType::INVALID:
        debug_log("Value type not supported\n");
        assert(false);
        break;
        }
    }
}

static void render_buffer(rendering::PrimitiveType primitive_type, rendering::Transform transform, rendering::BufferHandle& buffer_handle, const rendering::RenderPass &render_pass, RenderState &render_state, Renderer *renderer, rendering::Material& material, const Camera &camera, i32 count = 0, ShaderGL *shader = nullptr)
{
    Buffer& buffer = get_internal_buffer(renderer, render_state, buffer_handle);
    
    bind_vertex_array(buffer.vao, &render_state);

    ShaderGL gl_shader;
    rendering::Shader &shader_info = renderer->render.shaders[material.shader.handle];

    // If we specified a custom shader, use it
    if (shader)
    {
        gl_shader = *shader;
        shader_info = renderer->render.shaders[gl_shader.handle];
    }
    else
    {
        gl_shader = render_state.gl_shaders[shader_info.index];
    }

    b32 fallback = false;

    if (!gl_shader.program) // Use fallback if it exists
    {
        fallback = true;

        gl_shader = render_state.gl_shaders[renderer->render.fallback_shader.handle];

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

    // assert(gl_shader.location_count == material.uniform_value_count);
    
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

        if(count == 1)
        {
            glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, nullptr);
        }
        else
            glDrawElementsInstanced(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, nullptr, count);
    }
    else
    {
        if (buffer.ibo)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ibo);
            if(primitive_type == rendering::PrimitiveType::TRIANGLES)
            {
                glDrawElements(GL_TRIANGLES, buffer.index_buffer_count, GL_UNSIGNED_SHORT, (void *)nullptr);
            }
            else if(primitive_type == rendering::PrimitiveType::LINE_LOOP)
            {
                glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, 0);
                glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_SHORT, (GLvoid*)(4 * sizeof(GLushort)));
                glDrawElements(GL_LINES, 8, GL_UNSIGNED_SHORT, (GLvoid*)(8 * sizeof(GLushort)));
            }
        }
        else
        {
            if(primitive_type == rendering::PrimitiveType::TRIANGLES)
                glDrawArrays(GL_TRIANGLES, 0, buffer.vertex_count / 3);
            else if(primitive_type == rendering::PrimitiveType::LINES)
                glDrawArrays(GL_LINES_ADJACENCY, 0, buffer.vertex_count);
            else if(primitive_type == rendering::PrimitiveType::LINES)
                glDrawArrays(GL_LINE_LOOP, 0, buffer.vertex_count);
        }
    }
}

static void render_ui_pass(RenderState &render_state, Renderer *renderer)
{
    rendering::RenderPass &pass = renderer->render.ui.pass;

    Framebuffer &framebuffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];
    
    glViewport(0, 0, framebuffer.width, framebuffer.height);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);
    // glClear(GL_COLOR_BUFFER_BIT);
    // glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Draw everything imgui
    ui_rendering::draw_imgui_frame(render_state, &render_state.imgui_state);

    Buffer &font_buffer = get_internal_buffer(renderer, render_state, renderer->render.ui.font_buffer);

    glDisable(GL_DEPTH_TEST);

    for(i32 i = 0; i < Z_LAYERS; i++)
    {
        i32* command_indices = pass.ui.text_z_layers[i];
        i32 command_count = pass.ui.text_z_layer_counts[i];

        if(command_count > 0)
        {
            bind_vertex_array(font_buffer.vao, &render_state);
            glBindBuffer(GL_ARRAY_BUFFER, font_buffer.vbo);
        
            for(i32 j = 0; j < command_count; j++)
            {
                rendering::TextRenderCommand &command = pass.ui.text_commands[command_indices[j]];

                if (command.clip)
                {
                    glEnable(GL_SCISSOR_TEST);
                    math::Rect clip_rect = command.clip_rect;
                    glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
                }

                rendering::CharacterBufferHandle char_buf_handle = command.buffer;
                rendering::CharacterData *coords = pass.ui.coords[char_buf_handle.handle];

                i32 vertex_buffer_size = (i32)(6 * command.text_length * sizeof(rendering::CharacterData));
                i32 vertex_count = (i32)(6 * command.text_length * 3);
                r32* vertex_buffer = (r32 *)coords;

                update_buffer(&font_buffer, rendering::BufferType::VERTEX, vertex_buffer, vertex_count, vertex_buffer_size, rendering::BufferUsage::DYNAMIC, &render_state, renderer);
                render_buffer(rendering::PrimitiveType::TRIANGLES, {}, renderer->render.ui.font_buffer, pass, render_state, renderer, command.material, pass.camera, 0 ,&render_state.gl_shaders[command.shader_handle.handle]);

                if (command.clip)
                {
                    glDisable(GL_SCISSOR_TEST);
                }
            }
        }

        command_indices = pass.ui.ui_z_layers[i];
        command_count = pass.ui.ui_z_layer_counts[i];

        for(i32 j = 0; j < command_count; j++)
        {
            rendering::UIRenderCommand &command = pass.ui.render_commands[command_indices[j]];

            if (command.clip)
            {
                glEnable(GL_SCISSOR_TEST);
                math::Rect clip_rect = command.clip_rect;
                glScissor((i32)clip_rect.x, (i32)clip_rect.y, (i32)clip_rect.width, (i32)clip_rect.height);
            }

            render_buffer(rendering::PrimitiveType::TRIANGLES, {}, command.buffer, pass, render_state, renderer,
                          command.material, pass.camera, 0, &render_state.gl_shaders[command.shader_handle.handle]);

            if (command.clip)
            {
                glDisable(GL_SCISSOR_TEST);
            }
        }

        pass.ui.ui_z_layer_counts[i] = 0;
        pass.ui.text_z_layer_counts[i] = 0;
    }

    

    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    pass.ui.text_command_count = 0;
    pass.ui.render_command_count = 0;
}

static void render_line(rendering::RenderCommand& command, rendering::Material& material, rendering::RenderPass &pass, Renderer *renderer, RenderState &render_state)
{
    math::Vec3 line[4];
    line[0] = 2.0f * command.line.p0 - command.line.p1;
    line[1] = command.line.p0;
    line[2] = command.line.p1;
    line[3] = 2.0f * line[2] - line[1];

    rendering::update_buffer(renderer->render.line_buffer, rendering::BufferType::VERTEX, rendering::BufferUsage::DYNAMIC, (r32*)line, 4, sizeof(math::Vec3) * 4, renderer);
                    
    rendering::set_uniform_value(renderer, material, "thickness", command.line.thickness);
    rendering::set_uniform_value(renderer, material, "color", command.line.color);
    rendering::set_uniform_value(renderer, material, "miter_limit", 0.1f);
    
    render_buffer(rendering::PrimitiveType::LINES, command.transform, renderer->render.line_buffer, pass, render_state, renderer, material, pass.camera, command.count, &render_state.gl_shaders[material.shader.handle]);
}

static void render_pass(RenderState &render_state, Renderer *renderer, rendering::RenderPass &pass)
{    
    if(pass.type == rendering::RenderPassType::NORMAL)
    {
        if(pass.commands.render_command_count == 0 && pass.commands.depth_free_command_count == 0)
        {
            pass.commands.render_command_count = 0;
            pass.commands.depth_free_command_count = 0;
            return;
        }

        rendering::UniformBufferHandle matrix_ubo = renderer->render.mapped_ubos[(i32)rendering::UniformBufferMappingType::VP];

        rendering::UniformBufferUpdate matrix_update = rendering::generate_ubo_update(matrix_ubo, rendering::UniformBufferMappingType::VP, renderer);

        rendering::add_ubo_update_value(matrix_update, matrix_ubo,
                                        0,
                                        math::transpose(pass.camera.projection_matrix),
                                        renderer);
        
        rendering::add_ubo_update_value(matrix_update, matrix_ubo
                                        , 1, math::transpose(pass.camera.view_matrix)
                                        , renderer);

        rendering::update_uniform_buffer(matrix_update, renderer);
        
        Framebuffer &framebuffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];

        if(pass.settings & rendering::RenderPassSettings::BACKFACE_CULLING)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
        }
        else if(pass.settings & rendering::RenderPassSettings::FRONTFACE_CULLING)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
        }

        glViewport(0, 0, framebuffer.width, framebuffer.height);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer_handle);

        // @Incomplete: Not all framebuffers should have depth testing or clear both bits

        if(pass.settings & rendering::RenderPassSettings::DISABLE_DEPTH)
        {
            glDepthMask(GL_FALSE);
        }
        else if(framebuffer.depth_buffer_count > 0)
        {
            glDepthMask(GL_TRUE);
            glEnable(GL_DEPTH_TEST);
            glDepthFunc(GL_LESS);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }

        if(!(pass.settings & rendering::RenderPassSettings::DONT_CLEAR))
        {
            if(pass.has_clear_color)
                glClearColor(pass.clear_color.r, pass.clear_color.g, pass.clear_color.b, pass.clear_color.a);
            else
                glClearColor(renderer->clear_color.r, renderer->clear_color.g, renderer->clear_color.b, renderer->clear_color.a);
            
            if(framebuffer.tex_color_buffer_count > 0 && framebuffer.depth_buffer_count > 0)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            else if(framebuffer.tex_color_buffer_count > 0)
                glClear(GL_COLOR_BUFFER_BIT);
            else
                glClear(GL_DEPTH_BUFFER_BIT);
        }
            
        for (i32 i = 0; i < pass.commands.render_command_count; i++)
        {
            rendering::RenderCommand &command = pass.commands.render_commands[i];

            if(command.blend_mode == rendering::BlendMode::ONE_MINUS_SOURCE)
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else if(command.blend_mode == rendering::BlendMode::ONE)
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }
                
            rendering::Material &material = get_material_instance(command.material, renderer);
            ShaderGL *shader = &render_state.gl_shaders[material.shader.handle];

            switch(command.type)
            {
            case rendering::RenderCommandType::BUFFER:
            {
                render_buffer(command.buffer.primitive_type, command.transform, command.buffer.buffer, pass, render_state, renderer, material, pass.camera, command.count, shader);
            }
            break;
            case rendering::RenderCommandType::LINE:
            {
                render_line(command, material, pass, renderer, render_state);
            }
            break;
            }
        }

        glDisable(GL_DEPTH_TEST);
        
        for (i32 i = 0; i < pass.commands.depth_free_command_count; i++)
        {
            rendering::RenderCommand &command = pass.commands.depth_free_commands[i];
            rendering::Material &material = get_material_instance(command.material, renderer);

            if(command.blend_mode == rendering::BlendMode::ONE_MINUS_SOURCE)
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            else if(command.blend_mode == rendering::BlendMode::ONE)
            {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            }

            switch(command.type)
            {
            case rendering::RenderCommandType::BUFFER:
            {
                render_buffer(command.buffer.primitive_type, command.transform, command.buffer.buffer, pass, render_state, renderer, material, pass.camera, command.count, &render_state.gl_shaders[material.shader.handle]);
            }
            break;
            case rendering::RenderCommandType::LINE:
            {
                render_line(command, material, pass, renderer, render_state);
            }
            break;
            }
        }

        pass.commands.render_command_count = 0;
        pass.commands.depth_free_command_count = 0;
        glDisable(GL_CULL_FACE);

    }
    else if(pass.type == rendering::RenderPassType::READ_DRAW)
    {
        Framebuffer &read_framebuffer = render_state.v2.framebuffers[pass.read_framebuffer.handle - 1];
        Framebuffer &draw_framebuffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];
            
        glViewport(0, 0, draw_framebuffer.width, draw_framebuffer.height);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, read_framebuffer.buffer_handle);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, draw_framebuffer.buffer_handle);

        for(i32 i = 0; i < draw_framebuffer.tex_color_buffer_count; i++)
        {
            glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
            glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
            
            glBlitFramebuffer(0, 0, draw_framebuffer.width, draw_framebuffer.height, 0, 0, draw_framebuffer.width, draw_framebuffer.height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        }
            
        glReadBuffer(GL_COLOR_ATTACHMENT0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
                
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

static void render_all_passes(RenderState &render_state, Renderer *renderer)
{
    // @Incomplete: Create a better way for enabling/disabling the clipping planes
    // Check if we have clipping planes
    glEnable(GL_CLIP_PLANE0);

    glDisable(GL_BLEND);
    
    rendering::RenderPass &shadow_pass = renderer->render.passes[renderer->render.shadow_pass.handle - 1];
    render_pass(render_state, renderer, shadow_pass);
    shadow_pass.commands.render_command_count = 0;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Go backwards through the array to enable easy render pass adding
    for (i32 pass_index = renderer->render.pass_count - 1; pass_index >= 0; pass_index--)
    {
        if(pass_index == renderer->render.shadow_pass.handle - 1)
            continue;
        
        rendering::RenderPass &pass = renderer->render.passes[pass_index];
        render_pass(render_state, renderer, pass);
    }

    for (i32 i = 0; i < MAX_INSTANCE_BUFFERS; i++)
    {
		if (renderer->render.instancing.internal_float_buffers[i]->usage == GL_DYNAMIC_DRAW)
		{
			renderer->render.instancing.float_buffer_counts[i] = 0;
		}

        if(renderer->render.instancing.internal_float2_buffers[i]->usage == GL_DYNAMIC_DRAW)
        {
            renderer->render.instancing.float2_buffer_counts[i] = 0;
        }

        if(renderer->render.instancing.internal_float3_buffers[i]->usage == GL_DYNAMIC_DRAW)
        {
            renderer->render.instancing.float3_buffer_counts[i] = 0;
        }

        if(renderer->render.instancing.internal_float4_buffers[i]->usage == GL_DYNAMIC_DRAW)
        {
            renderer->render.instancing.float4_buffer_counts[i] = 0;
        }

        if(renderer->render.instancing.internal_mat4_buffers[i]->usage == GL_DYNAMIC_DRAW)
        {
            renderer->render.instancing.mat4_buffer_counts[i] = 0;
        }
	}

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void swap_buffers(RenderState &render_state)
{
    // @Note(Daniel): Temporary fix for MacOS
    #if __APPLE__
    glFinish();
    #endif
    
    glfwSwapBuffers(render_state.window);
}

static void render_post_processing_passes(RenderState &render_state, Renderer *renderer)
{
    glDisable(GL_DEPTH_TEST);

    bind_vertex_array(render_state.framebuffer_quad_vao, &render_state);

    for (i32 pass_index = 0; pass_index < renderer->render.post_processing_pass_count; pass_index++)
    {
        rendering::RenderPass &pass = renderer->render.post_processing_passes[pass_index];
        Framebuffer pass_buffer = render_state.v2.framebuffers[pass.framebuffer.handle - 1];

        glBindFramebuffer(GL_FRAMEBUFFER, pass_buffer.buffer_handle);
 
        ShaderGL shader = render_state.gl_shaders[pass.post_processing.shader_handle.handle];

        glUseProgram(shader.program);

        i32 texture_count = 0;

        for (i32 i = 0; i < pass.post_processing.uniform_value_count; i++)
        {
            rendering::UniformValue &value = pass.post_processing.uniform_values[i];
            set_uniform({}, pass, value, shader, {}, &texture_count, render_state, renderer);
        }
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_state.quad_index_buffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void *)nullptr);
    }

    bind_vertex_array(0, &render_state);
}

static void render(RenderState &render_state, Renderer *renderer, r64 delta_time)
{
    reload_shaders(render_state, renderer);

    b32 should_render = renderer->window_width != 0;

    if (should_render)
    {
        // Render through all passes
        render_all_passes(render_state, renderer);
        render_post_processing_passes(render_state, renderer);
        render_ui_pass(render_state, renderer);

        render_state.bound_texture = 0;

        Framebuffer &final_framebuffer = render_state.v2.framebuffers[renderer->render.final_framebuffer.handle - 1];
        renderer->framebuffer_width = render_state.framebuffer_width;
        renderer->framebuffer_height = render_state.framebuffer_height;

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

        if (renderer->frame_lock != 0)
        {
            render_state.total_delta = 0.0;
            render_state.frame_delta += 1.0 / renderer->frame_lock;
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
