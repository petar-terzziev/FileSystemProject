#include "FileCLI.h"
// We generally shouldn't need a FileSystem to build a cli. This should be changed.
FileCLI::FileCLI(const FileSystem& fs):
    api(fs)

{

}

void FileCLI::run() {
string command;
while (true) {
    this->api.printWorkindDir();
    cout <<"> ";
    getline(cin, command);
    parseCommand(command);
    saveFs();
}
}


 void FileCLI::parseCommand(const string& command) {
     size_t pos;
     if (command == "ls")
        parseLs();
     else if ( command == "help")
        parseHelp();
     else if ((pos = command.find(' ')) != std::string::npos){
        string type = command.substr(0, pos);
        if(type == "mkdir"){
        parseMkDir(command.substr(pos+1, command.size()));
        }
        else if(type == "rmdir"){
        parseRmDir(command.substr(pos+1, command.size()));
        }
        else if(type == "rm"){
        parseRmFile(command.substr(pos+1, command.size()));
        }
        else if (type == "cd") {
            parseCD(command.substr(pos+1, command.size()));
        }
        else if(type == "write") {
            const string args = command.substr(pos+1, command.size());
            parseWrite(args);
        }
        else if (type == "cat") {
            parseCat(command.substr(pos+1, command.size()));
        }
        } else {
        cout <<"Invalid command. Type help to view commands" <<endl;
     }
 }
 void FileCLI::parseCD(const string& path) {
     if (!this->api.cd(path)) {
        cout << "No such directory." <<endl;
     }

 }
    void FileCLI::parseLs() {
        this->api.ls();

    }
void FileCLI::parseMkDir(const string& dirName) {
      if (checkDirName(dirName)) {
        if (!this->api.mkDir(dirName)) {
            cout << "Directory already exists." <<endl;
        }
      } else {
        cout <<"Invalid directory name" <<endl;
      }
 }

void FileCLI::parseRmDir(const string& dirName) {
      if (checkDirName(dirName)) {
        if (!this->api.rmDir(dirName)) {
            cout << "Directory not empty" <<endl;
        }
      } else {
        cout <<"Invalid directory name" <<endl;
      }
 }

void FileCLI::parseRmFile(const string& fileName) {
      if (checkFileName(fileName)) {
        if (!this->api.rmFile(fileName)) {
            cout << "Directory not empty" <<endl;
        }
      } else {
        cout <<"Invalid directory name" <<endl;
      }
 }


 bool FileCLI::checkDirName (const string& dirName) {
     regex reg("[A-Za-z0-9]");
     return regex_search(dirName, reg);
 }

  bool FileCLI::checkFileName (const string& dirName) {
     regex reg2("[A-Za-z0-9]+\\.+[A-Za-z0-9]+");
     return regex_match(dirName, reg2);
 }

 void FileCLI::parseWrite(const string& arguments) {
     size_t endOfFileName;
     size_t contentBegins;
     size_t contentEnds;
     size_t optionPosition;
     //Maybe nested ifs are one bit too many here. Later separate into functions or something.
     if ((endOfFileName = arguments.find(' ')) != std::string::npos) {
         string fileName = arguments.substr(0, endOfFileName);
         if (!checkFileName(fileName)) {
            cout <<"Invalid file name" <<endl;
            return;
         }
         if ((contentBegins = arguments.find('"')) != std::string::npos &&
             (contentEnds = arguments.substr(contentBegins+1, arguments.size()).find('"')) != std::string::npos) {
                    string content = arguments.substr(contentBegins+1, contentEnds);
                    bool append = false;
                    if ((optionPosition = arguments.substr(contentEnds+1, arguments.size()).find("--")) != std::string::npos) {
                        append = (arguments.substr(optionPosition, arguments.size()) == "--append");
                    }
                        this->api.write(fileName, content, append);

         }

     } else {
        cout <<"Invalid arguments for the command" <<endl;
     }


 }

void FileCLI::parseCat(const string & fileName) {
     this->api.cat(fileName);
 }

void FileCLI::parseHelp(){
    cout <<" Commands are: \n ls - Lists files and directories in the current directory \n cd - Changes directory \n write {filename}.{extension} \"{content} \" [--append] - Writes to file \n mkdir - creates a directory in the current directory. \n rmdir- Removes an empty directory. \n rm - Removes a file." <<endl;

 }

 void FileCLI::saveFs () {
    this->api.saveFs();
 }

FileCLI::~FileCLI() {
}
