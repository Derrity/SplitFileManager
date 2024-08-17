#ifndef NETDISK_FILEMANAGER_H
#define NETDISK_FILEMANAGER_H
#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <regex>

class FileManager {
private:
    std::string filePath;
    std::size_t chunkSize;
    std::size_t GetFileSize();
public:
    FileManager(const std::string& _filePath,const std::size_t &_chunkSize);

    size_t getNumberOfChunks();
    void splitFile();
    void mergeFiles();
    void deleteChunks();
};
#endif //NETDISK_FILEMANAGER_H
