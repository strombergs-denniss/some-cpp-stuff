#include "utility.hpp"

std::string exec(const char* cmd) {
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("Failed to execute command");
    }

    std::array<char, 128> buffer;
    std::string result;

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

std::string selectFile(bool mode, const std::string &filter) {
    std::string command = "zenity --file-selection ";
    command += mode ? "--save " : "";
    command += !filter.empty() ? "--file-filter=\"" + filter + "\"" : "";

    return exec(command.c_str());
}
