#pragma once

#include <panel.h>
#include <iostream>

namespace window::utility
{
    void initNcurses(WINDOW *win);
    void deinitNcurses();

}