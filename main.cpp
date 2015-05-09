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

void triangle(TGAImage &image, Vector3l v0, Vector3l v1, Vector3l v2,
              TGAColor color, std::vector<long> &zbuffer)
{
    if (v0.y > v1.y)    std::swap(v0, v1);
    if (v0.y > v2.y)    std::swap(v0, v2);
    if (v1.y > v2.y)    std::swap(v1, v2);

    if (v0.y == v2.y)
    {
        return;
    }

    float total_hight = v2.y - v0.y;
    float low_sector_hight = v1.y - v0.y;
    float high_sector_hight = v2.y - v1.y;

    for (long y = v0.y; y <= v2.y; y++)
    {
        Vector3l left = v0 + (v2 - v0) * ((y - v0.y) / total_hight);
        Vector3l right;
        if (y <= v1.y)
        {
            float ratio = (low_sector_hight == 0) ? 1 : (y - v0.y) / low_sector_hight;
            right = v0 + (v1 - v0) * ratio;
        }
        else
        {
            float ratio = (high_sector_hight == 0) ? 1 : (y - v1.y) / high_sector_hight;
            right = v1 + (v2 - v1) * ratio;
        }

        if (left.x > right.x)
        {
            std::swap(left, right);
        }
        for (long x = left.x; x <= right.x; x++)
        {
            float ratio = (right.x == left.x) ? 1 : ((float)(x - left.x) / (right.x - left.x));
            Vector3l curr = left + (right - left) * ratio;

            unsigned long width = image.get_width();
            unsigned long offset = x + width * y;
            if (zbuffer[offset] < curr.z)
            {
                zbuffer[offset] = curr.z;
                image.set(x, y, color);
            }
        }
    }
}

int main(int argc, char** argv)
{
    TGAImage image(800, 800, TGAImage::RGB);

    std::shared_ptr<ObjModel> model;
    if (argc == 2)
    {
        model = std::make_shared<ObjModel>(argv[1]);
    }
    else
    {
        model = std::make_shared<ObjModel>("./african_head.obj");
    }

    Vector3f light = {0, 0, -1};
    std::vector<long> zbuffer;
    for (int i = 0; i < image.get_width() * image.get_height(); i++)
    {
        zbuffer.push_back(std::numeric_limits<int>::min());
    }

    for (std::size_t i = 0; i < model->GetFacesCount(); i++)
    {
        std::vector<unsigned long> face_vertices = model->GetFaceVertices(i);
        assert(face_vertices.size() == 3);

        Vector3f world_coords[3];
        Vector3l screen_coords[3];

        for (long i = 0; i < 3; i++)
        {
            const unsigned long depth = 255;
            world_coords[i] = model->GetVertexGeometric(face_vertices[i]);
            screen_coords[i] = Vector3l(std::round((world_coords[i].x + 1.) * image.get_width() / 2.),
                                        std::round((world_coords[i].y + 1.) * image.get_height() / 2.),
                                        std::round((world_coords[i].z + 1.) * depth / 2.));
        }
        Vector3f norm = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        norm.normalize();
        float intensity = norm * light;
        if (intensity > 0)
        {
            triangle(image, screen_coords[0], screen_coords[1], screen_coords[2],
                     TGAColor(intensity * 255, intensity * 255, intensity * 255, 255),
                     zbuffer);
        }
    }
    image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
    image.write_tga_file("output.tga");

    return 0;
}

