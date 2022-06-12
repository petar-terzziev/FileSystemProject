#ifndef FILECLI_H
#define FILECLI_H
#include "FileAPI.h"
#include <regex>
class FileCLI
{
    public:
        FileCLI(const FileSystem& fs);
        virtual ~FileCLI();
        void run();
    protected:

    private:
        void parseCommand(const string& command);
        void parseMkDir(const string& dirName);
        void parseRmDir(const string& dirName);
        void parseRmFile(const string& fileName);
        bool checkDirName (const string& dirName);
        bool checkFileName (const string& dirName);
        void parseLs();
        void parseHelp();
        void parseWrite(const string& arguments);
        void parseCat(const string& fileName);
        void saveFs();
        FileAPI api;
        void parseCD(const string& dirPath);
};

#endif // FILECLI_H
