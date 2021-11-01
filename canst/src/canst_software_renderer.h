/* date = April 6th 2021 9:36 am */

#ifndef CANST_SOFTWARE_RENDERER_H
#define CANST_SOFTWARE_RENDERER_H

struct framebuffer_t
{
    i32 width;
    i32 height;
    u8 *color_buffer;
    f32 *depth_buffer;
};

struct camera_t
{
    f32 aspect_ratio;
    f32 fov;
    f32 near_plane;
    f32 far_plane;
    mat4_t camera_to_world;
    // TODO(ajeej): Take this out when uniforms implemented
    mat4_t world_to_camera;
    mat4_t projection_matrix;
};

struct vertice_t
{
    v3_t pos;
    // TODO(ajeej): Implement Alpha
    v3_t color;
};

struct mesh_t
{
    v3_t pos;
    v3_t *verts;
    u32 *indices;
    v3_t *color;
};


#endif //CANST_SOFTWARE_RENDERER_H
