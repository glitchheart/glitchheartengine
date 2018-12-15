#ifndef RENDER_PIPELINE_H
#define RENDER_PIPELINE_H

namespace rendering
{
    
#define MAX_INSTANCE_BUFFERS 32
    
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

    struct FramebufferHandle
    {
        i32 handle;
    };

    struct RenderPassHandle
    {
        i32 handle;
    };

    struct PostProcessingRenderPassHandle
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
        MS_TEXTURE,

        STRUCTURE
    };

    struct TextureHandle
    {
		i32 handle;
    };

    struct MSTextureHandle
    {
		i32 handle;
    };

    struct InstanceBufferHandle
    {
        ValueType type;
        i32 handle;
    };

    struct VertexAttribute
    {
		ValueType type;
        char name[32];
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
            MSTextureHandle ms_texture;
		};
        
		VertexAttribute () {}
    };

    enum class VertexAttributeMappingType
    {
        NONE,
        POSITION,
        SCALE,
        ROTATION
    };

    struct VertexAttributeInstanced
    {
        VertexAttributeMappingType mapping_type;
        VertexAttribute attribute;
        InstanceBufferHandle instance_buffer_handle;
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
        SHADOW_VIEW_POSITION,
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

        CLIPPING_PLANE,
        
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

        i32 location_index;
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
            MSTextureHandle ms_texture;
		};
        
		UniformValue () {}
	};

    struct UniformEntry
    {
        UniformValue values[32];
        i32 value_count;
    };
    
    struct UniformArray
    {
        char name[32];
        UniformEntry *entries;

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

        VertexAttributeInstanced instanced_vertex_attributes[16];
        i32 instanced_vertex_attribute_count;

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
        
        VertexAttributeInstanced instanced_vertex_attributes[8];
        i32 instanced_vertex_attribute_count;
        
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

        b32 instanced;
        struct
        {
            
        } instancing;
        
		Material () {}
    };

	HANDLE(Buffer);
    HANDLE(InternalBuffer);
	
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

        i32 count;
        
        struct
        {
            rendering::RenderPassHandle pass_handle;
            rendering::ShaderHandle shader_handle;
        } pass;
    };

    struct ShadowCommand
    {
        Transform transform;
        BufferHandle buffer;
    };
    
    enum FramebufferType
    {
        NORMAL,
        POST_PROCESSING
    };

    enum ColorAttachmentFlags
    {
        RGBA_8 = 1 << 0,
        RGBA_16 = 1 << 1,
        RGBA_32 = 1 << 2,
        HDR = 1 << 3,
        MULTISAMPLED = 1 << 4
    };

    enum DepthAttachmentFlags
    {
        DEPTH_MULTISAMPLED = 1 << 0
    };

    enum class ColorAttachmentType
    {
        TEXTURE,
        RENDER_BUFFER
    };

    struct ColorAttachment
    {
        ColorAttachmentType type;
        u64 flags;

        union
        {
            TextureHandle texture;
            MSTextureHandle ms_texture;
        };
        
        u32 samples;
    };
    
    struct FramebufferInfo
    {
        char name[32];
        
        FramebufferType type;
        
        u32 width;
        u32 height;

        struct
        {        
            b32 enabled;
            ColorAttachment attachments[4];
            i32 count;
        } color_attachments;
        struct
        {
            b32 enabled;
            u64 flags;
            u32 samples;
        } depth_attachment;

        struct
        {
            PostProcessingRenderPassHandle pass_handles[4];
            i32 color_attachment_indices[4];
            char uniform_names[4][32];
            i32 count;
        } pending_textures;
    };

    enum class ClippingPlaneType
    {
        NONE,
        HORIZONTAL_ABOVE,
        HORIZONTAL_BELOW
    };
    
#define STANDARD_PASS "STANDARD_PASS"

    // @Note: When rendering a scene, multiple render passes can be used.
    // Each render component can register itself to a render pass
    // Each render pass has it's own render commands
    // Each render pass has a framebuffer handle
    // When reaching the graphics renderer, each render pass binds it's framebuffer and goes through all render commands
    // A render pass describes what framebuffer to render to and the commands that should be rendered
    struct RenderPass
    {
        char name[32];
        
        FramebufferHandle framebuffer;
        
        b32 use_scene_camera;
        Camera camera;

        struct
        {
            ClippingPlaneType type;
            math::Vec4 plane;
        } clipping_planes;
        
        union
        {
            struct
            {
                UniformValue uniform_values[64];
                i32 uniform_value_count;
                
                ShaderHandle shader_handle;
            } post_processing;
            struct
            {
                rendering::RenderCommand *render_commands;
                i32 render_command_count;
            } commands;
        };

        RenderPass() {}
    };
}

#endif
