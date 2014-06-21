/*
 * userdb.cpp
 *
 *  Created on 20 June 2014
 *      By: Tim Gerety
 */

/*

A class designed to keep track of users across hostnames

*/

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "userdb.h"
#include "miscbotlib.h"

using namespace std;

CUserDB::CUserDB()
{
    userdbfile = "userdb.db";
    readdb();
}

CUserDB::~CUserDB()
{
    writedb();
}

string CUserDB::checkUser(string username)
{
    string nick, user, host; int tmp = -1;
    if (parseUser(username, nick, user, host))
        tmp = checkUser(nick, user, host);
    return compileUser(tmp);
}

string CUserDB::spotUser(string username)
{
    string nick, user, host, ret = ""; int num;
    if (parseUser(username, nick, user, host)) {
        num = checkUser(nick, user, host);
        if (num == -1) { // Create a new member
            memberEntry newMember;
            newMember.nicks.push_back(nick);
            newMember.nickints.push_back(1);
            newMember.users.push_back(user);
            newMember.userints.push_back(1);
            newMember.hosts.push_back(host);
            newMember.hostints.push_back(1);
            members.push_back(newMember);
            ret = username;
        }
        else { // Add to existing member
            bool found = false;
            for (int c = 0; c < members[num].nicks.size() && !found; c++)
                if (toLower(members[num].nicks[c]) == toLower(nick))
                    found = true;
            if (!found) members[num].nicks.push_back(nick);
            found = false;
            for (int c = 0; c < members[num].users.size() && !found; c++)
                if (toLower(members[num].users[c]) == toLower(user))
                    found = true;
            if (!found) members[num].users.push_back(user);
            found = false;
            for (int c = 0; c < members[num].hosts.size() && !found; c++)
                if (toLower(members[num].hosts[c]) == toLower(user))
                    found = true;
            if (!found) members[num].hosts.push_back(user);
            ret = compileUser(num);
        }
    }
    return ret;
}

string CUserDB::compileUser(int num)
{
    string ret = "";
    if (num >= 0) ret = members[num].nicks[0] + "!" + members[num].users[0]
                      + "@" + members[num].hosts[0];
    return ret;
}

int CUserDB::checkUser(string nick, string user, string host)
{
    int pos = -1;
    if (members.size() > 0) {
        for (int x = 0; x < members.size() && pos == -1; x++) {
            for (int y = 0; y < members[x].nicks.size() && pos == -1; y++)
                if (toLower(members[x].nicks[y]) == toLower(nick)) pos = x;
            for (int y = 0; y < members[x].users.size() && pos == -1; y++)
                if (toLower(members[x].users[y]) == toLower(user)) pos = x;
            for (int y = 0; y < members[x].hosts.size() && pos == -1; y++)
                if (toLower(members[x].hosts[y]) == toLower(host)) pos = x;
        }
    }
    return pos;
}

bool CUserDB::parseUser(string fqdn, string& nick, string& user, string& host)
{
    int a = fqdn.find("!");
    int b = fqdn.find("@");
    bool ret = false;
    if (!(a == -1 || b == -1)) {
        nick = fqdn.substr(0,a++);
        user = fqdn.substr(a,b-a);
        host = fqdn.substr(b+1);
        ret = true;
    }
    return ret;
}

void CUserDB::readdb()
{
    ifstream ifile (userdbfile.c_str());
    if (ifile) { // File exists, let's read from it
        string strBuffer;
        cout<<"Loading user database...";
        while (getline(ifile, strBuffer)) {
            if (strBuffer.compare("member {") == 0) { // Begin member section
                memberEntry newMember;
                while (getline(ifile, strBuffer)) {
                    if (strBuffer.compare("    nicks {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.find(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.nicks.push_back(str.substr(0, pos));
                            newMember.nickints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    users {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.find(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.users.push_back(str.substr(0, pos));
                            newMember.userints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    hosts {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.find(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.hosts.push_back(str.substr(0, pos));
                            newMember.hostints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("}") == 0) break;
                }
                members.push_back(newMember);
            }
        }
        ifile.close();
        cout<<" Done!\n";
    }
    else {
        cout<<"No user database found\n";
    }
}

void CUserDB::writedb()
{
    if (members.size() > 0) {
        ofstream ofile (userdbfile.c_str());
        string tab = "    ";
        if (ofile) {
            cout<<"Loading user database...";
            for (int c = 0; c < members.size(); c++) {
                ofile<<"member {\n"<<tab<<"nicks {\n";
                for (int x = 0; x < members[c].nicks.size(); x++) {
                    ofile<<tab<<tab<<members[c].nicks[x]<<" "
                                   <<members[c].nickints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"users {\n";
                for (int x = 0; x < members[c].users.size(); x++) {
                    ofile<<tab<<tab<<members[c].users[x]<<" "
                                   <<members[c].userints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"hosts {\n";
                for (int x = 0; x < members[c].hosts.size(); x++) {
                    ofile<<tab<<tab<<members[c].hosts[x]<<" "
                                   <<members[c].hostints[x]<<endl;
                }
                ofile<<tab<<"}\n}\n\n";
            }
            ofile.close();
            cout<<" Done!\n";
        }
        else {
            cout<<"Error writing user database\n";
        }
    }
    else {
        cout<<"No users spotted\n";
    }
}
