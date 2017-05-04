#ifndef EDITOR_UI_H
#define EDITOR_UI_H

enum Editor_UI_State
{
    State_Off,
    State_ShowEntityList
};

struct editor_ui
{
    Editor_UI_State State;
    uint32 SelectedIndex;
};

#endif