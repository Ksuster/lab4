#ifndef RECEIVER_MESSAGE_H
#define RECEIVER_MESSAGE_H

#include<iostream>
#include<fstream>
#include<Windows.h>

using namespace std;

struct Message {
    Message() : message() {}

    Message(string str) : message() {
        strcpy(message, str.c_str());
    }

    char *toString() {
        return message;
    }

    friend fstream &operator<<(fstream &file, const Message &msg) {
        file.write((const char *) &msg, sizeof(struct Message));
        return file;
    }

    friend fstream &operator>>(fstream &file, Message &msg) {
        file.read((char *) &msg, sizeof(struct Message));
        return file;
    }

private:
    char message[20];
};

#endif