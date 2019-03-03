#ifndef CAMERA_H
#define CAMERA_H

enum CameraMode
{
    FREE_ROAM,
    CENTERED
};

struct Camera
{
    CameraMode mode;
    r32 zoom;

    math::Vec3 position;
    math::Vec3 target;

    math::Vec3 forward;
    math::Vec3 right;
    math::Vec3 up;
    
    math::Mat4 view_matrix;
    math::Mat4 projection_matrix;

    r32 yaw;
    r32 pitch;
};

static Camera create_camera(math::Vec3 position, math::Vec3 target, math::Mat4 projection_matrix)
{
    Camera camera;
    camera.mode = CameraMode::FREE_ROAM;
    camera.position = position;
    camera.target = target;
    
    camera.forward = math::normalize(target - position);
    camera.right = math::normalize(math::cross(math::Vec3(0, 1, 0), camera.forward));
    camera.up = math::normalize(math::cross(camera.forward, camera.right));
    
    camera.yaw = atan2(camera.forward.x, camera.forward.z) / DEGREE_IN_RADIANS;
    camera.pitch = asin(camera.forward.y) / DEGREE_IN_RADIANS;
    
	camera.view_matrix = math::look_at(camera.forward, camera.position);
    camera.projection_matrix = projection_matrix;
    
    return(camera);
}

static void translate_forward(Camera &camera, r32 amount)
{
    r32 dist = math::distance(camera.target, camera.position);
    camera.position += camera.forward * amount;
	camera.view_matrix = math::look_at(camera.forward, camera.position);
    camera.target = camera.position + camera.forward * dist;   
}

static void translate_up(Camera &camera, r32 amount)
{
    r32 dist = math::distance(camera.target, camera.position);
    camera.position += camera.up * amount;
	camera.view_matrix = math::look_at(camera.forward, camera.position);
    camera.target = camera.position + camera.forward * dist;
}

static void translate_right(Camera &camera, r32 amount)
{
    r32 dist = math::distance(camera.target, camera.position);
    camera.position += camera.right * amount;
	camera.view_matrix = math::look_at(camera.forward, camera.position);
    camera.target = camera.position + camera.forward * dist;   
}

static void update_rotation(Camera &camera)
{
    r32 pitch = camera.pitch * DEGREE_IN_RADIANS;
    r32 yaw = camera.yaw * DEGREE_IN_RADIANS;

    r32 dist = math::distance(camera.target, camera.position);

    camera.forward.x = math::cos(pitch) * math::sin(yaw);
    camera.forward.y = math::sin(pitch);
    camera.forward.z = math::cos(pitch) * math::cos(yaw);
    camera.forward = math::normalize(camera.forward);
    
    camera.target = camera.position + camera.forward * dist;

    camera.right = math::normalize(math::cross(math::Vec3(0, 1, 0), camera.forward));
    camera.up = math::normalize(math::cross(camera.forward, camera.right));
    
	camera.view_matrix = math::look_at(camera.forward, camera.position);
}

static void update_arcball_rotation(Camera &camera, r32 pitch_amount, r32 yaw_amount)
{
    r32 pitch = pitch_amount * DEGREE_IN_RADIANS;
    r32 yaw = yaw_amount * DEGREE_IN_RADIANS;

    math::Vec3 camera_focus_vector = camera.position - camera.target;

    math::Quat orientation = math::Quat();
    orientation = math::rotate(orientation, pitch, camera.right);
    orientation = math::rotate(orientation, yaw, math::Vec3(0.0f, 1.0f, 0.0f));
    camera.position = to_matrix(orientation) * camera_focus_vector + camera.target;

    camera.forward = math::normalize(camera.target - camera.position);
    camera.right = math::normalize(math::cross(math::Vec3(0, 1, 0), camera.forward));
    camera.up = math::normalize(math::cross(camera.forward, camera.right));
    
	camera.view_matrix = math::look_at(camera.forward, camera.position);
}

static void rotate_around_x(Camera &camera, r32 amount)
{
    camera.pitch += amount;
    
    if(camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if(camera.pitch < -89.0f)
        camera.pitch = -89.0f;

    if(camera.mode == CameraMode::FREE_ROAM)
        update_rotation(camera);
    else
        update_arcball_rotation(camera, amount * 10.0f, 0.0f);
}

static void invert_pitch(Camera &camera)
{
    camera.pitch = -camera.pitch;
    update_rotation(camera);
}

static void rotate_around_y(Camera &camera, r32 amount)
{
    camera.yaw += amount;

    if(camera.mode == CameraMode::FREE_ROAM)
        update_rotation(camera);
    else
        update_arcball_rotation(camera, 0.0f, -amount * 10.0f);
}

static void center(Camera &camera)
{
    camera.mode = CameraMode::CENTERED;
}

static void set_target(Camera &camera, math::Vec3 target)
{
    camera.target = target;
    
    camera.forward = math::normalize(camera.target - camera.position);
    camera.right = math::normalize(math::cross(math::Vec3(0, 1, 0), camera.forward));
    camera.up = math::normalize(math::cross(camera.forward, camera.right));
    
    camera.yaw = atan2(camera.forward.x, camera.forward.z) / DEGREE_IN_RADIANS;
    camera.pitch = asin(camera.forward.y) / DEGREE_IN_RADIANS;
    
	camera.view_matrix = math::look_at(camera.forward, camera.position);
}

static void free_roam(Camera &camera)
{
    camera.mode = CameraMode::FREE_ROAM;
}
#endif
