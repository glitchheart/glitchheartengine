#ifndef SHADER_H
#define SHADER_H

namespace rendering
{
    struct ShaderHandle
    {
		i32 handle;
    };

    struct MaterialInstanceHandle
    {
		i32 handle;
    };

    struct MaterialHandle
    {
		i32 handle;
    };

    enum ValueType
    {
		INVALID,
		FLOAT,
		FLOAT2,
		FLOAT3,
		FLOAT4,
		INTEGER,
		BOOL,
		MAT4,
		TEXTURE,

        STRUCTURE
    };

    struct TextureHandle
    {
		i32 handle;
    };

    struct VertexAttribute
    {
		ValueType type;
		i32 location;
		union
		{
			r32 float_val;
			math::Vec2 float2_val;
			math::Vec3 float3_val;
			math::Vec4 float4_val;
			i32 integer_val;
			b32 boolean_val;
			math::Mat4 mat4_val;
			TextureHandle texture;
		};

		VertexAttribute () {}
    };

    enum UniformMappingType
    {
		NONE,
		DIFFUSE_TEX,
		DIFFUSE_COLOR,
		SPECULAR_TEX,
		SPECULAR_COLOR,
		SPECULAR_EXPONENT,
		SPECULAR_INTENSITY_TEX,
		AMBIENT_COLOR,

		AMBIENT_TEX,

		DISSOLVE,
	
		SHADOW_MAP,
		MODEL,
		VIEW,
		PROJECTION,

        CAMERA_POSITION,
        
        POINT_LIGHTS,
        DIRECTIONAL_LIGHTS,
        POINT_LIGHT_COUNT,
        DIRECTIONAL_LIGHT_COUNT,
        LIGHT_SPACE_MATRIX,
        
        DIRECTIONAL_LIGHT_DIRECTION,
        DIRECTIONAL_LIGHT_AMBIENT,
        DIRECTIONAL_LIGHT_DIFFUSE,
        DIRECTIONAL_LIGHT_SPECULAR,

        POINT_LIGHT_POSITION,
        POINT_LIGHT_CONSTANT,
        POINT_LIGHT_LINEAR,
        POINT_LIGHT_QUADRATIC,
        POINT_LIGHT_AMBIENT,
        POINT_LIGHT_DIFFUSE,
        POINT_LIGHT_SPECULAR,
        
		MAX
    };

    struct Uniform
    {
		UniformMappingType mapping_type;
		ValueType type;
		char name[32];
        
        i32 structure_index; // If the uniforms is a structure we should save the index of the structure for later

        b32 is_array;
        i32 array_size;
    };

    struct Structure
    {
        char name[32];
        Uniform uniforms[32];
        i32 uniform_count;
    };

	struct UniformValue
	{
        char name[32];
		Uniform uniform;
        i32 array_index;
        
		union
		{
			r32 float_val = 0.0f;
			math::Vec2 float2_val;
			math::Vec3 float3_val;
			math::Vec4 float4_val;
			i32 integer_val;
			b32 boolean_val;
			math::Mat4 mat4_val;
			TextureHandle texture;
		};

		UniformValue () {}
	};

    struct UniformArrayEntry
    {
        UniformValue values[32];
        i32 value_count;
    };
    
    struct UniformArray
    {
        char name[32];
        UniformArrayEntry *entries;

        i32 entry_count;
        i32 max_size;
    };
    
    enum class DefinedValueType
    {
        INTEGER,
        FLOAT
    };
    
    struct DefinedValue
    {
        char name[32];
        DefinedValueType type;
        union
        {
            i32 integer_val;
            r32 float_val;
        };
    };
    
    struct Shader
    {
		VertexAttribute vertex_attributes[16];
		i32 vertex_attribute_count;

		Uniform *uniforms;
		i32 uniform_count;

        Structure structures[32];
        i32 structure_count;

		char* vert_shader;
		char* frag_shader;

		char path[256];

        time_t last_loaded;
        b32 loaded;
        
        DefinedValue defined_values[4];
        i32 defined_value_count;

        MemoryArena arena;
        
		Shader () {}
    };

    struct Material
    {
		ShaderHandle shader;
		UniformValue uniform_values[64];
		i32 uniform_value_count;

        MaterialHandle source_material;

        UniformArray arrays[8];
        i32 array_count;

        struct
        {
            b32 receives_light; // @Cleanup: Do we need this?
            i32 array_handle;
        } lighting;
        
		Material () {}
    };

	HANDLE(Buffer);
	
	struct BufferData
	{
		r32* vertex_buffer;
		i32 vertex_buffer_size;
        i32 vertex_count;
		u16* index_buffer;
		i32 index_buffer_count;
		i32 index_buffer_size;
	};

	enum BufferUsage
	{
		DYNAMIC,
		STATIC,
		STREAM
	};
	
	struct RegisterBufferInfo
	{
		VertexAttribute vertex_attributes[16];
		i32 vertex_attribute_count;

		size_t stride;

		BufferUsage usage;
		
		BufferData data;

		RegisterBufferInfo(const RegisterBufferInfo& other)
        {
            memcpy(vertex_attributes, other.vertex_attributes, sizeof(VertexAttribute) * other.vertex_attribute_count);
            vertex_attribute_count = other.vertex_attribute_count;
            stride = other.stride;
            usage = other.usage;
            data = other.data;
        }

		RegisterBufferInfo() {}
	};

    struct Transform
    {
		math::Vec3 position;
		math::Vec3 rotation;
		math::Vec3 scale;
    };
    
    struct RenderCommand
    {
		MaterialInstanceHandle material;
		Transform transform;
		BufferHandle buffer;
    };

    struct ShadowCommand
    {
        Transform transform;
        BufferHandle buffer;
    };
}

#endif
