#include "../include/Command.hpp"

Command::Command()
{
    _commands["NICK"] = &Command::Nick;
    _commands["PASS"] = &Command::Pass;
    _commands["USER"] = &Command::User;
    _commands["TOPIC"] = &Command::Topic;
    _commands["JOIN"] = &Command::Join;
    _commands["MODE"] = &Command::Mode;
    _commands["QUIT"] = &Command::Quit;
    _commands["INVITE"] = &Command::Invite;
    _commands["KICK"] = &Command::Kick;
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
        if (tmp->channelAlreadyExist(*it) && !chan->withKey() && !chan->getOnInvite())
            chan->addClient(sender);
        //rajouter pour les channel avec des mdp et sur invite
        it++;
    }
}

void Command::Topic(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    Channel *chan;
    if (params.empty())
        return ;
    if (!tmp->channelAlreadyExist(params[0]))
    {
        log_message_client(sender->getFd(), "This channel doesn't exist");
        return ;
    }
    else
    {
        chan = tmp->findChan(params[0]);
        if (chan->alreadyIn(sender))
        {
            if (params.size() > 1 && (!chan->getTopicRestricted() || (chan->getTopicRestricted() && chan->itsAnOp(sender))))
            {
                std::vector<std::string>::const_iterator it = params.begin();
                it++;
                std::string topic;
                while (it != params.end())
                {
                    topic += *it + " ";
                    it++;
                }
                chan->setTopic(topic);
                log_message_client(sender->getFd(), "You are set a new Topic in " + chan->getName() + " Channel"); 
            }
            else if (params.size() == 1)
                log_message_client(sender->getFd(), chan->getTopic());
        }
        else
            log_message_client(sender->getFd(), "You are not authorized to access this channel");
    }
}

void Command::Quit(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    log_message(sender->getNick() + " disconnected from the server");
    sendQuitMessageToAllChannels(sender, params);
    tmp->handleClientDeconnectionQUIT(sender);
    // ne pas oublier si le client est le seul operateur d'un channel
}

void Command::Kick(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (params.size() != 2)
    {
        log_message_client(sender->getFd(), "Not enough parameters for this command");
        return ;
    }
    if (!tmp->channelAlreadyExist(params[0]))
    {
        log_message_client(sender->getFd(), "This channel doesn't exist");
        return ;
    }
    Channel *chan = tmp->findChan(params[0]);
    if (!chan->itsAnOp(sender) || !chan->alreadyIn(sender))
    {
        log_message_client(sender->getFd(), "You are not authorized to kick someone in this channel");
        return ;
    }
    if (!tmp->clientExist(params[1]))
    {
        log_message_client(sender->getFd(), "This client " + params[1] + " doesn't exist");
        return ;
    }
    client *clientToKick = tmp->findClient(params[1]);
    if (!chan->alreadyIn(clientToKick))
    {
        log_message_client(sender->getFd(), "This client " + params[1] + " is not in this channel");
        return ;
    }
    chan->channelAllMessage(clientToKick->getNick() + " has been kick on this channel by " + sender->getNick());
    chan->removeClient(clientToKick);
}

void Command::Invite(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (params.size() != 2)
    {
        log_message_client(sender->getFd(), "Not enough parameters for this command");
        return ;
    }
    if (!tmp->channelAlreadyExist(params[1]))
    {
        log_message_client(sender->getFd(), "This channel doesn't exist");
        return ;
    }
    Channel *chan = tmp->findChan(params[1]);
    if (!chan->itsAnOp(sender) || !chan->alreadyIn(sender))
    {
        log_message_client(sender->getFd(), "You are not authorized to invite someone in this channel");
        return ;
    }
    if (!tmp->clientExist(params[0]))
    {
        log_message_client(sender->getFd(), "This client " + params[0] + " doesn't exist");
        return ;
    }
    client *clientToInvite = tmp->findClient(params[0]);
    if (chan->alreadyIn(clientToInvite))
    {
        log_message_client(sender->getFd(), "This client is already in this channel");
        return ;
    }
    log_message_client(clientToInvite->getFd(), sender->getNick() + " Invite " + clientToInvite->getNick() + " :" + chan->getName());
    clientToInvite->addNewInvite(chan);
    // ne pas oublier de faire un vector dans le client inviter pour avoir les channel dans lesquel il est invité
}
/* +++++++++++++++++++++++++++++ */


void Command::Mode(const std::vector<std::string>& params, client* sender, Server* tmp) 
{
    // Vérification des paramètres
    if (params.size() < 2)
    {
        log_message_client(sender->getFd(), "Error: Not enough parameters");
        return;
    }
    if (!tmp->channelAlreadyExist(params[0]))
    {
        log_message_client(sender->getFd(), "Error: No such channel");
        return;
    }
    Channel* chan = tmp->findChan(params[0]);
    if (!chan->alreadyIn(sender) && !chan->itsAnOp(sender))
    {
        log_message_client(sender->getFd(), "You are not authorized to access this channel");
        return;
    }
    if (!verifOptionMode(params[1]))
    {
        log_message_client(sender->getFd(), "Error: Invalid mode option");
        return;
    }
    std::stack<char> modeStack = initQueue(params[1]);
    bool addMode = true;
    int paramIndex = 2;
    while (!modeStack.empty())
    {
        char mode = modeStack.top();
        modeStack.pop();
        if (mode == '+')
            addMode = true;
        else if (mode == '-')
            addMode = false;
        else
            applyMode(chan, sender, mode, addMode, params, tmp, paramIndex);
    }
}

void Command::applyMode(Channel* chan, client* sender, char mode, bool addMode, const std::vector<std::string>& params, Server* tmp, int& paramIndex)
{
    switch (mode)
    {
        case 'i':
            handleInviteOnlyMode(chan, addMode);
            break;
        case 't':
            handleTopicRestrictionMode(chan, addMode);
            break;
        case 'k':
            handleChannelKeyMode(chan, sender, addMode, params, paramIndex);
            break;
        case 'o':
            handleOperatorMode(chan, sender, addMode, params, tmp, paramIndex);
            break;
        case 'l':
            handleUserLimitMode(chan, sender, addMode, params, paramIndex);
            break;
        default:
            log_message_client(sender->getFd(), "Error: Unknown mode");
            break;
    }
}

void Command::handleInviteOnlyMode(Channel* chan, bool addMode)
{
    if (addMode)
        chan->setInviteOnly(true);
    else
        chan->setInviteOnly(false);
}

void Command::handleTopicRestrictionMode(Channel* chan, bool addMode)
{
    if (addMode)
        chan->setTopicRestricted(true);
    else
        chan->setTopicRestricted(false);
}

void Command::handleChannelKeyMode(Channel* chan, client* sender, bool addMode, const std::vector<std::string>& params, int& paramIndex)
{
    if (addMode)
    {
        if (static_cast<std::vector<std::string>::size_type>(paramIndex) < params.size())
        {
            chan->setKey(params[paramIndex]);
            ++paramIndex;
        }
        else
            log_message_client(sender->getFd(), "Error: Missing key for +k mode");
    } 
    else
        chan->clearKey();
}

void Command::handleOperatorMode(Channel* chan, client* sender, bool addMode, const std::vector<std::string>& params, Server* tmp, int& paramIndex)
{
    client *target;
    if (static_cast<std::vector<std::string>::size_type>(paramIndex) < params.size())
    {
        target = tmp->findClient(params[paramIndex]);
        ++paramIndex;
        if (target == NULL)
        {
            log_message_client(sender->getFd(), "Error: Client not found for mode +o/-o");
            return ;  // Message d'erreur
        }
    }
    else
    {
        log_message_client(sender->getFd(), "Error: Missing name for +o/-o mode");
        return ;
    }
    if (addMode)
        chan->addOperator(target);
    else
        chan->removeOperator(target);
}

void Command::handleUserLimitMode(Channel* chan, client* sender, bool addMode, const std::vector<std::string>& params, int& paramIndex)
{
    if (addMode)
    {
        if (static_cast<std::vector<std::string>::size_type>(paramIndex) < params.size())
        {
            if (isValidNumber(params[paramIndex]))
                chan->setUserLimit(atoi(params[paramIndex].c_str()));  // Assurez-vous que params[2] existe
            else
                log_message_client(sender->getFd(), "Error: Invalid limit for +l mode");
            ++paramIndex;
        }
        else
            log_message_client(sender->getFd(), "Error: Missing number for +l mode");
    }
    else
        chan->clearUserLimit();
}
