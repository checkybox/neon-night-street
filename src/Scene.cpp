#include "Scene.h"
#include <glm/gtc/matrix_transform.hpp>

SceneNode::SceneNode() : mesh(nullptr), localTransform(glm::mat4(1.0f)) {}

SceneNode::SceneNode(std::shared_ptr<Mesh> mesh, const glm::mat4& transform)
    : mesh(mesh), localTransform(transform) {}

void SceneNode::addChild(const SceneNode& child) {
    children.push_back(child);
}

void SceneNode::draw(const Shader& shader, const glm::mat4& parentTransform) const {
    glm::mat4 globalTransform = parentTransform * localTransform;

    if (mesh) {
        shader.setMat4("model", globalTransform);
        mesh->draw();
    }

    for (const auto& child : children) {
        child.draw(shader, globalTransform);
    }
}

Scene::Scene() {}
Scene::~Scene() {}

std::shared_ptr<Mesh> Scene::createPlane(float width, float depth, const glm::vec3& color) {
    float hw = width * 0.5f;
    float hd = depth * 0.5f;

    std::vector<Vertex> vertices = {
        { glm::vec3(-hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3( hw, 0.0f, -hd), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3( hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3(-hw, 0.0f,  hd), glm::vec3(0.0f, 1.0f, 0.0f), color }
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
        { glm::vec3(-w, -h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color },
        { glm::vec3( w, -h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color },
        { glm::vec3( w,  h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color },
        { glm::vec3(-w,  h,  d), glm::vec3(0.0f, 0.0f, 1.0f), color },
        // Back face (normal 0, 0, -1)
        { glm::vec3( w, -h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3(-w, -h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3(-w,  h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color },
        { glm::vec3( w,  h, -d), glm::vec3(0.0f, 0.0f, -1.0f), color },
        // Top face (normal 0, 1, 0)
        { glm::vec3(-w,  h,  d), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3( w,  h,  d), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3( w,  h, -d), glm::vec3(0.0f, 1.0f, 0.0f), color },
        { glm::vec3(-w,  h, -d), glm::vec3(0.0f, 1.0f, 0.0f), color },
        // Bottom face (normal 0, -1, 0)
        { glm::vec3(-w, -h, -d), glm::vec3(0.0f, -1.0f, 0.0f), color },
        { glm::vec3( w, -h, -d), glm::vec3(0.0f, -1.0f, 0.0f), color },
        { glm::vec3( w, -h,  d), glm::vec3(0.0f, -1.0f, 0.0f), color },
        { glm::vec3(-w, -h,  d), glm::vec3(0.0f, -1.0f, 0.0f), color },
        // Right face (normal 1, 0, 0)
        { glm::vec3( w, -h,  d), glm::vec3(1.0f, 0.0f, 0.0f), color },
        { glm::vec3( w, -h, -d), glm::vec3(1.0f, 0.0f, 0.0f), color },
        { glm::vec3( w,  h, -d), glm::vec3(1.0f, 0.0f, 0.0f), color },
        { glm::vec3( w,  h,  d), glm::vec3(1.0f, 0.0f, 0.0f), color },
        // Left face (normal -1, 0, 0)
        { glm::vec3(-w, -h, -d), glm::vec3(-1.0f, 0.0f, 0.0f), color },
        { glm::vec3(-w, -h,  d), glm::vec3(-1.0f, 0.0f, 0.0f), color },
        { glm::vec3(-w,  h,  d), glm::vec3(-1.0f, 0.0f, 0.0f), color },
        { glm::vec3(-w,  h, -d), glm::vec3(-1.0f, 0.0f, 0.0f), color }
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

    // 1. Road surface
    auto roadMesh = createPlane(8.0f, 60.0f, glm::vec3(0.08f, 0.08f, 0.09f));
    glm::mat4 roadTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f));
    rootNodes.push_back(SceneNode(roadMesh, roadTransform));

    // 2. Center lane stripes
    auto stripeMesh = createPlane(0.2f, 2.0f, glm::vec3(0.85f, 0.75f, 0.1f));
    for (float z = -36.0f; z <= 16.0f; z += 5.0f) {
        glm::mat4 stripeTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z));
        rootNodes.push_back(SceneNode(stripeMesh, stripeTransform));
    }

    // 3. Sidewalks (elevated curbs)
    auto sidewalkMesh = createBox(3.0f, 0.2f, 60.0f, glm::vec3(0.2f, 0.2f, 0.22f));
    // Left sidewalk
    glm::mat4 leftSidewalkTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-5.5f, 0.1f, -10.0f));
    rootNodes.push_back(SceneNode(sidewalkMesh, leftSidewalkTransform));
    // Right sidewalk
    glm::mat4 rightSidewalkTransform = glm::translate(glm::mat4(1.0f), glm::vec3(5.5f, 0.1f, -10.0f));
    rootNodes.push_back(SceneNode(sidewalkMesh, rightSidewalkTransform));

    // 4. Modular buildings along both sides of the street
    glm::vec3 buildingColors[] = {
        glm::vec3(0.14f, 0.13f, 0.15f),
        glm::vec3(0.11f, 0.12f, 0.16f),
        glm::vec3(0.15f, 0.11f, 0.11f),
        glm::vec3(0.12f, 0.14f, 0.13f)
    };

    float zStart = -35.0f;
    for (int i = 0; i < 4; ++i) {
        float bDepth = 12.0f;
        float bWidth = 6.0f;
        float bHeight = 8.0f + (i % 3) * 2.5f;
        float zPos = zStart + i * 14.0f;

        auto buildingMesh = createBox(bWidth, bHeight, bDepth, buildingColors[i % 4]);

        // Left building block
        glm::mat4 leftBldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, bHeight * 0.5f, zPos));
        rootNodes.push_back(SceneNode(buildingMesh, leftBldTransform));

        // Right building block
        glm::mat4 rightBldTransform = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, bHeight * 0.5f, zPos));
        rootNodes.push_back(SceneNode(buildingMesh, rightBldTransform));
    }

    // 5. Street boundaries / Roadblock barriers
    auto barrierMesh = createBox(8.0f, 1.0f, 0.4f, glm::vec3(0.85f, 0.35f, 0.05f));
    glm::mat4 northBarrier = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, -39.0f));
    glm::mat4 southBarrier = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.5f, 19.0f));
    rootNodes.push_back(SceneNode(barrierMesh, northBarrier));
    rootNodes.push_back(SceneNode(barrierMesh, southBarrier));

    // 6. Hierarchical Streetlamps (Parent: Pole -> Child: Arm & Fixture)
    auto poleMesh = createBox(0.2f, 4.5f, 0.2f, glm::vec3(0.15f, 0.15f, 0.16f));
    auto armMesh = createBox(1.2f, 0.15f, 0.15f, glm::vec3(0.2f, 0.2f, 0.22f));
    auto fixtureMesh = createBox(0.4f, 0.2f, 0.3f, glm::vec3(0.0f, 0.85f, 0.85f)); // Cyan neon fixture

    float lampZPositions[] = { -30.0f, -15.0f, 0.0f, 12.0f };
    for (float z : lampZPositions) {
        // Left streetlamp
        glm::mat4 poleTransformLeft = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 2.25f, z));
        SceneNode poleNodeLeft(poleMesh, poleTransformLeft);

        glm::mat4 armTransformLeft = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.1f, 0.0f));
        SceneNode armNodeLeft(armMesh, armTransformLeft);

        glm::mat4 fixtureTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.15f, 0.0f));
        armNodeLeft.addChild(SceneNode(fixtureMesh, fixtureTransform));

        poleNodeLeft.addChild(armNodeLeft);
        rootNodes.push_back(poleNodeLeft);

        // Right streetlamp
        glm::mat4 poleTransformRight = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, 2.25f, z));
        SceneNode poleNodeRight(poleMesh, poleTransformRight);

        glm::mat4 armTransformRight = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.1f, 0.0f));
        SceneNode armNodeRight(armMesh, armTransformRight);

        glm::mat4 fixtureTransformRight = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.15f, 0.0f));
        armNodeRight.addChild(SceneNode(fixtureMesh, fixtureTransformRight));

        poleNodeRight.addChild(armNodeRight);
        rootNodes.push_back(poleNodeRight);
    }
}

void Scene::render(const Shader& shader) {
    for (const auto& node : rootNodes) {
        node.draw(shader);
    }
}
