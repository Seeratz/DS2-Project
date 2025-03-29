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


#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

const wchar_t* writerToReader = L"Local\\WriterToReader";
const wchar_t* readerToWriter = L"Local\\ReaderToWriter";
const int bufferSize = 256;

char* pWriteBuf;
char* pReadBuf;
bool running = true;

void sendMessage() {
    while (running) {
        std::string message;
        std::getline(std::cin, message);
        strncpy_s(pWriteBuf, bufferSize, message.c_str(), bufferSize);

        if (message == "exit") {
            running = false;
            break;
        }
    }
}

void receiveMessage() {
    while (running) {
        if (strlen(pReadBuf) > 0) {
            std::cout << "Friend: " << pReadBuf << "\n";
            memset(pReadBuf, 0, bufferSize);
        }
        Sleep(100);
    }
}

int main() {
    HANDLE hMapWrite = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, readerToWriter);
    HANDLE hMapRead = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, writerToReader);

    if (!hMapWrite || !hMapRead) {
        std::cerr << "Could not open file mapping (" << GetLastError() << ")\n";
        return 1;
    }

    pWriteBuf = (char*)MapViewOfFile(hMapWrite, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);
    pReadBuf = (char*)MapViewOfFile(hMapRead, FILE_MAP_ALL_ACCESS, 0, 0, bufferSize);

    if (!pWriteBuf || !pReadBuf) {
        std::cerr << "Could not map view of file (" << GetLastError() << ")\n";
        return 1;
    }

    std::cout << "Chat started. Type 'exit' to end chat.\n";

    std::thread receiveThread(receiveMessage);
    sendMessage();  // Main thread handles sending

    receiveThread.join();  // Ensure receiving thread stops

    UnmapViewOfFile(pWriteBuf);
    UnmapViewOfFile(pReadBuf);
    CloseHandle(hMapWrite);
    CloseHandle(hMapRead);

    return 0;
}
