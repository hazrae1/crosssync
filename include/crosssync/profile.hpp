#pragma once

#include <array>
#include <cstddef>
#include <istream>
#include <map>
#include <optional>
#include <string>

namespace crosssync {

inline constexpr const char* version = "1.0.0";
// Fictional calibration for the joke. This is not a verified game conversion.
inline constexpr double demo_sensitivity_divisor = 3.2;

struct Profile {
    std::optional<double> sensitivity;
    std::array<int, 3> crosshair_rgb{50, 255, 160};
    double crosshair_size = 2.5;
    double crosshair_gap = -2.0;
    double crosshair_thickness = 0.5;
    std::map<std::string, std::string> bindings;
    std::size_t supported_commands = 0;
    std::size_t ignored_commands = 0;
};

Profile parse_config(std::istream& input);
std::string demo_json(const Profile& profile);
std::string rgb_hex(const Profile& profile);
std::string builtin_config();

} // namespace crosssync
