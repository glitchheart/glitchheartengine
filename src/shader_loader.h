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
		DIFFUSE_TEX,
		DIFFUSE_COLOR,
		SPECULAR_TEX,
		SPECULAR_COLOR,
		SPECULAR_INTENSITY,
		AMBIENT_COLOR,
		AMBIENT_TEX,
	
		SHADOW_MAP,
		MODEL,
		VIEW,
		PROJECTION,

		MAX
    };

    struct UniformMapping
    {
		UniformMappingType type;
		i32 index;
    };

    
    struct Uniform
    {
		ValueType type;
		char name[32];
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

		Uniform () {}
    };
    
    struct Shader
    {
		VertexAttribute vertex_attributes[16];
		i32 vertex_attribute_count;

		Uniform uniforms[32];
		i32 uniform_count;

		UniformMapping mapped_uniforms[UniformMappingType::MAX];
		i32 mapped_uniform_count;

		Shader () {}
    };

    struct Material
    {
		ShaderHandle shader;
    };

    struct Transform
    {
		math::Vec3 position;
		math::Vec3 rotation;
		math::Vec3 scale;
    };
    
    struct RenderCommand
    {
		MaterialHandle material;
		ShaderHandle shader;
		Transform transform;
    };
}

#endif
