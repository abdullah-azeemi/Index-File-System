#include"IndexedFileSystem.h"


void IndexedFileSystem::saveState(bool encrypt = false) {
    ofstream diskFile("disk.bin", ios::binary);
    if (encrypt) {
        for (auto& block : disk) {
            encryptDecrypt(block.data, BLOCK_SIZE);
        }
    }
    diskFile.write(reinterpret_cast<const char*>(disk.data()), DISK_SIZE);
    diskFile.write(reinterpret_cast<const char*>(directory.data()), MAX_FILES * sizeof(FileEntry));
    for (const auto& [index, blocks] : allocationTable) {
        int numAllocatedBlocks = blocks.size();
        diskFile.write(reinterpret_cast<const char*>(&numAllocatedBlocks), sizeof(int));
        diskFile.write(reinterpret_cast<const char*>(blocks.data()), numAllocatedBlocks * sizeof(int));
    }
}

void IndexedFileSystem::createFile(const char* name, const char* data) {
    char encryptionKey = 'K';
    auto dirIt = find_if(directory.begin(), directory.end(), [](const FileEntry& entry) { return entry.isDeleted; });
    if (dirIt == directory.end()) {
        cout << "Directory is full, cannot create a new file.\n";
        return;
    }

    int blocksNeeded = (strlen(data) + BLOCK_SIZE - 1) / BLOCK_SIZE;
    vector<int> fileBlocks;
    for (int i = 0; i < NUM_BLOCKS && fileBlocks.size() < blocksNeeded; ++i) {
        if (all_of(allocationTable.begin(), allocationTable.end(), [&](const auto& pair) {
            return std::find(pair.second.begin(), pair.second.end(), i) == pair.second.end();
            })) {
            fileBlocks.push_back(i);
        }
    }

    if (fileBlocks.size() < blocksNeeded) {
        cout << "Not enough space to create file.\n";
        return;
    }

    int dataIndex = 0;
    for (int blockIndex : fileBlocks) {
        strncpy(disk[blockIndex].data, data + dataIndex, BLOCK_SIZE);
        encrypt(disk[blockIndex].data, BLOCK_SIZE, encryptionKey);
        dataIndex += BLOCK_SIZE;
    }

    strncpy(dirIt->name, name, MAX_FILE_NAME);
    dirIt->startBlock = fileBlocks.front();
    dirIt->sizeInBlocks = blocksNeeded;
    dirIt->isDeleted = false;
    allocationTable[std::distance(directory.begin(), dirIt)] = std::move(fileBlocks);

    cout << "File '" << name << "' created successfully.\n";
    saveState();
}
void IndexedFileSystem::deleteFile(const char* name) {
    auto dirIt = find_if(directory.begin(), directory.end(), [&](const FileEntry& entry) {
        return strncmp(entry.name, name, 64) == 0 && !entry.isDeleted && !entry.isDirectory;
        });

    if (dirIt == directory.end()) {
        cout << "File not found or is a directory.\n";
        return;
    }

    dirIt->isDeleted = true;

    for (int blockIndex : allocationTable[distance(directory.begin(), dirIt)]) {
        memset(disk[blockIndex].data, 0, BLOCK_SIZE);
    }

    allocationTable.erase(std::distance(directory.begin(), dirIt));

    cout << "File '" << name << "' deleted successfully.\n";

    saveState();
}

void IndexedFileSystem::truncateFile(const char* name, int newSize) {
    auto dirIt = find_if(directory.begin(), directory.end(), [&](const FileEntry& entry) {
        return strncmp(entry.name, name, MAX_FILE_NAME) == 0 && !entry.isDeleted;
        });
    if (dirIt == directory.end()) {
        cout << "File not found.\n";
        return;
    }

    int newBlocksNeeded = (newSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int currentBlocks = dirIt->sizeInBlocks;

    if (newBlocksNeeded > currentBlocks) {
        cout << "Truncation size exceeds current file size.\n";
        return;
    }

    for (int i = newBlocksNeeded; i < currentBlocks; ++i) {
        memset(disk[allocationTable[distance(directory.begin(), dirIt)][i]].data, 0, BLOCK_SIZE);
    }

    dirIt->sizeInBlocks = newBlocksNeeded;
    allocationTable[distance(directory.begin(), dirIt)].resize(newBlocksNeeded);
    cout << "File '" << name << "' truncated to " << newSize << " bytes.\n";
    saveState();
}
void IndexedFileSystem::createDirectory(const char* dirName) {
   
    auto dirIt = find_if(directory.begin(), directory.end(), [](const FileEntry& entry) { return entry.isDeleted; });
    if (dirIt == directory.end()) {
        cout << "No space left to create a new directory.\n";
        return;
    }

    int blocksNeeded = 1; 
    vector<int> fileBlocks;

    for (int i = 0; i < NUM_BLOCKS && fileBlocks.size() < blocksNeeded; ++i) {
        if (all_of(allocationTable.begin(), allocationTable.end(), [&](const auto& pair) {
            return std::find(pair.second.begin(), pair.second.end(), i) == pair.second.end();
            })) {
            fileBlocks.push_back(i);
        }
    }

    if (fileBlocks.size() < blocksNeeded) {
        cout << "Not enough space to create directory.\n";
        return;
    }

    
    int dataIndex = 0;
    for (int blockIndex : fileBlocks) {
        strncpy(disk[blockIndex].data, "", BLOCK_SIZE);  
        dataIndex += BLOCK_SIZE;
    }

    strncpy(dirIt->name, dirName, MAX_FILE_NAME);
    dirIt->startBlock = fileBlocks.front();
    dirIt->sizeInBlocks = 0;  
    dirIt->isDeleted = false;
    dirIt->isDirectory = true; 
    allocationTable[std::distance(directory.begin(), dirIt)] = std::move(fileBlocks);

    cout << "Directory '" << dirName << "' created successfully.\n";
    saveState();
}

void IndexedFileSystem::deleteDirectory(const char* dirName) {
    auto dirIt = find_if(directory.begin(), directory.end(), [&](const FileEntry& entry) {
        return strncmp(entry.name, dirName, MAX_FILE_NAME) == 0 && !entry.isDeleted && entry.isDirectory;
        });
    if (dirIt == directory.end()) {
        cout << "Directory not found or is a file.\n";
        return;
    }

    dirIt->isDeleted = true;
    for (int blockIndex : allocationTable[distance(directory.begin(), dirIt)]) {
        memset(disk[blockIndex].data, 0, BLOCK_SIZE);
    }
    allocationTable.erase(std::distance(directory.begin(), dirIt));

    cout << "Directory '" << dirName << "' deleted successfully.\n";
    saveState();
}


void IndexedFileSystem::listAll() {
    for (const auto& entry : directory) {
        if (!entry.isDeleted) {
            cout << (entry.sizeInBlocks == 0 ? "[DIR] " : "[FILE] ") << entry.name << '\n';
        }
    }
}

void IndexedFileSystem::readBlock(int blockIndex) {
    if (blockIndex < 0 || blockIndex >= NUM_BLOCKS) {
        cout << "Invalid block index.\n";
        return;
    }
    cout << "Block " << blockIndex << ": " << disk[blockIndex].data << '\n';
}

void IndexedFileSystem::writeBlock(int blockIndex, const char* data) {
    if (blockIndex < 0 || blockIndex >= NUM_BLOCKS) {
        cout << "Invalid block index.\n";
        return;
    }
    strncpy(disk[blockIndex].data, data, BLOCK_SIZE);
    cout << "Block " << blockIndex << " written.\n";
    saveState();
}

void IndexedFileSystem::formatPartition() {
    directory.assign(MAX_FILES, FileEntry{});
    disk.assign(NUM_BLOCKS, Block{});
    allocationTable.clear();
    cout << "Partition formatted.\n";
    saveState();
}
void IndexedFileSystem::readFile(const char* name) {
    auto dirIt = find_if(directory.begin(), directory.end(), [&](const FileEntry& entry) {
        return strncmp(entry.name, name, 64) == 0 && !entry.isDeleted;
        });

    if (dirIt == directory.end()) {
        cout << "File not found or deleted.\n";
        return;
    }

    char encryptionKey = 'K';  
    for (int blockIndex : allocationTable[distance(directory.begin(), dirIt)]) {
        decrypt(disk[blockIndex].data, BLOCK_SIZE, encryptionKey);
        cout << disk[blockIndex].data;
    }

    cout << '\n';  
}

void IndexedFileSystem::writeFile(const char* name, const char* data) {
    auto dirIt = find_if(directory.begin(), directory.end(), [&](const FileEntry& entry) {
        return strncmp(entry.name, name, MAX_FILE_NAME) == 0 && !entry.isDeleted;
        });
    if (dirIt == directory.end()) {
        cout << "File not found.\n";
        return;
    }

    int dataIndex = 0;
    for (int blockIndex : allocationTable[distance(directory.begin(), dirIt)]) {
        strncpy(disk[blockIndex].data, data + dataIndex, BLOCK_SIZE);
        dataIndex += BLOCK_SIZE;
    }
    cout << "Data written to file '" << name << "'.\n";
    saveState();
}

void IndexedFileSystem::listFiles() {
    cout << "Files in the directory:\n";
    for (const auto& entry : directory) {
        if (!entry.isDeleted && !entry.isDirectory) {
            cout << entry.name << '\n';
        }
    }
}

void IndexedFileSystem::listFilesAndDirectories() {
    cout << "Files and Directories:\n";

    for (const auto& entry : directory) {
        if (!entry.isDeleted) {
            if (entry.sizeInBlocks == 0) {  
                cout << "[DIR] " << entry.name << '\n';
            }
            else {
                cout << "[FILE] " << entry.name << '\n';
            }
        }
    }
}

