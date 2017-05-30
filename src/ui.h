enum UI_Type
{
    UI_Button,
    UI_Rect,
    UI_Sprite
};

struct health_bar
{
    UI_Type Type;
    glm::vec2 Position;
    ui_render_info RenderInfo;
};