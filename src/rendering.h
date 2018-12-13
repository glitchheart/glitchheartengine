#ifndef RENDERING_H
#define RENDERING_H

#define MAX_BONES 50
#define MAX_CHILDREN 30
#define MAX_INSTANCING_PAIRS 128

#define UI_COORD_DIMENSION 1000.0f


#define STB_TRUETYPE_IMPLEMENTATION
#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable : 4365) // int conversions
#pragma warning(disable : 4459)
#endif
#include "stb/stb_truetype.h"

#ifdef _WIN32
#pragma warning(pop)
#endif

struct TrueTypeFontInfo
{
	i32 ascent;
	i32 descent;
	i32 line_gap;
	r32 line_height;
	r32 scale;
	i32 baseline;
	i32 first_char;
	i32 char_count;
	i32 size;
	i32 atlas_width;
	i32 atlas_height;
	u32 oversample_x;
	u32 oversample_y;
	r32 largest_character_height;
	stbtt_fontinfo info;
	stbtt_packedchar char_data['~' - ' '];
};

struct FontData
{
	char* path;
	i32 size;
	char* name;
};

struct DirectionalLight
{
    math::Vec3 direction;
    math::Vec3 ambient;
    math::Vec3 diffuse;
    math::Vec3 specular;
};

struct PointLight
{
    math::Vec3 position;
    r32 constant;
    r32 linear;
    r32 quadratic;
    math::Vec3 ambient;
    math::Vec3 diffuse;
    math::Vec3 specular;
};

// @Incomplete: Remove the prefix from the values 
enum ShaderType
{
    SHADER_MESH,
    SHADER_MESH_INSTANCED,
    SHADER_DEPTH,
    SHADER_DEPTH_INSTANCED,
    SHADER_QUAD,
    SHADER_TEXTURE_QUAD,
    SHADER_STANDARD_FONT,
    SHADER_3D_TEXT,
    SHADER_SPRITESHEET,
    SHADER_FRAME_BUFFER,
    SHADER_SIMPLE_MODEL,
    SHADER_LINE,
    SHADER_PARTICLES,
    SHADER_ROUNDED_QUAD,
    SHADER_COUNT
};

// @Incomplete: Remove the prefix from the values 
enum RenderCommandType
{
    RENDER_COMMAND_LINE,
    RENDER_COMMAND_TEXT,
    RENDER_COMMAND_3D_TEXT,
    RENDER_COMMAND_QUAD,
    
    RENDER_COMMAND_SPOTLIGHT,
    RENDER_COMMAND_DIRECTIONAL_LIGHT,
    RENDER_COMMAND_POINT_LIGHT,
    
    RENDER_COMMAND_BUFFER,
    RENDER_COMMAND_MODEL,
    RENDER_COMMAND_MESH,
    RENDER_COMMAND_MESH_INSTANCED,
    RENDER_COMMAND_SHADER_START,
    RENDER_COMMAND_SHADER_END,
    RENDER_COMMAND_DEPTH_TEST,
    RENDER_COMMAND_PARTICLES,
    RENDER_COMMAND_CURSOR,
    RENDER_COMMAND_SUN_LIGHT,
    
    RENDER_COMMAND_COUNT
};

// @Incomplete: Remove the prefix from the values 
enum RelativeFlag
{
    RELATIVE_TOP,
    RELATIVE_LEFT,
    RELATIVE_RIGHT,
    RELATIVE_BOTTOM
};

// @Incomplete: Remove the prefix from the values 
enum Alignment
{
    ALIGNMENT_LEFT = (1 << 0),
    ALIGNMENT_RIGHT = (1 << 1),
    ALIGNMENT_CENTER_X = (1 << 2),
    ALIGNMENT_CENTER_Y = (1 << 3),
    ALIGNMENT_TOP = (1 << 4),
    ALIGNMENT_BOTTOM = (1 << 5)
};

struct VertexInfo
{
	math::Vec3 position;
	math::Vec2 uv;
	math::Vec3 normal;
	math::Rgba color;
};

struct RelativeUIQuadInfo
{
	math::Vec2 position;
	math::Vec2 scale;
	math::Vec2 ui_position;
};

r32 plane_vertices[] =
{
    -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, -0.5f,
    0.5f, 0.0f,  0.5f,
    0.5f, 0.0f,  0.5f,  
    -0.5f, 0.0f,  0.5f, 
    -0.5f, 0.0f, -0.5f
};

u16 plane_indices[] = 
{
    0, 1, 2,
    3, 4, 5
};

r32 plane_normals[] =
{
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f,
    0.0f, 1.0f,  0.0f
};

r32 plane_uvs[] =
{
    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f,
    1.0f, 0.0f, 
    0.0f, 0.0f, 
    0.0f, 1.0f
};

r32 cube_normals[] =
{
    0.0f,  0.0f, -1.0f,
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, -1.0f, 
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    0.0f,  0.0f, 1.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    -1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f, -1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f
};

r32 cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  
    0.5f, -0.5f, -0.5f,  
    0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f, -0.5f, 
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    
    -0.5f, -0.5f,  0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,  
    0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f,  0.5f, 
    
    -0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    
    0.5f,  0.5f,  0.5f, 
    0.5f,  0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,
    0.5f,  0.5f,  0.5f,
    
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, -0.5f,  0.5f,  
    0.5f, -0.5f,  0.5f,  
    -0.5f, -0.5f,  0.5f, 
    -0.5f, -0.5f, -0.5f, 
    
    -0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f, -0.5f, 
    0.5f,  0.5f,  0.5f, 
    0.5f,  0.5f,  0.5f,  
    -0.5f,  0.5f,  0.5f, 
    -0.5f,  0.5f, -0.5f,  
};

r32 cube_uvs[] =
{
    0.000059f, 1.0f - 0.000004f,
    0.000103f, 1.0f - 0.336048f,
    0.335973f, 1.0f - 0.335903f,
    1.000023f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    0.999958f, 1.0f - 0.336064f,
    
    0.667979f, 1.0f - 0.335851f,
    0.336024f, 1.0f - 0.671877f,
    0.667969f, 1.0f - 0.671889f,
    1.000023f, 1.0f - 0.000013f,
    0.668104f, 1.0f - 0.000013f,
    0.667979f, 1.0f - 0.335851f,
    
    0.000059f, 1.0f - 0.000004f,
    0.335973f, 1.0f - 0.335903f,
    0.336098f, 1.0f - 0.000071f,
    0.667979f, 1.0f - 0.335851f,
    0.335973f, 1.0f - 0.335903f,
    0.336024f, 1.0f - 0.671877f,
    
    1.000004f, 1.0f - 0.671847f,
    0.999958f, 1.0f - 0.336064f,
    0.667979f, 1.0f - 0.335851f,
    0.668104f, 1.0f - 0.000013f,
    0.335973f, 1.0f - 0.335903f,
    0.667979f, 1.0f - 0.335851f,
    
    0.335973f, 1.0f - 0.335903f,
    0.668104f, 1.0f - 0.000013f,
    0.336098f, 1.0f - 0.000071f,
    0.000103f, 1.0f - 0.336048f,
    0.000004f, 1.0f - 0.671870f,
    0.336024f, 1.0f - 0.671877f,
    
    0.000103f, 1.0f - 0.336048f,
    0.336024f, 1.0f - 0.671877f,
    0.335973f, 1.0f - 0.335903f,
    0.667969f, 1.0f - 0.671889f,
    1.000004f, 1.0f - 0.671847f,
    0.667979f, 1.0f - 0.335851f
};

u16 cube_indices[] = 
{
    0, 1, 2,
    3, 4, 5,
    6, 7, 8,
    9, 10, 11,
    12, 13, 14,
    15, 16, 17,
    
    18, 19, 20,
    21, 22, 23,
    24, 25, 26,
    27, 28, 29,
    30, 31, 32,
    33, 34, 35
};

r32 tetrahedron_vertices[] =
{
    // Bottom
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f,
    
    //Sides
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,
    
    -0.5f, 1.0f, -0.5f,
    0.0f, 0.0f,  0.0f,
    -0.5f, 0.0f, -1.0f,
    
    -0.5f, 1.0f, -0.5f,
    -1.0f, 0.0f, 0.0f,
    -0.5f, 0.0f, -1.0f
};

u16 tetrahedron_indices[] =
{
    0, 1, 2, 
    3, 4, 5,
    6, 7, 8,
    9, 10, 11
};

r32 tetrahedron_normals[] =
{
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f
};

struct Vertex
{
	math::Vec3 position;
	math::Vec2 uv;
	math::Vec3 normal;
	math::Rgba color;
};

struct Face
{
	u16 indices[3];
	math::Vec3 normal;
};

enum RenderMaterialType
{
    RM_INVALID,
    RM_COLOR,
    RM_TEXTURED
};

struct Mesh
{
	i32 buffer_handle;
    
	Vertex* vertices;
	i32 vertex_count;
    
	Face* faces;
	i32 face_count;
    
	i32 instance_offset_buffer_handle;
	i32 instance_color_buffer_handle;
	i32 instance_rotation_buffer_handle;
	i32 instance_scale_buffer_handle;
};

struct InstancedRenderCommand
{
	i32 mesh_handle;
	i32 material_handle;
	i32 original_material_handle;
	i32 count;
	math::Vec3 scale = math::Vec3(1, 1, 1);

	b32 has_particles;
	i32 particle_systems[256];
	u32 particle_count;
    
	b32 receives_shadows;
	b32 cast_shadows;

    InstancedRenderCommand () {}
};

struct BatchedCommand
{
    rendering::MaterialInstanceHandle material_handle;
    rendering::Transform transform;
    b32 casts_shadows;

    rendering::RenderPassHandle passes[8];
    rendering::ShaderHandle shader_handles[8];
    i32 pass_count;
};

struct NormalRenderCommand
{
	rendering::BufferHandle buffer_handle;
    rendering::MaterialHandle original_material;

    BatchedCommand commands[128];
    i32 count;
};

enum QueuedRenderCommandType
{
    NORMAL,
    INSTANCED
};

struct QueuedRenderCommand
{
	QueuedRenderCommandType type;
	union
	{
		NormalRenderCommand normal;
	};

    QueuedRenderCommand () {}
};

struct TextureInfo
{
	b32 has_data;
	char texture_name[50];
	i32 texture_handle;
};

struct Vec3Keys
{
	i32 num_keys;
	r32* time_stamps;
	math::Vec3* values;
};

struct QuatKeys
{
	i32 num_keys;
	r32* time_stamps;
	math::Quat* values;
};

struct BoneChannel
{
	i32 bone_index;
	Vec3Keys position_keys;
	QuatKeys rotation_keys;
	Vec3Keys scaling_keys;
};

struct SkeletalAnimation
{
	char* name;
	r32 duration;
	i32 num_bone_channels;
	BoneChannel* bone_channels;
};

struct SkeletalAnimationState
{
	b32 playing;
	b32 loop;
	r32 current_time;
};

struct Bone
{
	char name[30];
	i32 parent_id;
	u32 children[MAX_CHILDREN];
	i32 child_count;
    
	math::Mat4 transformation;
	math::Mat4 bone_offset;
};

struct MeshData
{
	i32 base_vertex;
	i32 base_index;
	i32 material_index;
	i32 num_indices;
};

enum ModelType
{
    MODEL_STATIC,
    MODEL_SKINNED
};

struct TransformInfo
{ 
	math::Vec3 position;
	math::Vec3 scale;
	math::Vec3 rotation;
};

enum ShaderAttributeType
{
    ATTRIBUTE_FLOAT,
    ATTRIBUTE_FLOAT2,
    ATTRIBUTE_FLOAT3,
    ATTRIBUTE_FLOAT4,
    ATTRIBUTE_INTEGER,
    ATTRIBUTE_BOOLEAN,
    ATTRIBUTE_MATRIX4
};

struct ShaderAttribute
{
	ShaderAttributeType type;
	char name[32];
	union
	{
		r32 float_var;
		math::Vec2 float2_var;
		math::Vec3 float3_var;
		math::Vec4 float4_var;
		i32 integer_var;
		b32 boolean_var;
		math::Mat4 matrix4_var;
	};
};

struct ShaderInfo
{
	i32 shader_handle;
	ShaderAttribute* shader_attributes;
	i32 shader_attribute_count;
};

struct MaterialHandle
{
	i32 handle;
};

struct TextureHandle
{
	i32 handle;
};

struct MeshHandle
{
	i32 handle;
};

struct Material
{
	RenderMaterialType type;
    
	MaterialHandle source_handle;
	ShaderInfo shader;

	math::Rgba ambient_color;
	math::Rgba diffuse_color;
	math::Rgba specular_color;
	r32 dissolve;
	r32 specular_exponent;
    
	TextureHandle ambient_texture;
	TextureHandle diffuse_texture;
	TextureHandle specular_texture;
	TextureHandle specular_intensity_texture;
};

enum WireframeType
{
    WT_NONE,
    WT_WITH_MESH,
    WT_WITHOUT_MESH
};

struct MeshInfo
{
	i32 mesh_handle;
	i32 instance_offset_buffer_handle;
	i32 instance_color_buffer_handle;
	i32 instance_rotation_buffer_handle;
	i32 instance_scale_buffer_handle;
    
	TransformInfo transform;
	Material material;
    
	WireframeType wireframe_type;
	math::Rgba wireframe_color;
    
	b32 cast_shadows;
	b32 receives_shadows;
};

struct RenderInfo
{
	b32 is_ui;
    
	b32 with_origin;
	math::Vec2 origin;
	math::Rgba color;
};

struct QuadTextureInfo
{
	i32 texture_handle;
	math::Vec2i frame_size;
	math::Vec2 texture_offset;
};

struct QuadInfo
{
	TransformInfo transform_info;
	ShaderInfo shader_info;
	RenderInfo render_info;
	QuadTextureInfo texture_info;
    
	r32 border_width;
	math::Rgba border_color;
	b32 rounded;
	b32 flipped;
	i32 animation_controller_handle;
	b32 clip;
	math::Rect clip_rect;
};

struct TextInfo
{
	math::Vec3 position;
	RenderInfo render_info;
    
	i32 font_handle;
	u64 alignment_flags;
	char* text;
	r32 scale;
};

enum CommandBlendMode
{
    CBM_ONE,
    CBM_ONE_MINUS_SRC_ALPHA
};

enum CursorType
{
    CURSOR_ARROW,
    CURSOR_CROSSHAIR,
    CURSOR_HAND,
    CURSOR_HRESIZE,
    CURSOR_IBEAM,
    CURSOR_VRESIZE
};

struct RenderCommand
{
	RenderCommandType type;
	ShaderType shader_type;
	b32 is_ui;
    
	math::Vec3 position;
	math::Vec3 scale;
	math::Vec3 rotation;
	math::Quat orientation;
	b32 with_origin;
	math::Vec2 origin;
	math::Rgba color;
    
	i32 shader_handle;
	ShaderAttribute* shader_attributes;
	i32 shader_attribute_count;
    
	b32 cast_shadows;
	b32 receives_shadows;
    
	b32 clip;
	math::Rect clip_rect;
    
	union
	{
		struct
		{
			math::Vec3 point1;
			math::Vec3 point2;
			r32 line_width;
			math::Rgba color; // @Cleanup: REMOVE!
		} line;
		struct
		{
			char text[512];
			math::Vec3 position;
			i32 font_handle;
			math::Rgba color; // @Cleanup: REMOVE!
			u64 alignment_flags;
			r32 scale;
			i32 z_layer;
		} text;
		struct
		{
			char text[512];
			i32 font_handle;
			u64 alignment_flags;
			math::Rgba color; // @Cleanup: REMOVE!
		} text_3d;
		struct
		{
			math::Rgba color;
		} sprite;
		struct
		{
			i32 texture_handle;
			b32 flipped;
			b32 rounded;
			math::Rgba color; // @Cleanup: REMOVE!
			b32 outlined;
			r32 line_width;
			b32 for_animation;
			math::Vec2 texture_size;
			math::Vec2i frame_size;
			math::Vec2 texture_offset;
			r32 border_width;
			math::Rgba border_color;
		} quad;
		struct
		{
			math::Vec3 direction;
			r32 cut_off;
			r32 outer_cut_off;
			math::Vec3 ambient;
			math::Vec3 diffuse;
			math::Vec3 specular;
            
			r32 constant;
			r32 linear;
			r32 quadratic;
		} spotlight;
		struct 
		{
			math::Vec3 direction;
			math::Vec3 ambient;
			math::Vec3 diffuse;
			math::Vec3 specular;
		} directional_light;
		struct
		{
			math::Vec3 ambient;
			math::Vec3 diffuse;
			math::Vec3 specular;
            
			r32 constant;
			r32 linear;
			r32 quadratic;
		} point_light;
		struct
		{
			i32 buffer_handle;
			i32 texture_handle;
		} buffer;
		struct
		{
			ModelType type;
			i32 buffer_handle;
			MeshData *meshes;
			i32 mesh_count;
			Material materials[10];
			i32 material_count;
			math::Rgba color;
			math::Mat4* bone_transforms;
			i32 bone_count;
		} model;
		struct
		{
			i32 buffer_handle;
			RenderMaterialType material_type;
			i32 diffuse_texture;
			i32 specular_texture;
			i32 ambient_texture;
			i32 specular_intensity_texture;
			math::Rgba diffuse_color;
			math::Rgba specular_color;
			math::Rgba ambient_color;
			r32 specular_exponent;
			WireframeType wireframe_type;
			math::Rgba wireframe_color;
		} mesh;
		struct
		{
			i32 buffer_handle;
			i32 instance_offset_buffer_handle;
			i32 instance_color_buffer_handle;
			i32 instance_rotation_buffer_handle;
			i32 instance_scale_buffer_handle;
            
			RenderMaterialType material_type;
			i32 diffuse_texture;
			i32 specular_texture;
			i32 ambient_texture;
			i32 specular_intensity_texture;
			math::Rgba diffuse_color;
			math::Rgba specular_color;
			math::Rgba ambient_color;
			r32 dissolve;
			r32 specular_exponent;
			WireframeType wireframe_type;
			math::Rgba wireframe_color;
			math::Vec3 *offsets;
			math::Rgba *colors;
			math::Vec3 *rotations;
			math::Vec3 *scalings;
			i32 offset_count;
		} mesh_instanced;
		struct
		{
			b32 on;
		} depth_test;
		struct
		{
			CursorType type;
		} cursor;
		struct
		{
			i32 handle;
			i32 buffer_handle;
			i32 offset_buffer_handle;
			i32 color_buffer_handle;
			i32 size_buffer_handle;
			i32 angle_buffer_handle;
			RenderMaterialType material_type;
			i32 diffuse_texture;
            
			i32 particle_count;
			math::Vec3 *offsets;
			math::Vec4 *colors;
			math::Vec2 *sizes;
			r32* angles;
			i32 texture_handle;
            
			CommandBlendMode blend_mode;
		} particles;
		struct
		{
			math::Rgba specular_color;
			math::Rgba diffuse_color;
			math::Rgba ambient_color;
			math::Vec3 position;
		} sun_light;
	};
};

enum FadingMode
{
    FADING_NONE,
    FADING_IN,
    FADING_OUT,
    FADING_OUT_IN
};

enum UIScalingFlag
{
    KEEP_ASPECT_RATIO = (1 << 0),
    NO_SCALING = (1 << 1),
    SCALE_WITH_WIDTH = (1 << 2),
    SCALE_WITH_HEIGHT = (1 << 3),
};

enum TextureFiltering
{
    LINEAR,
    NEAREST
};

struct TextureData
{
	TextureFiltering filtering;
	i32 handle;
	char* name;
	i32 width;
	i32 height;
	unsigned char* image_data;
};

struct ShaderData
{
	i32 handle;
	char name[512];
	char* vertex_shader_content;
	char* fragment_shader_content;
};

struct UIRenderInfo
{
	b32 rendered = true;
    
	i32 texture_handle;
	math::Vec2 texture_offset;
	math::Vec2 frame_size;
	u32 shader_index;
	math::Vec2 size = math::Vec2(1, 1);
	math::Vec4 color = math::Vec4(1, 1, 1, 1);
};

struct BufferData
{
	r32* vertex_buffer;
	i32 vertex_buffer_size;
	u16* index_buffer;
	i32 index_buffer_count;
	i32 index_buffer_size;
	b32 has_normals;
	b32 has_uvs;
	b32 skinned;
    
	ShaderType shader_type;
	i32 existing_handle = -1;
    
	b32 for_instancing;
	size_t instance_buffer_size;
};

struct ShadowMapMatrices
{
	math::Mat4 depth_projection_matrix;
	math::Mat4 depth_model_matrix;
	math::Mat4 depth_view_matrix;
	math::Mat4 depth_bias_matrix;
};

struct Resolution
{
	i32 width;
	i32 height;
};

struct ParticleSystemInfo;

struct RenderState;

typedef void (*LoadTexture)(TextureData &texture_data, RenderState *render_state, Renderer *renderer);
typedef void (*CreateFramebuffer)(rendering::FramebufferInfo &framebuffer_info, RenderState *render_state, Renderer *renderer);

struct GraphicsAPI
{
    LoadTexture load_texture;
    CreateFramebuffer create_framebuffer;
    RenderState *render_state;
};

struct Renderer
{
    GraphicsAPI api_functions;
    
	b32 should_close;
	r64 fps;
	r64 average_fps;
	r64 fps_sum;
	u64 current_frame;
	i32 pixels_per_unit;
	i32 frame_lock;
    
	WindowMode window_mode;

    Camera camera;
	
	RenderCommand *commands;
	i32 command_count;
    
	RenderCommand *ui_commands;
	i32 ui_command_count;
    
	MemoryArena command_arena;
    
	BufferData *buffers;
	i32 buffer_count;
    
	i32 *_internal_buffer_handles;
	i32 _current_internal_buffer_handle;
    
	i32 *updated_buffer_handles;
	i32 updated_buffer_handle_count;
    
	i32 *removed_buffer_handles;
	i32 removed_buffer_handle_count;
    
	Material *materials;
	i32 material_count;
    
	Mesh *meshes;
	i32 mesh_count;
    
	struct
	{
		i32 *_internal_handles;
		i32 *_tagged_removed;
		i32 _tagged_removed_count;
		i32 _current_internal_handle;
		i32 _max_particle_system_count;
        
		ParticleSystemInfo *particle_systems;
		i32 particle_system_count;
        
		RandomSeries entropy;
	} particles;
    
	TextureData *texture_data;
	i32 texture_count;
    
	ShaderData* shader_data;
	i32 shader_count;
    
	// Shadow map
	ShadowMapMatrices shadow_map_matrices;
    
	math::Mat4 ui_projection_matrix;
	i32 current_camera_handle;
    
	AnimationController* animation_controllers;
	i32 animation_controller_count;
    
	SpritesheetAnimation *spritesheet_animations;
	i32 spritesheet_animation_count;
    
	union
	{
		i32 viewport[4];
		math::Vec4i v;
		struct
		{
			i32 viewport_x;
			i32 viewport_y;
			i32 viewport_width;
			i32 viewport_height;
		};
	};
    
	union
	{
		Resolution resolution;
		struct
		{
			i32 window_width;
			i32 window_height;
		};
	};
    
	Resolution *available_resolutions;
	i32 available_resolutions_count;
	i32 current_resolution_index;
    
	Resolution ui_reference_resolution;
    
	r32 scale_x;
	r32 scale_y;
    
	math::Rgba clear_color;
	r32 line_width;
    
	b32 show_mouse_cursor;
    
	FontData *fonts;
	i32 font_count;
    
	TrueTypeFontInfo *tt_font_infos;
	i32 tt_font_count;
    
	i32 framebuffer_width;
	i32 framebuffer_height;
    
	MemoryArena mesh_arena;
	MemoryArena texture_arena;
	MemoryArena animation_arena;
	MemoryArena font_arena;
	MemoryArena particle_arena;
	MemoryArena buffer_arena;
	MemoryArena shader_arena;
    
	MemoryArena temp_arena;

	struct
	{
		rendering::Shader *shaders;
		i32 shader_count;

        rendering::ShaderHandle fallback_shader;
        rendering::ShaderHandle shadow_map_shader;
        rendering::ShaderHandle bloom_shader;
        rendering::ShaderHandle blur_shader;
        rendering::ShaderHandle hdr_shader;
        
		i32 shaders_to_reload[8];
		i32 shaders_to_reload_count;
	
		rendering::Material *materials;
		i32 material_count;

		rendering::Material *material_instances;
		i32 material_instance_count;
		
		rendering::RegisterBufferInfo *buffers;
		i32 buffer_count;
    
		i32 *_internal_buffer_handles;
		i32 _current_internal_buffer_handle;

		i32 *updated_buffer_handles;
		i32 updated_buffer_handle_count;
    
		i32 *removed_buffer_handles;
		i32 removed_buffer_handle_count;

        rendering::RenderPass passes[32];
        i32 pass_count;

        rendering::RenderPass post_processing_passes[32];
        i32 post_processing_pass_count;

        rendering::FramebufferInfo framebuffers[32];
        i32 framebuffer_count;

        MemoryArena render_pass_arena;
        
		rendering::RenderCommand *render_commands;
		i32 render_command_count;

        QueuedRenderCommand *queued_commands;
        i32 queued_command_count;

        rendering::ShadowCommand *shadow_commands;
        i32 shadow_command_count;

        DirectionalLight *directional_lights;
        i32 dir_light_count;

        PointLight *point_lights;
        i32 point_light_count;

        math::Mat4 light_space_matrix;
        
        math::Vec3 shadow_view_position;

        struct
        {
            b32 active;
            r32 exposure;
            i32 amount;
        } bloom;

        struct
        {
            b32 active;
            r32 exposure;
        } hdr;

        rendering::FramebufferHandle final_framebuffer;
        
	} render;
};


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

#define MAX_LINES 16

struct LineData
{
	math::Vec2 line_sizes[MAX_LINES];
	i32 line_count;
	r32 total_height;
	r32 line_spacing;
};

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

struct TextLengthInfo
{
	size_t length;
    
	r32* widths;
};

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

#endif
