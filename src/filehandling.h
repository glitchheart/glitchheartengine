#include <iostream>
#include <fstream>

static const char* LoadFile(const std::string Path)
{
    std::string Content;
    std::ifstream FileStream(Path, std::ios::in);

    if(!FileStream.is_open()) {
        std::cerr << "Could not read file " << Path << ". File does not exist." << std::endl;
        return "";
    }

    std::string Line = "";
    while(!FileStream.eof()) {
        std::getline(FileStream, Line);
        Content.append(Line + "\n");
    }

    FileStream.close();

    return Content.c_str();
}