#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <cmath>

SceneNode::SceneNode() 
    : mesh(nullptr), localTransform(glm::mat4(1.0f)), isEmissive(false), isRoad(false), texture(nullptr) {}

SceneNode::SceneNode(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, bool isEmissive, bool isRoad, std::shared_ptr<Texture> texture)
    : mesh(mesh), localTransform(transform), isEmissive(isEmissive), isRoad(isRoad), texture(texture) {}

void SceneNode::addChild(const SceneNode& child) {
    children.push_back(child);
}

void SceneNode::draw(const Shader& shader, const glm::mat4& parentTransform) const {
    glm::mat4 globalTransform = parentTransform * localTransform;

    if (mesh) {
        shader.setBool("isEmissive", isEmissive);
        shader.setBool("isRoad", isRoad);

        if (texture) {
            shader.setBool("hasTexture", true);
            texture->bind(0);
            shader.setInt("diffuseTexture", 0);
        } else {
            shader.setBool("hasTexture", false);
        }

        shader.setMat4("model", globalTransform);
        mesh->draw();
    }

    for (const auto& child : children) {
        child.draw(shader, globalTransform);
    }
}

Scene::Scene() {}
Scene::~Scene() {}

std::shared_ptr<Mesh> Scene::createPlane(float width, float depth, const glm::vec3& color, float tileX, float tileY) {
    float hw = width * 0.5f, hd = depth * 0.5f;
    std::vector<Vertex> vertices = {
        { {-hw, 0.0f, -hd}, {0.0f, 1.0f, 0.0f}, color, {0.0f, tileY} }, { { hw, 0.0f, -hd}, {0.0f, 1.0f, 0.0f}, color, {tileX, tileY} },
        { { hw, 0.0f,  hd}, {0.0f, 1.0f, 0.0f}, color, {tileX, 0.0f} }, { {-hw, 0.0f,  hd}, {0.0f, 1.0f, 0.0f}, color, {0.0f, 0.0f} }
    };
    std::vector<unsigned int> indices = { 0, 2, 1, 0, 3, 2 };
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Scene::createBox(float width, float height, float depth, const glm::vec3& color) {
    float w = width * 0.5f, h = height * 0.5f, d = depth * 0.5f;
    std::vector<Vertex> vertices = {
        { {-w, -h,  d}, { 0.0f,  0.0f,  1.0f}, color, {0.0f, 0.0f} }, { { w, -h,  d}, { 0.0f,  0.0f,  1.0f}, color, {1.0f, 0.0f} },
        { { w,  h,  d}, { 0.0f,  0.0f,  1.0f}, color, {1.0f, 1.0f} }, { {-w,  h,  d}, { 0.0f,  0.0f,  1.0f}, color, {0.0f, 1.0f} },
        { { w, -h, -d}, { 0.0f,  0.0f, -1.0f}, color, {0.0f, 0.0f} }, { {-w, -h, -d}, { 0.0f,  0.0f, -1.0f}, color, {1.0f, 0.0f} },
        { {-w,  h, -d}, { 0.0f,  0.0f, -1.0f}, color, {1.0f, 1.0f} }, { { w,  h, -d}, { 0.0f,  0.0f, -1.0f}, color, {0.0f, 1.0f} },
        { {-w,  h,  d}, { 0.0f,  1.0f,  0.0f}, color, {0.0f, 0.0f} }, { { w,  h,  d}, { 0.0f,  1.0f,  0.0f}, color, {1.0f, 0.0f} },
        { { w,  h, -d}, { 0.0f,  1.0f,  0.0f}, color, {1.0f, 1.0f} }, { {-w,  h, -d}, { 0.0f,  1.0f,  0.0f}, color, {0.0f, 1.0f} },
        { {-w, -h, -d}, { 0.0f, -1.0f,  0.0f}, color, {0.0f, 0.0f} }, { { w, -h, -d}, { 0.0f, -1.0f,  0.0f}, color, {1.0f, 0.0f} },
        { { w, -h,  d}, { 0.0f, -1.0f,  0.0f}, color, {1.0f, 1.0f} }, { {-w, -h,  d}, { 0.0f, -1.0f,  0.0f}, color, {0.0f, 1.0f} },
        { { w, -h,  d}, { 1.0f,  0.0f,  0.0f}, color, {0.0f, 0.0f} }, { { w, -h, -d}, { 1.0f,  0.0f,  0.0f}, color, {1.0f, 0.0f} },
        { { w,  h, -d}, { 1.0f,  0.0f,  0.0f}, color, {1.0f, 1.0f} }, { { w,  h,  d}, { 1.0f,  0.0f,  0.0f}, color, {0.0f, 1.0f} },
        { {-w, -h, -d}, {-1.0f,  0.0f,  0.0f}, color, {0.0f, 0.0f} }, { {-w, -h,  d}, {-1.0f,  0.0f,  0.0f}, color, {1.0f, 0.0f} },
        { {-w,  h,  d}, {-1.0f,  0.0f,  0.0f}, color, {1.0f, 1.0f} }, { {-w,  h, -d}, {-1.0f,  0.0f,  0.0f}, color, {0.0f, 1.0f} }
    };
    std::vector<unsigned int> indices = {
         0,  1,  2,  0,  2,  3,   4,  5,  6,  4,  6,  7,
         8,  9, 10,  8, 10, 11,  12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,  20, 21, 22, 20, 22, 23
    };
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Scene::createCylinder(float radius, float height, int segments, const glm::vec3& color, bool capEnds) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float hh = height * 0.5f, angleStep = 2.0f * 3.14159265f / segments;

    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep, cosA = std::cos(angle), sinA = std::sin(angle);
        glm::vec3 normal(cosA, 0.0f, sinA);
        vertices.push_back({ glm::vec3(cosA * radius, -hh, sinA * radius), normal, color, glm::vec2(float(i)/segments, 0.0f) });
        vertices.push_back({ glm::vec3(cosA * radius,  hh, sinA * radius), normal, color, glm::vec2(float(i)/segments, 1.0f) });
    }
    for (int i = 0; i < segments; ++i) {
        unsigned int b0 = 2 * i, t0 = 2 * i + 1, b1 = 2 * (i + 1), t1 = 2 * (i + 1) + 1;
        indices.push_back(b0); indices.push_back(b1); indices.push_back(t1);
        indices.push_back(b0); indices.push_back(t1); indices.push_back(t0);
    }
    if (capEnds) {
        unsigned int topCenter = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0.0f, hh, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.5f, 0.5f) });
        unsigned int topRing = static_cast<unsigned int>(vertices.size());
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            vertices.push_back({ glm::vec3(std::cos(angle) * radius, hh, std::sin(angle) * radius), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)) });
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(topCenter); indices.push_back(topRing + i); indices.push_back(topRing + i + 1);
        }
        unsigned int botCenter = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0.0f, -hh, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(0.5f, 0.5f) });
        unsigned int botRing = static_cast<unsigned int>(vertices.size());
        for (int i = 0; i <= segments; ++i) {
            float angle = i * angleStep;
            vertices.push_back({ glm::vec3(std::cos(angle) * radius, -hh, std::sin(angle) * radius), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(0.5f + 0.5f * std::cos(angle), 0.5f + 0.5f * std::sin(angle)) });
        }
        for (int i = 0; i < segments; ++i) {
            indices.push_back(botCenter); indices.push_back(botRing + i + 1); indices.push_back(botRing + i);
        }
    }
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Scene::createCornerCurb(float centerX, float centerZ, float radius, float startAngleRad, float endAngleRad, float innerX, float innerZ, float height, int segments, const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    unsigned int topInnerIdx = 0;
    vertices.push_back({ glm::vec3(innerX, height, innerZ), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.5f, 0.5f) });
    unsigned int botInnerIdx = 1;
    vertices.push_back({ glm::vec3(innerX, 0.0f, innerZ), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(0.5f, 0.5f) });

    unsigned int arcStartIdx = static_cast<unsigned int>(vertices.size());
    for (int i = 0; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments, angle = startAngleRad + t * (endAngleRad - startAngleRad);
        float cosA = std::cos(angle), sinA = std::sin(angle);
        glm::vec3 radialNorm(cosA, 0.0f, sinA);

        vertices.push_back({ glm::vec3(centerX + radius * cosA, height, centerZ + radius * sinA), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(t, 1.0f) });
        vertices.push_back({ glm::vec3(centerX + radius * cosA, 0.0f, centerZ + radius * sinA), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(t, 0.0f) });
        vertices.push_back({ glm::vec3(centerX + radius * cosA, height, centerZ + radius * sinA), radialNorm, color, glm::vec2(t, 1.0f) });
        vertices.push_back({ glm::vec3(centerX + radius * cosA, 0.0f, centerZ + radius * sinA), radialNorm, color, glm::vec2(t, 0.0f) });
    }
    for (int i = 0; i < segments; ++i) {
        unsigned int top0 = arcStartIdx + 4 * i, top1 = arcStartIdx + 4 * (i + 1);
        unsigned int bot0 = arcStartIdx + 4 * i + 1, bot1 = arcStartIdx + 4 * (i + 1) + 1;
        unsigned int wallTop0 = arcStartIdx + 4 * i + 2, wallBot0 = arcStartIdx + 4 * i + 3;
        unsigned int wallTop1 = arcStartIdx + 4 * (i + 1) + 2, wallBot1 = arcStartIdx + 4 * (i + 1) + 3;

        indices.push_back(topInnerIdx); indices.push_back(top1); indices.push_back(top0);
        indices.push_back(botInnerIdx); indices.push_back(bot0); indices.push_back(bot1);
        indices.push_back(wallBot1); indices.push_back(wallBot0); indices.push_back(wallTop0);
        indices.push_back(wallBot1); indices.push_back(wallTop0); indices.push_back(wallTop1);
    }
    return std::make_shared<Mesh>(vertices, indices);
}

void Scene::init() {
    rootNodes.clear();
    pointLights.clear();

    asphaltTex = std::make_shared<Texture>("assets/textures/asphalt.png", true);

    dirLight.direction = glm::vec3(-0.4f, -0.65f, -0.4f);
    dirLight.ambient   = glm::vec3(0.04f, 0.05f, 0.08f);
    dirLight.diffuse   = glm::vec3(0.12f, 0.14f, 0.20f);
    dirLight.specular  = glm::vec3(0.15f, 0.18f, 0.25f);

    // continuous base foundation ground plane
    auto baseGround = createPlane(60.0f, 80.0f, glm::vec3(0.12f), 24.0f, 32.0f);
    rootNodes.push_back(SceneNode(baseGround, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)), false, true, asphaltTex));

    // center lane stripes
    auto stripeMesh = createPlane(0.2f, 2.0f, glm::vec3(0.9f, 0.8f, 0.2f));
    for (float z = -36.0f; z <= -15.0f; z += 5.0f)
        rootNodes.push_back(SceneNode(stripeMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z))));
    for (float z = -1.0f; z <= 18.0f; z += 5.0f)
        rootNodes.push_back(SceneNode(stripeMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z))));

    // pedestrian zebra crossings inset inside each street approach
    auto zStripeN_S = createPlane(0.5f, 2.0f, glm::vec3(0.82f));
    auto zStripeW_E = createPlane(2.0f, 0.5f, glm::vec3(0.82f));
    for (float x = -3.0f; x <= 3.0f; x += 1.0f) {
        rootNodes.push_back(SceneNode(zStripeN_S, glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.006f, -14.0f))));
        rootNodes.push_back(SceneNode(zStripeN_S, glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.006f, -2.0f))));
    }
    for (float z = -10.5f; z <= -5.5f; z += 1.0f) {
        rootNodes.push_back(SceneNode(zStripeW_E, glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 0.006f, z))));
        rootNodes.push_back(SceneNode(zStripeW_E, glm::translate(glm::mat4(1.0f), glm::vec3( 6.5f, 0.006f, z))));
    }

    // straight sidewalk segments
    glm::vec3 curbColor(0.22f, 0.22f, 0.24f);
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 27.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.1f, 0.1f, -26.5f))));
    rootNodes.push_back(SceneNode(createBox(12.5f, 0.2f, 2.2f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-11.75f, 0.1f, -12.6f))));

    // konbini front sidewalk extended flush to recessed storefront
    rootNodes.push_back(SceneNode(createBox(3.1f, 0.2f, 11.5f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.55f, 0.1f, 2.75f))));
    rootNodes.push_back(SceneNode(createBox(12.5f, 0.2f, 2.2f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-11.75f, 0.1f, -3.4f))));

    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 27.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(5.1f, 0.1f, -26.5f))));
    rootNodes.push_back(SceneNode(createBox(12.5f, 0.2f, 2.2f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(11.75f, 0.1f, -12.6f))));
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 21.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(5.1f, 0.1f, 7.5f))));
    rootNodes.push_back(SceneNode(createBox(12.5f, 0.2f, 2.2f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(11.75f, 0.1f, -3.4f))));

    // curved rounded curb transitions connecting each corner
    rootNodes.push_back(SceneNode(createCornerCurb(-5.5f, -3.0f, 1.5f, 0.0f, -1.5707963f, -6.5f, -2.5f, 0.2f, 12, curbColor), glm::mat4(1.0f)));
    rootNodes.push_back(SceneNode(createCornerCurb(-5.5f, -13.0f, 1.5f, 1.5707963f, 0.0f, -6.5f, -13.5f, 0.2f, 12, curbColor), glm::mat4(1.0f)));
    rootNodes.push_back(SceneNode(createCornerCurb(5.5f, -3.0f, 1.5f, -1.5707963f, -3.14159265f, 6.5f, -2.5f, 0.2f, 12, curbColor), glm::mat4(1.0f)));
    rootNodes.push_back(SceneNode(createCornerCurb(5.5f, -13.0f, 1.5f, 3.14159265f, 1.5707963f, 6.5f, -13.5f, 0.2f, 12, curbColor), glm::mat4(1.0f)));

    // parking lot with lowered driveway ramp curb
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.03f, 5.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.1f, 0.015f, 11.0f))));
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 4.5f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.1f, 0.1f, 15.75f))));

    // parking bays with dual boundary lines and wheel stops
    auto parkLine = createPlane(4.5f, 0.12f, glm::vec3(0.9f, 0.8f, 0.2f));
    auto wheelStop = createBox(0.25f, 0.14f, 1.8f, glm::vec3(0.9f, 0.75f, 0.12f));
    for (float pz : { 9.0f, 11.6f, 14.2f, 16.8f })
        rootNodes.push_back(SceneNode(parkLine, glm::translate(glm::mat4(1.0f), glm::vec3(-9.25f, 0.007f, pz))));
    for (float pz : { 10.3f, 12.9f, 15.5f })
        rootNodes.push_back(SceneNode(wheelStop, glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, 0.07f, pz))));

    rootNodes.push_back(SceneNode(createBox(0.4f, 2.5f, 9.5f, glm::vec3(0.20f, 0.20f, 0.22f)), glm::translate(glm::mat4(1.0f), glm::vec3(-12.2f, 1.25f, 13.0f))));

    // street roadblocks and dead-end barriers
    auto barrierMesh = createBox(8.0f, 1.0f, 0.4f, glm::vec3(0.85f, 0.35f, 0.05f));
    rootNodes.push_back(SceneNode(barrierMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -39.0f))));
    rootNodes.push_back(SceneNode(barrierMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 19.0f))));
    rootNodes.push_back(SceneNode(barrierMesh, glm::translate(glm::mat4(1.0f), glm::vec3(18.0f, 0.5f, -8.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
    rootNodes.push_back(SceneNode(createBox(0.4f, 6.0f, 8.0f, glm::vec3(0.18f, 0.17f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(-18.5f, 3.0f, -8.0f))));

    // perimeter security walls sealing backstage light bleeds behind buildings
    auto fenceWall = createBox(0.3f, 4.0f, 26.0f, glm::vec3(0.16f, 0.16f, 0.18f));
    rootNodes.push_back(SceneNode(fenceWall, glm::translate(glm::mat4(1.0f), glm::vec3( 14.5f, 2.0f, -26.0f))));
    rootNodes.push_back(SceneNode(fenceWall, glm::translate(glm::mat4(1.0f), glm::vec3(-14.5f, 2.0f, -26.0f))));

    // west alley metal dumpster, crate and alley lamp
    rootNodes.push_back(SceneNode(createBox(1.6f, 1.2f, 2.4f, glm::vec3(0.16f, 0.25f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(-17.2f, 0.7f, -8.0f))));
    auto dumpsterLid = createBox(1.65f, 0.15f, 1.25f, glm::vec3(0.09f, 0.09f, 0.11f));
    rootNodes.push_back(SceneNode(dumpsterLid, glm::translate(glm::mat4(1.0f), glm::vec3(-17.2f, 1.35f, -8.6f))));
    rootNodes.push_back(SceneNode(dumpsterLid, glm::translate(glm::mat4(1.0f), glm::vec3(-17.2f, 1.35f, -7.4f))));
    rootNodes.push_back(SceneNode(createBox(0.8f, 0.8f, 0.8f, glm::vec3(0.35f, 0.28f, 0.20f)), glm::translate(glm::mat4(1.0f), glm::vec3(-17.2f, 0.4f, -6.0f))));
    rootNodes.push_back(SceneNode(createBox(0.3f, 0.15f, 0.2f, glm::vec3(0.2f, 0.85f, 0.8f)), glm::translate(glm::mat4(1.0f), glm::vec3(-18.2f, 3.0f, -8.0f)), true));

    auto addPoint = [&](const glm::vec3& p, const glm::vec3& c, float amb, float diff, float spec, float lin, float quad) {
        pointLights.push_back({ p, c * amb, c * diff, c * spec, 1.0f, lin, quad });
    };
    addPoint(glm::vec3(-17.0f, 2.8f, -8.0f), glm::vec3(0.2f, 0.85f, 0.8f), 0.05f, 1.0f, 1.0f, 0.18f, 0.09f);

    // konbini building with unified material color and seamless pillars
    glm::vec3 storeColor(0.24f, 0.23f, 0.22f);
    rootNodes.push_back(SceneNode(createBox(5.8f, 5.2f, 11.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.6f, 3.0f))));
    rootNodes.push_back(SceneNode(createBox(6.0f, 0.35f, 11.2f, glm::vec3(0.18f, 0.17f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 5.35f, 3.0f))));

    // side pillars and lintel flush at height 3.2m
    rootNodes.push_back(SceneNode(createBox(0.6f, 3.2f, 2.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-6.8f, 1.6f, -1.5f))));
    rootNodes.push_back(SceneNode(createBox(0.6f, 3.2f, 2.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-6.8f, 1.6f,  7.5f))));
    rootNodes.push_back(SceneNode(createBox(0.6f, 0.8f, 7.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-6.8f, 2.8f,  3.0f))));

    // service ladder on rear alley side
    auto ladderRail = createCylinder(0.02f, 4.6f, 6, glm::vec3(0.2f, 0.2f, 0.22f), true);
    rootNodes.push_back(SceneNode(ladderRail, glm::translate(glm::mat4(1.0f), glm::vec3(-12.8f, 2.5f, -2.0f))));
    rootNodes.push_back(SceneNode(ladderRail, glm::translate(glm::mat4(1.0f), glm::vec3(-12.8f, 2.5f, -1.6f))));
    auto ladderRung = createBox(0.03f, 0.03f, 0.4f, glm::vec3(0.25f, 0.25f, 0.27f));
    for (float ly = 0.6f; ly <= 4.6f; ly += 0.5f)
        rootNodes.push_back(SceneNode(ladderRung, glm::translate(glm::mat4(1.0f), glm::vec3(-12.8f, ly, -1.8f))));

    // dark canopy fascia resting directly on top of the lower blocks across full span
    rootNodes.push_back(SceneNode(createBox(0.85f, 0.8f, 11.1f, glm::vec3(0.16f, 0.16f, 0.18f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.78f, 3.6f, 3.0f))));

    // 3D bent tube 24/7 neon sign mounted on fascia
    auto neon24Mesh = Model::loadOBJ("assets/models/neon_sign_24_7.obj", glm::vec3(0.15f, 1.0f, 0.65f));
    if (neon24Mesh) {
        glm::mat4 neon24Tr = glm::translate(glm::mat4(1.0f), glm::vec3(-6.33f, 3.6f, 3.0f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(neon24Mesh, neon24Tr, true));
    }

    // inset double doors and handles
    rootNodes.push_back(SceneNode(createBox(0.1f, 2.3f, 1.6f, glm::vec3(0.12f, 0.12f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(-7.1f, 1.25f, 3.0f))));
    rootNodes.push_back(SceneNode(createBox(0.04f, 2.1f, 1.4f, glm::vec3(0.35f, 0.45f, 0.50f)), glm::translate(glm::mat4(1.0f), glm::vec3(-7.09f, 1.25f, 3.0f))));
    auto doorHandle = createCylinder(0.02f, 0.6f, 8, glm::vec3(0.85f), true);
    rootNodes.push_back(SceneNode(doorHandle, glm::translate(glm::mat4(1.0f), glm::vec3(-7.02f, 1.25f, 2.85f))));
    rootNodes.push_back(SceneNode(doorHandle, glm::translate(glm::mat4(1.0f), glm::vec3(-7.02f, 1.25f, 3.15f))));

    // inset display windows with ample clearance
    glm::vec3 warmStoreGlow(1.0f, 0.88f, 0.68f);
    auto windowFrame = createBox(0.12f, 2.3f, 1.9f, glm::vec3(0.14f, 0.14f, 0.16f));
    auto storeWindow = createBox(0.08f, 2.1f, 1.7f, warmStoreGlow);
    rootNodes.push_back(SceneNode(windowFrame, glm::translate(glm::mat4(1.0f), glm::vec3(-7.1f, 1.3f, 1.05f))));
    rootNodes.push_back(SceneNode(storeWindow,  glm::translate(glm::mat4(1.0f), glm::vec3(-7.07f, 1.3f, 1.05f)), true));
    rootNodes.push_back(SceneNode(windowFrame, glm::translate(glm::mat4(1.0f), glm::vec3(-7.1f, 1.3f, 4.95f))));
    rootNodes.push_back(SceneNode(storeWindow,  glm::translate(glm::mat4(1.0f), glm::vec3(-7.07f, 1.3f, 4.95f)), true));

    addPoint(glm::vec3(-5.5f, 2.0f, 3.0f), warmStoreGlow, 0.05f, 1.8f, 1.5f, 0.16f, 0.08f);

    // vending machines and trash can placed on alley sidewalk
    auto vend1Body = createBox(1.1f, 2.05f, 0.8f, glm::vec3(0.08f, 0.14f, 0.28f));
    rootNodes.push_back(SceneNode(vend1Body, glm::translate(glm::mat4(1.0f), glm::vec3(-8.2f, 1.125f, -3.1f))));
    auto vend1Panel = createBox(0.9f, 1.3f, 0.05f, glm::vec3(0.92f, 0.98f, 1.0f));
    rootNodes.push_back(SceneNode(vend1Panel, glm::translate(glm::mat4(1.0f), glm::vec3(-8.2f, 1.35f, -3.52f)), true));

    auto vend2Body = createBox(1.1f, 2.05f, 0.8f, glm::vec3(0.38f, 0.09f, 0.12f));
    rootNodes.push_back(SceneNode(vend2Body, glm::translate(glm::mat4(1.0f), glm::vec3(-9.6f, 1.125f, -3.1f))));
    auto vend2Panel = createBox(0.9f, 1.3f, 0.05f, glm::vec3(1.0f, 0.94f, 0.92f));
    rootNodes.push_back(SceneNode(vend2Panel, glm::translate(glm::mat4(1.0f), glm::vec3(-9.6f, 1.35f, -3.52f)), true));

    auto trashCan = createCylinder(0.26f, 1.1f, 16, glm::vec3(0.12f, 0.32f, 0.18f), true);
    rootNodes.push_back(SceneNode(trashCan, glm::translate(glm::mat4(1.0f), glm::vec3(-10.8f, 0.65f, -3.1f))));

    glm::vec3 vendColor(0.85f, 0.95f, 1.0f);
    addPoint(glm::vec3(-8.9f, 1.5f, -4.2f), vendColor, 0.04f, 1.9f, 1.6f, 0.18f, 0.09f);

    // ramen shop with low angled canopy
    rootNodes.push_back(SceneNode(createBox(7.5f, 6.5f, 11.0f, glm::vec3(0.22f, 0.17f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.25f, 3.25f, 3.0f))));
    rootNodes.push_back(SceneNode(createBox(0.1f, 2.2f, 1.6f, glm::vec3(0.15f, 0.12f, 0.10f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.2f, 3.0f))));
    auto ramenAwning = createBox(0.8f, 0.12f, 4.5f, glm::vec3(0.25f, 0.08f, 0.08f));
    glm::mat4 awningTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.25f, 2.45f, 3.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rootNodes.push_back(SceneNode(ramenAwning, awningTr));

    // 3D katakana neon sign mounted with wall brackets above the awning
    auto ramenNeonMesh = Model::loadOBJ("assets/models/neon_sign_ramen.obj", glm::vec3(1.0f, 0.22f, 0.70f));
    if (ramenNeonMesh) {
        glm::mat4 ramenTransform = glm::translate(glm::mat4(1.0f), glm::vec3(6.42f, 3.6f, 3.0f))
                                 * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(ramenNeonMesh, ramenTransform, true));
    }
    glm::vec3 magentaColor(1.0f, 0.20f, 0.65f);
    addPoint(glm::vec3(5.5f, 3.6f, 3.0f), magentaColor, 0.06f, 2.2f, 2.0f, 0.18f, 0.09f);

    // commercial buildings with 3D bar and hotel neon signs
    auto barBuilding = createBox(7.5f, 9.5f, 16.0f, glm::vec3(0.16f, 0.17f, 0.20f));
    rootNodes.push_back(SceneNode(barBuilding, glm::translate(glm::mat4(1.0f), glm::vec3(-10.25f, 4.75f, -22.0f))));
    auto barNeonMesh = Model::loadOBJ("assets/models/neon_sign_bar.obj", glm::vec3(0.0f, 0.95f, 1.0f));
    if (barNeonMesh) {
        glm::mat4 barTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-6.42f, 3.4f, -22.0f))
                               * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(barNeonMesh, barTransform, true));
    }
    glm::vec3 cyanBarColor(0.0f, 0.95f, 1.0f);
    addPoint(glm::vec3(-5.5f, 3.4f, -22.0f), cyanBarColor, 0.06f, 2.2f, 2.0f, 0.18f, 0.09f);

    auto hotelBuilding = createBox(7.5f, 9.0f, 16.0f, glm::vec3(0.20f, 0.18f, 0.16f));
    rootNodes.push_back(SceneNode(hotelBuilding, glm::translate(glm::mat4(1.0f), glm::vec3(10.25f, 4.5f, -22.0f))));
    auto hotelNeonMesh = Model::loadOBJ("assets/models/neon_sign_hotel.obj", glm::vec3(1.0f, 0.65f, 0.15f));
    if (hotelNeonMesh) {
        glm::mat4 hotelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(6.42f, 3.4f, -22.0f))
                                 * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(hotelNeonMesh, hotelTransform, true));
    }

    // sidewalk streetlamps: moved lamp in front of entrance to Z = -1.0 on both sides
    auto poleMesh = createBox(0.2f, 4.5f, 0.2f, glm::vec3(0.2f, 0.2f, 0.22f));
    auto armMesh  = createBox(1.2f, 0.15f, 0.15f, glm::vec3(0.25f, 0.25f, 0.27f));
    glm::vec3 cyanLightColor(0.0f, 0.9f, 1.0f);
    glm::vec3 amberLightColor(1.0f, 0.65f, 0.15f);
    auto cyanFixtureMesh  = createBox(0.4f, 0.2f, 0.3f, cyanLightColor);
    auto amberFixtureMesh = createBox(0.4f, 0.2f, 0.3f, amberLightColor);

    float leftLampZ[]  = { -30.0f, -18.0f, -1.0f, 7.0f };
    float rightLampZ[] = { -30.0f, -18.0f, -1.0f, 13.0f };

    for (float z : leftLampZ) {
        glm::mat4 poleL = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 2.25f, z));
        SceneNode poleLNode(poleMesh, poleL);
        glm::mat4 armL = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.1f, 0.0f));
        SceneNode armLNode(armMesh, armL);
        armLNode.addChild(SceneNode(cyanFixtureMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.15f, 0.0f)), true));
        poleLNode.addChild(armLNode);
        rootNodes.push_back(poleLNode);
        addPoint(glm::vec3(-3.2f, 4.2f, z), cyanLightColor, 0.05f, 1.3f, 1.2f, 0.14f, 0.07f);
    }
    for (float z : rightLampZ) {
        glm::mat4 poleR = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, 2.25f, z));
        SceneNode poleRNode(poleMesh, poleR);
        glm::mat4 armR = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.1f, 0.0f));
        SceneNode armRNode(armMesh, armR);
        armRNode.addChild(SceneNode(amberFixtureMesh, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.15f, 0.0f)), true));
        poleRNode.addChild(armRNode);
        rootNodes.push_back(poleRNode);
        addPoint(glm::vec3(3.2f, 4.2f, z), amberLightColor, 0.05f, 1.3f, 1.2f, 0.14f, 0.07f);
    }

    createSkyscrapers();
}

void Scene::setupLights(const Shader& shader) {
    shader.setVec3("dirLight.direction", dirLight.direction);
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);

    for (size_t i = 0; i < 16; ++i) {
        std::string prefix = "pointLights[" + std::to_string(i) + "].";
        if (i < pointLights.size()) {
            const auto& pl = pointLights[i];
            shader.setVec3(prefix + "position", pl.position);
            shader.setVec3(prefix + "ambient", pl.ambient);
            shader.setVec3(prefix + "diffuse", pl.diffuse);
            shader.setVec3(prefix + "specular", pl.specular);
            shader.setFloat(prefix + "constant", pl.constant);
            shader.setFloat(prefix + "linear", pl.linear);
            shader.setFloat(prefix + "quadratic", pl.quadratic);
        } else {
            shader.setVec3(prefix + "position", glm::vec3(0.0f, -100.0f, 0.0f));
            shader.setVec3(prefix + "ambient", glm::vec3(0.0f));
            shader.setVec3(prefix + "diffuse", glm::vec3(0.0f));
            shader.setVec3(prefix + "specular", glm::vec3(0.0f));
            shader.setFloat(prefix + "constant", 1.0f);
            shader.setFloat(prefix + "linear", 1.0f);
            shader.setFloat(prefix + "quadratic", 1.0f);
        }
    }
}

void Scene::render(const Shader& shader) {
    for (const auto& node : rootNodes) {
        node.draw(shader);
    }
}

void Scene::createSkyscrapers() {
    struct TowerDef {
        glm::vec3 pos;
        float w, h, d;
        int dir;
    };

    std::vector<TowerDef> towers = {
        { {-26.0f, 22.0f, -56.0f}, 18.0f, 44.0f, 16.0f, 0 }, { { -8.0f, 29.0f, -64.0f}, 16.0f, 58.0f, 16.0f, 0 },
        { { 10.0f, 25.0f, -58.0f}, 18.0f, 50.0f, 18.0f, 0 }, { { 28.0f, 20.0f, -52.0f}, 16.0f, 40.0f, 16.0f, 0 },
        { {-24.0f, 24.0f,  38.0f}, 16.0f, 48.0f, 16.0f, 1 }, { { -6.0f, 20.0f,  35.0f}, 18.0f, 40.0f, 16.0f, 1 },
        { { 12.0f, 26.0f,  40.0f}, 16.0f, 52.0f, 18.0f, 1 }, { { 30.0f, 21.0f,  36.0f}, 18.0f, 42.0f, 16.0f, 1 },
        { { 36.0f, 22.0f, -24.0f}, 16.0f, 44.0f, 18.0f, 2 }, { { 38.0f, 28.0f,  -8.0f}, 16.0f, 56.0f, 16.0f, 2 },
        { { 35.0f, 19.0f,   8.0f}, 16.0f, 38.0f, 18.0f, 2 }, { {-36.0f, 23.0f, -24.0f}, 16.0f, 46.0f, 18.0f, 3 },
        { {-38.0f, 28.0f,  -8.0f}, 16.0f, 56.0f, 16.0f, 3 }, { {-34.0f, 18.0f,   8.0f}, 16.0f, 36.0f, 18.0f, 3 }
    };

    glm::vec3 towerColor(0.12f, 0.13f, 0.16f);
    glm::vec3 redBeacon(1.0f, 0.15f, 0.15f);
    auto beaconMesh = createBox(0.6f, 0.6f, 0.6f, redBeacon);

    std::vector<Vertex> litVerts, unlitVerts;
    std::vector<unsigned int> litIdxs, unlitIdxs;

    auto addQuad = [](std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                      const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                      const glm::vec3& norm, const glm::vec3& col) {
        unsigned int b = static_cast<unsigned int>(verts.size());
        verts.push_back({ p0, norm, col, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ p1, norm, col, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ p2, norm, col, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ p3, norm, col, glm::vec2(0.0f, 1.0f) });
        idxs.push_back(b); idxs.push_back(b + 1); idxs.push_back(b + 2);
        idxs.push_back(b); idxs.push_back(b + 2); idxs.push_back(b + 3);
    };

    for (size_t i = 0; i < towers.size(); ++i) {
        const auto& t = towers[i];
        rootNodes.push_back(SceneNode(createBox(t.w, t.h, t.d, towerColor), glm::translate(glm::mat4(1.0f), t.pos)));
        rootNodes.push_back(SceneNode(beaconMesh, glm::translate(glm::mat4(1.0f), glm::vec3(t.pos.x, t.h + 0.3f, t.pos.z)), true));

        float span = (t.dir < 2) ? t.w : t.d;
        int cols = static_cast<int>(span / 2.6f);
        int floors = static_cast<int>((t.h - 8.0f) / 2.8f);

        for (int c = 0; c < cols; ++c) {
            float u = -span * 0.5f + 1.6f + c * 2.6f;
            for (int r = 0; r < floors; ++r) {
                float y = 5.0f + r * 2.8f;
                float val = std::fmod(std::abs(std::sin((c + 1) * 12.9898f + (r + 1) * 78.233f + (i + 1) * 37.719f) * 43758.5453f), 1.0f);

                bool isLit = (val > 0.68f);
                glm::vec3 col = isLit 
                    ? (val > 0.88f ? glm::vec3(0.32f, 0.35f, 0.38f) : (val > 0.78f ? glm::vec3(0.38f, 0.32f, 0.22f) : glm::vec3(0.40f, 0.36f, 0.28f)))
                    : glm::vec3(0.04f, 0.04f, 0.05f);

                auto& targetV = isLit ? litVerts : unlitVerts;
                auto& targetI = isLit ? litIdxs : unlitIdxs;

                if (t.dir == 0) {
                    float z = t.pos.z + t.d * 0.5f + 0.05f, x0 = t.pos.x + u;
                    addQuad(targetV, targetI, {x0, y, z}, {x0 + 1.2f, y, z}, {x0 + 1.2f, y + 1.4f, z}, {x0, y + 1.4f, z}, {0.0f, 0.0f, 1.0f}, col);
                } else if (t.dir == 1) {
                    float z = t.pos.z - t.d * 0.5f - 0.05f, x0 = t.pos.x + u;
                    addQuad(targetV, targetI, {x0 + 1.2f, y, z}, {x0, y, z}, {x0, y + 1.4f, z}, {x0 + 1.2f, y + 1.4f, z}, {0.0f, 0.0f, -1.0f}, col);
                } else if (t.dir == 2) {
                    float x = t.pos.x - t.w * 0.5f - 0.05f, z0 = t.pos.z + u;
                    addQuad(targetV, targetI, {x, y, z0}, {x, y, z0 + 1.2f}, {x, y + 1.4f, z0 + 1.2f}, {x, y + 1.4f, z0}, {-1.0f, 0.0f, 0.0f}, col);
                } else {
                    float x = t.pos.x + t.w * 0.5f + 0.05f, z0 = t.pos.z + u;
                    addQuad(targetV, targetI, {x, y, z0 + 1.2f}, {x, y, z0}, {x, y + 1.4f, z0}, {x, y + 1.4f, z0 + 1.2f}, {1.0f, 0.0f, 0.0f}, col);
                }
            }
        }
    }

    if (!litVerts.empty()) {
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(litVerts, litIdxs), glm::mat4(1.0f), true));
    }
    if (!unlitVerts.empty()) {
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(unlitVerts, unlitIdxs), glm::mat4(1.0f), false));
    }
}
