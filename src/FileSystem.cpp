#include "../include/FileSystem.h"

FileSystem::FileSystem()
{
    auto& root = this->fileIndexes[rootIdx];
    root.isDir = true;
    root.fileName = ".";
    root.parentDirIdx = invalidFileId;
    data.reserve(inMemory);
    this->currOffset = 0;
}

FileSystem::~FileSystem()
{
    //dtor
}

//void FileSystem::catFile(const string& fileName) {
//    int currIdx = this->fileIndexes[fileName];
  //  while (!this->nodesArray[currIdx].eof) {
    //    currIdx = this->nodesArray[currIdx].nextIdx;
      //  cout<<this->nodesArray[currIdx].data<< " ";
    //}
//}

void FileSystem::quickPrint (const string& fName, int curDirIdx) {
    int fIdx  = getIdxFromName(fName, curDirIdx);
    if (fIdx != invalidFileId) {
    auto & blocks = this->fileIndexes[fIdx].Indexes;
    int fileSize = this->fileIndexes[fIdx].fileSize;
    cout <<this->getFileContent(blocks, fileSize)<<endl;
    } else {
        cout <<"No such file" <<endl;
    }


}

string FileSystem::getFileContent (const vector<int>& blocks, int fileSize) {
    string contents;
    auto& data  = this->data;
    vector<char> currBlock;
    for(int i: blocks) {
            if(getBlock(data, currBlock, idToVirtual(i)))
                contents.append(&currBlock[0], blockSize);
    }
    contents.resize(fileSize);
    contents.erase(std::remove(contents.begin(), contents.end(), 0), contents.end());
    return contents;

}

vector<string> FileSystem::ls(int dirIdx) {
    vector<string> files;
    auto& nodes = this->fileIndexes[dirIdx].Indexes;
    for(auto it : nodes){
        files.push_back(this->fileIndexes[it].fileName);
    }
    return files;
}

//copies file 1 into file2
bool FileSystem::copyFile(string file1, string file2, int currDirIdx) {
    int file1Idx= getIdxFromName(file1, currDirIdx);
    string strContent = getFileContent(this->fileIndexes[file1Idx].Indexes, this->fileIndexes[file1Idx].fileSize);
    vector<char> content(strContent.begin(), strContent.end());
    return write(file2, content, currDirIdx, false);
}


string FileSystem::getPath(int fIdx) {
    string path = this->fileIndexes[fIdx].fileName;
    int parentIdx = this->fileIndexes[fIdx].parentDirIdx;
    while (parentIdx != invalidFileId) {
        path.insert(0,1, '/');
        path.insert(0, this->fileIndexes[parentIdx].fileName);
        parentIdx = this->fileIndexes[parentIdx].parentDirIdx;
    }
    return path;

}
int FileSystem::idToVirtual (int idx) {
return idx - this->currOffset;
}
// Tries to find the closest free block, by just checking the next one and then just going through the ones in memory.
int FileSystem::getBestFree (int prevIdx) {
        if (prevIdx > maxSize)
            return -1;
        if (prevIdx+1 < maxSize && !freeBlocks.test(prevIdx+1)) {
            return prevIdx+1;
        }
        for (int i= this->currOffset; i < freeBlocks.size(); i++){
            if (!freeBlocks.test(i)) {
                return i;
            }
        }
        for(int i = 0; i< this->currOffset; i++) {
            if (!freeBlocks.test(i)) {
                return i;
            }
        }
        return -1;
}

bool FileSystem::setFree(const vector<int>& fNodes) {
for (auto it: fNodes) {
    this->freeBlocks[it] = 0;
}
}

bool FileSystem::haveToSwap (int idx) {
    int absSubs = abs(idx - this->currOffset);
    return absSubs > data.size();
}
//Checks if block with that hash exists. Returns an index or -1
int FileSystem::checkBlockDuplicate(int blockHash) {
    auto findBlock = blockHashes.find(blockHash);
    if (findBlock != blockHashes.end()) {
        return findBlock->second;
    }
    return -1;
}


bool FileSystem::getBlock(const vector<char>& content, vector<char>& block, int idx) {
    if (blockSize*idx < content.size()){
        block.insert(block.begin(), &content[blockSize*idx], &content[blockSize*(idx+1)]);
        //If we have extra space in block fill rest of block with zeroes
        if(blockSize*(idx+1) >= content.size()){
            int leftover = blockSize*(idx+1) - content.size();
            fill(block.end() - leftover, block.end(), 0);
        }
        return true;
    }
    return false;
}

int FileSystem::writeBlock (const vector<char>& block, int prevIdx) {

    int idx = getBestFree(prevIdx);
    if (haveToSwap(idx)) {
        int newOffset = idx - idx%inMemory;
        FileReader::swapData(data,this->currOffset, newOffset);
    }
    int inMemoryId = idToVirtual(idx);
    this->data.insert(data.begin() + inMemoryId*blockSize, block.begin(), block.end());
    freeBlocks.set(idx);
    return idx;

}

void FileSystem::writeContent(int fileIdx,const vector<char>& content, bool append) {
        auto& fNodes = this->fileIndexes[fileIdx].Indexes;
        // If no append reset nodes
        if (!append) {
            setFree(fNodes);
            fNodes.clear();
            this->fileIndexes[fileIdx].fileSize = content.size();
        } else {
            this->fileIndexes[fileIdx].fileSize += content.size();
        }
        int contentSize = content.size() / blockSize;
        int j = 0;
        int prevIdx = -1;
        vector <char> block;
        block.reserve(blockSize);
        while (getBlock(content, block,j)) {
            string b(block.begin(), block.end());
            int checkBlock = FileSystem::checkBlockDuplicate(hash<string>{}(b));
            if (checkBlock >= 0 ) {
             fNodes.push_back(checkBlock);
             prevIdx = checkBlock;
            } else {
                int newIdx = writeBlock(block,prevIdx);
                fNodes.push_back(newIdx);
                blockHashes[hash<string>{}(b)] = newIdx;

            }
            j++;
            block.clear();
        }
}

bool FileSystem::write(const string& fileName, const vector<char>& content, int currDirIdx, bool append) {
    int fileIdx = this->getIdxFromName(fileName, currDirIdx);
    if(fileIdx == invalidFileId && !append){
        string toHash = fileName + this->getPath(currDirIdx);
        int newFileIdx = hash<string>{} (toHash);
        auto& fData  = fileIndexes[newFileIdx];
        fData.parentDirIdx = currDirIdx;
        fData.fileSize = content.size();
        fData.isDir = false;
        fData.fileName = fileName;
        fileIdx = newFileIdx;
        this->fileIndexes[currDirIdx].Indexes.push_back(fileIdx);
    }
    this->writeContent(fileIdx, content, append);
    return true;

}

//Creates a directory with the given name in the given current directory.
bool FileSystem::mkDir(const string& dirName, int currDirIdx) {
     //Unique string, cause there is no other dir with that path
    string toHash = this->getPath(currDirIdx) + "/"  + dirName;
    int dirIdx = hash<string>{} (toHash);
    if (this->fileIndexes.find(dirIdx) == this->fileIndexes.end()){
    auto& dirData = this->fileIndexes[dirIdx];
    dirData.isDir = true;
    dirData.fileName = dirName;
    dirData.parentDirIdx = currDirIdx;
    this->fileIndexes[currDirIdx].Indexes.push_back(dirIdx);
    return true;
    }
    return false;
}
//Searches for a file/directory by name in the current directory. Returns its index.
int FileSystem::getIdxFromName(const string& fileName, int currDirIdx) {
    if (fileName == "..") {
        return fileIndexes[currDirIdx].parentDirIdx;
    } else {
        auto& filesContained = this->fileIndexes[currDirIdx].Indexes;
        for (auto idx : filesContained) {
            if (fileIndexes[idx].fileName == fileName) {
                return idx;
            }
        }
        return invalidFileId;
    }
}


// Given a relative path and a current directory, returns a file Index.
//Could possibly also be done by constructing the absolute path from the relative one and hashing it, since that's the id.
int FileSystem::getDirIdx (string dirPath, int currDirIdx){
    size_t pos = 0;
    std::string token;
    while ((pos = dirPath.find('/')) != std::string::npos) {
        token = dirPath.substr(0, pos);
        currDirIdx = this->getIdxFromName(token, currDirIdx);
        if (currDirIdx == invalidFileId || !this->fileIndexes[currDirIdx].isDir) {
            return invalidFileId;
        }
        dirPath.erase(0, pos + 1);
    }
    currDirIdx = this->getIdxFromName(dirPath, currDirIdx);
    if (currDirIdx == invalidFileId || !this->fileIndexes[currDirIdx].isDir)
        return invalidFileId;
    return currDirIdx;
}

bool FileSystem::rmDir(const string& dirName, int currDirIdx) {
    int dirIdx = this->getIdxFromName(dirName, currDirIdx);
    if (dirIdx != invalidFileId && fileIndexes[dirIdx].Indexes.size() == 0) {
        fileIndexes.erase(dirIdx);
        fileIndexes[currDirIdx].Indexes.erase(find(fileIndexes[currDirIdx].Indexes.begin(), fileIndexes[currDirIdx].Indexes.end(),dirIdx));
        return true;
    }
    return false;
}

bool FileSystem::rm(const string& fileName, int currDirIdx) {
    int fileIdx = this->getIdxFromName(fileName, currDirIdx);
    if (fileIdx != invalidFileId) {
        setFree(fileIndexes[fileIdx].Indexes);
        fileIndexes.erase(fileIdx);
        fileIndexes[currDirIdx].Indexes.erase(find(fileIndexes[currDirIdx].Indexes.begin(), fileIndexes[currDirIdx].Indexes.end(),fileIdx));
        return true;
    }
    return false;
}

void FileSystem::save() {
    const FileSystem& theFs = *this;
    int offset = this->currOffset;
    FileReader::write(theFs, offset);
}

