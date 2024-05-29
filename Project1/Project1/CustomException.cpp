#include "CustomException.h"
#include <sstream>
CustomException::CustomException(int inputLine, const char* inputFile)
{
    file = inputFile;
    line = inputLine;
}

const char* CustomException::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* CustomException::GetType() const
{
    return "Custom Exception";
}

int CustomException::GetLine() const
{
    return line;
}

const std::string& CustomException::GetFile() const
{
    return file;
}

std::string CustomException::GetOriginString() const
{
    std::ostringstream oss;
    oss << "[File] " << file << std::endl;
    oss << "[Line] " << line;
    return oss.str();
}
