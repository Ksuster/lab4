#include <vector>
#include <windows.h>
#include <iostream>
#include <sstream>
#include "ProcessMessage.h"

using namespace std;

HANDLE startSingleProcess(std::string &binFileName, int i) {

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ostringstream ss;
    ss << binFileName << ' ' << i;
    if (!CreateProcess("sender.exe",
                       &ss.str()[0],
                       NULL,
                       NULL,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       NULL,
                       NULL,
                       &si,
                       &pi))
        GetLastError();

    CloseHandle(pi.hThread);
    return pi.hProcess;
}

vector<HANDLE> processesHandles;
HANDLE *startEvents;

vector<HANDLE> startSender(int processesCount, string &binaryFileName) {
    startEvents = new HANDLE[processesCount];
    for (int i = 0; i < processesCount; i++) {
        ostringstream handleName;
        handleName << "_ready_" << i;

        processesHandles.push_back(startSingleProcess(binaryFileName, i));
        startEvents[i] = CreateEvent(
                NULL,
                TRUE,
                FALSE, &handleName.str()[0]);
    }

    return processesHandles;
}

int main() {
    cout << "Enter binary file name:\n";
    string binFileName;
    cin >> binFileName;
    cout << "Enter message count:\n";
    int messageCount;
    cin >> messageCount;
    cout << "Enter sender count:\n";
    int senderCount;
    cin >> senderCount;

    fstream fs(binFileName.c_str(), ios::out | ios::binary | ios::trunc);
    fs.close();

    startSender(senderCount, binFileName);
    HANDLE hMutex = CreateMutex(NULL, FALSE, "file mutex");
    HANDLE msgSemaphore = CreateSemaphore(NULL, 0, messageCount, "msg emaphore");
    HANDLE fileWriteSemaphore = CreateSemaphore(NULL, messageCount, messageCount, "Write Semaphore");

    WaitForMultipleObjects(senderCount, startEvents, TRUE, INFINITE);

    while (true) {
        cout << "Options\n 1) read message \n 2) exit\n";
        int option;
        cin >> option;
        if (option == 2) break;

        WaitForSingleObject(msgSemaphore, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        fs.open(binFileName.c_str(), ios::binary | ios::in);
        Message message;
        fs >> message;

        cout << "New message : " << message.toString() << endl;

        vector<Message> allFileMessages;
        while (fs >> message) {
            allFileMessages.push_back(message);
        }
        fs.close();

        fs.open(binFileName.c_str(), ios::binary | ios::out);
        for (int i = 0; i < allFileMessages.size(); ++i) {
            fs << allFileMessages[i];
        }
        fs.close();

        ReleaseMutex(hMutex);
        ReleaseSemaphore(fileWriteSemaphore, 1, NULL);
    }

    for (int i = 0; i < processesHandles.size(); i++) {
        CloseHandle(processesHandles[i]);
        CloseHandle(startEvents[i]);
    }

    delete[] startEvents;
    CloseHandle(fileWriteSemaphore);
    CloseHandle(msgSemaphore);
    CloseHandle(hMutex);
}