// #include <windows.h>
// #include <iostream>
// #include <string>

// const wchar_t* writerToReader = L"Local\\WriterToReader";
// const wchar_t* readerToWriter = L"Local\\ReaderToWriter";
// const int bufferSize = 256;

// void sendMessage(HANDLE hMapFile, char* pBuf) {
//     std::string message;
//     std::cout << "You: ";
//     std::getline(std::cin, message);
//     strncpy_s(pBuf, bufferSize, message.c_str(), bufferSize);
// }

// void receiveMessage(char* pBuf) {
//     if (strlen(pBuf) > 0) {
//         std::cout << "Friend: " << pBuf << "\n";
//         memset(pBuf, 0, bufferSize);  // Clear the buffer after reading
//     }
// }

// int main() {
//     // Open shared memory for reader-to-writer
//     HANDLE hMapWrite = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, readerToWriter);
//     HANDLE hMapRead = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);

//     if (!hMapWrite || !hMapRead) {
//         std::cerr << "Could not open file mapping (" << GetLastError() << ")\n";
//         return 1;
//     }

//     char* pWriteBuf = (char*)MapViewOfFile(hMapWrite, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
//     char* pReadBuf = (char*)MapViewOfFile(hMapRead, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);

//     if (!pWriteBuf || !pReadBuf) {
//         std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
//         return 1;
//     }

//     std::cout << "Chat started. Type 'exit' to end chat.\n";

//     while (true) {
//         receiveMessage(pReadBuf);

//         sendMessage(hMapWrite, pWriteBuf);
//         if (strcmp(pWriteBuf, "exit") == 0) break;

//         Sleep(500);  // Allow the other process to respond
//     }

//     // Cleanup
//     UnmapViewOfFile(pWriteBuf);
//     UnmapViewOfFile(pReadBuf);
//     CloseHandle(hMapWrite);
//     CloseHandle(hMapRead);

//     return 0;
// }


// reader.cpp
// #include <windows.h>
// #include <iostream>
// #include <string>
// #include <thread>

// const wchar_t* writerToReader = L"Local\\WriterToReader";
// const wchar_t* readerToWriter = L"Local\\ReaderToWriter";
// const int bufferSize = 256 + 1;

// char* pReadBuf;
// bool running = true;

// void receiveMessage() {
//     while (running) {
//         if (strlen(pReadBuf) > 0) {
//             std::cout << "Friend: " << pReadBuf << "\n";
//             memset(pReadBuf, 0, bufferSize - 1);
//         }
//         Sleep(100);
//     }
// }

// int main() {
//     HANDLE hMapWrite = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, readerToWriter);
//     HANDLE hMapRead  = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);

//     if (!hMapWrite || !hMapRead) {
//         std::cerr << "Could not open file mapping (" << GetLastError() << ")\n";
//         return 1;
//     }

//     pReadBuf  = (char*)MapViewOfFile(hMapRead, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);

//     if (!pReadBuf) {
//         std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
//         return 1;
//     }

//     std::cout << "Chat started. Type 'exit' to end chat.\n";

//     receiveMessage();

//     UnmapViewOfFile(pReadBuf);
//     CloseHandle(hMapWrite);
//     CloseHandle(hMapRead);

//     return 0;
// }

// chat_reader.cpp
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

    HANDLE hMapWrite = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, readerToWriter);
    HANDLE hMapRead = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);

    pWriteBuf = (char*)MapViewOfFile(hMapWrite, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    pReadBuf = (char*)MapViewOfFile(hMapRead, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);

    std::cout << "Chat started (Reader). Type 'exit' to end chat.\n";
    std::thread recv(receiveMessage);
    sendMessage();
    recv.join();

    UnmapViewOfFile(pWriteBuf);
    UnmapViewOfFile(pReadBuf);
    CloseHandle(hMapWrite);
    CloseHandle(hMapRead);

    return 0;
}
