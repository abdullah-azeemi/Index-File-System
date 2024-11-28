#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <algorithm> 
using namespace std;

constexpr int MAX_FILE_NAME = 64;
constexpr int MAX_FILES = 128;
constexpr int BLOCK_SIZE = 1024;            // 1 KB
constexpr int DISK_SIZE = 64 * 1024 * 1024; // 64 MB
constexpr int NUM_BLOCKS = DISK_SIZE / BLOCK_SIZE;

struct Block {
    char data[BLOCK_SIZE]{};
};


struct FileEntry {
    char name[MAX_FILE_NAME];
    int startBlock = -1;
    int sizeInBlocks = 0;
    bool isDeleted = true;
    bool isDirectory;
};


class IndexedFileSystem {
private:
    vector<Block> disk;
    vector<FileEntry> directory;
    map<int, vector<int>> allocationTable;

    template <typename InputIterator, typename Predicate>
    bool all_of(InputIterator first, InputIterator last, Predicate pred) {
        for (; first != last; ++first) {
            if (!pred(*first)) {
                return false;
            }
        }
        return true;
    }


    void encryptDecrypt(char* data, size_t size) {
        char key = 'k';
        for (size_t i = 0; i < size; ++i) {
            data[i] ^= key;
        }
    }
    void encrypt(char* data, size_t length, char key) {
        for (size_t i = 0; i < length; ++i) {
            data[i] ^= key;  
        }
    }

    void decrypt(char* data, size_t length, char key) {
        encrypt(data, length, key);  
    }
public:
    IndexedFileSystem() : disk(NUM_BLOCKS), directory(MAX_FILES) {
        ifstream diskFile("disk.bin", ios::binary);
        if (diskFile) {
            diskFile.read(reinterpret_cast<char*>(disk.data()), DISK_SIZE);
            diskFile.read(reinterpret_cast<char*>(directory.data()), MAX_FILES * sizeof(FileEntry));
            int numAllocatedBlocks;
            for (int i = 0; i < MAX_FILES; ++i) {
                if (!directory[i].isDeleted) {
                    diskFile.read(reinterpret_cast<char*>(&numAllocatedBlocks), sizeof(int));
                    std::vector<int> blocks(numAllocatedBlocks);
                    diskFile.read(reinterpret_cast<char*>(blocks.data()), numAllocatedBlocks * sizeof(int));
                    allocationTable[i] = std::move(blocks);
                }
            }
        }
        else {
            ofstream newDiskFile("disk.bin", ios::binary);
        }
    }

    void saveState(bool encrypt);
    void createFile(const char* name, const char* data);
    void deleteFile(const char* name);

    void truncateFile(const char* name, int newSize);
    void createDirectory(const char* dirName);
    void deleteDirectory(const char* dirName);

    void listAll();

    void readBlock(int blockIndex);

    void writeBlock(int blockIndex, const char* data);

    void formatPartition();
    void readFile(const char* name);

    void writeFile(const char* name, const char* data);

    void listFiles();
    void listFilesAndDirectories();
};
