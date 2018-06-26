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
        camera.projection_matrix = math::perspective((r32)renderer.viewport[2] / (r32)renderer.viewport[3], 0.60f, 0.2f, 100.0f);
        
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
    assert(renderer.spritesheet_animation_count < MAX_SPRITESHEET_ANIMATIONS);
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
        TextureData data = renderer.texture_data[texture_handle - 1];
        return math::Vec2i(data.width, data.height);
    }
    return math::Vec2i();
}

static void load_texture(const char* full_texture_path, Renderer& renderer, TextureFiltering filtering, i32* handle = 0)
{
    TextureData* texture_data = &renderer.texture_data[renderer.texture_count];
    texture_data->filtering = filtering;
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
    TextureData_map_init(&renderer.texture_map, hash_string_jenkins, 64);
    
    DirectoryData dir_data = {};
    platform.get_all_files_with_extension(arena, path, "png", &dir_data, true);
    
    for (i32 file_index = 0; file_index < dir_data.files_length; file_index++)
    {
        load_texture(dir_data.file_paths[file_index], renderer, LINEAR);
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
#define PUSH_CENTERED_TEXT(text, position, color, font_handle) push_text(renderer, text, position, 1.0f, font_handle, color, ALIGNMENT_CENTER_X | ALIGNMENT_CENTER_Y)
static void push_text(Renderer& renderer, const char* text, math::Vec3 position, r32 scale, i32 font_handle, math::Rgba color, u64 alignment_flags = ALIGNMENT_LEFT, b32 is_ui = true)
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
    render_command->text.alignment_flags = alignment_flags;
    render_command->is_ui = is_ui;
}

static void push_text(Renderer& renderer, TextInfo text_info)
{
    push_text(renderer, text_info.text, text_info.position, text_info.scale, text_info.font_handle, text_info.render_info.color, text_info.alignment_flags, text_info.render_info.is_ui);
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

static void create_buffers_from_mesh(Renderer &renderer, Mesh &mesh, u64 vertex_data_flags, b32 has_normals, b32 has_uvs)
{
    i32 vertex_size = 3;
    
    BufferData data = {};
    
    if(has_normals)
    {
        data.has_normals = true;
        vertex_size += 3;
    }
    
    if(has_uvs)
    {
        data.has_uvs = true;
        vertex_size += 2;
    }
    
    data.vertex_buffer_size = mesh.vertex_count * vertex_size * (i32)sizeof(r32);
    data.vertex_buffer = push_size(&renderer.mesh_arena, data.vertex_buffer_size, r32);
    generate_vertex_buffer(data.vertex_buffer, mesh.vertices, mesh.vertex_count, vertex_size, has_normals, has_uvs);
    
    i32 index_count = mesh.face_count * 3;
    data.index_buffer_size = index_count * (i32)sizeof(u16);
    data.index_buffer_count = index_count;
    data.index_buffer = push_size(&renderer.mesh_arena, data.index_buffer_size, u16);
    generate_index_buffer(data.index_buffer, mesh.faces, mesh.face_count);
    
    renderer.buffers[renderer.buffer_count] = data;
    mesh.buffer_handle = renderer.buffer_count++;
}

static math::Vec3 compute_face_normal(Face f, Vertex *vertices)
{
    // Newell's Method
    // https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal
    math::Vec3 normal = math::Vec3(0.0f);
    math::Vec3 u = (vertices[f.indices[1]].position - vertices[f.indices[0]].position);
    math::Vec3 v = (vertices[f.indices[2]].position - vertices[f.indices[0]].position);
    
    return math::cross(u, v);
}

static void create_tetrahedron(Renderer &renderer, i32 *mesh_handle)
{
    Mesh &mesh = renderer.meshes[renderer.mesh_count++];
    mesh = {};
    mesh.vertices = push_array(&renderer.mesh_arena, sizeof(tetrahedron_vertices) / sizeof(r32) / 3, Vertex);
    mesh.faces = push_array(&renderer.mesh_arena, sizeof(tetrahedron_indices) / sizeof(u16) / 3, Face);
    
    mesh.vertex_count = sizeof(tetrahedron_vertices) / sizeof(r32) / 3;
    
    
    for(i32 i = 0; i < mesh.vertex_count; i++)
    {
        Vertex &vertex = mesh.vertices[i];
        vertex.position = math::Vec3(tetrahedron_vertices[i * 3], tetrahedron_vertices[i * 3 + 1], tetrahedron_vertices[i * 3 + 2]);
        vertex.normal = math::Vec3(tetrahedron_normals[i * 3], tetrahedron_normals[i * 3 + 1], tetrahedron_normals[i * 3 + 2]);
    }
    
    mesh.face_count = sizeof(tetrahedron_indices) / sizeof(u16) / 3;
    
    for(i32 i = 0; i < mesh.face_count; i++)
    {
        Face &face = mesh.faces[i];
        
        auto normal = compute_face_normal(face, mesh.vertices);
        
        face.indices[0] = tetrahedron_indices[i * 3];
        face.indices[1] = tetrahedron_indices[i * 3 + 1];
        face.indices[2] = tetrahedron_indices[i * 3 + 2];
    }
    
    *mesh_handle = renderer.mesh_count - 1;
    
    create_buffers_from_mesh(renderer, mesh, 0, true, false);
}

static void create_cube(Renderer &renderer, i32 *mesh_handle, b32 with_instancing = false, i32 *instance_buffer_handle = 0)
{
    Mesh &mesh = renderer.meshes[renderer.mesh_count++];
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
    
    *mesh_handle = renderer.mesh_count - 1;
    
    create_buffers_from_mesh(renderer, mesh, 0, true, true);
    
    if(with_instancing)
    {
        BufferData data = {};
        data.instance_buffer_size = sizeof(math::Vec3) * 900;
        data.for_instancing = true;
        renderer.buffers[renderer.buffer_count] = data;
        *instance_buffer_handle = renderer.buffer_count++;
    }
}

static void create_plane(Renderer &renderer, i32 *mesh_handle)
{
    Mesh &mesh = renderer.meshes[renderer.mesh_count++];
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
    
    *mesh_handle = renderer.mesh_count - 1;
    
    create_buffers_from_mesh(renderer, mesh, 0, true, true);
}

static void push_mesh(Renderer &renderer, MeshInfo mesh_info)
{
    RenderCommand *render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_MESH;
    render_command->position = mesh_info.transform.position;
    render_command->scale = mesh_info.transform.scale;
    render_command->rotation = mesh_info.transform.rotation;
    render_command->mesh.wireframe_type = mesh_info.wireframe_type;
    render_command->mesh.wireframe_color = mesh_info.wireframe_color;
    Mesh &mesh = renderer.meshes[mesh_info.mesh_handle];
    render_command->mesh.buffer_handle = mesh.buffer_handle;
    render_command->mesh.material_type = mesh_info.material.type;
    render_command->mesh.diffuse_texture = mesh_info.material.diffuse_texture;
    render_command->color = mesh_info.material.color;
    render_command->cast_shadows = mesh_info.cast_shadows;
}

static void push_mesh_instanced(Renderer &renderer, MeshInfo mesh_info, math::Vec3 *offsets, i32 offset_count)
{
    RenderCommand *render_command = push_next_command(renderer, false);
    render_command->type = RENDER_COMMAND_MESH_INSTANCED;
    render_command->position = mesh_info.transform.position;
    render_command->scale = mesh_info.transform.scale;
    render_command->rotation = mesh_info.transform.rotation;
    render_command->mesh_instanced.wireframe_type = mesh_info.wireframe_type;
    render_command->mesh_instanced.wireframe_color = mesh_info.wireframe_color;
    Mesh &mesh = renderer.meshes[mesh_info.mesh_handle];
    render_command->mesh_instanced.buffer_handle = mesh.buffer_handle;
    render_command->mesh_instanced.material_type = mesh_info.material.type;
    render_command->mesh_instanced.diffuse_texture = mesh_info.material.diffuse_texture;
    render_command->color = mesh_info.material.color;
    render_command->mesh_instanced.instance_buffer_handle = mesh_info.instance_buffer_handle;
    render_command->mesh_instanced.offsets = offsets;
    render_command->mesh_instanced.offset_count = offset_count;
    render_command->cast_shadows = mesh_info.cast_shadows;
}

static void push_particle_system(Renderer &renderer, i32 particle_system_handle)
{
    ParticleSystemInfo &particle_info = renderer.particle_systems[particle_system_handle];
    RenderCommand *render_command = push_next_command(renderer, false);
    
    render_command->type = RENDER_COMMAND_PARTICLES;
    render_command->position = particle_info.transform.position;
    //render_command->rotation = mesh_info.transform.rotation;
    
    // @Incomplete
    //render_command->particles.buffer_handle = particle_info.buffer_handle;
    //render_command->particles.material.diffuse_texture = 0; // @Incomplete
    render_command->particles.offset_buffer_handle = particle_info.offset_buffer_handle;
    render_command->particles.color_buffer_handle = particle_info.color_buffer_handle;
    render_command->particles.particle_count = particle_info.particle_count;
    render_command->particles.offsets = particle_info.offsets;
    render_command->particles.colors = particle_info.colors;
}

/*
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
*/

static void load_buffer(Renderer& renderer, r32* buffer, i32 buffer_size, i32* buffer_handle, b32 dynamic = false)
{
    BufferData data = {};
    data.has_normals = false;
    data.has_uvs = false;
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

static void create_particle_system(Renderer &renderer, i32 *handle, r64 life_time, i32 particles_per_second, i32 max_particles, MemoryArena *memory_arena)
{
    ParticleSystemInfo &system_info = renderer.particle_systems[renderer.particle_system_count++];
    
    system_info.life_time = life_time;
    system_info.particles_per_second = 1000; //particles_per_second;
    system_info.max_particles = max_particles;
    system_info.particle_count = 0;
    system_info.last_used_particle = 0;
    system_info.particles = push_array(memory_arena, system_info.max_particles, Particle);
    system_info.offsets = push_array(memory_arena, system_info.max_particles, math::Vec3);
    system_info.colors = push_array(memory_arena, system_info.max_particles, math::Vec4);
    
    BufferData offset_data = {};
    offset_data.for_instancing = true;
    offset_data.instance_buffer_size = sizeof(math::Vec3) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = offset_data;
    system_info.offset_buffer_handle = renderer.buffer_count++;
    
    BufferData color_data = {};
    color_data.for_instancing = true;
    color_data.instance_buffer_size = sizeof(math::Vec4) * system_info.max_particles;
    renderer.buffers[renderer.buffer_count] = color_data;
    system_info.color_buffer_handle = renderer.buffer_count++;
    *handle = renderer.particle_system_count - 1;
}

static void load_obj(Renderer &renderer, char *file_path, i32 *mesh_handle, b32 with_instancing = false, i32 *instance_buffer_handle = 0)
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
    
    if(file)
    {
        char buffer[256];
        
        while((fgets(buffer, sizeof(buffer), file) != NULL))
        {
            if(starts_with(buffer, "g")) // we're starting with new geometry
            {
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
    }
    
    Mesh &mesh = renderer.meshes[renderer.mesh_count++];
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
    
    *mesh_handle = renderer.mesh_count - 1;
    create_buffers_from_mesh(renderer, mesh, 0, true, true);
    
    if(with_instancing)
    {
        BufferData data = {};
        data.instance_buffer_size = sizeof(math::Vec3) * 900;
        data.for_instancing = true;
        renderer.buffers[renderer.buffer_count] = data;
        *instance_buffer_handle = renderer.buffer_count++;
    }
}