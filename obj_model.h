#pragma once

#include <vector>
#include "geometry.h"

class ObjModel
{
    public:
        ObjModel(const char *p_filePath);
        ~ObjModel();

        Vector3f GetVertexGeometric(unsigned long i)   { return _VerticesGeometric[i]; }

        std::vector<unsigned long> GetFaceVertices(unsigned long i) { return _FacesVertex[i]; }
        std::vector<unsigned long> GetFaceTextures(unsigned long i) { return _FacesTexture[i];}
        std::vector<unsigned long> GetFaceNormals(unsigned long i)  { return _FacesNormal[i]; }

        std::size_t GetFacesCount()    { return _FacesVertex.size(); }

    private:
        std::vector<Vector3f> _VerticesGeometric;
        std::vector<std::vector<unsigned long> > _FacesVertex;
        std::vector<std::vector<unsigned long> > _FacesTexture;
        std::vector<std::vector<unsigned long> > _FacesNormal;
};
