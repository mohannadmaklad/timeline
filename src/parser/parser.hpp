#pragma once

#include <string>
#include <chrono>
#include <vector>
#include <fstream>
#include <optional>

using DataPoint = std::pair<std::chrono::high_resolution_clock::time_point, std::string>;
// using clue to be able to abstract differnt data layouts
using Clue = std::pair<std::string, int>; /* Delimeter and index */

class Parser
{
public:
    Parser(std::string path, Clue timestamp_clue, Clue data_clue);
    std::optional<DataPoint> getNextDataPoint();

private:
    Clue _timestamp_clue;
    Clue _data_clue;
    std::ifstream _fstream;
};