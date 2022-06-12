#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <bitset>
#include <algorithm> //for std::remove
class FileSystem;
#include "FileReader.h"
using namespace std;
//Should be passed by the user eventually. All global variables should go.
const int blockSize = 4;
//It's a very small FS
const int maxSize = 81920;
const int inMemory = 50;
const int maxNameSize = 50;
//We won't allow files or directories other than root to begin with a dot.
const string invalidFileName="..";
const int invalidFileId = hash<string>{}(invalidFileName);
const int rootIdx = hash<string> {}(".");

class FileSystem
{
    public:
        FileSystem();
        virtual ~FileSystem();
        FileSystem(const FileSystem& other)=default;
        FileSystem& operator=(const FileSystem& other)=default;
        void quickPrint(const string& fName, int curDirIdx);
        bool write(const string& fileName, const vector<char>& content,int currDirIdx,bool append);
        bool copyFile(string lhs, string rhs, int currDirIdx);
        vector<string> ls(int dirIdx);
        bool mkDir(const string& dirName, int currDirIdx);
        bool rmDir (const string& dirName, int currDirIdx);
        bool rm(const string& fileName, int currDirIdx);
        string getPath(int fileIdx);
        string getName (int fileIdx);
        int getDirIdx(string dirPath, int currDirIdx);
        void save();


    protected:

    private:
        int checkBlockDuplicate(int block);
        void findFree();
        // This struct represents metaData for files and directories.
        struct FileData {
            string fileName;
            int parentDirIdx;
            int fileSize;
            bool isDir = false;
            //If it's a directory it stores fileIndexes. Otherwise blockIndexes.
            vector<int> Indexes;
        };
        int idToVirtual (int idx);
        //Checks for the closest blocks or gives a random free one.
        int getBestFree (int prevIdx);
        //Is the block in RAM or is it on the file.
        bool haveToSwap (int idx);
        bool setFree(const vector<int>& Indexes);
        bool getBlock(const vector<char>& content, vector<char>& block, int idx);
        int writeBlock (const vector<char>& block, int prevIdx);
        int getIdxFromName(const string& fName, int currDirIdx);
        void writeContent(int fileIdx, const vector<char>& content,bool append);
        string getFileContent(const vector<int>& blocks, int fileSize);
        bitset<maxSize> freeBlocks;
        vector<char> data; //indexes are actual indexes (+ an offset if needed).
        //Maybe fileMetaData would've been a more fitting name for this variable...
        unordered_map<int, FileData> fileIndexes;
        unordered_map<int, int> blockHashes;
        int currOffset = 0; //Which part of the fileSystem is in the RAM
        friend class FileReader;
};

#endif // FILESYSTEM_H
