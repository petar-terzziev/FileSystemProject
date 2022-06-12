#include "FileAPI.h"

FileAPI::FileAPI(const FileSystem& fSystem)
{
    this->fileSystem = fSystem;
    currDirIdx = rootIdx;
}

void FileAPI::printWorkindDir() {
    string path = this->fileSystem.getPath(this->currDirIdx);
    cout <<path;
}

bool FileAPI::cd  (const string& dirPath) {
    int idx = this->fileSystem.getDirIdx(dirPath, this->currDirIdx);
    if(idx != invalidFileId) {
        this->currDirIdx = idx;
        return true;
    } else {
        return false;
    }
}

bool FileAPI::mkDir(const string& dirName) {
    return this->fileSystem.mkDir(dirName, this->currDirIdx);
}
bool FileAPI::rmDir(const string& dirName) {
    return this->fileSystem.rmDir(dirName, this->currDirIdx);
}
bool FileAPI::rmFile(const string& fileName) {
    return this->fileSystem.rm(fileName, this->currDirIdx);
}
void FileAPI::ls() {
    auto dirs = this->fileSystem.ls(this->currDirIdx);
    for (auto& dir : dirs) {
            cout <<dir <<endl;
    }
}

void FileAPI::write(const string& fileName, const string& fileContent, bool append){
    vector<char> vectorContent(fileContent.begin(), fileContent.end());
    this->fileSystem.write(fileName, vectorContent, this->currDirIdx, append);
}

void FileAPI::cat( const string& fileName) {
    this->fileSystem.quickPrint(fileName, this->currDirIdx);
}
void FileAPI::saveFs() {
    this->fileSystem.save();
}
 FileAPI::~FileAPI(){}
