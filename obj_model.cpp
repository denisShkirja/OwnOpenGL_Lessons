#include <iostream>
#include <cctype>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <string>
#include "obj_model.h"

static
std::string &rtrim(std::string &str)
{
    auto f = [](unsigned char const c) { return std::isgraph(c); };
    str.erase(std::find_if(str.rbegin(), str.rend(), f).base(), str.end());
    return str;
}

static
std::string &ltrim(std::string &str)
{
    auto f = [](unsigned char const c) { return std::isgraph(c); };
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), f));
    return str;
}

static
std::string &trim(std::string &str)
{
    return rtrim(ltrim(str));
}

static
void concatinate_syserrmsg(std::string &errMsg)
{
    char sys_err_msg[256];
    if (strerror_r(errno, sys_err_msg, sizeof(sys_err_msg)))
    {
        strncpy(sys_err_msg, "Unknown error", sizeof(sys_err_msg));
    }
    errMsg += " ";
    errMsg += sys_err_msg;
}

ObjModel::ObjModel(const char *p_filePath)
{
    std::fstream in_file(p_filePath, std::ios_base::in);
    if (in_file.fail())
    {
        std::string err_msg;
        err_msg += "Can not open file '";
        err_msg += p_filePath;
        err_msg += "'.";
        concatinate_syserrmsg(err_msg);
        throw std::runtime_error(err_msg);
    }

    std::string line;
    unsigned long line_number = 0;
    while (std::getline(in_file, line))
    {
        line_number++;
        trim(line);

        if (line.empty() || line[0] == '#')
        {
            continue;
        }

        std::istringstream stream(line);

        if (line[0] == 'v' && std::isspace(line[1]))
        {
            stream.ignore(2);
            std::vector<float> values(std::istream_iterator<float>{stream}, std::istream_iterator<float>{});

            if (values.size() != 3 || !stream.eof())
            {
                std::cout << "Error: worng input on line " << line_number << std::endl;
            }
            else
            {
                Vector3f v(values[0], values[1], values[2]);
                _VerticesGeometric.push_back(v);
            }
        }
        else if (line[0] == 'v' && line[1] == 't' && std::isspace(line[2]))
        {
            stream.ignore(3);
            std::vector<float> values(std::istream_iterator<float>{stream}, std::istream_iterator<float>{});

            if (values.size() != 3 || !stream.eof())
            {
                std::cout << "Error: worng input on line " << line_number << std::endl;
            }
            else
            {
            }
        }
        else if (line[0] == 'v' && line[1] == 'n' && std::isspace(line[2]))
        {
            stream.ignore(3);
            std::vector<float> values(std::istream_iterator<float>{stream}, std::istream_iterator<float>{});

            if (values.size() != 3 || !stream.eof())
            {
                std::cout << "Error: worng input on line " << line_number << std::endl;
            }
            else
            {
            }
        }
        else if (line[0] == 'f' && std::isspace(line[1]))
        {
            ;
        }
    }
    if (!in_file.eof())
    {
        std::string err_msg;
        err_msg += "Can not read line from file.";
        concatinate_syserrmsg(err_msg);
        throw std::runtime_error(err_msg);
    }
}

ObjModel::~ObjModel()
{
    return;
}
