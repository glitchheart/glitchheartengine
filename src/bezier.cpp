namespace bezier
{
    
    static math::Vec2 get_point_on_curve(math::Vec2 p0, math::Vec2 p1, math::Vec2 p2, math::Vec2 p3, r32 t)
    {
        math::Vec2 a = math::lerp(p0, t, p1);
        math::Vec2 b = math::lerp(p1, t, p2);
        math::Vec2 c = math::lerp(p2, t, p3);
        math::Vec2 d = math::lerp(a, t, b);
        math::Vec2 e = math::lerp(b, t, c);
        math::Vec2 point_on_curve = math::lerp(d, t, e);
        return point_on_curve;
    }

    static math::Vec2 get_point_on_curve(BezierCurve curve, r32 t)
    {
        return get_point_on_curve(curve.p0, curve.p1, curve.p2, curve.p3, t);
    }

    static void draw_bezier_curve(BezierCurve curve, r32 t)
    {
        Renderer* renderer = core.renderer;

        i32 num_samples = 100;

        r32 base_x = 100.0f;
        r32 base_y = 100.0f;

        rendering::CreateUICommandInfo quad_command = rendering::create_ui_command_info();

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p0;
        quad_command.transform.scale = math::Vec2(2.0f);
        quad_command.z_layer = 50;
        quad_command.scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO;
        quad_command.clip = false;
        quad_command.color = math::Rgba(1.0, 1.0, 0.0, 1.0);
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p1;
        quad_command.color = COLOR_RED;
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p2;
        quad_command.color = COLOR_BLUE;
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p3;
        quad_command.color = COLOR_GREEN;
        rendering::push_ui_quad(renderer, quad_command);

        r32 ratio = t / (r32)num_samples;
        
        for(i32 i = 0; i < num_samples; i++)
        {
            math::Vec2 p = get_point_on_curve(curve, ratio * i);
            quad_command.transform.position = math::Vec2(base_x, base_y) + p;
            quad_command.transform.scale = math::Vec2(1.0f);
            quad_command.color = COLOR_WHITE;
            rendering::push_ui_quad(renderer, quad_command);            
        }
    }

}
