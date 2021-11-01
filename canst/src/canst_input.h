/* date = March 24th 2021 4:24 pm */

#ifndef CANST_INPUT_H
#define CANST_INPUT_H

// NOTE(ajeej): Used to access keys in key array
typedef enum key_id
{
    // NOTE(ajeej): Look in "input_keys.inc" for definitions
#define Key(name, str) Key_ID_##name,
#include "canst_keys.inc"
#undef Key
    Key_ID_Max,
} key_id;


typedef enum mouse_id
{
    Mouse_ID_Left,
    Mouse_ID_Right,
    Mouse_ID_Middle,
    Mouse_ID_Btn1,
    Mouse_ID_Btn2,
    
    Mouse_ID_Max,
} mouse_id;

// TODO(ajeej): Handle Characters
//              - Character Buffer?

typedef enum input_state
{
    
    Input_State_Down = (1<<0),
    Input_State_Pressed = (1<<1),
    Input_State_Released = (1<<2),
    Input_State_Double = (1<<3),
    Input_State_Ctrl = (1<<4),
    Input_State_Shift = (1<<5),
    Input_State_Alt = (1<<6),
    
} key_state;

typedef struct input
{
    u8 key_states[75];
    u8 mouse_states[5];
    v2_t mouse_pos;
    v2_t mouse_delta;
    v2_t mouse_scroll;
} input_t;

#define IsKeyPressed(input, id) ((input).key_states[id] & Input_State_Pressed)
#define IsKeyReleased(input, id) ((input).key_states[id] & Input_State_Released)
#define IsKeyDown(input, id) ((input).key_states[id] & Input_State_Down)
#define IsKeyUp(input, id) (!IsKeyDown(input, id))

#define IsMousePressed(input, id) ((input).mouse_states[id] & Input_State_Pressed)
#define IsMouseReleased(input, id) ((input).mouse_states[id] & Input_State_Released)
#define IsMouseDown(input, id) ((input).mouse_states[id] & Input_State_Down)
#define IsMouseUp(input, id) (!IsMouseDown(input, id))

#define IsKeyAndModPressed(input, id, mod) (IsKeyPressed(input, id) && (input.key_states[id] & mod))

#endif //CANST_INPUT_H
