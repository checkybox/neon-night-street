#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <string>

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
    float hw = width * 0.5f;
    float hd = depth * 0.5f;

    std::vector<Vertex> vertices = {
        { glm::vec3(-hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.0f,  tileY) },
        { glm::vec3( hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(tileX, tileY) },
        { glm::vec3( hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(tileX, 0.0f) },
        { glm::vec3(-hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.0f,  0.0f) }
    };

    std::vector<unsigned int> indices = { 0, 2, 1, 0, 3, 2 };
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> Scene::createBox(float width, float height, float depth, const glm::vec3& color) {
    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    std::vector<Vertex> vertices = {
        // Front face (normal 0, 0, 1)
        { glm::vec3(-w, -h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3( w, -h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3( w,  h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-w,  h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color, glm::vec2(0.0f, 1.0f) },
        // Back face (normal 0, 0, -1)
        { glm::vec3( w, -h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3(-w, -h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3(-w,  h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3( w,  h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color, glm::vec2(0.0f, 1.0f) },
        // Top face (normal 0, 1, 0)
        { glm::vec3(-w,  h,  d), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3( w,  h,  d), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3( w,  h, -d), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-w,  h, -d), glm::vec3(0.0f, 1.0f, 0.0f), color, glm::vec2(0.0f, 1.0f) },
        // Bottom face (normal 0, -1, 0)
        { glm::vec3(-w, -h, -d), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3( w, -h, -d), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3( w, -h,  d), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-w, -h,  d), glm::vec3(0.0f, -1.0f, 0.0f), color, glm::vec2(0.0f, 1.0f) },
        // Right face (normal 1, 0, 0) - oriented toward positive X (street-facing for left buildings)
        { glm::vec3( w, -h,  d), glm::vec3(1.0f, 0.0f, 0.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3( w, -h, -d), glm::vec3(1.0f, 0.0f, 0.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3( w,  h, -d), glm::vec3(1.0f, 0.0f, 0.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3( w,  h,  d), glm::vec3(1.0f, 0.0f, 0.0f), color, glm::vec2(0.0f, 1.0f) },
        // Left face (normal -1, 0, 0) - oriented toward negative X (street-facing for right buildings)
        { glm::vec3(-w, -h, -d), glm::vec3(-1.0f, 0.0f, 0.0f), color, glm::vec2(0.0f, 0.0f) },
        { glm::vec3(-w, -h,  d), glm::vec3(-1.0f, 0.0f, 0.0f), color, glm::vec2(1.0f, 0.0f) },
        { glm::vec3(-w,  h,  d), glm::vec3(-1.0f, 0.0f, 0.0f), color, glm::vec2(1.0f, 1.0f) },
        { glm::vec3(-w,  h, -d), glm::vec3(-1.0f, 0.0f, 0.0f), color, glm::vec2(0.0f, 1.0f) }
    };

    std::vector<unsigned int> indices = {
         0,  1,  2,  0,  2,  3,
         4,  5,  6,  4,  6,  7,
         8,  9, 10,  8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    return std::make_shared<Mesh>(vertices, indices);
}

void Scene::init() {
    rootNodes.clear();
    pointLights.clear();

    // 0. Load Textures with Trilinear Mipmapping
    asphaltTex = std::make_shared<Texture>("assets/textures/asphalt.png", true);
    ramenTex   = std::make_shared<Texture>("assets/textures/sign_ramen.png", true);
    barTex     = std::make_shared<Texture>("assets/textures/sign_bar.png", true);
    hotelTex   = std::make_shared<Texture>("assets/textures/sign_hotel.png", true);

    // Directional Moonlight
    dirLight.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    dirLight.ambient   = glm::vec3(0.03f, 0.03f, 0.07f);
    dirLight.diffuse   = glm::vec3(0.06f, 0.07f, 0.12f);
    dirLight.specular  = glm::vec3(0.08f, 0.09f, 0.15f);

    // 1. Road surface with textured asphalt and procedural non-repeating FBM puddles
    auto roadMesh = createPlane(8.0f, 60.0f, glm::vec3(0.12f, 0.12f, 0.13f), 4.0f, 24.0f);
    glm::mat4 roadTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    rootNodes.push_back(SceneNode(roadMesh, roadTransform, false, true, asphaltTex));

    // 2. Center lane stripes
    auto stripeMesh = createPlane(0.2f, 2.0f, glm::vec3(0.9f, 0.8f, 0.2f));
    for (float z = -36.0f; z <= 16.0f; z += 5.0f) {
        glm::mat4 stripeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z));
        rootNodes.push_back(SceneNode(stripeMesh, stripeTransform));
    }

    // 3. Sidewalks
    auto sidewalkMesh = createBox(3.0f, 0.2f, 60.0f, glm::vec3(0.22f, 0.22f, 0.24f));
    glm::mat4 leftSidewalkTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, 0.1f, -10.0f));
    rootNodes.push_back(SceneNode(sidewalkMesh, leftSidewalkTransform));

    glm::mat4 rightSidewalkTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.5f, 0.1f, -10.0f));
    rootNodes.push_back(SceneNode(sidewalkMesh, rightSidewalkTransform));

    // 4. Modular buildings
    glm::vec3 buildingColors[] = {
        glm::vec3(0.18f, 0.17f, 0.20f),
        glm::vec3(0.15f, 0.16f, 0.22f),
        glm::vec3(0.20f, 0.15f, 0.16f),
        glm::vec3(0.16f, 0.18f, 0.17f)
    };

    float zStart = -35.0f;
    for (int i = 0; i < 4; ++i) {
        float bDepth = 12.0f;
        float bWidth = 6.0f;
        float bHeight = 8.0f + (i % 3) * 2.5f;
        float zPos = zStart + i * 14.0f;

        auto buildingMesh = createBox(bWidth, bHeight, bDepth, buildingColors[i % 4]);

        glm::mat4 leftBldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, bHeight * 0.5f, zPos));
        rootNodes.push_back(SceneNode(buildingMesh, leftBldTransform));

        glm::mat4 rightBldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, bHeight * 0.5f, zPos));
        rootNodes.push_back(SceneNode(buildingMesh, rightBldTransform));
    }

    // 5. Roadblock barriers
    auto barrierMesh = createBox(8.0f, 1.0f, 0.4f, glm::vec3(0.85f, 0.35f, 0.05f));
    glm::mat4 northBarrier = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -39.0f));
    glm::mat4 southBarrier = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 19.0f));
    rootNodes.push_back(SceneNode(barrierMesh, northBarrier));
    rootNodes.push_back(SceneNode(barrierMesh, southBarrier));

    // 6. Hierarchical Streetlamps & 8 Streetlamp Point Lights
    auto poleMesh = createBox(0.2f, 4.5f, 0.2f, glm::vec3(0.2f, 0.2f, 0.22f));
    auto armMesh = createBox(1.2f, 0.15f, 0.15f, glm::vec3(0.25f, 0.25f, 0.27f));

    glm::vec3 cyanLightColor(0.0f, 0.9f, 1.0f);
    glm::vec3 amberLightColor(1.0f, 0.65f, 0.15f);

    auto cyanFixtureMesh = createBox(0.4f, 0.2f, 0.3f, cyanLightColor);
    auto amberFixtureMesh = createBox(0.4f, 0.2f, 0.3f, amberLightColor);

    float lampZPositions[] = { -30.0f, -15.0f, 0.0f, 12.0f };

    float constant = 1.0f;
    float linear = 0.14f;
    float quadratic = 0.07f;

    // 4 Left Streetlamps (Cyan Neon)
    for (float z : lampZPositions) {
        glm::mat4 poleTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 2.25f, z));
        SceneNode poleNode(poleMesh, poleTransform);

        glm::mat4 armTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.1f, 0.0f));
        SceneNode armNode(armMesh, armTransform);

        glm::mat4 fixtureTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.15f, 0.0f));
        armNode.addChild(SceneNode(cyanFixtureMesh, fixtureTransform, true));

        poleNode.addChild(armNode);
        rootNodes.push_back(poleNode);

        PointLight pl;
        pl.position = glm::vec3(-3.2f, 4.2f, z);
        pl.ambient = cyanLightColor * 0.05f;
        pl.diffuse = cyanLightColor * 1.3f;
        pl.specular = cyanLightColor * 1.2f;
        pl.constant = constant;
        pl.linear = linear;
        pl.quadratic = quadratic;
        pointLights.push_back(pl);
    }

    // 4 Right Streetlamps (Amber Neon)
    for (float z : lampZPositions) {
        glm::mat4 poleTransform = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, 2.25f, z));
        SceneNode poleNode(poleMesh, poleTransform);

        glm::mat4 armTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.1f, 0.0f));
        SceneNode armNode(armMesh, armTransform);

        glm::mat4 fixtureTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.15f, 0.0f));
        armNode.addChild(SceneNode(amberFixtureMesh, fixtureTransform, true));

        poleNode.addChild(armNode);
        rootNodes.push_back(poleNode);

        PointLight pl;
        pl.position = glm::vec3(3.2f, 4.2f, z);
        pl.ambient = amberLightColor * 0.05f;
        pl.diffuse = amberLightColor * 1.3f;
        pl.specular = amberLightColor * 1.2f;
        pl.constant = constant;
        pl.linear = linear;
        pl.quadratic = quadratic;
        pointLights.push_back(pl);
    }

    // 7. Storefront Neon Signs with Japanese & English CJK Typography
    auto signBoxMesh = createBox(0.15f, 1.8f, 3.6f, glm::vec3(1.0f));

    // Sign 1: Ramen Sign (Left storefront at Z = -8.0)
    glm::mat4 ramenTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-6.92f, 3.2f, -8.0f));
    rootNodes.push_back(SceneNode(signBoxMesh, ramenTransform, true, false, ramenTex));

    PointLight magentaLight;
    magentaLight.position = glm::vec3(-6.5f, 3.2f, -8.0f);
    glm::vec3 magentaColor(1.0f, 0.2f, 0.1f);
    magentaLight.ambient = magentaColor * 0.06f;
    magentaLight.diffuse = magentaColor * 1.8f;
    magentaLight.specular = magentaColor * 1.6f;
    magentaLight.constant = constant;
    magentaLight.linear = 0.22f;
    magentaLight.quadratic = 0.12f;
    pointLights.push_back(magentaLight);

    // Sign 2: Neon Bar Sign (Right storefront at Z = -22.0)
    glm::mat4 barTransform = glm::translate(glm::mat4(1.0f), glm::vec3(6.92f, 3.2f, -22.0f));
    rootNodes.push_back(SceneNode(signBoxMesh, barTransform, true, false, barTex));

    PointLight barLight;
    barLight.position = glm::vec3(6.5f, 3.2f, -22.0f);
    glm::vec3 barPinkColor(1.0f, 0.1f, 0.7f);
    barLight.ambient = barPinkColor * 0.06f;
    barLight.diffuse = barPinkColor * 1.8f;
    barLight.specular = barPinkColor * 1.6f;
    barLight.constant = constant;
    barLight.linear = 0.22f;
    barLight.quadratic = 0.12f;
    pointLights.push_back(barLight);

    // Sign 3: Cyber Hotel Sign (Right storefront at Z = 5.0)
    glm::mat4 hotelTransform = glm::translate(glm::mat4(1.0f), glm::vec3(6.92f, 3.2f, 5.0f));
    rootNodes.push_back(SceneNode(signBoxMesh, hotelTransform, true, false, hotelTex));
}

void Scene::setupLights(const Shader& shader) {
    shader.setVec3("dirLight.direction", dirLight.direction);
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);

    for (size_t i = 0; i < pointLights.size(); ++i) {
        std::string prefix = "pointLights[" + std::to_string(i) + "].";
        shader.setVec3(prefix + "position", pointLights[i].position);
        shader.setVec3(prefix + "ambient", pointLights[i].ambient);
        shader.setVec3(prefix + "diffuse", pointLights[i].diffuse);
        shader.setVec3(prefix + "specular", pointLights[i].specular);
        shader.setFloat(prefix + "constant", pointLights[i].constant);
        shader.setFloat(prefix + "linear", pointLights[i].linear);
        shader.setFloat(prefix + "quadratic", pointLights[i].quadratic);
    }
}

void Scene::render(const Shader& shader) {
    for (const auto& node : rootNodes) {
        node.draw(shader);
    }
}
