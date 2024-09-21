#ifndef CHANNEL_HPP
    #define CHANNEL_HPP

#include "irc.hpp"

class Channel
{
    public:
        Channel(std::string args, client *creator, Server *serv);
        ~Channel();
        void addClient(client *sender);
        void removeClient(client *tokick);
        bool alreadyIn(client *sender);
    /* +++ Getter +++ */
        std::string getUserList();
        std::string getName();
        bool withKey();
        std::string getTopic();
        void channelAllMessageWithoutSender(std::string message, client* sender);
        void channelAllMessage(std::string message);
        bool getOnInvite();
        std::string getKey();
        bool getTopicRestricted();
        bool itsAnOp(client *sender);
        std::map<int, client*>& getClientsInChan();
    /* ++++++++++++++ */
    /* +++ Setter +++ */
        void setTopic(std::string &topic);
        void setInviteOnly(bool mode);
        void setTopicRestricted(bool mode);
        void setKey(std::string pswd);
        void clearKey();
        void addOperator(client *newOp);
        void removeOperator(client *newOp);
        void setUserLimit(int limit);
        void clearUserLimit();
    /* ++++++++++++++ */
    private:
        Channel();
        bool _whithKey;
        std::map<int, client *> _clients;
        std::string _name;
        std::vector<client *> _operator;
        std::string _topic;
        bool _onInvite;
        bool _topicRestricted;
        bool _limits;
        std::string _mdp;
        int _nbLim;

};

#endif