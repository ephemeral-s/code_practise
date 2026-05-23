#pragma once

#include <fstream>
#include <string>
#include <sys/stat.h>

class Util
{
public:
    //从特定文件读取资源
    static bool ReadFileContent(const std::string& filename, std::string& out)
    {
        out.clear();
        std::fstream in(filename);
        if(!in.is_open())
        {
            return false;
        }
        std::string line;
        while(std::getline(in, line))
        {
            out += line;
        }
        in.close();
        return true;
    }

    //读取并删除大字符串的第一行
    static bool ReadOneLine(std::string& str, std::string& out, const std::string& sep)
    {
        size_t pos = str.find(sep);
        if(pos != std::string::npos)
        {
            out = str.substr(0, pos);
            str.erase(0, pos);
            return true;
        }
        return false;
    }

    //获取一个文件的字节数
    static ssize_t FileSize(const std::string& file)
    {
        struct stat ret;
        stat(file.c_str(), &ret);
        return ret.st_size;
    }
private:
};