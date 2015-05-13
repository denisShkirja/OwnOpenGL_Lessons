#pragma once

#include <vector>
#include "geometry.h"
#include "tgaimage.h"

class ObjModel
{
    public:
        ObjModel(const char *p_filePath);
        ~ObjModel();

        bool LoadDiffuseTexture(const char *p_filePath);
        TGAColor GetColor(unsigned long x, unsigned long y);

        Vector3f GetVertexGeometric(unsigned long i) { return _VerticesGeometric[i]; }
        Vector3f GetVertexNormal(unsigned long i)    { return _VerticesNormals[i]; }
        Vector2l GetVertexTexture(unsigned long i);

        std::size_t GetFacesCount()    { return _FacesVertex.size(); }
        std::vector<unsigned long> GetFaceVertices(unsigned long i) { return _FacesVertex[i]; }
        std::vector<unsigned long> GetFaceTextures(unsigned long i) { return _FacesTexture[i];}
        std::vector<unsigned long> GetFaceNormals(unsigned long i)  { return _FacesNormal[i]; }

    private:
        std::vector<Vector3f> _VerticesGeometric;
        std::vector<Vector2f> _VerticesTexture;
        std::vector<Vector3f> _VerticesNormals;

        std::vector<std::vector<unsigned long> > _FacesVertex;
        std::vector<std::vector<unsigned long> > _FacesTexture;
        std::vector<std::vector<unsigned long> > _FacesNormal;

        TGAImage _DiffuseTexture;
};
