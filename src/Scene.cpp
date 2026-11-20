#include "Scene.h"
#include "Cinematic.h"
#include "Audio.h"
#include "CityBuilder.h"
#include "AlleyBuilder.h"
#include "ShopBuilder.h"
#include "HotelBuilder.h"
#include "DecalBuilder.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <cmath>
#include <algorithm>

SceneNode::SceneNode() 
    : mesh(nullptr), localTransform(glm::mat4(1.0f)), isEmissive(false), isRoad(false), isFlickering(false), emissiveBoost(1.0f), texture(nullptr) {}

SceneNode::SceneNode(std::shared_ptr<Mesh> mesh, const glm::mat4& transform, bool isEmissive, bool isRoad, std::shared_ptr<Texture> texture, bool isFlickering, float emissiveBoost)
    : mesh(mesh), localTransform(transform), isEmissive(isEmissive), isRoad(isRoad), isFlickering(isFlickering), emissiveBoost(emissiveBoost), texture(texture) {}

void SceneNode::addChild(const SceneNode& child) {
    children.push_back(child);
}

// Uniform state cache preventing redundant shader uniform calls across scene graph nodes
struct RenderStateCache {
    int isEmissive = -1;
    int isRoad = -1;
    int isFlickering = -1;
    int hasTexture = -1;
};
static RenderStateCache s_stateCache;

void SceneNode::draw(const Shader& shader, const glm::mat4& parentTransform) const {
    glm::mat4 globalTransform = parentTransform * localTransform;

    if (mesh) {
        int em = isEmissive ? 1 : 0;
        if (s_stateCache.isEmissive != em) { shader.setBool("isEmissive", isEmissive); s_stateCache.isEmissive = em; }
        if (isEmissive) shader.setFloat("emissiveBoost", emissiveBoost);
        int rd = isRoad ? 1 : 0;
        if (s_stateCache.isRoad != rd) { shader.setBool("isRoad", isRoad); s_stateCache.isRoad = rd; }
        int fl = isFlickering ? 1 : 0;
        if (s_stateCache.isFlickering != fl) { shader.setBool("isFlickering", isFlickering); s_stateCache.isFlickering = fl; }

        if (texture) {
            if (s_stateCache.hasTexture != 1) {
                shader.setBool("hasTexture", true);
                s_stateCache.hasTexture = 1;
            }
            texture->bind(0);
            shader.setInt("diffuseTexture", 0);
        } else {
            if (s_stateCache.hasTexture != 0) {
                shader.setBool("hasTexture", false);
                s_stateCache.hasTexture = 0;
            }
        }

        shader.setMat4("model", globalTransform);
        mesh->draw();
    }

    for (const auto& child : children) {
        child.draw(shader, globalTransform);
    }
}

Scene::Scene() 
    : cinematic(nullptr), barLightIndex(-1), baseBarDiffuse(0.0f), baseBarSpecular(0.0f), baseBarAmbient(0.0f),
      currentFlicker(1.0f), barFlicker(1.0f), accumulatedTime(0.0f), surgeTimer(-1.0f), surgeDuration(2.20f), nextSurgeCooldown(15.0f) {}

Scene::~Scene() {}

// Trigger high-voltage surge event with arc audio and irregular neon flickering
void Scene::triggerFlickerSurge() {
    surgeTimer = 0.0f;
    nextSurgeCooldown = 12.0f + static_cast<float>(rand() % 61) * 0.1f;
    Audio::triggerArcSpark();
}

// Update broken neon sign stutter patterns and surge cooldown timers
void Scene::update(float deltaTime) {
    accumulatedTime += deltaTime;
    // Multi-frequency broken neon flicker cycle with intermittent voltage drops
    float barCycle = std::fmod(accumulatedTime, 4.2f);
    if (barCycle < 2.8f) {
        barFlicker = 0.96f + 0.08f * std::sin(accumulatedTime * 120.0f) * std::sin(accumulatedTime * 37.0f);
    } else {
        float bt = barCycle - 2.8f;
        float spark = std::sin(bt * 73.0f) * std::cos(bt * 139.0f) + std::sin(bt * 311.0f) * 0.5f;
        if ((bt > 0.15f && bt < 0.28f) || (bt > 0.65f && bt < 0.82f)) barFlicker = 0.04f;
        else if ((bt > 0.32f && bt < 0.36f) || (bt > 0.58f && bt < 0.62f)) barFlicker = 1.40f;
        else if (bt > 0.40f && bt < 0.55f) barFlicker = 0.08f;
        else if (bt > 0.85f && bt < 0.92f) barFlicker = 1.25f;
        else barFlicker = std::max(0.05f, std::min(1.4f, 0.7f + spark * 0.5f));
    }

    // Advance active surge timer or decrement interval to next surge
    if (surgeTimer >= 0.0f) {
        surgeTimer += deltaTime;
        if (surgeTimer > surgeDuration) surgeTimer = -1.0f;
    } else {
        nextSurgeCooldown -= deltaTime;
        if (nextSurgeCooldown <= 0.0f) triggerFlickerSurge();
    }
}

std::shared_ptr<Mesh> Scene::createPlane(float width, float depth, const glm::vec3& color, float tileX, float tileY) { return CityBuilder::createPlane(width, depth, color, tileX, tileY); }
std::shared_ptr<Mesh> Scene::createBox(float width, float height, float depth, const glm::vec3& color) { return CityBuilder::createBox(width, height, depth, color); }

// Procedural cylindrical mesh with radial surface normals and optional end cap disks
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

// Procedural curved sidewalk corner curb with radial outward normals and sector triangulation
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
    moonTex    = std::make_shared<Texture>("assets/textures/moon.jpg", true);

    dirLight.direction = glm::vec3(-0.4f, -0.65f, -0.4f); dirLight.ambient = glm::vec3(0.08f, 0.09f, 0.13f);
    dirLight.diffuse   = glm::vec3(0.18f, 0.20f, 0.28f); dirLight.specular = glm::vec3(0.15f, 0.18f, 0.25f);

    auto baseGround = createPlane(60.0f, 80.0f, glm::vec3(0.12f), 24.0f, 32.0f);
    rootNodes.push_back(SceneNode(baseGround, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f)), false, true, asphaltTex));

    // Batched highway and intersection road markings
    std::vector<Vertex> yStripeVerts;
    std::vector<unsigned int> yStripeIdxs;
    glm::vec3 yellowStripeCol(0.9f, 0.8f, 0.2f);
    for (float z : { -36.0f, -31.0f, -26.0f, -21.0f, -16.0f, -1.0f, 4.0f, 9.0f, 14.0f }) {
        CityBuilder::appendPlane(yStripeVerts, yStripeIdxs, 0.2f, 2.0f, yellowStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z)));
    }
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(yStripeVerts, yStripeIdxs), glm::mat4(1.0f)));

    std::vector<Vertex> wStripeVerts;
    std::vector<unsigned int> wStripeIdxs;
    glm::vec3 whiteStripeCol(0.82f);
    for (float x = -3.0f; x <= 3.0f; x += 1.0f) {
        CityBuilder::appendPlane(wStripeVerts, wStripeIdxs, 0.5f, 2.0f, whiteStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.006f, -14.0f)));
        CityBuilder::appendPlane(wStripeVerts, wStripeIdxs, 0.5f, 2.0f, whiteStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3(x, 0.006f, -2.0f)));
    }
    for (float z = -10.5f; z <= -5.5f; z += 1.0f) {
        CityBuilder::appendPlane(wStripeVerts, wStripeIdxs, 2.0f, 0.5f, whiteStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3(-6.5f, 0.006f, z)));
        CityBuilder::appendPlane(wStripeVerts, wStripeIdxs, 2.0f, 0.5f, whiteStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3( 6.5f, 0.006f, z)));
    }
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(wStripeVerts, wStripeIdxs), glm::mat4(1.0f)));

    // Batched sidewalk curb geometry
    glm::vec3 curbColor(0.22f, 0.22f, 0.24f);
    std::vector<Vertex> curbVerts;
    std::vector<unsigned int> curbIdxs;
    struct CurbDef { float w, h, d, x, y, z; };
    static const CurbDef curbs[] = {
        { 2.2f, 0.2f, 27.0f, -5.1f, 0.1f, -26.5f }, { 12.5f, 0.2f, 2.2f, -11.75f, 0.1f, -12.6f },
        { 3.1f, 0.2f, 11.5f, -5.55f, 0.1f, 2.75f },  { 12.5f, 0.2f, 2.2f, -11.75f, 0.1f, -3.4f },
        { 2.2f, 0.2f, 27.0f,  5.1f, 0.1f, -26.5f }, { 12.5f, 0.2f, 2.2f,  11.75f, 0.1f, -12.6f },
        { 2.2f, 0.2f, 21.0f,  5.1f, 0.1f, 7.5f },   { 12.5f, 0.2f, 2.2f,  11.75f, 0.1f, -3.4f },
        { 2.2f, 0.03f, 5.0f, -5.1f, 0.015f, 11.0f }, { 2.2f, 0.2f, 4.5f,  -5.1f, 0.1f, 15.75f }
    };
    for (const auto& c : curbs) {
        CityBuilder::appendBox(curbVerts, curbIdxs, c.w, c.h, c.d, curbColor, glm::translate(glm::mat4(1.0f), glm::vec3(c.x, c.y, c.z)));
    }
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(curbVerts, curbIdxs), glm::mat4(1.0f)));

    for (auto& cc : { std::make_tuple(-5.5f, -3.0f, 0.0f, -1.5707963f, -6.5f, -2.5f), std::make_tuple(-5.5f, -13.0f, 1.5707963f, 0.0f, -6.5f, -13.5f), std::make_tuple(5.5f, -3.0f, -1.5707963f, -3.14159265f, 6.5f, -2.5f), std::make_tuple(5.5f, -13.0f, 3.14159265f, 1.5707963f, 6.5f, -13.5f) })
        rootNodes.push_back(SceneNode(createCornerCurb(std::get<0>(cc), std::get<1>(cc), 1.5f, std::get<2>(cc), std::get<3>(cc), std::get<4>(cc), std::get<5>(cc), 0.2f, 12, curbColor), glm::mat4(1.0f)));

    // Batched parking stall lines
    std::vector<Vertex> parkLineVerts;
    std::vector<unsigned int> parkLineIdxs;
    for (float pz : { 9.0f, 11.6f, 14.2f, 16.8f })
        CityBuilder::appendPlane(parkLineVerts, parkLineIdxs, 4.5f, 0.12f, yellowStripeCol, glm::translate(glm::mat4(1.0f), glm::vec3(-9.25f, 0.007f, pz)));
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(parkLineVerts, parkLineIdxs), glm::mat4(1.0f)));

    auto wheelStop = createBox(0.25f, 0.14f, 1.8f, glm::vec3(0.9f, 0.75f, 0.12f));
    for (float pz : { 10.3f, 12.9f, 15.5f }) rootNodes.push_back(SceneNode(wheelStop, glm::translate(glm::mat4(1.0f), glm::vec3(-11.0f, 0.07f, pz))));

    struct ParkedCarDef { const char* modelPath; glm::vec3 bodyColor; glm::vec3 position; float rotationY; };
    static const ParkedCarDef parkedCars[] = {
        { "assets/models/car_sedan.obj",     glm::vec3(0.24f, 0.35f, 0.26f), glm::vec3(-9.1f, 0.0f, 10.3f),  90.0f },
        { "assets/models/car_hatchback.obj", glm::vec3(0.68f, 0.16f, 0.14f), glm::vec3(-9.1f, 0.0f, 15.5f),  90.0f },
        { "assets/models/car_sedan.obj",     glm::vec3(0.08f, 0.14f, 0.28f), glm::vec3(-4.55f, 0.0f, -57.5f), 0.0f },
        { "assets/models/car_hatchback.obj", glm::vec3(0.18f, 0.20f, 0.22f), glm::vec3( 4.55f, 0.0f, -38.0f), 180.0f },
        { "assets/models/car_sedan.obj",     glm::vec3(0.48f, 0.09f, 0.12f), glm::vec3( 4.55f, 0.0f,  21.0f), 180.0f },
        { "assets/models/car_hatchback.obj", glm::vec3(0.09f, 0.28f, 0.16f), glm::vec3(-4.55f, 0.0f,  33.0f), 0.0f }
    };
    for (const auto& pc : parkedCars) {
        auto carMesh = Model::loadOBJ(pc.modelPath, pc.bodyColor, &pc.bodyColor);
        if (carMesh) {
            glm::mat4 carTr = glm::translate(glm::mat4(1.0f), pc.position) * glm::rotate(glm::mat4(1.0f), glm::radians(pc.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(carMesh, carTr, false));
        }
    }

    rootNodes.push_back(SceneNode(createBox(0.4f, 2.5f, 9.5f, glm::vec3(0.20f, 0.20f, 0.22f)), glm::translate(glm::mat4(1.0f), glm::vec3(-12.2f, 1.25f, 13.0f))));

    // Enclosed dead-end streets, boundary walls, and Unturned chain-link fence
    AlleyBuilder::buildDeadEnds(rootNodes, pointLights);

    auto addPoint = [&](const glm::vec3& p, const glm::vec3& c, float amb, float diff, float spec, float lin, float quad) {
        pointLights.push_back({ p, c * amb, c * diff, c * spec, 1.0f, lin, quad });
    };
    addPoint(glm::vec3(-17.0f, 2.8f, -8.0f), glm::vec3(0.2f, 0.85f, 0.8f), 0.05f, 1.0f, 1.0f, 0.18f, 0.09f);

    // konbini building with unified material color and seamless pillars
    glm::vec3 storeColor(0.24f, 0.23f, 0.22f);
    rootNodes.push_back(SceneNode(createBox(5.8f, 5.2f, 11.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 2.6f, 3.0f))));
    rootNodes.push_back(SceneNode(createBox(6.0f, 0.35f, 11.2f, glm::vec3(0.18f, 0.17f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 5.35f, 3.0f))));

    for (float pz : { -1.5f, 7.5f }) rootNodes.push_back(SceneNode(createBox(0.6f, 3.2f, 2.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-6.8f, 1.6f, pz))));
    rootNodes.push_back(SceneNode(createBox(0.6f, 0.8f, 7.0f, storeColor), glm::translate(glm::mat4(1.0f), glm::vec3(-6.8f, 2.8f, 3.0f))));
    auto ladderRail = createCylinder(0.02f, 4.6f, 6, glm::vec3(0.2f, 0.2f, 0.22f), true);
    for (float rz : { -2.0f, -1.6f }) rootNodes.push_back(SceneNode(ladderRail, glm::translate(glm::mat4(1.0f), glm::vec3(-12.8f, 2.5f, rz))));
    auto ladderRung = createBox(0.03f, 0.03f, 0.4f, glm::vec3(0.25f, 0.25f, 0.27f));
    for (float ly = 0.6f; ly <= 4.6f; ly += 0.5f)
        rootNodes.push_back(SceneNode(ladderRung, glm::translate(glm::mat4(1.0f), glm::vec3(-12.8f, ly, -1.8f))));

    rootNodes.push_back(SceneNode(createBox(0.85f, 0.8f, 11.1f, glm::vec3(0.16f, 0.16f, 0.18f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.78f, 3.6f, 3.0f))));

    auto neon24Mesh = Model::loadOBJ("assets/models/neon_sign_24_7.obj", glm::vec3(0.15f, 1.0f, 0.65f));
    if (neon24Mesh) {
        glm::mat4 neon24Tr = glm::translate(glm::mat4(1.0f), glm::vec3(-6.33f, 3.6f, 3.0f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(neon24Mesh, neon24Tr, true, false, nullptr, true, 2.85f));
    }

    rootNodes.push_back(SceneNode(createBox(0.1f, 2.3f, 1.6f, glm::vec3(0.12f, 0.12f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(-7.1f, 1.25f, 3.0f))));
    rootNodes.push_back(SceneNode(createBox(0.04f, 2.1f, 1.4f, glm::vec3(0.35f, 0.45f, 0.50f)), glm::translate(glm::mat4(1.0f), glm::vec3(-7.09f, 1.25f, 3.0f))));
    auto doorHandle = createCylinder(0.02f, 0.6f, 8, glm::vec3(0.85f), true);
    for (float dz : { 2.85f, 3.15f }) rootNodes.push_back(SceneNode(doorHandle, glm::translate(glm::mat4(1.0f), glm::vec3(-7.02f, 1.25f, dz))));

    glm::vec3 warmStoreGlow(1.0f, 0.88f, 0.68f);
    auto windowFrame = createBox(0.12f, 2.3f, 1.9f, glm::vec3(0.14f, 0.14f, 0.16f));
    auto storeWindow = createBox(0.08f, 2.1f, 1.7f, warmStoreGlow);
    for (float wz : { 1.05f, 4.95f }) {
        rootNodes.push_back(SceneNode(windowFrame, glm::translate(glm::mat4(1.0f), glm::vec3(-7.1f, 1.3f, wz))));
        rootNodes.push_back(SceneNode(storeWindow,  glm::translate(glm::mat4(1.0f), glm::vec3(-7.07f, 1.3f, wz)), true));
    }
    addPoint(glm::vec3(-5.5f, 2.0f, 3.0f), warmStoreGlow, 0.05f, 1.8f, 1.5f, 0.16f, 0.08f);

    // detailed vending machines in the konbini alley alcove
    ShopBuilder::buildDetailedVendingMachine(rootNodes, glm::vec3(-8.2f, 0.0f, -3.1f), 180.0f, glm::vec3(0.08f, 0.14f, 0.28f));
    ShopBuilder::buildDetailedVendingMachine(rootNodes, glm::vec3(-9.6f, 0.0f, -3.1f), 180.0f, glm::vec3(0.38f, 0.09f, 0.12f));
    auto trashCan = createCylinder(0.26f, 1.1f, 16, glm::vec3(0.12f, 0.32f, 0.18f), true);
    rootNodes.push_back(SceneNode(trashCan, glm::translate(glm::mat4(1.0f), glm::vec3(-10.8f, 0.65f, -3.1f))));
    addPoint(glm::vec3(-8.9f, 1.5f, -3.8f), glm::vec3(0.85f, 0.95f, 1.0f), 0.04f, 1.9f, 1.6f, 0.18f, 0.09f);

    // Ramen shop recessed dining nook
    ShopBuilder::buildRamenShop(rootNodes, pointLights);

    // Bar & Lounge facade detailing
    barLightIndex = static_cast<int>(pointLights.size());
    ShopBuilder::buildBarFacade(rootNodes, pointLights);
    if (barLightIndex >= 0 && barLightIndex < static_cast<int>(pointLights.size())) {
        baseBarDiffuse = pointLights[barLightIndex].diffuse;
        baseBarSpecular = pointLights[barLightIndex].specular;
        baseBarAmbient = pointLights[barLightIndex].ambient;
    }

    // Hotel building (East side) with glowing double entrance, side door & windows
    HotelBuilder::buildHotel(rootNodes, pointLights);

    // Streetlamps
    auto poleMesh = createBox(0.2f, 4.5f, 0.2f, glm::vec3(0.2f, 0.2f, 0.22f));
    auto armMesh  = createBox(1.2f, 0.15f, 0.15f, glm::vec3(0.25f, 0.25f, 0.27f));
    glm::vec3 cyanLightColor(0.0f, 0.9f, 1.0f);
    glm::vec3 amberLightColor(1.0f, 0.65f, 0.15f);
    auto cyanFixtureMesh  = createBox(0.4f, 0.2f, 0.3f, cyanLightColor);
    auto amberFixtureMesh = createBox(0.4f, 0.2f, 0.3f, amberLightColor);

    float leftLampZ[]  = { -30.0f, -18.0f, -1.0f, 7.0f };
    float rightLampZ[] = { -30.0f, -18.0f, -1.0f, 13.0f };

    for (float z : leftLampZ) {
        SceneNode pole(poleMesh, glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 2.25f, z)));
        SceneNode arm(armMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.1f, 0.0f)));
        arm.addChild(SceneNode(cyanFixtureMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.15f, 0.0f)), true));
        pole.addChild(arm); rootNodes.push_back(pole);
        addPoint(glm::vec3(-3.2f, 4.2f, z), cyanLightColor, 0.05f, 1.3f, 1.2f, 0.14f, 0.07f);
    }
    for (float z : rightLampZ) {
        SceneNode pole(poleMesh, glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, 2.25f, z)));
        SceneNode arm(armMesh, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.1f, 0.0f)));
        arm.addChild(SceneNode(amberFixtureMesh, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.15f, 0.0f)), true));
        pole.addChild(arm); rootNodes.push_back(pole);
        addPoint(glm::vec3(3.2f, 4.2f, z), amberLightColor, 0.05f, 1.3f, 1.2f, 0.14f, 0.07f);
    }

    CityBuilder::buildExtendedStreet(rootNodes, pointLights, asphaltTex);
    AlleyBuilder::buildAlleys(rootNodes, pointLights);
    CityBuilder::buildSkyscrapers(skyscraperNodes, moonTex);
    DecalBuilder::buildDecals(rootNodes, decalNodes, pointLights);

    std::vector<SceneNode> opaque;
    partitionNodes(rootNodes, glm::mat4(1.0f), opaque, emissiveNodes);
    rootNodes = std::move(opaque);

    decalShader = std::unique_ptr<Shader>(new Shader("shaders/decal.vert", "shaders/decal.frag"));
    skyscraperShader = std::unique_ptr<Shader>(new Shader("shaders/skyscraper.vert", "shaders/skyscraper.frag"));
    emissiveShader = std::unique_ptr<Shader>(new Shader("shaders/emissive.vert", "shaders/emissive.frag"));
}

// Flatten and sort scene graph nodes into separate opaque and emissive rendering queues
void Scene::partitionNodes(const std::vector<SceneNode>& nodes, const glm::mat4& parentTransform,
                           std::vector<SceneNode>& opaque, std::vector<SceneNode>& emissive) {
    for (const auto& node : nodes) {
        glm::mat4 worldTr = parentTransform * node.localTransform;
        if (node.mesh) {
            SceneNode flat(node.mesh, worldTr, node.isEmissive, node.isRoad, node.texture, node.isFlickering, node.emissiveBoost);
            if (node.isEmissive) emissive.push_back(flat);
            else opaque.push_back(flat);
        }
        if (!node.children.empty()) partitionNodes(node.children, worldTr, opaque, emissive);
    }
}

// Bind directional moonlight and cache base point light intensities
void Scene::setupLights(const Shader& shader) {
    shader.setVec3("dirLight.direction", dirLight.direction);
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);

    basePointDiffuses.clear(); basePointSpeculars.clear(); basePointAmbients.clear();
    for (const auto& pl : pointLights) {
        basePointDiffuses.push_back(pl.diffuse);
        basePointSpeculars.push_back(pl.specular);
        basePointAmbients.push_back(pl.ambient);
    }
}

// Main scene draw pass binding global uniforms, dynamic lighting, and blackout states
void Scene::render(const Shader& shader, const glm::vec3& camPos, const glm::mat4& view, const glm::mat4& projection, float time, bool fogEnabled, bool flashlightOn, const glm::vec3& flashDir) {
    (void)time;
    s_stateCache = RenderStateCache();
    bool isSurge = (surgeTimer >= 0.0f);
    float surgeTime = isSurge ? surgeTimer : 0.0f;

    float cineNeon = cinematic ? cinematic->getNeonMultiplier() : 1.0f;
    float cineDir = cinematic ? cinematic->getDirLightMultiplier() : 1.0f;
    float effectiveNeon = barFlicker * cineNeon;
    shader.setFloat("neonFlicker", effectiveNeon);
    shader.setVec3("dirLight.direction", dirLight.direction);
    shader.setVec3("dirLight.ambient", dirLight.ambient * std::max(0.18f, cineDir));
    shader.setVec3("dirLight.diffuse", dirLight.diffuse * cineDir);
    shader.setVec3("dirLight.specular", dirLight.specular * cineDir);

    for (size_t i = 0; i < pointLights.size(); ++i) {
        if (i >= basePointDiffuses.size()) break;
        float lightFlicker = (static_cast<int>(i) == barLightIndex) ? barFlicker : 1.0f;
        if (isSurge && static_cast<int>(i) != barLightIndex) {
            float startOffset = std::fmod(static_cast<float>(i * 43 + 17) * 0.053f, 0.95f);
            float elemDur = 0.55f + std::fmod(static_cast<float>(i * 67 + 29) * 0.031f, 0.65f);
            float lt = surgeTime - startOffset;
            if (lt > 0.0f && lt < elemDur) {
                float jitter = 0.55f + 0.45f * std::sin(lt * (48.0f + static_cast<float>(i * 7 % 23) * 4.0f));
                if (lt > 0.15f && lt < 0.28f && (i % 3 == 0)) jitter *= 0.12f;
                else if (lt > 0.35f && lt < 0.48f && (i % 2 == 1)) jitter *= 0.18f;
                lightFlicker = std::max(0.08f, std::min(1.35f, jitter));
            }
        }
        float pMult = cinematic ? cinematic->getLightMultiplier(i, pointLights[i].position) : 1.0f;
        float finalMult = lightFlicker * pMult;
        pointLights[i].diffuse = basePointDiffuses[i] * finalMult; pointLights[i].specular = basePointSpeculars[i] * finalMult; pointLights[i].ambient = basePointAmbients[i] * finalMult;
    }

    int activeCount = static_cast<int>(std::min(pointLights.size(), size_t(32)));
    shader.setInt("activePointLights", activeCount);
    for (int i = 0; i < activeCount; ++i) {
        const auto& pl = pointLights[i];
        std::string p = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3(p + "position", pl.position); shader.setVec3(p + "diffuse", pl.diffuse);
        shader.setVec3(p + "specular", pl.specular); shader.setVec3(p + "ambient", pl.ambient);
        shader.setFloat(p + "constant", pl.constant); shader.setFloat(p + "linear", pl.linear);
        shader.setFloat(p + "quadratic", pl.quadratic);
    }

    if (emissiveShader && !emissiveNodes.empty()) {
        s_stateCache = RenderStateCache(); emissiveShader->use();
        emissiveShader->setMat4("view", view); emissiveShader->setMat4("projection", projection);
        emissiveShader->setVec3("viewPos", camPos); emissiveShader->setBool("fogEnabled", fogEnabled);
        emissiveShader->setFloat("neonFlicker", effectiveNeon);
        emissiveShader->setFloat("powerFactor", cineNeon);
        emissiveShader->setBool("spotLight.enabled", flashlightOn);
        emissiveShader->setVec3("spotLight.position", camPos);
        emissiveShader->setVec3("spotLight.direction", flashDir);
        emissiveShader->setFloat("spotLight.cutOff", 0.976296f); emissiveShader->setFloat("spotLight.outerCutOff", 0.953717f);
        for (const auto& node : emissiveNodes) node.draw(*emissiveShader);
    }

    s_stateCache = RenderStateCache();
    shader.use();
    for (const auto& node : rootNodes) node.draw(shader);

    if (skyscraperShader && !skyscraperNodes.empty()) {
        s_stateCache = RenderStateCache(); skyscraperShader->use();
        skyscraperShader->setMat4("view", view); skyscraperShader->setMat4("projection", projection);
        skyscraperShader->setVec3("viewPos", camPos); skyscraperShader->setBool("fogEnabled", fogEnabled);
        skyscraperShader->setVec3("dirLight.direction", dirLight.direction);
        skyscraperShader->setVec3("dirLight.ambient", dirLight.ambient * std::max(0.18f, cineDir));
        skyscraperShader->setVec3("dirLight.diffuse", dirLight.diffuse * cineDir);
        skyscraperShader->setFloat("powerFactor", cineNeon);
        for (const auto& node : skyscraperNodes) node.draw(*skyscraperShader);
    }

    if (decalShader && !decalNodes.empty()) {
        s_stateCache = RenderStateCache();
        glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        decalShader->use();
        decalShader->setMat4("view", view); decalShader->setMat4("projection", projection);
        decalShader->setVec3("viewPos", camPos); decalShader->setBool("fogEnabled", fogEnabled);
        decalShader->setFloat("neonFlicker", effectiveNeon);
        decalShader->setFloat("powerFactor", cineNeon);
        decalShader->setBool("spotLight.enabled", flashlightOn);
        decalShader->setVec3("spotLight.position", camPos);
        decalShader->setVec3("spotLight.direction", flashDir);
        decalShader->setFloat("spotLight.cutOff", 0.976296f); decalShader->setFloat("spotLight.outerCutOff", 0.953717f);
        for (const auto& decal : decalNodes) decal.draw(*decalShader);
        glDisable(GL_BLEND);
    }
}
