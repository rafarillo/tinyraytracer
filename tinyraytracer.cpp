#define _USE_MATH_DEFINES
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

const double FOV = M_PI/2.0;

class Sphere
{
private:
    float radius;
    Vec3f center;
public:
    Sphere(float r, Vec3f c) : radius(r), center(c){};
    ~Sphere(){};
    bool RayIntersect(const Vec3f &O, const Vec3f v);
};

bool Sphere::RayIntersect(const Vec3f & O, const Vec3f v)
{
    Vec3f L = center - O;
    float tc = L * v;
    float d2 = L * L - tc * tc;
    if(tc < 0)
    {
        return false;
    }
    if(sqrtf(d2) > radius)
    {
        return false;
    } 
    float t1c = sqrtf(radius * radius - d2);
    float t1 = tc - t1c;
    float t2 = tc + t1c;
    return true; 
}

void render() {
    const float width    = 1024;
    const float height   = 768;
    std::vector<Vec3f> framebuffer(width*height);
    Sphere f(2, Vec3f(-3, 0, -16));
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            float rate = 2*tan(FOV/2.0f);
            float x = (2*(i + 0.5)/(float)width  - 1)*tan(FOV/2.)*width/(float)height ;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(FOV/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            if(f.RayIntersect(Vec3f(0, 0, 0), dir))
            {
                // printf("Ray intersectin at (%.3f, %.3f) \n", x, y);
                framebuffer[i+j*width] = Vec3f(0.4, 0.4, 0.3);
            }
            else
            {
                // printf("Ray no intersectin at (%.3f, %.3f) \n", x, y);
                framebuffer[i+j*width] = Vec3f(0.2, 0.7, 0.8);
            }
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm", std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {
    render();

    return 0;
}

