#include "scrollableWindow.hpp"
#include <thread>
#include <algorithm>
#include <iostream>

ScrollablWindow::ScrollablWindow(const Position &position, const uint32_t width = -1, const uint32_t height = -1)
{
    _position = position;
    _width = width;
    _height = height;
    if (width == -1 || height == -1)
        getmaxyx(stdscr, _height, _width);

    //  create pad
    win = newpad(_height, _width);
    window::utility::initNcurses(win);

    int terminal_h, terminal_w;
    getmaxyx(stdscr, terminal_h, terminal_w);
    _visible_height = std::min((int)_height, terminal_h) - 1;
    _visible_width = std::min((int)_width, terminal_w) - 1;
    _start_pos = {0, 0};

    update();
}

ScrollablWindow::~ScrollablWindow()
{
    delwin(win);
}

void ScrollablWindow::setWidth(const uint32_t width)
{
    _width = width;
    resize(_width, _height);
}

void ScrollablWindow::setHeight(const uint32_t height)
{
    _height = height;
    resize(_width, _height);
}

void ScrollablWindow::resize(const uint32_t w, const uint32_t h)
{
    delwin(win);
    win = newpad(h, w);
    window::utility::initNcurses(win);
    update();
}

void ScrollablWindow::print(const std::string str)
{
    mvwprintw(win, 1, 1, "%s", str.c_str());
    update();
}

void ScrollablWindow::moveWindow(const Position &new_position)
{
    _position = new_position;
    update();
}

void ScrollablWindow::printString(const std::string s, const Position &position, TraverseStrategy strategy)
{
    bool use_position = (position.first == -1 && position.second == -1) ? false : true;

    int x, y;
    if (use_position)
    {
        x = position.first;
        y = position.second;
    }
    else
    {
        std::tie(x, y) = getPosition();
    }

    switch (strategy)
    {
    case HORIZONTAL:
        mvwprintw(win, y, x, s.c_str());
        break;

    case DIAGONAL_NEG_60:
    {
        auto str = s.c_str();
        for (int i = 0; i < s.length(); i++)
        {
            wmove(win, y, x);
            waddch(win, str[i]);
            x++;
            y++;
            wrefresh(win);
        }
        break;
    }
    case DIAGONAL_120:
    {
        auto str = s.c_str();
        for (int i = 0; i < s.length(); i++)
        {
            wmove(win, y, x);
            waddch(win, str[i]);
            x--;
            y--;
            wrefresh(win);
        }
        break;
    }
    case VERTICAL_90:
    {
        auto str = s.c_str();
        for (int i = 0; i < s.length(); i++)
        {
            wmove(win, y, x);
            waddch(win, str[i]);
            y--;
            wrefresh(win);
        }
        break;
    }
    case VERTICAL_270:
    {
        auto str = s.c_str();
        for (int i = 0; i < s.length(); i++)
        {
            wmove(win, y, x);
            waddch(win, str[i]);
            y++;
            wrefresh(win);
        }
        break;
    }
    }

    update();
}

void ScrollablWindow::changeColor(const std::string &s, const Position &position, TraverseStrategy strategy)
{
    attron(COLOR_PAIR(1));
    printString(s, position, strategy);
    attroff(COLOR_PAIR(1));
}

/* Private Methods */
Position ScrollablWindow::getPosition()
{
    int x, y;
    getyx(win, y, x);

    return {x, y};
}

void ScrollablWindow::update()
{
    refresh();
    prefresh(win, _start_pos.second, _start_pos.first, _position.second, _position.first, _visible_height, _visible_width);
}

void ScrollablWindow::scrollWindow(scrollDirection direction, int32_t steps)
{
    switch (direction)
    {
    case scrollDirection::RIGHT:
        if (_start_pos.first + _visible_width + steps <= _width)
            _start_pos.first += steps;
        else
            _start_pos.first += _width - (_start_pos.first + _visible_width + 1);

        break;

    case scrollDirection::LEFT:
        if (_start_pos.first - steps >= 0)
            _start_pos.first -= steps;
        else
            _start_pos.first = 0;
        break;

    case scrollDirection::UP:
        if (_start_pos.second + _visible_height + steps <= _height)
            _start_pos.second += steps;
        else

            _start_pos.second = _height - _visible_height - 1;
        break;

    case scrollDirection::DOWN:
        if (_start_pos.second - steps >= 0)
            _start_pos.second -= steps;
        else
            _start_pos.second = 0;
        break;

    default:
        break;
    }

    update();
}

void ScrollablWindow::listenToUserInput()
{
    auto key = 0;
    while (key != KEY_ENTER)
    {
        key = getch();
        if (key == KEY_RIGHT)
            scrollWindow(RIGHT, 5);
        else if (key == KEY_LEFT)
            scrollWindow(LEFT, 5);
        else if (key == KEY_UP)
            scrollWindow(DOWN, 5);
        else if (key == KEY_DOWN)
            scrollWindow(UP, 5);
    }
}
