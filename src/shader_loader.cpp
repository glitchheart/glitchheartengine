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
						else if(starts_with(mapped_buffer, "SPECULAR_INTENSITY"))
						{
							uniform.mapping_type = UniformMappingType::SPECULAR_INTENSITY;
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

	static MaterialHandle create_material(Renderer& renderer, ShaderHandle shader_handle)
	{
		Material& material = renderer.render.materials[renderer.render.material_count];
		material.shader = shader_handle;

		Shader& shader = renderer.render.shaders[shader_handle.handle];
		
		// @Incomplete: Get shader uniforms
		for(i32 i = 0; i < shader.uniform_count; i++)
		{
			Uniform& u = shader.uniforms[i];
			UniformValue& u_v = material.uniform_values[i];

			u_v.uniform = u;

			material.uniform_value_count++;
		}

		return { renderer.render.material_count++ };
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


	static void load_material_from_mtl(Renderer& renderer, MaterialInstanceHandle material_instance, const char* file_path)
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

			Material& material = renderer.render.material_instances[material_instance.handle];			

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
					if(UniformValue* u = mapping(material, UniformMappingType::SPECULAR_INTENSITY))
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
		return info;
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

	static void create_buffers_from_mesh(Renderer& renderer, Mesh& mesh, u64 vertex_data_flags, b32 has_normals, b32 has_uvs)
	{
		assert(renderer.render.buffer_count + 1 < global_max_custom_buffers);
		i32 vertex_size = 3;

		RegisterBufferInfo info = create_register_buffer_info();

		if(has_normals)
		{
			vertex_size += 3;
		}

		if(has_uvs)
		{
			vertex_size += 2;
		}

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
		
		mesh.buffer_handle = register_buffer(renderer, info).handle;
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

	static void load_obj(Renderer& renderer, char* file_path, MeshHandle *mesh_handle = nullptr, MaterialHandle *material_handle = nullptr)
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
			
			fclose(file);
		}
		assert(renderer.mesh_count + 1 < global_max_meshes);

		MeshHandle handle = { renderer.mesh_count++ };
		Mesh &mesh = renderer.meshes[handle.handle];
		mesh = {};

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

		
		
	}


	
	
	static MaterialInstanceHandle create_material_instance(Renderer& renderer, MaterialHandle material_handle)
	{
		Material& material = renderer.render.materials[material_handle.handle];
		renderer.render.material_instances[renderer.render.material_instance_count] = material;

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
				}
				else if(u_v.uniform.type == ValueType::INTEGER)
				{
					u_v.integer_val = value;
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
			}
		}
	}

	static void update_buffer(Renderer& renderer, BufferHandle handle, BufferData new_data)
	{
		// @Incomplete: Update from data
		// Find RegisterBufferInfo from handle
		// Update data in info from new_data
	}
}

