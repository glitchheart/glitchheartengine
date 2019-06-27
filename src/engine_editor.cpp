namespace editor
{
    static void _recursive_entity_item(scene::Entity& entity, scene::SceneManager *scene_manager)
    {
        if(entity.hide_in_ui)
        {
            return;
        }
    
        i32 tree_node_flags = ImGuiTreeNodeFlags_OpenOnArrow;
        if (entity.child_count == 0)
        {
            tree_node_flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (IS_ENTITY_HANDLE_VALID(scene_manager->selected_entity) && HANDLES_EQUAL(scene_manager->selected_entity, entity.handle))
        {
            tree_node_flags |= ImGuiTreeNodeFlags_Selected;
        }

        char buf[32];

        ImGui::PushID(entity.handle.handle);

        b32 is_active = scene::is_active(entity.handle, scene_manager->loaded_scene);
        if(!is_active)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.35f, 0.35f, 0.35f, 1.0f));
        }
        else
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }

        if (ImGui::TreeNodeEx(entity.name, tree_node_flags))
        {
            if (ImGui::IsItemClicked())
            {
                scene::select_entity(entity.handle, scene_manager);
            }

            for (i32 i = 0; i < entity.child_count; i++)
            {
                _recursive_entity_item(scene::get_entity(entity.children[i], scene_manager->loaded_scene), scene_manager);
            }


            ImGui::TreePop();
        }
        else
        {
            if (ImGui::IsItemClicked())
            {
                scene::select_entity(entity.handle, scene_manager);
            }
        }

        ImGui::PopStyleColor();
        
        ImGui::PopID();
    }

    static void _render_hierarchy(scene::Scene &scene)
    {
		static bool hierarchy_open = true;

		ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);

		ImGui::Begin("Hierarchy", &hierarchy_open, 0);
        
        for (i32 i = 0; i < scene.entity_count; i++)
        {
            scene::EntityHandle handle = scene.entities[i].handle;

            scene::Entity& entity = get_entity(handle, scene);

            if (!IS_ENTITY_HANDLE_VALID(entity.parent))
            {
                _recursive_entity_item(entity, scene.handle.manager);
            }
        }
    
        ImGui::End();
    }

    static void _render_inspector(scene::Scene &scene, InputController *input_controller)
    {
        scene::SceneManager *scene_manager = scene.handle.manager;
        
        if(IS_ENTITY_HANDLE_VALID(scene_manager->selected_entity))
        {
            scene::TransformComponent &selected_transform = scene::get_transform_comp(scene_manager->selected_entity, scene_manager->loaded_scene);
            scene::Entity& entity = scene::get_entity(scene_manager->selected_entity, scene_manager->loaded_scene);

            static bool open = true;
            
            ImGui::SetNextWindowSize(ImVec2(1000, 200), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowPos(ImVec2(core.renderer->window_width - 1000, 0), ImGuiCond_FirstUseEver);
            
            if(ImGui::Begin("Inspector", &open, 0))
            {
                bool is_active = scene::is_active(scene_manager->selected_entity, scene_manager->loaded_scene);
                if(ImGui::Checkbox("", &is_active))
                {
                    scene::set_active(scene_manager->selected_entity, is_active, scene_manager->loaded_scene);
                }

                ImGui::SameLine();

                ImGui::PushID("entity_name");
                ImGui::InputText("", entity.name, IM_ARRAYSIZE(entity.name));
                ImGui::PopID();
                
                if(ImGui::TreeNode("General"))
                {
                    bool show_bounding_box =  scene::get_bounding_box_enabled(scene_manager->selected_entity, scene_manager->loaded_scene);
                    if(ImGui::Checkbox("Show bounding box", &show_bounding_box))
                    {
                        scene::set_bounding_box_enabled(show_bounding_box, scene_manager->selected_entity, scene_manager->loaded_scene);
                    }
                    ImGui::TreePop();
                }
            
                if(ImGui::CollapsingHeader("Tags"))
                {
                    if(entity.tags.tag_count == 0)
                    {
                        ImGui::Text("No tags");
                    }
                    else
                    {
                        for(i32 i = 0; i < entity.tags.tag_count; i++)
                        {
                            ImGui::Text(entity.tags.tags[i]);
                        }
                    }

                    static char tag_text[256];
                    
                    if(ImGui::Button("Add"))
                    {
                        if(strlen(tag_text) > 0)
                        {
                            scene::set_entity_tag(tag_text, entity.handle, scene_manager->loaded_scene);
                            memset(tag_text, 0, strlen(tag_text));
                        }
                    }
                    ImGui::SameLine();
                    ImGui::InputText("", tag_text, IM_ARRAYSIZE(tag_text));
                }

                if(ImGui::CollapsingHeader("Transform"))
                {
                    math::Vec3 position = selected_transform.transform.position;
                    float arr[3];
                    arr[0] = position.x;
                    arr[1] = position.y;
                    arr[2] = position.z;

                    ImGui::DragFloat3("Position", arr, core.delta_time);
                    scene::set_position(selected_transform, math::Vec3(arr[0], arr[1], arr[2]));

                    math::Vec3 rotation = selected_transform.transform.euler_angles;
                    float arr1[3];
                    arr1[0] = rotation.x;
                    arr1[1] = rotation.y;
                    arr1[2] = rotation.z;

                    ImGui::DragFloat3("Rotation", arr1, core.delta_time);
                    scene::set_rotation(selected_transform, math::Vec3(arr1[0], arr1[1], arr1[2]));

                    math::Vec3 scale = selected_transform.transform.scale;
                    float arr2[3];
                    arr2[0] = scale.x;
                    arr2[1] = scale.y;
                    arr2[2] = scale.z;

                    ImGui::DragFloat3("Scale", arr2, core.delta_time);
                    scene::set_scale(selected_transform, math::Vec3(arr2[0], arr2[1], arr2[2]));
                }

                if(scene::has_light_component(scene_manager->selected_entity, scene_manager->loaded_scene))
                {
                    if(ImGui::CollapsingHeader("Light"))
                    {
                        scene::LightComponent &light = scene::get_light_comp(scene_manager->selected_entity, scene_manager->loaded_scene);
                        char *light_types[] = {"Directional", "Point"};

                        i32 light_type = (i32)light.type;
                        ImGui::Combo("Light type", &light_type, light_types, IM_ARRAYSIZE(light_types));
                        light.type = (scene::LightType)light_type;
                        
                        math::Vec3 *ambient = nullptr;
                        math::Vec3 *diffuse = nullptr;
                        math::Vec3 *specular = nullptr;
                
                        switch(light.type)
                        {
                        case scene::LightType::DIRECTIONAL:
                        {
                            ambient = &light.dir_light.ambient;
                            diffuse = &light.dir_light.diffuse;
                            specular = &light.dir_light.specular;

                            ImGui::SliderFloat3("Direction", light.dir_light.direction.e, -1.0f, 1.0f);
                        }
                        break;
                        case scene::LightType::POINT:
                        {
                            ambient = &light.point_light.ambient;
                            diffuse = &light.point_light.diffuse;
                            specular = &light.point_light.specular;

                            ImGui::DragFloat3("Position", light.point_light.position.e);
                            ImGui::DragFloat("Constant", &light.point_light.constant);
                            ImGui::DragFloat("Linear", &light.point_light.linear);
                            ImGui::DragFloat("Quadratic", &light.point_light.quadratic);
                        }
                        break;
                        default:
                        log_error("ERROR in inspector: spotlight not yet supported");
                        assert(false);
                        }

                        ImGui::SliderFloat3("Ambient", ambient->e, 0.0f, 1.0f);
                        ImGui::SliderFloat3("Diffuse", diffuse->e, 0.0f, 1.0f);
                        ImGui::SliderFloat3("Specular", specular->e, 0.0f, 1.0f);
                    }
                }

                if(ImGui::CollapsingHeader("Fields"))
                {
                    scene::EntityData *data_ptr = entity.entity_data;
            
                    for(i32 i = 0; i < entity.type_info.field_count; i++)
                    {
                        scene::Field &field = entity.type_info.fields[i];
                        unsigned char *ptr = ((unsigned char *)data_ptr + field.offset);
                
                        switch(field.type)
                        {
                        case scene::FieldType::INT:
                        {
                            ImGui::InputInt(field.name, ((i32*)ptr));
                        }
                        break;
                        case scene::FieldType::UINT:
                        {
                            ImGui::InputInt(field.name, ((i32*)ptr));
                        }
                        break;
                        case scene::FieldType::FLOAT:
                        {
                            ImGui::DragFloat(field.name, ((r32*)ptr), core.delta_time);
                        }
                        break;
                        case scene::FieldType::VEC2:
                        {
                        
                            ImGui::DragFloat2(field.name, ((math::Vec2*)ptr)->e, core.delta_time);
                        }
                        break;
                        case scene::FieldType::VEC3:
                        {
                            ImGui::DragFloat2(field.name, ((math::Vec3*)ptr)->e, core.delta_time);
                        }
                        break;
                        default:
                        break;
                        }
                    }
                }

                if(scene::has_particle_component(scene_manager->selected_entity, scene_manager->loaded_scene))
                {
                    if(ImGui::CollapsingHeader("Particle system"))
                    {
                        ParticleSystemHandle ps_handle = scene::get_particle_system_comp(scene_manager->selected_entity, scene_manager->loaded_scene).handle;
                        ParticleSystemInfo* ps = core.renderer->particles.api->get_particle_system_info(ps_handle, core.renderer);

                        ParticleSystemAttributes& attributes = ps->attributes;

                        if(ImGui::Button("Start"))
                        {
                            scene_manager->renderer->particles.api->start_particle_system(ps_handle, scene_manager ->renderer);
                        }

                        if(ImGui::CollapsingHeader("Info"))
                        {
                            ImGui::Text("Max particles: %d", ps->max_particles);
                            ImGui::Text("Active particles: %d", ps->max_particles - ps->dead_particle_count * 4);
                        }
                    
                        if(ImGui::CollapsingHeader("Attributes"))
                        {
                            bool looping = attributes.looping;
                            bool paused = ps->paused;
                            bool prewarm = attributes.prewarm;
                        
                            if(ImGui::Checkbox("Looping", &looping))
                            {
                                attributes.looping = looping;
                            }

                            if(ImGui::Checkbox("Paused", &paused))
                            {
                                ps->paused = paused;
                                core.renderer->particles.api->pause_particle_system(ps_handle, core.renderer, paused);
                            }

                            ImGui::DragScalar("Duration", ImGuiDataType_Double, &attributes.duration, core.delta_time);

                            if(ImGui::Checkbox("Prewarm", &prewarm))
                            {
                                attributes.prewarm = prewarm;
                            }

                            i32 particle_space = (i32)attributes.particle_space;
                            
                            char* particle_space_types[] = {"World", "Local"};
                            ImGui::Combo("Particle space", &particle_space, particle_space_types, IM_ARRAYSIZE(particle_space_types));

                            attributes.particle_space = (ParticleSpace)particle_space;
                            
                            // Start size
                            char* start_param_types[] = {"Constant", "Random Between Two Constants"};

                            i32 start_size_type = (i32)attributes.start_size_type;
                            
                            ImGui::Combo("Start size type", &start_size_type, start_param_types, IM_ARRAYSIZE(start_param_types));
                            attributes.start_size_type = (StartParameterType)start_size_type;
                            
                            if(attributes.start_size_type == StartParameterType::CONSTANT)
                            {
                                ImGui::DragFloat("Start size", &attributes.size.constant.start_size);
                            }
                            else
                            {
                                math::Vec2 constants(attributes.size.random_between_two_constants.s0, attributes.size.random_between_two_constants.s1);
                                ImGui::DragFloat2("Start size", constants.e);
                            
                                attributes.size.random_between_two_constants.s0 = constants.x;
                                attributes.size.random_between_two_constants.s1 = constants.y;
                            }
                            // End start size

                            // Start lifetime
                            i32 start_life_time_type = (i32)attributes.start_life_time_type;
                            
                            ImGui::Combo("Start lifetime type", &start_life_time_type, start_param_types, IM_ARRAYSIZE(start_param_types));

                            attributes.start_life_time_type = (StartParameterType)start_life_time_type;
                                
                            if(attributes.start_life_time_type == StartParameterType::CONSTANT)
                            {
                                ImGui::DragScalar("Start lifetime", ImGuiDataType_Double, &attributes.life.constant.life_time, core.delta_time);
                            }
                            else
                            {
                                math::Vec2 constants(attributes.life.random_between_two_constants.l0, attributes.life.random_between_two_constants.l1);
                                ImGui::DragFloat2("Start lifetime", constants.e);
                            
                                attributes.life.random_between_two_constants.l0 = constants.x;
                                attributes.life.random_between_two_constants.l1 = constants.y;
                            }
                
                            // End start lifetime
                
                            // // Start speed
                            i32 start_speed_type = (i32)attributes.start_speed_type;
                            
                            ImGui::Combo("Start speed type", &start_speed_type, start_param_types, IM_ARRAYSIZE(start_param_types));

                            attributes.start_speed_type = (StartParameterType)start_speed_type;
                            
                            if(attributes.start_speed_type == StartParameterType::CONSTANT)
                            {
                                ImGui::DragFloat("Start speed", &attributes.speed.constant.start_speed);
                            }
                            else
                            {
                                math::Vec2 constants(attributes.speed.random_between_two_constants.s0, attributes.speed.random_between_two_constants.s1);
                                ImGui::DragFloat2("Start speed", constants.e);
                            
                                attributes.speed.random_between_two_constants.s0 = constants.x;
                                attributes.speed.random_between_two_constants.s1 = constants.y;
                            }

                            // End start speed
                            ImGui::DragFloat3("Direction", attributes.direction.e, 0.0f, 1.0f);
                            ImGui::DragFloat3("Base position", attributes.base_position.e);
                            ImGui::DragInt("Per second", &attributes.particles_per_second);
                            ImGui::DragFloat("Gravity", &attributes.gravity);
                        }

                        if(ImGui::CollapsingHeader("Emission module"))
                        {
                            // Emission module
                            EmissionModule& emission_module = attributes.emission_module;
                
                            char* func_types[] = {"Direction", "Random direction", "Square 2D", "Square 2D random"
                                                  , "Square", "Square random", "Disc", "Disc random", "Circle", "Circle randome"};

                            i32 func_type = (i32)emission_module.emitter_func_type;
                            
                            ImGui::Combo("Emission function type", &func_type, func_types, IM_ARRAYSIZE(func_types));

                            emission_module.emitter_func_type = (EmissionFuncType)func_type;
                            
                            ImGui::DragFloat("Min", &emission_module.min);
                            ImGui::SameLine();
                            ImGui::DragFloat("Max", &emission_module.max);

                            if(ImGui::CollapsingHeader("Color over time"))
                            {
                                i32 deleted_index = -1;

                                ImGui::ListBoxHeader("Values");

                                static i32 selected_color_index = 0;
                                
                                for(i32 i = 0; i < ps->color_over_lifetime.value_count; i++)
                                {
                                    math::Rgba color = ps->color_over_lifetime.values[i];
                                    r64 key = ps->color_over_lifetime.keys[i];
                                    char buf[128];
                                    sprintf(buf, "Time: %f Value: %f %f %f %f", key, color.r, color.g, color.b, color.a);
                    
                                    if(ImGui::Selectable(buf, selected_color_index == i))
                                    {
                                        selected_color_index = i;
                                        if(KEY_DOWN(Key_Backspace) || KEY_DOWN(Key_Delete))
                                        {
                                            deleted_index = selected_color_index;
                                        }
                                    }
                                }

                                ImGui::ListBoxFooter();

                                if(deleted_index != -1)
                                {
                                    core.renderer->particles.api->remove_color_key(*ps, ps->color_over_lifetime.keys[deleted_index]);
                                }
                
                                for(i32 i = 0; i < ps->color_over_lifetime.value_count - 1; i++)
                                {
                                    if(ps->color_over_lifetime.keys[i] > ps->color_over_lifetime.keys[i + 1])
                                    {
                                        r64 current_key = ps->color_over_lifetime.keys[i];
                                        math::Rgba current_value = ps->color_over_lifetime.values[i];

                                        scene_manager->renderer->particles.api->remove_color_key(*ps, current_key);
                                        selected_color_index = core.renderer->particles.api->add_color_key(*ps, current_key, current_value);
                                        break;
                                    }
                                }

                                static r64 new_color_key = 0.0;
                                static math::Vec4 new_color;
                                
                                if(ps->color_over_lifetime.values)
                                {
                                    ImGui::DragScalar("Selected key", ImGuiDataType_Double, &ps->color_over_lifetime.keys[selected_color_index], core.delta_time);
                                    ImGui::ColorEdit4("Selected color", ps->color_over_lifetime.values[selected_color_index].e);
                                }
                                else
                                {
                                    // ui::field_color(global_state->ui_state, math::Rgba(1.0f), "Selected value");
                                    // math::clamp(0.0, ui::field_r64(global_state->ui_state, 0.0, "Selected key"), 1.0);
                                }

                                ImGui::DragScalar("New key", ImGuiDataType_Double, &new_color_key, core.delta_time);
                                ImGui::ColorEdit4("New color", new_color.e);

                                if(ImGui::Button("Add"))
                                {
                                    core.renderer->particles.api->add_color_key(*ps, new_color_key, new_color);
                                }

                            }

                            if(ImGui::CollapsingHeader("Size over time"))
                            {
                                static i32 selected_size_index = 0;
                                
                                i32 deleted_index = -1;
                            
                                ImGui::ListBoxHeader("Values");
                            
                                for(i32 i = 0; i < ps->size_over_lifetime.value_count; i++)
                                {
                                    math::Vec2 size = ps->size_over_lifetime.values[i];
                                    r64 key = ps->size_over_lifetime.keys[i];
                                    char buf[128];
                                    sprintf(buf, "Time: %f Value: %f %f", key, size.x, size.y);
                                
                                    if(ImGui::Selectable(buf, selected_size_index == i))
                                    {
                                        selected_size_index = i;
                                    
                                        if(KEY_DOWN(Key_Backspace) || KEY_DOWN(Key_Delete))
                                        {
                                            deleted_index = selected_size_index;
                                        }
                                    }
                                }
                                
                                ImGui::ListBoxFooter();
                            
                                if(deleted_index != -1)
                                {
                                    core.renderer->particles.api->remove_size_key(*ps, ps->size_over_lifetime.keys[deleted_index]);
                                }

                                for(i32 i = 0; i < ps->size_over_lifetime.value_count - 1; i++)
                                {
                                    if(ps->size_over_lifetime.keys[i] > ps->size_over_lifetime.keys[i + 1])
                                    {
                                        r64 current_key = ps->size_over_lifetime.keys[i];
                                        math::Vec2 current_value = ps->size_over_lifetime.values[i];

                                        scene_manager->renderer->particles.api->remove_size_key(*ps, current_key);
                                        selected_size_index = core.renderer->particles.api->add_size_key(*ps, current_key, current_value);
                                        break;
                                    }
                                }

                                if(ps->size_over_lifetime.values)
                                {
                                    ImGui::DragScalar("Selected key", ImGuiDataType_Double, &ps->size_over_lifetime.keys[selected_size_index], core.delta_time);
                                    ImGui::DragFloat2("Selected size", ps->size_over_lifetime.values[selected_size_index].e);
                                }
                                else
                                {
                                    // ui::field_v2(global_state->ui_state, math::Vec2(0.0f), "Selected value");
                                    // math::clamp(0.0, ui::field_r64(global_state->ui_state, 0.0, "Selected key"), 1.0);
                                }

                                static r64 new_size_key = 0.0;
                                static math::Vec2 new_size;
                                
                                ImGui::DragScalar("New key", ImGuiDataType_Double, &new_size_key, core.delta_time);
                                ImGui::DragFloat2("New size", new_size.e);
                            
                                if(ImGui::Button("Add"))
                                {
                                    core.renderer->particles.api->add_size_key(*ps, new_size_key, new_size);
                                }
                            }
                        
                            if(ImGui::CollapsingHeader("Size over time"))
                            {
                                i32 deleted_index = -1;

                                static i32 selected_speed_index = 0;
                                
                                ImGui::ListBoxHeader("Values");
                            
                                for(i32 i = 0; i < ps->speed_over_lifetime.value_count; i++)
                                {
                                    r32 speed = ps->speed_over_lifetime.values[i];
                                    r64 key = ps->speed_over_lifetime.keys[i];
                                    char buf[128];
                                    sprintf(buf, "Time: %f Value: %f", key, speed);
                    
                                    if(ImGui::Selectable(buf, selected_speed_index == i))
                                    {
                                        selected_speed_index = i;
                                    
                                        if(KEY_DOWN(Key_Backspace) || KEY_DOWN(Key_Delete))
                                        {
                                            deleted_index = selected_speed_index;
                                        }
                                    }
                                }

                                ImGui::ListBoxFooter();
                                
                                if(deleted_index != -1)
                                {
                                    core.renderer->particles.api->remove_speed_key(*ps, ps->speed_over_lifetime.keys[deleted_index]);
                                }

                                deleted_index = -1;

                                for(i32 i = 0; i < ps->speed_over_lifetime.value_count - 1; i++)
                                {
                                    if(ps->speed_over_lifetime.keys[i] > ps->speed_over_lifetime.keys[i + 1])
                                    {
                                        r64 current_key = ps->speed_over_lifetime.keys[i];
                                        r32 current_value = ps->speed_over_lifetime.values[i];

                                        scene_manager->renderer->particles.api->remove_speed_key(*ps, current_key);
                                        selected_speed_index = core.renderer->particles.api->add_speed_key(*ps, current_key, current_value);
                                        break;
                                    }
                                }

                                if(ps->speed_over_lifetime.values)
                                {
                                    ImGui::DragScalar("Selected key", ImGuiDataType_Double, &ps->speed_over_lifetime.keys[selected_speed_index], core.delta_time);
                                    ImGui::DragFloat("Selected speed", &ps->speed_over_lifetime.values[selected_speed_index]);
                                }
                                else
                                {
                                    // ui::field_r32(global_state->ui_state, 0.0f, "Selected value");
                                    // math::clamp(0.0, ui::field_r64(global_state->ui_state, 0.0, "Selected key"), 1.0);
                                }

                                static r64 new_speed_key = 0.0;
                                static r32 new_speed = 0.0f;

                                ImGui::DragScalar("New key", ImGuiDataType_Double, &new_speed_key, core.delta_time);
                                ImGui::DragFloat("New speed", &new_speed);

                                if(ImGui::Button("Add"))
                                {
                                    core.renderer->particles.api->add_speed_key(*ps, new_speed_key, new_speed);
                                }
                            }
                        }
                    }
                }
            }
            ImGui::End();
        }
    }

    static b32 is_template(const char *file_name)
    {
        size_t len = strlen(file_name);
        // printf("-4 %c -3 %c -2 %c -1 %c -0 %c\n", file_name[len - 4],
        //        , file_name[len - 3]
        //        , file_name[len - 2]
        //        , file_name[len - 1]
        //        , file_name[len]);
        
        return file_name[len - 5] == '.'
           && file_name[len - 4] == 't'
           && file_name[len - 3] == 'm'
           && file_name[len - 2] == 'p'
            && file_name[len - 1] == 'l';
    }

    static b32 is_scene(const char *file_name)
    {
        size_t len = strlen(file_name);
        // printf("-4 %c -3 %c -2 %c -1 %c -0 %c\n", file_name[len - 4],
        //        , file_name[len - 3]
        //        , file_name[len - 2]
        //        , file_name[len - 1]
        //        , file_name[len]);
        
        return file_name[len - 4] == '.'
            && file_name[len - 3] == 'g'
            && file_name[len - 2] == 's'
            && file_name[len - 1] == 'c';
    }
    
    static void _render_resources(project::ProjectState *state, scene::SceneManager *scene_manager)
    {
        FileList &current_structure = state->resources.resource_file_structures[state->resources.structure_count - 1];
    
        char buf[32];

        static bool open = true;

        ImGui::Begin("Resources", &open);
        ImGui::Text(current_structure.path);
        ImGui::SameLine();

        if(ImGui::Button("Reload"))
        {
            clear(&state->resources.arenas[state->resources.structure_count - 1]);
            
            // CLEAR!
            current_structure.files = push_array(&state->resources.arenas[state->resources.structure_count - 1], 256, File);
            current_structure.dirs = push_array(&state->resources.arenas[state->resources.structure_count - 1], 256, File);
            current_structure.allocated = true;
                
            platform.list_all_files_and_directories(current_structure.path, &current_structure);
        }
        
        ImGui::Separator();
    
        for(i32 i = 0; i < current_structure.dir_count; i++)
        {
            char *name = current_structure.dirs[i].name;

            if(state->resources.structure_count == 1 && starts_with(name, "."))
                continue;

            if(strlen(name) == 1 && name[0] == '.')
                continue;

            sprintf(buf, "[DIR] %s", name);
        
            if(ImGui::Selectable(buf))
            {
                if(starts_with(name, ".."))
                {
                    state->resources.structure_count--;
                }
                else
                {
                    FileList &list = state->resources.resource_file_structures[state->resources.structure_count++];
                
                    sprintf(list.path, "%s/%s", current_structure.path, name);

                    if(!list.allocated)
                    {
                        list.files = push_array(&state->resources.arenas[state->resources.structure_count - 1], 256, File);
                        list.dirs = push_array(&state->resources.arenas[state->resources.structure_count - 1], 256, File);
                        list.allocated = true;
                    }
                
                    platform.list_all_files_and_directories(list.path, &list);
                }
            
            }
        }

        for(i32 i = 0; i < current_structure.file_count; i++)
        {
            if(ImGui::Selectable(current_structure.files[i].name, false, ImGuiSelectableFlags_AllowDoubleClick))
            {
                if(ImGui::IsMouseDoubleClicked(0))
                {
                    if(is_template(current_structure.files[i].name))
                    {
                        char full_path[64];
                        sprintf(full_path, "%s/%s", current_structure.path, current_structure.files[i].name);

                        Camera &camera = scene::get_editor_camera(scene_manager->loaded_scene);
                        place_entity_from_template(camera.position + camera.forward * 15.0f, full_path, scene_manager->loaded_scene, true, true);
                    }
                    else if(is_scene(current_structure.files[i].name))
                    {
                        char full_path[64];
                        sprintf(full_path, "%s/%s", current_structure.path, current_structure.files[i].name);
                        scene::SceneHandle scene = scene::create_scene_from_file(full_path, scene_manager, true, 4096);
                        scene::load_scene(scene);
                    }
                }
            }
        } 
    
    
        ImGui::End();
    }

    static void set_editor_mode(EditorMode mode, EditorState *state)
    {
        state->mode = mode;
    }
}


