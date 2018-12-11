#ifndef CAMERA_H
#define CAMERA_H

struct Camera
{
    r32 zoom;
    math::Vec3 position;
    math::Quat orientation;
    math::Vec3 target;

    math::Mat4 view_matrix;
    math::Mat4 projection_matrix;    
};

static Camera create_camera(math::Vec3 position, math::Vec3 target, math::Mat4 projection_matrix)
{
    Camera camera;
    camera.projection_matrix = projection_matrix;
    camera.position = position;
    camera.target = target;
	camera.view_matrix = math::look_at_with_target(camera.position, camera.target);
    return(camera);
}

#endif
