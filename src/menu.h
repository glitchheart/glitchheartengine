#ifndef MENU_H
#define MENU_H

struct main_menu
{
    uint32 SelectedIndex;
    char* Options[3] = 
    {
        "Start game",
        "Settings",
        "Exit"
    };
};

#endif


