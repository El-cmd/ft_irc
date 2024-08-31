#include "../include/Command.hpp"

Command::Command()
{
    _commands["NICK"] = &Command::Nick;
    _commands["PASS"] = &Command::Pass;
    _commands["USER"] = &Command::User;
    _commands["TOPIC"] = &Command::Topic;
}

Command::~Command()
{
}

void Command::execute(const std::string &command, client *sender, Server *tmp)
{
    std::vector<std::string> parts = parse_command(command);
    if (parts.empty())
        return;
    std::string cmd_name = parts[0];
    parts.erase(parts.begin()); // Supprimez le nom de la commande de la liste des paramètres
    std::map<std::string, CommandHandler>::iterator it = _commands.find(cmd_name);
    if (it != _commands.end())
    {
        CommandHandler handler = it->second;
        (this->*handler)(parts, sender, tmp);
    } 
    else
        log_message_client(sender->getFd(), "Commande inconnue: " + cmd_name);
}

/* +++ Les commandes IRC +++ */
void Command::User(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    (void) tmp;
    if (!params[0].empty() && !params[1].empty())
    {
        sender->setUsername(params[0]);
        sender->setRealname(params[1]);
    }
    else
    {
        log_message_client(sender->getFd(), "Commande inconnue: Pas assez d'arguments");
        return ;
    }
    if (sender->getAuth() && !sender->getRealname().empty() && !sender->getUser().empty() && !sender->getNick().empty())
    {
        log_message(sender->getNick() + " is authentified");
        log_message_client(sender->getFd(), sender->getNick() + " is authentified");
    }
}

void Command::Pass(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (params.size() != 1)
    {
        log_message_client(sender->getFd(), " Commande inconnue: Pas assez d'arguments");
        return ;
    }
    if (tmp->auth(params[0]))
        sender->setAuth();
    if (sender->getAuth() && !sender->getRealname().empty() && !sender->getUser().empty() && !sender->getNick().empty())
    {
        log_message(sender->getNick() + " is authentified");
        log_message_client(sender->getFd(), sender->getNick() + " is authentified");
    }
}

void Command::Nick(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    (void) tmp;
    if (params.size() != 1)
    {
        log_message_client(sender->getFd(), "Commande inconnue: Pas assez d'arguments");
        return; //message d'erreur;
    }
    sender->setNick(params[0]);
    log_message_client(sender->getFd(), sender->getNick() + " is your new nickname.");
    if (sender->getAuth() && !sender->getRealname().empty() && !sender->getUser().empty() && !sender->getNick().empty())
    {
        log_message(sender->getNick() + " is authentified");
        log_message_client(sender->getFd(), sender->getNick() + " is authentified");
    }
}

void Command::Join(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    std::vector<std::string>::const_iterator it = params.begin();
    if (params.empty())
        return ; // message d'erreur
    Channel *chan;
    while (it != params.end())
    {
        if (!tmp->channelAlreadyExist(*it))
        {
            chan = new Channel(*it, sender, tmp);
            it++;
            break;
        }
        else
            chan = tmp->findChan(*it);
        if (tmp->channelAlreadyExist(*it) && !chan->withKey())
            chan->addClient(sender);
        //rajouter pour les channel avec des mdp
        it++;
    }
}

void Topic(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (params.empty())
        return ;
    if (!tmp->channelAlreadyExist(params[0]))
    {
        log_message_client(sender->getFd())
    }
}

/* +++++++++++++++++++++++++++++ */