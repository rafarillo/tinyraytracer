#define _USE_MATH_DEFINES
#include <limits>
#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include "geometry.h"

const double FOV = M_PI/2.0;
const Vec3f O = Vec3f(0.0, 0.0, 0.0);

class Light
{
public:
    Light(Vec3f position, float i) : pos(position), intensity(i){}
    Vec3f pos;
    float intensity;
};

class Material
{
public:
    Vec3f diffuse_color;
    float kd, ks;
    float shineness;
    Material(Vec3f color, float kd, float ks, float shineness) : diffuse_color(color), kd(kd), ks(ks), shineness(shineness){}
    Material() : diffuse_color(){}
};

class Sphere
{
private:
    float radius;
    Vec3f center;
    Material material;
public:
    Sphere(float r, Vec3f c, Material color) : radius(r), center(c), material(color){};
    ~Sphere(){};
    bool RayIntersect(const Vec3f &O, const Vec3f v, float & t1, float &t2);
    Material & GetMaterial(){return material;}
    Vec3f GetCenter(){return center;}
};

bool Sphere::RayIntersect(const Vec3f & O, const Vec3f v, float & t1, float &t2)
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
    t1 = tc - t1c;
    t2 = tc + t1c;
    return true; 
}

bool SceneIntersect(Vec3f O, Vec3f v, std::vector<Sphere> spheres, Vec3f &hit, Vec3f &N, Material &mat)
{
    bool intersect = false;
    for(auto s : spheres)
    {
        float t1;
        float t2;
        bool i = s.RayIntersect(O, v, t1, t2);
        if(i)
        {
            intersect = i;
            hit = O + v * t1;
            N = (hit - s.GetCenter()).normalize();
            mat = s.GetMaterial();
        }
        
    }
    return intersect;
}

Vec3f CastRay(Vec3f O, Vec3f v, std::vector<Sphere> spheres, std::vector<Light> lights)
{
    Vec3f hit, N;
    Material mat;
    if(!SceneIntersect(O, v, spheres, hit, N, mat))
    {
        return Vec3f(0.2, 0.7, 0.8);
    }

    float diffuseLightIntensity = 0.0;
    float specularLightIntensity = 0.0f;
    for(auto light : lights)
    {
        Vec3f lightDir = (light.pos - hit).normalize(); 
        Vec3f R =  (lightDir - N * 2.0f * (lightDir * N)).normalize();

        float lightHitDist = sqrt((light.pos - hit).norm());
        Vec3f shadowOrig = hit;
        Vec3f shadowHit, shadowN;
        Material shadowMat;
        if(SceneIntersect(shadowOrig, lightDir, spheres, shadowHit, shadowN, shadowMat))
        {
            continue;
        }
        diffuseLightIntensity += light.intensity + std::max(0.0f, N * lightDir);
        specularLightIntensity += powf(std::max(0.0f, R * v), mat.shineness) * light.intensity;
    }

    return mat.diffuse_color * diffuseLightIntensity * mat.kd + Vec3f(1.0, 1.0, 1.0) * mat.ks * specularLightIntensity;
}

void render(std::vector<Sphere> spheres, std::vector<Light> lights) {
    const float width    = 1024;
    const float height   = 768;
    std::vector<Vec3f> framebuffer(width*height);
    for (size_t j = 0; j<height; j++) {
        for (size_t i = 0; i<width; i++) {
            float rate = 2*tan(FOV/2.0f);
            float x = (2*(i + 0.5)/(float)width  - 1)*tan(FOV/2.)*width/(float)height ;
            float y = -(2*(j + 0.5)/(float)height - 1)*tan(FOV/2.);
            Vec3f dir = Vec3f(x, y, -1).normalize();
            framebuffer[i + j*width] = CastRay(O, dir, spheres, lights);
        }
    }

    std::ofstream ofs; // save the framebuffer to file
    ofs.open("./out.ppm", std::ofstream::out | std::ofstream::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (size_t i = 0; i < height*width; ++i) {
        for (size_t j = 0; j<3; j++) {
            ofs << (char)(255 * std::max(0.f, std::min(1.f, framebuffer[i][j])));
        }
    }
    ofs.close();
}

int main() {

    Material ivory(Vec3f(0.4, 0.4, 0.3), 0.6, 0.3, 50);
    Material red_rubber(Vec3f(0.3, 0.1, 0.1), 0.9, 0.1, 10);

    Sphere f1(2, Vec3f(-3, 0, -16), ivory);
    Sphere f2(2, Vec3f(-1.0, -1.5, -12), red_rubber);
    Sphere f3(3, Vec3f( 1.5, -0.5, -18), red_rubber);
    Sphere f4(4, Vec3f( 7, 5, -18), ivory);

    Light l1(Vec3f(-20, 20, 20), 1.5);

    std::vector<Sphere> spheres = {f1, f2, f3, f4};
    std::vector<Light> lights = {l1};
    render(spheres, lights);

    return 0;
}

