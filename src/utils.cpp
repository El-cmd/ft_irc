#include "../include/utils.hpp"

std::string get_current_time()
{
    std::time_t now = std::time(NULL);
    std::tm* local_time = std::localtime(&now);

    std::ostringstream oss;
    oss << (local_time->tm_year + 1900) << "-"
        << (local_time->tm_mon + 1) << "-"
        << local_time->tm_mday << " "
        << local_time->tm_hour << ":"
        << local_time->tm_min << ":"
        << local_time->tm_sec;

    return oss.str();
}

void log_message(const std::string& message)
{
    std::string time_str = get_current_time();
    // Afficher le log dans la console
    std::cout << "["<< BLEU << time_str << REINIT << "] " << message << std::endl;
}

std::string trim_all(const std::string& str) 
{
    std::string result;

    for (std::string::const_iterator it = str.begin(); it != str.end(); ++it)
    {
        if (!std::isspace(*it)) 
            result += *it;
    }

    return result;
}

std::vector<std::string> parse_command(const std::string &message) {
    std::vector<std::string> parts;
    std::istringstream stream(message);
    std::string part;

    while (stream >> part) {
        if (part[0] == ':') {
            part.erase(0, 1);  // Supprimer le ':' du début
            std::string remaining;
            std::getline(stream, remaining);
            part += " " + remaining;
            parts.push_back(part);
            break;
        } else {
            parts.push_back(part);
        }
    }

    return parts;
}
