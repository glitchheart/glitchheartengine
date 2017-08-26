static b32 PushButton(renderer& Renderer, const char* Text, math::v2 Position, math::v2 Size, math::rgba BackgroundColor, math::rgba TextColor, input_controller* InputController)
{
    b32 Clicked = false;
    
    auto X = InputController->MouseX;
    auto Y = Renderer.WindowHeight - InputController->MouseY;
    
    Clicked = X >= Position.X && X <= Position.X + Size.X && Y >= Position.Y && Y < Position.Y + Size.Y; 
    b32 MouseDown = MOUSE(Mouse_Left);
    
    PushFilledRect(Renderer, math::v3(Position.X, Position.Y, 0), math::v3(Size.X, Size.Y, 0), Clicked && MouseDown ? math::rgba(1, 1, 1, 1) : BackgroundColor);
    PushText(Renderer, Text, math::v3(Position.X + Size.X / 2, Position.Y + Size.Y / 2, 0), Font_Button, Clicked && MouseDown ? math::rgba(0, 0, 0, 1) : TextColor, Alignment_Center);
    
    return MOUSE_DOWN(Mouse_Left) && Clicked;
}

static b32 PushButton(renderer& Renderer, const char* Text, rect Rect, math::rgba BackgroundColor, math::rgba TextColor, input_controller* InputController)
{
    return PushButton(Renderer, Text, math::v2(Rect.X, Rect.Y), math::v2(Rect.Width, Rect.Height), BackgroundColor, TextColor, InputController);
}