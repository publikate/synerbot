/*
 * bot.h
 *
 *  Created on 30 May 2013
 *      By: Synergiance
 */

#include <string>
#include <vector>
#include <fstream>
#include <random>

#include "config.h"
#include "privleges.h"
#include "net.h"
#include "cmutex.h"
#include "english.h"
#include "quote.h"
#include "userdb.h"

using namespace std;

#ifndef IRCBOT_H_
#define IRCBOT_H_

class IrcBot
{
public:
    IrcBot(string cfg, int bDebug, bool bVerbose);
    virtual ~IrcBot();

    bool setup;

    void start();
    void stop();

private:
    // Config variables
    int debugMode;
    string nick;
    string usr;
    string realName;
    string server;
    string cfgFile;
    string quoteFile;
    string serverName;
    string channelName;
    string port;

    string rgxHello;

    CConfig* botConfig;
    CPrivleges* botPriv;
    CNetSocket* botSock;
    CMutex* MessageQueue;
    QuoteHandler* CQuotes;
    CEnglish* EngLang;
    mt19937* rnd;
    CUserDB* UserDB;

    // Important static values
    static long atoimax;

    // Variables
    vector<string> quotes;
    vector<string> bufquotes;

    bool addedQuotes, verboseMode;
    volatile bool stopping;

    // Functions
    char * timeNow();

    bool sendData(char *msg);
    bool sendData(string msg);

    void sendPong(string data);

    bool globalHandle(string cmd);
    void otherHandle(string command, string message);
    void msgHandel(string buf);
    void nicklistHandle(string list);
    void whoisHandle(string buf);

    int msgParse(string buf, string& sender, string& message, string& cmd);

    void AI(string sender, string cmd, string msg);

    int commandHandle(string cmd, string args, string talkto, string usr);
    bool extractCommandArgs(string message, string& command, string& args);

    void say(string target, string message);
    void action(string target, string message);

    // User Functions
    void editPrivs(string cmd, string args, string talkto);
    void lookup(string search, string talkto);

    // Utils
    void whois(string target);
};

#endif /* IRCBOT_H_ */
