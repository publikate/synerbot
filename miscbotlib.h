/*
 * miscbotlib.h
 *
 *  Created on 19 February 2014
 *      By: Tim Gerety
 */

#include <string>
#include <vector>

using namespace std;

string toLower(string str);
string toUpper(string str);

void trimWhite(string& text);

string makeBorder(string title);

bool getFirstWord(string message, string& first, string& rest);

bool matchesChars(char chr, string str);

bool parseUser(string fqdn, string& nick, string& user, string& host);

void compare(string str1, string str2, unsigned& begin, unsigned& end);
