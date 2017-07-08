#ifndef MENU_H
#define MENU_H

struct main_menu
{
    i32 SelectedIndex;
    u32 OptionCount = 3;
    
    char* Options[3] = 
    {
        "Continue",
        "Settings",
        "Exit"
    };
};

#endif


