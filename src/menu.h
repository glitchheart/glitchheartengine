#ifndef MENU_H
#define MENU_H

struct main_menu
{
    int32 SelectedIndex;
    uint32 OptionCount = 3;
    
    char* Options[3] = 
    {
        "Continue",
        "Settings",
        "Exit"
    };
};

#endif


