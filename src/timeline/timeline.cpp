#include "timeline.hpp"
#include <thread>
#include <iomanip>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <algorithm>

constexpr const int BASE_LEVEL_OFFSET = 0;
constexpr const int BARS_LEVEL_OFFSET = 0;
constexpr const int TIME_POINTS_LEVEL_OFFSET = 1;

void Timeline::drawBase()
{
    // read configuration file
    getConfig(_config);

    // resize window
    _window->setHeight(_config.window_h);
    _window->setWidth(_config.window_w);

    int resolution_factor;

    switch (_resolution)
    {
    case TIMELINE_RESOLUTION::mSECONDS:
    {
        resolution_factor = 1000;
        break;
    }
    case TIMELINE_RESOLUTION::SECONDS:
    default:
        resolution_factor = 1;
        break;
    }

    uint32_t len = _config.layout == LAYOUT::HORIZONTAL ? _config.window_w : _config.window_h;

    if (_config.layout == LAYOUT::HORIZONTAL)
    {
        for (int i = 0; i < len - 1; i++)
        {
            _window->printString("-", {i, _config.base_level + BASE_LEVEL_OFFSET});
        }
    }
    else
    {
        for (int i = 0; i < len - 1; i++)
        {
            _window->printString("|", {_config.base_level + BASE_LEVEL_OFFSET, i});
        }
    }

    // print the vertical bars and time data
    for (int i = 0; i < len - 1; i++)
    {
        int bar_x, bar_y, ts_x, ts_y;
        std::string bar;

        if (_config.layout == LAYOUT::HORIZONTAL)
        {
            bar_x = i * _config.space;
            bar_y = _config.base_level + BARS_LEVEL_OFFSET;
            ts_x = i * _config.space;
            ts_y = _config.base_level + TIME_POINTS_LEVEL_OFFSET;
            bar = "|";
        }
        else
        {
            bar_y = i * _config.space;
            bar_x = _config.base_level + BARS_LEVEL_OFFSET;
            ts_y = i * _config.space;
            ts_x = _config.base_level + TIME_POINTS_LEVEL_OFFSET + 2;
            bar = "-";
        }

        auto nextTp = _start_tp + std::chrono::seconds((int)(i * _config.delta) / resolution_factor);

        _window->printString(bar, {bar_x, bar_y});
        _window->printString(toString(nextTp), {ts_x, ts_y});
    }
}

void Timeline::operator<<(DataPoint value)
{
    auto arrow = getArrow(value.first, _config.layout, _config.data_position);
    auto arrow_md = getArrowMetaData(value.first, _config.layout, _config.data_position);
    auto data = value.second;
    auto data_md = getDataMetaData(value.first, _config.layout, _config.data_position);

    //  draw arrow
    _window->printString(arrow, {arrow_md.position.first, arrow_md.position.second}, arrow_md.ts);
    // draw data
    _window->changeColor(value.second, {data_md.position.first, data_md.position.second}, data_md.ts);
}

void Timeline::getConfig(TimelineConfig &cfg)
{
    std::ifstream cfg_file("./config.json");
    nlohmann::json cfg_json = nlohmann::json::parse(cfg_file);
    // x-axis type
    std::string x_axis = cfg_json.value("x-axis", "Steps");
    if (x_axis == "Steps")
    {
        cfg.format = X_AXIS_FORMAT::STEPS;
    }

    // starting point
    cfg.starting_point = cfg_json.value("start", 1);
    _start_tp = std::chrono::high_resolution_clock::time_point{std::chrono::seconds(_config.starting_point)};

    // delta
    cfg.delta = cfg_json.value("delta", 1.0);

    // space
    cfg.space = cfg_json.value("space", 20);

    // arrow length
    cfg.up_arrow_length = cfg_json.value("up-arrow-length", 2);
    cfg.down_arrow_length = cfg_json.value("down-arrow-length", 2);
    cfg.base_level = cfg_json.value("base-level", 5);

    // data position
    auto pos = cfg_json.value("data-position", "auto");
    if (pos == "up")
    {
        cfg.data_position = POSITION::UP;
    }
    else
    {
        cfg.data_position = POSITION::DOWN;
    }

    cfg.upper_data_offset = cfg_json.value("+ve-data-offset", 3);
    cfg.auto_decrease_upper_data_level_by = cfg_json.value("auto-decrease-upper-data-level-by", 0);

    cfg.lower_data_offset = cfg_json.value("-ve-data-offset", 3);
    cfg.auto_decrease_lower_data_level_by = cfg_json.value("auto-decrease-lower-data-level-by", 0);

    cfg.auto_change_direction = cfg_json.value("auto-change-direction", false);
    auto layout = cfg_json.value("layout", "h");

    if (layout == "h")
        cfg.layout = LAYOUT::HORIZONTAL;
    else
        cfg.layout = LAYOUT::VERTICAL;

    // diemnsions
    cfg.window_h = cfg_json.value("window-h", 100);
    cfg.window_w = cfg_json.value("window-w", 100);

    // arrow style
    auto style = cfg_json.value("arrow-style", "h");
    if (style == "h")
        cfg.arrow_style == ARROW_STYLE::HORIZONTAL;
    else if (style == "v")
        cfg.arrow_style = ARROW_STYLE::VERTICAL;
    else
        cfg.arrow_style = ARROW_STYLE::DIAGONAL;

    cfg.arrow_head = cfg_json.value("arrow-head", "<")[0];
    cfg.arrow_shape = cfg_json.value("arrow-shape", "*")[0];
}

std::string Timeline::toString(TP tp)
{
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    auto t = tp - std::chrono::high_resolution_clock::time_point{std::chrono::seconds(0)};
    auto t_s = std::chrono::duration_cast<std::chrono::seconds>(t).count();
    return std::to_string(t_s);
}

std::optional<int> Timeline::getX(TP tp)
{
    auto diff = tp - _start_tp;
    int x, offset;
    // check resolution
    switch (_resolution)
    {
    case TIMELINE_RESOLUTION::SECONDS:
        offset = std::chrono::duration_cast<std::chrono::seconds>(diff).count() * _config.space;
        x = (int)((double)offset / (_config.delta));
        break;

    case TIMELINE_RESOLUTION::mSECONDS:
        offset = std::chrono::duration_cast<std::chrono::microseconds>(diff).count() * _config.space;
        x = (int)((double)offset / (_config.delta * 1000));
        break;

    default:
        return std::nullopt;
        break;
    }

    return x;
}

std::string Timeline::getArrow(TP tp, LAYOUT layout, POSITION pos)
{
    auto x = getX(tp).value();
    std::string arrow(_config.down_arrow_length, _config.arrow_shape);

    if (layout == LAYOUT::HORIZONTAL && pos == POSITION::UP)
    {
        arrow[0] = _config.arrow_head;
    }
    else if (layout == LAYOUT::HORIZONTAL && pos == POSITION::DOWN)
    {
        arrow[0] = _config.arrow_head;
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::UP)
    {
        arrow[arrow.size() - 1] = _config.arrow_head;
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::DOWN)
    {
        arrow[0] = _config.arrow_head;
    }

    return arrow;
}

TimelineElement Timeline::getArrowMetaData(TP tp, LAYOUT layout, POSITION pos)
{
    TimelineElement te;

    if (layout == LAYOUT::HORIZONTAL && pos == POSITION::UP)
    {
        te.position.first = getX(tp).value();
        te.position.second = _config.base_level + 1;
        switch (_config.arrow_style)
        {
        case ARROW_STYLE::HORIZONTAL:
            te.ts = VERTICAL_270;
            break;
        case ARROW_STYLE::VERTICAL:
            te.ts = VERTICAL_270;
            break;
        default:
            te.ts = DIAGONAL_NEG_60;
        }
    }
    else if (layout == LAYOUT::HORIZONTAL && pos == POSITION::DOWN)
    {
        te.position.first = getX(tp).value();
        te.position.second = _config.base_level + 1;
        switch (_config.arrow_style)
        {
        case ARROW_STYLE::HORIZONTAL:
            te.ts = VERTICAL_270;
            break;
        case ARROW_STYLE::VERTICAL:
            te.ts = VERTICAL_270;
            break;
        default:
            te.ts = DIAGONAL_NEG_60;
        }
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::UP)
    {
        te.position.first = getX(tp).value();
        te.position.second = _config.base_level + 1;
        switch (_config.arrow_style)
        {
        case ARROW_STYLE::HORIZONTAL:
            te.ts = HORIZONTAL;
            break;
        case ARROW_STYLE::VERTICAL:
            te.ts = VERTICAL_270;
            break;

        default:
            te.ts = DIAGONAL_NEG_60;
        }
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::DOWN)
    {

        te.position.first = _config.base_level + 1;
        te.position.second = getX(tp).value();

        switch (_config.arrow_style)
        {
        case ARROW_STYLE::HORIZONTAL:
            te.ts = HORIZONTAL;
            break;
        case ARROW_STYLE::VERTICAL:
            te.ts = HORIZONTAL;
            break;

        default:
            te.ts = DIAGONAL_NEG_60;
        }
    }

    return te;
}

TimelineElement Timeline::getDataMetaData(TP tp, LAYOUT layout, POSITION pos)
{
    TimelineElement te;

    if (layout == LAYOUT::HORIZONTAL && pos == POSITION::UP)
    {
        te.position.first = getX(tp).value() - _config.up_arrow_length;
        te.position.second = _config.base_level - _config.upper_data_offset;
        te.ts = HORIZONTAL;
    }
    else if (layout == LAYOUT::HORIZONTAL && pos == POSITION::DOWN)
    {
        te.position.first = getX(tp).value() + _config.down_arrow_length;
        te.position.second = _config.base_level + _config.lower_data_offset;
        te.ts = HORIZONTAL;
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::UP)
    {
        te.position.first = _config.base_level - _config.upper_data_offset;
        te.position.second = getX(tp).value();
        te.ts = HORIZONTAL;
    }
    else if (layout == LAYOUT::VERTICAL && pos == POSITION::DOWN)
    {
        te.position.first = _config.base_level + _config.lower_data_offset;
        te.position.second = getX(tp).value();
        te.ts = HORIZONTAL;
    }

    return te;
}
