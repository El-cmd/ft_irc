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

void Command::PrivMsg(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
    int chanOrUser;
    std::string message;
    std::string chanstr;
    std::ostringstream oss;
    oss << "<" << VERT << sender->getNick() << REINIT << "> ";
    if (params.size() < 2)
    {
        log_message_client(sender->getFd(), "Not enough parameters for this command");
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
        log_message_client(sender->getFd(), "Veuillez rentrer le mot de passe avant tout autre action");
        return ;
    }
    if (!sender->getUser().empty() && !sender->getRealname().empty())
    {
        log_message_client(sender->getFd(), "Vous etes deja authentifier");
        return ;
    }
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
    if (sender->isRegister())
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
}

void Command::Nick(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    (void) tmp;
    if (!sender->getAuth())
    {
        log_message_client(sender->getFd(), "Veuillez rentrer le mot de passe avant tout autre action");
        return ;
    }
    if (params.size() != 1)
    {
        log_message_client(sender->getFd(), "Commande inconnue: Pas assez d'arguments");
        return;
    }
    sender->setNick(params[0]);
    log_message_client(sender->getFd(), sender->getNick() + " is your new nickname.");
    if (sender->isRegister())
    {
        log_message(sender->getNick() + " is authentified");
        log_message_client(sender->getFd(), sender->getNick() + " is authentified");
    }
}

void Command::Join(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
    if (params.empty())
        return ; // message d'erreur
    std::vector<std::string> channels;
    std::vector<std::string> keys;
    // On découpe le premier paramètre par des virgules pour obtenir les canaux
    std::stringstream ss(params[0]);
    std::string channel;
    Channel* chan;
    while (std::getline(ss, channel, ','))
    {
        channels.push_back(channel);
    }
    // Si des clés sont présentes, on les découpe par des virgules également
    if (params.size() > 1)
    {
        std::stringstream keyStream(params[1]);
        std::string key;
        while (std::getline(keyStream, key, ','))
        {
            keys.push_back(key);
        }
    }
    // Pour chaque canal, vérifier et ajouter le client
    size_t keyIndex = 0;  // Pour suivre les mots de passe fournis
    for (size_t i = 0; i < channels.size(); i++)
    {
        std::string current_channel = channels[i];
        std::string current_key = "";  // Valeur par défaut
    
        // Vérifier si le canal existe déjà
        if (!tmp->channelAlreadyExist(current_channel))
        {
            // Créer un nouveau canal si aucun n'existe
            chan = new Channel(current_channel, sender, tmp);
            tmp->newChannel(chan);
            continue;
        }
        else
            chan = tmp->findChan(current_channel);
        if (chan->alreadyIn(sender))
        {
            log_message_client(sender->getFd(), "Tu es deja dans le chan");
            continue;
;       }
        // Déterminer la clé du canal actuel
        if (chan->withKey())
        {
            // Si le canal nécessite une clé, on utilise la clé suivante disponible
            if (keyIndex < keys.size())
            {
                current_key = keys[keyIndex];
            }
            keyIndex++;  // Passer à la clé suivante pour le prochain canal avec clé
        }
        // Si le canal a une clé et que la clé fournie est incorrecte
        if (chan->withKey() && chan->getKey() != current_key)
        {
            // Clé incorrecte, envoyer une erreur à l'utilisateur
            log_message_client(sender->getFd(), "Nique ta mere");
            continue;  // Passer au canal suivant, ne pas ajouter le client
        }
        // Ajouter le client au canal
        chan->addClient(sender);
        //sender->addChannel(chan);
    }
}

void Command::Topic(const std::vector<std::string> &params, client *sender, Server *tmp)
{
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
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
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
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
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
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
    // ne pas oublier de verifier dans join quand cest channel privée de reagrder sil a une invite
}


/* +++++++++++++++++++++++++++++ */


void Command::Mode(const std::vector<std::string>& params, client* sender, Server* tmp) 
{
    if (!sender->isRegister())
    {
        log_message_client(sender->getFd(), "Vous n'etes pas authentifier");
        return ;
    }
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
