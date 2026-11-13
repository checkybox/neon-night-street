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

    // =========================================================================
    // ALLEY 1: EAST SIDE SERVICE ALLEY (Z in [-46.0, -43.5])
    // Between Cyber Capsule Hotel and Pencil Tower
    // =========================================================================
    // Brick back wall sealing alley
    addBoxNode(0.4f, 16.0f, 2.6f, glm::vec3(0.18f, 0.17f, 0.19f), glm::vec3(13.8f, 8.0f, -44.75f));
    // Alley asphalt ground
    rootNodes.push_back(SceneNode(CityBuilder::createPlane(7.0f, 2.5f, glm::vec3(0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.005f, -44.75f)), false, true));

    // Fire escape platforms mounted on Hotel side wall (Z = -46.0), projecting +Z into alley
    // Rotated -90 deg around Y so local +X points to global +Z
    if (fireEscapeMesh) {
        for (float y : { 3.4f, 6.8f, 10.2f }) {
            glm::mat4 feTr = glm::translate(glm::mat4(1.0f), glm::vec3(9.2f, y, -45.95f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(fireEscapeMesh, feTr));
        }
    }

    // Piles of black trash bags with twisted tops
    addBag(8.2f, 0.0f, -45.4f, 1.0f, 15.0f);
    addBag(8.8f, 0.0f, -45.2f, 0.95f, 75.0f);
    addBag(9.3f, 0.0f, -45.3f, 1.05f, 140.0f);
    addBag(8.5f, 0.35f, -45.3f, 0.85f, 210.0f); // stacked on top

    // Cardboard boxes & wooden pallet
    addBoxNode(1.1f, 0.12f, 1.1f, glm::vec3(0.32f, 0.22f, 0.14f), glm::vec3(11.2f, 0.06f, -44.5f));
    addBoxNode(0.7f, 0.65f, 0.7f, glm::vec3(0.42f, 0.32f, 0.20f), glm::vec3(11.2f, 0.44f, -44.5f));
    addBoxNode(0.55f, 0.5f, 0.55f, glm::vec3(0.38f, 0.28f, 0.18f), glm::vec3(11.2f, 1.02f, -44.5f));
    addBoxNode(0.6f, 0.5f, 0.8f, glm::vec3(0.35f, 0.26f, 0.16f), glm::vec3(12.2f, 0.25f, -44.6f));

    // Alley bulkhead wall lamp
    addBoxNode(0.15f, 0.2f, 0.15f, glm::vec3(1.0f, 0.8f, 0.4f), glm::vec3(9.2f, 2.6f, -45.85f), true);
    addPoint(glm::vec3(9.2f, 2.5f, -44.75f), glm::vec3(1.0f, 0.75f, 0.35f), 0.03f, 1.1f, 0.9f, 0.22f, 0.12f);

    // =========================================================================
    // ALLEY 2: WEST SIDE SERVICE ALLEY (Z in [-46.0, -43.5])
    // Between Izakaya Bar and Apartment Block
    // =========================================================================
    addBoxNode(0.4f, 13.5f, 2.6f, glm::vec3(0.18f, 0.17f, 0.19f), glm::vec3(-13.8f, 6.75f, -44.75f));
    rootNodes.push_back(SceneNode(CityBuilder::createPlane(7.0f, 2.5f, glm::vec3(0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.005f, -44.75f)), false, true));

    // Fire escape on Apartment Block side wall (Z = -43.5), projecting -Z into alley
    // Rotated +90 deg around Y so local +X points to global -Z
    if (fireEscapeMesh) {
        for (float y : { 3.4f, 6.8f, 10.2f }) {
            glm::mat4 feTr = glm::translate(glm::mat4(1.0f), glm::vec3(-9.2f, y, -43.55f))
                           * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            rootNodes.push_back(SceneNode(fireEscapeMesh, feTr));
        }
    }

    // Industrial dumpster
    addBoxNode(1.6f, 1.2f, 2.0f, glm::vec3(0.15f, 0.22f, 0.18f), glm::vec3(-11.5f, 0.6f, -44.75f));
    addBoxNode(1.65f, 0.1f, 1.05f, glm::vec3(0.10f, 0.10f, 0.12f), glm::vec3(-11.5f, 1.25f, -45.25f));
    addBoxNode(1.65f, 0.1f, 1.05f, glm::vec3(0.10f, 0.10f, 0.12f), glm::vec3(-11.5f, 1.25f, -44.25f));

    // Trash bags piled near dumpster
    addBag(-8.8f, 0.0f, -45.3f, 1.0f, 40.0f);
    addBag(-9.4f, 0.0f, -45.2f, 1.05f, 110.0f);
    addBag(-9.1f, 0.0f, -44.4f, 0.95f, 190.0f);
    addBag(-9.1f, 0.38f, -45.2f, 0.85f, 320.0f);

    // Cardboard boxes
    addBoxNode(0.8f, 0.6f, 0.6f, glm::vec3(0.38f, 0.28f, 0.18f), glm::vec3(-8.2f, 0.3f, -45.2f));
    addBoxNode(0.6f, 0.45f, 0.5f, glm::vec3(0.44f, 0.34f, 0.22f), glm::vec3(-8.2f, 0.82f, -45.2f));

    // Alley light
    addBoxNode(0.15f, 0.2f, 0.15f, glm::vec3(0.9f, 0.95f, 1.0f), glm::vec3(-9.2f, 2.8f, -43.65f), true);
    addPoint(glm::vec3(-9.2f, 2.7f, -44.75f), glm::vec3(0.7f, 0.85f, 1.0f), 0.03f, 1.1f, 0.9f, 0.22f, 0.12f);

    // =========================================================================
    // ALLEY 3: EAST SIDE RAMEN/ARCADE ALLEY (Z in [8.5, 11.2])
    // =========================================================================
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

    // =========================================================================
    // BOOKSTORE (MANGA & BOOKS CORNER) STOREFRONT DISPLAY (Z in [24.5, 38.0])
    // =========================================================================
    // Storefront display showcase window (warm illuminated backdrop)
    addBoxNode(0.08f, 2.1f, 3.8f, glm::vec3(0.95f, 0.88f, 0.65f), glm::vec3(6.45f, 1.35f, 33.0f), true);
    // Dark window framing grid
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 31.1f));
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 33.0f));
    addBoxNode(0.10f, 2.2f, 0.12f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 34.9f));
    addBoxNode(0.10f, 0.12f, 3.9f,  glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(6.43f, 1.35f, 33.0f));

    // =========================================================================
    // GLOWING 3D NEON LETTERS: "B O O K S" (WITH PROPER DIAGONAL 'K'!)
    // =========================================================================
    glm::vec3 bookNeonGold(1.0f, 0.88f, 0.22f);
    float bX = 6.36f, bY = 3.65f;

    auto addBStroke = [&](float w, float h, float d, const glm::vec3& p) {
        addBoxNode(w, h, d, bookNeonGold, p, true);
    };

    // 'B' at Z = 29.6
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 29.40f));
    addBStroke(0.06f, 0.08f, 0.35f, glm::vec3(bX, bY + 0.38f, 29.55f));
    addBStroke(0.06f, 0.08f, 0.32f, glm::vec3(bX, bY,        29.54f));
    addBStroke(0.06f, 0.08f, 0.35f, glm::vec3(bX, bY - 0.38f, 29.55f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY + 0.20f, 29.72f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY - 0.20f, 29.72f));

    // 'O' at Z = 30.3
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.12f));
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.48f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 30.30f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 30.30f));

    // 'O' at Z = 31.0
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 30.82f));
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 31.18f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 31.00f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 31.00f));

    // 'K' at Z = 31.7: Vertical spine + two distinct diagonal branches opening right (+Z)!
    addBStroke(0.06f, 0.85f, 0.08f, glm::vec3(bX, bY, 31.50f));
    glm::mat4 armUp = glm::translate(glm::mat4(1.0f), glm::vec3(bX, bY + 0.20f, 31.68f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(42.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.06f, 0.48f, 0.08f, bookNeonGold), armUp, true));
    glm::mat4 armDn = glm::translate(glm::mat4(1.0f), glm::vec3(bX, bY - 0.20f, 31.68f))
                    * glm::rotate(glm::mat4(1.0f), glm::radians(-42.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.06f, 0.48f, 0.08f, bookNeonGold), armDn, true));

    // 'S' at Z = 32.4
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY + 0.38f, 32.40f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY,        32.40f));
    addBStroke(0.06f, 0.08f, 0.36f, glm::vec3(bX, bY - 0.38f, 32.40f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY + 0.20f, 32.24f));
    addBStroke(0.06f, 0.40f, 0.08f, glm::vec3(bX, bY - 0.20f, 32.56f));
}
