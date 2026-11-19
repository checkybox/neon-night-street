#include "AlleyBuilder.h"
#include "CityBuilder.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

void AlleyBuilder::buildAlleys(std::vector<SceneNode>& rootNodes,
                             std::vector<PointLight>& pointLights) {
    auto addPoint = [&](const glm::vec3& p, const glm::vec3& c, float amb, float diff, float spec, float lin, float quad) {
        pointLights.push_back({ p, c * amb, c * diff, c * spec, 1.0f, lin, quad });
    };

    auto trashBagMesh   = Model::loadOBJ("assets/models/trash_bag.obj", glm::vec3(0.08f, 0.08f, 0.09f));
    auto fireEscapeMesh = Model::loadOBJ("assets/models/fire_escape.obj", glm::vec3(0.22f, 0.23f, 0.25f));

    auto addBag = [&](float x, float y, float z, float scale, float yaw) {
        if (!trashBagMesh) return;
        glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
                     * glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f))
                     * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
        rootNodes.push_back(SceneNode(trashBagMesh, tr));
    };

    auto addBoxNode = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive));
    };

    // East service alley
    addBoxNode(0.4f, 16.0f, 2.6f, glm::vec3(0.18f, 0.17f, 0.19f), glm::vec3(13.8f, 8.0f, -44.75f));
    rootNodes.push_back(SceneNode(CityBuilder::createPlane(7.0f, 2.5f, glm::vec3(0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.005f, -44.75f)), false, true));

    if (fireEscapeMesh) {
        for (float y : { 3.4f, 6.8f, 10.2f }) {
            glm::mat4 feTr = glm::translate(glm::mat4(1.0f), glm::vec3(9.2f, y, -45.95f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(fireEscapeMesh, feTr));
        }
    }

    addBag(8.2f, 0.0f, -45.4f, 1.0f, 15.0f);
    addBag(8.8f, 0.0f, -45.2f, 0.95f, 75.0f);
    addBag(9.3f, 0.0f, -45.3f, 1.05f, 140.0f);
    addBag(8.5f, 0.35f, -45.3f, 0.85f, 210.0f);

    addBoxNode(1.1f, 0.12f, 1.1f, glm::vec3(0.32f, 0.22f, 0.14f), glm::vec3(11.2f, 0.06f, -44.5f));
    addBoxNode(0.7f, 0.65f, 0.7f, glm::vec3(0.42f, 0.32f, 0.20f), glm::vec3(11.2f, 0.44f, -44.5f));
    addBoxNode(0.55f, 0.5f, 0.55f, glm::vec3(0.38f, 0.28f, 0.18f), glm::vec3(11.2f, 1.02f, -44.5f));
    addBoxNode(0.6f, 0.5f, 0.8f, glm::vec3(0.35f, 0.26f, 0.16f), glm::vec3(12.2f, 0.25f, -44.6f));

    addBoxNode(0.15f, 0.2f, 0.15f, glm::vec3(1.0f, 0.8f, 0.4f), glm::vec3(9.2f, 2.6f, -45.85f), true);
    addPoint(glm::vec3(9.2f, 2.5f, -44.75f), glm::vec3(1.0f, 0.75f, 0.35f), 0.03f, 1.1f, 0.9f, 0.22f, 0.12f);

    // West service alley
    addBoxNode(0.4f, 13.5f, 2.6f, glm::vec3(0.18f, 0.17f, 0.19f), glm::vec3(-13.8f, 6.75f, -44.75f));
    rootNodes.push_back(SceneNode(CityBuilder::createPlane(7.0f, 2.5f, glm::vec3(0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.005f, -44.75f)), false, true));

    if (fireEscapeMesh) {
        for (float y : { 3.4f, 6.8f, 10.2f }) {
            glm::mat4 feTr = glm::translate(glm::mat4(1.0f), glm::vec3(-9.2f, y, -43.55f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(fireEscapeMesh, feTr));
        }
    }

    addBoxNode(1.6f, 1.2f, 2.0f, glm::vec3(0.15f, 0.22f, 0.18f), glm::vec3(-11.5f, 0.6f, -44.75f));
    addBoxNode(1.65f, 0.1f, 1.05f, glm::vec3(0.10f, 0.10f, 0.12f), glm::vec3(-11.5f, 1.25f, -45.25f));
    addBoxNode(1.65f, 0.1f, 1.05f, glm::vec3(0.10f, 0.10f, 0.12f), glm::vec3(-11.5f, 1.25f, -44.25f));

    addBag(-8.8f, 0.0f, -45.3f, 1.0f, 40.0f);
    addBag(-9.4f, 0.0f, -45.2f, 1.05f, 110.0f);
    addBag(-9.1f, 0.0f, -44.4f, 0.95f, 190.0f);
    addBag(-9.1f, 0.38f, -45.2f, 0.85f, 320.0f);

    addBoxNode(0.8f, 0.6f, 0.6f, glm::vec3(0.38f, 0.28f, 0.18f), glm::vec3(-8.2f, 0.3f, -45.2f));
    addBoxNode(0.6f, 0.45f, 0.5f, glm::vec3(0.44f, 0.34f, 0.22f), glm::vec3(-8.2f, 0.82f, -45.2f));

    addBoxNode(0.15f, 0.2f, 0.15f, glm::vec3(0.9f, 0.95f, 1.0f), glm::vec3(-9.2f, 2.8f, -43.65f), true);
    addPoint(glm::vec3(-9.2f, 2.7f, -44.75f), glm::vec3(0.7f, 0.85f, 1.0f), 0.03f, 1.1f, 0.9f, 0.22f, 0.12f);

    // Arcade side alley
    addBoxNode(0.4f, 11.5f, 2.8f, glm::vec3(0.18f, 0.17f, 0.19f), glm::vec3(13.8f, 5.75f, 9.85f));
    rootNodes.push_back(SceneNode(CityBuilder::createPlane(7.0f, 2.7f, glm::vec3(0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.005f, 9.85f)), false, true));

    if (fireEscapeMesh) {
        for (float y : { 3.4f, 6.8f }) {
            glm::mat4 feTr = glm::translate(glm::mat4(1.0f), glm::vec3(9.2f, y, 11.15f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(fireEscapeMesh, feTr));
        }
    }

    addBag(8.6f, 0.0f, 9.3f, 1.0f, 25.0f);
    addBag(9.1f, 0.0f, 9.5f, 0.92f, 160.0f);
    addBoxNode(0.6f, 0.5f, 0.6f, glm::vec3(0.40f, 0.30f, 0.20f), glm::vec3(10.8f, 0.25f, 9.5f));

    // Bookstore display
    addBoxNode(0.08f, 2.1f, 3.8f, glm::vec3(0.95f, 0.88f, 0.65f), glm::vec3(6.45f, 1.35f, 33.0f), true);
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 31.1f));
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 33.0f));
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 34.9f));
    addBoxNode(0.10f, 0.12f, 3.9f,  glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 33.0f));

    // Books neon letters
    glm::vec3 bookNeonGold(1.0f, 0.88f, 0.22f);
    float bX = 6.36f, bY = 3.65f;

    auto addBStroke = [&](float w, float h, float d, const glm::vec3& p) {
        addBoxNode(w, h, d, bookNeonGold, p, true);
    };

    // B
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 29.40f));
    addBStroke(0.06f, 0.08f, 0.35f, glm::vec3(bX, bY + 0.38f, 29.55f));
    addBStroke(0.06f, 0.08f, 0.32f, glm::vec3(bX, bY,        29.54f));
    addBStroke(0.06f, 0.08f, 0.35f, glm::vec3(bX, bY - 0.38f, 29.55f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY + 0.20f, 29.72f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY - 0.20f, 29.72f));

    // O
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.12f));
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.48f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 30.30f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 30.30f));

    // O
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.82f));
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 31.18f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 31.00f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 31.00f));

    // K
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 31.50f));
    glm::mat4 armUp = glm::translate(glm::mat4(1.0f), glm::vec3(bX, bY + 0.20f, 31.68f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(42.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.06f, 0.48f, 0.08f, bookNeonGold), armUp, true));
    glm::mat4 armDn = glm::translate(glm::mat4(1.0f), glm::vec3(bX, bY - 0.20f, 31.68f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(-42.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.06f, 0.48f, 0.08f, bookNeonGold), armDn, true));

    // S
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 32.40f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY,        32.40f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 32.40f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY + 0.20f, 32.24f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY - 0.20f, 32.56f));
}

void AlleyBuilder::buildDeadEnds(std::vector<SceneNode>& rootNodes,
                                std::vector<PointLight>& pointLights) {
    auto addBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive));
    };

    auto trashBagMesh = Model::loadOBJ("assets/models/trash_bag.obj", glm::vec3(0.08f, 0.08f, 0.09f));
    auto addBag = [&](float x, float y, float z, float scale, float yaw) {
        if (!trashBagMesh) return;
        glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
                     * glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f))
                     * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
        rootNodes.push_back(SceneNode(trashBagMesh, tr));
    };

    glm::vec3 wallCol(0.18f, 0.17f, 0.19f);

    // East dead-end crossroad branch
    addBox(0.50f, 6.5f, 12.0f, wallCol, glm::vec3(18.5f, 3.25f, -8.25f));
    addBox(4.50f, 6.5f, 0.40f, wallCol, glm::vec3(16.25f, 3.25f, -14.0f));
    addBox(4.50f, 6.5f, 0.40f, wallCol, glm::vec3(16.25f, 3.25f, -2.5f));

    // Steel crash barrier gate with hazard paint
    addBox(0.40f, 1.0f, 8.0f, glm::vec3(0.85f, 0.40f, 0.10f), glm::vec3(17.8f, 0.5f, -8.0f));
    for (float bz = -11.0f; bz <= -5.0f; bz += 2.0f) {
        addBox(0.42f, 0.25f, 0.40f, glm::vec3(0.15f), glm::vec3(17.8f, 0.5f, bz));
    }

    // Industrial service doorway in perimeter wall
    addBox(0.06f, 2.30f, 1.20f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(18.22f, 1.15f, -6.5f));
    addBox(0.04f, 2.20f, 1.10f, glm::vec3(0.24f, 0.22f, 0.20f), glm::vec3(18.24f, 1.15f, -6.5f));
    addBox(0.15f, 0.15f, 0.15f, glm::vec3(1.0f, 0.70f, 0.20f), glm::vec3(18.20f, 2.50f, -6.5f), true);
    pointLights.push_back({ glm::vec3(17.6f, 2.4f, -6.5f), glm::vec3(1.0f, 0.70f, 0.20f) * 0.05f,
                            glm::vec3(1.0f, 0.70f, 0.20f) * 1.8f, glm::vec3(1.0f, 0.70f, 0.20f) * 1.5f,
                            1.0f, 0.18f, 0.09f });

    // Wooden cargo pallets stacked against East wall (batched)
    std::vector<Vertex> palletVerts;
    std::vector<unsigned int> palletIdxs;
    for (int p = 0; p < 4; ++p) {
        CityBuilder::appendBox(palletVerts, palletIdxs, 1.2f, 0.14f, 1.2f, glm::vec3(0.38f, 0.28f, 0.18f), glm::translate(glm::mat4(1.0f), glm::vec3(17.5f, 0.07f + p * 0.15f, -10.5f)));
    }
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(palletVerts, palletIdxs), glm::mat4(1.0f), false));
    // Cardboard boxes
    addBox(0.8f, 0.8f, 0.8f, glm::vec3(0.48f, 0.38f, 0.24f), glm::vec3(17.4f, 0.40f, -4.5f));
    addBox(0.6f, 0.6f, 0.6f, glm::vec3(0.42f, 0.34f, 0.22f), glm::vec3(17.4f, 1.10f, -4.5f));

    // West dead-end crossroad branch and restricted zone behind Konbini
    addBox(0.50f, 6.5f, 23.0f, wallCol, glm::vec3(-18.5f, 3.25f, -2.5f));
    // North perimeter wall directly across the street from the fence (closing gap seen in Screenshot 2)
    addBox(4.80f, 6.5f, 0.40f, wallCol, glm::vec3(-16.15f, 3.25f, -14.0f));
    addBox(4.80f, 0.12f, 0.50f, glm::vec3(0.24f, 0.24f, 0.26f), glm::vec3(-16.15f, 6.56f, -14.0f));
    // South perimeter wall enclosing the restricted yard behind the fence
    addBox(5.80f, 4.5f, 0.40f, wallCol, glm::vec3(-15.60f, 2.25f, 8.5f));
    addBox(5.80f, 0.12f, 0.50f, glm::vec3(0.24f, 0.24f, 0.26f), glm::vec3(-15.60f, 4.56f, 8.5f));

    // Unturned-Style Wire Grate Fence with angled top arms and barbed wire (batched)
    std::vector<Vertex> fenceVerts;
    std::vector<unsigned int> fenceIdxs;
    float fencePostXs[] = { -12.8f, -14.7f, -16.6f, -18.5f };
    glm::vec3 postCol(0.24f, 0.24f, 0.26f);
    glm::vec3 wireCol(0.20f, 0.21f, 0.23f);

    for (float px : fencePostXs) {
        CityBuilder::appendBox(fenceVerts, fenceIdxs, 0.14f, 2.20f, 0.14f, postCol, glm::translate(glm::mat4(1.0f), glm::vec3(px, 1.10f, -2.50f)));
        glm::mat4 armTr = glm::translate(glm::mat4(1.0f), glm::vec3(px, 2.30f, -2.40f))
                        * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        CityBuilder::appendBox(fenceVerts, fenceIdxs, 0.10f, 0.38f, 0.10f, postCol, armTr);
    }

    // 3 horizontal barbed wire strands across the angled top arms
    float barbYs[] = { 2.22f, 2.34f, 2.46f };
    float barbZs[] = { -2.46f, -2.38f, -2.30f };
    for (int b = 0; b < 3; ++b) {
        CityBuilder::appendBox(fenceVerts, fenceIdxs, 5.70f, 0.02f, 0.02f, glm::vec3(0.35f, 0.36f, 0.38f), glm::translate(glm::mat4(1.0f), glm::vec3(-15.65f, barbYs[b], barbZs[b])));
    }

    // Unturned coarse see-through wire grating: 5 horizontal wire rods
    float wireYs[] = { 0.25f, 0.70f, 1.15f, 1.60f, 2.05f };
    for (float wy : wireYs) {
        CityBuilder::appendBox(fenceVerts, fenceIdxs, 5.70f, 0.025f, 0.025f, wireCol, glm::translate(glm::mat4(1.0f), glm::vec3(-15.65f, wy, -2.50f)));
    }

    // Vertical wire rods forming open rectangular cells
    for (float vx = -18.35f; vx <= -12.95f; vx += 0.30f) {
        CityBuilder::appendBox(fenceVerts, fenceIdxs, 0.025f, 1.90f, 0.025f, wireCol, glm::translate(glm::mat4(1.0f), glm::vec3(vx, 1.15f, -2.50f)));
    }
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(fenceVerts, fenceIdxs), glm::mat4(1.0f), false));

    // Industrial Dumpsters in restricted yard
    addBox(1.6f, 1.2f, 2.4f, glm::vec3(0.14f, 0.24f, 0.16f), glm::vec3(-16.8f, 0.65f, 1.5f));
    addBox(1.65f, 0.12f, 1.22f, glm::vec3(0.09f, 0.09f, 0.11f), glm::vec3(-16.8f, 1.30f, 0.9f));
    addBox(1.65f, 0.12f, 1.22f, glm::vec3(0.09f, 0.09f, 0.11f), glm::vec3(-16.8f, 1.30f, 2.1f));

    addBox(1.6f, 1.2f, 2.4f, glm::vec3(0.12f, 0.16f, 0.26f), glm::vec3(-16.8f, 0.65f, 5.5f));
    addBox(1.65f, 0.12f, 1.22f, glm::vec3(0.09f, 0.09f, 0.11f), glm::vec3(-16.8f, 1.30f, 4.9f));
    addBox(1.65f, 0.12f, 1.22f, glm::vec3(0.09f, 0.09f, 0.11f), glm::vec3(-16.8f, 1.30f, 6.1f));

    // Cardboard boxes in restricted yard
    glm::vec3 boxCol1(0.48f, 0.38f, 0.24f), boxCol2(0.42f, 0.32f, 0.20f);
    addBox(0.9f, 0.8f, 0.9f, boxCol1, glm::vec3(-14.5f, 0.40f, 6.2f));
    addBox(0.8f, 0.7f, 0.8f, boxCol2, glm::vec3(-14.5f, 1.15f, 6.2f));
    addBox(0.6f, 0.5f, 0.6f, boxCol1, glm::vec3(-14.5f, 1.75f, 6.2f));
    addBox(0.7f, 0.6f, 0.7f, boxCol2, glm::vec3(-14.5f, 0.30f, 4.8f));
    addBox(0.8f, 0.7f, 0.8f, boxCol1, glm::vec3(-17.2f, 0.35f, -1.0f));
    addBox(0.6f, 0.5f, 0.6f, boxCol2, glm::vec3(-17.2f, 0.95f, -1.0f));

    // Trash bags
    addBag(-17.2f, 0.0f, 3.2f, 1.1f, 25.0f);
    addBag(-16.8f, 0.0f, 3.6f, 1.0f, 85.0f);
    addBag(-17.0f, 0.35f, 3.4f, 0.9f, 150.0f);
    addBag(-14.8f, 0.0f, 7.2f, 1.0f, 40.0f);
    addBag(-15.3f, 0.0f, 7.4f, 1.05f, 110.0f);

    // Glowing rear service exit door on back wall of Konbini (shoved out of wall to avoid clipping)
    addBox(0.08f, 2.35f, 1.25f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(-12.95f, 1.20f, 3.5f));
    addBox(0.06f, 2.25f, 1.12f, glm::vec3(0.18f, 0.20f, 0.22f), glm::vec3(-12.96f, 1.20f, 3.5f));
    addBox(0.06f, 0.65f, 0.65f, glm::vec3(1.0f, 0.88f, 0.55f), glm::vec3(-12.98f, 1.70f, 3.5f), true);
    addBox(0.08f, 0.20f, 0.50f, glm::vec3(0.12f, 0.95f, 0.35f), glm::vec3(-12.98f, 2.45f, 3.5f), true);
    addBox(0.24f, 0.08f, 0.24f, glm::vec3(0.35f, 0.35f, 0.38f), glm::vec3(-13.04f, 2.80f, 3.5f));
    addBox(0.16f, 0.03f, 0.16f, glm::vec3(1.0f, 0.85f, 0.55f), glm::vec3(-13.04f, 2.75f, 3.5f), true);
    pointLights.push_back({ glm::vec3(-13.4f, 2.6f, 3.5f), glm::vec3(1.0f, 0.85f, 0.55f) * 0.04f,
                            glm::vec3(1.0f, 0.85f, 0.55f) * 1.5f, glm::vec3(1.0f, 0.85f, 0.55f) * 1.2f,
                            1.0f, 0.20f, 0.10f });
}
