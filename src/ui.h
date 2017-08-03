#ifndef UI_H
#define UI_H

enum UI_Type
{
    UI_Button,
    UI_Rect,
    UI_Sprite
};

struct health_bar
{
    UI_Type Type;
    math::v2 Position;
    ui_render_info RenderInfo;
};

struct player_ui
{
    math::v2 ScreenPosition;
};

#endif