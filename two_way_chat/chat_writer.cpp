#include <windows.h>
#include <iostream>
#include <thread>
#include <fstream>
#include "trapdoor.h"
#include "user_manager.h"

const wchar_t* writerToReader = L"Local\\WriterToReader";
const wchar_t* readerToWriter = L"Local\\ReaderToWriter";
const int bufferSize = 512;

char* pWriteBuf;
char* pReadBuf;
bool running = true;

RSAKey myKey;
RSAKey peerKey;
std::string username, peerUsername;
std::string chatLogFile;

void sendMessage() {
    while (running) {
        std::string msg;
        std::getline(std::cin, msg);

        std::string hash = simpleSHA256(msg);
        std::cout << "SHA-256 Hash of Message: " << hash << std::endl;

        ll signature = signMessage(msg, myKey.d, myKey.n);
        std::string fullMessage = msg + "|" + std::to_string(signature);
        strncpy_s(pWriteBuf, bufferSize, fullMessage.c_str(), bufferSize);

        std::ofstream chatLog(chatLogFile, std::ios::app);
        if (chatLog.is_open()) {
            chatLog << "You: " << msg << "\n";
        }

        if (msg == "exit") {
            running = false;
            break;
        }
    }
}

void receiveMessage() {
    while (running) {
        if (strlen(pReadBuf) > 0) {
            std::string raw(pReadBuf);
            memset(pReadBuf, 0, bufferSize);

            size_t sep = raw.find('|');
            if (sep != std::string::npos) {
                std::string message = raw.substr(0, sep);
                ll signature = std::stoll(raw.substr(sep + 1));

                bool valid = verifySignature(message, signature, peerKey.e, peerKey.n);
                std::cout << peerUsername << ": " << message << (valid ? " [✔]" : " [✘ Invalid Signature]") << "\n";

                std::ofstream chatLog(chatLogFile, std::ios::app);
                if (chatLog.is_open()) {
                    chatLog << peerUsername << ": " << message << "\n";
                }
            }
        }
        Sleep(100);
    }
}

int main() {
    loadUsersFromFile("users.txt");

    std::cout << "Enter your username: ";
    std::getline(std::cin, username);

    if (userExists(username)) {
        myKey = getUserKey(username);
        std::cout << "Welcome back, " << username << "!\n";
    } else {
        myKey = generateRSAKeys();
        addUser(username, myKey);
        saveUsersToFile("users.txt");
        std::cout << "New user created: " << username << "\n";
    }

    std::cout << "Your Public Key (n e): " << myKey.n << " " << myKey.e << "\n";
    
    std::cout << "Enter peer username: ";
    std::getline(std::cin, peerUsername);
    
    std::cout << "Enter Peer Public Key (n e): ";
    std::cin >> peerKey.n >> peerKey.e;
    std::cin.ignore();

    chatLogFile = username + "_" + peerUsername + ".txt";

    HANDLE hMapWrite = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, writerToReader);
    HANDLE hMapRead = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, bufferSize, readerToWriter);

    pWriteBuf = (char*)MapViewOfFile(hMapWrite, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    pReadBuf = (char*)MapViewOfFile(hMapRead, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);

    std::cout << "Chat started (Writer). Type 'exit' to end chat.\n";

    std::thread recv(receiveMessage);
    sendMessage();
    recv.join();

    UnmapViewOfFile(pWriteBuf);
    UnmapViewOfFile(pReadBuf);
    CloseHandle(hMapWrite);
    CloseHandle(hMapRead);

    return 0;
}
