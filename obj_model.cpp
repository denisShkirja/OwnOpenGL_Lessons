#include <iostream>
#include <regex>
#include <stdexcept>
#include <fstream>
#include <string>
#include "obj_model.h"

static void concatinate_syserrmsg(std::string &errMsg)
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

    std::string float_regex = R"(((?:\+|-)?[0-9]+(?:\.[0-9]+)?(?:(?:e|E)(?:\+|-)?[0-9]+)?))";
    std::regex vertices_regex(R"(^(v(?:[tnp])?)[\s]+)" + float_regex +    // compulsory field
                                R"((?:[\s]+)" + float_regex + R"()?)" +   // optional fields
                                R"((?:[\s]+)" + float_regex + R"()?)" +
                                R"((?:[\s]+)" + float_regex + R"()?)" +
                              R"([\s]*$)");
    std::string geom_tex_norm_v = R"(([0-9]+)(?:(?:\/\/([0-9]+))|(?:\/([0-9]+)(?:\/([0-9]+))?))?)";
    std::regex faces_regex(R"(^f(([\s]+)" + geom_tex_norm_v + R"(){3,})[\s]*$))");
    std::regex comment_regex(R"(^[\s]*#.*$)");

    std::string in_str;
    while (std::getline(in_file, in_str))
    {
        std::smatch match;

        if (in_str.empty() || std::regex_match(in_str, match, comment_regex))
        {
            continue;
        }
        else if (std::regex_match(in_str, match, vertices_regex))
        {
            if (match[1] == "v")
            {
                float x = std::stof(match[2]);
                float y = std::stof(match[3]);
                float z = std::stof(match[4]);

                Vector3f vect(x, y, z);
                _VerticesGeometric.push_back(vect);
            }
            else if (match[1] == "vt")
            {
            }
            else if (match[1] == "vn")
            {
            }
            else if (match[1] == "vp")
            {
            }
        }
        else if(std::regex_match(in_str, match, faces_regex))
        {
            std::string face = match[1];

            std::vector<unsigned long> face_v_indices;
            std::vector<unsigned long> face_vt_indices;
            std::vector<unsigned long> face_vn_indices;

            bool error = false;
            std::regex face_elements(geom_tex_norm_v);
            while (std::regex_search(face, match, face_elements))
            {
                unsigned long v = 0;
                unsigned long vt = 0;
                unsigned long vn = 0;

                if (match[6].str().empty() == false)
                {
                    v = std::stoul(match[6]) - 1;
                    vt = std::stoul(match[7]) - 1;
                    vn = std::stoul(match[8]) - 1;
                    face_v_indices.push_back(v);
                    face_vt_indices.push_back(vt);
                    face_vn_indices.push_back(vn);
                }
                else if (match[4].str().empty() == false)
                {
                    v = std::stoul(match[4]) - 1;
                    vn = std::stoul(match[5]) - 1;
                    face_v_indices.push_back(v);
                    face_vn_indices.push_back(vn);
                }
                else if (match[2].str().empty() == false)
                {
                    v = std::stoul(match[2]) - 1;
                    vt = std::stoul(match[3]) - 1;
                    face_v_indices.push_back(v);
                    face_vt_indices.push_back(vt);
                }
                else if (match[1].str().empty() == false)
                {
                    v = std::stoul(match[1]) - 1;
                    face_v_indices.push_back(v);
                }
                else
                {
                    std::cerr << "\nCan not find indices in face match '" << face << "'.";
                    error = true;
                    break;
                }

                face = match.suffix();
            }

            if (error == false)
            {
                _FacesVertex.push_back(face_v_indices);
                if (face_vt_indices.empty() == false)
                {
                    _FacesTexture.push_back(face_vt_indices);
                }
                if (face_vn_indices.empty() == false)
                {
                    _FacesNormal.push_back(face_vn_indices);
                }
            }
        }
        else
        {
            std::cerr << "Can not parse line '" << in_str << "' in file '" << p_filePath << "'.\n";
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
