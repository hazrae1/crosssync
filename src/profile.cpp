#include "crosssync/profile.hpp"

#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace crosssync {
namespace {

std::vector<std::vector<std::string>> tokenize(const std::string& line) {
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> tokens;
    std::string token;
    bool quoted = false;
    bool started = false;
    auto finish_token = [&] {
        if (started) {
            tokens.push_back(token);
            token.clear();
            started = false;
        }
    };
    auto finish_command = [&] {
        finish_token();
        if (!tokens.empty()) {
            commands.push_back(tokens);
            tokens.clear();
        }
    };

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (quoted && c == '\\' && i + 1 < line.size() &&
            (line[i + 1] == '"' || line[i + 1] == '\\')) {
            token += line[++i];
        } else if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (!quoted && c == '/' && i + 1 < line.size() && line[i + 1] == '/') {
            break;
        } else if (!quoted && c == ';') {
            finish_command();
        } else if (!quoted && (c == ' ' || c == '\t' || c == '\r')) {
            finish_token();
        } else {
            token += c;
            started = true;
        }
    }
    if (quoted) {
        throw std::runtime_error("Unclosed quote in CFG input");
    }
    finish_command();
    return commands;
}

double number(const std::vector<std::string>& tokens, double min, double max) {
    if (tokens.size() != 2) {
        throw std::runtime_error(tokens.front() + " expects exactly one value");
    }
    std::istringstream value(tokens[1]);
    value.imbue(std::locale::classic());
    double result = 0.0;
    if (!(value >> result) || !value.eof() || !std::isfinite(result) || result < min || result > max) {
        throw std::runtime_error("Invalid or out-of-range value for " + tokens[0]);
    }
    return result;
}

std::string json_string(const std::string& input) {
    std::ostringstream out;
    out << '"';
    for (unsigned char c : input) {
        if (c == '"' || c == '\\') {
            out << '\\' << c;
        } else if (c < 0x20 || c >= 0x7f) {
            // CFG bytes are kept as escaped code points so the JSON stays valid ASCII.
            out << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
        } else {
            out << c;
        }
    }
    out << '"';
    return out.str();
}

} // namespace

Profile parse_config(std::istream& input) {
    Profile profile;
    std::string line;
    std::size_t line_number = 0;
    std::size_t total_bytes = 0;
    while (std::getline(input, line)) {
        ++line_number;
        total_bytes += line.size() + 1;
        if (total_bytes > 1024 * 1024) {
            throw std::runtime_error("CFG input exceeds the 1 MiB limit");
        }
        if (line_number == 1 && line.compare(0, 3, "\xef\xbb\xbf") == 0) {
            line.erase(0, 3);
        }
        try {
            for (const auto& tokens : tokenize(line)) {
                const auto& name = tokens.front();
                if (name == "sensitivity") {
                    const double value = number(tokens, 0.0, 100.0);
                    if (value == 0.0) {
                        throw std::runtime_error("Sensitivity must be greater than zero");
                    }
                    profile.sensitivity = value;
                } else if (name == "cl_crosshairsize") {
                    profile.crosshair_size = number(tokens, 0.0, 20.0);
                } else if (name == "cl_crosshairgap") {
                    profile.crosshair_gap = number(tokens, -20.0, 20.0);
                } else if (name == "cl_crosshairthickness") {
                    profile.crosshair_thickness = number(tokens, 0.0, 10.0);
                } else if (name == "cl_crosshaircolor_r" || name == "cl_crosshaircolor_g" ||
                           name == "cl_crosshaircolor_b") {
                    const double value = number(tokens, 0.0, 255.0);
                    if (value != std::floor(value)) {
                        throw std::runtime_error("RGB channels must be integers");
                    }
                    const std::size_t index = name.back() == 'r' ? 0 : (name.back() == 'g' ? 1 : 2);
                    profile.crosshair_rgb[index] = static_cast<int>(value);
                } else if (name == "bind") {
                    if (tokens.size() != 3 || tokens[1].empty() || tokens[2].empty()) {
                        throw std::runtime_error("bind expects a nonempty key and command");
                    }
                    profile.bindings[tokens[1]] = tokens[2];
                } else {
                    ++profile.ignored_commands;
                    continue;
                }
                ++profile.supported_commands;
            }
        } catch (const std::exception& error) {
            throw std::runtime_error("Line " + std::to_string(line_number) + ": " + error.what());
        }
    }
    if (input.bad()) {
        throw std::runtime_error("Unable to read CFG input");
    }
    if (!profile.sensitivity) {
        throw std::runtime_error("CFG must contain a sensitivity value");
    }
    return profile;
}

std::string rgb_hex(const Profile& profile) {
    std::ostringstream out;
    out << '#' << std::hex << std::uppercase << std::setfill('0');
    for (int channel : profile.crosshair_rgb) {
        out << std::setw(2) << channel;
    }
    return out.str();
}

std::string demo_json(const Profile& profile) {
    if (!profile.sensitivity) {
        throw std::runtime_error("Profile is missing sensitivity");
    }
    std::ostringstream out;
    out.imbue(std::locale::classic());
    out << std::fixed << std::setprecision(4);
    out << "{\n  \"schema\": \"crosssync.demo/v1\",\n"
        << "  \"generator\": \"CrossSync " << version << "\",\n"
        << "  \"simulation\": true,\n"
        << "  \"applied_to_game\": false,\n"
        << "  \"source\": \"Counter-Strike CFG\",\n"
        << "  \"target\": \"VALORANT mock profile\",\n"
        << "  \"notice\": \"Joke project. Fictional calibration; not a VALORANT import format.\",\n"
        << "  \"mouse\": {\n    \"source_sensitivity\": " << *profile.sensitivity << ",\n"
        << "    \"preview_sensitivity\": " << *profile.sensitivity / demo_sensitivity_divisor << ",\n"
        << "    \"demo_divisor\": " << demo_sensitivity_divisor << "\n  },\n"
        << "  \"crosshair_preview\": {\n    \"color\": " << json_string(rgb_hex(profile)) << ",\n"
        << "    \"source_size\": " << profile.crosshair_size << ",\n"
        << "    \"source_gap\": " << profile.crosshair_gap << ",\n"
        << "    \"source_thickness\": " << profile.crosshair_thickness << "\n  },\n"
        << "  \"source_bindings\": {";
    std::size_t index = 0;
    for (const auto& [key, command] : profile.bindings) {
        out << (index++ == 0 ? "\n" : ",\n") << "    " << json_string(key) << ": " << json_string(command);
    }
    out << (profile.bindings.empty() ? "" : "\n  ") << "},\n"
        << "  \"diagnostics\": {\n    \"supported_commands\": " << profile.supported_commands << ",\n"
        << "    \"ignored_commands\": " << profile.ignored_commands << "\n  }\n}\n";
    return out.str();
}

std::string builtin_config() {
    return "sensitivity 1.6\ncl_crosshaircolor_r 50\ncl_crosshaircolor_g 255\n"
           "cl_crosshaircolor_b 160\ncl_crosshairsize 2.5\ncl_crosshairgap -2\n"
           "cl_crosshairthickness 0.5\nbind MOUSE1 +attack\nbind SPACE +jump\n";
}

} // namespace crosssync
