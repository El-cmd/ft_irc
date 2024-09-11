#include "../include/utils.hpp"

std::string get_current_time()
{
    std::time_t now = std::time(NULL);
    std::tm* local_time = std::localtime(&now);

    std::ostringstream oss;
    oss << BLEU << (local_time->tm_year + 1900) << "-"
        << (local_time->tm_mon + 1) << "-"
        << local_time->tm_mday << " "
        << local_time->tm_hour << ":"
        << local_time->tm_min << ":"
        << local_time->tm_sec << REINIT;

    return oss.str();
}

void log_message(const std::string& message)
{
    std::string time_str = get_current_time();
    // Afficher le log dans la console
    std::cout << "["<< BLEU << time_str << REINIT << "] " << message << std::endl;
}

void log_message_client(int fd, const std::string &message)
{
    std::string time_str = get_current_time();
    std::string to_send = "[" + time_str + "] " + message + "\r\n";
    ssize_t bytes_sent = send(fd, to_send.c_str(), to_send.size(), 0);
    if (bytes_sent == -1)
        throw std::runtime_error("Error sending message to client");
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

bool verifOptionMode(std::string param)
{
    int i = 0;
    if (param[0] != '+' && param[0] != '-')
        return false;
    while (param[i])
    {
        if (param[i] != '+' && param[i] != '-' && param[i] != 'i' && param[i] != 't' && param[i] != 'k' && param[i] != 'o' && param[i] != 'l')
            return false;
        i++;
    }
    return true;
}

std::stack<char> initQueue(std::string param)
{
    int i = param.length() - 1;
    std::stack<char> _stack;
    while (i >= 0)
    {
        _stack.push(param[i]);
        i--;
    }
    return _stack;
}

bool isValidNumber(const std::string& str)
{
    char* endptr;
    errno = 0;
    long val = strtol(str.c_str(), &endptr, 10);
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN)
        return false;
    if (*endptr != '\0')
        return false;
    return true;
}

void sendQuitMessageToAllChannels(client* sender, const std::vector<std::string> params)
{
    std::string fullQuitMessage = sender->getNick() + " disconnected from the server";
    if (!params.empty())
    {
        fullQuitMessage += " :";
        for (std::vector<std::string>::const_iterator messIt = params.begin(); messIt != params.end(); ++messIt)
        {
            fullQuitMessage += *messIt;
            if (messIt + 1 != params.end())  // Éviter un espace à la fin
                fullQuitMessage += " ";
        }
    }
    std::set<client*> notifiedClients;
    std::vector<Channel*>::iterator chanIt = sender->getChan().begin();
    while (chanIt != sender->getChan().end())
    {
        Channel* channel = *chanIt;
        std::vector<int> clientsToRemove;
        std::map<int, client*>::iterator clientIt = channel->getClientsInChan().begin();
        while (clientIt != channel->getClientsInChan().end())
        {
            client* recipient = clientIt->second;
            if (recipient != sender && notifiedClients.find(recipient) == notifiedClients.end())
            {
                log_message_client(recipient->getFd(), fullQuitMessage);
                notifiedClients.insert(recipient);
            }
            if (recipient == sender)
                clientsToRemove.push_back(clientIt->first);
            ++clientIt;
        }
        for (std::vector<int>::iterator it = clientsToRemove.begin(); it != clientsToRemove.end(); ++it)
            channel->getClientsInChan().erase(*it);
        ++chanIt;
    }
    sender->getChan().clear();
}
