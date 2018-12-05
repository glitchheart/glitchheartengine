#include <sys/stat.h>

namespace rendering
{
    
	// @Robustness: New shader stuff
    static void error(const char* msg, const char* file)
    {
		debug_log("ERROR: %s in %s", msg, file);
    }

    static ValueType parse_type(char *type, const char *file_path, b32 invalid_is_valid)
    {
        ValueType result;
        
		if(starts_with(type, "float"))
        {
            result = ValueType::FLOAT;
        }
		else if(starts_with(type, "vec2"))
        {
            result = ValueType::FLOAT2;
        }
		else if(starts_with(type, "vec3"))
        {
            result = ValueType::FLOAT3;
        }
		else if(starts_with(type, "vec4"))
        {
            result = ValueType::FLOAT4;
        }
		else if(starts_with(type, "mat4"))
        {
            result = ValueType::MAT4;
        }
		else if(starts_with(type, "bool"))
        {
            result = ValueType::BOOL;
        }
		else if(starts_with(type, "int"))
        {
            result = ValueType::INTEGER;
        }
		else if(starts_with(type, "sampler2D"))
        {
            result = ValueType::TEXTURE;
        }
		else
        {
            result = ValueType::INVALID;

            if(!invalid_is_valid)
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
        while(*source[0] == ' ' || *source[0] == '\n')
            (*source)++;
    }
    
    static ValueType get_value_type(char** value, const char* file_path, b32 invalid_is_valid = false)
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
        
        while(*source[0] != ' ' && *source[0] != '\n' && *source[0] != ';')
        {
            buffer[index++] = **source;
            (*source)++;
        }

        buffer[index] = '\0';
    }

    static i32 get_structure_index(char *type_name, Shader &shader)
    {
        for(i32 i = 0; i < shader.structure_count; i++)
        {
            if(strncmp(type_name, shader.structures[i].name, strlen(type_name)) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    static Structure *get_structure(char *type_name, Shader &shader)
    {
        for(i32 i = 0; i < shader.structure_count; i++)
        {
            if(strncmp(type_name, shader.structures[i].name, strlen(type_name)) == 0)
            {
                return &shader.structures[i];
            }
        }
        return nullptr;
    }

    static DefinedValue *get_defined_value(char *name, Shader &shader)
    {
        for(i32 i = 0; i < shader.defined_value_count; i++)
        {
            if(strcmp(name, shader.defined_values[i].name) == 0)
            {
                return &shader.defined_values[i];
            }
        }
        return nullptr;
    }
    
    static void parse_uniform_array_data(char **rest, Uniform &uniform, Shader &shader)
    {
        if(starts_with(*rest, "["))
        {
            uniform.is_array = true;
            char array_size[32];
            sscanf(*rest, "[%[^]]]", array_size);

            if(DefinedValue *defined_value = get_defined_value(array_size, shader))
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
                    
        while(rest[0] == ' ')
            rest++;
        
        sscanf(rest, "%[^[ \n]", uniform.name);

        rest += strlen(uniform.name);

        parse_uniform_array_data(&rest, uniform, shader);
                                
        if(uniform.name[strlen(uniform.name) - 1] == ';')
        {
            uniform.name[strlen(uniform.name) - 1] = '\0';
        }
        else
        {
            i32 c = 0;
            i32 m_c = 0;
            char mapped_buffer[32];
		    
            while(rest[c] != ';' && rest[c] != '\n')
            {
                if(rest[c] != ':' && rest[c] != ' ')
                {
                    mapped_buffer[m_c++] = rest[c];
                }
                c++;
            }
                    
            if(m_c > 0)
            {
                //@Note: Check for correct indices
                original_buffer[strlen(original_buffer) - strlen(rest) + 3] = '\0';
                original_buffer[strlen(original_buffer) - strlen(rest) + 2] = '\n';
                original_buffer[strlen(original_buffer) - strlen(rest) + 1] = ';';
                mapped_buffer[m_c] = '\0';

                if(starts_with(mapped_buffer, "DIFFUSE_TEX"))
                {
                    uniform.mapping_type = UniformMappingType::DIFFUSE_TEX;
                }
                else if(starts_with(mapped_buffer, "DIFFUSE_COLOR"))
                {
                    uniform.mapping_type = UniformMappingType::DIFFUSE_COLOR;
                }
                else if(starts_with(mapped_buffer, "SPECULAR_TEX"))
                {
                    uniform.mapping_type = UniformMappingType::SPECULAR_TEX;
                }
                else if(starts_with(mapped_buffer, "SPECULAR_COLOR"))
                {
                    uniform.mapping_type = UniformMappingType::SPECULAR_COLOR;
                }
                else if(starts_with(mapped_buffer, "SPECULAR_EXPONENT"))
                {
                    uniform.mapping_type = UniformMappingType::SPECULAR_EXPONENT;
                }
                else if(starts_with(mapped_buffer, "AMBIENT_COLOR"))
                {
                    uniform.mapping_type = UniformMappingType::AMBIENT_COLOR;
                }
                else if(starts_with(mapped_buffer, "AMBIENT_TEX"))
                {
                    uniform.mapping_type = UniformMappingType::AMBIENT_TEX;
                }
                else if(starts_with(mapped_buffer, "SHADOW_MAP"))
                {
                    uniform.mapping_type = UniformMappingType::SHADOW_MAP;
                }
                else if(starts_with(mapped_buffer, "MODEL"))
                {
                    uniform.mapping_type = UniformMappingType::MODEL;
                }
                else if(starts_with(mapped_buffer, "VIEW"))
                {
                    uniform.mapping_type = UniformMappingType::VIEW;
                }
                else if(starts_with(mapped_buffer, "PROJECTION"))
                {
                    uniform.mapping_type = UniformMappingType::PROJECTION;
                }
                else if(starts_with(mapped_buffer, "CAMERA_POSITION"))
                {
                    uniform.mapping_type = UniformMappingType::CAMERA_POSITION;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHTS"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHTS;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHTS"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHTS;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_COUNT"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHT_COUNT;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_COUNT"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_COUNT;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_DIRECTION"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHT_DIRECTION;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_AMBIENT"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHT_AMBIENT;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_DIFFUSE"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHT_DIFFUSE;
                }
                else if(starts_with(mapped_buffer, "DIRECTIONAL_LIGHT_SPECULAR"))
                {
                    uniform.mapping_type = UniformMappingType::DIRECTIONAL_LIGHT_SPECULAR;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_POSITION"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_POSITION;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_CONSTANT"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_CONSTANT;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_LINEAR"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_LINEAR;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_QUADRATIC"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_QUADRATIC;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_AMBIENT"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_AMBIENT;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_DIFFUSE"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_DIFFUSE;
                }
                else if(starts_with(mapped_buffer, "POINT_LIGHT_SPECULAR"))
                {
                    uniform.mapping_type = UniformMappingType::POINT_LIGHT_SPECULAR;
                }
                else
                {
                    char error_buf[256];
                    sprintf(error_buf, "Found invalid uniform mapping type '%s'", mapped_buffer);
                    error(error_buf, file_path);
                }
            }
        }

        return(uniform);
    }
    
    static void parse_structure_variables(char **source, char *total_buffer, Structure &structure, Shader &shader, const char *file_path)
    {
        size_t i = 0;
        
        char buffer[256];

        while(read_line(buffer, 256, source))
        {
            b32 should_break = false;
            
            char *rest = &buffer[0];
            eat_spaces_and_newlines(&rest);

            if(starts_with(rest, "struct"))
            {
                sscanf(rest, "struct %[^\n]", structure.name);
            }
            else if(starts_with(rest, "}"))
            {
                should_break = true;
            }
            else if(starts_with(rest, "{"))
            {}
            else
            {
                // GET TYPE
                ValueType type = get_value_type(&rest, file_path);
                if(type != ValueType::INVALID)
                {
                    Uniform uniform = parse_uniform(type, rest, buffer, shader, file_path);
                    structure.uniforms[structure.uniform_count++] = uniform;
                }
            }

            strncpy(&total_buffer[i], buffer, strlen(buffer));
            i += strlen(buffer);

            if(should_break)
                break;
        }

        total_buffer[i++] = '\0';
    }

    static char* load_shader_text(MemoryArena* arena, char* source, Shader& shader, Uniform **uniforms_array, i32 *uniform_count, const char* file_path, size_t* file_size = nullptr)
    {
		size_t i = 0;

		MemoryArena temp_arena = {};
		auto temp_mem = begin_temporary_memory(&temp_arena);

		char* result;

		size_t temp_current_size = strlen(source) * 10;
		char* temp_result = push_string(&temp_arena, temp_current_size);

		char buffer[256];

		while(read_line(buffer, 256, &source))
        {
            if(starts_with(buffer, "#vert") || starts_with(buffer, "#frag"))
            {
                break;
            }
            else if(starts_with(buffer, "#include \""))
            {
                // @Note: Game shaders are included with ""
                char include_name[256];
                sscanf(buffer, "#include \"%s\"", include_name);

                char* included_path = concat("../assets/shaders/", include_name, &temp_arena);
				
                FILE* included_shd = fopen(included_path, "r");

                if(included_shd)
                {
                    char* included_source = read_file_into_buffer(&temp_arena, included_shd);
                    char* path = concat(concat(file_path, "<-", &temp_arena), included_path, &temp_arena);
					
                    char* included_text = load_shader_text(&temp_arena, included_source, shader, uniforms_array, uniform_count, path);

                    if(i + strlen(buffer) > temp_current_size)
                    {
                        error("Temp buffer is too small", file_path);
                    }
					
                    strncpy(&temp_result[i], included_text, strlen(included_text));
                    i += strlen(included_text);
                    if(file_size)
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
            else if(starts_with(buffer, "#include <"))
            {
                // @Note: Engine shaders are included with <>
                char include_name[256];
                sscanf(buffer, "#include <%[^>]", include_name);
                char* included_path = concat("../engine_assets/standard_shaders/", include_name, &temp_arena);
				
                FILE* included_shd = fopen(included_path, "r");

                if(included_shd)
                {
                    char* included_source = read_file_into_buffer(&temp_arena, included_shd);
                    char* path = concat(concat(file_path, "<-", &temp_arena), included_path, &temp_arena);
					
                    char* included_text = load_shader_text(&temp_arena, included_source, shader, uniforms_array, uniform_count, path);
                    
                    if(i + strlen(buffer) > temp_current_size)
                    {
                        error("Temp buffer is too small", file_path);
                    }
					
                    strncpy(&temp_result[i], included_text, strlen(included_text));
                    i += strlen(included_text);
                    
                    if(file_size)
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
            else if(starts_with(buffer, "#define"))
            {
                // @Incomplete: float defines missing
                DefinedValue defined_value = {};
                defined_value.type = DefinedValueType::INTEGER;
                sscanf(buffer, "#define %s %d", defined_value.name, &defined_value.integer_val);
                shader.defined_values[shader.defined_value_count++] = defined_value;
            }
            else if(starts_with(buffer, "struct"))
            {
                source -= strlen(buffer);

                char total_buffer[1024];
                Structure &structure = shader.structures[shader.structure_count++];
                parse_structure_variables(&source, total_buffer, structure, shader, file_path);
                
                strncpy(&temp_result[i], total_buffer, strlen(total_buffer));
                i += strlen(total_buffer);
                    
                continue;
            }
            else if(starts_with(buffer, "uniform"))
            {
                char* rest = &buffer[strlen("uniform") + 1];
                char *prev = rest;
                
                ValueType type = get_value_type(&rest, file_path, true);
                
                if(type == ValueType::INVALID) // We might have a struct
                {
                    char name[32];
                    sscanf(prev, "%[^ ]", name);
                    
                    i32 structure_index = get_structure_index(name, shader);
                    
                    if(structure_index < 0)
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
            else if(starts_with(buffer, "layout"))
            {
                VertexAttribute& vertex_attribute = shader.vertex_attributes[shader.vertex_attribute_count++];
                char* rest = &buffer[strlen("layout") + 1];
		
                while(rest[0] == ' ' || rest[0] == '(')
                {
                    rest++;
                }

                if(starts_with(rest, "location"))
                {
                    rest += strlen("location");
                    // @Note: Eat spaces until we see =
                    while(rest[0] == ' ')
                    {
                        rest++;
                    }

                    if(rest[0] == '=')
                    {
                        rest++;
                        vertex_attribute.location = strtol(rest, &rest, 10);

                        while(rest[0] == ' ' || rest[0] == ')')
                            rest++;

                        if(starts_with(rest, "in"))
                        {
                            while(rest[0] == ' ')
                                rest++;

                            rest += 2;
			    
                            vertex_attribute.type = get_value_type(&rest, file_path);
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

            if(i + strlen(buffer) > temp_current_size)
            {
                error("Temp buffer is too small", file_path);
            }
			
            strncpy(&temp_result[i], buffer, strlen(buffer));

            i += strlen(buffer);
            if(file_size)
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
    
        if(last_loaded != 0 && last_loaded < time_new)
        {
            return true;
        }

        return false;
    }

    static void set_last_loaded(Shader &shader)
    {
        struct stat sb1;
        stat(shader.path, &sb1);
        shader.last_loaded =  sb1.st_mtime;
    }

    static void check_for_shader_file_changes(Renderer &renderer)
    {
        for(i32 i = 0; i < renderer.render.shader_count; i++)
        {
            if(check_dirty(renderer.render.shaders[i]))
            {
                renderer.render.shaders_to_reload[renderer.render.shaders_to_reload_count++] = i;
            }
        }
    }

    static void load_shader(Renderer &renderer, Shader &shader)
    {
		FILE* file = fopen(shader.path, "r");

        shader.loaded = false;
        
		if(file)
        {
            size_t size = 0;
            char* source = read_file_into_buffer(&renderer.shader_arena, file, &size);
	    
            shader.vert_shader = nullptr;
            shader.frag_shader = nullptr;
           
            Uniform *uniforms = (Uniform*)malloc(sizeof(Uniform) * 512);
            i32 uniform_count = 0;
            
            for(size_t i = 0; i < size; i++)
            {
                if(starts_with(&source[i], "#vert"))
                {
                    shader.vert_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#vert") + 1], shader, &uniforms, &uniform_count, shader.path, &i);
                }
                else if(starts_with(&source[i], "#frag"))
                {
                    shader.frag_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#frag") + 1], shader, &uniforms, &uniform_count, shader.path, &i);
                }
            }

            shader.uniforms = nullptr;
            shader.uniform_count = 0;
        
            shader.uniforms = push_array(&shader.arena, uniform_count, Uniform);
            shader.uniform_count = (i32)uniform_count;
            memcpy(shader.uniforms, uniforms, uniform_count * sizeof(Uniform));
            free(uniforms);

            // #if DEBUG
            // 			FILE* shd = fopen("../out.shd", "w");
            // 			fwrite(shader.vert_shader, strlen(shader.vert_shader), 1, shd);
            // 			fwrite(shader.frag_shader, strlen(shader.frag_shader), 1, shd);
            // 			fclose(shd);
            // #endif
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
    
    static ShaderHandle load_shader(Renderer& renderer, const char* file_path)
    {
		assert(renderer.render.shader_count + 1 < 64);
		Shader& shader = renderer.render.shaders[renderer.render.shader_count];
        strncpy(shader.path, file_path, strlen(file_path));
                
        load_shader(renderer, shader);
		return { renderer.render.shader_count++ };
	}
    
    static void set_fallback_shader(Renderer &renderer, const char* path)
    {
        renderer.render.fallback_shader = load_shader(renderer, path);
    }
    
    static UniformValue *get_array_variable_mapping(MaterialInstanceHandle handle, const char *array_name, UniformMappingType type, Renderer &renderer)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            if(strcmp(material.arrays[i].name, array_name) == 0)
            {
                UniformArrayEntry &first_entry = material.arrays[i].entries[0];
                
                for(i32 j = 0; j < first_entry.value_count; j++)
                {
                    if(first_entry.values[j].uniform.mapping_type == type)
                    {
                        return &first_entry.values[j];
                    }
                }
                break;
            }
        }

		return nullptr;
    }

	static UniformValue* mapping(Material& material, UniformMappingType type)
	{
		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            if(material.uniform_values[i].uniform.mapping_type == type)
            {
                return &material.uniform_values[i];
            }
        }

		return nullptr;
	}

    static UniformValue *get_mapping(MaterialInstanceHandle handle, UniformMappingType type, Renderer &renderer)
    {
        Material &material = renderer.render.material_instances[handle.handle];
        return mapping(material, type);
    }

    static UniformValue* get_value(Material& material, ValueType type, const char *name)
	{
		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue &value = material.uniform_values[i];
            
            if(value.uniform.type == type && strcmp(value.uniform.name, name) == 0)
            {
                return &value;
            }
        }

		return nullptr;
	}

    static void set_shader_values(Material &material, Shader &shader, Renderer &renderer)
    {
        UniformValue *uniform_vals = nullptr;
        UniformArray *arrays = nullptr;
        
		// @Incomplete: Get shader uniforms
		for(i32 uni_i = 0; uni_i < shader.uniform_count; uni_i++)
        {
            Uniform u = shader.uniforms[uni_i];

            // If we're working with an array type we should add all the possible uniforms for the whole array
            if(u.is_array)
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
                
                if(u.type == ValueType::STRUCTURE)
                {
                    Structure &structure = shader.structures[u.structure_index];

                    i32 num_to_allocate = u.array_size;
                    array.entries = push_array(&renderer.mesh_arena, num_to_allocate, UniformArrayEntry);
                    
                    if(u.mapping_type == UniformMappingType::DIRECTIONAL_LIGHTS || u.mapping_type == UniformMappingType::POINT_LIGHTS)
                    {
                        material.lighting.receives_light = true;
                    }
                    
                    for(i32 i = 0; i < u.array_size; i++)
                    {
                        UniformArrayEntry entry = {};
                        entry.value_count = 0;
                        
                        for(i32 j = 0; j < structure.uniform_count; j++)
                        {
                            Uniform struct_uni = structure.uniforms[j];
                            Uniform new_uniform = struct_uni;
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
                    array.entries = push_array(&renderer.mesh_arena, num_to_allocate, UniformArrayEntry);
                    
                    for(i32 i = 0; i < u.array_size; i++)
                    {
                        UniformArrayEntry entry = {};
                        entry.value_count = 0;
                        
                        UniformValue u_v = {};
                        Uniform new_uniform = u;
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
                buf_push(uniform_vals, u_v);
            }
        }
        
        size_t size = buf_len(uniform_vals);
        size_t array_size = buf_len(arrays);
        
        if(!material.uniform_values)
        {
            material.uniform_values = push_array(&renderer.mesh_arena, size, UniformValue);
        }

        if(!material.arrays)
        {
            material.arrays = push_array(&renderer.mesh_arena, array_size, UniformArray);
        }

        material.uniform_value_count = (i32)size;
        memcpy(material.uniform_values, uniform_vals, size * sizeof(UniformValue));
        material.array_count = (i32)array_size;
        memcpy(material.arrays, arrays, array_size * sizeof(UniformValue));
    }

	static void set_old_material_values(Material &new_material, Material &old_material)
	{
		for(i32 uniform_index = 0; uniform_index < new_material.uniform_value_count; uniform_index++)
        {
            UniformValue &value = new_material.uniform_values[uniform_index];
                
            if(UniformValue *old_value = get_value(old_material, value.uniform.type, value.uniform.name))
            {
                switch(value.uniform.type)
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
                default:
                assert(false);
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
        for(i32 i = 0; i < renderer.render.material_count; i++)
        {
            Material &material = renderer.render.materials[i];
            Material new_material = {};
            get_updated_material(&new_material, material, shader, renderer);
            renderer.render.materials[i] = new_material;
        }

        for(i32 i = 0; i < renderer.render.material_instance_count; i++)
        {
            Material &material = renderer.render.material_instances[i];
            Material new_material = {};
            get_updated_material(&new_material, material, shader, renderer);
            renderer.render.materials[i] = new_material;
        }
    }
    
	static MaterialHandle create_material(Renderer& renderer, ShaderHandle shader_handle)
	{
		Material& material = renderer.render.materials[renderer.render.material_count];
		material.shader = shader_handle;

        Shader& shader = renderer.render.shaders[shader_handle.handle];
        
        set_shader_values(material, shader, renderer);
		
		return { renderer.render.material_count++ };
	}

    static Material *get_material(MaterialInstanceHandle instance_handle, Renderer &renderer)
    {
        return &renderer.render.material_instances[instance_handle.handle];
    }

	static void load_texture(const char* full_texture_path, Renderer& renderer, TextureFiltering filtering, i32* handle = 0)
	{
		TextureData* texture_data = &renderer.texture_data[renderer.texture_count];
		texture_data->filtering = filtering;
		texture_data->handle = renderer.texture_count++;
    
		PlatformFile png_file = platform.open_file(full_texture_path, POF_READ | POF_OPEN_EXISTING | POF_IGNORE_ERROR);
    
		if(png_file.handle)
        {
            platform.seek_file(png_file, 0, SO_END);
            auto size = platform.tell_file(png_file);
            platform.seek_file(png_file, 0, SO_SET);
        
            auto temp_mem = begin_temporary_memory(&renderer.texture_arena);
            auto tex_data = push_size(&renderer.texture_arena, size + 1, stbi_uc);
            platform.read_file(tex_data, size, 1, png_file);
        
            texture_data->image_data = stbi_load_from_memory(tex_data, size, &texture_data->width, &texture_data->height, 0, STBI_rgb_alpha);
            platform.close_file(png_file);
            end_temporary_memory(temp_mem);
        }
    
		if(!texture_data->image_data)
        {
            printf("Texture could not be loaded: %s\n", full_texture_path);
        }
    
		if(handle)
			*handle = texture_data->handle + 1; // We add one to the handle, since we want 0 to be an invalid handle
	}


	static void load_material_from_mtl(Renderer& renderer, MaterialHandle material_handle, const char* file_path)
	{
		// @Incomplete: We need a better way to do this!
		// Find the directory of the file
		size_t index = 0;
		for(size_t i = 0; i < strlen(file_path); i++)
        {
            if(file_path[i] == '/')
            {
                index = i + 1;
            }
        }

    	TemporaryMemory temp_block = begin_temporary_memory(&renderer.temp_arena);
    
		char *dir = push_string(temp_block.arena, index);
		strncpy(dir, file_path, index);
    
		dir[index] = 0;
		FILE* file = fopen(file_path, "r");

		if(file)
        {
            char buffer[256];

            Material& material = renderer.render.materials[material_handle.handle];			

            while(fgets(buffer, sizeof(buffer), file))
            {
                if(starts_with(buffer, "newmtl"))
                {
                    // @Incomplete: Save name
                }
                else if(starts_with(buffer, "illum")) // illumination
                {
                }
                else if(starts_with(buffer, "Ka")) // ambient color
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::AMBIENT_COLOR))
                    {
                        sscanf(buffer, "Ka %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if(starts_with(buffer, "Kd")) // diffuse color
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::DIFFUSE_COLOR))
                    {
                        sscanf(buffer, "Kd %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if(starts_with(buffer, "Ks")) // specular color
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::SPECULAR_COLOR))
                    {
                        sscanf(buffer, "Ks %f %f %f", &u->float4_val.r, &u->float4_val.g, &u->float4_val.b);
                        u->float4_val.a = 1.0f;
                    }
                }
                else if(starts_with(buffer, "Ns")) // specular exponent
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::SPECULAR_EXPONENT))
                    {
                        sscanf(buffer, "Ns %f", &u->float_val);
                    }
                }
                else if(starts_with(buffer, "d"))
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::DISSOLVE))
                    {
                        sscanf(buffer, "d %f", &u->float_val);
                    }
                }
                else if(starts_with(buffer, "map_Ka")) // ambient map
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::AMBIENT_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ka %s", name);
						
                        if(name[0] == '.')
                            load_texture(name, renderer, LINEAR, &u->texture.handle);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, &u->texture.handle);
                    }
                }
                else if(starts_with(buffer, "map_Kd")) // diffuse map
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::DIFFUSE_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Kd %s", name);
						
                        if(name[0] == '.')
                            load_texture(name, renderer, LINEAR, &u->texture.handle);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, &u->texture.handle);
                    }
                }
                else if(starts_with(buffer, "map_Ks")) // specular map
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::SPECULAR_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ks %s", name);
						
                        if(name[0] == '.')
                            load_texture(name, renderer, LINEAR, &u->texture.handle);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, &u->texture.handle);
                    }
                }
                else if(starts_with(buffer, "map_Ns")) // specular intensity map
                {
                    if(UniformValue* u = mapping(material, UniformMappingType::SPECULAR_INTENSITY_TEX))
                    {
                        char name[64];
                        sscanf(buffer, "map_Ns %s", name);
						
                        if(name[0] == '.')
                            load_texture(name, renderer, LINEAR, &u->texture.handle);
                        else
                            load_texture(concat(dir, name, temp_block.arena), renderer, LINEAR, &u->texture.handle);
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

	static RegisterBufferInfo create_register_buffer_info()
	{
		RegisterBufferInfo info = {};
		info.vertex_attribute_count = 0;
        info.stride = 0;
		return info;
	}

    static size_t size_for_type(ValueType type)
    {
        switch(type)
        {
        default:
        case ValueType::INVALID:
        case ValueType::TEXTURE:
        assert(false);
        case ValueType::FLOAT:
        return sizeof(r32);
        case ValueType::FLOAT2:
        return sizeof(r32) * 2;
        case ValueType::FLOAT3:
        return sizeof(r32) * 3;
        case ValueType::INTEGER:
        case ValueType::BOOL:
        return sizeof(i32);
        case ValueType::MAT4:
        return sizeof(r32) * 16;
        }
    }

    static void add_vertex_attrib(ValueType type, RegisterBufferInfo &info)
    {
        VertexAttribute attribute = {};
        attribute.type = type;
        info.vertex_attributes[info.vertex_attribute_count++] = attribute;
        
        info.stride += size_for_type(type);
    }

	static void generate_vertex_buffer(r32* vertex_buffer, Vertex* vertices, i32 vertex_count, i32 vertex_size, b32 has_normals, b32 has_uvs)
	{
		i32 vertex_data_count = vertex_size;
    
		for(i32 i = 0; i < vertex_count; i++)
        {
            i32 increment_by = 1;
            i32 base_index = i * vertex_data_count;
            Vertex vertex = vertices[i];
            vertex_buffer[base_index] = vertex.position.x;
            vertex_buffer[base_index + increment_by++] = vertex.position.y;
            vertex_buffer[base_index + increment_by++] = vertex.position.z;
        
            if(has_normals)
            {
                vertex_buffer[base_index + increment_by++] = vertex.normal.x;
                vertex_buffer[base_index + increment_by++] = vertex.normal.y;
                vertex_buffer[base_index + increment_by++] = vertex.normal.z;
            }
        
            if(has_uvs)
            {
                vertex_buffer[base_index + increment_by++] = vertex.uv.x;
                vertex_buffer[base_index + increment_by++] = vertex.uv.y;
            }
        }
	}

	static void generate_index_buffer(u16* index_buffer, Face* faces, i32 face_count)
	{
		i32 face_data_count = 3;
    
		for(i32 i = 0; i < face_count; i++)
        {
            i32 base_index = i * face_data_count;
            Face face = faces[i];
            index_buffer[base_index] = face.indices[0];
            index_buffer[base_index + 1] = face.indices[1];
            index_buffer[base_index + 2] = face.indices[2];
        }
	}

	static i32 _find_unused_handle(Renderer& renderer)
	{
		for(i32 index = renderer.render._current_internal_buffer_handle; index < global_max_custom_buffers; index++)
        {
            if(renderer.render._internal_buffer_handles[index] == -1)
            {
                renderer.render._current_internal_buffer_handle = index;
                return index;
            }
        }
    
		for(i32 index = 0; index < global_max_custom_buffers; index++)
        {
            if(renderer.render._internal_buffer_handles[index] == -1)
            {
                renderer.render._current_internal_buffer_handle = index;
                return index;
            }
        }
		
		assert(false);
    
		return -1;
	}

	static BufferHandle register_buffer(Renderer& renderer, RegisterBufferInfo info)
	{
		assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);
		assert(renderer.render._internal_buffer_handles);
    
		i32 unused_handle = _find_unused_handle(renderer) + 1;
    
		renderer.render._internal_buffer_handles[unused_handle - 1] = renderer.render.buffer_count++;

		renderer.render.buffers[renderer.render._internal_buffer_handles[unused_handle - 1]] = info;

		return { unused_handle };
	}

	static BufferHandle create_buffers_from_mesh(Renderer& renderer, Mesh& mesh, u64 vertex_data_flags, b32 has_normals, b32 has_uvs)
	{
		assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);
		i32 vertex_size = 3;

		RegisterBufferInfo info = create_register_buffer_info();

        add_vertex_attrib(ValueType::FLOAT3, info);
        
		if(has_normals)
        {
            add_vertex_attrib(ValueType::FLOAT3, info);
            vertex_size += 3;
        }

		if(has_uvs)
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
		
		return { register_buffer(renderer, info).handle };
	}

    
    static BufferHandle create_plane(Renderer &renderer)
    {
        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer.mesh_arena, sizeof(plane_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer.mesh_arena, sizeof(plane_indices) / sizeof(u16) / 3, Face);
    
        mesh.vertex_count = sizeof(plane_vertices) / sizeof(r32) / 3;
    
        for(i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(plane_vertices[i * 3], plane_vertices[i * 3 + 1], plane_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(plane_normals[i * 3], plane_normals[i * 3 + 1], plane_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(plane_uvs[i * 2], plane_uvs[i * 2 + 1]);
        }
    
        mesh.face_count = sizeof(plane_indices) / sizeof(u16) / 3;
    
        for(i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];
        
            face.indices[0] = plane_indices[i * 3];
            face.indices[1] = plane_indices[i * 3 + 1];
            face.indices[2] = plane_indices[i * 3 + 2];
        }
        
        return { create_buffers_from_mesh(renderer, mesh, 0, true, true) };
    }
    
    static BufferHandle create_cube(Renderer &renderer)
    {
        Mesh mesh;
        mesh = {};
        mesh.vertices = push_array(&renderer.mesh_arena, sizeof(cube_vertices) / sizeof(r32) / 3, Vertex);
        mesh.faces = push_array(&renderer.mesh_arena, sizeof(cube_indices) / sizeof(u16) / 3, Face);
    
        mesh.vertex_count = sizeof(cube_vertices) / sizeof(r32) / 3;
    
        for(i32 i = 0; i < mesh.vertex_count; i++)
        {
            Vertex &vertex = mesh.vertices[i];
            vertex.position = math::Vec3(cube_vertices[i * 3], cube_vertices[i * 3 + 1], cube_vertices[i * 3 + 2]);
            vertex.normal = math::Vec3(cube_normals[i * 3], cube_normals[i * 3 + 1], cube_normals[i * 3 + 2]);
            vertex.uv = math::Vec2(cube_uvs[i * 2], cube_uvs[i * 2 + 1]);
        }
    
        mesh.face_count = sizeof(cube_indices) / sizeof(u16) / 3;
    
        for(i32 i = 0; i < mesh.face_count; i++)
        {
            Face &face = mesh.faces[i];
        
            face.indices[0] = cube_indices[i * 3];
            face.indices[1] = cube_indices[i * 3 + 1];
            face.indices[2] = cube_indices[i * 3 + 2];
        }

        return { create_buffers_from_mesh(renderer, mesh, 0, true, true) };
    }


	static b32 vertex_equals(Vertex &v1, Vertex &v2)
	{
		return v1.position.x == v2.position.x && v1.position.y == v2.position.y && v1.position.z == v2.position.z && v1.uv.x == v2.uv.x && v1.uv.y == v2.uv.y && v1.normal.x == v2.normal.x && v1.normal.y == v2.normal.y && v1.normal.z == v2.normal.z;
    
	}

	static i32 check_for_identical_vertex(Vertex &vertex, math::Vec2 uv, math::Vec3 normal, Vertex *final_vertices, b32* should_add)
	{
		size_t current_size = buf_len(final_vertices);
		vertex.uv = uv;
		vertex.normal = normal;
    
		for(size_t index = 0; index < current_size; index++)
        {
            Vertex &existing = final_vertices[index];
        
            if(vertex_equals(existing, vertex))
            {
                return (i32)index;
            }
        }
    
		*should_add = true;
    
		return (i32)current_size;
	}

	static BufferHandle load_obj(Renderer& renderer, char* file_path, MaterialHandle *material_handle)
	{
		FILE* file = fopen(file_path, "r");

		b32 with_uvs = false;
		b32 with_normals = false;
		
		Vertex* vertices = nullptr;
		math::Vec3 *normals = nullptr;
		math::Vec2 *uvs = nullptr;

		Vertex *final_vertices = nullptr;

		Face *faces = nullptr;

		i32 vert_index = 0;
		i32 normal_index = 0;
		i32 uv_index = 0;

		// Right now we only support one mtl-file per obj-file
		// And since we only support one mesh per obj-file at the moment that should be fine.
		// @Robustness: We have to support more advanced files later... Maybe...
		b32 has_mtl_file = false;
		char mtl_file_name[32];

		if(file)
        {
            char buffer[256];
        
            while((fgets(buffer, sizeof(buffer), file) != NULL))
            {
                if(starts_with(buffer, "g")) // we're starting with new geometry
                {
                    // @Incomplete: Save the name of the geometry
                }
                else if(starts_with(buffer, "mtllib")) // Material file
                {
                    // Read the material file-name
                    sscanf(buffer, "mtllib %s", mtl_file_name);
                }
                else if(starts_with(buffer, "usemtl")) // Used material for geometry
                {
                    has_mtl_file = true;
                    // Ignored, for now.
                    // This is only relevant when we've got multiple materials
                }
                else if(starts_with(buffer, "v ")) // vertex
                {
                    Vertex vertex = {};
                    sscanf(buffer, "v %f %f %f", &vertex.position.x, &vertex.position.y, &vertex.position.z);
                    buf_push(vertices, vertex);
                    vert_index++;
                }
                else if(starts_with(buffer, "vn")) // vertex normal
                {
                    with_normals = true;
                    math::Vec3 normal(0.0f);
                    sscanf(buffer, "vn %f %f %f", &normal.x, &normal.y, &normal.z);
                    buf_push(normals, normal);
                    normal_index++;
                }
                else if(starts_with(buffer, "vt")) // vertex uv
                {
                    with_uvs = true;
                    math::Vec2 uv(0.0f);
                    sscanf(buffer, "vt %f %f", &uv.x, &uv.y);
                    uv.y = 1.0f - uv.y;
                    buf_push(uvs, uv);
                    uv_index++;
                }
                else if(starts_with(buffer, "f")) // face
                {
                    Face face = {};
                    math::Vec3i normal_indices = {};
                    math::Vec3i uv_indices = {};
                
                    if(with_uvs && with_normals)
                    {
                        sscanf(buffer, "f %hd/%d/%d %hd/%d/%d %hd/%d/%d", &face.indices[0], &uv_indices.x, &normal_indices.x, &face.indices[1], &uv_indices.y, &normal_indices.y, &face.indices[2], &uv_indices.z, &normal_indices.z);
                    }
                    else if(with_uvs)
                    {
                        sscanf(buffer, "f %hd/%d %hd/%d %hd/%d", &face.indices[0], &uv_indices.x, &face.indices[1], &uv_indices.y, &face.indices[2], &uv_indices.z);
                    }
                
                    else if(with_normals)
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
                
                    if(with_uvs)
                    {
                        uv1 = uvs[uv_indices.x - 1];
                    }
                
                    if(with_normals)
                    {
                        n1 = normals[normal_indices.x - 1];
                    }
                
                    face.indices[0] = (u16)check_for_identical_vertex(v1, uv1, n1, final_vertices, &should_add);
                
                    if(should_add)
                    {
                        buf_push(final_vertices, v1);
                    }
                
                    should_add = false;
                    Vertex &v2 = vertices[face.indices[1]];
                    math::Vec2 uv2(0.0f);
                    math::Vec3 n2(0.0f);
                
                    if(with_uvs)
                    {
                        uv2 = uvs[uv_indices.y - 1];
                    }
                
                    if(with_normals)
                    {
                        n2 = normals[normal_indices.y - 1];
                    }
                
                    face.indices[1] = (u16)check_for_identical_vertex(v2, uv2, n2, final_vertices, &should_add);
                
                    if(should_add)
                    {
                        buf_push(final_vertices, v2);
                    }
                
                    should_add = false;
                    Vertex &v3 = vertices[face.indices[2]];
                
                    math::Vec2 uv3(0.0f);
                    math::Vec3 n3(0.0f);
                
                    if(with_uvs)
                    {
                        uv3 = uvs[uv_indices.z - 1];
                    }
                
                    if(with_normals)
                    {
                        n3 = normals[normal_indices.z - 1];
                    }
                
                    face.indices[2] = (u16)check_for_identical_vertex(v3, uv3, n3, final_vertices,  &should_add);
                
                    if(should_add)
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

            // If we specified a material to load the data into
            if(material_handle && has_mtl_file)
            {
                // Find the directory of the file
                size_t index = 0;
                for(size_t i = 0; i < strlen(file_path); i++)
                {
                    if(file_path[i] == '/')
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

            return { create_buffers_from_mesh(renderer, mesh, 0, with_normals, with_uvs) };

        }
        else
        {
            error("File not found", file_path);
            return { 0 };
        }
	}
    
	static MaterialInstanceHandle create_material_instance(Renderer& renderer, MaterialHandle material_handle)
	{
		Material& material = renderer.render.materials[material_handle.handle];

        renderer.render.material_instances[renderer.render.material_instance_count] = material;
        renderer.render.material_instances[renderer.render.material_instance_count].source_material = material_handle;
        
		return { renderer.render.material_instance_count++ };
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, r32 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT);
                u_v.float_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, math::Vec2 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT2);
                u_v.float2_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, math::Vec3 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT3);
                u_v.float3_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, math::Vec4 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::FLOAT4);
                u_v.float4_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, i32 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::INTEGER || u_v.uniform.type == ValueType::BOOL);
                if(u_v.uniform.type == ValueType::BOOL)
                {
                    u_v.boolean_val = value;
                    break;
                }
                else if(u_v.uniform.type == ValueType::INTEGER)
                {
                    u_v.integer_val = value;
                    break;
                }
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, math::Mat4 value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::MAT4);
                u_v.mat4_val = value;
                break;
            }
        }
	}

	static void set_uniform_value(Renderer& renderer, MaterialInstanceHandle handle, const char* name, TextureHandle value)
	{
		Material& material = renderer.render.material_instances[handle.handle];

		for(i32 i = 0; i < material.uniform_value_count; i++)
        {
            UniformValue& u_v = material.uniform_values[i];
            if(strncmp(u_v.uniform.name, name, strlen(name)) == 0)
            {
                assert(u_v.uniform.type == ValueType::TEXTURE);
                u_v.texture = value;
                break;
            }
        }
	}

    
    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, r32 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

    
    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec2 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

    
    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec3 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Vec4 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

    
    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, i32 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
                    {
                        if(u_v.uniform.type == ValueType::BOOL)
                        {
                            u_v.boolean_val = value;
                            break;
                        }
                        else if(u_v.uniform.type == ValueType::INTEGER)
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

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, math::Mat4 value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

    static void set_uniform_array_value(Renderer &renderer, MaterialInstanceHandle handle, const char *array_name, i32 index, const char *variable_name, rendering::TextureHandle value)
    {
        Material &material = renderer.render.material_instances[handle.handle];

        for(i32 i = 0; i < material.array_count; i++)
        {
            UniformArray &array = material.arrays[i];
            if(strcmp(array.name, array_name) == 0)
            {
                UniformArrayEntry &entry = array.entries[index];
            
                for(i32 j = 0; j < entry.value_count; j++)
                {
                    UniformValue& u_v = entry.values[j];
                    if(strcmp(u_v.name, variable_name) == 0)
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

	static void update_buffer(Renderer& renderer, BufferHandle handle, BufferData new_data)
	{
		// @Incomplete: Update from data
		// Find RegisterBufferInfo from handle
		// Update data in info from new_data
	}

    static void push_buffer(Renderer &renderer, BufferHandle buffer_handle, MaterialInstanceHandle material_instance_handle, Transform &transform)
    {
        assert(renderer.render.render_command_count < global_max_render_commands);
        
        RenderCommand render_command = {};
        render_command.buffer = buffer_handle;
        render_command.material = material_instance_handle;
        render_command.transform = transform;

        renderer.render.render_commands[renderer.render.render_command_count++] = render_command;
    }
}
