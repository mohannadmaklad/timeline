#include "utility.hpp"

namespace window::utility
{
    void initNcurses(WINDOW *win)
    {
        initscr();
        clear();
        // Hide the cursor
        curs_set(0);
        if (start_color() != OK)
        {
            std::cout << "Terminal doesn't support colors!";
        }
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        std::cout << "NcursesWrapper constructed\n";
        keypad(stdscr, TRUE);
        if (mousemask(ALL_MOUSE_EVENTS, NULL) == OK)
        {
            std::cout << "Mouse position events are supported.\n";
        }
        else
        {
            std::cout << "Mouse position events are not supported.\n";
        }

        // change background and draw box
        init_pair(1, COLOR_BLACK, COLOR_GREEN);
        wbkgd(win, COLOR_PAIR(1));
        box(win, 0, 0);
    }

    void deinitNcurses()
    {
        endwin();
    }
}