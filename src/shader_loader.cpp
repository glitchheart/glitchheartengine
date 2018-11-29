namespace rendering
{
    
// @Robustness: New shader stuff
    static void error(const char* msg, const char* file)
    {
		debug_log("ERROR: %s in %s", msg, file);
		assert(false);
    }

    static ValueType get_value_type(char** value, const char* file_path)
    {
		ValueType result;
		char type[16];
		sscanf(*value, "%s", type);
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
			error("invalid value type", file_path);
		}
		    
		*value += strlen(type);
		return result;
    }

    static char* load_shader_text(MemoryArena* arena, char* source, Shader& shader, const char* file_path, size_t* file_size = nullptr)
    {
		size_t i = 0;

		MemoryArena temp_arena = {};
		auto temp_mem = begin_temporary_memory(&temp_arena);

		char* result;

		size_t temp_current_size = strlen(source) * 2;
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
					
					char* included_text = load_shader_text(&temp_arena, included_source, shader, path);

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
					
					char* included_text = load_shader_text(&temp_arena, included_source, shader, path);

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
			else if(starts_with(buffer, "uniform"))
			{
				char* rest = &buffer[strlen("uniform") + 1];

				i32 current_handle = shader.uniform_count;
				Uniform& uniform = shader.uniforms[shader.uniform_count++];

				uniform.type = get_value_type(&rest, file_path);
		    
				while(rest[0] == ' ')
					rest++;

				sscanf(rest, "%s", uniform.name);

				rest += strlen(uniform.name);

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
						buffer[strlen(buffer) - strlen(rest) + 3] = '\0';
						buffer[strlen(buffer) - strlen(rest) + 2] = '\n';
						buffer[strlen(buffer) - strlen(rest) + 1] = ';';
						mapped_buffer[m_c] = '\0';

						UniformMapping& mapping = shader.mapped_uniforms[shader.mapped_uniform_count++];
						mapping.index = current_handle;
			    
						if(starts_with(mapped_buffer, "DIFFUSE_TEX"))
						{
							mapping.type = UniformMappingType::DIFFUSE_TEX;
						}
						else if(starts_with(mapped_buffer, "DIFFUSE_COLOR"))
						{
							mapping.type = UniformMappingType::DIFFUSE_COLOR;
						}
						else if(starts_with(mapped_buffer, "SPECULAR_TEX"))
						{
							mapping.type = UniformMappingType::SPECULAR_TEX;
						}
						else if(starts_with(mapped_buffer, "SPECULAR_COLOR"))
						{
							mapping.type = UniformMappingType::SPECULAR_COLOR;
						}
						else if(starts_with(mapped_buffer, "SPECULAR_INTENSITY"))
						{
							mapping.type = UniformMappingType::SPECULAR_INTENSITY;
						}
						else if(starts_with(mapped_buffer, "AMBIENT_COLOR"))
						{
							mapping.type = UniformMappingType::AMBIENT_COLOR;
						}
						else if(starts_with(mapped_buffer, "AMBIENT_TEX"))
						{
							mapping.type = UniformMappingType::AMBIENT_TEX;
						}
						else if(starts_with(mapped_buffer, "SHADOW_MAP"))
						{
							mapping.type = UniformMappingType::SHADOW_MAP;
						}
						else if(starts_with(mapped_buffer, "MODEL"))
						{
							mapping.type = UniformMappingType::MODEL;
						}
						else if(starts_with(mapped_buffer, "VIEW"))
						{
							mapping.type = UniformMappingType::VIEW;
						}
						else if(starts_with(mapped_buffer, "PROJECTION"))
						{
							mapping.type = UniformMappingType::PROJECTION;
						}
						else
						{
							error("Found invalid uniform mapping type", file_path);
						}
					}
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
    
    static ShaderHandle load_shader(Renderer& renderer, const char* file_path)
    {
		assert(renderer.render.shader_count + 1 < 64);
		Shader& shader = renderer.render.shaders[renderer.render.shader_count];

		// @Incomplete: LOAD STUFF
	
		FILE* file = fopen(file_path, "r");

		if(file)
		{
			size_t size = 0;
			char* source = read_file_into_buffer(&renderer.shader_arena, file, &size);
	    
			shader.vert_shader = nullptr;
			shader.frag_shader = nullptr;

			strncpy(shader.path, file_path, strlen(file_path));
			
			for(size_t i = 0; i < size; i++)
			{
				if(starts_with(&source[i], "#vert"))
				{
					shader.vert_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#vert") + 1], shader, file_path, &i);
				}
				else if(starts_with(&source[i], "#frag"))
				{
					shader.frag_shader = load_shader_text(&renderer.shader_arena, &source[i + strlen("#frag") + 1], shader, file_path, &i);
				}
			}

			assert(shader.vert_shader && shader.frag_shader);

#if DEBUG
			FILE* shd = fopen("../out.shd", "w");
			fwrite(shader.vert_shader, strlen(shader.vert_shader), 1, shd);
			fwrite(shader.frag_shader, strlen(shader.frag_shader), 1, shd);
			fclose(shd);
#endif
			fclose(file);
		}
		else
		{
			error("File not found", file_path);
		}

		return { renderer.render.shader_count++ };
	}

	static MaterialHandle create_material(Renderer& renderer, ShaderHandle shader)
	{
		Material& material = renderer.render.materials[renderer.render.material_count];
		material.shader = shader;

		// @Incomplete: Get shader uniforms

		return { renderer.render.material_count++ };
	}

	static MaterialInstanceHandle create_material_instance(Renderer& renderer, MaterialHandle material_handle)
	{
		Material& material = renderer.render.materials[material_handle.handle];
		renderer.render.material_instances[renderer.render.material_instance_count] = material;



		return { renderer.render.material_instance_count++ };
	}
}

