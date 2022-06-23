#include<stdlib.h>
#include <sstream>
#include <windows.h>
#include <process.h>
#include <iostream>
#include "ProcessMessage.h"

using namespace std;

int main(int argc, char *argv[]) {
    string fileName = argv[1];
    int id = atoi(argv[2]);
    fstream fs;
    ostringstream name;
    name << "_ready_" << id;
    HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "fs mutex");
    HANDLE msgSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "msg semaphore");
    HANDLE fileWriteSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, "write semaphore");

    HANDLE senderStartEvent = OpenEvent(EVENT_MODIFY_STATE,
                                        FALSE,
                                        &name.str()[0]);
    SetEvent(senderStartEvent);

    while (true) {
        cout << "\n\n\nSender " << id << " \n Options: \n" << "1) send message \n 2) exit \n";
        int option;
        cin >> option;
        if (option == 2) {
            ReleaseMutex(hMutex);
            break;
        }

        WaitForSingleObject(fileWriteSemaphore, INFINITE);
        WaitForSingleObject(hMutex, INFINITE);

        cout << "enter message\n";
        string message;
        cin >> message;

        fs.open(fileName.c_str(), ios::out | ios::binary | ios::app);
        fs << Message(message);
        fs.close();

        ReleaseMutex(hMutex);
        ReleaseSemaphore(msgSemaphore, 1, NULL);
    }

    CloseHandle(hMutex);
    CloseHandle(msgSemaphore);
    CloseHandle(fileWriteSemaphore);
    CloseHandle(senderStartEvent);
}