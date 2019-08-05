namespace bezier
{
    
    static math::Vec2 get_point_on_curve(math::Vec2 p0, math::Vec2 p1, math::Vec2 p2, math::Vec2 p3, r32 t)
    {
        r32 u = 1.0f - t;
        r32 t2 = t * t;
        r32 u2 = u * u;
        r32 u3 = u2 * u;
        r32 t3 = t2 * t;
        
        return 
            p0 * u3 +
            p1 * (3.0f * u2 * t) +
            p2 * (3.0f * u * t2) +
            (p3 * t3);
    }

    static math::Vec2 get_point_on_curve(CubicBezier curve, r32 t)
    {
        return get_point_on_curve(curve.p0, curve.p1, curve.p2, curve.p3, t);
    }

    static void draw_bezier_curve(CubicBezier curve, r32 t, math::Vec2 position, r32 scale)
    {
        Renderer* renderer = core.renderer;

        i32 num_samples = 100;

        r32 base_x = position.x;
        r32 base_y = position.y;

        rendering::CreateUICommandInfo quad_command = rendering::create_ui_command_info();

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p0;
        quad_command.transform.scale = math::Vec2(2.0f);
        quad_command.z_layer = 50;
        quad_command.scaling_flag = UIScalingFlag::KEEP_ASPECT_RATIO;
        quad_command.clip = false;
        quad_command.color = math::Rgba(1.0, 1.0, 0.0, 1.0);
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p1 * scale;
        quad_command.color = COLOR_RED;
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p2 * scale;
        quad_command.color = COLOR_BLUE;
        rendering::push_ui_quad(renderer, quad_command);

        quad_command.transform.position = math::Vec2(base_x, base_y) + curve.p3 * scale;
        quad_command.color = COLOR_GREEN;
        rendering::push_ui_quad(renderer, quad_command);

        r32 ratio = t / (r32)num_samples;
        
        for(i32 i = 0; i < num_samples; i++)
        {
            math::Vec2 p = get_point_on_curve(curve, ratio * i) * scale;
            quad_command.transform.position = math::Vec2(base_x, base_y) + p;
            quad_command.transform.scale = math::Vec2(1.0f);
            quad_command.color = COLOR_BLACK;
            rendering::push_ui_quad(renderer, quad_command);            
        }
    }

}
