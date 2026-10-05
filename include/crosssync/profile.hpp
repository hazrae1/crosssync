#pragma once

#include <array>
#include <cstddef>
#include <istream>
#include <map>
#include <optional>
#include <string>

namespace crosssync {

inline constexpr const char* version = "1.1.0";

struct Profile {
    std::optional<double> sensitivity;
    std::array<int, 3> crosshair_rgb{50, 255, 160};
    std::map<std::string, double> crosshair_parameters;
    std::map<std::string, double> source_only_settings;
    std::map<std::string, std::string> bindings;
    std::size_t supported_commands = 0;
    std::size_t ignored_commands = 0;
};

Profile parse_config(std::istream& input);
std::string profile_json(const Profile& profile);
std::string rgb_hex(const Profile& profile);
std::string builtin_config();

} // namespace crosssync
