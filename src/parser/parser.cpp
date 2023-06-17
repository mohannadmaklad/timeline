#include "parser.hpp"
#include <iostream>
#include <cassert>
#include <fplus/fplus.hpp>

Parser::Parser(std::string path, Clue timestamp_clue, Clue data_clue) : _timestamp_clue(timestamp_clue), _data_clue(data_clue)
{
    _fstream = std::ifstream(path);
}

std::optional<DataPoint> Parser::getNextDataPoint()
{
    std::string line;
    if (std::getline(_fstream, line).fail())
        return std::nullopt;

    auto dp = fplus::fwd::apply(line,
                                fplus::fwd::split_by_token(std::string(" , "), false),
                                fplus::fwd::keep_idxs(std::vector<std::size_t>{0, 7}));

    return std::make_pair(std::chrono::high_resolution_clock::time_point{std::chrono::seconds(std::stoi(dp[0]))}, dp[1]);
}
