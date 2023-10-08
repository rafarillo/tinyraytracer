#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

class Sphere
{
private:
    float radius;
    Vec3f center;
public:
    Sphere(float r, Vec3f c) : radius(r), center(c){};
    ~Sphere();
    bool RayIntersect(const Vec3f &O, const Vec3f v);
};

bool Sphere::RayIntersect(const Vec3f & O, const Vec3f v)
{
    Vec3f L = center - O;
    float tc = L * v;
    float d2 = L * L + tc * tc;
    if(tc < 0)
    {
        printf("This ray truly don't intersect");
        return false;
    }
    if(d2 > radius * radius)
    {
        printf("This ray doest intersect the sphere");
        return false;
    } 
    float t1c = sqrtf(radius * radius - d2);
    float t1 = tc - t1c;
    float t2 = tc + t1c;
    return true; 
}

void render() {
    const int width    = 1024;
    const int height   = 768;
    std::vector<Vec3f> framebuffer(width*height);

    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            framebuffer[i+j*width] = Vec3f(j/float(height),i/float(width), 0);
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

