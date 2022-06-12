#ifndef FILEAPI_H
#define FILEAPI_H
#include "FileSystem.h"

class FileAPI
{
    public:
        FileAPI(const FileSystem& fSystem);
        virtual ~FileAPI();
        FileAPI& operator=(const FileAPI& rhs)=default;
        void printWorkindDir();
        bool cd (const string& filePath);
        bool mkDir(const string& dirName);
        bool rmDir(const string& dirName);
        bool rmFile(const string& fileName);
        void ls();
        void write(const string& fileName, const string& fileContent, bool append);
        void cat(const string& fileName);
        void saveFs();

    private:
        FileSystem fileSystem;
        int currDirIdx;
};

#endif // FILEAPI_H
