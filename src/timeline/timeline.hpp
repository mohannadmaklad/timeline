#pragma once
#include <chrono>
#include <iostream>
#include <memory>
#include <scrollableWindow.hpp>
#include <optional>

using Duration = std::chrono::duration<unsigned int, std::ratio<1, 1>>;
using TP = std::chrono::high_resolution_clock::time_point;
using DataPoint = std::pair<std::chrono::high_resolution_clock::time_point, std::string>;

struct TimelineElement
{
    std::pair<int, int> position;
    TraverseStrategy ts;
};

enum class X_AXIS_FORMAT
{
    STEPS,
    SECONDS
};

enum class TIMELINE_RESOLUTION
{
    uSECONDS,
    mSECONDS,
    SECONDS,
    MINUTES,
    HOURS,
    AUTO = SECONDS
};

enum class POSITION
{
    UP,
    DOWN,
};

enum class LAYOUT
{
    HORIZONTAL,
    VERTICAL
};

enum class ARROW_STYLE
{
    HORIZONTAL,
    VERTICAL,
    DIAGONAL
};

struct TimelineConfig
{
    LAYOUT layout;
    X_AXIS_FORMAT format;
    // physical space on the screen
    uint16_t space;
    // the value of the first point
    uint32_t starting_point;
    double delta;
    uint8_t up_arrow_length;
    uint8_t down_arrow_length;
    ARROW_STYLE arrow_style;
    char arrow_head;
    char arrow_shape;
    uint8_t base_level;
    POSITION data_position;
    // relative to the timeline
    uint8_t upper_data_offset;
    uint8_t lower_data_offset;
    uint8_t auto_decrease_upper_data_level_by;
    uint8_t auto_decrease_lower_data_level_by;
    bool auto_change_direction;
    uint32_t window_h;
    uint32_t window_w;
};

class Timeline
{
public:
    Timeline(std::shared_ptr<ScrollablWindow> window) : _window(window),
                                                        _resolution(TIMELINE_RESOLUTION::AUTO) {}

    ~Timeline() = default;
    void drawBase();
    void operator<<(DataPoint value);

public:
    std::chrono::high_resolution_clock::time_point _start_tp;
    TIMELINE_RESOLUTION _resolution;
    TimelineConfig _config;

private:
    std::shared_ptr<ScrollablWindow> _window;
    void getConfig(TimelineConfig &cfg);
    std::string toString(TP tp);
    std::optional<int> getX(TP tp);
    std::string getArrow(TP tp, LAYOUT layout, POSITION pos);
    TimelineElement getArrowMetaData(TP tp, LAYOUT layout, POSITION pos);
    TimelineElement getDataMetaData(TP tp, LAYOUT layout, POSITION pos);
};