#include "Message.hpp"

Message::Message() : _msg_type(""), _contents({{"", ""}}) {}

Message::Message(string unflattened_message) : _contents()
{
    size_t pos1 = unflattened_message.find('{');
    size_t pos2 = unflattened_message.find('}');
    _msg_type = unflattened_message.substr(pos1 + 1, pos2 - pos1 - 1); //FIXME: unsure of the math here

    pos1 = unflattened_message.find('{', pos2);
    pos2 = unflattened_message.find('}', pos1);

    string unflattened_contents = unflattened_message.substr(pos1 + 1, pos2 - pos1 - 1);
    string key, value;

    pos1 = unflattened_contents.find_first_of('(');

    while (pos1 != string::npos)
    {
        pos2 = unflattened_contents.find(':', pos1);
        key = unflattened_contents.substr(pos1 + 1, pos2 - pos1 - 1);
        pos1 = pos2;
        pos2 = unflattened_contents.find(')', pos1);
        value = unflattened_contents.substr(pos1 + 1, pos2 - pos1 - 1);
        _contents[key] = value;
        pos1 = unflattened_contents.find('(', pos2);
    }
}

Message::Message(string msg_type, map<string, string> contents) : _msg_type(msg_type),
                                                                  _contents(contents)
{
}

string Message::flatten()
{
    string flattened_message;

    flattened_message += "MESSAGETYPE:";
    flattened_message += ("{" + _msg_type + "}");
    flattened_message += "MESSAGEBODY:";
    flattened_message += "{";
    for_each(_contents.begin(), _contents.end(),
             [&](pair<string, string> n) { flattened_message += "(" + n.first + ":" + n.second + ")"; });
    flattened_message += "}";
    return flattened_message;
}