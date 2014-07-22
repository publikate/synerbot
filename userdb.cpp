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
#include <sstream>
//#include <regex>

#include "userdb.h"
#include "miscbotlib.h"
#include "net.h"

using namespace std;

CUserDB::CUserDB()
{
    userdbfile = "userdb.db";
    readdb();
    debugMode = false;
}

CUserDB::~CUserDB()
{
    writedb();
}

void CUserDB::setDebug(bool mode)
{
    debugMode = mode;
}

string CUserDB::checkUser(string username)
{
    string nick, user, host; int tmp = -1;
    if (parseUser(username, nick, user, host))
        tmp = searchUser(nick, user, host, "");
    return compileUser(tmp);
}

string CUserDB::spotUser(string nick, string user, string host, string name)
{
    string ret = "";
    int num = searchUser(nick, user, host, name);
    if (num == -1) { // Create a new member
        memberEntry newMember;
        newMember.nicks.push_back(nick);
        newMember.nickints.push_back(1);
        newMember.users.push_back(user);
        newMember.userints.push_back(1);
        newMember.hosts.push_back(host);
        newMember.hostints.push_back(1);
        newMember.names.push_back(name);
        newMember.nameints.push_back(1);
        members.push_back(newMember);
        ret = nick + "!" + user + "@" + host;
    }
    else { // Add to existing member
        bool found = false;
        unsigned c;
        for (c = 0; c < members[num].nicks.size() && !found; c++)
            if (toLower(members[num].nicks[c]) == toLower(nick))
                found = true;
        if (found) {
            members[num].nickints[c-1]++;
        }
        else {
            members[num].nicks.push_back(nick);
            members[num].nickints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].users.size() && !found; c++)
            if (toLower(members[num].users[c]) == toLower(user))
                found = true;
        if (found) {
            members[num].userints[c-1]++;
        }
        else {
            members[num].users.push_back(user);
            members[num].userints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].hosts.size() && !found; c++)
            if (toLower(members[num].hosts[c]) == toLower(host))
                found = true;
        if (found) {
            members[num].hostints[c-1]++;
        }
        else {
            members[num].hosts.push_back(host);
            members[num].hostints.push_back(1);
        }
        found = false;
        for (c = 0; c < members[num].names.size() && !found; c++)
            if (toLower(members[num].names[c]) == toLower(name))
                found = true;
        if (found) {
            members[num].nameints[c-1]++;
        }
        else {
            members[num].names.push_back(name);
            members[num].nameints.push_back(1);
        }
        ret = compileUser(num);
    }
    return ret;
}

string CUserDB::compileUser(int num)
{// Returns a string representing the time a user has been seen
    string ret = "";
    if (num >= 0) ret = members[num].nicks[0] + "!" + members[num].users[0]
                      + "@" + members[num].hosts[0];
    return ret;
}

memberEntry CUserDB::getUser(int num)
{// Returns the user at specified index
    return members[num];
}

string CUserDB::checkUser(string nick, string user, string host, string name)
{// Checks the name on a user search
    return compileUser(searchUser(nick, user, host, name));
}

int CUserDB::searchUser(string nick, string user, string host, string name)
{// Searches and finds a user based on parameters
    int pos = -1;
    unsigned char score = 0;
    if (debugMode) {// Debug mode
        cout<<"Searching user database for:\nNick: "<<nick<<"\nUser: "
            <<user<<"\nHost: "<<host<<"\nName: "<<name<<endl;
    }
    if (members.size() > 0) {
        for (unsigned x = 0; x < members.size(); x++) {
            unsigned y;
            unsigned char tmpScore = 0;
            if (nick != "")
                for (y = 0; y < members[x].nicks.size(); y++)
                    if (toLower(members[x].nicks[y]) == toLower(nick))
                        tmpScore += 30;
            if (user != "")
                for (y = 0; y < members[x].users.size(); y++)
                    if (toLower(members[x].users[y]) == toLower(user))
                        tmpScore += 35;
            if (host != "")
                for (y = 0; y < members[x].hosts.size(); y++)
                    if (toLower(members[x].hosts[y]) == toLower(host))
                        tmpScore += 10;
            if (name != "")
                for (y = 0; y < members[x].names.size(); y++)
                    if (toLower(members[x].names[y]) == toLower(name))
                        tmpScore += 25;
            if (debugMode) {// Debug Mode
                cout<<compileUser(x)<<" "<<int(tmpScore)<<endl;
            }
            if (tmpScore > score) {
                score = tmpScore;
                pos = x;
            }
        }
        if (score < 45) pos = -1;
    }
    return pos;
}

posPair CUserDB::lookupUser(string nick, string user, string host, string name)
{// Searches and finds a user based on parameters
    posPair scores;
    if (debugMode) {// Debug mode
        cout<<"Searching user database for:\nNick: "<<nick<<"\nUser: "
            <<user<<"\nHost: "<<host<<"\nName: "<<name<<endl;
    }
    if (members.size() > 0) {
        for (unsigned x = 0; x < members.size(); x++) {
            unsigned y;
            unsigned char tmpScore = 0;
            if (nick != "")
                for (y = 0; y < members[x].nicks.size(); y++)
                    if (toLower(members[x].nicks[y]) == toLower(nick))
                        tmpScore += 30;
            if (user != "")
                for (y = 0; y < members[x].users.size(); y++)
                    if (toLower(members[x].users[y]) == toLower(user))
                        tmpScore += 35;
            if (host != "")
                for (y = 0; y < members[x].hosts.size(); y++)
                    if (toLower(members[x].hosts[y]) == toLower(host))
                        tmpScore += 10;
            if (name != "")
                for (y = 0; y < members[x].names.size(); y++)
                    if (toLower(members[x].names[y]) == toLower(name))
                        tmpScore += 25;
            if (debugMode) {// Debug Mode
                cout<<compileUser(x)<<" "<<int(tmpScore)<<endl;
            }
            if (tmpScore > 0) {
                scores.positions.push_back(x);
                scores.scores.push_back(tmpScore);
            }
        }
    }
    return scores;
}

void CUserDB::readdb()
{// Reads the user database into memory
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
                            int pos = str.rfind(' ');
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
                            int pos = str.rfind(' ');
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
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.hosts.push_back(str.substr(0, pos));
                            newMember.hostints.push_back(num);
                        }
                    }
                    else if (strBuffer.compare("    names {") == 0) {
                        while (getline(ifile, strBuffer)) {
                            string str = strBuffer;
                            trimWhite(str);
                            if (str.compare("}") == 0) break;
                            if (str == "") continue;
                            int pos = str.rfind(' ');
                            int num = atoi(str.substr(pos+1).c_str());
                            newMember.names.push_back(str.substr(0, pos));
                            newMember.nameints.push_back(num);
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
            cout<<"Saving user database...";
            for (unsigned c = 0; c < members.size(); c++) {
                ofile<<"member {\n"<<tab<<"nicks {\n";
                for (unsigned x = 0; x < members[c].nicks.size(); x++) {
                    ofile<<tab<<tab<<members[c].nicks[x]<<" "
                                   <<members[c].nickints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"users {\n";
                for (unsigned x = 0; x < members[c].users.size(); x++) {
                    ofile<<tab<<tab<<members[c].users[x]<<" "
                                   <<members[c].userints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"hosts {\n";
                for (unsigned x = 0; x < members[c].hosts.size(); x++) {
                    ofile<<tab<<tab<<members[c].hosts[x]<<" "
                                   <<members[c].hostints[x]<<endl;
                }
                ofile<<tab<<"}\n"<<tab<<"names {\n";
                for (unsigned x = 0; x < members[c].names.size(); x++) {
                    ofile<<tab<<tab<<members[c].names[x]<<" "
                                   <<members[c].nameints[x]<<endl;
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

//------------------------------------------------------------------------------
//    Member Entry Code

string memberEntry::firstSeen()
{// Returns the first time the member has been seen by the bot
    stringstream ss;
    ss<<"First seen as: "<<nicks[0]<<"!"<<users[0]
      <<"@"<<hosts[0]<<" ("<<names[0]<<")";
    return ss.str();
}

string memberEntry::mostSeen()
{// Returns most seen variation of current member entry
    stringstream ss;
    string tmpstr;
    unsigned tmp = 0;
    ss<<"Most seen as: ";
    tmp = getHighest(nickints);
    tmpstr = nicks[tmp];
    ss<<tmpstr<<"!";
    tmp = getHighest(userints);
    tmpstr = users[tmp];
    ss<<tmpstr<<"@";
    tmpstr = string();
    getHighestMask(hostints, hosts, tmpstr);
    ss<<tmpstr<<" (";
    if (debugMode) getHighestHostMask(hostints, hosts, tmpstr);
    tmp = getHighest(nameints);
    tmpstr = names[tmp];
    ss<<tmpstr<<")";
    return ss.str();
}

string memberEntry::lastSeen()
{// Returns the last time that the member has been seen by the bot
    stringstream ss;
    ss<<"This function is not currently ready for production";
    return ss.str();
}

unsigned memberEntry::getHighest(const vector<int>& array)
{// Returns the entry with the most visability
    unsigned a = 0;
    for (unsigned c = 0; c < array.size(); c++)
        if (array[c] > array[a]) a = c;
    return a;
}

// Boiler Plate - (yes I love boiler plating things, sue me)
int memberEntry::getHighestMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask)
{ int a; return getHighestMask(stringNums, strings, mask, a); }

int memberEntry::getHighestMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask, int& num)
{// Create a wildcard mask for any string of characters
    int tmp = 0; string tmpstr;
    vector<string> masks;
    vector<int> maskNums;
    if (debugMode) cout<<"BEGIN SEARCH FOR HIGHEST MASK\n";
    if (stringNums.size() != strings.size()) return -1;
    if (debugMode) cout<<"Size: "<<strings.size()<<endl;
    for (unsigned x = 0; x < strings.size() - 1; x++) {
        if (debugMode) cout<<"Comparing: "<<strings[x]<<endl;
        for (unsigned y = x + 1; y < strings.size(); y++) {
            unsigned a, b; int d = 0; string str;
            if (debugMode) cout<<"To: "<<strings[y]<<endl;
            compare(strings[x], strings[y], a, b);
            if (debugMode) cout<<"Begin: "<<a<<" End: "<<b<<endl;
            if (a > 0 || b > 0) {
                if (a > 0) str += strings[x].substr(0,a);
                str += "*";
                if (b > 0) str += strings[x].substr(strings[x].size() - b);
                d = stringNums[x] + stringNums[y];
                if (debugMode) cout<<"Hypothetical mask: "<<str<<" "<<d<<endl;
            }
            if (d != 0) {
                for (unsigned e = 0; e < masks.size(); e++) {
                    if (str == masks[e]) {
                        str = ""; d = 0;
                        if (debugMode) cout<<"Duplicate\n";
                        break;
                    }
                }
                if (d > 0) {
                    d = 0;
                    for (unsigned e = 0; e < strings.size(); e++) {
                        unsigned f, g;
                        compare(str,strings[e],f,g);
                        if (f==a && g==b) d += stringNums[e];
                    }
                    masks.push_back(str);
                    maskNums.push_back(d);
                    if (debugMode) cout<<"Added\n";
                }
            }
        }
    }
    if (debugMode) cout<<"SEARCHING RAW\n";
    for (unsigned y = 0; y < strings.size(); y++) {
        if (debugMode) cout<<strings[y]<<" "<<stringNums[y];
        if (stringNums[y] > tmp) {
            tmp = stringNums[y];
            tmpstr = strings[y];
            if (debugMode) cout<<" √";
        }
        if (debugMode) cout<<endl;
    }
    if (debugMode) cout<<"SEARCHING GENERATED\n";
    for (unsigned y = 0; y < masks.size(); y++) {
        if (debugMode) cout<<masks[y]<<" "<<maskNums[y];
        if (maskNums[y] > tmp) {
            tmp = maskNums[y];
            tmpstr = masks[y];
            if (debugMode) cout<<" √";
        }
        if (debugMode) cout<<endl;
    }
    if (debugMode) cout<<"Final Result: "<<tmpstr<<endl;
    mask = tmpstr;
    num = tmp;
    return 0;
}

// Boiler Plate - (yes I love boiler plating things, sue me)
int memberEntry::getHighestHostMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask)
{ int a; return getHighestHostMask(stringNums, strings, mask, a); }

int memberEntry::getHighestHostMask (const vector<int>& stringNums,
    const vector<string>& strings, string& mask, int& num)
{// Makes a wildcard mask of hostnames/IPs
    vector< vector<string> > DNShosts;
    vector< vector<unsigned char> > IPv4hosts;
    vector< vector<int> > IPv6hosts;
    vector<string> hosts;
    vector<int> DNShostNums;
    vector<int> IPv4hostNums;
    vector<int> IPv6hostNums;
    vector<int> hostNums;
    string tmpMask; int tmpNum;
    unsigned c, x, y; unsigned char z; // Reusable variables
    for (unsigned x = 0; x < strings.size(); x++) {
        string str = strings[x];
        if (debugMode) cout<<"Identifying: "<<str<<endl;
        if (check_IPv4(str)) {
            vector<unsigned char> IPv4hostBits;
            IPv4parse(str, IPv4hostBits);
            IPv4hosts.push_back(IPv4hostBits);
            IPv4hostNums.push_back(stringNums[x]);
        } else if (check_IPv6(str)) {
            vector<int> IPv6hostBits;
            IPv6parse(str, IPv6hostBits);
            IPv6hosts.push_back(IPv6hostBits);
            IPv6hostNums.push_back(stringNums[x]);
        } else if (str.find('.') != string::npos) {
            vector<string> DNShostBits;
            DNSparse(str, DNShostBits);
            DNShostNums.push_back(stringNums[x]);
            DNShosts.push_back(DNShostBits);
        } else {// Add to array for old method
            if (debugMode) cout<<"Adding to legacy array\n";
            hosts.push_back(str);
            hostNums.push_back(stringNums[x]);
        }
    }
    if (hosts.size() > 0) {
        if (debugMode) cout<<"Acting legacy method\n";
        getHighestMask(hostNums, hosts, tmpMask, tmpNum);
    }
    if (IPv4hosts.size() > 0) {
        if (debugMode) cout<<"Comparing IPv4 hosts\n";
        vector< vector<unsigned char> > tmpList;
        vector< vector<bool> > tmpMaskList;
        vector<unsigned char> tmpItem;
        vector<bool> tmpMaskItem;
        vector<int> tmpNumList;
        for (z = 0; z < 4; z++) {
            tmpItem.push_back(0);
            tmpMaskItem.push_back(0);
        }
        for (x = 0; x < IPv4hosts.size() - 1; x++) {
            for (y = x + 1; y < IPv4hosts.size(); y++) {
                for (z = 0; z < 4; z++) {
                    if (IPv4hosts[x][z] == IPv4hosts[y][z]) {
                        if (z > 0 && tmpMaskItem[z-1]) {
                            tmpItem[z] = IPv4hosts[x][z];
                            tmpMaskItem[z] = true;
                        }
                    } else tmpMaskItem[z] = false;
                }
                if (tmpMaskItem[0]) {
                    bool verdict;
                    for (c = 0; c < tmpList.size(); c++) {
                        verdict = true;
                        for (z = 0; z < 4 && verdict; z++) {
                            if (tmpMaskItem[z] != tmpMaskList[c][z])
                                verdict = false;
                            if (tmpItem[z] != tmpList[c][z])
                                verdict = false;
                        }
                        if (verdict) break;
                    }
                    if (!verdict) {
                        tmpList.push_back(tmpItem);
                        tmpMaskList.push_back(tmpMaskItem);
                        tmpNumList.push_back(IPv4hostNums[x] + IPv4hostNums[y]);
                    }
                }
            }
        }
        //code
    }
    if (IPv6hosts.size() > 0) {
        if (debugMode) cout<<"Comparing IPv6 hosts\n";
        for (x = 0; x < IPv6hosts.size() - 1; x++) {
            for (y = x + 1; y < IPv6hosts.size(); y++) {
                //code
            }
        }
    }
    return 0;
}

bool memberEntry::IPv4parse(string str, vector<unsigned char>& array)
{// Parses an IPv4 quad octet into 4 chars, returns false if not IPv4
    if (!check_IPv4(str)) return false;
    if (debugMode) cout<<"IPv4 string itentified, parsing...\n";
    size_t a; unsigned char b, c, d, e; char num [3];
    for (;;) {// More efficient than while (true)
        a = str.find('.'); e = 0;
        for (c = 0; c < 3; c++) num[c] = 0;
        if (a == string::npos) {
            if (debugMode) cout<<str<<" ";
            for (c = 0; c < 3 && c < str.length(); c++)
                num[c] = str[str.length() - 1 - c];
        } else {
            if (debugMode) cout<<str.substr(0,a)<<" ";
            for (c = 0; c < 3 && c < a; c++)
                num[c] = str[a-1-c];
            str.erase(0,a+1);
        }
        for (c = 0; c < 3; c++) {// Base 10 convert without a null term
            b = 1;
            for (d = 0; d < c; d++) b *= 10;
            e += b * (num[c] - 48);
        }
        array.push_back(e);
        if (a == string::npos) break;
    }
    cout<<"Done!\n";
    return true;
}

bool memberEntry::IPv6parse(string str, vector<int>& array)
{// Parses an IPv6 quad octet into 4 chars, returns false if not IPv6
    if (!check_IPv6(str)) return false;
    if (debugMode) cout<<"IPv6 string itentified, parsing...\n";
    size_t a, b; unsigned char c;
    a = str.find("::");
    if (a == string::npos) {// No '::' in address
        for (;;) {
            b = str.find(":");
            if (b == string::npos) {// We reached the end
                array.push_back(quadhextoint(str));
                if (debugMode) cout<<str<<"->"<<array.back()<<" ";
                break;
            } else {// Eat the string and move on to the next
                array.push_back(quadhextoint(str.substr(0,b)));
                if (debugMode) cout<<str.substr(0,b)<<"->"<<array.back()<<" ";
                str.erase(0,b+1); // EAT THE DAMN STRING RAWR
            }
        }
    } else {// Address is shorthand, we'll have to do this in 2 parts
        // Initialize 8 ints in the array to 0
        for (c=0;c<8;c++) array.push_back(0);
        for (c=0;c<8;c++) {// Iterator + limiter pretty much
            b = str.find(":");
            if (b == 0) {// '::' iss at the beginning of the address
                str.erase(0,2);
                break;
            }
            array[c] = quadhextoint(str.substr(0,b));
            if (debugMode) cout<<str.substr(0,b)<<"->"<<array[c]<<" ";
            if (a == b) {// We reached the '::'
                str.erase(0, b + 2);
                break;
            } else {// Move a up and eat the beginning of the address
                str.erase(0, ++b);
                a -= b;
            }
        }
        if (debugMode) cout<<":: ";
        for (c=0;c<8;c++) {
            b = str.rfind(":");
            if (b == string::npos) {// Last group in IPv6 address
                array[7-c] = quadhextoint(str);
                if (debugMode) cout<<str<<"->"<<array[7-c]<<" ";
                break;
            } else {// Not the last group, continue happily
                array[7-c] = quadhextoint(str.substr(b+1));
                if (debugMode) cout<<str.substr(b+1)<<"->"<<array[7-c]<<" ";
                str.erase(b, str.size() - b + 1);
            }
        }
    }
    if (debugMode) cout<<"Done!\n";
    return true;
}

bool memberEntry::DNSparse(string str, vector<string>& array)
{// Parses a dot formatted DNS entry into it's sub parts
    size_t a;
    if (str.find('.') == string::npos) return false;
    if (debugMode) cout<<"DNS accepted, parsing...\n";
    for (;;) {
        a = str.rfind('.');
        if (a == string::npos) {
            array.push_back(str);
            if (debugMode) cout<<str<<" ";
            break;
        } else {
            array.push_back(str.substr(a+1));
            if (debugMode) cout<<str.substr(a+1)<<" ";
            str = str.substr(0,a);
        }
    }
    if (debugMode) cout<<"Done!\n";
    return true;
}

int memberEntry::quadhextoint(string hexNum)
{// Converts up to 4 hex characters to an int, returns -1 if string is too long
    size_t a, b; char c; unsigned d, e = 0; unsigned char f;
    if ((a = hexNum.size()) > 4) return -1;
    for (b = 0; b < a; b++) {
        c = hexNum[a-1-b] - 48;
        if (c > 9) c -= 7; // A-F range
        if (c > 9) c -= 32; // a-f range
        d = 1;
        for (f = 0; f < b; f++) d *= 16;
        e += d * c;
    }
    return e;
}
