

internal void
draw_point(framebuffer_t *framebuffer, i32 x, i32 y, v3_t color)
{
    // TODO(Ajeej): Check for x and y being out of bounds
    i32 index = (y * framebuffer->width + x) * 4;
    
    if(x > 0 && x < framebuffer->width &&
       y > 0 && y < framebuffer->height)
    {
        framebuffer->color_buffer[index + 0] = (u8)(255*color.r);
        framebuffer->color_buffer[index + 1] = (u8)(255*color.g);
        framebuffer->color_buffer[index + 2] = (u8)(255*color.b);
    }
}

internal void
draw_square(framebuffer_t *framebuffer, i32 x, i32 y, v3_t color)
{
    u32 width = 5;
    u32 height = 5;
    irect2d_t bounds;
    bounds.minx = x - width;
    bounds.miny = y - height;
    bounds.maxx = x + width;
    bounds.maxy = y + height;
    
    for(i32 y = bounds.miny;
        y < bounds.maxy;
        y++)
    {
        for(i32 x = bounds.minx;
            x< bounds.maxx;
            x++)
        {
            draw_point(framebuffer, x, y, color);
        }
    }
}

internal mat4_t
create_perspective_matrix(f32 aspect_ratio, f32 fov, f32 n, f32 f)
{
    
    mat4_t result = {0};
    
    f32 t = 0, r = 0, b = 0, l = 0;
    
    t = Tan(to_radians(fov*0.5f)) * n;
    r = t* aspect_ratio;
    b = -t;
    l = -r;
    
    
    result = {
        (2*n)/(r-l), 0,          (r+l)/(r-l),   0,
        0,          (2*n)/(t-b), (t+b)/(t-b),   0,
        0,           0,          -(f+n)/(f-n), -2*f*n/(f-n),
        0,           0,          -1,            0,
    };
    
    
    return result;
    
}

internal irect2d_t
get_bounding_box(u32 width, u32 height, v2_t coords[3])
{
    irect2d_t result = {};
    v2_t min = get_min(coords[0], coords[1], coords[2]);
    v2_t max = get_max(coords[0], coords[1], coords[2]);
    result.minx = max((i32)floorf32(min.x), 0);
    result.miny = max((i32)floorf32(min.y), 0);
    result.maxx = min((i32)ceilf32(max.x), width - 1);
    result.maxy = min((i32)ceilf32(max.y), height - 1);
    
    return result;
}

internal b32
is_backfacing(v3_t coords[3])
{
    v3_t a = coords[0];
    v3_t b = coords[1];
    v3_t c = coords[2];
    
    f32 signed_area = 
        a.x * b.y - a.y * b.x +
        b.x * c.y - b.y * c.x +
        c.x * a.y - c.y * a.x;
    
    return signed_area >= 0;
}

internal v4_t
get_clip_coords(v3_t world_pos,
                mat4_t world_to_camera,
                mat4_t projection_matrix,
                u32 image_width,
                u32 image_height)
{
    v4_t world = v4(world_pos.x, world_pos.y, world_pos.z, 1);
    
    v4_t camera_pos;
    v4_t clip_pos;
    
    mat4_t view_projection_matrix = projection_matrix * world_to_camera;
    
    //camera_pos = MultiplyV4Mat4(world, world_to_camera);
    
    clip_pos = view_projection_matrix * world;
    
    return clip_pos;
}

internal v3_t
map_to_raster_space(u32 width, u32 height, v3_t coord)
{
    f32 x = (coord.x + 1)*0.5f * (f32)width;
    f32 y = (coord.y + 1)*0.5f * (f32)height;
    f32 z = (coord.y + 1)*0.5f;
    return v3(x, y, z);
}

internal v3_t
get_weights(v2_t verts[3], v2_t p)
{
    v2_t a = verts[0];
    v2_t b = verts[1];
    v2_t c = verts[2];
    v2_t ab = b - a;
    v2_t ac = c - a;
    v2_t ap = p - a;
    f32 ratio = 1/(ab.x * ac.y - ab.y * ac.x);
    f32 w1 = (ac.y * ap.x - ac.x * ap.y) * ratio;
    f32 w2 = (ab.x * ap.y - ab.y * ap.x) * ratio;
    v3_t w = v3(1-w1-w2, w1, w2);
    return w;
}

#define MAX_VARYINGS 10

typedef enum plane_t
{
    Positive_W,
    Positive_X,
    Negative_X,
    Positive_Y,
    Negative_Y,
    Positive_Z,
    Negative_Z,
} plane_t;

internal u32
is_inside_plane(v4_t coord, plane_t plane)
{
    switch(plane)
    {
        case Positive_W:
        {
            return (coord.w >= EPSILON);
        } break;
        
        case Positive_X:
        {
            return (coord.x <= +coord.w);
        } break;
        
        case Negative_X:
        {
            return (coord.x >= -coord.w);
        } break;
        
        case Positive_Y:
        {
            return (coord.y <= +coord.w);
        } break;
        
        case Negative_Y:
        {
            return (coord.y >= -coord.w);
        } break;
        
        case Positive_Z:
        {
            return (coord.z <= +coord.w);
        } break;
        
        case Negative_Z:
        {
            return (coord.z >= -coord.w);
        } break;
        
        default:
        {
            return 0;
        } break;
    }
}

internal f32
get_intersection_ratio(v4_t prev, v4_t curr, plane_t plane)
{
    switch(plane)
    {
        case Positive_W:
        {
            return (prev.w - EPSILON) / (prev.w - curr.w);
        } break;
        
        case Positive_X:
        {
            return (prev.w - prev.x) / ((prev.w - prev.x) - (curr.w - curr.x));
        } break;
        
        case Negative_X:
        {
            return (prev.w + prev.x) / ((prev.w + prev.x) - (curr.w + curr.x));
        } break;
        
        case Positive_Y:
        {
            return (prev.w - prev.y) / ((prev.w - prev.y) - (curr.w - curr.y));
        } break;
        
        case Negative_Y:
        {
            return (prev.w + prev.y) / ((prev.w + prev.y) - (curr.w + curr.y));
        } break;
        
        case Positive_Z:
        {
            return (prev.w - prev.z) / ((prev.w - prev.z) - (curr.w - curr.z));
        } break;
        
        case Negative_Z:
        {
            return (prev.w + prev.z) / ((prev.w + prev.z) - (curr.w + curr.z));
        } break;
        
        default:
        {
            return 0;
        } break;
    }
}

internal i32
clip_against_plane(plane_t plane, u32 in_num_verts,
                   v4_t in_coords[MAX_VARYINGS],
                   v3_t in_color[MAX_VARYINGS], v4_t out_coords[MAX_VARYINGS],
                   v3_t out_color[MAX_VARYINGS])
{
    i32 out_num_verts = 0;
    for(i32 i = 0;
        i < in_num_verts;
        i++)
    {
        u32 prev_index = (i-1+in_num_verts) % in_num_verts;
        u32 curr_index = i;
        v4_t prev_coord = in_coords[prev_index];
        v4_t curr_coord = in_coords[curr_index];
        v3_t prev_color = in_color[prev_index];
        v3_t curr_color = in_color[curr_index];
        b32 prev_inside = is_inside_plane(prev_coord, plane);
        b32 curr_inside = is_inside_plane(curr_coord, plane);
        
        if(prev_inside != curr_inside)
        {
            v4_t *dest_coord = &out_coords[out_num_verts];
            v3_t *dest_color = &out_color[out_num_verts];
            f32 ratio = get_intersection_ratio(prev_coord, curr_coord, plane);
            
            *dest_coord = lerp(prev_coord, curr_coord, ratio);
            
            *dest_color = lerp(prev_color, curr_color, ratio);
            
            out_num_verts += 1;
        }
        
        if(curr_inside)
        {
            v4_t *dest_coord = &out_coords[out_num_verts];
            v3_t *dest_color = &out_color[out_num_verts];
            
            *dest_coord = curr_coord;
            *dest_color = curr_color;
            
            out_num_verts += 1;
        }
    }
    
    return out_num_verts;
}

#define CLIP_IN2OUT(plane)                          \
do {                                            \
num_verts = clip_against_plane(plane,       \
num_verts,   \
in_coords,   \
in_color,    \
out_coords,  \
out_color);  \
if(num_verts < 3)                           \
return 0;                               \
} while(0)

#define CLIP_OUT2IN(plane)                          \
do {                                            \
num_verts = clip_against_plane(plane,       \
num_verts,   \
out_coords,  \
out_color,   \
in_coords,   \
in_color);   \
if(num_verts < 3)                           \
return 0;                               \
} while(0)



internal b32 is_vertex_visible(v4_t vertex)
{
    return absolute(vertex.x) <= vertex.w && absolute(vertex.y) <= vertex.w && absolute(vertex.z) <= vertex.w;
}

internal i32 clip_triangle(v4_t in_coords[MAX_VARYINGS], v4_t out_coords[MAX_VARYINGS],
                           v3_t in_color[MAX_VARYINGS], v3_t out_color[MAX_VARYINGS])
{
    b32 full_visible = 1;
    full_visible &= is_vertex_visible(in_coords[0]);
    full_visible &= is_vertex_visible(in_coords[1]);
    full_visible &= is_vertex_visible(in_coords[2]);
    if(full_visible)
    {
        out_coords[0] = in_coords[0];
        out_coords[1] = in_coords[1];
        out_coords[2] = in_coords[2];
        out_color[0] = in_color[0];
        out_color[1] = in_color[1];
        out_color[2] = in_color[2];
        
        return 3;
    }
    else
    {
        i32 num_verts = 3;
        CLIP_IN2OUT(Positive_W);
        CLIP_OUT2IN(Positive_X);
        CLIP_IN2OUT(Negative_X);
        CLIP_OUT2IN(Positive_Y);
        CLIP_IN2OUT(Negative_Y);
        CLIP_OUT2IN(Positive_Z);
        CLIP_IN2OUT(Negative_Z);
        
        return num_verts;;
    }
}

internal b32
rasterize_triangle(framebuffer_t *framebuffer,
                   v4_t clip_coords[3], v3_t color[3],
                   u32 image_width, u32 image_height)
{
    
    v3_t ndc_coords[3];
    b32 is_backface = 0;
    v2_t screen_coords[3];
    f32 screen_depths[3];
    f32 inv_w[3];
    
    for(u32 i = 0;
        i < 3;
        i++)
    {
        ndc_coords[i] = (clip_coords[i]/clip_coords[i].w).xyz;
        inv_w[i] = 1/clip_coords[i].w;
    }
    
    is_backface = is_backfacing(ndc_coords);
    if(is_backface)
        return 1;
    
    for(u32 i = 0;
        i < 3;
        i++)
    {
        v3_t mapped_coord = map_to_raster_space(image_width, image_height, ndc_coords[i]);
        screen_coords[i] = v2(mapped_coord.x, mapped_coord.y);
        screen_depths[i] = mapped_coord.z;
    }
    
    
    irect2d_t bbox = get_bounding_box(image_width, image_height, screen_coords);
    
    if(!(bbox.minx > image_width ||
         bbox.miny > image_height ||
         bbox.maxx < 0 ||
         bbox.maxy < 0))
    {
        for(u32 y = bbox.miny;
            y <= bbox.maxy;
            y++)
        {
            for(u32 x = bbox.minx;
                x <= bbox.maxx;
                x++)
            {
                v2_t point = v2((f32)x + 0.5f,(f32)y + 0.5f);
                v3_t w = get_weights(screen_coords, point);
                b32 valid_w = 1;
                for(u32 i = 0;
                    i < ArrayCount(w.e);
                    i++)
                {
                    valid_w &= w.e[i] > -EPSILON;
                }
                
                // TODO(ajeej): Triangle overlapping (top-left rule)
                if(valid_w)
                {
                    f32 depth = screen_depths[0]*w.x+
                        screen_depths[1]*w.y+
                        screen_depths[2]*w.z;
                    u32 index = (y)*framebuffer->width + (x);
                    if(depth <= framebuffer->depth_buffer[index])
                    {
                        framebuffer->depth_buffer[index] = depth;
                        
                        f32 w0 = inv_w[0]*w.x;
                        f32 w1 = inv_w[1]*w.y;
                        f32 w2 = inv_w[2]*w.z;
                        f32 normalizer = 1/(w0 + w1 + w2);
                        
                        v3_t final_color;
                        
                        for(u32 i = 0;
                            i < 3;
                            i++)
                        {
                            final_color.e[i] = normalizer*(color[0].e[i]*w0 + color[1].e[i]*w1 + color[2].e[i]*w2);
                        }
                        
                        draw_point(framebuffer, point.x, point.y, final_color);
                    }
                }
            }
        }
    }
    
    return 0;
}

internal void
draw_triangle(framebuffer_t *framebuffer, v3_t world[3], v3_t in_color[MAX_VARYINGS], camera_t *camera, u32 image_width, u32 image_height)
{
    v4_t in_clip_coords[MAX_VARYINGS];
    
    for(u32 i = 0;
        i < 3;
        i++)
    {
        in_clip_coords[i] = get_clip_coords(world[i], camera->world_to_camera, camera->projection_matrix, 
                                            image_width, image_height);
    }
    
    v4_t out_clip_coords[MAX_VARYINGS];
    
    v3_t out_color[MAX_VARYINGS];
    
    i32 num_verts = clip_triangle(in_clip_coords, out_clip_coords,
                                  in_color, out_color);
    
    for(i32 i = 0;
        i < num_verts-2;
        i++)
    {
        u32 index0 = 0;
        u32 index1 = i + 1;
        u32 index2 = i + 2;
        v4_t clip_coords[3];
        v3_t color[3];
        
        clip_coords[0] = out_clip_coords[index0];
        clip_coords[1] = out_clip_coords[index1];
        clip_coords[2] = out_clip_coords[index2];
        color[0] = out_color[index0];
        color[1] = out_color[index1];
        color[2] = out_color[index2];
        
        rasterize_triangle(framebuffer, clip_coords,
                           color, image_width, image_height);
    }
}