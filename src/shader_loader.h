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
		TEXTURE
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
		SPECULAR_INTENSITY,
		SPECULAR_INTENSITY_TEX,
		AMBIENT_COLOR,

		AMBIENT_TEX,

		DISSOLVE,
	
		SHADOW_MAP,
		MODEL,
		VIEW,
		PROJECTION,

		MAX
    };
    
    struct Uniform
    {
		UniformMappingType mapping_type;
		ValueType type;
		char name[32];
    };

	struct UniformValue
	{
		Uniform uniform;
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
    
    struct Shader
    {
		VertexAttribute vertex_attributes[16];
		i32 vertex_attribute_count;

		Uniform uniforms[32];
		i32 uniform_count;

		char* vert_shader;
		char* frag_shader;

		char path[256];

        time_t last_loaded;
		
		Shader () {}
    };

    struct Material
    {
		ShaderHandle shader;
		UniformValue uniform_values[32];
		i32 uniform_value_count;

        MaterialHandle source_material;
        
		Material () {}
    };

	HANDLE(Buffer);
	
	struct BufferData
	{
		r32* vertex_buffer;
		i32 vertex_buffer_size;
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
}

#endif
