#include "../include/FileReader.h"

void FileReader::swapData (vector<char>& data,int prevOffset, int offset) {
    std::fstream file("FSFile.txt");
    int metaDataSize;
    file.read(reinterpret_cast<char*> (&metaDataSize), sizeof(int));
    file.seekp(prevOffset + metaDataSize, file.beg);
    file.write(&data[0],inMemory*blockSize);
    file.seekg(offset + metaDataSize, file.beg);
    file.read(&data[0],inMemory*blockSize);

}

//Mock function to test write/read FS functionality. Obsolete now as it wouldn't write a valid fs
void FileReader::quickWrite(int dataBlocks, int offset) {
    std::ofstream file("FSFile.txt",std::ios::out);
    int bS = blockSize;
    bool s = false;
    vector<char> data;
    data.reserve(dataBlocks*blockSize);
    file.seekp(sizeof(int), file.beg);
    for(int i = 0; i < dataBlocks; i++) {
        for (int j = 0; j < blockSize; j++) {
            data.push_back('w');
        }
    }
    int numOfFiles = 2;
    file.write(reinterpret_cast<char*>(&numOfFiles), sizeof(int));
    int fIdx;
    FileSystem::FileData fData {};
    fData.fileName = "alabala";
    fData.parentDirIdx = 1;
    fData.isDir = false;
    int numOfNodes;
    vector<char> fileNodes;
    for (int i = 0; i<numOfFiles; i++) {
        file.write(reinterpret_cast<char*> (&fIdx), sizeof(int));
        file.write(fData.fileName.c_str(), fData.fileName.size() + 1);
        file.write(reinterpret_cast<char*> (&fData.parentDirIdx), sizeof(int));
        file.write(reinterpret_cast<char*> (&fData.isDir), sizeof(bool));
        file.write(reinterpret_cast<char*> (&numOfNodes), sizeof(int));
        fileNodes.reserve(numOfNodes);
        file.write(reinterpret_cast<char*> (&fileNodes[0]), sizeof(int)*numOfNodes);

    }
    int numOfHashes;
    file.write(reinterpret_cast<char*>(&numOfHashes), sizeof(int));
    int blockHash;
    int blockIdx;
    for (int i = 0; i<numOfHashes; i++) {
        blockHash =i;
        blockIdx = i+1;
        file.write(reinterpret_cast<char*> (&blockHash), sizeof(int));
        file.write(reinterpret_cast<char*> (&blockIdx), sizeof(int));
    }
    int metaDataSize = file.tellp();
    file.seekp(0, file.beg);
    file.write(reinterpret_cast<char*>(&metaDataSize), sizeof(int));
    file.seekp(offset + metaDataSize, file.beg);
    file.write(&data[0],dataBlocks*blockSize);
}


void FileReader::write(const FileSystem& fS, int offset) {
    std::ofstream file("FSFile.txt",std::ios::out | std::ios::binary);
    auto& data = fS.data;
    int bS = blockSize;
    bool s = false;
    file.write(reinterpret_cast<char*>(&bS), sizeof(int));
    int numOfFiles = fS.fileIndexes.size();
    auto& fileIndexes = fS.fileIndexes;
    string freeBlocks =fS.freeBlocks.to_string();
    file.write(freeBlocks.c_str(), freeBlocks.size());
    file.write(reinterpret_cast<char*>(&numOfFiles), sizeof(int));
    for (auto& it : fileIndexes) {
        file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.first)), sizeof(int));
        int nameSize = it.second.fileName.size();
        file.write(reinterpret_cast<char*> (&nameSize), sizeof(int));
        file.write(it.second.fileName.c_str(), it.second.fileName.size());
        file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.second.fileSize)), sizeof(int));
        file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.second.parentDirIdx)), sizeof(int));
        file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.second.isDir)), sizeof(bool));
        int s = it.second.Indexes.size();
        file.write(reinterpret_cast<char*> (&s), sizeof(int));
        for(int j = 0; j<s; j++) {
            file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.second.Indexes[j])), sizeof(int));
        }
    }
    int numOfHashes = fS.blockHashes.size();
    file.write(reinterpret_cast<char*>(&numOfHashes), sizeof(int));
    int blockHash;
    int blockIdx;
    for (auto it: fS.blockHashes) {
        file.write(const_cast<char*>(reinterpret_cast<const char*> (&it.first)), sizeof(int));
        file.write(reinterpret_cast<char*> (&it.second), sizeof(int));
    }
    int dataSize = data.size();
    file.write(reinterpret_cast<char*> (&dataSize), sizeof(int));
    int metaDataSize = file.tellp();
    file.seekp(0, file.beg);
    file.write(reinterpret_cast<char*>(&metaDataSize), sizeof(int));
    file.seekp(offset + metaDataSize, file.beg);
    file.write(&data[0],dataSize);
    file.close();
}

void FileReader::read(int offset, FileSystem& fS) {
    std::ifstream file("FSFile.txt",std::ios::in |std::ios::binary);
    file.seekg(0, file.end);
    //We assume that if there's something in the file, it's a valid file system.
    if (file.tellg() == 0) {
        return;
    } else {
        file.seekg(0, file.beg);
    }
    auto& blockHashes = fS.blockHashes;
    auto& fileIndexes = fS.fileIndexes;
    int bS = blockSize;
    int numOfFiles;
    int metaDataSize;

    file.read(reinterpret_cast<char*>(&metaDataSize), sizeof(int));
    string bitsetString;
    bitsetString.resize(maxSize);
    file.read(const_cast<char*>(bitsetString.c_str()), maxSize);
    bitset<maxSize> fBlocks(bitsetString.c_str());
    fS.freeBlocks = fBlocks;
    file.read(reinterpret_cast<char*>(&numOfFiles), sizeof(int));
    int fIdx;
    FileSystem::FileData fData {};
    int pIdx;
    bool isDir;
    vector<int> fileNodes;
    string fName;
    fName.reserve(maxNameSize);
    int nameSize;
    for (int i = 0; i<numOfFiles; i++) {
        file.read(reinterpret_cast<char*> (&fIdx), sizeof(int));
        file.read(reinterpret_cast<char*> (&nameSize), sizeof(int));
        fName.resize(nameSize);
        file.read(const_cast <char*>(fName.c_str()), nameSize);
        file.read(reinterpret_cast<char*> (&fData.fileSize), sizeof(int));
        file.read(reinterpret_cast<char*> (&fData.parentDirIdx), sizeof(int));
        file.read(reinterpret_cast<char*> (&fData.isDir), sizeof(bool));
        int numOfNodes;
        file.read(reinterpret_cast<char*> (&numOfNodes), sizeof(int));
        fData.Indexes.resize(numOfNodes);
        for(int j = 0; j<numOfNodes; j++) {
            file.read(reinterpret_cast<char*> (&fData.Indexes[j]), sizeof(int));
        }
        fData.fileName = fName;
        fileIndexes[fIdx] = fData;
        fData.Indexes.clear();

    }
    int numOfHashes;
    file.read(reinterpret_cast<char*>(&numOfHashes), sizeof(int));
    int blockHash;
    int blockIdx;
    for (int i = 0; i<numOfHashes; i++) {
        file.read(reinterpret_cast<char*> (&blockHash), sizeof(int));
        file.read(reinterpret_cast<char*> (&blockIdx), sizeof(int));
        blockHashes[blockHash] = blockIdx;
    }
    int dataSize;
    file.read(reinterpret_cast<char*> (&dataSize), sizeof(int));
    file.seekg(offset + metaDataSize, file.beg);
    vector<char> data;
    data.resize(dataSize);
    file.read(&data[0],dataSize);
    fS.data = data;
    file.close();


}
