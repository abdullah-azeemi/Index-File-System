# Indexed File System

## Description

This project implements a simple **Indexed File System** using a custom disk structure. It allows for basic file management features such as creating, reading, writing, deleting, truncating files, and managing directories. The system uses block-level storage and supports encryption for added security. It also provides an option to format the partition and save the state of the file system.

### Key Features:
- **File Management:**
  - Create, delete, and truncate files.
  - Read and write data to files.
  - Support for files up to 64 bytes in name size.
  
- **Directory Management:**
  - Create and delete directories.
  - List files and directories.

- **Encryption:**
  - Files are encrypted using a simple XOR encryption method during writing and decryption during reading for enhanced security.

- **File Allocation:**
  - Indexed allocation method is used to allocate blocks for files.
  
- **State Management:**
  - Saves the state of the file system to a disk file (`disk.bin`), supporting both encrypted and unencrypted storage.

## Requirements

- C++20
- Standard C++ library
- A C++ compiler (e.g., GCC)

## Functions

### File Operations:
- **createFile(name, data)**  
  Creates a file with the specified name and data.

- **readFile(name)**  
  Reads the content of a file, decrypts the data if encryption is enabled.

- **writeFile(name, data)**  
  Writes the specified data to a file, encrypts it if encryption is enabled.

- **deleteFile(name)**  
  Deletes a file from the file system.

- **truncateFile(name, newSize)**  
  Truncates a file to the specified size (in bytes).

### Directory Operations:
- **createDirectory(dirName)**  
  Creates a new directory.

- **deleteDirectory(dirName)**  
  Deletes an existing directory.

- **listFiles()**  
  Lists all files in the file system, excluding directories.

- **listFilesAndDirectories()**  
  Lists all files and directories, excluding deleted ones.

### Partition Operations:
- **formatPartition()**  
  Formats the file system, clearing all files and directories.

- **saveState(encrypt)**  
  Saves the current state of the file system to `disk.bin`. Supports encryption.

### Utility Functions:
- **listAll()**  
  Lists all files and directories in the system (including deleted ones).

- **readBlock(blockIndex)**  
  Reads a block of data from the disk.

- **writeBlock(blockIndex, data)**  
  Writes data to a specific block on the disk.

## Compilation

To compile the program, you can use the following command (assuming you're using `g++` as the compiler):

```bash
g++ -std=c++20 -o IndexedFileSystem IndexedFileSystem.cpp
```

## Usage

1. **Create a file system**:  
   On the first run, the disk file (`disk.bin`) will be empty. You can format the partition and start creating files and directories.

2. **Create a file**:  
   ```cpp
   createFile("file1.txt", "This is the content of file1.");
   ```

3. **Read a file**:  
   ```cpp
   readFile("file1.txt");
   ```

4. **Delete a file**:  
   ```cpp
   deleteFile("file1.txt");
   ```

5. **Create a directory**:  
   ```cpp
   createDirectory("mydir");
   ```

6. **List files and directories**:  
   ```cpp
   listFilesAndDirectories();
   ```

7. **Format the partition**:  
   ```cpp
   formatPartition();
   ```

8. **Encrypt and save**:  
   You can save the state of the file system with or without encryption:
   ```cpp
   saveState(true);  // With encryption
   saveState(false); // Without encryption
   ```

## Bonus Features

- **Variable File Name Size**: File names can now be up to 64 bytes in length.
- **Partition Encryption**: Optionally encrypt the file system before saving.
