
struct animation
{
    uint32 FrameRate;
    
};


struct animation_manager
{
    union
    {
        animation Animation[16];
        struct
        {
        };
    };
};

