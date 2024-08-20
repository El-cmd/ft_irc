#include "../include/log.hpp"

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