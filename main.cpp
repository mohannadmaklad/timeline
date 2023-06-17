#include <iostream>
#include <ncurses.h>
#include <thread>
#include <chrono>
#include <string>
#include <vector>
#include <timeline.hpp>
#include <sstream>
#define TAKE_A_NAP(D) std::this_thread::sleep_for(std::chrono::milliseconds(D))
#include <parser.hpp>

int main()
{
    initscr();
    clear();
    noecho();
    curs_set(0);
    cbreak();
    keypad(stdscr, 1);
    start_color();

    Parser p = Parser{"./data.txt", {" , ", 0}, {" , ", 7}};

    auto win = std::make_shared<ScrollablWindow>(Position{0, 0}, 100, 100);
    auto legend = std::make_shared<ScrollablWindow>(Position{100, 16}, 40, 4);

    Timeline timeline{win};
    timeline._resolution = TIMELINE_RESOLUTION::SECONDS;
    timeline.drawBase();

    auto dp = p.getNextDataPoint();
    while (dp)
    {
        timeline << dp.value();
        dp = p.getNextDataPoint();
    }

    std::thread t{&ScrollablWindow::listenToUserInput, std::static_pointer_cast<ScrollablWindow>(win)};
    t.join();

    // getch();

    // // delwin(win);
    // endwin();

    return 0;
}
