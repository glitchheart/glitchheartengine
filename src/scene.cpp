namespace scene
{
    static void register_type(const RegisteredEntityType &registered, SceneManager *scene_manager)
    {
        scene_manager->registered_types[scene_manager->registered_type_count++] = registered;
    }

#define register_member(registered, entity_type, field, field_type) _register_member(registered, offset_of(entity_type, field), #field, field_type)
    static void _register_member(RegisteredEntityType& registered, size_t offset, char* name, FieldType type)
    {
        strcpy(registered.fields[registered.field_count].name, name);
        registered.fields[registered.field_count].offset = offset;
        registered.fields[registered.field_count++].type = type;
    }

    static void load_entity_field(const char *name, const char *data_buffer, EntityData *entity, const RegisteredEntityType &registered)
    {
        assert(entity);
        
        for(i32 i = 0; i < registered.field_count; i++)
        {
            Field field = registered.fields[i];
            if(strcmp(field.name, name) == 0)
            {
                unsigned char *ptr = ((unsigned char *)entity + field.offset);

                switch(field.type)
                {
                case FieldType::INT:
                sscanf(data_buffer, "%d", (i32*)ptr);
                break;
                case FieldType::UINT:
                sscanf(data_buffer, "%d", (u32*)ptr);
                break;
                case FieldType::BOOL:
                sscanf(data_buffer, "%d", (b32*)ptr);
                break;
                case FieldType::FLOAT:
                sscanf(data_buffer, "%f", (r32*)ptr);
                break;
                case FieldType::VEC2:
                sscanf(data_buffer, "%f %f", &((math::Vec2*)ptr)->x, &((math::Vec2*)ptr)->y);
                break;
                case FieldType::VEC3:
                sscanf(data_buffer, "%f %f %f", &((math::Vec3*)ptr)->x, &((math::Vec3*)ptr)->y, &((math::Vec3*)ptr)->z);
                break;
                case FieldType::VEC4:
                case FieldType::COLOR:
                sscanf(data_buffer, "%f %f %f %f", &((math::Vec4*)ptr)->x, &((math::Vec4*)ptr)->y, &((math::Vec4*)ptr)->z, &((math::Vec4*)ptr)->w);
                break;
                case FieldType::STRING:
                sscanf(data_buffer, "%[^\n]", (char*)ptr);
                break;
                }
            }
        }
    }

    static void translate_x(TransformComponent& comp, r32 x)
    {
        rendering::translate_x(comp.transform, x);
    }

    static void translate_y(TransformComponent& comp, r32 y)
    {
        rendering::translate_y(comp.transform, y);
    }

    static void translate_z(TransformComponent& comp, r32 z)
    {
        rendering::translate_z(comp.transform, z);
    }

    static void translate(TransformComponent& comp, math::Vec3 translation)
    {
        rendering::translate(comp.transform, translation);
    }

    static void rotate(TransformComponent& comp, math::Vec3 rotation)
    {
        rendering::rotate(comp.transform, rotation);
    }

    static void rotate(TransformComponent& comp, math::Quat rotation)
    {
        rendering::rotate(comp.transform, rotation);
    }

    static void set_rotation(TransformComponent& comp, math::Vec3 rotation)
    {
        rendering::set_rotation(comp.transform, rotation);
    }

    static void set_rotation(TransformComponent& comp, math::Quat orientation)
    {
        rendering::set_rotation(comp.transform, orientation);
    }

    static void set_rotation_x(TransformComponent& comp, r32 x)
    {
        rendering::set_rotation_x(comp.transform, x);
    }

    static void set_rotation_y(TransformComponent& comp, r32 y)
    {
        rendering::set_rotation_y(comp.transform, y);
    }

    static void set_rotation_z(TransformComponent& comp, r32 z)
    {
        rendering::set_rotation_z(comp.transform, z);
    }

    static void scale(TransformComponent& comp, math::Vec3 scale)
    {
        rendering::scale(comp.transform, scale);
    }

    static void set_scale(TransformComponent& comp, math::Vec3 scale)
    {
        rendering::set_scale(comp.transform, scale);
    }

    static void set_scale_x(TransformComponent& comp, r32 x)
    {
        rendering::set_scale_x(comp.transform, x);
    }

    static void set_scale_y(TransformComponent& comp, r32 y)
    {
        rendering::set_scale_y(comp.transform, y);
    }

    static void set_scale_z(TransformComponent& comp, r32 z)
    {
        rendering::set_scale_z(comp.transform, z);
    }

    static void set_position(TransformComponent& comp, math::Vec3 position)
    {
        rendering::set_position(comp.transform, position);
    }

    static void set_position_x(TransformComponent& comp, r32 x)
    {
        rendering::set_position_x(comp.transform, x);
    }

    static void set_position_y(TransformComponent& comp, r32 y)
    {
        rendering::set_position_y(comp.transform, y);
    }

    static void set_position_z(TransformComponent& comp, r32 z)
    {
        rendering::set_position_z(comp.transform, z);
    }

	static void set_uniform_value(EntityHandle handle, const char* name, r32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec2 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec3 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Vec4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, i32 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, math::Mat4 value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

	static void set_uniform_value(EntityHandle handle, const char* name, rendering::TextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

    static void set_uniform_value(EntityHandle handle, const char* name, rendering::MSTextureHandle value, SceneHandle &scene)
	{
        RenderComponent &render = get_render_comp(handle, scene);
        assert(value.handle != 0);
        for(i32 i = 0; i < render.render_pass_count; i++)
        {
            rendering::set_uniform_value(scene.manager->renderer, render.material_handles[i], name, value);
        }
	}

    static i32 _find_next_free_internal_handle(SceneManager *scene_manager)
    {
        i32 handle = -1;

        if(scene_manager->_internal_scene_handles[scene_manager->current_internal_index] == -1)
        {
            handle = scene_manager->current_internal_index;
        }
        else
        {
            i32 found_handle = _find_handle_in_range(scene_manager->current_internal_index, global_max_scenes, scene_manager->_internal_scene_handles);
            if(found_handle == -1)
            {
                found_handle = _find_handle_in_range(0, scene_manager->current_internal_index, scene_manager->_internal_scene_handles);
            }
            handle = found_handle;
        }

        // Set the current index to look for a free handle
        scene_manager->current_internal_index = handle + 1;
        if(scene_manager->current_internal_index == global_max_scenes)
        {
            scene_manager->current_internal_index = 0;
        }
        
        return handle;
    }

    static void _setup_scene(SceneHandle handle, SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size)
    {
        Scene &scene = get_scene(handle);
        scene = {};

        for(i32 i = 0; i < 32; i++)
        {
            scene.render_pass_cameras[i] = {0};
        }
        
        scene.handle = handle;
        scene.memory_arena = {};
        
        scene.persistent = persistent;
        scene.scene_manager = scene_manager;
        scene.valid = true;
        scene.loaded = false;
        scene.template_state = &scene_manager->template_state;
        scene.renderer = scene_manager->renderer;
        scene.max_entity_count = initial_entity_array_size;

        list::init(&scene.entities, initial_entity_array_size);
        scene.transform_component_count = 0;
        scene.render_component_count = 0;
        scene.animator_component_count = 0;
		
        auto &memory_arena = scene.memory_arena;
        // scene.entities = push_array(&memory_arena, initial_entity_array_size, Entity);
        // scene._internal_handles = push_array(&memory_arena, initial_entity_array_size, i32);
        scene.active_entities = push_array(&memory_arena, initial_entity_array_size, b32);
        scene.transform_components = push_array(&memory_arena, initial_entity_array_size, TransformComponent);
        scene.render_components = push_array(&memory_arena, initial_entity_array_size, RenderComponent);
        scene.light_components = push_array(&memory_arena, initial_entity_array_size, LightComponent);
        scene.camera_components = push_array(&memory_arena, initial_entity_array_size, CameraComponent);
        scene.particle_system_components = push_array(&memory_arena, initial_entity_array_size, ParticleSystemComponent);
        scene.animator_components = push_array(&memory_arena, initial_entity_array_size, AnimatorComponent*);

        if(scene_manager->debug_cube.handle == 0)
        {
            scene_manager->debug_cube = rendering::create_cube(scene_manager->renderer, nullptr);
            scene_manager->debug_shader_handle = rendering::load_shader(scene_manager->renderer, "../assets/shaders/unlit.shd");
            scene_manager->debug_material = rendering::create_material(scene_manager->renderer, scene_manager->debug_shader_handle);
        }

        for(i32 index = 0; index < initial_entity_array_size; index++)
        {
            scene.active_entities[index] = true;
            scene.transform_components[index].transform = rendering::create_transform(math::Vec3(0, 0, 0), math::Vec3(1, 1, 1), math::Vec3(0.0f));
            
            scene.entities[index].child_count = 0;
            scene.entities[index].parent = EMPTY_ENTITY_HANDLE;
        }
    }
    
    static SceneHandle create_scene(SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size = 1024)
    {
        i32 internal_handle = _find_next_free_internal_handle(scene_manager);
        assert(internal_handle != -1);

        i32 real_handle = scene_manager->scene_count++;
        scene_manager->_internal_scene_handles[internal_handle] = real_handle;

        SceneHandle handle;
        handle.handle = internal_handle + 1;
        handle.manager = scene_manager;
        
        _setup_scene(handle, scene_manager, persistent, initial_entity_array_size);
        
        return handle;
    }

    static void save_scene(const char *file_path, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        FILE *file = fopen(file_path, "w");

        if(file)
        {
            fprintf(file, "settings\n");
            fprintf(file, "shadows::near: %f\n", scene.settings.shadows.near_plane);
            fprintf(file, "shadows::far: %f\n", scene.settings.shadows.far_plane);
            fprintf(file, "shadows::fov: %f\n", scene.settings.shadows.fov);
            fprintf(file, "shadows::map_width: %d\n", scene.settings.shadows.map_width);
            fprintf(file, "shadows::map_height: %d\n", scene.settings.shadows.map_height);

            fprintf(file, "\n");
            
            for(i32 i = 0; i < scene.entities.count; i++)
            {
                Entity &entity = scene.entities[i];
                if(entity.savable)
                {
                    if(i > 0)
                        fprintf(file, "\n"); // Spacing between object declarations
                    
                    TransformComponent &transform = get_transform_comp(entity.handle, scene_handle);
                    
                    fprintf(file, "obj %s\n", entity.name);
                    fprintf(file, "active: %d\n", scene.active_entities[i]);
                    
                    if(strlen(entity.template_path) == 0)
                    {
                        fprintf(file, "empty\n");
                    }
                    else
                    {
                        fprintf(file, "template: %s\n", entity.template_path);
                    }
                    
                    fprintf(file, "type: %d\n", entity.type);

                    if(entity.tags.tag_count > 0)
                    {
                        fprintf(file, "tags: ");

                        for(i32 j = 0; j < entity.tags.tag_count; j++)
                        {                            
                            fprintf(file, "%s", entity.tags.tags[j]);

                            if(j < entity.tags.tag_count - 1)
                            {
                                fprintf(file, ", ");
                            }
                        }

                        fprintf(file, "\n");
                    }

                    Scene &scene = get_scene(scene_handle);
                    
                    fprintf(file, "position: %f %f %f\n", transform.transform.position.x, transform.transform.position.y, transform.transform.position.z);
                    fprintf(file, "scale: %f %f %f\n", transform.transform.scale.x, transform.transform.scale.y, transform.transform.scale.z);
                    fprintf(file, "rotation: %f %f %f\n", transform.transform.euler_angles.x, transform.transform.euler_angles.y, transform.transform.euler_angles.z);

                    if(has_camera_component(entity.handle, scene_handle))
                    {
                        CameraComponent &camera_comp = get_camera_comp(entity.handle, scene_handle);
                        fprintf(file, "camera::fov: %f\n", camera_comp.camera.fov);
                        fprintf(file, "camera::near: %f\n", camera_comp.camera.near_plane);
                        fprintf(file, "camera::far: %f\n", camera_comp.camera.far_plane);
                        fprintf(file, "camera::main: %d\n", HANDLES_EQUAL(entity.handle, scene.main_camera_handle));
                    }
                    else if(has_light_component(entity.handle, scene_handle))
                    {
                        LightComponent &light = get_light_comp(entity.handle, scene_handle);


                        switch(light.type)
                        {
                        case LightType::DIRECTIONAL:
                        {
                            fprintf(file, "light::direction: %f %f %f\n", light.dir_light.direction.x, light.dir_light.direction.y, light.dir_light.direction.z);
                            fprintf(file, "light::ambient: %f %f %f\n", light.dir_light.ambient.x, light.dir_light.ambient.y, light.dir_light.ambient.z);
                            fprintf(file, "light::diffuse: %f %f %f\n", light.dir_light.diffuse.x, light.dir_light.diffuse.y, light.dir_light.diffuse.z);
                            fprintf(file, "light::specular: %f %f %f\n", light.dir_light.specular.x, light.dir_light.specular.y, light.dir_light.specular.z);                            
                        }
                        break;
                        case LightType::POINT:
                        {
                            fprintf(file, "light::position: %f %f %f\n", light.point_light.position.x, light.point_light.position.y, light.point_light.position.z);
                            fprintf(file, "light::constant: %f\n", light.point_light.constant);
                            fprintf(file, "light::linear: %f\n", light.point_light.linear);
                            fprintf(file, "light::quadratic: %f\n", light.point_light.quadratic);
                            fprintf(file, "light::ambient: %f %f %f\n", light.point_light.ambient.x, light.point_light.ambient.y, light.point_light.ambient.z);
                            fprintf(file, "light::diffuse: %f %f %f\n", light.point_light.diffuse.x, light.point_light.diffuse.y, light.point_light.diffuse.z);
                            fprintf(file, "light::specular: %f %f %f\n", light.point_light.specular.x, light.point_light.specular.y, light.point_light.specular.z);                            
                        }
                        break;
                        default:
                        assert(false);
                        break;
                        }
                    }

                    RegisteredEntityType* type_info = get_registered_type(entity.type, scene_handle.manager);

                    if(type_info)
                    {
                        EntityData* data = entity.entity_data;
                        for(i32 j = 0; j < type_info->field_count; j++)
                        {
                            Field& field = type_info->fields[j];
                            fprintf(file, "%s: ", field.name);
                            unsigned char *ptr = ((unsigned char *)data + field.offset);
                            switch(field.type)
                            {
                            case FieldType::INT:
                            {
                                fprintf(file, "%d\n", *(i32*)(ptr));
                            }
                            break;
                            case FieldType::UINT:
                            {
                                fprintf(file, "%d\n", *(u32*)(ptr));
                            }
                            break;
                            case FieldType::BOOL:
                            {
                                fprintf(file, "%d\n", *(b32*)(ptr));
                            }
                            break;
                            case FieldType::FLOAT:
                            {
                                fprintf(file, "%f\n", *(r32*)(ptr));
                            }
                            break;
                            case FieldType::VEC2:
                            {
                                math::Vec2* vec = (math::Vec2*)(ptr);
                                fprintf(file, "%f %f\n", vec->x, vec->y);
                            }
                            break;
                            case FieldType::VEC3:
                            {
                                math::Vec3* vec = (math::Vec3*)(ptr);
                                fprintf(file, "%f %f %f\n", vec->x, vec->y, vec->z);
                            }
                            break;
                            case FieldType::VEC4:
                            case FieldType::COLOR:
                            {
                                math::Vec4* vec = (math::Vec4*)(ptr);
                                fprintf(file, "%f %f %f %f\n", vec->x, vec->y, vec->z, vec->w);
                            }
                            break;
                            case FieldType::STRING:
                            {
                                fprintf(file, "%s\n", (char*)(ptr));
                            }
                            break;
                            }
                        }
                    }
                }
            }
            fclose(file);
        }
    }

    static RegisteredEntityType * get_registered_type(u32 type_id, SceneManager *manager)
    {
        for(i32 i = 0; i < manager->registered_type_count; i++)
        {
            if(manager->registered_types[i].type_id == type_id)
                return &manager->registered_types[i];
        }
        return nullptr;
    }

    static void parse_scene_object(FILE *file, SceneHandle scene, char *name)
    {
        EntityHandle handle = { -1 };

        b32 active = true;
        
        char buffer[256];
        b32 hide_in_ui = false;

        RegisteredEntityType *type_info = nullptr;
        EntityData* entity_data = nullptr;
        
        while(fgets(buffer, 256, file))
        {
            if(starts_with(buffer, "\n"))
                break;

            if(starts_with(buffer, "empty"))
            {
                handle = register_entity(COMP_TRANSFORM, scene, true);
            }
            else if(starts_with(buffer, "template"))
            {
                char template_path[256];
                sscanf(buffer, "template: %[^\n]", template_path);
                handle = register_entity_from_template_file(template_path, scene, true);
            }
            else if(starts_with(buffer, "hide_in_ui"))
            {
                sscanf(buffer, "hide_in_ui: %d", &hide_in_ui);
            }
            else if(starts_with(buffer, "active"))
            {
                sscanf(buffer, "active: %d", &active);
            }
            else if(starts_with(buffer, "tags"))
            {
                char tag_buf[256];
                sscanf(buffer, "tags: %[^\n]", tag_buf);

                char tag[64];
                i32 tag_index = 0;
                
                for(size_t i = 0; i < strlen(tag_buf) + 1; i++)
                {
                    char c = tag_buf[i];
                    if(c == ',' || c == '\n' || c == '\r' || c == '\0')
                    {
                        tag[tag_index] = '\0';
                        tag_index = 0;
                        set_entity_tag(tag, handle, scene);
                    }
                    else if(c == ' ')
                    {
                        continue;
                    }
                    else
                    {
                        tag[tag_index++] = c;
                    }
                }
            }
            else if(starts_with(buffer, "type"))
            {
                if(!IS_ENTITY_HANDLE_VALID(handle))
                {
                    handle = register_entity(COMP_TRANSFORM, scene, true);
                }
                
                u32 type;
                sscanf(buffer, "type: %d", &type);
                type_info = get_registered_type(type, scene.manager);
                
                if(type_info)
                {
                    if(scene.manager->callbacks.on_load_entity_of_type)
                    {
                        entity_data = scene.manager->callbacks.on_load_entity_of_type(handle, type_info->type_id, scene);
                        if(entity_data)
                        {
                            entity_data->handle.handle = handle.handle;
                            Entity& entity = get_entity(handle, scene);
                            entity.savable = true;
                            entity.entity_data = entity_data;
                            entity.type = type;
                            entity.type_info = *type_info;
                        }
                    }
                }
            }
            else if(starts_with(buffer, "position"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_position;
                sscanf(buffer, "position: %f %f %f", &new_position.x, &new_position.y, &new_position.z);
                rendering::set_position(transform.transform, new_position);
                
                rendering::recompute_transform(transform.transform);
            }
            else if(starts_with(buffer, "scale"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_scale;
                sscanf(buffer, "scale: %f %f %f", &new_scale.x, &new_scale.y, &new_scale.z);
                rendering::set_scale(transform.transform, new_scale);
                
                rendering::recompute_transform(transform.transform);
            }
            else if(starts_with(buffer, "rotation"))
            {
                assert(handle.handle != -1);
                TransformComponent &transform = get_transform_comp(handle, scene);

                math::Vec3 new_rotation;
                sscanf(buffer, "rotation: %f %f %f", &new_rotation.x, &new_rotation.y, &new_rotation.z);
                rendering::set_rotation(transform.transform, new_rotation);
                
                rendering::recompute_transform(transform.transform);
            }
            else if(starts_with(buffer, "id"))
            {
            }
            else if(starts_with(buffer, "camera::"))
            {
                if(!has_camera_component(handle, scene))
                {
                    add_camera_component(handle, scene);
                }

                CameraComponent &camera_comp = get_camera_comp(handle, scene);
                
                if(starts_with(buffer, "camera::fov"))
                {
                    sscanf(buffer, "camera::fov: %f", &camera_comp.camera.fov);
                }
                else if(starts_with(buffer, "camera::near"))
                {
                    sscanf(buffer, "camera::near: %f", &camera_comp.camera.near_plane);
                }
                else if(starts_with(buffer, "camera::far"))
                {
                    sscanf(buffer, "camera::far: %f", &camera_comp.camera.far_plane);
                }
                else if(starts_with(buffer, "camera::main"))
                {
                    b32 is_main_camera = false;

                    sscanf(buffer, "camera::main: %d", &is_main_camera);

                    if(is_main_camera)
                    {
                        set_main_camera(handle, scene); 
                    }
                }
            }
            else if(starts_with(buffer, "light::"))
            {
                LightComponent &light_comp = get_light_comp(handle, scene);
                if(starts_with(buffer, "light::direction"))
                {
                    if(light_comp.type != LightType::DIRECTIONAL)
                    {
                        log_error("ERROR in scene loading: Light type mismatch");
                    }
                    sscanf(buffer, "light::direction: %f %f %f", &light_comp.dir_light.direction.x, &light_comp.dir_light.direction.y, &light_comp.dir_light.direction.z);
                }
                else if(starts_with(buffer, "light::ambient"))
                {
                    math::Vec3 ambient;
                    sscanf(buffer, "light::ambient: %f %f %f", &ambient.x, &ambient.y, &ambient.z);
                    switch(light_comp.type)
                    {
                    case LightType::DIRECTIONAL:
                    {
                        light_comp.dir_light.ambient = ambient;
                    }
                    break;
                    case LightType::POINT:
                    {
                        light_comp.point_light.ambient = ambient;                        
                    }
                    break;
                    default:
                    assert(false);
                    break;
                    }
                }
                else if(starts_with(buffer, "light::diffuse"))
                {
                    math::Vec3 diffuse;
                    sscanf(buffer, "light::diffuse: %f %f %f", &diffuse.x, &diffuse.y, &diffuse.z);
                    switch(light_comp.type)
                    {
                    case LightType::DIRECTIONAL:
                    {
                        light_comp.dir_light.diffuse = diffuse;
                    }
                    break;
                    case LightType::POINT:
                    {
                        light_comp.point_light.diffuse = diffuse;                        
                    }
                    break;
                    default:
                    assert(false);
                    break;
                    }
                }
                else if(starts_with(buffer, "light::specular"))
                {
                    math::Vec3 specular;
                    sscanf(buffer, "light::specular: %f %f %f", &specular.x, &specular.y, &specular.z);
                    switch(light_comp.type)
                    {
                    case LightType::DIRECTIONAL:
                    {
                        light_comp.dir_light.specular = specular;
                    }
                    break;
                    case LightType::POINT:
                    {
                        light_comp.point_light.specular = specular;                        
                    }
                    break;
                    default:
                    assert(false);
                    break;
                    }
                }
                else if(starts_with(buffer, "light::position"))
                {
                    if(light_comp.type != LightType::POINT)
                    {
                        log_error("ERROR in scene loading: Light type mismatch");
                    }
                    sscanf(buffer, "light::direction: %f %f %f", &light_comp.point_light.position.x, &light_comp.point_light.position.y, &light_comp.point_light.position.z);
                }
                else if(starts_with(buffer, "light::constant"))
                {
                    if(light_comp.type != LightType::POINT)
                    {
                        log_error("ERROR in scene loading: Light type mismatch");
                    }
                    sscanf(buffer, "light::constant: %f", &light_comp.point_light.constant);
                }
                else if(starts_with(buffer, "light::linear"))
                {
                    if(light_comp.type != LightType::POINT)
                    {
                        log_error("ERROR in scene loading: Light type mismatch");
                    }
                    sscanf(buffer, "light::linear: %f", &light_comp.point_light.linear);
                }
                else if(starts_with(buffer, "light::quadratic"))
                {
                    if(light_comp.type != LightType::POINT)
                    {
                        log_error("ERROR in scene loading: Light type mismatch");
                    }
                    sscanf(buffer, "light::quadratic: %f", &light_comp.point_light.quadratic);
                }
            }
            else
            {
                if(!type_info || !entity_data)
                {
                    debug("entity type not found\n");
                    continue;
                }
                
                char name[32];
                u32 end = 0;
                
                for(end = 0; end < strlen(buffer); end++)
                {
                    if(buffer[end] == ':')
                        break;
                    
                    name[end] = buffer[end];
                }
                name[end] = '\0';
                
                char *val = buffer + strlen(name) + 2;
                
                load_entity_field(name, val, entity_data, *type_info);
            }
        }

		if (entity_data && scene.manager->callbacks.on_loaded_entity_of_type)
		{
			scene.manager->callbacks.on_loaded_entity_of_type(handle, type_info->type_id, scene);
		}

		set_hide_in_ui(handle, hide_in_ui, scene);

        if(!is_whitespace(name))
        {
            Scene &s = get_scene(scene);
            _set_entity_name(handle, name, s);
        }

        scene::set_active(handle, active, scene);
    }

static Camera get_standard_camera(SceneManager& manager)
{
    Camera camera = {};
    math::Vec3 position = math::Vec3(0.0f, 15.0f, -2.0f);
    math::Vec3 target = math::Vec3(0.0f, 0.0f, 2.0f);
    r32 fov = 90 * DEGREE_IN_RADIANS;
    r32 z_near = 0.1f;
    r32 z_far = 200.0f;

    math::Mat4 projection = math::perspective((r32)manager.renderer->window_width / (r32)manager.renderer->window_height, fov, z_near, z_far);

    camera = create_camera(position, target, projection);
    return camera;
}

    static void parse_scene_settings(FILE* file, SceneHandle handle)
    {
        Scene& scene = get_scene(handle);

        Settings& settings = scene.settings;
        settings.shadows.near_plane = 0.1f;
        settings.shadows.far_plane = 10.0f;
        settings.shadows.fov = 110.0f;
        settings.shadows.map_width = 1024;
        settings.shadows.map_height = 1024;

        // settings.camera.position = math::Vec3(0.0f, 15.0f, -2.0f);
        // settings.camera.target = math::Vec3(0.0f, 0.0f, 2.0f);
        // settings.camera.fov = 90 * DEGREE_IN_RADIANS;
        // settings.camera.z_near = 0.1f;
        // settings.camera.z_far = 50.0f;

        scene::EntityHandle camera_handle = EMPTY_ENTITY_HANDLE;
        
        char buffer[256];
        while(fgets(buffer, 256, file))
        {
            if(starts_with(buffer, "\n"))
                break;

            if(starts_with(buffer, "shadows::near"))
            {
                sscanf(buffer, "shadows::near: %f", &settings.shadows.near_plane);
            }
            else if(starts_with(buffer, "shadows::far"))
            {
                sscanf(buffer, "shadows::far: %f", &settings.shadows.far_plane);            
            }
            else if(starts_with(buffer, "shadows::fov"))
            {
                sscanf(buffer, "shadows::fov: %f", &settings.shadows.fov);
            }
            else if(starts_with(buffer, "shadows::map_width"))
            {
                sscanf(buffer, "shadows::map_width: %d", &settings.shadows.map_width);
            }
            else if(starts_with(buffer, "shadows::map_height"))
            {
                sscanf(buffer, "shadows::map_height: %d", &settings.shadows.map_height);
            }
            else if(starts_with(buffer, "camera::"))
            {
                if(!IS_ENTITY_HANDLE_VALID(camera_handle))
                {
                    camera_handle = register_entity(COMP_TRANSFORM | COMP_CAMERA, scene.handle, true);
                    set_main_camera(camera_handle, scene.handle);
                }

                CameraComponent &camera_comp = get_camera_comp(camera_handle, scene.handle);
                
                _set_entity_name(camera_handle, "Main camera", scene);
                
                if(starts_with(buffer, "camera::position"))
                {
                    TransformComponent &transform = get_transform_comp(camera_handle, scene.handle);

                    math::Vec3 position;
                    sscanf(buffer, "camera::position: %f %f %f", &position.x, &position.y, &position.z);
                    set_position(transform, position);
                }
                else if(starts_with(buffer, "camera::fov"))
                {
                    sscanf(buffer, "camera::fov: %f", &camera_comp.camera.fov);
                }
                else if(starts_with(buffer, "camera::near"))
                {
                    sscanf(buffer, "camera::near: %f", &camera_comp.camera.near_plane);
                }
                else if(starts_with(buffer, "camera::far"))
                {
                    sscanf(buffer, "camera::far: %f", &camera_comp.camera.far_plane);
                }
            }
        }
    }
    
    static void _create_scene_from_file(const char *scene_file_path, SceneManager *scene_manager, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        strcpy(scene.file_path, scene_file_path);

        Settings& settings = scene.settings;
        settings.shadows.near_plane = 0.1f;
        settings.shadows.far_plane = 10.0f;
        settings.shadows.fov = 110.0f;
        settings.shadows.map_width = 1024;
        settings.shadows.map_height = 1024;
        
        // settings.camera.position = math::Vec3(0.0f, 15.0f, -2.0f);
        // settings.camera.target = math::Vec3(0.0f, 0.0f, 2.0f);
        
        // settings.camera.fov = 90 * DEGREE_IN_RADIANS;
        // settings.camera.z_near = 0.1f;
        // settings.camera.z_far = 50.0f;
        
        FILE *file = fopen(scene_file_path, "r");
        
        if(file)
        {
            char line_buffer[256];
            while(fgets(line_buffer, 256, file))
            {
                if(starts_with(line_buffer, "obj"))
                {
                    char name[32];
                    memset(name, 0, 32);
 
                    if(!starts_with(line_buffer, "obj\n"))
                        sscanf(line_buffer, "obj %[^\n]", name);
                   
                    parse_scene_object(file, handle, name);
                }
                else if(starts_with(line_buffer, "settings"))
                {
                    parse_scene_settings(file, handle);
                }
            }                
            fclose(file);
        }
    }
    
    static SceneHandle create_scene_from_file(const char *scene_file_path, SceneManager *scene_manager, b32 persistent, i32 initial_entity_array_size)
    {
        SceneHandle handle = create_scene(scene_manager, persistent, initial_entity_array_size);
        _create_scene_from_file(scene_file_path, scene_manager, handle);

        update_cameras(get_scene(handle), scene_manager);
        deactivate_particle_systems(handle);
        
        return handle;
    }

    static void reload_scene(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        
        b32 persistent = scene.persistent;
        i32 max_entity_count = scene.max_entity_count;
        char scene_file_path[256];
        strcpy(scene_file_path, scene.file_path);
        
        scene::free_scene(handle, false);
        _setup_scene(handle, handle.manager, persistent, max_entity_count);
        _create_scene_from_file(scene_file_path, handle.manager, handle);
        load_scene(handle);
    }
    
    struct InstancePair
    {
        rendering::MaterialHandle material_handle;
        rendering::BufferHandle buffer_handle;
        rendering::MaterialInstanceHandle material_instances[1024];
		rendering::RenderPassHandle pass_handle;
		i32 count;

        rendering::VertexAttribute attributes[8];
        i32 attribute_count;

        b32 is_static;
    };
    
    static void allocate_instance_buffers(Scene &scene)
    {
        InstancePair *instance_pairs = (InstancePair*)malloc(sizeof(InstancePair) * 256);
        i32 pair_count = 0;
        
        Renderer *renderer = scene.renderer;

        // Find out how many instance buffers we need to allocate
        for(i32 i = 0; i < scene.render_component_count; i++)
        {
            RenderComponent &comp = scene.render_components[i];

            for(i32 pass_index = 0; pass_index < comp.render_pass_count; pass_index++)
            {
				rendering::RenderPassHandle pass_handle = comp.render_passes[pass_index];
                rendering::MaterialInstanceHandle instance_handle = comp.material_handles[pass_index];
                rendering::Material &material_instance = rendering::get_material_instance(instance_handle, renderer);

                if(material_instance.instanced_vertex_attribute_count > 0)
                {
                    b32 found = false;
            
                    for(i32 j = 0; j < pair_count; j++)
                    {
                        InstancePair &pair = instance_pairs[j];
                        if(pair.pass_handle.handle == pass_handle.handle && pair.buffer_handle.handle == comp.buffer_handle.handle && pair.material_handle.handle == comp.original_materials[pass_index].handle)
                        {
                            found = true;
                            pair.material_instances[pair.count++] = instance_handle;
                        }
                    }

                    if(!found)
                    {
                        InstancePair &pair = instance_pairs[pair_count++];
                        pair.attribute_count = 0;
                        pair.count = 0;
                        pair.is_static = comp.is_static;
                        pair.buffer_handle = comp.buffer_handle;
                        pair.material_handle = material_instance.source_material;
						pair.pass_handle = pass_handle;

                        for(i32 j = 0; j < material_instance.instanced_vertex_attribute_count; j++)
                        {
                            pair.attributes[pair.attribute_count++] = material_instance.instanced_vertex_attributes[j].attribute;
                        }
                    
                        pair.material_instances[pair.count++] = instance_handle;
                    }
                }

                comp.material_handles[pass_index] = instance_handle;
            }
        }

        // Allocate the buffers
        for(i32 i = 0; i < pair_count; i++)
        {
            InstancePair &pair = instance_pairs[i];

            rendering::BufferUsage usage = pair.is_static ? rendering::BufferUsage::STATIC : rendering::BufferUsage::DYNAMIC;
            
            InstanceBufferData data = {};
            data.buffer_handle = pair.buffer_handle;
            data.source_material_handle = pair.material_handle;
            data.pass_handle = pair.pass_handle;
            data.max_count = pair.count;
            // Allocate all buffers
            for(i32 j = 0; j < pair.attribute_count; j++)
            {
                rendering::InstanceBufferHandle instance_buffer_handle = rendering::allocate_instance_buffer(pair.attributes[j].type, math::next_power_of_two(pair.count), usage, renderer);

                data.instance_buffer_handles[data.instance_buffer_count++] = instance_buffer_handle;
                
                // Update all materials with the correct buffer handle
                for(i32 k = 0; k < pair.count; k++)
                {
                    rendering::Material &material = get_material_instance(pair.material_instances[k], renderer);
                    material.instanced_vertex_attributes[j].instance_buffer_handle = instance_buffer_handle;
                }

            }

            scene.instance_buffer_data[scene.instance_buffer_data_count++] = data;
            assert(scene.instance_buffer_data_count < MAX_INSTANCE_BUFFER_HANDLES);
        }

		free(instance_pairs);
    }

    static void free_instance_buffers(Scene &scene)
    {
        for(i32 i = 0; i < scene.instance_buffer_data_count; i++)
        {
            InstanceBufferData data = scene.instance_buffer_data[i];
            for(i32 j = 0; j < data.instance_buffer_count; j++)
                rendering::free_instance_buffer(data.instance_buffer_handles[j], scene.renderer);
        }
        scene.instance_buffer_data_count = 0;
    }

    static void free_scene(SceneHandle handle, b32 invalidate_handle)
    {
        SceneManager *scene_manager = handle.manager;
        
        if(!scene_manager || handle.handle <= 0)
            return;
        
        if(scene_manager->callbacks.on_scene_will_be_freed)
            scene_manager->callbacks.on_scene_will_be_freed(handle);
        
        i32 internal_handle = scene_manager->_internal_scene_handles[handle.handle - 1];

        Scene &scene = scene_manager->scenes[internal_handle];
        
        if(scene.valid)
        {
			scene.valid = false;
            scene.loaded = false;

			if (scene.entities.count > 0)
            {
                for(i32 i = 0; i < scene.render_component_count; i++)
                {
                    for(i32 j = 0; j < scene.render_components[i].render_pass_count; j++)
                    {
                        rendering::delete_material_instance(scene.renderer, scene.render_components[i].material_handles[j]);
                    }
                }
                
                list::free(&scene.entities);
                scene.transform_component_count = 0;
                scene.render_component_count = 0;
            
                for(i32 index = 0; index < scene.particle_system_component_count; index++)
                {
                    ParticleSystemComponent& ps_comp = scene.particle_system_components[index];
                    scene_manager->renderer->particles.api->remove_particle_system(scene.renderer, ps_comp.handle);
                }
                
                scene.particle_system_component_count = 0;
                scene.light_component_count = 0;
                clear(&scene.memory_arena);
            }

            free_instance_buffers(scene);

            if(invalidate_handle)
            {
                scene_manager->_internal_scene_handles[handle.handle - 1] = -1;
                if(internal_handle < scene_manager->scene_count - 1)
                {
                    scene_manager->scenes[internal_handle] = scene_manager->scenes[scene_manager->scene_count - 1];
                    for(i32 i = 0; i < global_max_scenes; i++)
                    {
                        if(scene_manager->_internal_scene_handles[i] == scene_manager->scene_count - 1)
                        {
                            scene_manager->_internal_scene_handles[i] = internal_handle;
                            break;
                        }
                    }
                }
            
                scene_manager->scene_count--;
            }
            
            scene_manager->scene_loaded = false;
        }
    }

    static void deactivate_particle_systems(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);                
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            handle.manager->renderer->particles.api->stop_particle_system(scene.particle_system_components[i].handle, scene.renderer);
        }
    }

    static void activate_particle_systems(SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            ParticleSystemInfo* ps = handle.manager->renderer->particles.api->get_particle_system_info(scene.particle_system_components[i].handle, handle.manager->renderer);
            if(ps->attributes.play_on_awake)
            {
                handle.manager->renderer->particles.api->start_particle_system(scene.particle_system_components[i].handle, scene.renderer);
            }
            handle.manager->renderer->particles.api->pause_particle_system(scene.particle_system_components[i].handle, scene.renderer, false);
        }
    }
    
    static math::Ray cast_ray(Scene &scene, i32 mouse_x, i32 mouse_y)
    {
        i32 width = scene.renderer->window_width;
        i32 height = scene.renderer->window_height;

        CameraComponent &camera_comp = _get_camera_comp(scene.scene_manager->editor_camera, scene);
        TransformComponent &transform = _get_transform_comp(scene.scene_manager->editor_camera, scene);
        Camera &camera = camera_comp.camera;
        
        r32 x = (2.0f * mouse_x) / width - 1.0f;
        r32 y = 1.0f - (2.0f * mouse_y) / height;
        r32 z = 1.0f;
        
        math::Vec3 ray_nds = math::Vec3(x, y, z);
        math::Vec4 ray_clip = math::Vec4(ray_nds.x, ray_nds.y, -1.0f, 1.0f);
        math::Vec4 ray_eye = math::inverse(camera.projection_matrix) * ray_clip;
        ray_eye = math::Vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);

        math::Vec3 ray_wor = (inverse(camera.view_matrix) * ray_eye).xyz;
        ray_wor = math::normalize(ray_wor);
        
        math::Ray ray;
        ray.origin = transform.transform.position;
        ray.direction = ray_wor;

        return ray;
    }

    static math::Ray cast_ray(SceneHandle handle, i32 mouse_x, i32 mouse_y)
    {
        return cast_ray(get_scene(handle), mouse_x, mouse_y);
    }

#define EPSILON 0.00001f

    b32 ray_intersects_plane(math::Vec3 plane_normal, math::Vec3 plane_position, math::Vec3 ray_origin, math::Vec3 ray_direction, math::Vec3 &intersection_point)
    {
        // assuming vectors are all normalized
        float denom = math::dot(plane_normal, ray_direction);
        if (denom > 1e-6)
        {
            math::Vec3 p0l0 = plane_position - ray_origin;
            r32 t = math::dot(p0l0, plane_normal) / denom;
            intersection_point = ray_origin + t * ray_direction;
            return (t >= 0);
        }

        return false; 
        
    }

    math::Vec3 ray_vs_plane(math::Vec3 ray_origin, math::Vec3 ray_direction, math::Vec3 plane_normal, r32 plane_d)
    {
        r32 denominator = math::dot(plane_normal, ray_direction);
        r32 numerator = math::dot(plane_normal, ray_origin);
        r32 t = -((plane_d + numerator) / denominator);
        return ray_origin + t * ray_direction;
    }
    
    b32 ray_vs_plane(math::Vec3 *x_point, math::Vec3 p0, math::Vec3 dir, math::Vec3 plane_point, math::Vec3 plane_n)
    {
        math::Vec3 local_p0 = p0 - plane_point;

        r32 denom = math::dot(plane_n, dir);
        r32 epsilon = 0.0001f;
        if(ABS(denom) < epsilon) return false;

        r32 t = (math::dot(local_p0, plane_n) / denom);

        debug("t: %f\n", t);
        
        *x_point = p0 + dir * t;
        
        if(t < 0) return false;

        return true;
    }

    static void deactivate_gizmo_arrows(SceneManager *manager)
    {
        manager->gizmos.active = false;
    }

    static EntityHandle get_root_entity(EntityHandle handle, Scene &scene)
    {
        Entity &entity = get_entity(handle, scene);

        if(IS_ENTITY_HANDLE_VALID(entity.parent))
        {
            return get_root_entity(entity.parent, scene);
        }
        else
        {
            if(!entity.selection_enabled)
                return { -1 };
            else
                return handle;
        }
    }

    static void add_to_list(EntityList& list, EntityHandle entity)
    {
        list.handles[list.entity_count++] = entity;
    }
    static EntityHandle pick_entity(SceneHandle handle, i32 mouse_x, i32 mouse_y)
    {
        Scene &scene = get_scene(handle);

        math::Ray ray = cast_ray(scene, mouse_x, mouse_y);       
        
        if(IS_ENTITY_HANDLE_VALID(handle.manager->selected_entity))
        {
            TransformComponent &selected_transform = get_transform_comp(handle.manager->selected_entity, handle);
            math::BoundingBox box;
            math::Vec3 real_scale = math::Vec3(1, 1, 1) * handle.manager->gizmos.current_distance_to_camera;
            
            box.min = math::Vec3(selected_transform.transform.position.x - real_scale.x * 0.5f, selected_transform.transform.position.y - real_scale.y * 0.5f, selected_transform.transform.position.z - real_scale.z * 0.5f);
            box.max = math::Vec3(selected_transform.transform.position.x + real_scale.x * 0.5f, selected_transform.transform.position.y + real_scale.y * 0.5f, selected_transform.transform.position.z + real_scale.z * 0.5f);

            math::Vec3 intersection_point;
            if(aabb_ray_intersection(ray, box, &intersection_point))
            {
                return handle.manager->gizmos.scale_cubes[3];
            }
        }

        EntityList entity_list = {};
        
        for(i32 i = 0; i < scene.entities.count; i++)
        {
            const scene::Entity &ent = scene.entities[i];

            if (scene.active_entities[i])
            {
                if (ent.comp_flags & scene::COMP_RENDER)
                {
                    scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
                    scene::RenderComponent &render_comp = scene.render_components[ent.render_handle.handle];

                    // Ignore the entity if it is not selectable and it has no parent
                    if(!ent.selection_enabled && !IS_ENTITY_HANDLE_VALID(ent.parent))
                        continue;

                    math::BoundingBox box = render_comp.bounding_box;

                    box.min = transform.transform.model * box.min;
                    box.max = transform.transform.model * box.max;

                    math::Vec3 intersection_point;
                    if(aabb_ray_intersection(ray, box, &intersection_point))
                    {
                        add_to_list(entity_list, ent.handle);
                    }
                }
            }
        }

        r32 dist = 100000; // Just set a crazy max distance
        EntityHandle entity_handle = { -1 };
        MemoryArena temp_arena = {};
        TemporaryMemory temp_mem = begin_temporary_memory(&temp_arena);

        TransformComponent &editor_camera_transform = _get_transform_comp(scene.scene_manager->editor_camera, scene);
        
        for(i32 i = 0; i < entity_list.entity_count; i++)
        {
            Entity entity = get_entity(entity_list.handles[i], scene);
            rendering::Transform transform = get_transform_comp(entity_list.handles[i], scene).transform;

            RenderComponent render = get_render_comp(entity_list.handles[i], scene.handle);
            if(Mesh* mesh = rendering::get_mesh(scene.renderer, render.buffer_handle.loaded_mesh_handle))
            {
                math::Mat4 inv_world = math::inverse(transform.model);
                math::Vec3 intersection_point;

                for(i32 j = 0; j < mesh->face_count; j++)
                {
                    Face face = mesh->faces[j];
                    Vertex v1 = mesh->vertices[face.indices[0]];
                    Vertex v2 = mesh->vertices[face.indices[1]];
                    Vertex v3 = mesh->vertices[face.indices[2]];

                    math::Ray local_ray = ray;
                    
                    local_ray.origin = (inv_world * math::Vec4(ray.origin, 1.0f)).xyz;
                    local_ray.direction = (inv_world * math::Vec4(ray.direction, 0.0f)).xyz;

                    if(triangle_ray_intersection(local_ray, v1.position, v2.position, v3.position, &intersection_point))
                    {
                        intersection_point = transform.model * intersection_point;
                        r32 new_dist = math::distance(editor_camera_transform.transform.position, intersection_point);

                        if(new_dist < dist)
                        {
                            entity_handle = entity_list.handles[i];
                            dist = new_dist;
                        }
                    }
                }
            }
        }
		
        end_temporary_memory(temp_mem);

        if(IS_ENTITY_HANDLE_VALID(entity_handle))
            handle.manager->gizmos.selected_gizmo = Gizmos::NONE;
        else
            return {0};
        

        Entity entity = get_entity(entity_handle, scene);
    
        // Look for selectable parents
        if(entity.selection_enabled)
        {
            return entity_handle;
        }

        EntityHandle parent = get_root_entity(entity.handle, scene);
                            
        // If a selectable parent was found
        if(IS_ENTITY_HANDLE_VALID(parent))
        {
            return parent;
        }
        else
        {
            return { 0 };
        }
    }

    static b32 entity_exists(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene& scene = get_scene(scene_handle);
        return list::has_value(&scene.entities, entity_handle.handle);
    }

    static b32 has_render_component(EntityHandle entity_handle, SceneHandle& scene)
    {
        if(!entity_exists(entity_handle, scene))
            return false;
        
        Entity& entity = get_entity(entity_handle, scene);

        return (b32)(entity.comp_flags & COMP_RENDER);
    }

    static b32 has_transform_component(EntityHandle entity_handle, SceneHandle& scene)
    {
        if(!entity_exists(entity_handle, scene))
            return false;
        
        Entity& entity = get_entity(entity_handle, scene);

        return (b32)(entity.comp_flags & COMP_TRANSFORM);
    }

    static b32 has_particle_component(EntityHandle entity_handle, SceneHandle& scene)
    {
        if(!entity_exists(entity_handle, scene))
            return false;
        
        Entity& entity = get_entity(entity_handle, scene);

        return (b32)(entity.comp_flags & COMP_PARTICLES);
    }

    static b32 has_light_component(EntityHandle entity_handle, SceneHandle& scene)
    {
        if(!entity_exists(entity_handle, scene))
            return false;
        
        Entity& entity = get_entity(entity_handle, scene);

        return (b32)(entity.comp_flags & COMP_LIGHT);
    }

    static b32 has_camera_component(EntityHandle entity_handle, SceneHandle scene)
    {
        if(!entity_exists(entity_handle, scene))
            return false;
        
        Entity& entity = get_entity(entity_handle, scene);

        return (b32)(entity.comp_flags & COMP_CAMERA);
    }
    
    static void set_wireframe_enabled(b32 enabled, EntityHandle entity_handle, SceneHandle &handle, b32 update_children = true)
    {
        Scene &scene = get_scene(handle);
        if(has_render_component(entity_handle, handle))
        {
            RenderComponent &render_comp = _get_render_comp(entity_handle, scene);
            render_comp.wireframe_enabled = enabled;            
        }

        if(update_children)
        {
            Entity &entity = get_entity(entity_handle, handle);
        
            for(i32 i = 0; i < entity.child_count; i++)
            {
                scene::set_wireframe_enabled(enabled, entity.children[i], handle);
            }
        }
    }

    static void set_bounding_box_enabled(b32 enabled, EntityHandle entity_handle, SceneHandle &handle)
    {
        Scene &scene = get_scene(handle);
        if(has_render_component(entity_handle, handle))
        {
            RenderComponent &render_comp = _get_render_comp(entity_handle, scene);
            render_comp.bounding_box_enabled = enabled;            
        }
    }

    static b32 get_bounding_box_enabled(EntityHandle entity_handle, SceneHandle &handle)
    {
        Scene &scene = get_scene(handle);
        if(has_render_component(entity_handle, handle))
        {
            RenderComponent &render_comp = _get_render_comp(entity_handle, scene);
            return render_comp.bounding_box_enabled;            
        }
        return false;
    }

    static void _register_gizmos(SceneHandle scene)
    {
        SceneManager *manager = scene.manager;
        manager->gizmos.active = true;
    }

    static void draw_gizmos(SceneManager *manager)
    {
        TranslationConstraint c = manager->gizmos.constraint;
        if(manager->gizmos.active)
        {
            r32 unit = manager->gizmos.current_distance_to_camera;
            
            TransformComponent &transform_comp = get_transform_comp(manager->selected_entity, manager->loaded_scene);
            rendering::Transform t = rendering::create_transform(transform_comp.transform.position, math::Vec3(1.0f), math::Vec3(0.0f));
            math::Vec3 yellow(RGB_FLOAT(189), RGB_FLOAT(183), RGB_FLOAT(107));

            if(manager->gizmos.transformation_type == TransformationType::SCALE)
            {
                r32 scale_cube_size = 0.2f;
                
                set_uniform_value(manager->gizmos.scale_cubes[0], "color", c == TranslationConstraint::X ? yellow : math::Vec3(1.0f, 0.0f, 0.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[1], "color", c == TranslationConstraint::Y ? yellow : math::Vec3(0.0f, 1.0f, 0.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[2], "color", c == TranslationConstraint::Z ? yellow : math::Vec3(0.0f, 0.0f, 1.0f), manager->loaded_scene);
                set_uniform_value(manager->gizmos.scale_cubes[3], "color", manager->gizmos.scaling_mode == ScalingMode::ALL_AXIS ? yellow : math::Vec3(1.0f, 1.0f, 1.0f), manager->loaded_scene);
                
                TransformComponent &x_t = get_transform_comp(manager->gizmos.scale_cubes[0], manager->loaded_scene);
                set_position(x_t, transform_comp.transform.position + math::Vec3(unit, 0, 0));
                set_scale(x_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));

                TransformComponent &y_t = get_transform_comp(manager->gizmos.scale_cubes[1], manager->loaded_scene);
                set_position(y_t, transform_comp.transform.position + math::Vec3(0, unit, 0));
                set_scale(y_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));

                TransformComponent &z_t = get_transform_comp(manager->gizmos.scale_cubes[2], manager->loaded_scene);
                set_position(z_t, transform_comp.transform.position + math::Vec3(0, 0, unit));
                set_scale(z_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));
                
                TransformComponent &center_t = get_transform_comp(manager->gizmos.scale_cubes[3], manager->loaded_scene);
                set_position(center_t, transform_comp.transform.position);
                set_scale(center_t, math::Vec3(unit * scale_cube_size, unit * scale_cube_size, unit * scale_cube_size));
            }
                
            // Draw transform manipulators
            math::Vec3 v1 = math::Vec3(0.0f, 0.0f, 0.0f);
            math::Vec3 v2 = math::Vec3(manager->gizmos.current_distance_to_camera, 0.0f, 0.0f);

            r32 line_thickness = 5.0f;

            // X
            rendering::set_uniform_value(manager->renderer, manager->gizmos.x_material, "color", c == TranslationConstraint::X ? math::Rgba(yellow, 1.0f) : math::Rgba(1.0f, 0.0f, 0.0f, 1.0f));
            math::Rgba color = c == TranslationConstraint::X ? math::Rgba(yellow, 1.0f) : math::Rgba(1.0f, 0.0f, 0.0f, 1.0f);

            rendering::push_line_to_render_pass(manager->renderer, v1, v2, line_thickness, color, t, manager->gizmos.x_material,  manager->renderer->render.standard_opaque_pass, rendering::CommandType::NO_DEPTH);

            // Y
            v1 = math::Vec3(0.0f, 0.0f, 0.0f);
            v2 = math::Vec3(0.0f, manager->gizmos.current_distance_to_camera, 0.0f);

            color = c == TranslationConstraint::Y ? math::Rgba(yellow, 1.0f) : math::Rgba(0.0f, 1.0f, 0.0f, 1.0f);

            rendering::push_line_to_render_pass(manager->renderer, v1, v2, line_thickness, color, t, manager->gizmos.y_material, manager->renderer->render.standard_opaque_pass, rendering::CommandType::NO_DEPTH);

            // Z
            v1 = math::Vec3(0.0f, 0.0f, 0.0f);
            v2 = math::Vec3(0.0f, 0.0f, manager->gizmos.current_distance_to_camera);

            color = c == TranslationConstraint::Z ? math::Rgba(yellow, 1.0f) : math::Rgba(0.0f, 0.0f, 1.0f, 1.0f);

            rendering::push_line_to_render_pass(manager->renderer, v1, v2, line_thickness, color, t, manager->gizmos.z_material, manager->renderer->render.standard_opaque_pass, rendering::CommandType::NO_DEPTH);
        }
    }

    // Finds the line between two lines with the minimum distance
    b32 line_vs_line(Line l1, Line l2, math::Vec3 *points)
    {
        r32 mua = 0.0f;
        r32 mub = 0.0f;
        
        math::Vec3 p1 = l1.start;
        math::Vec3 p2 = l1.end;
        math::Vec3 p3 = l2.start;
        math::Vec3 p4 = l2.end;
        
        math::Vec3 p13,p43,p21;
        r32 d1343,d4321,d1321,d4343,d2121;
        r32 numer,denom;

        p13.x = p1.x - p3.x;
        p13.y = p1.y - p3.y;
        p13.z = p1.z - p3.z;
        p43.x = p4.x - p3.x;
        p43.y = p4.y - p3.y;
        p43.z = p4.z - p3.z;
        
        if (ABS(p43.x) < EPSILON && ABS(p43.y) < EPSILON && ABS(p43.z) < EPSILON)
            return false;

        p21.x = p2.x - p1.x;
        p21.y = p2.y - p1.y;
        p21.z = p2.z - p1.z;

        if (ABS(p21.x) < EPSILON && ABS(p21.y) < EPSILON && ABS(p21.z) < EPSILON)
            return false;

        d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
        d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
        d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
        d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
        d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

        denom = d2121 * d4343 - d4321 * d4321;

        if (ABS(denom) < EPSILON)
            return false;
        numer = d1343 * d4321 - d1321 * d4343;

        mua = numer / denom;
        mub = (d1343 + d4321 * (mua)) / d4343;

        points[0].x = p1.x + mua * p21.x;
        points[0].y = p1.y + mua * p21.y;
        points[0].z = p1.z + mua * p21.z;
        points[1].x = p3.x + mub * p43.x;
        points[1].y = p3.y + mub * p43.y;
        points[1].z = p3.z + mub * p43.z;

        return true;
    }

    static Line line_from_ray(math::Ray ray)
    {
        Line l;
        l.start = ray.origin;
        l.end = ray.origin + ray.direction * 1000;
        return l;
    }
    
    static void update_transform(TransformComponent &transform, SceneManager *manager, InputController *input_controller, r64 delta_time)
    {
        if(manager->dragging)
        {
            if(manager->gizmos.transformation_type == TransformationType::SCALE && manager->gizmos.scaling_mode == ScalingMode::ALL_AXIS)
            {
                math::Vec2 mouse_position = math::Vec2(input_controller->mouse_x, manager->renderer->window_height - input_controller->mouse_y);
                math::Vec2 mouse_offset = manager->gizmos.scale_mouse_offset - mouse_position;
                r32 diff = (mouse_offset.x + mouse_offset.y) * SCALE_SENSITIVITY;
                
                rendering::set_scale(transform.transform, manager->gizmos.initial_scale + math::Vec3(diff, diff, diff));
            }
            else
            {
                math::Vec3 points[2];
                Scene &scene = get_scene(manager->loaded_scene);
                math::Ray ray = cast_ray(scene, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);
            
                Line l1 = line_from_ray(ray);
            
                line_vs_line(l1, manager->gizmos.current_line, points);
                
                switch(manager->gizmos.constraint)
                {
                case TranslationConstraint::X:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_x(transform.transform, points[1].x - manager->gizmos.initial_offset.x);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_x(transform.transform, points[1].x - manager->gizmos.initial_offset.x);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                case TranslationConstraint::Y:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_y(transform.transform, points[1].y - manager->gizmos.initial_offset.y);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_y(transform.transform, points[1].y - manager->gizmos.initial_offset.y);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                case TranslationConstraint::Z:
                {
                    if(manager->gizmos.transformation_type == TransformationType::POSITION)
                    {
                        rendering::set_position_z(transform.transform, points[1].z - manager->gizmos.initial_offset.z);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::SCALE)
                    {
                        rendering::set_scale_z(transform.transform, points[1].z - manager->gizmos.initial_offset.z);
                    }
                    else if(manager->gizmos.transformation_type == TransformationType::ROTATION)
                    {
                        // @Incomplete
                    }
                }
                break;
                default:
                break;
                }
            }
        }
    }

    static void _select_gizmo(SceneHandle handle, EntityHandle gizmo)
    {
        //RenderComponent &render_selected = get_render_comp(gizmo, handle);
        //rendering::set_uniform_value(*handle.manager->renderer, render_selected.material_handle, "color", math::Rgba(1.0f, 1.0f, 0.0f, 1.0f));
    }

    static void set_editor_camera_lock(b32 locked, SceneManager *scene_manager)
    {
        scene_manager->editor.lock_camera = locked;
    }

    static void editor_save(SceneManager *scene_manager)
    {
        // @Incomplete: Save scene-file
        if(scene_manager->callbacks.on_save)
            scene_manager->callbacks.on_save(scene_manager->loaded_scene);
    }
    
    static void set_selection_enabled(b32 enabled, SceneManager *scene_manager)
    {
        scene_manager->editor.selection_enabled = enabled;

        if(!scene_manager->editor.selection_enabled)
        {
            if(IS_ENTITY_HANDLE_VALID(scene_manager->selected_entity))
                scene::set_wireframe_enabled(false, scene_manager->selected_entity, scene_manager->loaded_scene);
            
            scene_manager->selected_entity = { -1 };
            scene_manager->gizmos.active = false;
        }
    }
    
    static void toggle_selection_enabled(SceneManager *scene_manager)
    {
        set_selection_enabled(!scene_manager->editor.selection_enabled, scene_manager);
   } 

    static Entity& get_entity(EntityHandle handle, Scene &scene)
    {
        assert(list::has_value(&scene.entities, handle.handle));
        Entity& entity = list::get(&scene.entities, handle.handle);
        
        assert(entity.comp_flags & COMP_TRANSFORM);
        
        return entity;
    }
    
    static Entity& get_entity(EntityHandle handle, SceneHandle& scene_handle)
    {
        Scene& scene = get_scene(scene_handle);
        assert(handle.handle != 0);
        return get_entity(handle, scene);
    }
    
    static void delete_entity(EntityHandle handle, SceneManager *manager)
    {
        if(manager->callbacks.on_entity_will_be_deleted)
            manager->callbacks.on_entity_will_be_deleted(handle, manager->loaded_scene);

        Entity &entity = get_entity(handle, manager->loaded_scene);
        
        for(i32 i = 0; i < entity.child_count; i++)
        {
            delete_entity(entity.children[i], manager);
        }
        
        unregister_entity(handle, manager->loaded_scene);
    }

    static void set_transformation_type(TransformationType type, SceneManager *scene_manager)
    {
        scene_manager->gizmos.transformation_type = type;
        switch(type)
        {
        case TransformationType::POSITION:
        {
            for(i32 i = 0; i < 4; i++)
            {
                set_active(scene_manager->gizmos.scale_cubes[i], false, scene_manager->loaded_scene);
            }
        }
        break;
        case TransformationType::SCALE:
        {
            for(i32 i = 0; i < 4; i++)
            {
                set_active(scene_manager->gizmos.scale_cubes[i], true, scene_manager->loaded_scene);
            }
        }
        break;
        case TransformationType::ROTATION:
            assert(false);
            break;
        }
    }

    static void update_editor_camera(Camera &camera, TransformComponent &component, Scene &scene, InputController *input_controller, r64 delta_time)
    {
        if(ImGui::IsMouseDragging(1))
        {
            set_mouse_lock(true, *scene.renderer);
        }
        else
        {
            set_mouse_lock(false, *scene.renderer);
        }
        
        if(KEY(Key_LeftCtrl))
            return;

        // Update camera
        if(MOUSE(Mouse_Right))
        {
            if(KEY(Key_W))
            {
                scene::set_position(component, component.transform.position + component.transform.forward * (r32)delta_time * 10.0f);
            }

            if(KEY(Key_S))
            {
                scene::set_position(component, component.transform.position - component.transform.forward * (r32)delta_time * 10.0f);
            }

            if(KEY(Key_A))
            {
                scene::set_position(component, component.transform.position - component.transform.right * (r32)delta_time * 10.0f);
            }

            if(KEY(Key_D))
            {
                scene::set_position(component, component.transform.position + component.transform.right * (r32)delta_time * 10.0f);
            }
        }

        if(MOUSE(Mouse_Middle))
        {
            scene::set_position(component, component.transform.position + component.transform.up * input_controller->mouse_y_delta * 0.01f);
            scene::set_position(component, component.transform.position - component.transform.right * input_controller->mouse_x_delta * 0.01f);
        }

        // @Incomplete
        // if(KEY_DOWN(Key_LeftAlt))
        //     center(camera);
        // else if(KEY_UP(Key_LeftAlt))
        //     free_roam(camera);
    
        if(MOUSE(Mouse_Right))
        {
            scene::set_rotation(component, component.transform.euler_angles - math::Vec3((r32)input_controller->mouse_y_delta * 0.1f, 0.0f, 0.0f));
            scene::set_rotation(component, component.transform.euler_angles - math::Vec3(0.0f, (r32)input_controller->mouse_x_delta * 0.1f, 0.0f));
        }

        if(MOUSE_DOWN(Mouse_Right))
        {
            set_mouse_lock(true, *scene.renderer);
        }
        else if(MOUSE_UP(Mouse_Right))
             set_mouse_lock(false, *scene.renderer);
    }

    static void deselect_everything(SceneManager *manager)
    {
        if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
        {
            scene::set_wireframe_enabled(false, manager->selected_entity, manager->loaded_scene);
            manager->selected_entity = { -1 };
            manager->gizmos.active = false;
        }
    }
    
    static EntityHandle get_selected_entity(SceneManager *manager)
    {
        return manager->selected_entity;
    }

    static void select_entity(EntityHandle entity, SceneManager *manager)
    {
        if(IS_ENTITY_HANDLE_VALID(entity) && !HANDLES_EQUAL(entity, manager->selected_entity))
        {
            // Deselect the previously selected entity
            if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                scene::set_wireframe_enabled(false, manager->selected_entity, manager->loaded_scene);                 
            
            manager->selected_entity = entity;
            manager->gizmos.active = true;

            scene::set_wireframe_enabled(true, entity, manager->loaded_scene);
            manager->callbacks.on_entity_selected(entity, manager->loaded_scene);
        }
    }
    
    static void find_all_template_files(SceneManager *scene_manager)
    {
        DirectoryData data = {};
        platform.get_all_files_with_extension("../assets/templates/PLACABLES/", "tmpl", &data, true);
        scene_manager->editor.template_files = data;
    }

    static void editor_setup(SceneManager *manager)
    {
        TransformComponent &main_cam_transform = get_main_camera_transform(manager->loaded_scene);
        TransformComponent &editor_cam_transform = get_transform_comp(manager->editor_camera, manager->loaded_scene);

        scene::set_position(editor_cam_transform, main_cam_transform.transform.position);
        scene::set_rotation(editor_cam_transform, main_cam_transform.transform.euler_angles);
        // Register all debug entities
        for(i32 i = 0; i < 4; i++)
        {
            manager->gizmos.scale_cubes[i] = register_entity_from_template_file("../assets/templates/editor/scale_cube.tmpl", manager->loaded_scene, false);
            set_active(manager->gizmos.scale_cubes[i], false, manager->loaded_scene);
            TransformComponent &transform = get_transform_comp(manager->gizmos.scale_cubes[i], manager->loaded_scene);
            set_position(transform, math::Vec3(0, 1 + i, 0));
            set_hide_in_ui(manager->gizmos.scale_cubes[i], true, manager->loaded_scene);
        }
    }

    static void update_shadow_framebuffer(SceneHandle handle)
    {
        Renderer* renderer = handle.manager->renderer;
        Scene& scene = get_scene(handle);
        Settings& settings = scene.settings;
        
        rendering::FramebufferHandle framebuffer_handle = rendering::get_write_framebuffer_from_pass(renderer->render.shadow_pass, renderer);
        rendering::FramebufferInfo& info = rendering::get_framebuffer(framebuffer_handle, renderer);

        if(info.width != (u32)settings.shadows.map_width ||
           info.height != (u32)settings.shadows.map_height)
        {
            rendering::reload_framebuffer(framebuffer_handle, settings.shadows.map_width, settings.shadows.map_height,renderer);
        }
    }

    static void set_main_camera(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        scene.main_camera_handle = entity_handle;
    }

    static void set_camera_preview(EntityHandle entity_handle, SceneManager *scene_manager)
    {
        scene_manager->camera_preview.show_camera_preview = true;
        scene_manager->camera_preview.handle = entity_handle;
    }

    static void stop_camera_preview(SceneManager *scene_manager)
    {
        scene_manager->camera_preview.show_camera_preview = false;
        scene_manager->camera_preview.handle = EMPTY_ENTITY_HANDLE;
    }

    static void update_camera(Camera &camera, TransformComponent &transform, SceneManager *scene_manager)
    {
        camera.view_matrix = math::look_at_with_target(transform.transform.position, transform.transform.position + transform.transform.forward);
        camera.projection_matrix = math::perspective((r32)scene_manager->renderer->window_width / (r32)scene_manager->renderer->window_height, camera.fov, camera.near_plane, camera.far_plane);
        
        camera.pos = transform.transform.position;
        camera.forward = transform.transform.forward;
        camera.right = transform.transform.right;
    }

    static void update_cameras(Scene &scene, SceneManager *scene_manager)
    {
        for(i32 i = 0; i < scene.camera_component_count; i++)
        {
            CameraComponent &camera_comp = scene.camera_components[i];
            TransformComponent &transform = get_transform_comp(camera_comp.entity, scene.handle);

            update_camera(camera_comp.camera, transform, scene_manager);
        }
    }

    static void set_camera_for_render_pass(EntityHandle camera_entity, rendering::RenderPassHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        scene.render_pass_cameras[handle.handle - 1] = camera_entity;
    }

    static void update_scene_settings(SceneHandle handle)
    {
        const Scene& scene = get_scene(handle);
        const Settings& settings = scene.settings;
        Renderer* renderer = handle.manager->renderer;

        renderer->render.shadow_settings.z_near = settings.shadows.near_plane;
        renderer->render.shadow_settings.z_far = settings.shadows.far_plane;
        renderer->render.shadow_settings.fov = settings.shadows.fov;
        renderer->render.shadow_settings.size = settings.shadows.map_width;

        update_shadow_framebuffer(handle);
    }
    
    static void _set_particle_systems_paused(b32 paused, scene::SceneManager *scene_manager)
    {
        scene::Scene& scene = scene::get_scene(scene_manager->loaded_scene);
    
        for(i32 i = 0; i < scene.particle_system_component_count; i++)
        {
            scene::ParticleSystemComponent comp = scene.particle_system_components[i];
            scene.renderer->particles.api->pause_particle_system(comp.handle, scene.renderer, paused);
        }
    }
    
    static void update_scene_editor(SceneHandle handle, InputController *input_controller, RenderState &render_state, r64 delta_time)
    {
        Scene &scene = get_scene(handle);
        
        SceneManager *manager = handle.manager;

        update_scene_settings(handle);
        
        if(KEY_DOWN(Key_E) && KEY(Key_LeftCtrl))
        {
            stop_camera_preview(manager);
            
            if(manager->mode == SceneMode::RUNNING)
            {
                manager->renderer->api_functions.show_mouse_cursor(true, &render_state);
                manager->mode = SceneMode::EDITING;

                scene::reload_scene(manager->loaded_scene);
                _set_particle_systems_paused(true, manager);

                TransformComponent &main_cam_transform = get_main_camera_transform(manager->loaded_scene);
                TransformComponent &editor_cam_transform = get_transform_comp(manager->editor_camera, manager->loaded_scene);

				scene::set_position(editor_cam_transform, main_cam_transform.transform.position);
				scene::set_rotation(editor_cam_transform, main_cam_transform.transform.euler_angles);

                if(manager->callbacks.on_started_edit_mode)
                    manager->callbacks.on_started_edit_mode(handle);                
            }
            else
            {
                manager->mode = SceneMode::RUNNING;
                
                scene::reload_scene(manager->loaded_scene);
                
                manager->renderer->api_functions.show_mouse_cursor(false, &render_state);

                // Disable wireframes
                /*if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                    scene::set_wireframe_enabled(false, manager->selected_entity, handle);*/
                
                // When exiting to running mode we should make sure to notify the game about it, to ensure that all
                // editor-specific entities are cleaned up before the game is running again.
                if(manager->callbacks.on_exited_edit_mode)
                    manager->callbacks.on_exited_edit_mode(handle);
                
                // Disable gizmos
                manager->gizmos.active = false;

                manager->selected_entity = { -1 };
            }
        }

        if(manager->mode == SceneMode::EDITING)
        {
            Scene &scene = get_scene(manager->loaded_scene);
            
            TransformComponent &camera_transform = get_transform_comp(manager->editor_camera, manager->loaded_scene);

            auto &io = ImGui::GetIO();
            if(!io.WantCaptureKeyboard && (KEY_DOWN(Key_Delete) || KEY_DOWN(Key_Backspace)))
            {
                if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                {
                    delete_entity(manager->selected_entity, manager);
                    
                    manager->selected_entity = { -1 };
                    manager->gizmos.active = false;
                }
            }
            
            if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
            {
                TransformComponent &t = get_transform_comp(manager->selected_entity, handle);
                
                update_transform(t, manager, input_controller, delta_time);
                manager->gizmos.current_distance_to_camera = math::distance(camera_transform.transform.position, t.transform.position) * 0.1f;
                
                if(KEY_DOWN(Key_F))
                {
                    // @Incomplete
                    //set_target(camera, t.transform.position);
                }
            }

            if(KEY_DOWN(Key_Escape))
            {
                deselect_everything(manager);
            }

            if(KEY(Key_LeftCtrl) && KEY_DOWN(Key_D))
            {
                if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                {
                    Entity e = get_entity(manager->selected_entity, manager->loaded_scene);
                    //if(e.savable)
                    //{
                        EntityHandle new_entity = register_entity_from_template_file(e.template_path, manager->loaded_scene, true);
                        for(i32 i = 0; i < e.tags.tag_count; i++)
                        {
                            set_entity_tag(e.tags.tags[i], new_entity, manager->loaded_scene);
                        }
                        
                        TransformComponent &transform = get_transform_comp(e.handle, manager->loaded_scene);
                        TransformComponent &new_transform = get_transform_comp(new_entity, manager->loaded_scene);
                        set_scale(new_transform, transform.transform.scale);
                        set_position(new_transform, transform.transform.position);
                        set_rotation(new_transform, transform.transform.euler_angles);

                        select_entity(new_entity, manager);
                        //}
                }
            }
            
            if(MOUSE_DOWN(Mouse_Left))
            {
                manager->dragging = true;
                
                if(manager->editor.selection_enabled)
                {
                    if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                    {
                        TransformComponent &t = get_transform_comp(manager->selected_entity, handle);

                        math::Vec3 pos = t.transform.position;
                        math::Vec3 start;

                        switch(manager->gizmos.transformation_type)
                        {
                        case TransformationType::POSITION:
                        {
                            start = t.transform.position;
                        }
                        break;
                        case TransformationType::SCALE:
                        {
                            start = t.transform.scale;
                        }
                        break;
                        case TransformationType::ROTATION:
                        assert(false);
                        break;
                        }
                        
                        manager->gizmos.initial_scale = t.transform.scale;
                    
                        TranslationConstraint constraint = TranslationConstraint::NONE;

                        math::Vec3 points[2];
                    
                        math::Ray ray = cast_ray(scene, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);

                        r32 max_distance = GIZMO_TOLERANCE * manager->gizmos.current_distance_to_camera;
                        r32 gizmo_size = manager->gizmos.current_distance_to_camera;
                        r32 current_distance = 1000.0f;
                        Line l1 = line_from_ray(ray);

                        // Check X axis
                        Line l2;
                        l2.start = pos;
                        l2.end = pos + math::Vec3(gizmo_size, 0, 0);
                
                        if(line_vs_line(l1, l2, points))
                        {
                            r32 transform_dist = math::distance(points[1], pos);
                            r32 x_dist = math::distance(points[0], points[1]);
                            if(x_dist < max_distance && points[1].x > pos.x && points[0].x > pos.x && transform_dist < gizmo_size)
                            {
                                current_distance = x_dist;
                                constraint = TranslationConstraint::X;
                                manager->gizmos.current_line = l2;
                                manager->gizmos.initial_offset = points[1] - start;
                            }
                        }

                        // Check Y axis
                        l2.end = pos + math::Vec3(0, gizmo_size, 0);
                
                        if(line_vs_line(l1, l2, points))
                        {
                            r32 transform_dist = math::distance(points[1], pos);
                            r32 y_dist = math::distance(points[0], points[1]);
                            if(y_dist < max_distance && points[1].y > pos.y && points[0].y > pos.y && transform_dist < gizmo_size)
                            {
                                current_distance = y_dist;
                                constraint = TranslationConstraint::Y;
                                manager->gizmos.current_line = l2;
                                manager->gizmos.initial_offset = points[1] - start;
                            }
                        }

                        // Check Z axis
                        l2.end = pos + math::Vec3(0, 0, gizmo_size);

                        if(line_vs_line(l1, l2, points))
                        {
                            r32 transform_dist = math::distance(points[1], pos);
                            r32 z_dist = math::distance(points[0], points[1]);
                            if(z_dist < max_distance && points[1].z > pos.z && points[0].z > pos.z && transform_dist < gizmo_size)
                            {
                                current_distance = z_dist;
                                constraint = TranslationConstraint::Z;
                                manager->gizmos.current_line = l2;
                                manager->gizmos.initial_offset = points[1] - start;
                            }
                        }

                        manager->gizmos.constraint = constraint;
                    }

                    scene::EntityHandle entity = scene::pick_entity(handle, (i32)input_controller->mouse_x, (i32)input_controller->mouse_y);
                    
                    if(HANDLES_EQUAL(entity, manager->gizmos.scale_cubes[3]))
                    {
                        manager->gizmos.scaling_mode = ScalingMode::ALL_AXIS;
                        manager->gizmos.scale_mouse_offset = math::Vec2((r32)input_controller->mouse_x, (r32)manager->renderer->window_height - input_controller->mouse_y);
                    }
                    else if(manager->gizmos.constraint == TranslationConstraint::NONE)
                    {
                        select_entity(entity, manager);
                    }
                }
            }
            
            if(MOUSE_UP(Mouse_Left))
            {
                manager->gizmos.scaling_mode = ScalingMode::SINGLE_AXIS;
                if(IS_ENTITY_HANDLE_VALID(manager->selected_entity))
                {
                    manager->callbacks.on_entity_updated(manager->selected_entity, handle);
                }
                
                manager->gizmos.constraint = TranslationConstraint::NONE;
                manager->dragging = false;
            }

            if(!manager->editor.lock_camera)
            {
                if(manager->camera_preview.show_camera_preview)
                {
                    CameraComponent &preview_cam = get_camera_comp(manager->camera_preview.handle, handle);
                    TransformComponent &preview_transform = get_transform_comp(manager->camera_preview.handle, handle);
                    update_editor_camera(preview_cam.camera, preview_transform, scene, input_controller, delta_time);
                }
                else
                {
					CameraComponent& cam = get_camera_comp(manager->editor_camera, handle);
					TransformComponent& trans = get_transform_comp(manager->editor_camera, handle);
                    update_editor_camera(cam.camera, trans, scene, input_controller, delta_time);
                }
            }
        }
    }

    static void unload_current_scene(SceneManager *scene_manager)
    {
        scene::deactivate_particle_systems(scene_manager->loaded_scene);
        scene_manager->loaded_scene = { -1 , nullptr};
        scene_manager->scene_loaded = false;
    }
    
    static void load_scene(SceneHandle handle, u64 load_flags)
    {
        SceneManager *scene_manager = handle.manager;

        stop_camera_preview(scene_manager);
        
        if(scene_manager->scene_loaded)
        {
            scene::deactivate_particle_systems(scene_manager->loaded_scene);
            scene::Scene &loaded_scene = get_scene(scene_manager->loaded_scene);
            
            if(!loaded_scene.persistent || load_flags & SceneLoadFlags::FREE_CURRENT_SCENE)
            {
                free_instance_buffers(get_scene(scene_manager->loaded_scene));
                loaded_scene.loaded = false;
                free_scene(scene_manager->loaded_scene);
                scene_manager->loaded_scene = { -1 , nullptr};
                scene_manager->scene_loaded = false;
            }
        }
        
        Scene *scene = &get_scene(handle);
        assert(scene->valid);

        scene_manager->loaded_scene = handle;
            
        b32 first_load = !scene->loaded;
        
        if(!scene->loaded)
        {
            if(scene_manager->mode == SceneMode::EDITING)
            {
                if(!IS_ENTITY_HANDLE_VALID(scene->editor_camera))
                    scene->editor_camera = register_entity(COMP_TRANSFORM | COMP_CAMERA, handle, false);
                    
                scene_manager->editor_camera = scene->editor_camera;
                
                CameraComponent &camera_comp = get_camera_comp(scene->editor_camera, handle);
                camera_comp.camera.far_plane = 500.0f;
                scene::set_hide_in_ui(scene->editor_camera, true, handle);
				scene::set_entity_name(scene->editor_camera, "Editor Camera", handle);
            }
            
            if(scene_manager->callbacks.on_scene_will_load)
                scene_manager->callbacks.on_scene_will_load(handle);

            if(scene_manager->mode == SceneMode::EDITING)
            {
                editor_setup(scene_manager);
            
                if(scene_manager->callbacks.on_load)
                    scene_manager->callbacks.on_load(handle);
            }
            
			// @Robustness: This is not good behaviour...
			// Make sure to reget the scene in case a scene was freed in the callback
			scene = &get_scene(handle);

            allocate_instance_buffers(*scene);
            scene->loaded = true;
        }
        else
            scene_manager->editor_camera = scene->editor_camera;

        if(scene_manager->callbacks.on_scene_loaded)
            scene_manager->callbacks.on_scene_loaded(handle, first_load);        
        
        activate_particle_systems(handle);

        Settings& settings = scene->settings;
        
        scene_manager->renderer->render.shadow_settings.z_near = settings.shadows.near_plane;
        scene_manager->renderer->render.shadow_settings.z_far = settings.shadows.far_plane;
        scene_manager->renderer->render.shadow_settings.fov = settings.shadows.fov;

        update_shadow_framebuffer(scene_manager->loaded_scene);
        
        scene_manager->scene_loaded = true;
    }

    Settings& get_scene_settings(SceneHandle handle)
    {
        Scene& scene = get_scene(handle);
        return scene.settings;
    }
    
    // i32 _unused_entity_handle(Scene &scene)
    // {
    //     for(i32 index = scene.current_internal_handle; index < scene.max_entity_count; index++)
    //     {
    //         i32 i = scene._internal_handles[index];
    //         if(i == -1)
    //         {
    //             scene.current_internal_handle = index;
    //             return(index);
    //         }
    //     }
        
    //     // start from 0 afterwards
    //     for(i32 index = 0; index < scene.current_internal_handle; index++)
    //     {
    //         i32 i = scene._internal_handles[index];
    //         if(i == -1)
    //         {
    //             scene.current_internal_handle = index;
    //             return(index);
    //         }
    //     }
        
    //     debug("All entities are in use.");
    //     assert(false);
        
    //     return -1;
    // }
    
    static RenderComponent& _add_render_component(Scene &scene, EntityHandle entity_handle, b32 cast_shadows = true)
    {
        Entity &entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_RENDER;
        
        entity.render_handle = { scene.render_component_count++ };
        scene::RenderComponent &comp = scene.render_components[entity.render_handle.handle];
        comp.wireframe_enabled = false;
        comp.render_pass_count = 0;
        comp.casts_shadows = cast_shadows;
        
        return(comp);
    }

    static RenderComponent& add_render_component(SceneHandle handle, EntityHandle entity_handle, b32 cast_shadows = true)
    {
        Scene &scene = get_scene(handle);
        return _add_render_component(scene, entity_handle, cast_shadows);
    }

    static void setup_instance_buffers(scene::RenderComponent &render, Scene &scene)
    {
        rendering::BufferUsage usage = render.is_static ? rendering::BufferUsage::STATIC : rendering::BufferUsage::DYNAMIC;
            
        for(i32 pass_index = 0; pass_index < render.render_pass_count; pass_index++)
        {
            // We have to look for the right instance buffers or allocate them
            rendering::Material &material_instance = rendering::get_material_instance(render.material_handles[pass_index], scene.renderer);
                    
            if(material_instance.instanced_vertex_attribute_count > 0)
            {
                InstanceBufferData *data = nullptr;
                        
                for(i32 i = 0; i < scene.instance_buffer_data_count; i++)
                {
                    InstanceBufferData &current_data = scene.instance_buffer_data[i];
                    if(current_data.pass_handle.handle == render.render_passes[pass_index].handle && current_data.buffer_handle.handle == render.buffer_handle.handle // The same buffer
                       && current_data.source_material_handle.handle == material_instance.source_material.handle) // The same source material)
                    {
                        data = &current_data;
                        break;
                    }
                }

                if(data)
                {
                    data->max_count++;
                            
                    for(i32 i = 0; i < material_instance.instanced_vertex_attribute_count; i++)
                    {
                        // We got a match!
                        rendering::InstanceBufferHandle instance_buffer_handle = data->instance_buffer_handles[i];
                        i32 max = rendering::get_instance_buffer_max(instance_buffer_handle, scene.renderer);
                                
                        material_instance.instanced_vertex_attributes[i].instance_buffer_handle = instance_buffer_handle;
                                    
                        if(data->max_count > max)
                        {
                            i32 new_max = math::next_power_of_two(max + 1);
                            realloc_instance_buffer(instance_buffer_handle, new_max, scene.renderer);
                        }
                    }
                }
                else
                {
                    // Allocate all the needed buffers
                    data = &scene.instance_buffer_data[scene.instance_buffer_data_count++];
                    data->max_count = 1;
                    data->buffer_handle = render.buffer_handle;
                    data->source_material_handle = material_instance.source_material;
                    data->pass_handle = render.render_passes[pass_index];
                            
                    for(i32 i = 0; i < material_instance.instanced_vertex_attribute_count; i++)
                    {
                        rendering::InstanceBufferHandle instance_buffer_handle = rendering::allocate_instance_buffer(material_instance.instanced_vertex_attributes[i].attribute.type, math::next_power_of_two(1), usage, scene.renderer);
                        material_instance.instanced_vertex_attributes[i].instance_buffer_handle = instance_buffer_handle;
                        data->instance_buffer_handles[data->instance_buffer_count++] = instance_buffer_handle;
                    }
                }
            }
        }
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, rendering::MaterialInstanceHandle material, RenderComponent &comp, Renderer *renderer)
    {
        comp.render_passes[comp.render_pass_count] = render_pass_handle;
        rendering::Material &instance = get_material_instance(material, renderer);
        comp.original_materials[comp.render_pass_count] = instance.source_material;
        comp.material_handles[comp.render_pass_count] = create_material_instance(renderer, instance.source_material);
        comp.render_pass_count++;
    }

    static void add_to_render_pass(const char *pass_name, rendering::MaterialInstanceHandle material, RenderComponent &comp, Renderer *renderer)
    {
        rendering::RenderPassHandle render_pass_handle = rendering::get_render_pass_handle_for_name(pass_name, renderer);
        add_to_render_pass(render_pass_handle, material, comp, renderer);
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, RenderComponent &comp, Renderer *renderer)
    {
        add_to_render_pass(render_pass_handle, comp.material_handles[0], comp, renderer);
    }      

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, RenderComponent& render_comp, SceneHandle &scene_handle)
    {
        add_to_render_pass(render_pass_handle, render_comp, scene_handle.manager->renderer);
        
        Scene &scene = get_scene(scene_handle);
        if(scene.loaded)
        {
            setup_instance_buffers(render_comp, scene);
        }
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, EntityHandle entity, SceneHandle &scene_handle)
    {
        Entity &e = get_entity(entity, scene_handle);
        if(e.comp_flags & COMP_RENDER)
        {
            RenderComponent &render_comp = get_render_comp(entity, scene_handle);
            add_to_render_pass(render_pass_handle, render_comp, scene_handle);
        }

        for(i32 i = 0; i < e.child_count; i++)
        {
            add_to_render_pass(render_pass_handle, e.children[i], scene_handle);
        }
    }

    static void add_to_render_pass(rendering::RenderPassHandle render_pass_handle, rendering::MaterialInstanceHandle material, EntityHandle entity, SceneHandle &scene)
    {
        RenderComponent &render_comp = get_render_comp(entity, scene);
        add_to_render_pass(render_pass_handle, material, render_comp, scene.manager->renderer);
    }

    static void add_all_to_render_pass(rendering::RenderPassHandle render_pass_handle, SceneHandle handle)
    {
        Scene &scene = scene::get_scene(handle);
        for(i32 i = 0; i < scene.render_component_count; i++)
        {
            add_to_render_pass(render_pass_handle, scene.render_components[i], handle);
        }
    }

    static void add_all_with_tag_to_render_pass(rendering::RenderPassHandle render_pass_handle, const char* tag, SceneHandle handle)
    {
        if(render_pass_handle.handle > global_max_render_passes)
        {
            debug_log("Invalid render pass handle: %d, max is %d", render_pass_handle.handle, global_max_render_passes);
            assert(false);
            return;
        }

        Scene &scene = scene::get_scene(handle);
        for(i32 i = 0; i < scene.entities.count; i++)
        {
            Entity& entity = scene.entities[i];
            if(has_tag(tag, entity.handle, handle))
            {
                if(has_render_component(entity.handle, handle))
                {
                    RenderComponent& render_comp = get_render_comp(entity.handle, handle);
                    add_to_render_pass(render_pass_handle, render_comp, handle.manager->renderer);
                }

                for(i32 c = 0; c < entity.child_count; c++)
                {
                    EntityHandle& child_handle = entity.children[c];
                    if(!IS_ENTITY_HANDLE_VALID(child_handle))
                    {
                        debug_log("Referencing invalid child");
                        continue;
                    }
                    
                    Entity& child = get_entity(child_handle, handle);

                    // if(child.child_count > 0)
                    // {
                    //     // debug_log("Warning: nested children not supported here");
                    //     // continue;
                    // }
                    
                    if(has_render_component(child.handle, handle))
                    {
                        RenderComponent& child_render = get_render_comp(child.handle, handle);
                        add_to_render_pass(render_pass_handle, child_render, handle.manager->renderer);
                    }
                }
            }
        }
    }

    static void remove_from_render_pass(rendering::RenderPassHandle render_pass_handle, scene::EntityHandle entity, SceneHandle& scene)
    {
        if(!has_render_component(entity, scene))
        {
            // @Incomplete/@Hack: Fix this some other way?
            return;
        }
        
        RenderComponent &render_comp = get_render_comp(entity, scene);
        for(i32 i = 0; i < render_comp.render_pass_count; i++)
        {
            if(render_comp.render_passes[i].handle == render_pass_handle.handle)
            {
                rendering::delete_material_instance(scene.manager->renderer, render_comp.material_handles[i]);
                
                render_comp.render_passes[i] = render_comp.render_passes[render_comp.render_pass_count - 1];
                render_comp.material_handles[i] = render_comp.material_handles[render_comp.render_pass_count - 1];
				render_comp.original_materials[i] = render_comp.original_materials[render_comp.render_pass_count - 1];
                render_comp.render_pass_count--;
                break;
            }
        }
    }

    static TransformComponent& _add_transform_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity& entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_TRANSFORM;
        entity.transform_handle = { scene.transform_component_count++ };

        scene::TransformComponent &comp = scene.transform_components[entity.transform_handle.handle];
        
        comp.transform = rendering::create_transform(math::Vec3(0.0f), math::Vec3(1.0f), math::Vec3(0.0f));
        comp.entity = entity_handle;
        return(comp);
    }

    static TransformComponent& add_transform_component(SceneHandle handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_transform_component(scene, entity_handle);
    }

    static AnimatorComponent& add_animator_component(SceneHandle handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        Entity& entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_ANIMATOR;
        entity.animator_handle = { scene.animator_component_count++ };

        scene.animator_components[entity.animator_handle.handle] = push_struct(&scene.memory_arena, AnimatorComponent);
        
        scene::AnimatorComponent &comp = *scene.animator_components[entity.animator_handle.handle];
        
        comp.running = false;
        comp.anim_count = 0;
        comp.entity = entity_handle;
        return(comp);
    }
    
    static ParticleSystemComponent & _add_particle_system_component(Scene &scene, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material)
    {
        Entity& entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_PARTICLES;
        
        entity.particle_system_handle = {scene.particle_system_component_count++};
        scene::ParticleSystemComponent &comp = scene.particle_system_components[entity.particle_system_handle.handle];
        
        comp.handle = scene.renderer->particles.api->create_particle_system(scene.renderer, max_particles, material, attributes.buffer);
        ParticleSystemInfo* info = scene.renderer->particles.api->get_particle_system_info(comp.handle, scene.renderer);
        assert(info);
        
        info->attributes = attributes;

        if(attributes.play_on_awake)
        {
            scene.renderer->particles.api->start_particle_system(comp.handle, scene.renderer);
            scene.renderer->particles.api->pause_particle_system(comp.handle, scene.renderer, false);
        }
        
        return(comp);
    }

    static ParticleSystemComponent & add_particle_system_component(SceneHandle handle, EntityHandle entity_handle, ParticleSystemAttributes attributes, i32 max_particles, rendering::MaterialHandle material)
    {
        Scene &scene = get_scene(handle);
        return _add_particle_system_component(scene, entity_handle, attributes, max_particles, material);
    }

    static LightComponent& _add_light_component(Scene &scene, EntityHandle entity_handle)
    {
        Entity& entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_LIGHT;
        entity.light_handle = { scene.light_component_count++ };
        scene::LightComponent &comp = scene.light_components[entity.light_handle.handle];
        
        return(comp);
    }
    
    static LightComponent & add_light_component(SceneHandle &handle, EntityHandle entity_handle)
    {
        Scene &scene = get_scene(handle);
        return _add_light_component(scene, entity_handle);
    }

    static CameraComponent& _add_camera_component(EntityHandle entity_handle, Scene &scene)
    {
        Entity& entity = list::get(&scene.entities, entity_handle.handle);
        entity.comp_flags |= COMP_CAMERA;
        entity.camera_handle = { scene.camera_component_count++ };
        
        Camera camera = {};
        
        camera.fov = 90 * DEGREE_IN_RADIANS;
        camera.near_plane = 0.1f;
        camera.far_plane = 50.0f;
        
        scene::CameraComponent &comp = scene.camera_components[entity.camera_handle.handle];
        comp.entity = entity_handle;
        comp.camera = camera;
        
        return(comp);
    }
    
    static CameraComponent & add_camera_component(EntityHandle entity_handle, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return _add_camera_component(entity_handle, scene);
    }

    // Returns a new valid "EntityHandle". "comp_flags" Specifies the components that the entity should contain.
    static EntityHandle _register_entity(u64 comp_flags, Scene &scene, b32 savable = false)
    {
        Entity new_entity = {};
        new_entity.savable = savable;
        new_entity.selection_enabled = true;

        new_entity.comp_flags = comp_flags;
        new_entity.child_count = 0;
        new_entity.parent = EMPTY_ENTITY_HANDLE;
        i32 new_handle = list::add(&scene.entities, new_entity);
        
        EntityHandle handle = { new_handle };
        handle.scene_handle = scene.handle;
        Entity& entity = list::get(&scene.entities, handle.handle);
        entity.handle = handle;
        
        if(comp_flags & COMP_TRANSFORM)
        {
            _add_transform_component(scene, handle);
        }
        
        if(comp_flags & COMP_RENDER)
        {
            _add_render_component(scene, handle, true);
        }

        if(comp_flags & COMP_LIGHT)
        {
            _add_light_component(scene, handle);
        }

        if(comp_flags & COMP_CAMERA)
        {
            _add_camera_component(handle, scene);
        }

        return(handle);
    }

    static EntityHandle register_entity(u64 comp_flags, SceneHandle scene_handle, b32 savable = false)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity(comp_flags, scene, savable);
    }

    static EntityHandle register_entity_with_entity_data(u32 type_id, EntityData *data, SceneHandle scene_handle, b32 savable = false)
    {
        EntityHandle handle = register_entity(COMP_TRANSFORM, scene_handle, savable);
        Entity &entity = get_entity(handle, scene_handle);
        entity.entity_data = data;
        entity.type_info = *get_registered_type(type_id, scene_handle.manager);
        entity.type = type_id;
        return handle;
    }
    
    static TemplateHandle _create_template_copy_with_new_render_data(rendering::Material *temp_materials, EntityTemplate *template_to_copy, EntityTemplateState &template_state, Renderer *renderer, const rendering::MeshObjectData &obj_data)
    {
        EntityTemplate new_template = *template_to_copy;
        new_template.child_count = 0;
        new_template.render.buffer_handle = obj_data.buffer;
        new_template.render.mesh_scale = obj_data.mesh_scale;
        new_template.render.bounding_box = obj_data.bounding_box;

        const rendering::MaterialPair &pair = obj_data.pair;
        
        for(i32 i = 0; i < pair.pass_count; i++)
        {
            new_template.render.material_handles[i] = create_material(renderer, temp_materials[pair.passes[i]]);
            strcpy(new_template.render.render_pass_names[i], pair.pass_names[i]);
        }

        template_state.templates[template_state.template_count++] = new_template;
        return { template_state.template_count };
    }

    static void add_render_pass(EntityTemplate *templ, const char *pass_name, rendering::MaterialHandle material)
    {
        strcpy(templ->render.render_pass_names[templ->render.render_pass_count], pass_name);
        templ->render.material_handles[templ->render.render_pass_count] = material;
        templ->render.render_pass_count++;
    }

    static void _load_template(EntityTemplate *templ, const char *path, EntityTemplateState &template_state, Scene &scene)
    {
        // This will be used at the end of the function to look for child entities that were described in the obj-file
		
		templ->render.material_handle = { -1 };
        templ->type_id = -1;
        
        rendering::PassMaterial pass_materials[4];
        i32 pass_material_count = 0;
        
		rendering::MeshObjectInfo obj_info;
        obj_info.object_count = 0;
        
        FILE *file = fopen(path, "r");
        
        if(file)
        {
            sprintf(templ->file_path, "%s", path);

            i32 last_slash = 0;
            for(i32 i = 0; i < (i32)strlen(path); i++)
            {
                if(path[i] == '/')
                    last_slash = i;
            }

            i32 real_index = 0;
            for(i32 i = last_slash + 1; i < (i32)strlen(path); i++)
            {
                char c = path[i];
                if(c != '.')
                    templ->name[real_index++] = c;
                else
                {
                    templ->name[real_index++] = '\0';
                    break;
                }
            }
            
            char buffer[256];

            templ->hide_in_ui = false;

            while(fgets(buffer, 256, file))
            {
                if(starts_with(buffer, "type"))
                {
                    sscanf(buffer, "type_id: %d", &templ->type_id);
                }
                else if(starts_with(buffer, "hide_in_ui"))
                {
                    sscanf(buffer, "hide_in_ui: %d", &templ->hide_in_ui);
                }
                else if(starts_with(buffer, "-transform"))
                {
                    templ->comp_flags |= COMP_TRANSFORM;
                    templ->transform.position = math::Vec3();
                    templ->transform.scale = math::Vec3(1, 1, 1);
                    templ->transform.rotation = math::Vec3(0, 0, 0);
                    templ->transform.child_handle = EMPTY_TEMPLATE_HANDLE;
                    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "position"))
                        {
                            sscanf(buffer, "position: %f %f %f\n", &templ->transform.position.x, &templ->transform.position.y, &templ->transform.position.z);
                        }
                        else if(starts_with(buffer, "scale"))
                        {
                            sscanf(buffer, "scale: %f %f %f\n", &templ->transform.scale.x, &templ->transform.scale.y, &templ->transform.scale.z);
                        }
                        else if(starts_with(buffer, "rotation"))
                        {
                            sscanf(buffer, "rotation: %f %f %f\n", &templ->transform.rotation.x, &templ->transform.rotation.y, &templ->transform.rotation.z);
                        }
                        else if(starts_with(buffer, "child_templ"))
                        {
                            // @Incomplete
                            // Child template referenced
                        }
                    }
                }
                else if(starts_with(buffer, "-tags"))
                {
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        char tag_buf[256];
                        sscanf(buffer, "%s", tag_buf);

                        if(templ->tags.tag_count < MAX_ENTITY_TAGS)
                        {
                            strncpy(templ->tags.tags[templ->tags.tag_count++], tag_buf, strlen(tag_buf) + 1);
                        }
                    }
                }
				else if (starts_with(buffer, "-render"))
				{
					templ->comp_flags |= COMP_RENDER;
                    templ->render.is_static = false;
                    templ->render.render_mode = scene::RenderMode::RENDER_OPAQUE;
                    
					while (fgets(buffer, 256, file) && !starts_with(buffer, "-"))
					{
						// FIRST PARSE ALL SHADER PASS INFORMATION
                        if(starts_with(buffer, "mode:"))
                        {
                            char mode[32];
                            sscanf(buffer, "mode: %[^\n]", mode);

                            if(starts_with(mode, "opaque"))
                                templ->render.render_mode = scene::RenderMode::RENDER_OPAQUE;
                            else if(starts_with(mode, "transparent"))
                                templ->render.render_mode = scene::RenderMode::RENDER_TRANSPARENT;
                        }
                        else if(starts_with(buffer, "static:"))
                        {
                            sscanf(buffer, "static: %d", &templ->render.is_static);
                        } 
                        else if (starts_with(buffer, "shd"))
						{
							if (starts_with(buffer, "shd::uvs:"))
							{
                                char *pass = nullptr;

                                if(templ->render.render_mode == scene::RenderMode::RENDER_OPAQUE)
                                    pass = "opaque";
                                else if(templ->render.render_mode == scene::RenderMode::RENDER_TRANSPARENT)
                                    pass = "transparency";
                                
								rendering::PassMaterial pass_mat = {};
								pass_mat.pass_type = rendering::PassType::WITH_UVS;

                                
								strncpy(pass_mat.pass_name, pass, strlen(pass) + 1);

								char shader_file[256];
								sscanf(buffer, "shd::uvs: %s", shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}
							else if (starts_with(buffer, "shd::no_uvs:"))
							{
                                char *pass = nullptr;

                                if(templ->render.render_mode == scene::RenderMode::RENDER_OPAQUE)
                                    pass = "opaque";
                                else if(templ->render.render_mode == scene::RenderMode::RENDER_TRANSPARENT)
                                    pass = "transparency";
                                
								rendering::PassMaterial pass_mat = {};
								pass_mat.pass_type = rendering::PassType::NO_UVS;
								strncpy(pass_mat.pass_name, pass, strlen(pass) + 1);

								char shader_file[256];
								sscanf(buffer, "shd::no_uvs: %s", shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}
							else if (starts_with(buffer, "shd::shadows:"))
							{
								rendering::PassMaterial pass_mat = {};
								pass_mat.pass_type = rendering::PassType::SHADOWS;
								strcpy(pass_mat.pass_name, SHADOW_PASS);

								char shader_file[256];
								sscanf(buffer, "shd::shadows: %s", shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}
							else if (starts_with(buffer, "shd::shadows_uvs:"))
							{
								rendering::PassMaterial pass_mat = {};
								pass_mat.pass_type = rendering::PassType::SHADOWS_WITH_UVS;
								strcpy(pass_mat.pass_name, SHADOW_PASS);

								char shader_file[256];
								sscanf(buffer, "shd::shadows_uvs: %s", shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}
							else if (starts_with(buffer, "shd::pass"))
							{
								rendering::PassMaterial pass_mat = {};

								pass_mat.pass_type = rendering::PassType::NONE;

								char shader_file[256];
								sscanf(buffer, "shd::pass: %s - %s", pass_mat.pass_name, shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}
							else if (starts_with(buffer, "shd:"))
							{
                                char *pass = nullptr;

                                if(templ->render.render_mode == scene::RenderMode::RENDER_OPAQUE)
                                    pass = "opaque";
                                else if(templ->render.render_mode == scene::RenderMode::RENDER_TRANSPARENT)
                                    pass = "transparency";
                                
								rendering::PassMaterial pass_mat = {};
								pass_mat.pass_type = rendering::PassType::STANDARD;
								strncpy(pass_mat.pass_name, pass, strlen(pass) + 1);

								char shader_file[256];
								sscanf(buffer, "shd: %s", shader_file);

								rendering::ShaderHandle shader = rendering::load_shader(scene.renderer, shader_file);
								pass_mat.material = create_material_copyable(scene.renderer, shader);
								pass_materials[pass_material_count++] = pass_mat;
							}

						}
						else if (starts_with(buffer, "obj"))
						{
							char obj_file[256];
							sscanf(buffer, "obj: %s", obj_file);

							obj_info = rendering::load_obj(scene.renderer, obj_file);

                            if(obj_info.object_count == 1)
                            {
                                templ->render.mesh_scale = obj_info.data[0].mesh_scale;
                                templ->render.bounding_box = obj_info.data[0].bounding_box;
                            }
						}
						else if (starts_with(buffer, "prim"))
						{
							char *prim_type = buffer + sizeof(char) * 6;

							if (starts_with(prim_type, "cube"))
							{
								obj_info.data[0].buffer = rendering::create_cube(scene.renderer, &templ->render.mesh_scale, &templ->render.bounding_box);
								obj_info.data[0].use_one_material = true;
								obj_info.data[0].use_material = true;
								obj_info.object_count++;
							}
							else if (starts_with(prim_type, "plane"))
							{
								obj_info.data[0].buffer = rendering::create_plane(scene.renderer, &templ->render.mesh_scale, &templ->render.bounding_box);
								obj_info.data[0].use_one_material = true;
								obj_info.data[0].use_material = true;
								obj_info.object_count++;
							}
						}
						else if (starts_with(buffer, "ignore depth"))
						{
							sscanf(buffer, "ignore depth: %d\n", &templ->render.ignore_depth);
						}
						else if (starts_with(buffer, "receives shadows"))
						{
							//sscanf(buffer, "receives shadows: %d\n", &templ->render.receives_shadows);
						}
						else if (starts_with(buffer, "cast shadows"))
						{
							sscanf(buffer, "cast shadows: %d\n", &templ->render.casts_shadows);
						}
						else if (starts_with(buffer, "mtl"))
						{
							sscanf(buffer, "mtl: %s", obj_info.mtl_file_path);
							obj_info.has_mtl = true;
						}
					}

					b32 has_shadow_shader = false;
					b32 has_uv_shadow_shader = false;

					for (i32 i = 0; i < pass_material_count; i++)
					{
						if (pass_materials[i].pass_type == rendering::PassType::SHADOWS)
						{
							has_shadow_shader = true;
						}
						else if (pass_materials[i].pass_type == rendering::PassType::SHADOWS_WITH_UVS)
						{
							has_uv_shadow_shader = true;
						}
					}

					if (templ->render.casts_shadows)
					{
						if (!has_shadow_shader)
						{
							rendering::Material shadow_material = scene.renderer->render.materials[scene.renderer->render.shadow_map_material.handle];

							rendering::PassMaterial pass;
							pass.pass_type = rendering::PassType::SHADOWS;
							pass.material = shadow_material;
							strcpy(pass.pass_name, SHADOW_PASS);
							pass_materials[pass_material_count++] = pass;
						}

						if (!has_uv_shadow_shader)
						{
							if (!has_shadow_shader)
							{
								rendering::Material shadow_material = scene.renderer->render.materials[scene.renderer->render.shadow_map_material.handle];

								rendering::PassMaterial pass;
								pass.pass_type = rendering::PassType::SHADOWS_WITH_UVS;
								pass.material = shadow_material;
								strcpy(pass.pass_name, SHADOW_PASS);
								pass_materials[pass_material_count++] = pass;
							}
						}
					}
                        
                    // PARSE YOUR ANUS!
                    assert(obj_info.has_mtl);

                    rendering::MaterialPair mat_pairs[64];
                    i32 mat_pair_count = 0;
                        
					rendering::Material *temp = (rendering::Material*)malloc(sizeof(rendering::Material) * 128);
					i32 temp_count = 0;

                    load_materials_from_mtl(temp, &temp_count, pass_materials, pass_material_count, mat_pairs, &mat_pair_count, obj_info.mtl_file_path, scene.renderer);

                    for(i32 i = 0; i < obj_info.object_count; i++)
                    {
                        rendering::MeshObjectData &data = obj_info.data[i];
                            
                        if(data.use_material)
                        {
                            for(i32 j = 0; j < mat_pair_count; j++)
                            {
                                rendering::MaterialPair &pair = mat_pairs[j];

                                if(strcmp(pair.name, "glitch_default") == 0 || (strcmp(pair.name, data.material_name) == 0 && strlen(pair.name) == strlen(data.material_name)))
                                {
                                    data.pair = pair;
                                }
                            }
                        }
						else
						{
							for (i32 j = 0; j < mat_pair_count; j++)
							{
								rendering::MaterialPair &pair = mat_pairs[j];

								if (strcmp(pair.name, "glitch_default") == 0)
								{
									data.pair = pair;
								}
							}
						}
                    }

                    rendering::MaterialPair pair = obj_info.data[0].pair;
                    templ->render.buffer_handle = obj_info.data[0].buffer;
                    
                    for(i32 i = 0; i < pair.pass_count; i++)
                    {
                        templ->render.material_handles[i] = create_material(scene.renderer, temp[pair.passes[i]]);
                        strcpy(templ->render.render_pass_names[i], pair.pass_names[i]);
                    }

					templ->render.render_pass_count = pair.pass_count;

                    if(obj_info.object_count > 1)
                    {
                        for(i32 i = 0; i < obj_info.object_count; i++)
                        {
                            const rendering::MeshObjectData &data = obj_info.data[i];
                            templ->child_handles[templ->child_count++] = _create_template_copy_with_new_render_data(temp, templ, template_state, scene.renderer, data);
                            
                        }
            
                        templ->comp_flags = templ->comp_flags & ~ COMP_RENDER;
                    }

					free(temp);
                }
                else if(starts_with(buffer, "-particles"))
                {
                    templ->comp_flags |= COMP_PARTICLES;
		    
                    templ->particles.max_particles = 0;
                    templ->particles.color_over_lifetime.value_count = 0;
                    templ->particles.size_over_lifetime.value_count = 0;
                    templ->particles.speed_over_lifetime.value_count = 0;
                    templ->particles.material_handle = {};
                    templ->particles.shader_handle = {};
		    
                    ParticleSystemAttributes attributes = scene.renderer->particles.api->get_default_attributes();
		    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "max_particles"))
                        {
                            sscanf(buffer, "max_particles: %d", &templ->particles.max_particles);
                        }
                        else if(starts_with(buffer, "obj"))
                        {
                            // char obj_file[256];
                            // sscanf(buffer, "obj: %s", obj_file);

                            // attributes.buffer = rendering::load_obj(scene.renderer, obj_file, &templ->particles.material_handle, nullptr);
                        }
                        else if(starts_with(buffer, "shd"))
                        {
                            char shader_file[256];
                            sscanf(buffer, "shd: %s", shader_file);
                            rendering::ShaderHandle shader_handle = rendering::load_shader(scene.renderer, shader_file);
                            templ->particles.material_handle = rendering::create_material(scene.renderer, shader_handle);

                            // @Incomplete: Multiple passes?
                            // Add the pass information
                            templ->particles.shader_handle = shader_handle;
                        }
                        else if(starts_with(buffer, "play_on_awake"))
                        {
                            sscanf(buffer, "play_on_awake: %d", &attributes.play_on_awake);
                        }
                        else if(starts_with(buffer, "start_size"))
                        {
                            if(attributes.size.type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "start_size: %f", &attributes.size.constant.value);
                                attributes.size.type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_start_size"))
                        {
                            sscanf(buffer, "random_start_size: %f %f", &attributes.size.random_between_two_constants.v0, &attributes.size.random_between_two_constants.v1);
                            attributes.size.type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "start_color"))
                        {
                            sscanf(buffer, "start_color: %f %f %f %f", &attributes.start_color.r, &attributes.start_color.g, &attributes.start_color.b, &attributes.start_color.a);
                        }
                        else if(starts_with(buffer, "start_speed"))
                        {
                            if(attributes.speed.type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "start_speed: %f", &attributes.speed.constant.value);
                                attributes.speed.type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_start_speed"))
                        {
                            sscanf(buffer, "random_start_speed: %f %f", &attributes.speed.random_between_two_constants.v0, &attributes.speed.random_between_two_constants.v0);
                            attributes.speed.type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "life_time"))
                        {
                            if(attributes.lifetime.type != StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS)
                            {
                                sscanf(buffer, "life_time: %f", &attributes.lifetime.constant.value);
                                attributes.lifetime.type = StartParameterType::CONSTANT;
                            }
                        }
                        else if(starts_with(buffer, "random_life_time"))
                        {
                            sscanf(buffer, "random_life_time: %f %f", &attributes.lifetime.random_between_two_constants.v0, &attributes.lifetime.random_between_two_constants.v1);
                            attributes.lifetime.type = StartParameterType::RANDOM_BETWEEN_TWO_CONSTANTS;
                        }
                        else if(starts_with(buffer, "per_second"))
                        {
                            sscanf(buffer, "per_second: %d", &attributes.particles_per_second);
                        }
                        else if(starts_with(buffer, "spread"))
                        {
                            sscanf(buffer, "spread: %f", &attributes.spread);
                        }
                        else if(starts_with(buffer, "direction"))
                        {
                            sscanf(buffer, "direction: %f %f %f", &attributes.direction.x, &attributes.direction.y, &attributes.direction.z);
                        }
                        else if(starts_with(buffer, "texture"))
                        {
                            char texture_path[128];
                            sscanf(buffer, "texture: %s", texture_path);
                            load_texture(texture_path, scene.renderer, LINEAR, REPEAT, TextureFormat::RGBA, TextureUsage::STATIC, attributes.texture_handle);
                        }
                        else if(starts_with(buffer, "gravity"))
                        {
                            sscanf(buffer, "gravity: %f", &attributes.gravity);
                        }
                        else if(starts_with(buffer, "emitter_func"))
                        {
                            char func_name[32];
                            sscanf(buffer, "emitter_func: %s", func_name);

                            attributes.emission_module.emitter_func_type = EmissionFuncType::RANDOM_DIRECTION;
                            if(starts_with(func_name, "2d_square_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_2D_RANDOM;
                            }
                            else if(starts_with(func_name, "2d_square"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_2D;
                            }
                            else if(starts_with(func_name, "random_dir"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::RANDOM_DIRECTION;
                            }
                            else if(starts_with(func_name, "dir"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DIRECTION;
                            }
                            else if(starts_with(func_name, "square_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE_RANDOM;
                            }
                            else if(starts_with(func_name, "square"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::SQUARE;
                            }
                            else if(starts_with(func_name, "disc_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DISC_RANDOM;
                            }
                            else if(starts_with(func_name, "disc"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::DISC;
                            }
                            else if(starts_with(func_name, "circle_random"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::CIRCLE_RANDOM;
                            }
                            else if(starts_with(func_name, "circle"))
                            {
                                attributes.emission_module.emitter_func_type = EmissionFuncType::CIRCLE;
                            }                            
                        }
                        else if(starts_with(buffer, "emission_min"))
                        {
                            sscanf(buffer, "emission_min: %f", &attributes.emission_module.min);
                        }
                        else if(starts_with(buffer, "emission_max"))
                        {
                            sscanf(buffer, "emission_max: %f", &attributes.emission_module.max);
                        }
                        else if(starts_with(buffer, "prewarm"))
                        {
                            sscanf(buffer, "prewarm: %d", &attributes.prewarm);
                        }
                        else if(starts_with(buffer, "looping"))
                        {
                            sscanf(buffer, "looping: %d", &attributes.looping);
                        }
                        else if(starts_with(buffer, "duration"))
                        {
                            sscanf(buffer, "duration: %f", &attributes.duration);
                        }
                        else if(starts_with(buffer, "base_position"))
                        {
                            sscanf(buffer, "base_position: %f %f %f", &attributes.base_position.x, &attributes.base_position.y, &attributes.base_position.z);
                        }
                        else if(starts_with(buffer, "size_key"))
                        {
                            math::Vec2 &value = templ->particles.size_over_lifetime.values[templ->particles.size_over_lifetime.value_count];
                            r32 &key = templ->particles.size_over_lifetime.keys[templ->particles.size_over_lifetime.value_count];
                            sscanf(buffer, "size_key: %f %f %f", &key, &value.x, &value.y);
                            templ->particles.size_over_lifetime.value_count++;
                        }
                        else if(starts_with(buffer, "color_key"))
                        {
                            b32 keep_start_color = false;
                            for(size_t i = 0; i < strlen(buffer); i++)
                            {
                                if(buffer[i] == '-')
                                {
                                    keep_start_color = true;
                                    break;
                                }
                            }

                            math::Rgba &value = templ->particles.color_over_lifetime.values[templ->particles.color_over_lifetime.value_count];
                            r32 &key = templ->particles.color_over_lifetime.keys[templ->particles.color_over_lifetime.value_count];

                            if(keep_start_color)
                            {
                                sscanf(buffer, "color_key: %f - %f", &key, &value.a);
                                value.r = attributes.start_color.r;
                                value.g = attributes.start_color.g;
                                value.b = attributes.start_color.b;
                            }
                            else
                            {
                                sscanf(buffer, "color_key: %f %f %f %f %f", &key, &value.r, &value.g, &value.b, &value.a);
                            }
                            
                            templ->particles.color_over_lifetime.value_count++;
                        }
                        else if(starts_with(buffer, "speed_key"))
                        {
                            r32 &value = templ->particles.speed_over_lifetime.values[templ->particles.speed_over_lifetime.value_count];
                            r32 &key = templ->particles.speed_over_lifetime.keys[templ->particles.speed_over_lifetime.value_count];
                            sscanf(buffer, "speed_key: %f %f", &key, &value);
                            templ->particles.speed_over_lifetime.value_count++;
                        }
                    }

                    templ->particles.attributes = attributes;
                }
                else if(starts_with(buffer, "-light"))
                {
                    templ->comp_flags |= COMP_LIGHT;
                    
                    while(fgets(buffer, 256, file) && !starts_with(buffer, "-"))
                    {
                        if(starts_with(buffer, "type"))
                        {
                            char buf[256];
                            sscanf(buffer, "type: %s", buf);
                            if(starts_with(buf, "directional"))
                            {
                                templ->light.type = LightType::DIRECTIONAL;
                            }
                            else if(starts_with(buf, "point"))
                            {
                                templ->light.type = LightType::POINT;                                
                            }
                            else
                            {
                                log_error("ERROR in template loading: Unknown light type");
                            }
                        }
                        else if(starts_with(buffer, "direction"))
                        {
                            assert(templ->light.type == LightType::DIRECTIONAL);
                        }
                        else if(starts_with(buffer, "ambient"))
                        {
                            sscanf(buffer, "ambient: %f %f %f", &templ->light.ambient.x, &templ->light.ambient.y, &templ->light.ambient.z);
                        }
                        else if(starts_with(buffer, "diffuse"))
                        {
                            sscanf(buffer, "diffuse: %f %f %f", &templ->light.diffuse.x, &templ->light.diffuse.y, &templ->light.diffuse.z);
                        }
                        else if(starts_with(buffer, "specular"))
                        {
                            sscanf(buffer, "specular: %f %f %f", &templ->light.specular.x, &templ->light.specular.y, &templ->light.specular.z);                            
                        }
                        else if(starts_with(buffer, "position"))
                        {
                            assert(templ->light.type == LightType::POINT);
                            sscanf(buffer, "position: %f %f %f", &templ->light.point.position.x, &templ->light.point.position.y, &templ->light.point.position.z);
                        }
                        else if(starts_with(buffer, "constant"))
                        {
                            assert(templ->light.type == LightType::POINT);
                            sscanf(buffer, "constant: %f", &templ->light.point.constant);
                        }
                        else if(starts_with(buffer, "linear"))
                        {
                            assert(templ->light.type == LightType::POINT);
                            sscanf(buffer, "linear: %f", &templ->light.point.linear);
                        }
                        else if(starts_with(buffer, "quadratic"))
                        {
                            assert(templ->light.type == LightType::POINT);
                            sscanf(buffer, "quadratic: %f", &templ->light.point.quadratic);
                        }
                    }
                }
            }
            
            fclose(file);
        }
        else
        {
            log_error("ERROR in template loading: Could not find file %s", path);
            assert(false);
        }
    }

#define EMPTY_TRANSFORM { math::Vec3(), math::Vec3(1, 1, 1), math::Vec3(), EMPTY_COMP_HANDLE, EMPTY_COMP_HANDLE };

    static inline void _set_entity_name(EntityHandle handle, const char *name, Scene& scene)
    {
        Entity& entity = list::get(&scene.entities, handle.handle);
        strcpy(entity.name, name);
    }

    static inline void set_entity_name(EntityHandle handle, const char *name, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        _set_entity_name(handle, name, scene);
    }

    static inline void _set_entity_template_path(EntityHandle handle, const char *template_path, Scene& scene)
    {
        Entity& entity = list::get(&scene.entities, handle.handle);
        strcpy(entity.template_path, template_path);
    }

    static inline void _set_entity_type(EntityHandle handle, u32 type, Scene &scene)
    {
        Entity& entity = list::get(&scene.entities, handle.handle);
        entity.type = type;
    }

    static EntityHandle _register_entity_with_template(EntityTemplate &templ, Scene &scene, b32 savable)
    {
        EntityHandle handle = _register_entity(templ.comp_flags, scene, savable);
        _set_entity_name(handle, templ.name, scene);
        _set_entity_template_path(handle, templ.file_path, scene);
        set_hide_in_ui(handle, templ.hide_in_ui, scene.handle);

        for(i32 i = 0; i < templ.tags.tag_count; i++)
        {
            set_entity_tag(templ.tags.tags[i], handle, scene.handle);
        }
        
        if(templ.comp_flags & COMP_TRANSFORM)
        {
            TransformComponent &transform = _get_transform_comp(handle, scene);
            transform.transform = rendering::create_transform(templ.transform.position, templ.transform.scale, templ.transform.rotation);
            // @Incomplete: Parent and child handles
        }
        
        if(templ.comp_flags & COMP_RENDER)
        {
            RenderComponent &render = _get_render_comp(handle, scene);
            render.ignore_depth = templ.render.ignore_depth;
            render.buffer_handle = templ.render.buffer_handle;
            render.bounding_box_enabled = false;
            
            render.casts_shadows = templ.render.casts_shadows;
            render.mesh_scale = templ.render.mesh_scale;
            render.bounding_box = templ.render.bounding_box;
            render.is_static = templ.render.is_static;

            for(i32 i = 0; i < templ.render.render_pass_count; i++)
            {
                rendering::RenderPassHandle render_pass_handle = rendering::get_render_pass_handle_for_name(templ.render.render_pass_names[i], scene.renderer);
                render.render_passes[i] = render_pass_handle;
                render.original_materials[i] = templ.render.material_handles[i];
                render.material_handles[i] = rendering::create_material_instance(scene.renderer, templ.render.material_handles[i]);
            }
            
			render.render_pass_count = templ.render.render_pass_count;

            if(scene.loaded)
            {
                setup_instance_buffers(render, scene);
            }
        }
        
        if(templ.comp_flags & COMP_PARTICLES)
        {
            scene::ParticleSystemComponent &ps_comp = scene::_add_particle_system_component(scene, handle, templ.particles.attributes, templ.particles.max_particles, templ.particles.material_handle);

            ParticleSystemInfo *ps = scene.renderer->particles.api->get_particle_system_info(ps_comp.handle, scene.renderer);
            
            for(i32 i = 0; i < templ.particles.size_over_lifetime.value_count; i++)
            {
                r32 key = templ.particles.size_over_lifetime.keys[i];
                math::Vec2 value = templ.particles.size_over_lifetime.values[i];
                scene.renderer->particles.api->add_size_key(ps->size_over_lifetime, key, value, &ps->arena);
            }

            for(i32 i = 0; i < templ.particles.color_over_lifetime.value_count; i++)
            {
                r32 key = templ.particles.color_over_lifetime.keys[i];
                math::Rgba value = templ.particles.color_over_lifetime.values[i];
                scene.renderer->particles.api->add_color_key(ps->color_over_lifetime, key, value, &ps->arena);
            }

            for(i32 i = 0; i < templ.particles.speed_over_lifetime.value_count; i++)
            {
                r32 key = templ.particles.speed_over_lifetime.keys[i];
                r32 value = templ.particles.speed_over_lifetime.values[i];
                scene.renderer->particles.api->add_speed_key(ps->speed_over_lifetime, key, value, &ps->arena);
            }

            if(templ.comp_flags & COMP_TRANSFORM)
            {
                ps->transform = rendering::create_transform(ps->attributes.base_position + templ.transform.position,
                                                            templ.transform.scale, templ.transform.rotation);
            }

            if(templ.particles.attributes.texture_handle.handle != 0)
            {
                rendering::set_uniform_value(scene.renderer, ps->material_handle, "tex0", templ.particles.attributes.texture_handle);
            }

            ps_comp.render_pass = rendering::get_render_pass_handle_for_name(STANDARD_PASS, scene.renderer);
        }

        if(templ.comp_flags & COMP_LIGHT)
        {
            LightComponent &light_comp = _add_light_component(scene, handle);

            switch(templ.light.type)
            {
            case LightType::DIRECTIONAL:
            {
                light_comp.type = LightType::DIRECTIONAL;
                DirectionalLight &light = light_comp.dir_light;

                light.direction = templ.light.directional.direction;
                light.ambient = templ.light.ambient;
                light.diffuse = templ.light.diffuse;
                light.specular = templ.light.specular;
            }
            break;
            case LightType::POINT:
            {
                light_comp.type = LightType::POINT;
                PointLight &light = light_comp.point_light;

                light.position = templ.light.point.position;
                light.constant = templ.light.point.constant;
                light.linear = templ.light.point.linear;
                light.quadratic = templ.light.point.quadratic;
                light.ambient = templ.light.ambient;
                light.diffuse = templ.light.diffuse;
                light.specular = templ.light.specular;
            }
            break;
            default:
            assert(false);
            break;
            }
        }

        return(handle);
    }
    
    static EntityHandle _register_entity_from_template_file(const char *path, Scene &scene, b32 savable = false, Tags* tags = nullptr)
    {
        EntityTemplateState *template_state = scene.template_state;
        
        EntityTemplate *templ = nullptr;
        i32 handle = -1;
        
        for(i32 i = 0; i < template_state->template_count; i++)
        {
            if(strcmp(template_state->templates[i].file_path, path) == 0)
            {
                handle = i;
                break;
            }
        }
        
        if(handle != -1) // The template was already loaded
        {
            templ = &template_state->templates[handle];
        }
        else
        {
            templ = &template_state->templates[template_state->template_count++];
            _load_template(templ, path, *template_state, scene);
        }
        
        assert(templ);

        EntityHandle entity = _register_entity_with_template(*templ, scene, savable);
            
        //// Add all children registered to the template
        for(i32 i = 0; i < templ->child_count; i++)
        {
			scene::EntityHandle child = _register_entity_with_template(template_state->templates[templ->child_handles[i].handle - 1], scene, false);
			scene::add_child(entity, child, scene.handle);
            scene::set_entity_selection_enabled(child, false, scene.handle);
        }

        if(tags)
        {
            // @Incomplete: Tags
            for(i32 i = 0; i < tags->tag_count; i++)
            {
                set_entity_tag(tags->tags[i], entity, scene.handle);
            }
        }

		if (templ->type_id != -1)
		{
			Entity& e = get_entity(entity, scene);

			if (auto * type_info = get_registered_type(templ->type_id, scene.handle.manager))
			{
				e.type_info = *type_info;
			}

			e.savable = true;
			e.type = templ->type_id;

			EntityData* entity_data = scene.handle.manager->callbacks.on_load_entity_of_type(entity, templ->type_id, scene.handle);

			if (entity_data)
			{
				entity_data->handle.handle = entity.handle;
				e.entity_data = entity_data;
			}
		}
        
        return(entity);
    }
			
    static EntityHandle register_entity_from_template_file(const char *path, SceneHandle scene_handle, b32 savable, Tags* tags)
    {
        Scene &scene = get_scene(scene_handle);
        return _register_entity_from_template_file(path, scene, savable, tags);
    }
    
    i32 _pack_transform_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_TRANSFORM)
        {
            for(i32 index = entity.transform_handle.handle; index < scene.transform_component_count - 1; index++)
            {
                scene.transform_components[index] = scene.transform_components[index + 1];
            }
            scene.transform_component_count--;
            return entity.transform_handle.handle;
        }
        return -1;
    }
    
    i32 _pack_render_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_RENDER)
        {
            for(i32 index = entity.render_handle.handle; index < scene.render_component_count - 1; index++)
            {
                scene.render_components[index] = scene.render_components[index + 1];
            }
            scene.render_component_count--;
            return entity.render_handle.handle;
        }
        return -1;
    }

    i32 _pack_light_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_LIGHT)
        {
            for(i32 index = entity.light_handle.handle; index < scene.light_component_count - 1; index++)
            {
                scene.light_components[index] = scene.light_components[index + 1];
            }
            scene.light_component_count--;
            return entity.light_handle.handle;
        }
        return -1;
    }

    i32 _pack_particle_system_components(Entity &entity, Scene &scene)
    {
        if(entity.comp_flags & COMP_PARTICLES)
        {
            ParticleSystemHandle ps_handle = scene.particle_system_components[entity.particle_system_handle.handle].handle;
            scene.renderer->particles.api->remove_particle_system(scene.renderer, ps_handle);
            for(i32 index = entity.particle_system_handle.handle; index < scene.particle_system_component_count - 1; index++)
            {
                scene.particle_system_components[index] = scene.particle_system_components[index + 1];
            }
            scene.particle_system_component_count--;
            return entity.particle_system_handle.handle;
        }
        return -1;
    }
    
    static void _unregister_entity(EntityHandle handle, Scene &scene)
    {
        i32 removed_handle = handle.handle;
        
        // Invalidate handle
        handle.handle = 0;
        
        // If only one entity was present, we should just zero everything...
        if(scene.entities.count == 1)
        {
            list::clear(&scene.entities);
            scene.transform_component_count = 0;
            scene.render_component_count = 0;
            scene.particle_system_component_count = 0;
        }
        else
        {
            // Get the handle into the real entity array
            Entity &entity = list::get(&scene.entities, removed_handle);
            
            // Pack the components in scene by removing the unregistered entity's components and moving the rest to pack the arrays. If the returned handles are -1 the entity didn't have that component set.
            i32 transform_handle = _pack_transform_components(entity, scene);
            i32 render_handle = _pack_render_components(entity, scene);
            i32 particle_system_handle = _pack_particle_system_components(entity, scene);
            i32 light_component_handle = _pack_light_components(entity, scene);
                        
            // Run through all existing entities and pack the array to remove the unregistered one. All component handles affected by the removal are decremented.

            i32 real_handle = list::get_internal_handle(&scene.entities, removed_handle);
            for(i32 index = real_handle; index < scene.entities.count - 1; index++)
            {
                scene.entities[index] = scene.entities[index + 1];

                if(transform_handle != -1 && scene.entities[index].transform_handle.handle > transform_handle)
                {
                    scene.entities[index].transform_handle.handle--;
                }
                
                if(render_handle != -1 && scene.entities[index].render_handle.handle > render_handle)
                {
                    scene.entities[index].render_handle.handle--;
                }
                
                if(particle_system_handle != -1 && scene.entities[index].particle_system_handle.handle > particle_system_handle)
                {
                    scene.entities[index].particle_system_handle.handle--;
                }

                if(light_component_handle != -1 && scene.entities[index].light_handle.handle > light_component_handle)
                {
                    scene.entities[index].light_handle.handle--;
                }
                
                scene.active_entities[index] = scene.active_entities[index + 1];
            }

            // list::remove(&scene.entities, removed_handle);
            scene.entities.count--;

            // Remember to reset the internal handle to -1 to indicate an unused index
            scene.entities._internal_handles[removed_handle - 1] = -1;
            
            //@Note: We also have to update the internal handles if they were placed after the unregistered entity
            for(i32 internal_index = 0; internal_index < scene.max_entity_count; internal_index++)
            {
                i32 current_handle = scene.entities._internal_handles[internal_index];
                if(current_handle > real_handle)
                {
                    scene.entities._internal_handles[internal_index] = current_handle - 1;
                }
            }
        }
    }
    
    static void unregister_entity(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);

        if(!list::has_value(&scene.entities, handle.handle))
            return;

        Entity entity = get_entity(handle, scene_handle);
            
        _unregister_entity(handle, scene);

        for(i32 i = 0; i < entity.child_count; i++)
        {
            unregister_entity(entity.children[i], scene_handle);
        }
        
        entity.child_count = 0;
    }

    static void unregister_entity(EntityHandle handle)
    {
        unregister_entity(handle, handle.scene_handle);
    }

    static EntityHandle place_entity_from_template(math::Vec3 position, const char* path, SceneHandle scene, b32 savable = true, b32 select = false)
    {
        EntityHandle entity = register_entity_from_template_file(path, scene, savable);
        TransformComponent &transform = get_transform_comp(entity, scene);
        rendering::set_position(transform.transform, position);
        if(select)
        {
            select_entity(entity, scene.manager);
        }
        
        return entity;
    }

    static b32 is_active(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        if(handle.handle > 0)
        {
            i32 internal_handle = list::get_internal_handle(&scene.entities, handle.handle);
            if(internal_handle > -1)
            {
                return scene.active_entities[internal_handle];
            }
        }
        return false;
    }
    
    static void _set_active(EntityHandle handle, b32 active, Scene &scene, b32 recursive)
    {
        if(handle.handle > 0)
        {
            i32 internal_handle = list::get_internal_handle(&scene.entities, handle.handle);
            if(internal_handle > -1)
            {
                scene.active_entities[internal_handle] = active;
            }

            if(recursive)
            {
                i32 child_count = get_child_count(handle, scene.handle); // @Speed: This is yet another lookup

                for(i32 i = 0; i < child_count; i++)
                {
                    _set_active(get_child_handle(handle, i, scene.handle), active, scene, recursive);
                }
            }
        }
    }

    static void set_active(EntityHandle handle, b32 active, SceneHandle scene_handle, b32 recursive)
    {
        Scene &scene = get_scene(scene_handle);
        _set_active(handle, active, scene, recursive);
    }

    static void set_hide_in_ui(EntityHandle handle, b32 hide, SceneHandle scene_handle)
    {
        Entity& entity = get_entity(handle, scene_handle);
        entity.hide_in_ui = hide;
    }

    static void set_entity_selection_enabled(EntityHandle entity_handle, b32 enabled, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        Entity &entity = list::get(&scene.entities, entity_handle.handle);
        entity.selection_enabled = enabled;
    }

    static EntityHandle find_entity_by_name(const char* name, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        for(i32 i = 0; i < scene.entities.count; i++)
        {
            Entity& e = scene.entities[i];
            if(strcmp(e.name, name) == 0)
            {
                return e.handle;
            }
        }
        return EMPTY_ENTITY_HANDLE;
    }

    static void set_entity_tag(const char *tag, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);

        if(has_tag(tag, entity_handle, scene_handle))
            return;
        
        Entity &entity = list::get(&scene.entities, entity_handle.handle);
        if(entity.tags.tag_count >= MAX_ENTITY_TAGS)
        {
            debug_log("Max count for entity tags reached (%d/%d).", MAX_ENTITY_TAGS, MAX_ENTITY_TAGS);
            assert(false);
            return;
        }

        strncpy(entity.tags.tags[entity.tags.tag_count++], tag, strlen(tag) + 1);
    }

    static const EntityList find_entities_with_tag(const char* tag, SceneHandle scene_handle)
    {
        assert(strlen(tag) > 0);
        Scene &scene = get_scene(scene_handle);

        EntityList list = {};

        for(i32 i = 0; i < scene.entities.count; i++)
        {
            const Entity& entity = scene.entities[i];
            if(has_tag(tag, entity.handle, scene_handle))
            {
                assert(list.entity_count < ENTITY_LIST_SIZE);
                list.handles[list.entity_count++] = entity.handle;
            }
        }
        return list;
    }

    static const Tags& get_entity_tags(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        if(!IS_ENTITY_HANDLE_VALID(entity_handle))
        {
            debug_log("Entity handle not valid: %d\n", entity_handle.handle);
            assert(false);
        }
        return get_entity(entity_handle, scene_handle).tags;
    }

    static b32 has_tag(const char* tag_name, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        const Entity &entity = get_entity(entity_handle, scene_handle);
        for(i32 i = 0; i < entity.tags.tag_count; i++)
        {
            if(strcmp(entity.tags.tags[i], tag_name) == 0)
            {
                return true;
            }
        }
        return false;        
    }
    
    static void set_entity_name(const char *name, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        Entity &entity = list::get(&scene.entities, entity_handle.handle);
        strcpy(entity.name, name);
    }

    static const char * get_entity_name(EntityHandle entity_handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        const Entity &entity = list::get(&scene.entities, entity_handle.handle);
        return entity.name;
    }

    static TransformComponent & get_transform_comp(TransformComponentHandle handle, Scene &scene)
    {
        return scene.transform_components[handle.handle];
    }
    
    static TransformComponent & get_transform_comp(TransformComponentHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return get_transform_comp(handle, scene);
    }
    
    // Returns a direct pointer to the TransformComponent of the specified entity
    static TransformComponent& _get_transform_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);

        Entity &entity = list::get(&scene.entities, handle.handle);
        assert(entity.comp_flags & COMP_TRANSFORM);

        TransformComponent& comp = scene.transform_components[entity.transform_handle.handle];
        return(comp);
    }

    static TransformComponent& get_transform_comp(EntityHandle entity, Scene &scene)
    {
        return _get_transform_comp(entity, scene);
    }
     
    static TransformComponent& get_transform_comp(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return get_transform_comp(entity, scene);
    }

    static AnimatorComponent& get_animator_comp(EntityHandle entity_handle, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);

        Entity &entity = list::get(&scene.entities, entity_handle.handle);        
        assert(entity.comp_flags & COMP_ANIMATOR);
        
        AnimatorComponent& comp = *scene.animator_components[entity.animator_handle.handle];
        return(comp);
    }
     
    // @Note(Daniel): Should we really return a pointer here? A reference might suffice, since we don't ever use the null-value for anything....
    // Returns a direct pointer to the RenderComponent of the specified entity
    static RenderComponent& _get_render_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        Entity &entity = list::get(&scene.entities, handle.handle);        
        assert(entity.comp_flags & COMP_RENDER);
        
        RenderComponent& comp = scene.render_components[entity.render_handle.handle];
        return(comp);
    }

    static RenderComponent& get_render_comp(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return _get_render_comp(entity, scene);
    }
    
    static CameraComponent& _get_camera_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        Entity &entity = list::get(&scene.entities, handle.handle);        
        assert(entity.comp_flags & COMP_CAMERA);
        
        CameraComponent& comp = scene.camera_components[entity.camera_handle.handle];
        return(comp);
    }

    static CameraComponent& get_camera_comp(EntityHandle entity, SceneHandle handle)
    {
        Scene &scene = get_scene(handle);
        return _get_camera_comp(entity, scene);
    }
    
    static ParticleSystemComponent& _get_particle_system_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);

        Entity &entity = list::get(&scene.entities, handle.handle);        
        assert(entity.comp_flags & COMP_PARTICLES);
        
        ParticleSystemComponent& comp = scene.particle_system_components[entity.particle_system_handle.handle];
        return(comp);
    }

    static ParticleSystemComponent& get_particle_system_comp(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _get_particle_system_comp(handle, scene);
    }
    
    static LightComponent &_get_light_comp(EntityHandle handle, Scene &scene)
    {
        assert(handle.handle != 0);
        Entity &entity = list::get(&scene.entities, handle.handle);        
        assert(entity.comp_flags & COMP_LIGHT);
        
        LightComponent& comp = scene.light_components[entity.light_handle.handle];
        return(comp);
    }

    static LightComponent &get_light_comp(EntityHandle handle, SceneHandle scene_handle)
    {
        Scene &scene = get_scene(scene_handle);
        return _get_light_comp(handle, scene);
    }

    static EntityHandle get_main_camera_handle(SceneHandle handle)
    {
        Scene& scene = get_scene(handle);
        return scene.main_camera_handle;
    }

    static TransformComponent &get_main_camera_transform(SceneHandle handle)
    {
        Scene& scene = get_scene(handle);
        return _get_transform_comp(scene.main_camera_handle, scene);
    }
    
    static CameraComponent &get_main_camera_comp(SceneHandle handle)
    {
        Scene& scene = get_scene(handle);
        return _get_camera_comp(scene.main_camera_handle, scene);
    }
    
    static Camera& get_scene_camera(SceneHandle handle)
    {
        Scene& scene = get_scene(handle);
        CameraComponent &camera_comp = get_camera_comp(scene.main_camera_handle, handle);
        return camera_comp.camera;
    }

    static Camera& get_current_camera(SceneHandle handle)
    {
        if(handle.manager->mode == SceneMode::RUNNING)
        {
            return get_scene_camera(handle);
        }
        else
        {
            if(handle.manager->camera_preview.show_camera_preview)
            {
                CameraComponent &component = get_camera_comp(handle.manager->camera_preview.handle, handle);
                return component.camera;
            }
            else
            {
                CameraComponent &component = get_camera_comp(handle.manager->editor_camera, handle);
                return component.camera;
            }
        }
    }

    static TransformComponent &get_current_camera_transform(SceneHandle handle)
    {
        if(handle.manager->mode == SceneMode::RUNNING)
        {
            return get_main_camera_transform(handle);
        }
        else
        {
            if(handle.manager->camera_preview.show_camera_preview)
            {
                return get_transform_comp(handle.manager->camera_preview.handle, handle);
            }
            else
                return get_transform_comp(handle.manager->editor_camera, handle);
        }
    }
    
    static CameraComponent &get_current_camera_comp(SceneHandle handle)
    {
        if(handle.manager->mode == SceneMode::RUNNING)
        {
            return get_main_camera_comp(handle);
        }
        else
        {
            if(handle.manager->camera_preview.show_camera_preview)
            {
                return get_camera_comp(handle.manager->camera_preview.handle, handle);
            }
            else
                return get_camera_comp(handle.manager->editor_camera, handle);
        }
    }
    
#define SET_MAT_ARRAY_VALUE(type) static void set_uniform_array_value(scene::EntityHandle handle, const char *array_name, i32 index, const char *variable_name, type value, scene::SceneHandle &scene) \
    {                                                                   \
    scene::RenderComponent &render = get_render_comp(handle, scene);    \
        for(i32 i = 0; i < render.render_pass_count; i++) \
        {\
            rendering::set_uniform_array_value(scene.manager->renderer, render.material_handles[i], array_name, index, variable_name, value); \
        } \
    }                                                                   \

    SET_MAT_ARRAY_VALUE(r32)
    SET_MAT_ARRAY_VALUE(math::Vec2)
    SET_MAT_ARRAY_VALUE(math::Vec3)
    SET_MAT_ARRAY_VALUE(math::Vec4)
    SET_MAT_ARRAY_VALUE(i32)
    SET_MAT_ARRAY_VALUE(math::Mat4)
    SET_MAT_ARRAY_VALUE(rendering::TextureHandle)


    static void recompute_transforms(TransformComponent& root, Scene& scene)
    {
        rendering::recompute_transform(root.transform);
        Entity &entity = get_entity(root.entity, scene);
        
        if(IS_ENTITY_HANDLE_VALID(entity.parent))
        {
            scene::TransformComponent &parent_trans = get_transform_comp(entity.parent, scene);
            math::Mat4 parent_model = parent_trans.transform.model;
            root.transform.model = parent_model * root.transform.model;
        }

        for(i32 i = 0; i < entity.child_count; i++)
        {
            TransformComponent& child = get_transform_comp(entity.children[i], scene);
            recompute_transforms(child, scene);
        }
    }

    static i32 get_child_count(EntityHandle handle, SceneHandle scene)
    {
        Entity& entity = get_entity(handle, scene);
        return entity.child_count;
    }

    static EntityHandle get_child_handle(EntityHandle handle, i32 index, SceneHandle scene)
    {
        Entity& entity = get_entity(handle, scene);
        return entity.children[index];
    }

    static void add_child(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        if(parent_handle.handle == 0)
            return;
        
        assert(parent_handle.handle != child_handle.handle);

        Entity& parent = get_entity(parent_handle, scene);
        Entity& child = get_entity(child_handle, scene);
        
        TransformComponent &parent_transform = get_transform_comp(parent_handle, scene);
        TransformComponent &child_transform = get_transform_comp(child_handle, scene);

        assert(parent.child_count + 1 < MAX_CHILDREN);
        assert(parent_handle.handle != child_handle.handle);
        
        parent.children[parent.child_count++] = child_handle;
        child.parent = parent_handle;
        
        child_transform.transform.position -= parent_transform.transform.position;
        child_transform.transform.position /= parent_transform.transform.scale;
        child_transform.transform.euler_angles -= parent_transform.transform.euler_angles;
        child_transform.transform.scale /= parent_transform.transform.scale;
        child_transform.transform.dirty = true;
    }

    static void remove_child(EntityHandle parent_handle, EntityHandle child_handle, SceneHandle& scene)
    {
        Entity& parent = get_entity(parent_handle, scene);
        Entity& child = get_entity(child_handle, scene);
        TransformComponent &parent_transform = get_transform_comp(parent_handle, scene);
        TransformComponent &child_transform = get_transform_comp(child_handle, scene);
        
        for(i32 i = 0; i < parent.child_count; i++)
        {
            if(parent.children[i].handle == child_handle.handle)
            {
                child.parent = EMPTY_ENTITY_HANDLE;

                child_transform.transform.position += parent_transform.transform.position;
                child_transform.transform.position *= parent_transform.transform.scale;
                child_transform.transform.euler_angles += parent_transform.transform.euler_angles;
                child_transform.transform.scale *= parent_transform.transform.scale;
                child_transform.transform.dirty = true;
                
                parent.children[i] = parent.children[parent.child_count - 1];
                parent.child_count--;
                break;
            }
        }
    }

    static b32 has_parent(EntityHandle child_handle, SceneHandle scene)
    {
        Entity &child = get_entity(child_handle, scene);
        return IS_ENTITY_HANDLE_VALID(child.parent);
    }
    
    static void add_parent(EntityHandle child_handle, EntityHandle parent_handle, SceneHandle scene)
    {
        add_child(parent_handle, child_handle, scene);
    }

    static void remove_parent(EntityHandle child_handle, SceneHandle scene)
    {
        Entity& child = get_entity(child_handle, scene);
        if(IS_ENTITY_HANDLE_VALID(child.parent))
        {
            remove_child(child.parent, child_handle, scene);
        }
    }

    static AnimatorParameterHandle add_animator_parameter(const AnimatorParameter &param, EntityHandle entity, SceneHandle scene_handle)
    {
        AnimatorComponent &animator = get_animator_comp(entity, scene_handle);

        animator.params[animator.param_count++] = param;
        
        return { animator.param_count  };
    }

    static AnimationTransitionHandle add_animation_transition(scene::EntityHandle entity_handle, RootAnimationHandle from_handle, RootAnimationHandle to_handle, SceneHandle scene_handle)
    {
        AnimationTransitionHandle handle = {-1};

        AnimatorComponent &animator = get_animator_comp(entity_handle, scene_handle);
        Animation &animation = animator.animations[from_handle.handle - 1];

        assert(animation.transition_count < MAX_ANIMATION_TRANSITIONS);

        auto& trans = animation.transitions[animation.transition_count++];
        trans = {};
        trans.from_handle = from_handle;
        trans.to_handle = to_handle;
        handle.handle = animation.transition_count;
        
        return handle;
    }

    static AnimationTransitionConditionHandle add_transition_condition(scene::EntityHandle entity_handle, const AnimationTransitionCondition &condition, RootAnimationHandle animation_handle, AnimationTransitionHandle transition_handle, SceneHandle scene_handle)
    {
        AnimationTransitionConditionHandle handle = {-1};
        
        AnimatorComponent &animator = get_animator_comp(entity_handle, scene_handle);
        Animation &animation = animator.animations[animation_handle.handle - 1];
        AnimationTransition &transition = animation.transitions[transition_handle.handle - 1];

        assert(transition.condition_count < NAX_TRANSITION_CONDITIONS);

        transition.conditions[transition.condition_count++] = condition;
        handle.handle = transition.condition_count;
        
        return handle;
    }
    
    static void _set_animation_playing(b32 playing, RootAnimationHandle animation_handle, SceneHandle scene_handle)
    {
        AnimatorComponent &animator = get_animator_comp(animation_handle.entity, scene_handle);
        Animation &animation = animator.animations[animation_handle.handle - 1];
        
        animator.running = playing;
        animator.current_handle = animation_handle;

        for(i32 i = 0; i < animation.float_anim_count; i++)
        {
            animation.float_animations[i].running = playing;
            animation.float_animations[i].current_key_frame = 0;
            animation.float_animations[i].current_time = 0.0;
        }

        for(i32 i = 0; i < animation.vec3_anim_count; i++)
        {
            animation.vec3_animations[i].running = playing;
            animation.vec3_animations[i].current_key_frame = 0;
            animation.vec3_animations[i].current_time = 0.0;
        }
    }

    static void play_animation(RootAnimationHandle animation_handle,  SceneHandle scene_handle)
    {
        _set_animation_playing(true, animation_handle, scene_handle);
    }

    static void stop_animation(RootAnimationHandle animation_handle, EntityHandle entity_handle, SceneHandle scene_handle)
    {
        _set_animation_playing(false, animation_handle, scene_handle);
    }

    static AnimationHandle add_uniform_value_animation(AnimatorComponent &animator, RootAnimationHandle root_handle, EntityHandle entity, AnimationType type, const char *value_name)
    {
        Animation &root = animator.animations[root_handle.handle - 1];
        
        AnimationHandle handle = {0};
        handle.type = type;

        if(type == AnimationType::FLOAT)
        {
            assert(root.float_anim_count < MAX_ANIMATIONS);

            auto& anim = root.float_animations[root.float_anim_count++];
            anim.entity = entity;

            strcpy(anim.value_name, value_name);

            handle.handle = root.float_anim_count;
        }
        else if(type == AnimationType::VEC3)
        {
            assert(root.vec3_anim_count < MAX_ANIMATIONS);

            auto& anim = root.vec3_animations[root.vec3_anim_count++];
            anim.entity = entity;

            strcpy(anim.value_name, value_name);

            handle.handle = root.vec3_anim_count;
        }

        return handle;
    }

    static AnimationHandle add_uniform_value_animation(RootAnimationHandle root_handle, EntityHandle entity, AnimationType type, const char *value_name, SceneHandle scene)
    {
        AnimatorComponent &comp = get_animator_comp(root_handle.entity, scene);
        return add_uniform_value_animation(comp, root_handle, entity, type, value_name);
    }

    static AnimationHandle add_transform_animation(AnimatorComponent &animator, RootAnimationHandle root_handle, EntityHandle entity, Vec3Type vec3_type)
    {
        Animation &animation = animator.animations[root_handle.handle - 1];

        AnimationHandle handle = {0};
        handle.type = AnimationType::VEC3;
        
        assert(animation.vec3_anim_count < MAX_ANIMATIONS);

        auto& anim = animation.vec3_animations[animation.vec3_anim_count++];
        anim.entity = entity;
        anim.type = vec3_type;

        handle.handle = animation.vec3_anim_count;

        return handle;
    }

    static AnimationHandle add_transform_animation(RootAnimationHandle root_handle, EntityHandle entity, Vec3Type vec3_type, SceneHandle scene)
    {
        AnimatorComponent &comp = get_animator_comp(root_handle.entity, scene);
        return add_transform_animation(comp, root_handle, entity, vec3_type);
    }

    static RootAnimationHandle add_root_animation(EntityHandle entity_handle, AnimatorComponent &animator, b32 loop = false)
    {
        RootAnimationHandle handle = { 0, 0 };

        assert(animator.anim_count < MAX_ANIMATIONS);

        auto& anim = animator.animations[animator.anim_count++];
        anim.loop = loop;

        handle.handle = animator.anim_count;
        handle.entity = entity_handle;

        return handle;
    }

    static RootAnimationHandle add_root_animation(EntityHandle entity, SceneHandle scene, b32 loop = false)
    {
        AnimatorComponent &comp = get_animator_comp(entity, scene);
        return add_root_animation(entity, comp, loop);
    }

    static void set_animation_callback(RootAnimationHandle root, AnimationCallback callback, SceneHandle scene)
    {
        AnimatorComponent &comp = get_animator_comp(root.entity, scene);
        Animation &animation = comp.animations[root.handle - 1];
        animation.callback = callback;
    }

    static void add_animation_float_key_frame(r32 value, r32 time, RootAnimationHandle root_handle, AnimationHandle handle, SceneHandle scene_handle, AnimationEasingMode mode = AnimationEasingMode::LERP)
    {
        AnimatorComponent &comp = get_animator_comp(root_handle.entity, scene_handle);
        Animation &animation = comp.animations[root_handle.handle - 1];

        FloatAnimation &float_anim = animation.float_animations[handle.handle - 1];
        float_anim.easing_mode = mode;
        float_anim.key_frame_values[float_anim.count] = value;
        float_anim.key_frame_times[float_anim.count] = time;
        float_anim.count++;
    }

    static void add_animation_vec3_key_frame(math::Vec3 value, r32 time, RootAnimationHandle root_handle, AnimationHandle handle, SceneHandle scene_handle, AnimationEasingMode mode = AnimationEasingMode::LERP)
    {
        AnimatorComponent &comp = get_animator_comp(root_handle.entity, scene_handle);
        Animation &animation = comp.animations[root_handle.handle - 1];

        Vec3Animation &vec3_anim = animation.vec3_animations[handle.handle - 1];
        vec3_anim.easing_mode = mode;
        vec3_anim.key_frame_values[vec3_anim.count] = value;
        vec3_anim.key_frame_times[vec3_anim.count] = time;
        vec3_anim.count++;
    }

    static b32 update_animation(FloatAnimation &animation, Animation &root, AnimatorComponent &animator, Scene &scene, r64 delta_time)
    {
        b32 should_run = true;

        r64 frame_time = animation.key_frame_times[animation.current_key_frame];
        r32 key_frame_value = animation.key_frame_values[animation.current_key_frame];

        r32 t = 0.0;

        r32 next_value = animation.key_frame_values[animation.current_key_frame + 1];
        r64 next_time = animation.key_frame_times[animation.current_key_frame + 1];
            
        r64 time_distance = next_time - frame_time;

        t = math::clamp((r32)(animation.current_time / time_distance), 0.0f, 1.0f);
        r32 value = 0.0f;

        switch(animation.easing_mode)
        {
        case AnimationEasingMode::LERP:
        value = math::linear_tween(key_frame_value, t, next_value);
        break;
        case AnimationEasingMode::EASE_IN:
        value = math::ease_in_quad(key_frame_value, t, next_value);
        break;
        case AnimationEasingMode::EASE_OUT:
        value = math::ease_out_quad(key_frame_value, t, next_value);
        break;
        default:
        assert(false);
        break;
        }

        set_uniform_value(animation.entity, animation.value_name, value, scene.handle);

        if(t >= 1.0f) // The frame is done
        {
            animation.current_key_frame++;
            animation.current_time = 0.0;

            if((animation.current_key_frame == animation.count - 1))
            {
                animation.current_key_frame = 0;
                should_run = root.loop;
                animation.running = root.loop;
            }
        }

        if(animation.running)
            animation.current_time += delta_time;

        return should_run;
    }

    static b32 update_animation(Vec3Animation &animation, Animation &root, AnimatorComponent &animator, Scene &scene, r64 delta_time)
    {
        b32 should_run = true;

        r64 frame_time = animation.key_frame_times[animation.current_key_frame];
        math::Vec3 key_frame_value = animation.key_frame_values[animation.current_key_frame];

        r32 t = 0.0;

        math::Vec3 next_value = animation.key_frame_values[animation.current_key_frame + 1];
        r64 next_time = animation.key_frame_times[animation.current_key_frame + 1];
            
        r64 time_distance = next_time - frame_time;

        t = math::clamp((r32)(animation.current_time / time_distance), 0.0f, 1.0f);
        math::Vec3 value = math::Vec3(0, 0, 0);

        switch(animation.easing_mode)
        {
        case AnimationEasingMode::LERP:
        value = math::linear_tween(key_frame_value, t, next_value);
        break;
        case AnimationEasingMode::EASE_IN:
        value = math::ease_in_quad(key_frame_value, t, next_value);
        break;
        case AnimationEasingMode::EASE_OUT:
        value = math::ease_out_quad(key_frame_value, t, next_value);
        break;
        default:
        assert(false);
        break;
        }

        switch(animation.type)
        {
        case Vec3Type::UNIFORM:
        {}
        break;
        case Vec3Type::TRANSFORM_POSITION:
        {
            TransformComponent &comp = get_transform_comp(animation.entity, scene);
            scene::set_position(comp, value);
        }
        break;
        case Vec3Type::TRANSFORM_ROTATION:
        {
            TransformComponent &comp = get_transform_comp(animation.entity, scene);
            scene::set_rotation(comp, value);
        }
        break;
        case Vec3Type::TRANSFORM_SCALE:
        {
            TransformComponent &comp = get_transform_comp(animation.entity, scene);
            scene::set_scale(comp, value);
        }
        break;
        }

        if(t >= 1.0f) // The frame is done
        {
            animation.current_key_frame++;
            animation.current_time = 0.0;

            if(animation.current_key_frame == animation.count - 1)
            {
                animation.current_key_frame = 0;
                should_run = root.loop;
                animation.running = root.loop;
            }
        }

        if(animation.running)
            animation.current_time += delta_time;

        return should_run;
    }

    static void update_root_animation(AnimatorComponent &animator, Animation &animation, i32 *running_count, Scene &scene, r64 delta_time)
    {
        for(i32 j = 0; j < animation.float_anim_count; j++)
        {
            if(animation.float_animations[j].running)
                *running_count += update_animation(animation.float_animations[j], animation, animator, scene, delta_time);
        }

        for(i32 j = 0; j < animation.vec3_anim_count; j++)
        {
            if(animation.vec3_animations[j].running)
                *running_count += update_animation(animation.vec3_animations[j], animation, animator, scene, delta_time);
        }
    }

    static b32 transition_conditions_true(AnimatorComponent &animator, AnimationTransition &transition)
    {
        if(transition.condition_count == 0)
        {
            return true;
        }

        //b32 transition_true = true;

        for(i32 i = 0; i < transition.condition_count; i++)
        {

        }

        return false;
    }
    
    static void update_animators(scene::Scene &scene, Renderer *renderer, r64 delta_time)
    {
        for(i32 i = 0; i < scene.animator_component_count; i++)
        {
            AnimatorComponent &animator = *scene.animator_components[i];
            
            if(animator.running)
            {
                i32 running_count = 0;

                Animation &animation = animator.animations[animator.current_handle.handle - 1];

                update_root_animation(animator, animation, &running_count, scene, delta_time);
                
                animator.running = running_count > 0;

                // @Incomplete: Change this to "current animation running" instead
                if(!animator.running)
                {
                    if(animation.callback)
                        animation.callback();
                
                    for(i32 j = 0; j < animation.transition_count; j++)
                    {
                        if(transition_conditions_true(animator, animation.transitions[j]))
                        {
                            _set_animation_playing(true, animation.transitions[j].to_handle, scene.handle);
                            break;
                        }
                    }
                }
            }
        }
    }

    static void push_scene_for_rendering(scene::Scene &scene, Renderer *renderer)
    {
        update_cameras(scene, scene.scene_manager);
        
        Camera &scene_camera = get_current_camera(scene.scene_manager->loaded_scene);

        for(i32 i = 0; i < renderer->render.pass_count; i++)
        {
            rendering::RenderPass &pass = renderer->render.passes[i];
            if(scene.render_pass_cameras[i].handle > 0)
            {
                CameraComponent &camera_comp = _get_camera_comp(scene.render_pass_cameras[i], scene);
                pass.camera = camera_comp.camera;
                
            }
            else
            {
                pass.camera = scene_camera;
            }
        }
    
        renderer->render.dir_light_count = 0;
        renderer->render.point_light_count = 0;

		Pass *passes = renderer->render.pass_commands;
		i32 pass_count = 0;

        i32 particles_to_push[64];
        i32 particles_count = 0;

        for(i32 ent_index = 0; ent_index < scene.entities.count; ent_index++)
        {
            const scene::Entity *ent = &scene.entities[ent_index];
            
            if(scene.active_entities[ent_index])
            {
                TransformComponent* start_component = &scene.transform_components[ent->transform_handle.handle];
                
                if(start_component->transform.dirty)
                {
                    while(IS_ENTITY_HANDLE_VALID(ent->parent))
                    {
                        Entity *parent = &get_entity(ent->parent, scene);
                        TransformComponent& parent_transform = get_transform_comp(ent->parent, scene);
                        
                        if(parent_transform.transform.dirty)
                        {
                            ent = parent;
                            start_component = &parent_transform;
                        }
                        else
                        {
                            break;
                        }
                    }

                    assert(start_component);
                    recompute_transforms(*start_component, scene);
                }
            }
        }
    
        for(i32 ent_index = 0; ent_index < scene.entities.count; ent_index++)
        {
            const scene::Entity &ent = scene.entities[ent_index];
        
            if (scene.active_entities[ent_index])
            {
                scene::TransformComponent &transform = scene.transform_components[ent.transform_handle.handle];
                
                // Create a copy of the position, rotation and scale since we don't want the parents transform to change the child's transform. Only when rendering.
                math::Vec3 position = transform.transform.position;
            
                if(ent.comp_flags & scene::COMP_LIGHT)
                {
                    scene::LightComponent &light_comp = scene.light_components[ent.light_handle.handle];
                
                    switch(light_comp.type)
                    {
                    case scene::LightType::DIRECTIONAL:
                    {
                        Camera &camera = get_scene_camera(scene.scene_manager->loaded_scene);
                        renderer->render.directional_lights[renderer->render.dir_light_count++] = light_comp.dir_light;
                        rendering::calculate_light_space_matrices(renderer, camera, light_comp.dir_light.direction);
                    }
                    
                    break;
                    case scene::LightType::POINT:
                    light_comp.point_light.position = position;
                    renderer->render.point_lights[renderer->render.point_light_count++] = light_comp.point_light;
                    break;
                    default:
                    assert(false);
                    }
                }

                if (ent.comp_flags & scene::COMP_RENDER)
                {
                    scene::RenderComponent &render = scene.render_components[ent.render_handle.handle];

                    for(i32 pass_index = 0; pass_index < render.render_pass_count; pass_index++)
                    {
						Pass *pass_command = nullptr;

						for (i32 i = 0; i < pass_count; i++)
						{
							if (passes[i].pass_handle.handle == render.render_passes[pass_index].handle)
							{
								pass_command = &passes[i];
							}
						}

						if (!pass_command)
						{
							pass_command = &passes[pass_count++];
							pass_command->pass_handle = render.render_passes[pass_index];
							pass_command->command_count = 0;
						}

						QueuedRenderCommand *command = nullptr;
                        rendering::Material &mat_instance = get_material_instance(render.material_handles[pass_index], renderer);

                        if(!render.is_static || !render.is_pushed[pass_index])
                        {
                            if(render.is_static)
                            {
                                render.is_pushed[pass_index] = true;
                            }
                            
                            // We can make one call instead
                            for (i32 i = 0; i < mat_instance.instanced_vertex_attribute_count; i++)
                            {
                                rendering::VertexAttributeInstanced &va = mat_instance.instanced_vertex_attributes[i];
                                rendering::VertexAttribute &attr = va.attribute;

                                switch (attr.type)
                                {
                                case rendering::ValueType::FLOAT:
								rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float_val, renderer);
								break;
                                case rendering::ValueType::FLOAT2:
								rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float2_val, renderer);
								break;
                                case rendering::ValueType::FLOAT3:
                                {
                                    math::Vec3 val = attr.float3_val;

                                    if (va.mapping_type != rendering::VertexAttributeMappingType::NONE)
                                    {
                                        if (va.mapping_type == rendering::VertexAttributeMappingType::POSITION)
                                        {
                                            assert(false);
                                            // val = render_command.transform.position;
                                        }
                                        else if (va.mapping_type == rendering::VertexAttributeMappingType::ROTATION)
                                        {
                                            assert(false);
                                            // val = render_command.transform.rotation;
                                        }
                                        else if (va.mapping_type == rendering::VertexAttributeMappingType::SCALE)
                                        {
                                            assert(false);
                                            // val = render_command.transform.scale;
                                        }

                                    }
                                    rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                                }
                                break;
                                case rendering::ValueType::FLOAT4:
								rendering::add_instance_buffer_value(va.instance_buffer_handle, attr.float4_val, renderer);
								break;
                                case rendering::ValueType::MAT4:
                                {
                                    math::Mat4 val = attr.mat4_val;

                                    if (va.mapping_type == rendering::VertexAttributeMappingType::MODEL)
                                    {
                                        rendering::Transform &t = transform.transform;
                                        val = math::transpose(t.model);
                                    }
                                    rendering::add_instance_buffer_value(va.instance_buffer_handle, val, renderer);
                                }
                                break;
                                default:
								assert(false);
                                }
                            }
                        }
                        
					    for(i32 i = 0; i < pass_command->command_count; i++)
                        {
                            QueuedRenderCommand &cmd = pass_command->queued_commands[i];
                            if(cmd.buffer_handle.handle == render.buffer_handle.handle
                               && cmd.original_material.handle == mat_instance.source_material.handle && cmd.ignore_depth == render.ignore_depth)
                            {
                                // It's a doozy
                                command = &cmd;
                                break;
                            }
                        }

                        if(!command)
                        {
                            command = &pass_command->queued_commands[pass_command->command_count++];
                            command->ignore_depth = render.ignore_depth;
                            command->buffer_handle = render.buffer_handle;
                            command->original_material = mat_instance.source_material;
                            command->count = 0;
                        }

                        if(render.wireframe_enabled)
                        {
                            rendering::push_buffer_to_render_pass(renderer, render.buffer_handle, renderer->render.wireframe_material, transform.transform, renderer->render.standard_opaque_pass, rendering::CommandType::NO_DEPTH);
                        }

                        if(render.bounding_box_enabled)
                        {
                            math::BoundingBox box = render.bounding_box;
                            math::Vec3 size = math::Vec3(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);
                            math::Vec3 box_position = math::Vec3((box.min.x + box.max.x) / 2.0f, (box.min.y + box.max.y) / 2.0f, (box.min.z + box.max.z) / 2.0f);
                            
                            rendering::Transform box_transform = rendering::create_transform(box_position, size, math::Quat());
                            box_transform.model = transform.transform.model * box_transform.model;

                            rendering::push_buffer_to_render_pass(renderer, renderer->render.bounding_box_buffer, renderer->render.bounding_box_material, box_transform, renderer->render.standard_opaque_pass, rendering::CommandType::WITH_DEPTH, rendering::PrimitiveType::LINE_LOOP);
                        }
                        
                        CombinedCommand &batch_command = command->commands[command->count];
                        batch_command.transform = transform.transform;
                        batch_command.material_handle = render.material_handles[pass_index];
                        batch_command.casts_shadows = render.casts_shadows;
                        batch_command.material_handle = render.material_handles[pass_index];
                        
                        command->count++;
                    }
                }
                else if(ent.comp_flags & scene::COMP_PARTICLES)
                {
                    if(ent.particle_system_handle.handle != -1)
                    {
                        scene::ParticleSystemComponent &ps = scene.particle_system_components[ent.particle_system_handle.handle];
                    
                        i32 _internal_handle = renderer->particles._internal_handles[ps.handle.handle - 1];
                        ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];
                    
                        system.transform.position = system.attributes.base_position;
                    
                        if(ent.comp_flags & scene::COMP_TRANSFORM)
                        {
                            system.transform.position += math::translation(transform.transform.model);
                            system.transform.scale = transform.transform.scale;
                            system.transform.orientation = transform.transform.orientation;
                            
                            if(transform.transform.dirty)
                            {
                                rendering::recompute_transform(system.transform);
                            }
                        }

                        if(system.simulating || system.paused)
                        {
                            particles_to_push[particles_count++] = ps.handle.handle;
                        }
                    }
                }
            }
        }

        rendering::update_lighting_ubo(renderer);

        for(i32 index = 0; index < pass_count; index++)
        {
            Pass &pass = passes[index];

            for(i32 i = 0; i < pass.command_count; i++)
            {
                QueuedRenderCommand &queued_command = pass.queued_commands[i];
                CombinedCommand &first_command = queued_command.commands[0];

                b32 has_rendered = false;

                for(i32 batch_index = 0; batch_index < queued_command.count; batch_index++)
                {
                    CombinedCommand &render_command = queued_command.commands[batch_index];

                    rendering::Material &mat_instance = get_material_instance(render_command.material_handle, renderer);
                
                    if (mat_instance.instanced_vertex_attribute_count == 0)
                    {
                        rendering::push_buffer_to_render_pass(renderer, queued_command.buffer_handle, render_command.material_handle, render_command.transform, pass.pass_handle, queued_command.ignore_depth ? rendering::CommandType::NO_DEPTH : rendering::CommandType::WITH_DEPTH);
                        has_rendered = true;
                        continue;
                    }
                }

                if(has_rendered)
                    continue;
                
                rendering::push_instanced_buffer_to_render_pass(renderer, queued_command.count, queued_command.buffer_handle, first_command.material_handle,
                                                                pass.pass_handle, queued_command.ignore_depth ? rendering::CommandType::NO_DEPTH : rendering::CommandType::WITH_DEPTH);
            }
        }
    
        for(i32 i = 0; i < particles_count; i++)
        {
            i32 _internal_handle = renderer->particles._internal_handles[particles_to_push[i] - 1];
            ParticleSystemInfo& system = renderer->particles.particle_systems[_internal_handle];

            rendering::BufferHandle buffer = system.attributes.buffer.handle != 0 ? system.attributes.buffer : (system.attributes.texture_handle.handle != 0 ? renderer->particles.textured_quad_buffer : renderer->particles.quad_buffer);

            rendering::push_instanced_buffer_to_render_pass(renderer, system.particle_count, buffer, system.material_handle, renderer->render.standard_opaque_pass, rendering::CommandType::WITH_DEPTH);
        }

        draw_gizmos(scene.scene_manager);
    }
}
