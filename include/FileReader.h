#ifndef FILEREADER_H
#define FILEREADER_H
#include "FileSystem.h"
using namespace std;
class FileReader
{
    public:
        FileReader()=default;
        virtual ~FileReader()=default;
        FileReader(const FileReader& other)=default;
        FileReader& operator=(const FileReader& other)=default;
        static void read( int offset, FileSystem& fS);
        static void write(const FileSystem& fS, int offset);
        void quickWrite( int dataBlocks, int offset);
        static void swapData(vector<char>& data, int prevOffset, int offset);

    protected:

    private:
};

#endif // FILEREADER_H
