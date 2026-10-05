#include "crosssync/profile.hpp"

#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {
void require(bool value, const char* message) {
    if (!value) throw std::runtime_error(message);
}
crosssync::Profile parse(const std::string& text) {
    std::istringstream input(text);
    return crosssync::parse_config(input);
}
void rejects(const std::string& input) {
    bool rejected = false;
    try { (void)parse(input); } catch (const std::runtime_error&) { rejected = true; }
    require(rejected, "Invalid CFG was accepted");
}
} // namespace

int main() {
    try {
        const auto profile = parse(crosssync::builtin_config());
        require(profile.sensitivity && *profile.sensitivity == 1.6, "Wrong source sensitivity");
        require(profile.supported_commands == 13, "Wrong supported-command count");
        require(profile.bindings.size() == 2, "Bindings were lost");
        require(profile.source_only_settings.size() == 4, "CS-specific settings were lost");
        require(profile.source_only_settings.at("viewmodel_offset_z") == -2.0, "Viewmodel offset changed");
        require(profile.crosshair_parameters.at("cl_crosshair_length") == 8.0, "Crosshair parameter changed");
        require(crosssync::rgb_hex(profile) == "#32FFA0", "Wrong crosshair color");

        const auto quoted = parse("\xef\xbb\xbf // UTF-8 BOM\r\nsensitivity \"2\"; sensitivity 1.6 // last wins\n"
                                  "bind \"K\" \"say https://example.test; \\\"hi\\\"\"\n"
                                  "exec secret.cfg; quit; alias bad command\n");
        require(*quoted.sensitivity == 1.6, "Multiple commands or comments parsed incorrectly");
        require(quoted.bindings.at("K") == "say https://example.test; \"hi\"", "Quoted content changed");
        require(quoted.ignored_commands == 3, "Unsupported commands were not ignored");

        const auto json = crosssync::profile_json(quoted);
        require(json.find("\"schema\": \"crosssync.profile/v1\"") != std::string::npos, "Wrong schema");
        require(json.find("\"source_sensitivity\": 1.6000") != std::string::npos, "Source sensitivity changed");
        require(json.find("\"workflow\": \"manual_setup\"") != std::string::npos, "Missing workflow");
        require(json.find("\\\"hi\\\"") != std::string::npos, "JSON quotes were not escaped");
        const auto escaped = crosssync::profile_json(parse("sensitivity 1\nbind K \"say\t\\\\test\"\n"));
        require(escaped.find("\\u0009") != std::string::npos, "JSON control character was not escaped");
        require(escaped.find("\\\\test") != std::string::npos, "JSON backslash was not escaped");

        rejects("sensitivity -1\n");
        rejects("sensitivity 0\n");
        rejects("sensitivity NaN\n");
        rejects("sensitivity inf\n");
        rejects("sensitivity 1junk\n");
        rejects("sensitivity 1 2\n");
        rejects("sensitivity \"1\n");
        rejects("sensitivity 1\ncl_crosshaircolor_r 256\n");
        rejects("sensitivity 1\ncl_crosshaircolor_r 1.5\n");
        rejects("sensitivity 1\nviewmodel_fov 90\n");
        rejects("sensitivity 1\nviewmodel_offset_x 3\n");
        rejects("sensitivity 1\nviewmodel_offset_y -3\n");
        rejects("sensitivity 1\nbind K\n");
        rejects("sensitivity 1\nbind \"\" +jump\n");
        rejects("// missing sensitivity\n");
        rejects("sensitivity 1\n" + std::string(1024 * 1024, ' '));
        std::cout << "Profile parser, CS-specific settings, validation and JSON escaping: passed\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Test failure: " << error.what() << '\n';
        return 1;
    }
}
