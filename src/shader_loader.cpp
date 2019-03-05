#include <sys/stat.h>

namespace rendering
{

// @Robustness: New shader stuff
    static void error(const char *msg, const char *file)
    {
        debug_log("ERROR: %s in %s", msg, file);
    }

    static ValueType parse_type(char *type, const char *file_path, b32 invalid_is_valid)
    {
        ValueType result;

        if (starts_with(type, "float"))
        {
            result = ValueType::FLOAT;
        }
        else if (starts_with(type, "vec2"))
        {
            result = ValueType::FLOAT2;
        }
        else if (starts_with(type, "vec3"))
        {
            result = ValueType::FLOAT3;
        }
        else if (starts_with(type, "vec4"))
        {
            result = ValueType::FLOAT4;
        }
        else if (starts_with(type, "mat4"))
        {
            result = ValueType::MAT4;
        }
        else if (starts_with(type, "bool"))
        {
            result = ValueType::BOOL;
        }
        else if (starts_with(type, "int"))
        {
            result = ValueType::INTEGER;
        }
        else if (starts_with(type, "sampler2DMS"))
        {
            result = ValueType::MS_TEXTURE;
        }
        else if (starts_with(type, "sampler2D"))
        {
            result = ValueType::TEXTURE;
        }
        else
        {
            result = ValueType::INVALID;

            if (!invalid_is_valid)
            {
                char error_buf[256];
                sprintf(error_buf, "Invalid value type '%s'", type);
                error(error_buf, file_path);
            }
        }

        return result;
    }

    static void eat_spaces_and_newlines(char **source)
    {
        while (*source[0] == ' ' || *source[0] == '\n')
            (*source)++;
    }

    static ValueType get_value_type(char **value, const char *file_path, b32 invalid_is_valid = false)
    {
        ValueType result;
        char type[32];
        sscanf(*value, "%s", type);

        result = parse_type(type, file_path, invalid_is_valid);
        *value += strlen(type) + 1;
        return result;
    }

// Eats a word and ignores new-lines
    static void parse_word(char **source, char *buffer)
    {
        i32 index = 0;

        while (*source[0] != ' ' && *source[0] != '\n' && *source[0] != ';')
        {
            buffer[index++] = **source;
            (*source)++;
        }

        buffer[index] = '\0';
    }

    static i32 get_structure_index(char *type_name, Shader &shader)
    {
        for (i32 i = 0; i < shader.structure_count; i++)
        {
            if (strcmp(type_name, shader.structures[i].name) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    static Structure *get_structure(char *type_name, Shader &shader)
    {
        for (i32 i = 0; i < shader.structure_count; i++)
        {
            if (strcmp(type_name, shader.structures[i].name) == 0)
            {
                return &shader.structures[i];
            }
        }
        return nullptr;
    }

    static DefinedValue *get_defined_value(char *name, Shader &shader)
    {
        for (i32 i = 0; i < shader.defined_value_count; i++)
        {
            if (strcmp(name, shader.defined_values[i].name) == 0)
            {
                return &shader.defined_values[i];
            }
        }
        return nullptr;
    }

    static VertexAttributeMappingType parse_vertex_attribute_mapping(char *mapped_buffer)
    {
        VertexAttributeMappingType type = VertexAttributeMappingType::NONE;

        if (starts_with(mapped_buffer, "POSITION"))
        {
            type = VertexAttributeMappingType::POSITION;
        }
        else if (starts_with(mapped_buffer, "SCALE"))
        {
            type = VertexAttributeMappingType::SCALE;
        }
        else if (starts_with(mapped_buffer, "ROTATION"))
        {
            type = VertexAttributeMappingType::ROTATION;
        }
        else if (starts_with(mapped_buffer, "MODEL"))
        {
            type = VertexAttributeMappingType::MODEL;
        }
        else if (starts_with(mapped_buffer, "DIFFUSE_COLOR"))
        {
            type = VertexAttributeMappingType::DIFFUSE_COLOR;
        }
        else if (starts_with(mapped_buffer, "PARTICLE_POSITION"))
        {
            type = VertexAttributeMappingType::PARTICLE_POSITION;
        }
        else if (starts_with(mapped_buffer, "PARTICLE_COLOR"))
        {
            type = VertexAttributeMappingType::PARTICLE_COLOR;
        }
        else if (starts_with(mapped_buffer, "PARTICLE_SIZE"))
        {
            type = VertexAttributeMappingType::PARTICLE_SIZE;
        }
        else if (starts_with(mapped_buffer, "PARTICLE_ANGLE"))
        {
            type = VertexAttributeMappingType::PARTICLE_ANGLE;
        }
        return type;
    }

    static CustomUniformHandle parse_custom_mapping(ValueType type, char *mapped_buffer, Renderer *renderer)
    {
        for(i32 i = 0; i < renderer->render.custom_mapping_count; i++)
        {
            CustomUniformMapping& mapping = renderer->render.custom_mappings[i];

            if(strcmp(mapping.name, mapped_buffer) == 0)
            {
                //@Note: Already exists as a mapping
                return { i };
            }
        }

        assert(renderer->render.custom_mapping_count < MAX_CUSTOM_UNIFORM_MAPPINGS);
        CustomUniformMapping& new_mapping = renderer->render.custom_mappings[renderer->render.custom_mapping_count++];
        strncpy(new_mapping.name, mapped_buffer, strlen(mapped_buffer));
        new_mapping.type = type;

        return { renderer->render.custom_mapping_count - 1 };
    }

    static UniformMappingType parse_mapping(char *mapped_buffer)
    {
        UniformMappingType type = UniformMappingType::NONE;

        if (starts_with(mapped_buffer, "DIFFUSE_TEX"))
        {
            type = UniformMappingType::DIFFUSE_TEX;
        }
        else if (starts_with(mapped_buffer, "DIFFUSE_COLOR"))
        {
            type = UniformMappingType::DIFFUSE_COLOR;
        }
        else if (starts_with(mapped_buffer, "SPECULAR_TEX"))
        {
            type = UniformMappingType::SPECULAR_TEX;
        }
        else if (starts_with(mapped_buffer, "SPECULAR_COLOR"))
        {
            type = UniformMappingType::SPECULAR_COLOR;
        }
        else if (starts_with(mapped_buffer, "SPECULAR_EXPONENT"))
        {
            type = UniformMappingType::SPECULAR_EXPONENT;
        }
        else if (starts_with(mapped_buffer, "AMBIENT_COLOR"))
        {
            type = UniformMappingType::AMBIENT_COLOR;
        }
        else if (starts_with(mapped_buffer, "AMBIENT_TEX"))
        {
            type = UniformMappingType::AMBIENT_TEX;
        }
        else if (starts_with(mapped_buffer, "SHADOW_MAP"))
        {
            type = UniformMappingType::SHADOW_MAP;
        }
        else if (starts_with(mapped_buffer, "SHADOW_VIEW_POSITION"))
        {
            type = UniformMappingType::SHADOW_VIEW_POSITION;
        }
        else if (starts_with(mapped_buffer, "VIEWPORT_SIZE"))
        {
            type = UniformMappingType::VIEWPORT_SIZE;
        }
        else if (starts_with(mapped_buffer, "MODEL"))
        {
            type = UniformMappingType::MODEL;
        }
        else if (starts_with(mapped_buffer, "VIEW"))
        {
            type = UniformMappingType::VIEW;
        }
        else if (starts_with(mapped_buffer, "PROJECTION"))
        {
            type = UniformMappingType::PROJECTION;
        }
        else if (starts_with(mapped_buffer, "CAMERA_POSITION"))
        {
            type = UniformMappingType::CAMERA_POSITION;
        }        
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHTS"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHTS;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHTS"))
        {
            type = UniformMappingType::POINT_LIGHTS;
        }
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_COUNT"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHT_COUNT;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_COUNT"))
        {
            type = UniformMappingType::POINT_LIGHT_COUNT;
        }
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_DIRECTION"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHT_DIRECTION;
        }
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_AMBIENT"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHT_AMBIENT;
        }
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_DIFFUSE"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHT_DIFFUSE;
        }
        else if (starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_SPECULAR"))
        {
            type = UniformMappingType::DIRECTIONAL_LIGHT_SPECULAR;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_POSITION"))
        {
            type = UniformMappingType::POINT_LIGHT_POSITION;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_CONSTANT"))
        {
            type = UniformMappingType::POINT_LIGHT_CONSTANT;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_LINEAR"))
        {
            type = UniformMappingType::POINT_LIGHT_LINEAR;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_QUADRATIC"))
        {
            type = UniformMappingType::POINT_LIGHT_QUADRATIC;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_AMBIENT"))
        {
            type = UniformMappingType::POINT_LIGHT_AMBIENT;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_DIFFUSE"))
        {
            type = UniformMappingType::POINT_LIGHT_DIFFUSE;
        }
        else if (starts_with(mapped_buffer, "POINT_LIGHT_SPECULAR"))
        {
            type = UniformMappingType::POINT_LIGHT_SPECULAR;
        }
        else if (starts_with(mapped_buffer, "LIGHT_SPACE_MATRIX"))
        {
            type = UniformMappingType::LIGHT_SPACE_MATRIX;
        }
        else if (starts_with(mapped_buffer, "CLIPPING_PLANE"))
        {
            type = UniformMappingType::CLIPPING_PLANE;
        }
        else if (starts_with(mapped_buffer, "CAMERA_UP"))
        {
            type = UniformMappingType::CAMERA_UP;
        }
        else if (starts_with(mapped_buffer, "CAMERA_RIGHT"))
        {
            type = UniformMappingType::CAMERA_RIGHT;
        }
        else if (starts_with(mapped_buffer, "CAMERA_FORWARD"))
        {
            type = UniformMappingType::CAMERA_FORWARD;
        }
        else if(starts_with(mapped_buffer, "FRAMEBUFFER_WIDTH"))
        {
            type = UniformMappingType::FRAMEBUFFER_WIDTH;
        }
        else if(starts_with(mapped_buffer, "FRAMEBUFFER_HEIGHT"))
        {
            type = UniformMappingType::FRAMEBUFFER_HEIGHT;
        }
        else if(starts_with(mapped_buffer, "TIME"))
        {
            type = UniformMappingType::TIME;
        }
        
        return type;
    }

    static void parse_uniform_array_data(char **rest, Uniform &uniform, Shader &shader)
    {
        if (starts_with(*rest, "["))
        {
            uniform.is_array = true;
            char array_size[32];
            sscanf(*rest, "[%[^]]]", array_size);

            if (DefinedValue *defined_value = get_defined_value(array_size, shader))
            {
                (*rest) += strlen(defined_value->name) + 2;

                uniform.array_size = defined_value->integer_val;
            }
            else
            {
                uniform.array_size = strtol(*rest, rest, 10);
            }
        }
    }

    static Uniform parse_uniform(Renderer *renderer, ValueType type, char *rest, char *original_buffer, Shader &shader, const char *file_path)
    {
        Uniform uniform = {};
        uniform.type = type;

        while (rest[0] == ' ')
            rest++;

        sscanf(rest, "%[^[ \n]", uniform.name);

        rest += strlen(uniform.name);

        parse_uniform_array_data(&rest, uniform, shader);

        if (uniform.name[strlen(uniform.name) - 1] == ';')
        {
            uniform.name[strlen(uniform.name) - 1] = '\0';
        }
        else
        {
            i32 c = 0;
            i32 m_c = 0;
            i32 custom_m_c = -1;
            char mapped_buffer[32];

            while (rest[c] != ';' && rest[c] != '\n')
            {
                if(rest[c] == '$')
                {
                    assert(m_c == 0);
                    custom_m_c++;
                }
                else if (rest[c] != ':' && rest[c] != ' ' && custom_m_c == -1)
                {
                    mapped_buffer[m_c++] = rest[c];
                }
                else if(rest[c] != '$' && rest[c] != ' ' && m_c == 0 && custom_m_c != -1)
                {
                    mapped_buffer[custom_m_c++] = rest[c];
                }
                c++;
            }

            if (m_c > 0)
            {
                //@Note: Check for correct indices
                original_buffer[strlen(original_buffer) - strlen(rest) + 2] = '\0';
                original_buffer[strlen(original_buffer) - strlen(rest) + 1] = '\n';
                original_buffer[strlen(original_buffer) - strlen(rest)] = ';';
                mapped_buffer[m_c] = '\0';
                uniform.mapping_type = parse_mapping(mapped_buffer);
            }
            else if(custom_m_c > 0)
            {
                //@Note: Check for correct indices
                original_buffer[strlen(original_buffer) - strlen(rest) + 2] = '\0';
                original_buffer[strlen(original_buffer) - strlen(rest) + 1] = '\n';
                original_buffer[strlen(original_buffer) - strlen(rest)] = ';';
                mapped_buffer[custom_m_c] = '\0';
                uniform.custom_mapping = parse_custom_mapping(type, mapped_buffer, renderer);
                uniform.mapping_type = UniformMappingType::CUSTOM;
            }
        }

        return (uniform);
    }

    static void parse_structure_variables(Renderer *renderer, char **source, char *total_buffer, Structure &structure, Shader &shader, const char *file_path)
    {
        size_t i = 0;

        char buffer[256];

        while (read_line(buffer, 256, source))
        {
            b32 should_break = false;

            char *rest = &buffer[0];
            eat_spaces_and_newlines(&rest);

            if (starts_with(rest, "struct"))
            {
                sscanf(rest, "struct %[^\n]", structure.name);
            }
            else if (starts_with(rest, "}"))
            {
                should_break = true;
            }
            else if (starts_with(rest, "{"))
            {
            }
            else if (strlen(rest) == 0)
            {
            } // @Incomplete: Better fix here
            else
            {
                // GET TYPE
                ValueType type = get_value_type(&rest, file_path);
                if (type != ValueType::INVALID)
                {
                    Uniform uniform = parse_uniform(renderer, type, rest, buffer, shader, file_path);
                    structure.uniforms[structure.uniform_count++] = uniform;
                }
            }

            strncpy(&total_buffer[i], buffer, strlen(buffer));
            i += strlen(buffer);

            if (should_break)
                break;
        }

        total_buffer[i++] = '\0';
    }

    static char *load_shader_text(Renderer *renderer, MemoryArena *arena, char *source, bool is_vertex_shader, Shader &shader, Uniform **uniforms_array, i32 *uniform_count, const char *file_path, size_t *file_size = nullptr)
    {
        size_t i = 0;

        MemoryArena temp_arena = {};
        auto temp_mem = begin_temporary_memory(&temp_arena);

        char *result;
        b32 instancing_enabled = false;

        size_t temp_current_size = strlen(source) * 15;
        char *temp_result = push_string(&temp_arena, temp_current_size);

        char buffer[256];

        while (read_line(buffer, 256, &source))
        {
            if (starts_with(buffer, "#vert") || starts_with(buffer, "#geo") || starts_with(buffer, "#frag"))
            {
                break;
            }
            else if (starts_with(buffer, "#include \""))
            {
                // @Note: Game shaders are included with ""
                char include_name[256];

                i32 quotes_found  = 0;
                i32 index = 0;
                
                for(size_t c = 0; c < strlen(buffer); c++)
                {
                    if(buffer[c] == '\"')
                    {
                        quotes_found++;
                        if(quotes_found == 2)
                        {
                            include_name[index] = '\0';
                            break;
                        }
                    }
                    else if(quotes_found == 1)
                    {
                        include_name[index++] = buffer[c];
                    }
                }

                char *included_path = concat("../assets/shaders/", include_name, &temp_arena);

                FILE *included_shd = fopen(included_path, "r");

                if (included_shd)
                {
                    char *included_source = read_file_into_buffer(&temp_arena, included_shd);
                    char *path = concat(concat(file_path, "<-", &temp_arena), included_path, &temp_arena);

                    char *included_text = load_shader_text(renderer, &temp_arena, included_source, is_vertex_shader, shader, uniforms_array, uniform_count, path);

                    if (i + strlen(buffer) > temp_current_size)
                    {
                        error("Temp buffer is too small", file_path);
                    }

                    strncpy(&temp_result[i], included_text, strlen(included_text));
                    i += strlen(included_text);
                    if (file_size)
                    {
                        *file_size += strlen(buffer);
                    }

                    fclose(included_shd);

                    continue;
                }
                else
                {
                    char err_buf[256];
                    sprintf(err_buf, "Include file not found %s", included_path);
                    error(err_buf, file_path);
                }
            }
            else if (starts_with(buffer, "#include <"))
            {
                // @Note: Engine shaders are included with <>
                char include_name[256];
                sscanf(buffer, "#include <%[^>]", include_name);
                char *included_path = concat("../engine_assets/standard_shaders/", include_name, &temp_arena);

                FILE *included_shd = fopen(included_path, "r");

                if (included_shd)
                {
                    char *included_source = read_file_into_buffer(&temp_arena, included_shd);
                    char *path = concat(concat(file_path, "<-", &temp_arena), included_path, &temp_arena);

                    char *included_text = load_shader_text(renderer, &temp_arena, included_source, is_vertex_shader, shader, uniforms_array, uniform_count, path);

                    if (i + strlen(buffer) > temp_current_size)
                    {
                        error("Temp buffer is too small", file_path);
                    }

                    strncpy(&temp_result[i], included_text, strlen(included_text));
                    i += strlen(included_text);

                    if (file_size)
                    {
                        *file_size += strlen(buffer);
                    }

                    fclose(included_shd);

                    continue;
                }
                else
                {
                    char err_buf[256];
                    sprintf(err_buf, "Include file not found %s", included_path);
                    error(err_buf, file_path);
                }
            }
            else if (starts_with(buffer, "#BEGIN_INSTANCING"))
            {
                instancing_enabled = true;
                continue;
            }
            else if (starts_with(buffer, "#END_INSTANCING"))
            {
                instancing_enabled = false;
                continue;
            }
            else if (starts_with(buffer, "#define"))
            {
                // @Incomplete: float defines missing
                DefinedValue defined_value = {};
                defined_value.type = DefinedValueType::INTEGER;
                sscanf(buffer, "#define %s %d", defined_value.name, &defined_value.integer_val);
                shader.defined_values[shader.defined_value_count++] = defined_value;
            }
            else if (starts_with(buffer, "struct"))
            {
                source -= strlen(buffer);

                char total_buffer[1024];
                Structure &structure = shader.structures[shader.structure_count++];
                structure.uniform_count = 0;
                parse_structure_variables(renderer, &source, total_buffer, structure, shader, file_path);

                strncpy(&temp_result[i], total_buffer, strlen(total_buffer));
                i += strlen(total_buffer);

                continue;
            }
            else if (starts_with(buffer, "uniform"))
            {
                char *rest = &buffer[strlen("uniform") + 1];
                char *prev = rest;

                ValueType type = get_value_type(&rest, file_path, true);

                if (type == ValueType::INVALID) // We might have a struct
                {
                    char name[32];
                    sscanf(prev, "%[^ ]", name);

                    i32 structure_index = get_structure_index(name, shader);

                    if (structure_index < 0)
                    {
                        char buf[32];
                        sprintf(buf, "Structure '%s' not found\n", prev);
                        error(buf, file_path);
                    }
                    else
                    {
                        Uniform uniform = parse_uniform(renderer, ValueType::STRUCTURE, rest, buffer, shader, file_path);
                        uniform.structure_index = structure_index;
                        (*uniforms_array)[(*uniform_count)++] = uniform;
                    }
                }
                else
                {
                    Uniform uniform = parse_uniform(renderer, type, rest, buffer, shader, file_path);
                    (*uniforms_array)[(*uniform_count)++] = uniform;
                }
            }
            else if (starts_with(buffer, "layout"))
            {
                if (is_vertex_shader)
                {
                    VertexAttribute *vertex_attribute = nullptr;

                    if (instancing_enabled)
                    {
                        vertex_attribute = &shader.instanced_vertex_attributes[shader.instanced_vertex_attribute_count++].attribute;
                    }
                    else
                    {
                        vertex_attribute = &shader.vertex_attributes[shader.vertex_attribute_count++];
                    }

                    char *rest = &buffer[strlen("layout") + 1];

                    while (rest[0] == ' ' || rest[0] == '(')
                    {
                        rest++;
                    }

                    if (starts_with(rest, "location"))
                    {
                        rest += strlen("location");
                        // @Note: Eat spaces until we see =
                        while (rest[0] == ' ')
                        {
                            rest++;
                        }

                        if (rest[0] == '=')
                        {
                            rest++;
                            vertex_attribute->location = strtol(rest, &rest, 10);

                            while (rest[0] == ' ' || rest[0] == ')')
                                rest++;

                            if (starts_with(rest, "in"))
                            {
                                while (rest[0] == ' ')
                                    rest++;

                                rest += 2;

                                vertex_attribute->type = get_value_type(&rest, file_path);

                                eat_spaces_and_newlines(&rest);
                                parse_word(&rest, vertex_attribute->name);
                                eat_spaces_and_newlines(&rest);

                                if (rest[0] == ':') // Oh boy, we've go a mapping
                                {
                                    char *start = &rest[0];
                                    rest += 1;
                                    eat_spaces_and_newlines(&rest);
                                    char mapping_name[32];
                                    parse_word(&rest, mapping_name);
                                    shader.instanced_vertex_attributes[shader.instanced_vertex_attribute_count - 1].mapping_type = parse_vertex_attribute_mapping(mapping_name);
                                    buffer[strlen(buffer) - (strlen(start) + 1) + 2] = '\0';
                                    buffer[strlen(buffer) - (strlen(start) + 1) + 1] = '\n';
                                    buffer[strlen(buffer) - (strlen(start) + 1)] = ';';
                                }
                            }
                            else if (starts_with(rest, "out"))
                            {
                                // @Incomplete: IGNORE
                            }
                            else
                            {
                                error("layout 'in' keyword missing", file_path);
                            }
                        }
                        else
                        {
                            error("layout location invalid, found no =", file_path);
                        }
                    }
                    else
                    {
                        error("layout location not found", file_path);
                    }
                }
            }

            if (i + strlen(buffer) > temp_current_size)
            {
                error("Temp buffer is too small", file_path);
            }

            strncpy(&temp_result[i], buffer, strlen(buffer));

            i += strlen(buffer);
            if (file_size)
            {
                *file_size += strlen(buffer);
            }
        }

        result = push_string(arena, i);
        memcpy(result, temp_result, i);

        end_temporary_memory(temp_mem);

        return result;
    }
    
// Shader reload
    static b32 check_dirty(Shader &shader)
    {
        struct stat sb;
        stat(shader.path, &sb);

        time_t last_loaded = shader.last_loaded;
        time_t time_new = sb.st_mtime;

        shader.last_loaded = time_new;

        if (last_loaded != 0 && last_loaded < time_new)
        {
            return true;
        }

        return false;
    }

    static void set_last_loaded(Shader &shader)
    {
        struct stat sb1;
        stat(shader.path, &sb1);
        shader.last_loaded = sb1.st_mtime;
    }

    static void check_for_shader_file_changes(Renderer *renderer)
    {
        for (i32 i = 0; i < renderer->render.shader_count; i++)
        {
            if (check_dirty(renderer->render.shaders[i]))
            {
                renderer->render.shaders_to_reload[renderer->render.shaders_to_reload_count++] = i;
            }
        }
    }

    static int out_count = 0;

    static void load_shader(Renderer *renderer, Shader &shader)
    {
        FILE *file = fopen(shader.path, "r");

        shader.loaded = false;

        if (file)
        {
            size_t size = 0;
            char *source = read_file_into_buffer(&renderer->shader_arena, file, &size);

            shader.vert_shader = nullptr;
            shader.geo_shader = nullptr;
            shader.frag_shader = nullptr;

            Uniform *uniforms = (Uniform *)malloc(sizeof(Uniform) * 512);
            i32 uniform_count = 0;

			shader.instanced_vertex_attribute_count = 0;
            shader.vertex_attribute_count = 0;
            shader.uniform_count = 0;
            shader.structure_count = 0;
            shader.defined_value_count = 0;

            for (size_t i = 0; i < size; i++)
            {
                if (starts_with(&source[i], "#vert"))
                {
                    shader.vert_shader = load_shader_text(renderer, &renderer->shader_arena, &source[i + strlen("#vert") + 1], true, shader, &uniforms, &uniform_count, shader.path, &i);
                }
                else if (starts_with(&source[i], "#geo"))
                {
                    shader.geo_shader = load_shader_text(renderer, &renderer->shader_arena, &source[i + strlen("#geo") + 1], false, shader, &uniforms, &uniform_count, shader.path, &i);
                }
                else if (starts_with(&source[i], "#frag"))
                {
                    shader.frag_shader = load_shader_text(renderer, &renderer->shader_arena, &source[i + strlen("#frag") + 1], false, shader, &uniforms, &uniform_count, shader.path, &i);
                }
            }

            shader.uniforms = nullptr;
            shader.uniform_count = 0;

            shader.uniforms = push_array(&shader.arena, uniform_count, Uniform);
            shader.uniform_count = (i32)uniform_count;
            memcpy(shader.uniforms, uniforms, uniform_count * sizeof(Uniform));
            free(uniforms);

#if DEBUG
            char out_name[256];
            strcpy(out_name, shader.path);
            for (size_t i = 0; i < strlen(out_name); i++)
            {
                if (out_name[i] == '/')
                {
                    out_name[i] = '_';
                }
            }

            char out_path[256];

            sprintf(out_path, "../out/%s.shd", out_name);

            // FILE *shd = fopen(out_path, "w");
            // fwrite(shader.vert_shader, strlen(shader.vert_shader), 1, shd);
            // fwrite(shader.frag_shader, strlen(shader.frag_shader), 1, shd);
            // fclose(shd);
#endif

            // fclose(file);
            // shader.loaded = shader.vert_shader && shader.frag_shader;
            // set_last_loaded(shader);
        }
        else
        {
            shader.loaded = false;
            error("File not found", shader.path);
        }
    }

    ShaderHandle get_shader_by_path(Renderer *renderer, const char *path)
    {
        for(i32 i = 0; i < renderer->render.shader_count; i++)
        {
            if(strcmp(renderer->render.shaders[i].path, path) == 0)
                return { i }; 
        }

        return { -1 };
    }
    
    static ShaderHandle load_shader(Renderer *renderer, const char *file_path)
    {
        assert(renderer->render.shader_count + 1 < 64);

        ShaderHandle handle = get_shader_by_path(renderer, file_path);
        if(handle.handle != -1)
            return handle;
        
        Shader &shader = renderer->render.shaders[renderer->render.shader_count];
        strncpy(shader.path, file_path, strlen(file_path));
        shader.index = renderer->render.shader_count;
        load_shader(renderer, shader);
        handle.handle = renderer->render.shader_count++;
        return handle;
    }

} // namespace rendering
