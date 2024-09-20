#include "../include/Command.hpp"

Command::Command()
{
    _commands["NICK"] = &Command::Nick;
    _commands["PASS"] = &Command::Pass;
    _commands["USER"] = &Command::User;
    _commands["TOPIC"] = &Command::Topic; //besoin du #
    _commands["JOIN"] = &Command::Join; //besoin du #
    _commands["MODE"] = &Command::Mode; //besoin du #
    _commands["QUIT"] = &Command::Quit;
    _commands["KICK"] = &Command::Kick; //besoin du #
    _commands["INVITE"] = &Command::Invite; //besoin du #
    _commands["PRIVMSG"] = &Command::PrivMsg; //besoin du #
}

Command::~Command()
{
}

void Command::execute(const std::string &command, client *sender, Server *tmp)
{
    std::vector<std::string> begin = trim_all(command);
    std::vector<std::string> parts;
    std::vector<std::string>::iterator itt = begin.begin();
    while (itt != begin.end())
    {
        parts = parse_command(*itt);
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
            sender->sendRpl(ERR_UNKNOWNCOMMAND(sender->getNick(), cmd_name));
        itt++;
    }
}

/* +++ Les commandes IRC +++ */

void Command::PrivMsg(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    int chanOrUser;
    std::string message;
    std::string chanstr;
    std::ostringstream oss;
    oss << "<" << VERT << sender->getNick() << REINIT << "> ";
    if (params.size() < 2)
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "PRIVMSG"));
        return ;
    }
    if (params[0][0] == '#')
    {
        if (!params[0][1])
        {
            return ; // mettre message d'erreur
        }
        chanOrUser = 1;
    }
    else
        chanOrUser = 0;
    if (chanOrUser)
    {
        if (!tmp->channelAlreadyExist(params[0]))
        {
            log_message_client(sender->getFd(), "This channel doesn't exist");
            return ;
        }
        Channel *chan = tmp->findChan(params[0]);
        if (!chan->alreadyIn(sender))
        {
            log_message_client(sender->getFd(), "Cannot send a message to a channel if you are not in");
            return ;
        }
        sendMessageToChannel(chan, joinStringsExcludingFirst(params), sender);

    }
    else
    {
        if (!tmp->clientExist(params[0]))
        {
            log_message_client(sender->getFd(), "This client " + params[0] + " doesn't exist");
            return ;
        }
        std::ostringstream oss;
        oss << "<" << VERT << sender->getNick() << REINIT << "> ";
        client *toSend = tmp->findClient(params[0]);
        log_message_client(toSend->getFd(), oss.str() + joinStringsExcludingFirst(params));
    }
}

void Command::User(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    (void) tmp;
    if (!sender->getAuth())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    if (!sender->getUser().empty() && !sender->getRealname().empty())
    {
        sender->sendRpl(ERR_ALREADYREGISTERED(sender->getNick()));
        return ;
    }
    if (params.size() == 4)
    {
        sender->setUsername(params[0]);
        sender->setRealname(params[3]);
    }
    else
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "USER"));
        return ;
    }
    if (sender->isRegister())
    {
        sender->sendRpl(RPL_WELCOME(sender->getNick()));
        log_message(sender->getNick() + " has authenticated");
    }
}

void Command::Pass(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (sender->isRegister())
    {
        sender->sendRpl(ERR_ALREADYREGISTERED(sender->getNick()));
        return;
    }
    if (params.size() != 1)
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "PASS"));
        return;
    }

    if (!tmp->auth(params[0]))
    {
        sender->sendRpl(ERR_PASSWDMISMATCH(sender->getNick()));
        return;
    }
    sender->setAuth();
}

void Command::Nick(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->getAuth())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    if (params.size() != 1)
    {
        sender->sendRpl(ERR_NONICKNAMEGIVEN(sender->getNick()));
        return;
    }
    if (!tmp->isValidNick(params[0]))
    {
        sender->sendRpl(ERR_NICKNAMEINUSE(params[0]));
        return ;
    }
    if (!sender->getNick().empty())
        log_message(sender->getNick() + " changed nickname with " + params[0]);
    sender->setNick(params[0]);
    if (sender->isRegister())
    {
        sender->sendRpl(RPL_WELCOME(sender->getNick()));
        log_message(sender->getNick() + " has authenticated");
    }
}

void Command::Join(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    if (params.empty())
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "JOIN"));
        return ;
    }
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    std::stringstream ss(params[0]);
    std::string channel;
    Channel* chan;
    while (std::getline(ss, channel, ','))
        channels.push_back(channel);
    if (params.size() > 1)
    {
        std::stringstream keyStream(params[1]);
        std::string key;
        while (std::getline(keyStream, key, ','))
            keys.push_back(key);
    }
    size_t keyIndex = 0;
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string current_channel = channels[i];
        std::string current_key = "";

        if (!tmp->channelAlreadyExist(current_channel))
        {
            chan = new Channel(current_channel, sender, tmp);
            tmp->newChannel(chan);
            sender->sendRpl(RPL_NOTOPIC(sender->getNick(), current_channel));
            sender->sendRpl(RPL_NAMREPLY(sender->getNick(), current_channel, chan->getUserList()));
            sender->sendRpl(RPL_ENDOFNAMES(sender->getNick(), current_channel));
            continue;
        }
        else
            chan = tmp->findChan(current_channel);
        if (chan->alreadyIn(sender))
        {
            sender->sendRpl(ERR_USERONCHANNEL(sender->getNick(), sender->getUser(), current_channel));
            continue;
;       }
        if (chan->withKey())
        {
            if (keyIndex < keys.size())
            {
                current_key = keys[keyIndex];
            }
            keyIndex++;
        }
        if (chan->withKey() && chan->getKey() != current_key)
        {
            sender->sendRpl(ERR_BADCHANNELKEY(sender->getNick(), current_channel));
            continue;
        }
        chan->addClient(sender);
        if (chan->getTopic().empty())
            sender->sendRpl(RPL_NOTOPIC(sender->getNick(), current_channel));
        else
            sender->sendRpl(RPL_TOPIC(sender->getNick(), current_channel, chan->getTopic()));
        sender->sendRpl(RPL_NAMREPLY(sender->getNick(), current_channel, chan->getUserList()));
        sender->sendRpl(RPL_ENDOFNAMES(sender->getNick(), current_channel));
        //ne pas oublier la capacité des chan et les chan sur invitationç
    }
}

void Command::Topic(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    Channel *chan;
    if (params.empty())
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "TOPIC"));
        return ;
    }
    if (!tmp->channelAlreadyExist(params[0]))
    {
        sender->sendRpl(ERR_NOSUCHCHANNEL(sender->getNick(), params[0]));
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
                topic.erase(topic.size() - 1);
                chan->setTopic(topic);
                sender->sendRpl(RPL_TOPIC(sender->getNick(), chan->getName(), topic));
            }
            else if (params.size() > 1 && chan->getTopicRestricted() && !chan->itsAnOp(sender))
                sender->sendRpl(ERR_CHANOPRIVSNEEDED(sender->getNick(), chan->getName()));
            else if (params.size() == 1)
            {
                if (chan->getTopic().empty())
                    sender->sendRpl(RPL_NOTOPIC(sender->getNick(), chan->getName()));
                else
                    sender->sendRpl(RPL_TOPIC(sender->getNick(), chan->getName(), chan->getTopic()));
            }
        }
        else
            sender->sendRpl(ERR_NOTONCHANNEL(sender->getNick(), chan->getName()));
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
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    if (params.size() != 2)
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "KICK"));
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
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    if (params.size() != 2)
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "INVITE"));
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
    // ne pas oublier de verifier dans join quand cest channel privée de reagrder sil a une invite
}


/* +++++++++++++++++++++++++++++ */


void Command::Mode(const std::vector<std::string>& params, client* sender, Server* tmp) 
{
    if (!sender->isRegister())
    {
        sender->sendRpl(ERR_NOTREGISTERED(sender->getNick()));
        return ;
    }
    // Vérification des paramètres
    if (params.size() < 2)
    {
        sender->sendRpl(ERR_NEEDMOREPARAMS(sender->getNick(), "MODE"));
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
