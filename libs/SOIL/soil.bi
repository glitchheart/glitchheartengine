#ifndef __SOIL_BI__
#define __SOIL_BI__

' SOIL Simple OpenGL Image Library
' written by Jonathan Dummer
' http://www.lonesock.net/soil.html
'
' based on Sean's image loader
' http://www.nothings.org/

#ifndef __FB_64BIT__
#inclib "soil32"
#else
#inclib "soil64"
#endif

enum CHANNEL_FLAGS
  SOIL_LOAD_AUTO = 0
  SOIL_LOAD_L    = 1
  SOIL_LOAD_LA   = 2
  SOIL_LOAD_RGB  = 3
  SOIL_LOAD_RGBA = 4
end enum

enum 
  SOIL_CREATE_NEW_ID = 0
end enum

enum SOIL_FLAGS
  SOIL_FLAG_POWER_OF_TWO    = (1 shl 0)
  SOIL_FLAG_MIPMAPS         = (1 shl 1)
  SOIL_FLAG_TEXTURE_REPEATS = (1 shl 2)
  SOIL_FLAG_MULTIPLY_ALPHA  = (1 shl 3)
  SOIL_FLAG_INVERT_Y        = (1 shl 4)
  SOIL_FLAG_COMPRESS_TO_DXT = (1 shl 5)
  SOIL_FLAG_DDS_LOAD_DIRECT = (1 shl 6)
  SOIL_FLAG_NTSC_SAFE_RGB   = (1 shl 7)
  SOIL_FLAG_CoCg_Y          = (1 shl 8)
end enum

enum SAVE_FLAGS
  SOIL_SAVE_TYPE_TGA = 0
  SOIL_SAVE_TYPE_BMP = 1
  SOIL_SAVE_TYPE_DDS = 2
end enum

const SOIL_DDS_CUBEMAP_FACE_ORDER = "EWUDNS"

extern "C"

declare function SOIL_load_OGL_texture ( _
  byval filename         as zstring ptr, _
  byval channelflags     as ulong, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_load_OGL_cubemap ( _
  byval x_pos_file       as zstring ptr, _
  byval x_neg_file       as zstring ptr, _
  byval y_pos_file       as zstring ptr, _
  byval y_neg_file       as zstring ptr, _
  byval z_pos_file       as zstring ptr, _
  byval z_neg_file       as zstring ptr, _
  byval channelflags     as ulong  , _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_load_OGL_single_cubemap ( _
  byval filename         as zstring ptr, _
  byval face_order       as zstring ptr, _
  byval channelflags     as ulong, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_load_OGL_texture_from_memory ( _
  byval buffer           as any ptr, _
  byval buffer_length    as long, _ 
  byval channelflags     as ulong, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_load_OGL_cubemap_from_memory ( _
  byval x_pos_buffer        as any ptr, _
  byval x_pos_buffer_length as long, _
  byval x_neg_buffer        as any ptr, _
  byval x_neg_buffer_length as long, _
  byval y_pos_buffer        as any ptr, _
  byval y_pos_buffer_length as long, _
  byval y_neg_buffer        as any ptr, _
  byval y_neg_buffer_length as long, _
  byval z_pos_buffer        as any ptr, _
  byval z_pos_buffer_length as long, _
  byval z_neg_buffer        as any ptr, _
  byval z_neg_buffer_length as long, _
  byval channelflags        as ulong, _
  byval reuse_texture_ID    as ulong, _
  byval soilflags           as ulong) as ulong

declare function SOIL_load_OGL_single_cubemap_from_memory ( _
  byval buffer           as any ptr, _
  byval buffer_length    as long, _
  byval face_order       as zstring ptr, _
  byval channelflags     as ulong, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_create_OGL_texture ( _
  byval lpData           as any ptr, _
  byval w                as long, _
  byval h                as long, _
  byval channelflags     as ulong, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_create_OGL_single_cubemap ( _
  byval lpData           as any ptr, _
  byval w                as long, _
  byval h                as long, _
  byval channelflags     as ulong, _
  byval face_order       as zstring ptr, _
  byval reuse_texture_ID as ulong, _
  byval soilflags        as ulong) as ulong

declare function SOIL_save_screenshot ( _
  byval filename   as zstring ptr, _
  byval saveflags  as long, _
  byval x          as long, _
  byval y          as long, _
  byval w          as long, _
  byval h          as long) as ulong

declare function SOIL_load_image ( _
  byval filename       as zstring ptr, _
  byval w              as long ptr, _
  byval h              as long ptr, _
  byval channels       as long ptr, _
  byval channelflags   as ulong) as any ptr

declare function SOIL_load_image_from_memory ( _
  byval buffer         as any ptr, _
  byval buffer_length  as long, _
  byval w              as long ptr, _
  byval h              as long ptr, _
  byval channels       as long ptr, _
  byval channelflags   as ulong) as any ptr

declare function SOIL_save_image ( _
  byval filename   as zstring ptr, _
  byval saveflag   as ulong, _
  byval w          as long, _
  byval h          as long, _
  byval channels   as ulong, _
  byval lpData     as any ptr) as long

declare sub      SOIL_free_image_data ( _
  byref img_Data as any ptr)

declare function SOIL_last_result () as zstring ptr

end extern

#endif ' __SOIL_BI__
