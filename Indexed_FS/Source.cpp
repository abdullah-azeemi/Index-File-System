#include"IndexedFileSystem.h"

int main() {
    IndexedFileSystem fs;


    while (true) {
        cout << "\nChoose an option:\n"
            "1. Create file\n"
            "2. Delete file\n"
            "3. Read file\n"
            "4. Write file\n"
            "5. List files\n"
            "6. Truncate files\n"
            "7. Create Directory\n"
            "8. Delete Directory\n"
            "9. List Files & Directories\n"
            "10. Format partition\n"
            "0. Exit\n";

        int choice;
        cin >> choice;
        cin.ignore();
        char fileName[MAX_FILE_NAME], data[BLOCK_SIZE];

        switch (choice) {
        case 1:
            cout << "Enter file name: ";
            cin.getline(fileName, MAX_FILE_NAME);
            cout << "Enter data: ";
            cin.getline(data, BLOCK_SIZE);
            fs.createFile(fileName, data);
            break;
        case 2:
            cout << "Enter file name: ";
            cin.getline(fileName, MAX_FILE_NAME);
            fs.deleteFile(fileName);
            break;
        case 3:
            cout << "Enter file name: ";
            cin.getline(fileName, MAX_FILE_NAME);
            fs.readFile(fileName);
            break;
        case 4:
            cout << "Enter file name: ";
            cin.getline(fileName, MAX_FILE_NAME);
            cout << "Enter data: ";
            cin.getline(data, BLOCK_SIZE);
            fs.writeFile(fileName, data);
            break;
        case 5:
            fs.listFiles();
            break;
        case 6:
            int newSize;
            cout << "Enter the file name: ";
            cin.getline(fileName, MAX_FILE_NAME);
            cout << "Enter the new size: ";
            cin >> newSize;
            fs.truncateFile(fileName, newSize);
            break;
        case 7:
            cout << "Enter the directory name to create: ";
            cin.getline(fileName, MAX_FILE_NAME);
            fs.createDirectory(fileName);
            break;
        case 8:
            cout << "Enter the directory name to delete: ";
            cin.getline(fileName, MAX_FILE_NAME);
            fs.deleteDirectory(fileName);
            break;
        case 9:
            fs.listFilesAndDirectories();
            break;

        case 10:
            fs.formatPartition();
            break;
        case 0:
            cout << "Exiting...\n";
            return 0;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    }
}
