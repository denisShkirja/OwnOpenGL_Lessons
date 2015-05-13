#include <cassert>
#include <iostream>
#include <memory>
#include <limits>
#include <cmath>
#include <utility>

#include "tgaimage.h"
#include "obj_model.h"

const TGAColor white  = TGAColor(255, 255, 255, 255);
const TGAColor red    = TGAColor(255, 0,   0,   255);
const TGAColor blue   = TGAColor(0,   0,   255, 255);
const TGAColor green  = TGAColor(0,   255, 0,   255);
const TGAColor purple = TGAColor(255, 0,   255, 255);

void line(TGAImage &image, int x0, int y0, int x1, int y1, TGAColor color)
{
    bool xy_swap = false;

    if (std::abs(y1 - y0) > std::abs(x1 - x0))
    {
        xy_swap = true;
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int x_delta = x1 - x0;
    int y_delta = 2 * std::abs(y1 - y0);
    int y_error = 0;
    int y_curr = y0;
    for (int x_curr = x0; x_curr < x1; x_curr++)
    {
        if (xy_swap)
        {
            image.set(y_curr, x_curr, color);
        }
        else
        {
            image.set(x_curr, y_curr, color);
        }

        y_error += y_delta;
        if (y_error > x_delta)
        {
            y_curr += (y1 > y0) ? 1 : -1;
            y_error -= 2 * x_delta;
        }
    }
}

void triangle(TGAImage &image, std::array<Vector3l, 3> &v, std::array<Vector3f, 3> &n,
              std::array<Vector2l, 3> &u, std::vector<long> &zbuffer, ObjModel &model, Vector3f &light)
{
    if (v[0].y > v[1].y)
    {
        std::swap(v[0], v[1]);
        std::swap(n[0], n[1]);
        std::swap(u[0], u[1]);
    }
    if (v[0].y > v[2].y)
    {
        std::swap(v[0], v[2]);
        std::swap(n[0], n[2]);
        std::swap(u[0], u[2]);
    }
    if (v[1].y > v[2].y)
    {
        std::swap(v[1], v[2]);
        std::swap(n[1], n[2]);
        std::swap(u[1], u[2]);
    }

    if (v[0].y == v[2].y)
    {
        return;
    }

    float total_hight = v[2].y - v[0].y;
    float low_sector_hight = v[1].y - v[0].y;
    float high_sector_hight = v[2].y - v[1].y;

    for (long y = v[0].y; y <= v[2].y; y++)
    {
        Vector2l left_u = u[0] + (u[2] - u[0]) * ((y - v[0].y) / total_hight);
        Vector3l left_v = v[0] + (v[2] - v[0]) * ((y - v[0].y) / total_hight);
        Vector3f left_n = n[0] + (n[2] - n[0]) * ((y - v[0].y) / total_hight);
        Vector2l right_u;
        Vector3l right_v;
        Vector3f right_n;

        if (y <= v[1].y)
        {
            float ratio = (low_sector_hight == 0) ? 1 : (y - v[0].y) / low_sector_hight;
            right_v = v[0] + (v[1] - v[0]) * ratio;
            right_u = u[0] + (u[1] - u[0]) * ratio;
            right_n = n[0] + (n[1] - n[0]) * ratio;
        }
        else
        {
            float ratio = (high_sector_hight == 0) ? 1 : (y - v[1].y) / high_sector_hight;
            right_v = v[1] + (v[2] - v[1]) * ratio;
            right_u = u[1] + (u[2] - u[1]) * ratio;
            right_n = n[1] + (n[2] - n[1]) * ratio;
        }

        if (left_v.x > right_v.x)
        {
            std::swap(left_v, right_v);
            std::swap(left_u, right_u);
            std::swap(left_n, right_n);
        }
        for (long x = left_v.x; x <= right_v.x; x++)
        {
            float ratio = (right_v.x == left_v.x) ? 1 : ((float)(x - left_v.x) / (right_v.x - left_v.x));
            long z = left_v.z + (right_v.z - left_v.z) * ratio;

            unsigned long width = image.get_width();
            unsigned long offset = x + width * y;
            if (zbuffer[offset] < z)
            {
                Vector3f curr_n = left_n + (right_n - left_n) * ratio;
                curr_n.normalize();
                float intensity = curr_n * light;
                if (intensity > 0)
                {
                    Vector2l curr_u = left_u + (right_u - left_u) * ratio;
                    TGAColor color = model.GetColor(curr_u.x, curr_u.y);
                    color = TGAColor(color.r * intensity, color.g * intensity, color.b * intensity, color.a);

                    zbuffer[offset] = z;
                    image.set(x, y, color);
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    TGAImage image(800, 800, TGAImage::RGB);

    std::shared_ptr<ObjModel> model;
    if (argc == 3)
    {
        model = std::make_shared<ObjModel>(argv[1]);
        if (!model->LoadDiffuseTexture(argv[2]))
        {
            std::cerr << std::endl << "Error: Can't load " << argv[2] << ".";
            return 0;
        }
    }
    else
    {
        model = std::make_shared<ObjModel>("./african_head.obj");
        if (!model->LoadDiffuseTexture("./african_head_diffuse.tga"))
        {
            std::cerr << std::endl << "Error: Can't load african_head_diffuse.tga.";
            return 0;
        }
    }

    Vector3f light = {0, 0, 1};
    std::vector<long> zbuffer;
    for (int i = 0; i < image.get_width() * image.get_height(); i++)
    {
        zbuffer.push_back(std::numeric_limits<int>::min());
    }

    for (std::size_t i = 0; i < model->GetFacesCount(); i++)
    {
        std::vector<unsigned long> face_vertices = model->GetFaceVertices(i);
        std::vector<unsigned long> face_textures = model->GetFaceTextures(i);
        std::vector<unsigned long> face_normals = model->GetFaceNormals(i);
        assert(face_vertices.size() == 3);
        assert(face_vertices.size() == 3);
        assert(face_normals.size() == 3);

        std::array<Vector3f, 3> world_coords;
        std::array<Vector3f, 3> normal_coords;
        std::array<Vector3l, 3> screen_coords;
        std::array<Vector2l, 3> texture_coords;

        for (long i = 0; i < 3; i++)
        {
            const unsigned long depth = 255;
            texture_coords[i] = model->GetVertexTexture(face_textures[i]);
            normal_coords[i] = model->GetVertexNormal(face_normals[i]);
            world_coords[i] = model->GetVertexGeometric(face_vertices[i]);
            screen_coords[i] = Vector3l(std::round((world_coords[i].x + 1.) * image.get_width() / 2.),
                                        std::round((world_coords[i].y + 1.) * image.get_height() / 2.),
                                        std::round((world_coords[i].z + 1.) * depth / 2.));
        }
        triangle(image, screen_coords, normal_coords, texture_coords,
                    zbuffer, *model, light);
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    return 0;
}

