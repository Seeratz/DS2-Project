#include <windows.h>
#include <iostream>
#include <thread>
#include "trapdoor.h"

const wchar_t* writerToReader = L"Local\\WriterToReader";
const wchar_t* readerToWriter = L"Local\\ReaderToWriter";
const int bufferSize = 512;

char* pWriteBuf;
char* pReadBuf;
bool running = true;

RSAKey myKey;
RSAKey peerKey;

void sendMessage() {
    while (running) {
        std::string msg;
        std::getline(std::cin, msg);

        // Compute SHA-256 hash of the message
    std::string hash = simpleSHA256(msg);
    std::cout << "SHA-256 Hash of Message: " << hash << std::endl;

        ll signature = signMessage(msg, myKey.d, myKey.n);
        std::string fullMessage = msg + "|" + std::to_string(signature);
        strncpy_s(pWriteBuf, bufferSize, fullMessage.c_str(), bufferSize);

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
                // std::cout << "Friend: " << message << (valid ? " ✅" : " ❌") << "\n";
                std::cout << "Friend: " << message << "\n";
            }
        }
        Sleep(100);
    }
}

int main() {
    myKey = generateRSAKeys();
    std::cout << "Your Public Key (n e): " << myKey.n << " " << myKey.e << "\n";
    std::cout << "Enter Peer Public Key (n e): ";
    std::cin >> peerKey.n >> peerKey.e;
    std::cin.ignore();

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
