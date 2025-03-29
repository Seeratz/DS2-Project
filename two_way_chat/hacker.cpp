// #include <windows.h>
// #include <iostream>
// #include <string>
// #include <thread>

// const wchar_t* writerToReader = L"Local\\WriterToReader";
// const int bufferSize = 256;

// char* pHackedBuf;

// void hackMessages() {
//     while (true) {
//         if (strlen(pHackedBuf) > 0) {
//             std::cout << "[HACKER] Intercepted message: " << pHackedBuf << "\n";

//             // Modify the message (Example: Replace words)
//             std::string hackedMessage = "HACKED: " + std::string(pHackedBuf);
//             strncpy_s(pHackedBuf, bufferSize, hackedMessage.c_str(), bufferSize);

//             std::cout << "[HACKER] Modified message: " << pHackedBuf << "\n";
//             Sleep(500); // Allow some time before next interception
//         }
//     }
// }

// int main() {
//     HANDLE hMap = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);
//     if (!hMap) {
//         std::cerr << "[HACKER] Could not open file mapping (" << GetLastError() << ")\n";
//         return 1;
//     }

//     pHackedBuf = (char*)MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
//     if (!pHackedBuf) {
//         std::cerr << "[HACKER] Could not map view of file (" << GetLastError() << ")\n";
//         return 1;
//     }

//     std::cout << "[HACKER] Hacker started. Waiting to modify messages...\n";
//     hackMessages();  // Keep hacking incoming messages

//     UnmapViewOfFile(pHackedBuf);
//     CloseHandle(hMap);

//     return 0;
// }


#include <windows.h>
#include <iostream>
#include <cstring>
#include <thread>

const wchar_t* writerToReader = L"Local\\WriterToReader";
const int bufferSize = 256;

char* pHackBuf;
bool running = true;

void hackMessage() {
    while (running) {
        if (strlen(pHackBuf) > 0) {  // Only modify if a message exists
            std::cout << "[HACKER] Intercepted message: " << pHackBuf << "\n";
            
            std::string hackedMessage = "HACKED: " + std::string(pHackBuf);
            strncpy_s(pHackBuf, bufferSize, hackedMessage.c_str(), bufferSize);
            
            std::cout << "[HACKER] Modified message: " << hackedMessage << "\n";

            Sleep(500);  // Avoid modifying too frequently
        }
    }
}

int main() {
    HANDLE hMapHack = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);
    if (!hMapHack) {
        std::cerr << "[HACKER] Could not open file mapping (" << GetLastError() << ")\n";
        return 1;
    }

    pHackBuf = (char*)MapViewOfFile(hMapHack, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    if (!pHackBuf) {
        std::cerr << "[HACKER] Could not map view of file (" << GetLastError() << ")\n";
        return 1;
    }

    std::cout << "[HACKER] Hacker started. Waiting to modify messages...\n";
    
    std::thread hackThread(hackMessage);
    hackThread.join();  // Keep running

    UnmapViewOfFile(pHackBuf);
    CloseHandle(hMapHack);

    return 0;
}
