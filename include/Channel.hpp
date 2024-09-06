#ifndef CHANNEL_HPP
    #define CHANNEL_HPP

#include "irc.hpp"

class Channel
{
    public:
        Channel(std::string args, client *creator, Server *serv);
        ~Channel();
        void addClient(client *sender);
        bool alreadyIn(client *sender);
    /* +++ Getter +++ */
        std::string getName();
        bool withKey();
        std::string getTopic();
        bool getOnInvite();
        bool getTopicRestricted();
        bool itsAnOp(client *sender);
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