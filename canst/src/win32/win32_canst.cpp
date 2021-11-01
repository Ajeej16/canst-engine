#include <windows.h>

// TODO(ajeej): Make includes for this
#include "canst_settings.h"
#include "canst_utils.h"
#include "canst_math.h"
#include "canst_input.h"
#include "canst_software_renderer.h"

struct win32_surface_t
{
    i32 width;
    i32 height;
    u8 *present_buffer;
    BITMAPINFO info;
};


global input_t global_input = {0};
global win32_surface_t global_surface = {0};
global b32 global_running = 0;

#include "canst_software_renderer.cpp"

// TODO(ajeej): Add surface to contain present buffer
internal void
win32_create_surface(win32_surface_t *surface, i32 width, i32 height)
{
    if(surface->present_buffer)
        free(surface->present_buffer);
    
    surface->width = width;
    surface->height = height;
    
    surface->info.bmiHeader.biSize = sizeof(surface->info.bmiHeader);
    surface->info.bmiHeader.biWidth = width;
    surface->info.bmiHeader.biHeight = -height;
    surface->info.bmiHeader.biPlanes = 1;
    surface->info.bmiHeader.biBitCount = 32;
    surface->info.bmiHeader.biCompression = BI_RGB;
    
    u32 size = width*height*4;
    surface->present_buffer = (u8 *)malloc(size*sizeof(*surface->present_buffer));
    
}

internal LRESULT
win32_winproc(HWND window_handle, UINT message,
              WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    
    switch(message)
    {
        
        case WM_CLOSE:
        case WM_DESTROY:
        case WM_QUIT:
        {
            
            global_running = 0;
            result = 0;
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            HDC dc = BeginPaint(window_handle, &paint);
            
            RECT rect = paint.rcPaint;
            i32 x = rect.left;
            i32 y = rect.top;
            i32 width = rect.right - x;
            i32 height = rect.bottom - y;
            StretchDIBits(dc,
                          0, 0, width, height,
                          0, 0, global_surface.width, global_surface.height,
                          (void*)global_surface.present_buffer,
                          &global_surface.info,
                          DIB_RGB_COLORS, SRCCOPY);
            
            EndPaint(window_handle, &paint);
        } break;
        
        default:
        {
            result = DefWindowProc(window_handle, message,
                                   wparam, lparam);
        } break;
    }
    
    
    
    return result;
}

internal b32
update_input_state(u8 *input_state, b32 is_down, b32 was_down, u8 modifiers)
{
    b32 was_pressed_or_released = 0;
    
    // TODO(ajeej): Find a way to make modifier update faster
    *input_state &= ~(Input_State_Ctrl|Input_State_Alt|Input_State_Shift);
    *input_state |= modifiers;
    
    if(is_down)
    {
        *input_state |= Input_State_Down;
        if(!was_down)
        {
            *input_state |= Input_State_Pressed;
            was_pressed_or_released = 1;
        }
    }
    else
    {
        *input_state &= ~(Input_State_Down);
        *input_state &= ~(Input_State_Pressed);
        *input_state |= Input_State_Released;
        was_pressed_or_released = 1;
    }
    
    return was_pressed_or_released;
}

internal void
win32_process_messages(HWND window_handle)
{
    MSG message = {0};
    
    local_persist u64 prev_inputs[16];
    local_persist u64 input_count = 0;
    
    for(u32 input = 0;
        input < input_count;
        input++)
    {
        u64 index = prev_inputs[input];
        if(global_input.key_states[index] & Input_State_Pressed)
            global_input.key_states[index] &= ~Input_State_Pressed;
        else if (global_input.key_states[index] & Input_State_Released)
            global_input.key_states[index] &= ~Input_State_Released;
        else if (global_input.mouse_states[index] & Input_State_Pressed)
            global_input.mouse_states[index] &= ~Input_State_Pressed;
        else if (global_input.mouse_states[index] & Input_State_Released)
            global_input.mouse_states[index] &= ~Input_State_Released;
    }
    input_count = 0;
    
    for(;;)
    {
        // TODO(ajeej): Assert that pkr_count doesn't go above 16
        BOOL got_message = FALSE;
        DWORD skip_messages[] = { 0x738, 0xFFFFFFFF, };
        DWORD last_message = 0;
        for(u32 skip_index = 0;
            skip_index< ArrayCount(skip_messages);
            skip_index++)
        {
            DWORD skip = skip_messages[skip_index];
            got_message = PeekMessage(&message, 0, last_message, skip-1, PM_REMOVE);
            if(got_message)
                break;
            
            last_message = skip+1;
        }
        
        if(!got_message)
        {
            break;
        }
        
        u64 wparam = message.wParam;
        u64 lparam = message.lParam;
        
        u8 modifiers = 0;
        if(GetKeyState(VK_CONTROL) & 0x0080)
        {
            modifiers |= Input_State_Ctrl;
        }
        if(GetKeyState(VK_MENU) & 0x0080)
        {
            modifiers |= Input_State_Alt;
        }
        if(GetKeyState(VK_SHIFT) & 0x0080)
        {
            modifiers |= Input_State_Shift;
        }
        
        
        switch(message.message)
        {
            
            case WM_LBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Left];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Left;
            } break;
            
            case WM_LBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Left];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Left;
            } break;
            
            case WM_RBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Right];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Right;
            } break;
            
            case WM_RBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Right];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Right;
            } break;
            
            case WM_MBUTTONDOWN:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Middle];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Middle;
            } break;
            
            case WM_MBUTTONUP:
            {
                u8 *mouse_state = &global_input.mouse_states[Mouse_ID_Middle];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = Mouse_ID_Middle;
            } break;
            
            case WM_XBUTTONDOWN:
            {
                u64 btn = 0;
                if(wparam & (1<<6))
                    btn= Mouse_ID_Btn1;
                else if(wparam & (1<<6))
                    btn= Mouse_ID_Btn2;
                
                u8 *mouse_state = &global_input.mouse_states[btn];
                b32 was_down = *mouse_state & Input_State_Down;
                if(update_input_state(mouse_state, 1, was_down, modifiers))
                    prev_inputs[input_count++] = btn;
            } break;
            
            case WM_XBUTTONUP:
            {
                u64 btn = 0;
                if(wparam & (1<<6))
                    btn= Mouse_ID_Btn1;
                else if(wparam & (1<<6))
                    btn= Mouse_ID_Btn2;
                
                u8 *mouse_state = &global_input.mouse_states[btn];
                if(update_input_state(mouse_state, 0, 0, modifiers))
                    prev_inputs[input_count++] = btn;
            } break;
            
            // TODO(ajeej): Implement Doubl Click
            
            case WM_SYSKEYUP:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_KEYDOWN:
            {
                u64 key_code = wparam;
                b8 is_down = ((lparam & (1<< 31)) == 0);
                b8 was_down = ((lparam & (1<< 30)) > 0);
                u64 key = 0;
                u8 *key_state = 0;
                
                if(key_code >= 'A' && key_code <= 'Z')
                    key = Key_ID_A + (key_code - 'A');
                else if (key_code >= '0' && key_code <= '9')
                    key = Key_ID_0 + (key_code - '0');
                else if (key_code == VK_ESCAPE)
                    key = Key_ID_Esc;
                else if (key_code >= VK_F1 && key_code <= VK_F12)
                    key = Key_ID_F1 + (key_code - VK_F1);
                else if (key_code == VK_OEM_3)
                    key = Key_ID_GraveAccent;
                else if (key_code == VK_OEM_MINUS)
                    key = Key_ID_Minus;
                else if (key_code == VK_OEM_PLUS)
                    key = Key_ID_Equal;
                else if (key_code == VK_BACK)
                    key = Key_ID_Backspace;
                else if (key_code == VK_TAB)
                    key = Key_ID_Tab;
                else if (key_code == VK_SPACE)
                    key = Key_ID_Space;
                else if (key_code == VK_RETURN)
                    key = Key_ID_Enter;
                else if (key_code == VK_UP)
                    key = Key_ID_Up;
                else if (key_code == VK_LEFT)
                    key = Key_ID_Left;
                else if (key_code == VK_DOWN)
                    key = Key_ID_Down;
                else if (key_code == VK_RIGHT)
                    key = Key_ID_Right;
                else if (key_code == VK_DELETE)
                    key = Key_ID_Delete;
                else if (key_code == VK_PRIOR)
                    key = Key_ID_PageUp;
                else if (key_code == VK_NEXT)
                    key = Key_ID_PageDown;
                else if (key_code == VK_HOME)
                    key = Key_ID_Home;
                else if (key_code == VK_END)
                    key = Key_ID_End;
                else if (key_code == VK_OEM_2)
                    key = Key_ID_ForwardSlash;
                else if (key_code == VK_OEM_102)
                    key = Key_ID_BackSlash;
                else if (key_code == VK_OEM_PERIOD)
                    key = Key_ID_Period;
                else if (key_code == VK_OEM_COMMA)
                    key = Key_ID_Comma;
                else if (key_code == VK_OEM_7)
                    key = Key_ID_Quote;
                else if (key_code == VK_OEM_4)
                    key = Key_ID_LeftBracket;
                else if (key_code == VK_OEM_6)
                    key = Key_ID_RightBracket;
                else if (key_code == VK_CONTROL)
                {
                    key = Key_ID_Ctrl;
                    modifiers &= ~Input_State_Ctrl;
                }
                else if (key_code == VK_SHIFT)
                {
                    key = Key_ID_Shift;
                    modifiers &= ~Input_State_Shift;
                }
                else if (key_code == VK_MENU)
                {
                    key = Key_ID_Alt;
                    modifiers &= ~Input_State_Alt;
                }
                
                if(update_input_state(&global_input.key_states[key],
                                      is_down, was_down, modifiers))
                    prev_inputs[input_count++] = key;
                
            } break;
            
            default:
            {
                TranslateMessage(&message);
                DispatchMessage(&message);
            } break;
        }
    }
}

// TODO(ajeej): Some surfaces can be platform independent
//              Remeber to seperate these parts out in a
//              platform indepentent renderer file
internal void
win32_blitBGR(framebuffer_t *src, win32_surface_t *dst)
{
    i32 width = min(dst->width, src->width);
    i32 height = min(dst->height, src->height);
    
    
    // TODO(ajeej): Improve speed
    for(i32 y = 0;
        y < height;
        y++)
    {
        for(i32 x = 0;
            x < width;
            x++)
        {
            // NOTE(ajeej): Flipping seems to flip the final image
            //              since rasterization already makes it render
            //              from top to down
            i32 flipped_y = (height - 1) - y;
            i32 src_index = (y*width+x)*4;
            i32 dst_index = (flipped_y*width+x)*4;
            u8 *src_pixel = &src->color_buffer[src_index];
            u8 *dst_pixel = &dst->present_buffer[dst_index];
            dst_pixel[0] = src_pixel[2];
            dst_pixel[1] = src_pixel[1];
            dst_pixel[2] = src_pixel[0];
        }
    }
}



int
WinMain(HINSTANCE instance, HINSTANCE prev_instance,
        LPSTR cmd_line, INT show_cmd)
{
    
    // TODO(ajeej): Initialize
    
    WNDCLASS window_class = {};
    {
        // TODO(ajeej): Implement double click
        window_class.style = CS_HREDRAW|CS_VREDRAW;//|CS_DBLCLKS;
        window_class.lpfnWndProc = win32_winproc;
        window_class.hInstance = instance;
        window_class.lpszClassName = "canstWindowClass";
        window_class.hCursor = LoadCursor(0, IDC_ARROW);
    }
    
    if(!RegisterClass(&window_class))
    {
        // TODO(ajeej): Debug Message
        goto quit;
    }
    
    HWND window_handle = CreateWindow(window_class.lpszClassName,
                                      WINDOW_TITLE,
                                      WS_OVERLAPPEDWINDOW,
                                      CW_USEDEFAULT, CW_USEDEFAULT,
                                      DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                      0, 0, instance, 0);
    
    if(!window_handle)
    {
        // TODO(ajeej): Debug Message
        goto quit;
    }
    
    framebuffer_t framebuffer = {};
    framebuffer.width = DEFAULT_WINDOW_WIDTH;
    framebuffer.height = DEFAULT_WINDOW_HEIGHT;
    framebuffer.color_buffer = (u8 *)malloc(framebuffer.width*framebuffer.height*sizeof(*framebuffer.color_buffer)*4);
    framebuffer.depth_buffer = (f32 *)malloc(framebuffer.width*framebuffer.height*sizeof(*framebuffer.depth_buffer));
    
    HDC memory_dc;
    win32_create_surface(&global_surface, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
    
    ShowWindow(window_handle, show_cmd);
    UpdateWindow(window_handle);
    
    global_running = 1;
    
    
    
    mat4_t camera_to_world = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, -10,
        0, 0, 0, 1,
    };
    
    u32 image_width = DEFAULT_WINDOW_WIDTH;
    u32 image_height = DEFAULT_WINDOW_HEIGHT;
    
    camera_t camera = {};
    camera.fov = 45.0f;
    camera.aspect_ratio = image_width/(f32)image_height;
    camera.near_plane = 0.1f;
    camera.far_plane = 100.0f;
    camera.camera_to_world = camera_to_world;
    
    v3_t verts[] = { 
        //Front
        { -0.5f, -0.5f,  0.5f },
        {  0.5f, -0.5f,  0.5f },
        {  0.5f,  0.5f,  0.5f },
        { -0.5f,  0.5f,  0.5f },
        //Back
        { -0.5f, -0.5f, -0.5f },
        { -0.5f,  0.5f, -0.5f },
        {  0.5f,  0.5f, -0.5f },
        {  0.5f, -0.5f, -0.5f },
        //Top
        { -0.5f,  0.5f, -0.5f },
        { -0.5f,  0.5f,  0.5f },
        {  0.5f,  0.5f,  0.5f },
        {  0.5f,  0.5f, -0.5f },
        //Bottom
        { -0.5f, -0.5f, -0.5f },
        {  0.5f, -0.5f, -0.5f },
        {  0.5f, -0.5f,  0.5f },
        { -0.5f, -0.5f,  0.5f },
        //Right
        {  0.5f, -0.5f, -0.5f },
        {  0.5f,  0.5f, -0.5f },
        {  0.5f,  0.5f,  0.5f },
        {  0.5f, -0.5f,  0.5f },
        //Left
        { -0.5f, -0.5f, -0.5f },
        { -0.5f, -0.5f,  0.5f },
        { -0.5f,  0.5f,  0.5f },
        { -0.5f,  0.5f, -0.5f },
    };
    
    u32 indices[] = {
        0, 1, 2,     2, 3, 0,
        4, 5, 6,     4, 6, 7,
        8, 9, 10,    8, 10, 11,
        12, 13, 14,   12, 14, 15,
        16, 17, 18,  16, 18, 19,
        20, 21, 22,  20, 22, 23,
    };
    
    v3_t colors[] = {
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        {1, 0, 1},
        
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        {1, 0, 0},
        
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        {0, 1, 0},
        
        {0, 1, 1},
        {0, 1, 1},
        {0, 1, 1},
        {0, 1, 1},
        
        {1, 1, 0},
        {1, 1, 0},
        {1, 1, 0},
        {1, 1, 0},
        
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1},
    };
    
    mesh_t square = {};
    square.pos = v3(0, 0, 0);
    square.verts = (v3_t *)malloc(sizeof(*square.verts)*24);
    square.indices = (u32 *)malloc(sizeof(*square.indices)*36);
    square.color = (v3_t *)malloc(sizeof(*square.color)*24);
    square.verts = verts;
    square.indices = indices;
    square.color = colors;
    mat4_t model_matrix = identity();
    
    camera.projection_matrix = create_perspective_matrix(camera.aspect_ratio,
                                                         camera.fov,
                                                         camera.near_plane,
                                                         camera.far_plane);
    
    v3_t camera_pos = v3(0, 0, 10);
    v3_t camera_rot = v3(0, 180, 0);
    f32 camera_speed = CAMERA_SPEED;
    
    v3_t camera_front = v3(0, 0, -1);
    v3_t camera_right = v3(1, 0, 0);
    
    while(global_running)
    {
        
        for(u32 i = 0; 
            i < 3;
            i++)
        {
            camera_front[i] = camera.camera_to_world(i, 2); 
            camera_right[i] = camera.camera_to_world(i, 0); 
        }
        
        // Message Loop
        {
            win32_process_messages(window_handle);
            
            if(IsKeyDown(global_input, Key_ID_S))
            {
                camera_pos += camera_front*camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_W))
            {
                camera_pos -= camera_front*camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_A))
            {
                camera_pos -= camera_right*camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_D))
            {
                camera_pos += camera_right*camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_Ctrl))
            {
                camera_pos.y -= camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_Space))
            {
                camera_pos.y += camera_speed;
            }
            if(IsKeyDown(global_input, Key_ID_Up))
            {
                camera_rot.x -= CAMERA_ROTATION;
            }
            if(IsKeyDown(global_input, Key_ID_Down))
            {
                camera_rot.x += CAMERA_ROTATION;
            }
            if(IsKeyDown(global_input, Key_ID_Left))
            {
                camera_rot.y -= CAMERA_ROTATION;
            }
            if(IsKeyDown(global_input, Key_ID_Right))
            {
                camera_rot.y += CAMERA_ROTATION;
            }
        }
        
        camera.camera_to_world = identity(); 
        camera.camera_to_world(2, 2) = -1;
        camera.camera_to_world = rotate(camera.camera_to_world, camera_rot.x, v3(1, 0, 0));
        camera.camera_to_world = rotate(camera.camera_to_world, camera_rot.y, v3(0, 1, 0));
        camera.camera_to_world = rotate(camera.camera_to_world, camera_rot.z, v3(0, 0, -1));
        camera.camera_to_world = translate(camera.camera_to_world, camera_pos);
        local_persist b32 i = 0;
        
        
        for(i32 y = 0;
            y < framebuffer.height;
            y++)
        {
            for(i32 x = 0;
                x < framebuffer.width;
                x++)
            {
                u8 *pixel = framebuffer.color_buffer + (y*framebuffer.width+x)*4;
                pixel[0] = 0;
                pixel[1] = 0;
                pixel[2] = 0;
                pixel[3] = 0;
                
                framebuffer.depth_buffer[y*framebuffer.width+x] = 10.0f;
            }
        }
        
        
        
#if 1        
        camera.world_to_camera = inverse(camera.camera_to_world);
        
        for(u32 i = 0;
            i < 2*6;
            i++)
        {
            v3_t *verts = square.verts;
            u32 *indices = square.indices + i*3;
            
            v3_t world[3];
            v3_t color[3];
            world[0] = model_matrix*verts[indices[0]];
            world[1] = model_matrix*verts[indices[1]];
            world[2] = model_matrix*verts[indices[2]];
            color[0] = square.color[indices[0]];
            color[1] = square.color[indices[1]];
            color[2] = square.color[indices[2]];
            
            draw_triangle(&framebuffer, world, color, &camera, image_width, image_height);
        }
#endif
        
        win32_blitBGR(&framebuffer, &global_surface);
        HDC window_dc = GetDC(window_handle);
        RECT rect = {0};
        GetClientRect(window_handle, &rect);
        StretchDIBits(window_dc,
                      0, 0, rect.right-rect.left, rect.bottom-rect.top,
                      0, 0, global_surface.width, global_surface.height,
                      (void*)global_surface.present_buffer,
                      &global_surface.info,
                      DIB_RGB_COLORS, SRCCOPY);
        ReleaseDC(window_handle, window_dc);
    }
    
    quit:
    return 0;
}