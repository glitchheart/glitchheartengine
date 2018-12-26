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

    static Uniform parse_uniform(ValueType type, char *rest, char *original_buffer, Shader &shader, const char *file_path)
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
            char mapped_buffer[32];

            while (rest[c] != ';' && rest[c] != '\n')
            {
                if (rest[c] != ':' && rest[c] != ' ')
                {
                    mapped_buffer[m_c++] = rest[c];
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
        }

        return (uniform);
    }

    static void parse_structure_variables(char **source, char *total_buffer, Structure &structure, Shader &shader, const char *file_path)
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
                    Uniform uniform = parse_uniform(type, rest, buffer, shader, file_path);
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

    static char *load_shader_text(MemoryArena *arena, char *source, bool is_vertex_shader, Shader &shader, Uniform **uniforms_array, i32 *uniform_count, const char *file_path, size_t *file_size = nullptr)
    {
        size_t i = 0;

        MemoryArena temp_arena = {};
        auto temp_mem = begin_temporary_memory(&temp_arena);

        char *result;
        b32 instancing_enabled = false;

        size_t temp_current_size = strlen(source) * 10;
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
                sscanf(buffer, "#include \"%s\"", include_name);

                char *included_path = concat("../assets/shaders/", include_name, &temp_arena);

                FILE *included_shd = fopen(included_path, "r");

                if (included_shd)
                {
                    char *included_source = read_file_into_buffer(&temp_arena, included_shd);
                    char *path = concat(concat(file_path, "<-", &temp_arena), included_path, &temp_arena);

                    char *included_text = load_shader_text(&temp_arena, included_source, is_vertex_shader, shader, uniforms_array, uniform_count, path);

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

                    char *included_text = load_shader_text(&temp_arena, included_source, is_vertex_shader, shader, uniforms_array, uniform_count, path);

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
                parse_structure_variables(&source, total_buffer, structure, shader, file_path);

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
                        Uniform uniform = parse_uniform(ValueType::STRUCTURE, rest, buffer, shader, file_path);
                        uniform.structure_index = structure_index;
                        (*uniforms_array)[(*uniform_count)++] = uniform;
                    }
                }
                else
                {
                    Uniform uniform = parse_uniform(type, rest, buffer, shader, file_path);
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

    static void check_for_shader_file_changes(Renderer &renderer)
    {
        for (i32 i = 0; i < renderer.render.shader_count; i++)
        {
            if (check_dirty(renderer.render.shaders[i]))
            {
                renderer.render.shaders_to_reload[renderer.render.shaders_to_reload_count++] = i;
            }
        }
    }

    static int out_count = 0;

    static void load_shader(Renderer &renderer, Shader &shader)
    {
        FILE *file = fopen(shader.path, "r");

        shader.loaded = false;

        if (file)
        {
            size_t size = 0;
            char *source = read_file_into_buffer(&renderer.shader_arena, file, &size);

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
                    shader.vert_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#vert") + 1], true, shader, &uniforms, &uniform_count, shader.path, &i);
                }
                else if (starts_with(&source[i], "#geo"))
                {
                    shader.geo_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#geo") + 1], false, shader, &uniforms, &uniform_count, shader.path, &i);
                }
                else if (starts_with(&source[i], "#frag"))
                {
                    shader.frag_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#frag") + 1], false, shader, &uniforms, &uniform_count, shader.path, &i);
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

            FILE *shd = fopen(out_path, "w");
            fwrite(shader.vert_shader, strlen(shader.vert_shader), 1, shd);
            fwrite(shader.frag_shader, strlen(shader.frag_shader), 1, shd);
            fclose(shd);
#endif

            fclose(file);
            shader.loaded = shader.vert_shader && shader.frag_shader;
            set_last_loaded(shader);
        }
        else
        {
            shader.loaded = false;
            error("File not found", shader.path);
        }
    }

    static ShaderHandle load_shader(Renderer &renderer, const char *file_path)
    {
        assert(renderer.render.shader_count + 1 < 64);
        Shader &shader = renderer.render.shaders[renderer.render.shader_count];
        strncpy(shader.path, file_path, strlen(file_path));
        shader.index = renderer.render.shader_count;
        load_shader(renderer, shader);
        return {renderer.render.shader_count++};
    }
    
    static void set_shader_values(Material &material, Shader &shader, Renderer &renderer)
    {
        material.array_count = 0;
        memcpy(material.instanced_vertex_attributes, shader.instanced_vertex_attributes, sizeof(VertexAttributeInstanced) * shader.instanced_vertex_attribute_count);
        material.instanced_vertex_attribute_count = shader.instanced_vertex_attribute_count;

        UniformValue *uniform_vals = nullptr;
        UniformArray *arrays = nullptr;

        i32 location_index = 0;

        // @Incomplete: Get shader uniforms
        for (i32 uni_i = 0; uni_i < shader.uniform_count; uni_i++)
        {
            Uniform u = shader.uniforms[uni_i];

            // If we're working with an array type we should add all the possible uniforms for the whole array
            if (u.is_array)
            {
                // First we push the array placeholder inside the uniforms array
                UniformValue value = {};
                value.uniform = u;
                value.array_index = material.array_count++;
                strncpy(value.name, u.name, strlen(u.name) + 1);
                buf_push(uniform_vals, value);

                UniformArray array = {};
                strncpy(array.name, u.name, strlen(u.name) + 1);
                array.entry_count = 0;
                array.max_size = u.array_size;

                if (u.type == ValueType::STRUCTURE)
                {
                    Structure &structure = shader.structures[u.structure_index];

                    i32 num_to_allocate = u.array_size;
                    array.entries = push_array(&renderer.mesh_arena, num_to_allocate, UniformEntry);

                    if (u.mapping_type == UniformMappingType::DIRECTIONAL_LIGHTS || u.mapping_type == UniformMappingType::POINT_LIGHTS)
                    {
                        material.lighting.receives_light = true;
                    }

                    for (i32 i = 0; i < u.array_size; i++)
                    {
                        UniformEntry entry = {};
                        entry.value_count = 0;

                        for (i32 j = 0; j < structure.uniform_count; j++)
                        {
                            Uniform struct_uni = structure.uniforms[j];
                            Uniform new_uniform = struct_uni;
                            new_uniform.location_index = location_index++;
                            UniformValue u_v = {};

                            strncpy(u_v.name, struct_uni.name, strlen(struct_uni.name) + 1);
                            sprintf(new_uniform.name, "%s[%d].%s", u.name, i, struct_uni.name);

                            u_v.uniform = new_uniform;
                            entry.values[entry.value_count++] = u_v;
                        }

                        array.entries[array.entry_count++] = entry;
                    }
                }
                else
                {
                    i32 num_to_allocate = u.array_size;
                    array.entries = push_array(&renderer.mesh_arena, num_to_allocate, UniformEntry);

                    for (i32 i = 0; i < u.array_size; i++)
                    {
                        UniformEntry entry = {};
                        entry.value_count = 0;

                        UniformValue u_v = {};
                        Uniform new_uniform = u;
                        new_uniform.location_index = location_index++;
                        sprintf(new_uniform.name, "%s[%d]", u.name, i);
                        u_v.uniform = new_uniform;
                        entry.values[entry.value_count++] = u_v;
                        array.entries[array.entry_count++] = entry;
                    }
                }

                buf_push(arrays, array);
            }
            else
            {
                UniformValue u_v = {};
                strncpy(u_v.name, u.name, strlen(u.name) + 1);
                u_v.uniform = u;
                u_v.uniform.location_index = location_index++;
                buf_push(uniform_vals, u_v);
            }
        }

        size_t size = buf_len(uniform_vals);
        size_t array_size = buf_len(arrays);

        if (!material.uniform_values)
        {
            //material.uniform_values = push_array(&renderer.mesh_arena, size, UniformValue);
        }

        if (!material.arrays)
        {
            //material.arrays = push_array(&renderer.mesh_arena, array_size, UniformArray);
        }

        material.uniform_value_count = (i32)size;
        memcpy(material.uniform_values, uniform_vals, size * sizeof(UniformValue));
        material.array_count = (i32)array_size;
        memcpy(material.arrays, arrays, array_size * sizeof(UniformValue));

        buf_free(arrays);
        buf_free(uniform_vals);
    }

    static MaterialInstanceHandle create_internal_material_instance(Renderer &renderer, MaterialHandle material_handle)
    {
        Material &material = renderer.render.materials[material_handle.handle];

        i32 handle = renderer.render.internal_material_count++;
        
        renderer.render.internal_materials[handle] = material;
        renderer.render.internal_materials[handle].source_material = material_handle;

        return { handle, true, { - 1} };
    }

    static MaterialHandle create_material(Renderer &renderer, ShaderHandle shader_handle)
    {
        Material &material = renderer.render.materials[renderer.render.material_count];
        material.shader = shader_handle;

        Shader &shader = renderer.render.shaders[shader_handle.handle];

        set_shader_values(material, shader, renderer);

        return {renderer.render.material_count++};
    }
    
    static MaterialInstanceHandle create_material_instance(Renderer &renderer, MaterialHandle material_handle, MaterialInstanceArrayHandle array_handle)
    {
        Material &material = renderer.render.materials[material_handle.handle];

        i32 internal_index = renderer.render._internal_material_instance_array_handles[array_handle.handle - 1];
        i32 handle = renderer.render.material_instance_array_counts[internal_index]++;

        renderer.render.material_instance_arrays[internal_index][handle] = material;
        renderer.render.material_instance_arrays[internal_index][handle].source_material = material_handle;

        return {handle, false, array_handle};
    }

    static void set_fallback_shader(Renderer &renderer, const char *path)
    {
        renderer.render.fallback_shader = load_shader(renderer, path);
    }

    static void set_shadow_map_shader(Renderer &renderer, const char *path)
    {
        renderer.render.shadow_map_shader = load_shader(renderer, path);
    }

    static void set_wireframe_shader(Renderer &renderer, const char *path)
    {
        renderer.render.wireframe_shader = load_shader(renderer, path);
        MaterialHandle material = create_material(renderer, renderer.render.wireframe_shader);
        renderer.render.wireframe_material = create_internal_material_instance(renderer, material);
    }

    static void set_bloom_shader(Renderer &renderer, const char *path)
    {
        renderer.render.bloom_shader = load_shader(renderer, path);
    }

    static void set_blur_shader(Renderer &renderer, const char *path)
    {
        renderer.render.blur_shader = load_shader(renderer, path);
    }

    static void set_hdr_shader(Renderer &renderer, const char *path)
    {
        renderer.render.hdr_shader = load_shader(renderer, path);
    }

    static void set_final_framebuffer(Renderer &renderer, FramebufferHandle framebuffer)
    {
        renderer.render.final_framebuffer = framebuffer;
    }

    static void set_light_space_matrices(Renderer &renderer, math::Mat4 projection_matrix, math::Vec3 view_position, math::Vec3 target)
    {
        math::Mat4 view_matrix = math::look_at_with_target(view_position, target);
        renderer.render.shadow_view_position = view_position;
        renderer.render.light_space_matrix = projection_matrix * view_matrix;
    }

    static inline Material &get_material_instance(MaterialInstanceHandle handle, Renderer &renderer)
    {
        if(handle.internal)
        {
            return renderer.render.internal_materials[handle.handle];
        }
        
        i32 array_index = renderer.render._internal_material_instance_array_handles[handle.array_handle.handle - 1];
        return renderer.render.material_instance_arrays[array_index][handle.handle];
    }

    static UniformValue *get_array_variable_mapping(MaterialInstanceHandle handle, const char *array_name, UniformMappingType type, Renderer &renderer)
    {
        Material &material = get_material_instance(handle, renderer);
        ;

        for (i32 i = 0; i < material.array_count; i++)
        {
            if (strcmp(material.arrays[i].name, array_name) == 0)
            {
                UniformEntry &first_entry = material.arrays[i].entries[0];

                for (i32 j = 0; j < first_entry.value_count; j++)
                {
                    if (first_entry.values[j].uniform.mapping_type == type)
                    {
                        return &first_entry.values[j];
                    }
                }
                break;
            }
        }

        return nullptr;
    }

    static UniformValue *mapping(Material &material, UniformMappingType type)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            if (material.uniform_values[i].uniform.mapping_type == type)
            {
                return &material.uniform_values[i];
            }
        }

        return nullptr;
    }

    static UniformValue *get_mapping(MaterialInstanceHandle handle, UniformMappingType type, Renderer &renderer)
    {
        Material &material = get_material_instance(handle, renderer);
        return mapping(material, type);
    }

    static UniformValue *get_value(Material &material, ValueType type, const char *name)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &value = material.uniform_values[i];

            if (value.uniform.type == type && strcmp(value.uniform.name, name) == 0)
            {
                return &value;
            }
        }

        return nullptr;
    }

    static void set_old_material_values(Material &new_material, Material &old_material)
    {
        for (i32 uniform_index = 0; uniform_index < new_material.uniform_value_count; uniform_index++)
        {
            UniformValue &value = new_material.uniform_values[uniform_index];

            if (value.uniform.is_array)
            {
                // @Incomplete: NOT SUPPORTED

                // Uniform &u = value.uniform;

                // if(u.type == ValueType::STRUCTURE)
                // {

                // }
                // else
                // {
                //     // UniformArray &array = new_material
                //     // for(i32 i = 0; i < array.entry_count; i++)
                //     // {
                //     //     UniformArrayEntry &entry = array.entries[i];
                //     //     if(Uniform
                //     // }
                // }
            }
            else
            {
                if (UniformValue *old_value = get_value(old_material, value.uniform.type, value.uniform.name))
                {
                    switch (value.uniform.type)
                    {
                    case ValueType::FLOAT:
					value.float_val = old_value->float_val;
					break;
                    case ValueType::FLOAT2:
					value.float2_val = old_value->float2_val;
					break;
                    case ValueType::FLOAT3:
					value.float3_val = old_value->float3_val;
					break;
                    case ValueType::FLOAT4:
					value.float4_val = old_value->float4_val;
					break;
                    case ValueType::INTEGER:
					value.integer_val = old_value->integer_val;
					break;
                    case ValueType::BOOL:
					value.boolean_val = old_value->boolean_val;
					break;
                    case ValueType::MAT4:
					value.mat4_val = old_value->mat4_val;
					break;
                    case ValueType::TEXTURE:
					value.texture = old_value->texture;
					break;
                    case ValueType::MS_TEXTURE:
					value.ms_texture = old_value->ms_texture;
					break;
                    case ValueType::STRUCTURE:
					// @Incomplete
					break;
                    default:
					assert(false);
                    }
                }
            }
        }
    }

    static void get_updated_material(Material *new_material, Material &current_material, Shader &shader, Renderer &renderer)
    {
        set_shader_values(*new_material, shader, renderer);
        set_old_material_values(*new_material, current_material);
    }

    static void update_materials_with_shader(Renderer &renderer, Shader &shader)
    {
        for (i32 i = 0; i < renderer.render.material_count; i++)
        {
            Material &material = renderer.render.materials[i];
            if (material.shader.handle == shader.index)
            {
                Material new_material = {};
                new_material.shader = material.shader;
                get_updated_material(&new_material, material, shader, renderer);
                memcpy(new_material.instanced_vertex_attributes, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count * sizeof(VertexAttributeInstanced));
                new_material.instanced_vertex_attribute_count = material.instanced_vertex_attribute_count;
                renderer.render.materials[i] = new_material;
            }
        }

        for (i32 i = 0; i < renderer.render.material_instance_array_count; i++)
        {
            for (i32 j = 0; j < renderer.render.material_instance_array_counts[i]; j++)
            {
                Material &material = renderer.render.material_instance_arrays[i][j];
                if (material.shader.handle == shader.index)
                {
                    Material new_material = {};
                    new_material.shader = material.shader;
                    get_updated_material(&new_material, material, shader, renderer);
                    memcpy(new_material.instanced_vertex_attributes, material.instanced_vertex_attributes, material.instanced_vertex_attribute_count * sizeof(VertexAttributeInstanced));
                    new_material.instanced_vertex_attribute_count = material.instanced_vertex_attribute_count;
                    renderer.render.material_instance_arrays[i][j] = new_material;
                }
            }
        }
    }

    static void load_texture(Renderer &renderer, TextureFiltering filtering, TextureWrap wrap, unsigned char *data, i32 width, i32 height, TextureFormat format, TextureHandle &handle)
    {
        if (handle.handle == 0)
        {
            handle.handle = renderer.render.texture_count++ + 1;
            assert(handle.handle != 0);
        }

        Texture* texture = renderer.render.textures[handle.handle - 1];

        renderer.api_functions.load_texture(texture, filtering, wrap, format, width, height, data, renderer.api_functions.render_state, &renderer);
    }

    static void load_texture(const char *full_texture_path, Renderer &renderer, TextureFiltering filtering, TextureWrap wrap, TextureFormat format, TextureHandle &handle)
    {
        if(handle.handle == 0)
        {
            handle.handle = renderer.render.texture_count++ + 1;
            assert(handle.handle != 0);
        }

        PlatformFile png_file = platform.open_file(full_texture_path, POF_READ | POF_OPEN_EXISTING | POF_IGNORE_ERROR);

        if(png_file.handle)
        {
            platform.seek_file(png_file, 0, SO_END);
            i32 size = platform.tell_file(png_file);
            platform.seek_file(png_file, 0, SO_SET);

            TemporaryMemory temp_mem = begin_temporary_memory(&renderer.texture_arena);
            stbi_uc* tex_data = push_size(&renderer.texture_arena, size + 1, stbi_uc);
            platform.read_file(tex_data, size, 1, png_file);

            i32 width;
            i32 height;
            
            unsigned char* image_data = stbi_load_from_memory(tex_data, size, &width, &height, 0, STBI_rgb_alpha);
            platform.close_file(png_file);
            end_temporary_memory(temp_mem);

            assert(renderer.api_functions.load_texture);

            Texture* texture = renderer.render.textures[handle.handle - 1];
            
            renderer.api_functions.load_texture(texture, filtering, wrap, format, width, height, image_data, renderer.api_functions.render_state, &renderer);

            stbi_image_free(image_data);
        }
        else
        {
            printf("Texture could not be loaded: %s\n", full_texture_path);
            assert(false);
        }
    }

    static void update_buffer(Renderer &renderer, UpdateBufferInfo update_info)
    {
        assert(renderer.render.updated_buffer_handle_count + 1 < global_max_custom_buffers);
        assert(renderer.render._internal_buffer_handles);

        // @Incomplete: Update from data
        // Find RegisterBufferInfo from handle
        // Update data in info from new_data

        i32 internal_handle = renderer.render._internal_buffer_handles[update_info.buffer.handle - 1];

        renderer.render.buffers[internal_handle].data = update_info.update_data;

        renderer.render.updated_buffer_handles[renderer.render.updated_buffer_handle_count++] = internal_handle;
    }

    static size_t size_for_type(ValueType type)
    {
        switch (type)
        {
        default:
        case ValueType::INVALID:
        case ValueType::TEXTURE:
        case ValueType::MS_TEXTURE:
		assert(false);
        case ValueType::FLOAT:
		return sizeof(r32);
        case ValueType::FLOAT2:
		return sizeof(r32) * 2;
        case ValueType::FLOAT3:
		return sizeof(r32) * 3;
        case ValueType::FLOAT4:
		return sizeof(r32) * 4;
        case ValueType::INTEGER:
        case ValueType::BOOL:
		return sizeof(i32);
        case ValueType::MAT4:
		return sizeof(r32) * 16;
        }
    }

    static math::Vec2i get_scale(Renderer& renderer)
    {
        return {renderer.framebuffer_width, renderer.framebuffer_height};
    }
    

    static math::Vec3 to_ui(Renderer& renderer, math::Vec2 coord)
    {
        math::Vec2i scale = get_scale(renderer);
        math::Vec3 res;
        res.x = ((r32)coord.x / (r32)scale.x) * UI_COORD_DIMENSION;
        res.y = ((r32)coord.y / (r32)scale.y) * UI_COORD_DIMENSION;
        res.z = 0.0f;
        return res;
    }

    math::Vec2 from_ui(Renderer& renderer, math::Vec3 coord)
    {
        math::Vec2i scale = get_scale(renderer);
        math::Vec2 res(0.0f);
        res.x = (((r32)coord.x / (r32)UI_COORD_DIMENSION) * scale.x);
        res.y = (((r32)coord.y / (r32)UI_COORD_DIMENSION) * scale.y);
        return res;
    }

    r32 from_ui(Renderer& renderer, i32 scale, r32 coord)
    {
        return ((r32)coord / (r32)UI_COORD_DIMENSION) * (r32)scale;
    }

    r32 to_ui(Renderer& renderer, i32 scale, r32 coord)
    {
        return (coord / (r32)scale) * (r32)UI_COORD_DIMENSION;
    }

        static math::Vec2 get_text_size(const char *text, TrueTypeFontInfo font)
    {
        math::Vec2 size;
        r32 placeholder_y = 0.0;

        i32 lines = 1;

        r32 current_width = 0.0f;
    
        for(u32 i = 0; i < strlen(text); i++)
        {
            if(text[i] != '\n' && text[i] != '\r')
            {
                stbtt_aligned_quad quad;
                stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                    text[i] - font.first_char, &size.x, &placeholder_y, &quad, 1);
        
                if(quad.y1 - quad.y0 > size.y)
                {
                    size.y = quad.y1 - quad.y0;
                }
        
                i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
                current_width += (r32)kerning * font.scale;
            }
            else
            {
                if(size.x > current_width)
                    current_width = size.x;

                size.x = 0.0f;
                lines++;
            }
        }

        return math::Vec2(current_width, size.y * lines * (lines - 1));
    }

    static TrueTypeFontInfo get_tt_font_info(Renderer& renderer, i32 handle)
    {
        assert(handle >= 0 && handle < renderer.tt_font_count);
        return renderer.tt_font_infos[handle];
    }

    static LineData get_line_size_data(const char *text, TrueTypeFontInfo font)
    {
        math::Vec2 size;
        r32 placeholder_y = 0.0;
    
        LineData line_data = {};
        line_data.total_height = 0.0f;
        line_data.line_count = 1;
    
        line_data.line_spacing = (r32)font.size + font.line_gap * font.scale;
    
        for(u32 i = 0; i < strlen(text); i++)
        {
            if(text[i] != '\n' && text[i] != '\r')
            {
                stbtt_aligned_quad quad;
                stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                    text[i] - font.first_char, &line_data.line_sizes[line_data.line_count - 1].x, &placeholder_y, &quad, 1);
        
                if(quad.y1 - quad.y0 > size.y)
                {
                    line_data.line_sizes[line_data.line_count - 1].y = quad.y1 - quad.y0;
                }
        
                i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
                line_data.line_sizes[line_data.line_count - 1].x += (r32)kerning * font.scale;
            }
            else
            {
                line_data.line_count++;
            }
        }

        if(line_data.line_count == 1)
        {
            line_data.total_height = line_data.line_sizes[0].y;
        }
        else
            line_data.total_height = (line_data.line_count - 1) * line_data.line_spacing;
    
        return line_data;
    }


    static math::Vec2 get_text_size_scaled(Renderer& renderer, const char* text, TrueTypeFontInfo font, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        LineData line_data = get_line_size_data(text, font);
        math::Vec2 font_size = line_data.line_sizes[0];
        math::Vec2 result(0.0f);
    
        math::Vec2i scale = get_scale(renderer);
    
        result.x = (font_size.x / (r32)scale.x) * UI_COORD_DIMENSION;
    
        if(scaling_flags & UIScalingFlag::KEEP_ASPECT_RATIO)
        {
            r32 ratio = font_size.y / font_size.x;
            result.y = font_size.x * ratio;
        }
        else
        {
            result.y = (font_size.y / (r32)scale.y) * UI_COORD_DIMENSION;
        }
    
        return result;
    }
    
// Gets an array of text widths for each character
// Remember to free
    static TextLengthInfo get_char_widths_scaled(Renderer& renderer, const char* text, TrueTypeFontInfo &font, MemoryArena* arena)
    {
        TextLengthInfo info = {};
    
        info.length = strlen(text);
        info.widths = push_array(arena, info.length, r32);//(r32*)calloc(info.length, sizeof(r32));
    
        r32 placeholder_y = 0.0f;
    
        math::Vec2i scale = get_scale(renderer);
    
        for(size_t i = 0; i < info.length; i++)
        {
            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(font.char_data, font.atlas_width, font.atlas_height,
                                text[i] - font.first_char, &info.widths[i], &placeholder_y, &quad, 1);
        
            i32 kerning = stbtt_GetCodepointKernAdvance(&font.info, text[i] - font.first_char, text[i + 1] - font.first_char);
        
            info.widths[i] += (r32)kerning * font.scale;
            info.widths[i] = ((r32)info.widths[i] / (r32)scale.x) * UI_COORD_DIMENSION;
        }
    
        return info;
    }


    static void load_font(Renderer &renderer, const char *path, i32 size, FontHandle &handle)
    {
        i32 index = handle.handle;

        if (index == 0)
        {
            handle.handle = renderer.tt_font_count++;
            index = handle.handle;
        }
        
        TrueTypeFontInfo &font_info = renderer.tt_font_infos[index];
        char buf[256];
        strncpy(buf, path, strlen(path) + 1);

        if(font_info.ttf_buffer)
        {
            free(font_info.ttf_buffer);
        }

        TextureHandle texture = font_info.texture;
        
        font_info = {};
        
        strncpy(font_info.path, buf, strlen(buf) + 1);

        RenderPass& pass = renderer.render.ui.pass;

        FramebufferInfo& framebuffer = renderer.render.framebuffers[pass.framebuffer.handle - 1];

        font_info.resolution_loaded_for.width = (i32)framebuffer.width;
        font_info.resolution_loaded_for.height = (i32)framebuffer.height;

        font_info.oversample_x = 1;
        font_info.oversample_y = 1;
        font_info.first_char = ' ';
        font_info.char_count = '~' - ' ';
        font_info.load_size = size;

        font_info.size = (i32)from_ui(renderer, (i32)framebuffer.height, (r32)font_info.load_size);

        i32 count_per_line = (i32)math::ceil(math::sqrt((r32)font_info.char_count));
        font_info.atlas_width = math::multiple_of_number(font_info.size * count_per_line, 4);
        font_info.atlas_height = math::multiple_of_number(font_info.size * count_per_line, 4);

        font_info.ttf_buffer = (unsigned char*)malloc(1<<20);
        
        TemporaryMemory temp_mem = begin_temporary_memory(&renderer.font_arena);

        unsigned char *temp_bitmap = push_array(&renderer.font_arena, font_info.atlas_width * font_info.atlas_height, unsigned char);

        fread(font_info.ttf_buffer, 1, 1 << 20, fopen(font_info.path, "rb"));

        stbtt_InitFont(&font_info.info, font_info.ttf_buffer, 0);
        font_info.scale = stbtt_ScaleForPixelHeight(&font_info.info, 15);
        stbtt_GetFontVMetrics(&font_info.info, &font_info.ascent, &font_info.descent, &font_info.line_gap);
        font_info.baseline = (i32)(font_info.ascent * font_info.scale);

        stbtt_pack_context context;
        if (!stbtt_PackBegin(&context, temp_bitmap, font_info.atlas_width, font_info.atlas_height, 0, 1, nullptr))
            printf("Failed to initialize font");

        stbtt_PackSetOversampling(&context, font_info.oversample_x, font_info.oversample_y);
        if (!stbtt_PackFontRange(&context, font_info.ttf_buffer, 0, (r32)font_info.size, font_info.first_char, font_info.char_count, font_info.char_data))
            printf("Failed to pack font");

        stbtt_PackEnd(&context);

        load_texture(renderer, TextureFiltering::LINEAR, TextureWrap::CLAMP_TO_EDGE, temp_bitmap, font_info.atlas_width, font_info.atlas_height, TextureFormat::RED, texture);

        font_info.texture = texture;

        font_info.line_height = font_info.size + font_info.line_gap * font_info.scale;

        r32 largest_character = 0;

        for (i32 i = 0; i < font_info.char_count; i++)
        {
            char str[2];
            str[0] = (char)(font_info.first_char + i);
            str[1] = '\0';
            math::Vec2 char_size = get_text_size(str, font_info);
            if (char_size.y > largest_character)
            {
                largest_character = char_size.y;
            }
        }

        font_info.largest_character_height = largest_character;

        end_temporary_memory(temp_mem);
    }

    static RegisterBufferInfo create_register_buffer_info()
    {
        RegisterBufferInfo info = {};
        info.data = {};
        info.vertex_attribute_count = 0;
        info.stride = 0;
        return info;
    }

    static void add_vertex_attrib(ValueType type, RegisterBufferInfo &info)
    {
        VertexAttribute attribute = {};
        attribute.type = type;
        info.vertex_attributes[info.vertex_attribute_count++] = attribute;

        info.stride += size_for_type(type);
    }

    static void load_material_from_mtl(Renderer &renderer, MaterialHandle material_handle, const char *file_path)
    {
        // @Incomplete: We need a better way to do this!
        // Find the directory of the file
        size_t index = 0;
        for (size_t i = 0; i < strlen(file_path); i++)
        {
            if (file_path[i] == '/')
            {
                index = i + 1;
            }
        }

        TemporaryMemory temp_block = begin_temporary_memory(&renderer.temp_arena);

        char *dir = push_string(temp_block.arena, index);
        strncpy(dir, file_path, index);

        dir[index] = 0;
        FILE *file = fopen(file_path, "r");

        if (file)
        {
            char buffer[256];

            Material &material = renderer.render.materials[material_handle.handle];
            if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
            {
                u->float4_val = math::Rgba(1, 1, 1, 1);
            }

            if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
            {
                u->float4_val = math::Rgba(0, 0, 0, 1);
            }

            if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
            {
                u->float4_val = math::Rgba(0.0f);
            }

            while (fgets(buffer, sizeof(buffer), file))
            {
                if (starts_with(buffer, "newmtl"))
                {
                    // @Incomplete: Save name
                }
                else if (starts_with(buffer, "illum")) // illumination
                {
                }
                else if (starts_with(buffer, "Ka")) // ambient color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_COLOR))
                    {
                        sscanf(buffer, "Ka %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Kd")) // diffuse color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
                    {
                        sscanf(buffer, "Kd %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Ks")) // specular color
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_COLOR))
                    {
                        sscanf(buffer, "Ks %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if (starts_with(buffer, "Ns")) // specular exponent
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
                    {
                        sscanf(buffer, "Ns %f", &u->float_val);
                    }
                }
                else if (starts_with(buffer, "d"))
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DISSOLVE))
                    {
                        sscanf(buffer, "d %f", &u->float_val);
                    }
                }
                else if (starts_with(buffer, "map_Ka")) // ambient map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::AMBIENT_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ka %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Kd")) // diffuse map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::DIFFUSE_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Kd %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Ks")) // specular map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ks %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                    }
                }
                else if (starts_with(buffer, "map_Ns")) // specular intensity map
                {
                    if (UniformValue *u = mapping(material, UniformMappingType::SPECULAR_INTENSITY_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ns %s", name);

                        if (name[0] == '.')
                            load_texture(name, renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, REPEAT, TextureFormat::RGBA, u->texture);
                    }
                }
            }

            fclose(file);
        }
        else
        {
            error("Can't load .mtl file", file_path);
        }

        end_temporary_memory(temp_block);
    }

    static void generate_vertex_buffer(r32 *vertex_buffer, Vertex *vertices, i32 vertex_count, i32 vertex_size, b32 has_normals, b32 has_uvs)
    {
        i32 vertex_data_count = vertex_size;

        for (i32 i = 0; i < vertex_count; i++)
        {
            i32 increment_by = 1;
            i32 base_index = i * vertex_data_count;
            Vertex vertex = vertices[i];
            vertex_buffer[base_index] = vertex.position.x;
            vertex_buffer[base_index + increment_by++] = vertex.position.y;
            vertex_buffer[base_index + increment_by++] = vertex.position.z;

            if (has_normals)
            {
                vertex_buffer[base_index + increment_by++] = vertex.normal.x;
                vertex_buffer[base_index + increment_by++] = vertex.normal.y;
                vertex_buffer[base_index + increment_by++] = vertex.normal.z;
            }

            if (has_uvs)
            {
                vertex_buffer[base_index + increment_by++] = vertex.uv.x;
                vertex_buffer[base_index + increment_by++] = vertex.uv.y;
            }
        }
    }

    static i32 _find_unused_handle(Renderer &renderer)
    {
        for (i32 index = renderer.render._current_internal_buffer_handle; index < global_max_custom_buffers; index++)
        {
            if (renderer.render._internal_buffer_handles[index] == -1)
            {
                renderer.render._current_internal_buffer_handle = index;
                return index;
            }
        }

        for (i32 index = 0; index < global_max_custom_buffers; index++)
        {
            if (renderer.render._internal_buffer_handles[index] == -1)
            {
                renderer.render._current_internal_buffer_handle = index;
                return index;
            }
        }

        assert(false);

        return -1;
    }

    static void generate_index_buffer(u16 *index_buffer, Face *faces, i32 face_count)
    {
        i32 face_data_count = 3;

        for (i32 i = 0; i < face_count; i++)
        {
            i32 base_index = i * face_data_count;
            Face face = faces[i];
            index_buffer[base_index] = face.indices[0];
            index_buffer[base_index + 1] = face.indices[1];
            index_buffer[base_index + 2] = face.indices[2];
        }
    }

    static BufferHandle register_buffer(Renderer &renderer, RegisterBufferInfo info)
    {
        assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);
        assert(renderer.render._internal_buffer_handles);

        i32 unused_handle = _find_unused_handle(renderer) + 1;

        renderer.render._internal_buffer_handles[unused_handle - 1] = renderer.render.buffer_count++;

        renderer.render.buffers[renderer.render._internal_buffer_handles[unused_handle - 1]] = info;

        return {unused_handle};
    }

    static BufferHandle create_quad_buffer(Renderer &renderer, u64 anchor = 0, b32 uvs = false)
    {
        assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);

        // @Note: Untextured
        i32 vertex_size = 2;

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::STATIC;
        add_vertex_attrib(ValueType::FLOAT2, info);

        r32 *quad_vertices = nullptr;

        math::Vec2 pivot = math::Vec2(0.5f);

        if (anchor & UIAlignment::BOTTOM)
        {
            pivot.y = 0.0f;
        }
        else if (anchor & UIAlignment::TOP)
        {
            pivot.y = 1.0f;
        }

        if (anchor & UIAlignment::LEFT)
        {
            pivot.x = 0.0f;
        }
        else if (anchor & UIAlignment::RIGHT)
        {
            pivot.x = 1.0f;
        }

        if (uvs)
        {
            add_vertex_attrib(ValueType::FLOAT2, info);
            vertex_size += 2;
            r32 vertices[16] =
                {
                    0.0f - pivot.x, 0.0f - pivot.y, 0.0f, 1.0f,
                    1.0f - pivot.x, 0.0f - pivot.y, 1.0f, 1.0f,
                    1.0f - pivot.x, 1.0f - pivot.y, 1.0f, 0.0f,
                    0.0f - pivot.x, 1.0f - pivot.y, 0.0f, 0.0f};

            quad_vertices = vertices;
        }
        else
        {
            r32 vertices[8] =
                {
                    0.0f - pivot.x,
                    0.0f - pivot.y,
                    1.0f - pivot.x,
                    0.0f - pivot.y,
                    1.0f - pivot.x,
                    1.0f - pivot.y,
                    0.0f - pivot.x,
                    1.0f - pivot.y,
                };

            quad_vertices = vertices;
        }

        u16 quad_indices[6] =
            {
                0, 1, 2,
                0, 2, 3};

        info.data.vertex_count = 4;
        info.data.vertex_buffer_size = info.data.vertex_count * vertex_size * (i32)sizeof(r32);

        info.data.vertex_buffer = push_size(&renderer.buffer_arena, info.data.vertex_buffer_size, r32);

        for (i32 i = 0; i < info.data.vertex_count * vertex_size; i++)
        {
            info.data.vertex_buffer[i] = quad_vertices[i];
        }

        i32 index_count = 6;
        info.data.index_buffer_size = index_count * (i32)sizeof(u16);
        info.data.index_buffer_count = index_count;

        info.data.index_buffer = push_size(&renderer.buffer_arena, info.data.index_buffer_size, u16);

        for (i32 i = 0; i < index_count; i++)
        {
            info.data.index_buffer[i] = quad_indices[i];
        }

        return {register_buffer(renderer, info).handle};
    }

    static BufferHandle create_buffers_from_mesh(Renderer &renderer, Mesh &mesh, u64 vertex_data_flags, b32 has_normals, b32 has_uvs)
    {
        assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);
        i32 vertex_size = 3;

        RegisterBufferInfo info = create_register_buffer_info();
        info.usage = BufferUsage::STATIC;
        add_vertex_attrib(ValueType::FLOAT3, info);

        if (has_normals)
        {
            add_vertex_attrib(ValueType::FLOAT3, info);
            vertex_size += 3;
        }

        if (has_uvs)
        {
            add_vertex_attrib(ValueType::FLOAT2, info);
            vertex_size += 2;
        }

        info.data.vertex_count = mesh.vertex_count;
        info.data.vertex_buffer_size = mesh.vertex_count * vertex_size * (i32)sizeof(r32);

        info.data.vertex_buffer = push_size(&renderer.mesh_arena, info.data.vertex_buffer_size, r32);
        generate_vertex_buffer(info.data.vertex_buffer, mesh.vertices, mesh.vertex_count, vertex_size, has_normals, has_uvs);

        i32 index_count = mesh.face_count * 3;
        info.data.index_buffer_size = index_count * (i32)sizeof(u16);
        info.data.index_buffer_count = index_count;

        // @Robustness:(Niels): How do we make sure that this is cleared if the mesh is removed?
        // Or will that never happen? Maybe use malloc/free instead? Or maybe at some point
        // we really __should__ create a more general purpose allocator ourselves...
        info.data.index_buffer = push_size(&renderer.mesh_arena, info.data.index_buffer_size, u16);
        generate_index_buffer(info.data.index_buffer, mesh.faces, mesh.face_count);

        return {register_buffer(renderer, info).handle};
    }
    
    static BufferHandle create_plane(Renderer &renderer, math::Vec3 *scale)
    {
        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;

        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer.mesh_arena, sizeof(plane_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer.mesh_arena, sizeof(plane_indices) / sizeof(u16) / 3, Face);
        mesh.vertex_count = sizeof(plane_vertices) / sizeof(r32) / 3;

        for (i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(plane_vertices[i * 3], plane_vertices[i * 3 + 1], plane_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(plane_normals[i * 3], plane_normals[i * 3 + 1], plane_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(plane_uvs[i * 2], plane_uvs[i * 2 + 1]);
            
            min_x = MIN(min_x, vertex.position.x);
            min_y = MIN(min_y, vertex.position.y);
            min_z = MIN(min_z, vertex.position.z);
            max_x = MAX(max_x, vertex.position.x);
            max_y = MAX(max_y, vertex.position.y);
            max_z = MAX(max_z, vertex.position.z);
        }

        mesh.face_count = sizeof(plane_indices) / sizeof(u16) / 3;

        for (i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];

            face.indices[0] = plane_indices[i * 3];
            face.indices[1] = plane_indices[i * 3 + 1];
            face.indices[2] = plane_indices[i * 3 + 2];
        }

        if(scale)
        {
            *scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z); 
        }
        
        return {create_buffers_from_mesh(renderer, mesh, 0, true, true)};
    }

    static BufferHandle create_cube(Renderer &renderer, math::Vec3 *scale)
    {
        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;

        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer.mesh_arena, sizeof(cube_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer.mesh_arena, sizeof(cube_indices) / sizeof(u16) / 3, Face);

        mesh.vertex_count = sizeof(cube_vertices) / sizeof(r32) / 3;

        for (i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(cube_vertices[i * 3], cube_vertices[i * 3 + 1], cube_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(cube_normals[i * 3], cube_normals[i * 3 + 1], cube_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(cube_uvs[i * 2], cube_uvs[i * 2 + 1]);

            min_x = MIN(min_x, vertex.position.x);
            min_y = MIN(min_y, vertex.position.y);
            min_z = MIN(min_z, vertex.position.z);
            max_x = MAX(max_x, vertex.position.x);
            max_y = MAX(max_y, vertex.position.y);
            max_z = MAX(max_z, vertex.position.z);
        }

        mesh.face_count = sizeof(cube_indices) / sizeof(u16) / 3;

        for (i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];

            face.indices[0] = cube_indices[i * 3];
            face.indices[1] = cube_indices[i * 3 + 1];
            face.indices[2] = cube_indices[i * 3 + 2];
        }

        if(scale)
        {
            *scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z); 
        }
        
        return {create_buffers_from_mesh(renderer, mesh, 0, true, true)};
    }

    static b32 vertex_equals(Vertex &v1, Vertex &v2)
    {
        return v1.position.x == v2.position.x && v1.position.y == v2.position.y && v1.position.z == v2.position.z && v1.uv.x == v2.uv.x && v1.uv.y == v2.uv.y && v1.normal.x == v2.normal.x && v1.normal.y == v2.normal.y && v1.normal.z == v2.normal.z;
    }

    static i32 check_for_identical_vertex(Vertex &vertex, math::Vec2 uv, math::Vec3 normal, Vertex *final_vertices, b32 *should_add)
    {
        size_t current_size = buf_len(final_vertices);
        vertex.uv = uv;
        vertex.normal = normal;

        for (size_t index = 0; index < current_size; index++)
        {
            Vertex &existing = final_vertices[index];

            if (vertex_equals(existing, vertex))
            {
                return (i32)index;
            }
        }

        *should_add = true;

        return (i32)current_size;
    }

    static BufferHandle load_obj(Renderer &renderer, char *file_path, MaterialHandle *material_handle, math::Vec3 *scale)
    {
        FILE *file = fopen(file_path, "r");

        b32 with_uvs = false;
        b32 with_normals = false;

        Vertex *vertices = nullptr;
        math::Vec3 *normals = nullptr;
        math::Vec2 *uvs = nullptr;

        Vertex *final_vertices = nullptr;

        Face *faces = nullptr;

        i32 vert_index = 0;
        i32 normal_index = 0;
        i32 uv_index = 0;

        r32 min_x = 10000;
        r32 min_y = 10000;
        r32 min_z = 10000;
        r32 max_x = -10000;
        r32 max_y = -10000;
        r32 max_z = -10000;

        // Right now we only support one mtl-file per obj-file
        // And since we only support one mesh per obj-file at the moment that should be fine.
        // @Robustness: We have to support more advanced files later... Maybe...
        b32 has_mtl_file = false;
        char mtl_file_name[32];

        if (file)
        {
            char buffer[256];

            while ((fgets(buffer, sizeof(buffer), file) != NULL))
            {
                if (starts_with(buffer, "g")) // we're starting with new geometry
                {
                    // @Incomplete: Save the name of the geometry
                }
                else if (starts_with(buffer, "mtllib")) // Material file
                {
                    // Read the material file-name
                    sscanf(buffer, "mtllib %s", mtl_file_name);
                }
                else if (starts_with(buffer, "usemtl")) // Used material for geometry
                {
                    has_mtl_file = true;
                    // Ignored, for now.
                    // This is only relevant when we've got multiple materials
                }
                else if (starts_with(buffer, "v ")) // vertex
                {
                    Vertex vertex = {};
                    sscanf(buffer, "v %f %f %f", &vertex.position.x, &vertex.position.y, &vertex.position.z);

                    min_x = MIN(min_x, vertex.position.x);
                    min_y = MIN(min_y, vertex.position.y);
                    min_z = MIN(min_z, vertex.position.z);
                    max_x = MAX(max_x, vertex.position.x);
                    max_y = MAX(max_y, vertex.position.y);
                    max_z = MAX(max_z, vertex.position.z);
                    
                    buf_push(vertices, vertex);
                    vert_index++;
                }
                else if (starts_with(buffer, "vn")) // vertex normal
                {
                    with_normals = true;
                    math::Vec3 normal(0.0f);
                    sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                    buf_push(normals, normal);
                    normal_index++;
                }
                else if (starts_with(buffer, "vt")) // vertex uv
                {
                    with_uvs = true;
                    math::Vec2 uv(0.0f);
                    sscanf(buffer, "vt %f %f", &uv.x, &uv.y);
                    uv.y = 1.0f - uv.y;
                    buf_push(uvs, uv);
                    uv_index++;
                }
                else if (starts_with(buffer, "f")) // face
                {
                    Face face = {};
                    math::Vec3i normal_indices = {};
                    math::Vec3i uv_indices = {};

                    if (with_uvs && with_normals)
                    {
                        sscanf(buffer, "f %hd/%d/%d %hd/%d/%d %hd/%d/%d", &face.indices[0], &uv_indices.x, &normal_indices.x, &face.indices[1], &uv_indices.y, &normal_indices.y, &face.indices[2], &uv_indices.z, &normal_indices.z);
                    }
                    else if (with_uvs)
                    {
                        sscanf(buffer, "f %hd/%d %hd/%d %hd/%d", &face.indices[0], &uv_indices.x, &face.indices[1], &uv_indices.y, &face.indices[2], &uv_indices.z);
                    }

                    else if (with_normals)
                    {
                        sscanf(buffer, "f %hd//%d %hd//%d %hd//%d", &face.indices[0], &normal_indices.x, &face.indices[1], &normal_indices.y, &face.indices[2], &normal_indices.z);
                    }

                    // The obj-format was made by geniuses and therefore the indices are not 0-indexed. Such wow.
                    face.indices[0] -= 1;
                    face.indices[1] -= 1;
                    face.indices[2] -= 1;

                    b32 should_add = false;
                    Vertex v1 = vertices[face.indices[0]];
                    math::Vec2 uv1(0.0f);
                    math::Vec3 n1(0.0f);

                    if (with_uvs)
                    {
                        uv1 = uvs[uv_indices.x - 1];
                    }

                    if (with_normals)
                    {
                        n1 = normals[normal_indices.x - 1];
                    }

                    face.indices[0] = (u16)check_for_identical_vertex(v1, uv1, n1, final_vertices, &should_add);

                    if (should_add)
                    {
                        buf_push(final_vertices, v1);
                    }

                    should_add = false;
                    Vertex &v2 = vertices[face.indices[1]];
                    math::Vec2 uv2(0.0f);
                    math::Vec3 n2(0.0f);

                    if (with_uvs)
                    {
                        uv2 = uvs[uv_indices.y - 1];
                    }

                    if (with_normals)
                    {
                        n2 = normals[normal_indices.y - 1];
                    }

                    face.indices[1] = (u16)check_for_identical_vertex(v2, uv2, n2, final_vertices, &should_add);

                    if (should_add)
                    {
                        buf_push(final_vertices, v2);
                    }

                    should_add = false;
                    Vertex &v3 = vertices[face.indices[2]];

                    math::Vec2 uv3(0.0f);
                    math::Vec3 n3(0.0f);

                    if (with_uvs)
                    {
                        uv3 = uvs[uv_indices.z - 1];
                    }

                    if (with_normals)
                    {
                        n3 = normals[normal_indices.z - 1];
                    }

                    face.indices[2] = (u16)check_for_identical_vertex(v3, uv3, n3, final_vertices, &should_add);

                    if (should_add)
                    {
                        buf_push(final_vertices, v3);
                    }

                    buf_push(faces, face);
                }
            }
            fclose(file);

            assert(renderer.mesh_count + 1 < global_max_meshes);

            Mesh mesh;
            mesh.vertices = push_array(&renderer.mesh_arena, buf_len(final_vertices), Vertex);
            mesh.faces = push_array(&renderer.mesh_arena, buf_len(faces), Face);
            mesh.vertex_count = (i32)buf_len(final_vertices);
            mesh.face_count = (i32)buf_len(faces);

            memcpy(mesh.vertices, final_vertices, mesh.vertex_count * sizeof(Vertex));
            memcpy(mesh.faces, faces, mesh.face_count * sizeof(Face));

            buf_free(final_vertices);
            buf_free(vertices);
            buf_free(normals);
            buf_free(uvs);
            buf_free(faces);

            if(scale)
            {
                *scale = math::Vec3(max_x - min_x, max_y - min_y, max_z - min_z); 
            }
            
            // If we specified a material to load the data into
            if (material_handle && has_mtl_file)
            {
                // Find the directory of the file
                size_t index = 0;
                for (size_t i = 0; i < strlen(file_path); i++)
                {
                    if (file_path[i] == '/')
                    {
                        index = i + 1;
                    }
                }

                auto temp_block = begin_temporary_memory(&renderer.temp_arena);

                char *dir = push_string(temp_block.arena, index);
                strncpy(dir, file_path, index);

                dir[index] = 0;
                char *material_file_path = concat(dir, mtl_file_name, &renderer.temp_arena);

                load_material_from_mtl(renderer, *material_handle, material_file_path);

                end_temporary_memory(temp_block);
            }

            return {create_buffers_from_mesh(renderer, mesh, 0, with_normals, with_uvs)};
        }
        else
        {
            error("File not found", file_path);
            return {0};
        }
    }
    
    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, r32 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT);
                va.float_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT);
                u_v.float_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, r32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, math::Vec2 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT2);
                va.float2_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT2);
                u_v.float2_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, math::Vec2 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, math::Vec3 value)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT3);
                u_v.float3_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, math::Vec3 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, math::Vec4 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::FLOAT4);
                va.float4_val = value;
                return;
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT4);
                u_v.float4_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, math::Vec4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, i32 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::INTEGER || va.type == ValueType::BOOL);
                if (va.type == ValueType::BOOL)
                {
                    va.boolean_val = value;
                    return;
                }
                else if (va.type == ValueType::INTEGER)
                {
                    va.integer_val = value;
                    return;
                }
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::INTEGER || u_v.uniform.type == ValueType::BOOL);
                if (u_v.uniform.type == ValueType::BOOL)
                {
                    u_v.boolean_val = value;
                    break;
                }
                else if (u_v.uniform.type == ValueType::INTEGER)
                {
                    u_v.integer_val = value;
                    break;
                }
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, i32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, math::Mat4 value)
    {
        for (i32 i = 0; i < material.instanced_vertex_attribute_count; i++)
        {
            VertexAttribute &va = material.instanced_vertex_attributes[i].attribute;
            if (strcmp(va.name, name) == 0)
            {
                assert(va.type == ValueType::MAT4);
                if (va.type == ValueType::MAT4)
                {
                    va.mat4_val = value;
                    return;
                }
            }
        }

        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::MAT4);
                u_v.mat4_val = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, math::Mat4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, TextureHandle value)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::TEXTURE);
                assert(value.handle != 0);
                u_v.texture = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, TextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_value(Renderer &renderer, Material &material, const char *name, MSTextureHandle value)
    {
        for (i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &u_v = material.uniform_values[i];
            if (strcmp(u_v.uniform.name, name) == 0)
            {
                assert(u_v.uniform.type == ValueType::MS_TEXTURE);
                u_v.ms_texture = value;
                break;
            }
        }
    }

    static void set_uniform_value(Renderer &renderer, MaterialInstanceHandle handle, const char *name, MSTextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_value(renderer, material, name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material &material, const char *array_name, i32 index, const char *variable_name, r32 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];

            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT);
                        u_v.float_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, r32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material &material, const char *array_name, i32 index, const char *variable_name, math::Vec2 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT2);
                        u_v.float2_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec2 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material &material, const char *array_name, i32 index, const char *variable_name, math::Vec3 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT3);
                        u_v.float3_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec3 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material& material, const char *array_name, i32 index, const char *variable_name, math::Vec4 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];
                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::FLOAT4);
                        u_v.float4_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material &material, const char *array_name, i32 index, const char *variable_name, i32 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        if (u_v.uniform.type == ValueType::BOOL)
                        {
                            u_v.boolean_val = value;
                            break;
                        }
                        else if (u_v.uniform.type == ValueType::INTEGER)
                        {
                            u_v.integer_val = value;
                            break;
                        }
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, i32 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void set_uniform_array_value(Renderer &renderer, Material& material, const char *array_name, i32 index, const char *variable_name, math::Mat4 value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::MAT4);
                        u_v.mat4_val = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Mat4 value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

// @Incomplete: Add MSTexture support for arrays
    static void set_uniform_array_value(Renderer &renderer, Material &material, const char *array_name, i32 index, const char *variable_name, rendering::TextureHandle value)
    {
        for (i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if (strcmp(array.name, array_name) == 0)
            {
                UniformEntry &entry = array.entries[index];

                for (i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue &u_v = entry.values[j];
                    if (strcmp(u_v.name, variable_name) == 0)
                    {
                        assert(u_v.uniform.type == ValueType::TEXTURE);
                        u_v.texture = value;
                        break;
                    }
                }
                break;
            }
        }
    }

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, TextureHandle value)
    {
        Material &material = get_material_instance(handle, renderer);
        set_uniform_array_value(renderer, material, array_name, index, variable_name, value);
    }

    static void update_buffer(Renderer &renderer, BufferHandle handle, BufferData new_data)
    {
        // @Incomplete: Update from data
        // Find RegisterBufferInfo from handle
        // Update data in info from new_data
    }

    static void push_shadow_buffer(Renderer &renderer, BufferHandle buffer_handle, Transform &transform)
    {
        ShadowCommand shadow_command = {};
        shadow_command.buffer = buffer_handle;
        shadow_command.transform = transform;
        renderer.render.shadow_commands[renderer.render.shadow_command_count++] = shadow_command;
    }

    static void push_buffer_to_render_pass(Renderer &renderer, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, Transform &transform, ShaderHandle shader_handle, RenderPassHandle render_pass_handle)
    {
        assert(renderer.render.render_command_count < global_max_render_commands);

        RenderCommand render_command = {};
        render_command.buffer = buffer_handle;
        render_command.material = material_instance_handle;
        render_command.transform = transform;
        render_command.pass.shader_handle = shader_handle;
        RenderPass &pass = renderer.render.passes[render_pass_handle.handle - 1];
        pass.commands.render_commands[pass.commands.render_command_count++] = render_command;
    }

    static void push_instanced_buffer_to_render_pass(Renderer &renderer, i32 count, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, ShaderHandle shader_handle, RenderPassHandle render_pass_handle)
    {
        RenderCommand render_command = {};
        render_command.count = count;
        render_command.buffer = buffer_handle;
        render_command.material = material_instance_handle;
        render_command.pass.shader_handle = shader_handle;
        RenderPass &pass = renderer.render.passes[render_pass_handle.handle - 1];
        pass.commands.render_commands[pass.commands.render_command_count++] = render_command;
    }

    static void push_instanced_buffer_to_shadow_pass(Renderer &renderer, i32 count, BufferHandle buffer_handle, VertexAttributeInstanced *instanced_attrs, i32 attr_count)
    {
        ShadowCommand shadow_command = {};
        memcpy(shadow_command.instanced_vertex_attributes, instanced_attrs, attr_count * sizeof(VertexAttributeInstanced));
        shadow_command.instanced_vertex_attribute_count = attr_count;
        shadow_command.count = count;
        shadow_command.buffer = buffer_handle;
        renderer.render.shadow_commands[renderer.render.shadow_command_count++] = shadow_command;
    }

    static math::Vec2 get_relative_size(Renderer &renderer, math::Vec2 size, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec2 scaled_size;

        if (scaling_flags & UIScalingFlag::SCALE_WITH_WIDTH)
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
            scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        }
        else if (scaling_flags & UIScalingFlag::SCALE_WITH_HEIGHT)
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
            scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
        }
        else
        {
            scaled_size.x = (size.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;

            if (scaling_flags & UIScalingFlag::KEEP_ASPECT_RATIO)
            {
                r32 ratio = size.y / size.x;
                scaled_size.y = scaled_size.x * ratio;
            }
            else
            {
                scaled_size.y = (size.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
            }
        }

        return scaled_size;
    }

// @Note Gets info about UI position for rendering things relative to each other
// We often want to be able to render things next to each other perfectly on different scales
// This function should help with that
// Parameters:
// renderer:      The renderer
// position:      The position of the original quad
// relative_size: The size of the original quad
// size:          The size of the thing you want to render next to the original
// relative:      The relative flag (top, bottom, left, right)
// centered:      Whether or not the original quad was centered (need to know this for origin etc.)
// scaling_flags: How do we scale these UI elements?
// origin:        The origin
    static RelativeUIQuadInfo get_relative_info(Renderer &renderer, math::Vec2 position, math::Vec2 relative_size, math::Vec2 size, RelativeFlag relative, b32 centered, u64 scaling_flags = UIScalingFlag::KEEP_ASPECT_RATIO, math::Vec2 origin = math::Vec2(0.0f))
    {
        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec3 pos;
        pos.x = (position.x / UI_COORD_DIMENSION) * resolution_scale.x;
        pos.y = (position.y / UI_COORD_DIMENSION) * resolution_scale.y;
        pos.z = 0.0f;

        pos.x -= origin.x;
        pos.y -= origin.y;

        math::Vec3 scaled_size = math::Vec3(get_relative_size(renderer, relative_size, scaling_flags), 0.0f);

        math::Vec3 relative_pos = math::Vec3(pos.x, pos.y, 0.0f);

        math::Vec3 new_size = math::Vec3(get_relative_size(renderer, size, scaling_flags), 0.0f);

        r32 factor_x = scaled_size.x / origin.x;
        r32 factor_y = scaled_size.y / origin.y;

        if (origin.y == 0.0f)
        {
            factor_y = 1.0f;
        }
        if (origin.x == 0.0f)
        {
            factor_x = 1.0f;
        }

        switch (relative)
        {
        case RELATIVE_TOP:
        {
            relative_pos.y += (i32)scaled_size.y / factor_y;
        }
        break;
        case RELATIVE_LEFT:
        {
            if (origin.x == 0.0f)
            {
                relative_pos.x -= (i32)scaled_size.x / factor_x;
            }
            else
            {
                relative_pos.x -= (i32)scaled_size.x / factor_x + new_size.x;
            }
        }
        break;
        case RELATIVE_RIGHT:
        {
            if (origin.x == 0.0f)
            {
                relative_pos.x += (i32)scaled_size.x / factor_x;
            }
            else
            {
                relative_pos.x += (i32)scaled_size.x / factor_x + scaled_size.x;
            }
        }
        break;
        case RELATIVE_BOTTOM:
        {
            relative_pos.y -= (i32)scaled_size.y / factor_y - scaled_size.y + new_size.y;
        }
        break;
        }

        math::Vec2 ui_position = math::Vec2(0, 0);
        ui_position.x = ((relative_pos.x / (r32)resolution_scale.x) * UI_COORD_DIMENSION);
        ui_position.y = ((relative_pos.y / (r32)resolution_scale.y) * UI_COORD_DIMENSION);

        return {math::Vec2(relative_pos.x, relative_pos.y), math::Vec2(new_size.x, new_size.y), ui_position};
    }

    static void calculate_current_x_from_line_data(r32 *x, math::Vec2 text_size, u64 alignment_flags)
    {
        if (alignment_flags & UIAlignment::RIGHT)
        {
            *x -= text_size.x;
        }
        else if ((alignment_flags & UIAlignment::LEFT) == 0)
        {
            *x -= text_size.x / 2.0f;
        }
    }

    static math::Rect scale_clip_rect(Renderer &renderer, math::Rect clip_rect, u64 ui_scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO)
    {
        math::Vec2i resolution_scale = get_scale(renderer);
        math::Rect scaled_clip_rect;

        scaled_clip_rect.x = (clip_rect.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        scaled_clip_rect.y = (clip_rect.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;

        r32 clip_ratio = clip_rect.height / clip_rect.width;
        scaled_clip_rect.width = (clip_rect.width / UI_COORD_DIMENSION) * (r32)resolution_scale.x;

        if (ui_scaling_flag & UIScalingFlag::KEEP_ASPECT_RATIO)
        {
            scaled_clip_rect.height = scaled_clip_rect.width * clip_ratio;
        }
        else
        {
            scaled_clip_rect.height = (clip_rect.height / UI_COORD_DIMENSION) * (r32)resolution_scale.y;
        }

        return scaled_clip_rect;
    }

    static void generate_text_coordinates(const char *text, TrueTypeFontInfo &font_info, math::Vec3 position, u64 alignment_flags, FramebufferInfo &framebuffer, CharacterData **coords)
    {
        // @Note: Compute the coord buffer
        i32 n = 0;

        LineData line_data = get_line_size_data(text, font_info);

        r32 start_x = position.x;
        r32 y = position.y;
        r32 x = position.x;
        i32 current_line = 0;

        if (alignment_flags & UIAlignment::TOP)
        {
            //y = line_data.total_height;
        }
        else if (alignment_flags & UIAlignment::BOTTOM)
        {
            y += line_data.total_height;
        }
        else
        {
            y -= line_data.line_sizes[0].y * 0.5f;
        }

        y = framebuffer.height - y;

        calculate_current_x_from_line_data(&x, line_data.line_sizes[current_line], alignment_flags);

        for (u32 i = 0; i < strlen(text); i++)
        {
            char c = text[i];

            if (c == '\n')
            {
                current_line++;

                y += font_info.line_height;
                x = start_x;

                if (current_line != line_data.line_count)
                {
                    calculate_current_x_from_line_data(&x, line_data.line_sizes[current_line], alignment_flags);
                }

                continue;
            }

            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(font_info.char_data, font_info.atlas_width, font_info.atlas_height, text[i] - font_info.first_char, &x, &y, &quad, 1);

            r32 x_min = quad.x0;
            r32 x_max = quad.x1;
            r32 y_min = framebuffer.height - quad.y0;
            r32 y_max = framebuffer.height - quad.y1;

            (*coords)[n++] = {x_max, y_max, quad.s1, quad.t1};
            (*coords)[n++] = {x_max, y_min, quad.s1, quad.t0};
            (*coords)[n++] = {x_min, y_min, quad.s0, quad.t0};
            (*coords)[n++] = {x_min, y_max, quad.s0, quad.t1};
            (*coords)[n++] = {x_max, y_max, quad.s1, quad.t1};
            (*coords)[n++] = {x_min, y_min, quad.s0, quad.t0};

            i32 kerning = stbtt_GetCodepointKernAdvance(&font_info.info, text[i] - font_info.first_char, text[i + 1] - font_info.first_char);
            x += (r32)kerning * font_info.scale;
        }
    }

    static void push_text(Renderer &renderer, CreateTextCommandInfo info, const char *text)
    {
        RenderPass &pass = renderer.render.ui.pass;
        TextRenderCommand &command = pass.ui.text_commands[pass.ui.text_command_count];

        command.font = info.font;

        FramebufferInfo *framebuffer = &renderer.render.framebuffers[pass.framebuffer.handle - 1];
        TrueTypeFontInfo &font_info = renderer.tt_font_infos[info.font.handle];

        if (font_info.resolution_loaded_for.width != (i32)framebuffer->width || font_info.resolution_loaded_for.height != (i32)framebuffer->height)
        {
            load_font(renderer, font_info.path, font_info.load_size, info.font);
        }

        framebuffer = &renderer.render.framebuffers[pass.framebuffer.handle - 1];

        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec3 pos;
        pos.x = (info.position.x / UI_COORD_DIMENSION) * resolution_scale.x;
        pos.y = (info.position.y / UI_COORD_DIMENSION) * resolution_scale.y;
        pos.z = 0.0f;

        math::Rect scaled_clip_rect = scale_clip_rect(renderer, info.clip_rect);

        command.clip = info.clip;
        command.clip_rect = scaled_clip_rect;
        command.text_length = strlen(text);

        // @Incomplete: Set material?
        command.material = renderer.render.materials[renderer.render.ui.font_material.handle];

        set_uniform_value(renderer, command.material, "color", info.color);
        set_uniform_value(renderer, command.material, "z", (r32)info.z_layer);
        set_uniform_value(renderer, command.material, "tex", font_info.texture);

        command.shader_handle = command.material.shader;

        CharacterData *coords = pass.ui.coords[pass.ui.text_command_count];

        generate_text_coordinates(text, font_info, pos, info.alignment_flags, *framebuffer, &coords);

        command.buffer = {pass.ui.text_command_count++};
    }

    static void push_buffer_to_ui_pass(Renderer &renderer, BufferHandle buffer_handle, ShaderHandle shader, CreateUICommandInfo info)
    {
        UIRenderCommand render_command = {};
        render_command.buffer = buffer_handle;

        if (info.texture_handle.handle != 0)
        {
            render_command.material = renderer.render.materials[renderer.render.ui.textured_material.handle];
            set_uniform_value(renderer, render_command.material, "tex0", info.texture_handle);
        }
        else
        {
            render_command.material = renderer.render.materials[renderer.render.ui.material.handle];
            set_uniform_value(renderer, render_command.material, "color", info.color);
        }

        Transform transform = {};
        transform.position = math::Vec3(info.position.x, info.position.y, info.z_layer);
        transform.scale = info.scale;
        transform.rotation = info.rotation;

        render_command.transform = transform;
        render_command.shader_handle = render_command.material.shader;
        render_command.clip_rect = info.clip_rect;
        RenderPass &pass = renderer.render.ui.pass;
        pass.ui.render_commands[pass.ui.render_command_count++] = render_command;
    }

    static UpdateBufferInfo create_update_buffer_info(Renderer &renderer, BufferHandle handle)
    {
        i32 internal_handle = renderer.render._internal_buffer_handles[handle.handle - 1];

        RegisterBufferInfo reg_info = renderer.render.buffers[internal_handle];
        UpdateBufferInfo info = {};
        info.buffer = handle;
        info.update_data = reg_info.data;

        return info;
    }

    static CreateUICommandInfo create_ui_command_info()
    {
        CreateUICommandInfo info = {};
        info.position = math::Vec2(0.0f);
        info.rotation = math::Vec3(0.0f);
        info.scale = math::Vec2(0.0f);

        info.z_layer = 0;
        info.color = math::Rgba(1.0f);
        info.clip_rect = math::Rect(0.0f);
        info.clip = true;
        info.anchor_flag = 0;

        info.scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO;
        info.texture_handle = {0};

        return info;
    }

    static CreateTextCommandInfo create_text_command_info()
    {
        CreateTextCommandInfo info = {};
        info.position = math::Vec2(0.0f);
        info.rotation = math::Vec3(0.0f);
        info.scale = math::Vec2(0.0f);

        info.z_layer = 0;
        info.color = math::Rgba(1.0f);
        info.clip_rect = math::Rect(0.0f);
        info.clip = false;
        info.alignment_flags = UIAlignment::LEFT; // @Incomplete: Default to centered instead?

        return info;
    }

    static void push_ui_quad(Renderer &renderer, CreateUICommandInfo info)
    {
        CreateUICommandInfo scaled_info = info;
        math::Vec2i resolution_scale = get_scale(renderer);

        math::Vec2 pos;
        pos.x = (info.position.x / UI_COORD_DIMENSION) * (r32)resolution_scale.x;
        pos.y = (info.position.y / UI_COORD_DIMENSION) * (r32)resolution_scale.y;

        scaled_info.position = pos;
        scaled_info.z_layer = info.z_layer;

        scaled_info.scale = get_relative_size(renderer, info.scale, info.scaling_flag);
        scaled_info.clip_rect = scale_clip_rect(renderer, info.clip_rect, info.scaling_flag);

        scaled_info.rotation = info.rotation;
        scaled_info.color = info.color;
        scaled_info.anchor_flag = info.anchor_flag;

        u64 anchor = info.anchor_flag;

        BufferHandle buffer = {};
        ShaderHandle shader = {};

        if (info.texture_handle.handle != 0)
        {
            if (anchor & UIAlignment::TOP)
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.top_left_textured_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.top_right_textured_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.top_x_centered_textured_quad_buffer;
                }
            }
            else if (anchor & UIAlignment::BOTTOM)
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.bottom_left_textured_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.bottom_right_textured_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.bottom_x_centered_textured_quad_buffer;
                }
            }
            else
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.left_y_centered_textured_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.right_y_centered_textured_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.centered_textured_quad_buffer;
                }
            }
        }
        else
        {
            if (anchor & UIAlignment::TOP)
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.top_left_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.top_right_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.top_x_centered_quad_buffer;
                }
            }
            else if (anchor & UIAlignment::BOTTOM)
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.bottom_left_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.bottom_right_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.bottom_x_centered_quad_buffer;
                }
            }
            else
            {
                if (anchor & UIAlignment::LEFT)
                {
                    buffer = renderer.render.ui.left_y_centered_quad_buffer;
                }
                else if (anchor & UIAlignment::RIGHT)
                {
                    buffer = renderer.render.ui.right_y_centered_quad_buffer;
                }
                else
                {
                    buffer = renderer.render.ui.centered_quad_buffer;
                }
            }
        }

        push_buffer_to_ui_pass(renderer, buffer, shader, scaled_info);
    }
} // namespace rendering
