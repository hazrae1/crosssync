#include "crosssync/profile.hpp"

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

namespace {

struct Options {
    std::filesystem::path input;
    std::filesystem::path output = "output/valorant-profile.json";
    bool dry_run = false;
    bool fast = false;
    bool color = true;
};

bool terminal_colors() {
    if (std::getenv("NO_COLOR") != nullptr) return false;
#ifdef _WIN32
    if (!_isatty(_fileno(stdout))) return false;
    const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    return GetConsoleMode(handle, &mode) && SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#else
    return isatty(STDOUT_FILENO) != 0;
#endif
}

void help() {
    std::cout << "CrossSync " << crosssync::version << " | CS2 -> VALORANT configuration bridge\n"
              << "Local CFG import and configuration profile export.\n\n"
              << "Usage: crosssync [options]\n\n"
              << "  --input <file.cfg>        Read a CFG (built-in sample by default)\n"
              << "  --output <file.json>      Save profile (default: output/valorant-profile.json)\n"
              << "  --dry-run                 Preview without creating a file\n"
              << "  --fast                    Skip presentation delays\n"
              << "  --no-color                Plain terminal output\n"
              << "  --help                    Show this help\n"
              << "  --version                 Show version\n";
}

Options options(int argc, char** argv) {
    Options result;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--input" || arg == "--output") {
            if (i + 1 == argc || std::string(argv[i + 1]).rfind("--", 0) == 0) {
                throw std::runtime_error("Missing value for " + arg);
            }
            const std::filesystem::path path = argv[++i];
            if (path.empty()) throw std::runtime_error("Empty path for " + arg);
            if (arg == "--input") result.input = path;
            else result.output = path;
        } else if (arg == "--dry-run") result.dry_run = true;
        else if (arg == "--fast") result.fast = true;
        else if (arg == "--no-color") result.color = false;
        else throw std::runtime_error("Unknown option: " + arg + " (use --help)");
    }
    const auto filename = result.output.filename().string();
    constexpr const char* suffix = ".json";
    if (filename.size() < 5 || filename.compare(filename.size() - 5, 5, suffix) != 0) {
        throw std::runtime_error("Output must end in .json");
    }
    if (!result.input.empty() && std::filesystem::exists(result.output) &&
        std::filesystem::equivalent(result.input, result.output)) {
        throw std::runtime_error("Input and output must be different files");
    }
    result.color = result.color && terminal_colors();
    return result;
}

void stage(const Options& opts, int percent, const std::string& label) {
    const auto completed = static_cast<std::size_t>(percent / 5);
    std::cout << "  " << (opts.color ? "\033[38;2;72;234;181m" : "") << "["
              << std::string(completed, '=') << std::string(20 - completed, '.') << "] "
              << std::setw(3) << percent << "% " << (opts.color ? "\033[0m" : "") << label << '\n';
    std::cout.flush();
    if (!opts.fast) std::this_thread::sleep_for(std::chrono::milliseconds(240));
}

void write_report(const std::filesystem::path& path, const std::string& content) {
    if (!path.parent_path().empty()) std::filesystem::create_directories(path.parent_path());
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output) throw std::runtime_error("Cannot open output: " + path.string());
    output << content;
    output.close();
    if (!output) throw std::runtime_error("Cannot write output: " + path.string());
}

} // namespace

int main(int argc, char** argv) {
    try {
        // Help and version do not create output or start a session.
        for (int i = 1; i < argc; ++i) {
            if (std::string(argv[i]) == "--help") { help(); return 0; }
            if (std::string(argv[i]) == "--version") { std::cout << crosssync::version << '\n'; return 0; }
        }
        const auto opts = options(argc, argv);
        std::cout << '\n' << (opts.color ? "\033[38;2;255;82;101m" : "")
                  << "   +------------------------------------------------------+\n"
                  << "   |  C R O S S S Y N C                         v" << crosssync::version << "    |\n"
                  << "   |  CS2  ->  VALORANT          CONFIGURATION BRIDGE     |\n"
                  << "   +------------------------------------------------------+\n"
                  << (opts.color ? "\033[0m" : "")
                  << "\n  SESSION   LOCAL / CONFIGURATION EXPORT\n"
                  << "  SOURCE    " << (opts.input.empty() ? "built-in sample" : opts.input.string()) << "\n"
                  << "  TARGET    " << (opts.dry_run ? "preview only" : opts.output.string()) << "\n\n";
        stage(opts, 10, "Initialize migration workspace");
        crosssync::Profile profile;
        if (opts.input.empty()) {
            std::istringstream sample(crosssync::builtin_config());
            profile = crosssync::parse_config(sample);
        } else {
            if (!std::filesystem::is_regular_file(opts.input)) {
                throw std::runtime_error("Input must be a regular CFG file");
            }
            if (std::filesystem::file_size(opts.input) > 1024 * 1024) {
                throw std::runtime_error("CFG input exceeds the 1 MiB limit");
            }
            std::ifstream input(opts.input, std::ios::binary);
            if (!input) throw std::runtime_error("Cannot open CFG input");
            profile = crosssync::parse_config(input);
        }
        stage(opts, 30, "Parse source configuration");
        stage(opts, 55, "Collect input and crosshair parameters");
        stage(opts, 75, "Index source-specific settings and keybinds");
        const auto report = crosssync::profile_json(profile);
        stage(opts, 90, "Serialize configuration profile");
        if (!opts.dry_run) write_report(opts.output, report);
        stage(opts, 100, opts.dry_run ? "Preview ready" : "Configuration profile exported");
        std::cout << "\n  MIGRATION PREVIEW\n"
                  << "  --------------------------------------------------------\n"
                  << std::fixed << std::setprecision(4)
                  << "  Source sensitivity   " << *profile.sensitivity << "\n"
                  << "  Crosshair color      " << (crosssync::rgb_hex(profile).empty() ? "not specified" : crosssync::rgb_hex(profile)) << "\n"
                  << "  Source keybinds      " << profile.bindings.size() << " staged\n"
                  << "  CS-specific options  " << profile.source_only_settings.size() << " preserved\n"
                  << "  CFG commands         " << profile.supported_commands << " read / "
                  << profile.ignored_commands << " ignored\n\n"
                  << "  Configuration session complete.\n\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "CrossSync error: " << error.what() << '\n';
        return 1;
    }
}
