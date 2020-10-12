#pragma once

#include <string>
#include <map>
#include <algorithm>

using namespace std;

class Message
{

public:
    Message(string msg_type, map<string, string> contents);
    Message(string flattened_message);
    Message();

    string get_msg_type() const;
    map<string,string> get_contents() const;

    string flatten(); //Flattens the message into a string

private:
    string _msg_type;
    map<string, string> _contents;
};