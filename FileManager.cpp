#include "FileManager.h"

std::size_t FileManager::getNumberOfChunks() {
    std::size_t numberOfChunks = 0;
    for (const auto &entry: std::filesystem::directory_iterator(std::filesystem::current_path())) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(this->filePath + ".part") == 0) {
                numberOfChunks++;
            }
        }
    }
    return numberOfChunks;
}

FileManager::FileManager(const std::string& _filePath,const std::size_t &_chunkSize):filePath(_filePath),chunkSize(_chunkSize) {}

void FileManager::splitFile() {
    std::ifstream inputFile(this->filePath, std::ios::binary | std::ios::ate);
    if (!inputFile.is_open()) {
        std::cerr << "Error: Could not open file " << this->filePath << std::endl;
        return;
    }
    std::size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);
    if (fileSize <= this->chunkSize) {
        std::cout << "File size is less than or equal to chunk size, no splitting needed." << std::endl;
        inputFile.close();
        return;
    }
    std::size_t numChunks = fileSize / this->chunkSize;
    std::size_t lastChunkSize = fileSize % this->chunkSize;
    if (lastChunkSize > 0) {
        numChunks++;
    }
    std::vector<char> buffer(this->chunkSize);
    for (std::size_t i = 0; i < numChunks; ++i) {
        std::size_t currentChunkSize = (i == numChunks - 1 && lastChunkSize > 0) ? lastChunkSize : chunkSize;
        buffer.resize(currentChunkSize);
        inputFile.read(buffer.data(), currentChunkSize);
        std::ofstream outputFile(this->filePath + ".part" + std::to_string(i), std::ios::binary);
        outputFile.write(buffer.data(), currentChunkSize);
    }
    inputFile.close();
}

void FileManager::mergeFiles() {
    std::size_t numberOfChunks = getNumberOfChunks();
    std::ofstream outputFile(this->filePath, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Cannot open file: " << this->filePath << std::endl;
        return;
    }
    for (std::size_t i = 0; i < numberOfChunks; ++i) {
        std::ifstream inputFile(this->filePath + ".part" + std::to_string(i), std::ios::binary);
        if (!inputFile) {
            std::cerr << "Cannot open chunk file: " << this->filePath << ".part" << i << std::endl;
            continue;
        }
        outputFile << inputFile.rdbuf();
        inputFile.close();
        std::filesystem::remove(this->filePath + ".part" + std::to_string(i));
    }
    outputFile.close();
}

void FileManager::deleteChunks() {
    std::regex chunkPattern(this->filePath+"\\.part\\d+");

    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path())) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, chunkPattern)) {
                std::filesystem::remove(entry.path());
                //std::cout << "Deleted chunk file: " << filename << std::endl;
            }
        }
    }
}