#include <chrono>
#include <iostream>
#include <fstream>
#include <algorithm>

#include "math/point.hpp"
#include "core/ray_tracer.hpp"

#include "core/object.hpp"
#include "object/plane.hpp"
#include "object/sphere.hpp"
#include "object/triangle.hpp"
#include "object/triangle_mesh.hpp"

#include "core/material.hpp"
#include "material/glass.hpp"
#include "material/mirror.hpp"
#include "material/glossy.hpp"
#include "material/diffuse.hpp"

#include "core/light.hpp"
#include "light/area_light.hpp"
#include "light/point_light.hpp"
#include "light/distant_light.hpp"

#include "core/sampler.hpp"
#include "sampler/uniform_sampler.hpp"

#include "accelerator/AABB_tree.hpp"

#include "core/microfacet.hpp"
#include "microfacet/Beckmann.hpp"
#include "microfacet/TrowbridgeReitz.hpp"

int main(int argc, char **argv) {
    Screen screen(1080, 1080, 40.0);

    Scene scene;

    shared_ptr<Material> red = make_shared<Diffuse>(kDiffuse, Vector3d(0.63, 0.065, 0.05), make_shared<UniformHemiSampler3D>());
    shared_ptr<Material> green = make_shared<Diffuse>(kDiffuse, Vector3d(0.14, 0.45, 0.091), make_shared<UniformHemiSampler3D>());
    shared_ptr<Material> white = make_shared<Diffuse>(kDiffuse, Vector3d(0.725, 0.71, 0.68), make_shared<UniformHemiSampler3D>());

    TriangleMesh left_wall = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/left.obj", red);
    scene.AddObject(make_shared<TriangleMesh>(left_wall));

    TriangleMesh right_wall = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/right.obj", green);
    scene.AddObject(make_shared<TriangleMesh>(right_wall));

    TriangleMesh back_wall = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/back.obj", white);
    scene.AddObject(make_shared<TriangleMesh>(back_wall));

    TriangleMesh floor = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/floor.obj", white);
    scene.AddObject(make_shared<TriangleMesh>(floor));

    TriangleMesh ceiling = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/ceiling.obj", white);
    scene.AddObject(make_shared<TriangleMesh>(ceiling));

    // TriangleMesh tall_cube = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/tall_cube.obj", white);
    // scene.AddObject(make_shared<TriangleMesh>(tall_cube));

    // TriangleMesh tall_cube1 = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/tall_cube1.obj", white);
    // scene.AddObject(make_shared<TriangleMesh>(tall_cube1));

    // shared_ptr<Material> mirror = make_shared<Mirror>(kMirror, Vector3d(), Vector3d(0.8));
    // TriangleMesh tall_cube2 = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/tall_cube2.obj", mirror);
    // scene.AddObject(make_shared<TriangleMesh>(tall_cube2));

    // TriangleMesh short_cube = TriangleMesh::CreateTriangleMesh("../models/Cornellbox/short_cube.obj", white);
    // scene.AddObject(make_shared<TriangleMesh>(short_cube));

    // shared_ptr<Material> glass = make_shared<Glass>(kGlass, Vector3d(), 1.5, Vector3d(1.0), Vector3d(1.0));
    shared_ptr<MicrofacetDistribution> GGX = make_shared<TrowbridgeReitzDistribution>(TrowbridgeReitzDistribution::RoughnessToAlpha(0.1));
    shared_ptr<Material> glossy = make_shared<Glossy>(kGlossy, Vector3d(), Vector3d(1.0), GGX);

    Sphere big_sphere(Point3d(-10.0, -15.0, -110.0), 10.0, glossy);
    scene.AddObject(make_shared<Sphere>(big_sphere));
    
    Sphere small_sphere(Point3d(14.0, -19.0, -91.0), 6.0, glossy);
    scene.AddObject(make_shared<Sphere>(small_sphere));

    Vector3d radiance = Vector3d(30.0);
    Point3d pos(0.0, 24.5, -100.0);
    Vector3d width(15.0, 0.0, 0.0);
    Vector3d height(0.0, 0.0, -15.0);
    Vector3d normal(0.0, -1.0, 0.0);
    AreaLight area_light(radiance, pos, width, height, normal, make_shared<UniformGridSampler2D>());
    scene.AddLight(make_shared<AreaLight>(area_light));

    auto objects = scene.objects();
    scene.BuildAABBTree(make_shared<AABBTree>(4, objects));

    RayTracer ray_tracer(screen, scene);

    std::cout << "Rendering..." << std::endl;
    auto start = std::chrono::system_clock::now();
    ray_tracer.Render();
    auto stop  = std::chrono::system_clock::now();

    std::cout << "Render complete: \n";
    std::cout << "Time taken: " << std::chrono::duration_cast<std::chrono::hours>  (stop - start).count() << " hours\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::minutes>(stop - start).count() << " minutes\n";
    std::cout << "          : " << std::chrono::duration_cast<std::chrono::seconds>(stop - start).count() << " seconds\n";

    return 0;
}

