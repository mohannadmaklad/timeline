#include <panel.h>
#include <utility>
#include <string>
#include "utility.hpp"

enum scrollDirection
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

enum TraverseStrategy
{
    HORIZONTAL,
    VERTICAL_90,
    VERTICAL_270,
    DIAGONAL_60,
    DIAGONAL_NEG_60, // Starts from top left and goes to bottom right
    DIAGONAL_120     // Starts from bottom right and goes to top left
};

using Position = std::pair<int, int>;

class ScrollablWindow
{
public:
    ScrollablWindow(const Position &position, const uint32_t width, const uint32_t height);
    ~ScrollablWindow();
    void setWidth(const uint32_t);
    void setHeight(const uint32_t);
    void resize(const uint32_t, const uint32_t);
    void print(const std::string);
    void moveWindow(const Position &new_position);
    void printString(const std::string s, const Position &position = {-1, -1}, TraverseStrategy strategy = HORIZONTAL);
    void changeColor(const std::string &s, const Position &position, TraverseStrategy strategy);
    void scrollWindow(scrollDirection direction, int32_t steps);
    void listenToUserInput();

private:
    void update();
    Position getPosition();

private:
    WINDOW *win;
    Position _start_pos;
    Position _position;
    uint32_t _height, _width;
    uint32_t _visible_width, _visible_height;
};