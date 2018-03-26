#include "animation.h"

enum CameraFlags
{
    C_FLAG_ORTHOGRAPHIC = (1 << 0),
    C_FLAG_PERSPECTIVE  = (1 << 1),
    C_FLAG_NO_LOOK_AT     = (1 << 2)
};

struct CameraParams
{
    u32 view_flags;
};

static CameraParams default_camera_params()
{
    CameraParams params;
    params.view_flags = C_FLAG_ORTHOGRAPHIC | C_FLAG_NO_LOOK_AT;
    return params;
}

static CameraParams orthographic_camera_params()
{
    CameraParams params;
    params.view_flags = C_FLAG_ORTHOGRAPHIC;
    return params;
}

static CameraParams perspective_camera_params()
{
    CameraParams params;
    params.view_flags = C_FLAG_PERSPECTIVE;
    return params;
}

// @Incomplete
static inline void camera_transform(Renderer& renderer, Camera& camera, math::Vec3 position = math::Vec3(), math::Quat orientation = math::Quat(), math::Vec3 target = math::Vec3(), r32 zoom = 1.0f, r32 near = -1.0f, r32 far = 1.0f, CameraParams params = default_camera_params())
{
    camera.viewport_width = renderer.window_width;
    camera.viewport_height = renderer.window_height;
    if(params.view_flags & C_FLAG_ORTHOGRAPHIC)
    {
        camera.projection_matrix = math::ortho(0.0f, renderer.viewport[2] / zoom, 0.0f, renderer.viewport[3] / zoom, near, far);
        camera.view_matrix = math::Mat4(1.0f);
        
        camera.position = position;
        camera.orientation = orientation;
        camera.target = target;
        
        if(!is_identity(orientation))
        {
            camera.view_matrix = to_matrix(orientation) * camera.view_matrix;
        }
        else if(!(params.view_flags & C_FLAG_NO_LOOK_AT))
        {
            auto dist = sqrt(1.0f / 3.0f);
            camera.view_matrix = math::look_at(math::Vec3(dist, dist, dist), math::Vec3(0.0f));
        }
        
        camera.view_matrix = math::translate(camera.view_matrix, math::Vec3(-position.x, -position.y, position.z));
        
        //camera.view_matrix = math::Translate(camera.view_matrix, position);
        camera.view_matrix = math::translate(camera.view_matrix, math::Vec3(renderer.viewport[2] / zoom / 2, renderer.viewport[3] / zoom / 2, 0.0f));
        
        
    }
    else if(params.view_flags & C_FLAG_PERSPECTIVE)
    {
        camera.projection_matrix = math::perspective((r32)renderer.viewport[2] / (r32)renderer.viewport[3], 0.60f, 0.1f, 100.0f);
        
        camera.view_matrix = math::Mat4(1.0f);
        
        auto dist = sqrt(1.0f / 3.0f);
        
        dist = 20.0f;
        
        camera.view_matrix = math::look_at(math::Vec3(dist, dist, dist), target);
        
        if(!is_identity(orientation))
        {
            camera.view_matrix = to_matrix(orientation) * camera.view_matrix;
        }
        
        camera.position = position;
        camera.orientation = orientation;
        camera.target = target;
    }
}

// The InfoHandle is used to be able to reference the same animation without having to load the animation again. 
static void add_animation(Renderer& renderer, SpritesheetAnimation animation, const char* animation_name)
{
    strcpy(animation.name, animation_name);
    renderer.spritesheet_animations[renderer.spritesheet_animation_count++] = animation;
    Assert(renderer.spritesheet_animation_count < MAX_SPRITESHEET_ANIMATIONS);
}

static void load_shader(MemoryArena* arena, const char* full_shader_path, Renderer& renderer, i32* handle)
{
    ShaderData* shader_data = &renderer.shader_data[renderer.shader_count];
    shader_data->handle = renderer.shader_count++;
    *handle = shader_data->handle + 1;
    sprintf(shader_data->name, "%s", full_shader_path);
    shader_data->vertex_shader_content = 0;
    shader_data->fragment_shader_content = 0;
    
    u32 size = 0;
    FILE* file;
    
    auto temp_mem = begin_temporary_memory(arena);
    file = fopen(concat(full_shader_path, ".vert", arena), "rb");
    
    if(file)
    {
        fseek(file, 0, SEEK_END);
        size = (u32)ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // @Incomplete: Use built-in memory arena
        shader_data->vertex_shader_content = (char*) malloc(sizeof(char) * size + 1);
        fread(shader_data->vertex_shader_content, 1, (size_t)size, file);
        shader_data->vertex_shader_content[size] = '\0';
        fclose(file);
    }
    else
    {
        printf("Invalid file path: '%s'\n", full_shader_path);
    }
    
    file = fopen(concat(full_shader_path, ".frag", arena), "rb");
    
    if(file)
    {
        fseek(file, 0, SEEK_END);
        size = (u32)ftell(file);
        fseek(file, 0, SEEK_SET);
        
        // @Incomplete: Use built-in memory arena
        shader_data->fragment_shader_content = (char*) malloc(sizeof(char) * size + 1);
        fread(shader_data->fragment_shader_content, 1, (size_t)size, file);
        shader_data->fragment_shader_content[size] = '\0';
        
        fclose(file);
    }
    else
    {
        printf("Invalid file path: '%s'\n", full_shader_path);
    }
    end_temporary_memory(temp_mem);
}

#define get_texture_size(handle) texture_size(handle, renderer)
static math::Vec2i texture_size(i32 texture_handle, Renderer& renderer)
{
    if(texture_handle <= renderer.texture_count)
    {
        texture_data data = renderer.texture_data[texture_handle - 1];
        return math::Vec2i(data.width, data.height);
    }
    return math::Vec2i();
}

static void load_texture(const char* full_texture_path, Renderer& renderer, i32* handle = 0)
{
    texture_data* texture_data = &renderer.texture_data[renderer.texture_count];
    
    texture_data->handle = renderer.texture_count++;
    
    texture_data->image_data = stbi_load(full_texture_path, &texture_data->width, &texture_data->height, 0, STBI_rgb_alpha);
    
    if(!texture_data->image_data)
    {
        printf("Texture cold not be loaded: %s\n", full_texture_path);
    }
    
    if(handle)
        *handle = texture_data->handle + 1; // We add one to the handle, since we want 0 to be an invalid handle
}

static void load_textures(Renderer& renderer, const char* path, MemoryArena* arena)
{
    texture_data_map_init(&renderer.texture_map, hash_string_jenkins, 64);
    
    DirectoryData dir_data = {};
    platform.get_all_files_with_extension(arena, path, "png", &dir_data, true);
    
    for (i32 file_index = 0; file_index < dir_data.files_length; file_index++)
    {
        load_texture(dir_data.file_paths[file_index], renderer);
    }
}

static void load_textures(Renderer& renderer, MemoryArena* arena)
{
    load_textures(renderer, "../assets/textures/", arena);
}

static RenderCommand* push_next_command(Renderer& renderer, b32 is_ui)
{
    if(is_ui)
    {
        renderer.ui_command_count++;
        RenderCommand* command = push_struct(&renderer.ui_commands, RenderCommand);
        command->shader_handle = -1;
        return command;
    }
    else
    {
        renderer.command_count++;
        RenderCommand* command = push_struct(&renderer.commands, RenderCommand);
        command->shader_handle = -1;
        return command;
    }
}

static void enable_depth_test(Renderer& renderer)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_DEPTH_TEST;
    render_command->depth_test.on = true;
}

static void disable_depth_test(Renderer& renderer)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_DEPTH_TEST;
    render_command->depth_test.on = false;
}

static void push_shader(Renderer& renderer, i32 shader_handle, ShaderAttribute* attributes, i32 attribute_count)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_SHADER_START;
    render_command->shader.handle = shader_handle;
    render_command->shader.attributes = attributes;
    render_command->shader.attribute_count = attribute_count;
}

static void end_shader(Renderer& renderer)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_SHADER_END;
}

static void push_line(Renderer& renderer, math::Vec3 point1, math::Vec3 point2, r32 line_width, math::Rgba color, b32 is_ui = false)
{
    RenderCommand* render_command = push_next_command(renderer, is_ui);
    
    render_command->type = RENDER_COMMAND_LINE;
    render_command->line.point1 = point1;
    render_command->line.point2 = point2;
    render_command->line.line_width = line_width;
    render_command->line.color = color;
    render_command->is_ui = is_ui;
}

#define PUSH_TEXT(text, position, color, font_handle) push_text(renderer, text, position, 1.0f, font_handle, color)
#define PUSH_CENTERED_TEXT(text, position, color, font_handle) push_text(renderer, text, position, 1.0f, font_handle, color, ALIGNMENT_CENTER)
static void push_text(Renderer& renderer, const char* text, math::Vec3 position, r32 scale, i32 font_handle, math::Rgba color, Alignment alignment = ALIGNMENT_LEFT, b32 is_ui = true)
{
    RenderCommand* render_command = push_next_command(renderer, is_ui);
    
    render_command->type = RENDER_COMMAND_TEXT;
    
    strcpy(render_command->text.text, text);
    
    render_command->text.position = position;
    if(scale == 0.0f)
        render_command->text.scale = 1.0f;
    else
        render_command->text.scale = scale;
    //render_command->Text.FontType = FontType;
    render_command->text.font_handle = font_handle;
    render_command->text.color = color;
    render_command->text.alignment = alignment;
    render_command->is_ui = is_ui;
}

static void push_text(Renderer& renderer, TextInfo text_info)
{
    push_text(renderer, text_info.text, text_info.position, text_info.scale, text_info.font_handle, text_info.render_info.color, text_info.alignment, text_info.render_info.is_ui);
}

static void push_filled_quad(Renderer& renderer, math::Vec3 position, b32 flipped, math::Vec3 size, math::Vec3 rotation = math::Vec3(), math::Rgba color = math::Rgba(1.0f, 1.0f, 1.0f, 1.0f), i32 texture_handle = 0, b32 is_ui = true, i32 animation_controller_handle = 0, b32 with_origin = false, math::Vec2 origin = math::Vec2(0.0f, 0.0f), i32 shader_handle = 0, ShaderAttribute* shader_attributes = 0, i32 shader_attribute_count = 0, math::Vec2 texture_offset = math::Vec2(-1.0f, -1.0f), math::Vec2i frame_size = math::Vec2i(0, 0))
{
    RenderCommand* render_command = push_next_command(renderer, is_ui);
    
    render_command->type = RENDER_COMMAND_QUAD;
    render_command->position = position;
    render_command->rotation = rotation;
    render_command->with_origin = with_origin;
    render_command->origin = origin;
    render_command->scale = size;
    render_command->quad.flipped = flipped;
    render_command->quad.color = color;
    render_command->quad.outlined = false;
    render_command->quad.texture_handle = texture_handle - 1;
    
    if(animation_controller_handle != 0)
    {
        auto& controller = renderer.animation_controllers[animation_controller_handle - 1];
        SpritesheetAnimation& animation = renderer.spritesheet_animations[controller.nodes[controller.current_node].animation_handle];
        SpritesheetFrame& frame = animation.frames[controller.current_frame_index];
        render_command->quad.texture_handle = animation.texture_handle - 1;
        render_command->quad.texture_size = math::Vec2((r32)renderer.texture_data[render_command->quad.texture_handle].width, (r32)renderer.texture_data[render_command->quad.texture_handle].height);
        render_command->quad.frame_size = math::Vec2i(frame.frame_width, frame.frame_height);
        render_command->quad.texture_offset = math::Vec2(frame.x, frame.y);
        render_command->quad.for_animation = true;
    }
    else
    {
        render_command->quad.texture_offset = texture_offset;
        render_command->quad.frame_size = frame_size;
        
        if(texture_handle != 0)
        {
            render_command->quad.texture_size = math::Vec2((r32)renderer.texture_data[render_command->quad.texture_handle].width, (r32)renderer.texture_data[render_command->quad.texture_handle].height);
        }
    }
    
    render_command->shader_handle = shader_handle - 1;
    render_command->shader_attributes = shader_attributes;
    render_command->shader_attribute_count = shader_attribute_count;
    
    render_command->is_ui = is_ui;
}

static void push_filled_quad(Renderer& renderer, QuadInfo quad_info)
{
    push_filled_quad(renderer, quad_info.transform_info.position, quad_info.flipped, quad_info.transform_info.scale, quad_info.transform_info.rotation, quad_info.render_info.color, quad_info.texture_info.texture_handle, quad_info.render_info.is_ui, quad_info.animation_controller_handle, quad_info.render_info.with_origin, quad_info.render_info.origin, quad_info.shader_info.shader_handle, quad_info.shader_info.shader_attributes, quad_info.shader_info.shader_attribute_count, quad_info.texture_info.texture_offset, quad_info.texture_info.frame_size);
}

static void push_filled_quad_not_centered(Renderer& renderer, math::Vec3 position, b32 flipped, math::Vec3 size, math::Vec3 rotation = math::Vec3(), math::Rgba color = math::Rgba(1.0f, 1.0f, 1.0f, 1.0f), i32 texture_handle = 0, b32 is_ui = true, i32 animation_controller_handle = 0, i32 shader_handle = 0, ShaderAttribute* shader_attributes = 0, i32 shader_attribute_count = 0, math::Vec2 texture_offset = math::Vec2(-1.0f, -1.0f), math::Vec2i frame_size = math::Vec2i(0, 0))
{
    push_filled_quad(renderer, position, flipped, size, rotation, color, texture_handle, is_ui, animation_controller_handle, true, math::Vec2(), shader_handle, shader_attributes, shader_attribute_count, texture_offset, frame_size); 
}

static void push_outlined_quad(Renderer& renderer, math::Vec3 position,  math::Vec3 size, math::Vec3 rotation, math::Rgba color, b32 is_ui = false, r32 line_width = 1.0f)
{
    RenderCommand* render_command = push_next_command(renderer, is_ui);
    
    render_command->type = RENDER_COMMAND_QUAD;
    render_command->position = position;
    render_command->rotation = rotation;
    render_command->scale = size;
    render_command->quad.color = color;
    render_command->quad.outlined = true;
    render_command->quad.texture_handle = 0;
    render_command->quad.line_width = line_width;
    render_command->is_ui = is_ui;
}

static void push_wireframe_cube(Renderer& renderer, math::Vec3 position, math::Vec3 scale, math::Quat orientation, math::Rgba color, r32 line_width)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    
    render_command->type = RENDER_COMMAND_WIREFRAME_CUBE;
    render_command->wireframe_cube.color = color;
    render_command->wireframe_cube.line_width = line_width;
    render_command->position = position;
    render_command->scale = scale;
    render_command->orientation = orientation;
    render_command->is_ui = false;
}

static void push_spotlight(Renderer& renderer, math::Vec3 position, math::Vec3 direction, r32 cut_off, r32 outer_cut_off, math::Vec3 ambient, math::Vec3 diffuse, math::Vec3 specular, r32 constant, r32 linear, r32 quadratic)
{
    RenderCommand* render_command = push_struct(&renderer.light_commands, RenderCommand);
    renderer.light_command_count++;
    
    render_command->type = RENDER_COMMAND_SPOTLIGHT;
    
    render_command->position = position;
    
    auto& spotlight = render_command->spotlight;
    spotlight.direction = direction;
    spotlight.cut_off = cut_off;
    spotlight.outer_cut_off = outer_cut_off;
    spotlight.ambient = ambient;
    spotlight.diffuse = diffuse;
    spotlight.specular = specular;
    spotlight.constant = constant;
    spotlight.linear = linear;
    spotlight.quadratic = quadratic;
}

static void push_directional_light(Renderer& renderer, math::Vec3 direction, math::Vec3 ambient, math::Vec3 diffuse, math::Vec3 specular)
{
    RenderCommand* render_command = push_struct(&renderer.light_commands, RenderCommand);
    renderer.light_command_count++;
    
    render_command->type = RENDER_COMMAND_DIRECTIONAL_LIGHT;
    
    auto& directional_light = render_command->directional_light;
    directional_light.direction = direction;
    directional_light.ambient = ambient;
    directional_light.diffuse = diffuse;
    directional_light.specular = specular;
}

static void push_point_light(Renderer& renderer, math::Vec3 position, math::Vec3 ambient, math::Vec3 diffuse, math::Vec3 specular, r32 constant, r32 linear, r32 quadratic)
{
    RenderCommand* render_command = push_struct(&renderer.light_commands, RenderCommand);
    renderer.light_command_count++;
    
    render_command->type = RENDER_COMMAND_POINT_LIGHT;
    
    render_command->position = position;
    
    auto& point_light = render_command->point_light;
    point_light.ambient = ambient;
    point_light.diffuse = diffuse;
    point_light.specular = specular;
    point_light.constant = constant;
    point_light.linear = linear;
    point_light.quadratic = quadratic;
}

static void push_buffer(Renderer& renderer, i32 buffer_handle, i32 texture_handle, math::Vec3 rotation = math::Vec3(), b32 is_ui = false, math::Vec3 position = math::Vec3(), math::Vec3 scale = math::Vec3(1.0f), math::Rgba color = math::Rgba(1, 1, 1, 1))
{
    RenderCommand* render_command = push_next_command(renderer, is_ui);
    
    render_command->type = RENDER_COMMAND_BUFFER;
    render_command->buffer.buffer_handle = buffer_handle;
    render_command->buffer.texture_handle = texture_handle - 1;
    render_command->rotation = rotation;
    render_command->position = position;
    render_command->scale = scale;
    render_command->is_ui = is_ui;
    render_command->color = color;
    
    render_command->shader_handle = -1;
    render_command->shader_attributes = 0;
    render_command->shader_attribute_count = 0;
}

static void push_model(Renderer& renderer, Model& model, MemoryArena* arena)
{
    RenderCommand* render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_MODEL;
    render_command->position = model.position;
    render_command->scale = model.scale;
    render_command->orientation = model.orientation;
    render_command->model.buffer_handle = model.buffer_handle;
    
    for(i32 index = 0; index < model.material_count; index++)
    {
        if(model.materials[index].diffuse_texture.has_data && model.materials[index].diffuse_texture.texture_handle == -1 && strlen(model.materials[index].diffuse_texture.texture_name) > 0)
        {
            model.materials[index].diffuse_texture.texture_handle = renderer.texture_map[model.materials[index].diffuse_texture.texture_name]->handle;
        }
    }
    
    memcpy(&render_command->model.meshes, model.meshes, sizeof(model.meshes));
    memcpy(&render_command->model.materials, model.materials, sizeof(model.materials));
    
    // @Incomplete: Check if the texture handle has been set for the materials
    render_command->model.type = model.type;
    render_command->model.mesh_count = model.mesh_count;
    render_command->model.material_count = model.material_count;
    render_command->model.bone_count = model.bone_count;
    
    if(model.type == MODEL_SKINNED)
    {
        render_command->model.bone_transforms = push_size(arena, sizeof(math::Mat4) * model.bone_count, math::Mat4);
        
        for(i32 index = 0; index < model.bone_count; index++)
        {
            render_command->model.bone_transforms[index] = model.current_poses[index];
        }
    }
    
    render_command->model.color = math::Rgba(1.0f, 1.0f, 1.0f, 1.0f);
    render_command->is_ui = false;
}

static void load_buffer(Renderer& renderer, r32* buffer, i32 buffer_size, i32* buffer_handle, b32 dynamic = false)
{
    BufferData data = {};
    data.vertex_buffer = buffer;
    data.vertex_buffer_size = buffer_size;
    data.index_buffer_count = 0;
    
    renderer.buffers[renderer.buffer_count] = data;
    
    *buffer_handle = renderer.buffer_count++;
}

static void update_buffer(Renderer& renderer, r32* buffer, i32 buffer_size, i32 buffer_handle)
{
    BufferData data = {};
    data.vertex_buffer = buffer;
    data.vertex_buffer_size = buffer_size;
    data.index_buffer_count = 0;
    data.existing_handle = buffer_handle;
    renderer.buffers[buffer_handle] = data;
    renderer.updated_buffer_handles[renderer.updated_buffer_handle_count++] = buffer_handle;
}

static i32 load_font(Renderer& renderer, char* path, i32 size, char* name)
{
    FontData data = {};
    data.path = push_string(&renderer.font_arena, path);
    data.size = size;
    data.name = push_string(&renderer.font_arena, name);
    
    renderer.fonts[renderer.font_count] = data;
    return renderer.font_count++;
}

static void load_font(Renderer& renderer, char* path, i32 size, i32* handle)
{
    FontData data = {};
    data.path = push_string(&renderer.font_arena, path);
    data.size = size;
    
    renderer.fonts[renderer.font_count] = data;
    *handle = renderer.font_count++;
}

static b32 is_eof(ChunkFormat& format)
{
    return strcmp(format.format, "EOF") == 0;
}

static void load_glim_model(Renderer& renderer, char* file_path, Model* model, MemoryArena* arena)
{
    // NOTE(Niels): Not implemented yet
    Assert(false);
    /*ModelHeader header = {};
    
    FILE *file = fopen(file_path, "rb");
    if(file)
    {
        fread(&header, sizeof(ModelHeader), 1, file);
        
        if(strcmp(header.version, "1.6") != 0)
        {
            // @Incomplete: Missing
            //err("Wrong file version. Expected version 1.6");
            return;
        }
        
        ModelData model_data;
        fread(&model_data, sizeof(ModelData), 1, file);
        fread(model->meshes, (size_t)model_data.mesh_chunk_size, 1, file);
        
        model->type = (ModelType)model_data.model_type;
        model->mesh_count = model_data.num_meshes;
        
        r32* vertex_buffer = push_temp_size(model_data.vertex_buffer_chunk_size, r32);
        fread(vertex_buffer, (size_t)model_data.vertex_buffer_chunk_size, 1, file);
        u32* index_buffer = push_temp_size(model_data.index_buffer_chunk_size, u32);
        fread(index_buffer, (size_t)model_data.index_buffer_chunk_size, 1, file);
        
        model->material_count = model_data.num_materials;
        if(model_data.num_materials > 0)
            fread(&model->materials, (size_t)model_data.material_chunk_size, 1, file);
            
        model->global_inverse_transform = model_data.global_inverse_transform;
        
        model->bone_count = model_data.num_bones;
        if(model_data.num_bones > 0)
        {
            model->bones = push_array(&renderer.animation_arena, model_data.num_bones, Bone);
            model->current_poses = push_array(&renderer.animation_arena, model_data.num_bones, math::Mat4);
            fread(model->bones, (size_t)model_data.bone_chunk_size, 1, file);
        }
        
        BufferData data = {};
        data.skinned = model->bone_count > 0;
        copy_temp(data.vertex_buffer, vertex_buffer, (size_t)model_data.vertex_buffer_chunk_size, r32);
        copy_temp(data.index_buffer, index_buffer, (size_t)model_data.index_buffer_chunk_size, u32);
        
        data.has_normals = model_data.has_normals;
        data.has_uvs = model_data.has_uvs;
        
        data.vertex_buffer_size = model_data.vertex_buffer_chunk_size;
        data.index_buffer_size = model_data.index_buffer_chunk_size;
        data.index_buffer_count = model_data.num_indices;
        
        model->buffer_handle = renderer.buffer_count++;
        
        model->animation_state.playing = false;
        model->animation_state.loop = false;
        model->animation_state.current_time = 0.0f;
        
        renderer.buffers[renderer.buffer_count - 1] = data;
        
        // Load animations
        AnimationHeader a_header;
        fread(&a_header, sizeof(AnimationHeader), 1, file);
        
        model->animation_count = a_header.num_animations;
        model->animations = a_header.num_animations > 0 ? push_array(&renderer.animation_arena, a_header.num_animations, SkeletalAnimation) : 0;
        
        for(i32 index = 0; index < model->animation_count; index++)
        {
            AnimationChannelHeader ac_header;
            fread(&ac_header, sizeof(AnimationChannelHeader), 1, file);
            
            SkeletalAnimation* animation = &model->animations[index];
            animation->duration = ac_header.duration;
            animation->num_bone_channels = ac_header.num_bone_channels;
            
            animation->bone_channels = push_array(&renderer.animation_arena, animation->num_bone_channels, BoneChannel);
            
            for(i32 bone_channel_index = 0; bone_channel_index < animation->num_bone_channels; bone_channel_index++)
            {
                BoneAnimationHeader ba_header;
                fread(&ba_header, sizeof(BoneAnimationHeader), 1, file);
                
                BoneChannel* bone_channel = &animation->bone_channels[bone_channel_index];
                bone_channel->bone_index = ba_header.bone_index;
                
                bone_channel->position_keys.num_keys = ba_header.num_position_channels;
                bone_channel->position_keys.time_stamps = push_array(&renderer.animation_arena, bone_channel->position_keys.num_keys, r32);
                bone_channel->position_keys.values = push_array(&renderer.animation_arena, bone_channel->position_keys.num_keys, math::Vec3);
                fread(bone_channel->position_keys.time_stamps, sizeof(r32) * bone_channel->position_keys.num_keys, 1, file);
                fread(bone_channel->position_keys.values, sizeof(math::Vec3) * bone_channel->position_keys.num_keys, 1, file);
                
                bone_channel->rotation_keys.num_keys = ba_header.num_rotation_channels;
                bone_channel->rotation_keys.time_stamps = push_array(&renderer.animation_arena, bone_channel->rotation_keys.num_keys, r32);
                bone_channel->rotation_keys.values = push_array(&renderer.animation_arena, bone_channel->rotation_keys.num_keys, math::Quat);
                fread(bone_channel->rotation_keys.time_stamps, sizeof(r32) * bone_channel->rotation_keys.num_keys, 1, file);
                fread(bone_channel->rotation_keys.values, sizeof(math::Quat) * bone_channel->rotation_keys.num_keys, 1, file);
                
                bone_channel->scaling_keys.num_keys = ba_header.num_scaling_channels;
                bone_channel->scaling_keys.time_stamps = push_array(&renderer.animation_arena, bone_channel->scaling_keys.num_keys, r32);
                bone_channel->scaling_keys.values = push_array(&renderer.animation_arena, bone_channel->scaling_keys.num_keys, math::Vec3);
                fread(bone_channel->scaling_keys.time_stamps, sizeof(r32) * bone_channel->scaling_keys.num_keys, 1, file);
                fread(bone_channel->scaling_keys.values, sizeof(math::Vec3) * bone_channel->scaling_keys.num_keys, 1, file);
            }
        }
        
        fclose(file);
    }
    else
    {
        printf("Model file not found: %s", file_path);
    }*/
}

static void add_particle_system(Renderer& renderer, math::Vec3 position, i32 texture_handle, r32 rate, r32 speed, i32* handle)
{
}
static void update_particle_system_position(Renderer& renderer, i32 handle, math::Vec2 new_position)
{
}

static void remove_particle_system(Renderer& renderer, i32 handle)
{
}