#include "StreetProps.h"
#include "CityBuilder.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void StreetProps::buildConstructionBarriers(std::vector<SceneNode>& rootNodes,
                                           std::vector<PointLight>& pointLights) {
    std::vector<Vertex> barVerts;
    std::vector<unsigned int> barIdxs;

    auto addBBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        CityBuilder::appendBox(barVerts, barIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };

    glm::vec3 concCol(0.28f, 0.28f, 0.30f);
    glm::vec3 hazYellow(0.95f, 0.78f, 0.08f);
    glm::vec3 hazBlack(0.08f, 0.08f, 0.09f);
    glm::vec3 barrelOrange(0.92f, 0.40f, 0.08f);
    glm::vec3 barrelWhite(0.90f, 0.90f, 0.92f);
    glm::vec3 amberBeaconCol(1.0f, 0.65f, 0.08f);

    float barrierZs[] = { -74.0f, 44.0f };

    for (float bz : barrierZs) {
        float dirZ = (bz < 0.0f) ? 1.0f : -1.0f;

        // 4 modular concrete Jersey highway barriers across the 8m carriageway
        for (float bx = -3.15f; bx <= 3.15f; bx += 2.10f) {
            // Wide sloped concrete base
            addBBox(2.00f, 0.30f, 0.65f, concCol, glm::vec3(bx, 0.15f, bz));
            // Tapered middle transition
            addBBox(1.98f, 0.35f, 0.45f, concCol, glm::vec3(bx, 0.45f, bz));
            // Upper vertical concrete stem
            addBBox(1.96f, 0.55f, 0.26f, concCol, glm::vec3(bx, 0.85f, bz));

            // Reflective diagonal hazard warning panels
            for (int s = 0; s < 6; ++s) {
                glm::vec3 sCol = (s % 2 == 0) ? hazYellow : hazBlack;
                addBBox(0.28f, 0.38f, 0.02f, sCol, glm::vec3(bx - 0.70f + s * 0.28f, 0.85f, bz + dirZ * 0.14f));
            }

            // Top lifting pin holes
            addBBox(0.08f, 0.08f, 0.28f, hazBlack, glm::vec3(bx - 0.65f, 1.05f, bz));
            addBBox(0.08f, 0.08f, 0.28f, hazBlack, glm::vec3(bx + 0.65f, 1.05f, bz));
        }

        // Heavy steel road closure crossbar mounted above the Jersey barriers
        for (float sx : { -3.15f, 3.15f }) {
            addBBox(0.08f, 0.28f, 0.08f, glm::vec3(0.50f, 0.50f, 0.54f), glm::vec3(sx, 1.16f, bz - dirZ * 0.02f));
        }
        addBBox(8.40f, 0.12f, 0.08f, glm::vec3(0.85f, 0.85f, 0.88f), glm::vec3(0.0f, 1.30f, bz));
        for (int s = 0; s < 16; ++s) {
            glm::vec3 sCol = (s % 2 == 0) ? hazYellow : hazBlack;
            addBBox(0.50f, 0.10f, 0.02f, sCol, glm::vec3(-3.75f + s * 0.50f, 1.30f, bz + dirZ * 0.055f));
        }

        // Warning sign plaque: "ROAD CLOSED"
        addBBox(1.80f, 0.65f, 0.04f, barrelWhite, glm::vec3(0.0f, 1.75f, bz));
        addBBox(1.74f, 0.59f, 0.02f, hazBlack, glm::vec3(0.0f, 1.75f, bz + dirZ * 0.025f));
        addBBox(1.60f, 0.18f, 0.02f, barrelOrange, glm::vec3(0.0f, 1.82f, bz + dirZ * 0.035f));
        addBBox(1.60f, 0.18f, 0.02f, barrelWhite, glm::vec3(0.0f, 1.62f, bz + dirZ * 0.035f));

        // Two high-visibility fluted traffic barrels with weighted rubber bases
        for (float tx : { -4.1f, 4.1f }) {
            // Hexagonal weighted black rubber base
            addBBox(0.65f, 0.10f, 0.65f, hazBlack, glm::vec3(tx, 0.05f, bz - dirZ * 0.35f));
            // Stacked orange and reflective white barrel segments
            addBBox(0.52f, 0.25f, 0.52f, barrelOrange, glm::vec3(tx, 0.22f, bz - dirZ * 0.35f));
            addBBox(0.50f, 0.22f, 0.50f, barrelWhite,  glm::vec3(tx, 0.44f, bz - dirZ * 0.35f));
            addBBox(0.48f, 0.25f, 0.48f, barrelOrange, glm::vec3(tx, 0.66f, bz - dirZ * 0.35f));
            addBBox(0.46f, 0.22f, 0.46f, barrelWhite,  glm::vec3(tx, 0.88f, bz - dirZ * 0.35f));
            addBBox(0.44f, 0.18f, 0.44f, barrelOrange, glm::vec3(tx, 1.06f, bz - dirZ * 0.35f));
            // Top carry handle
            addBBox(0.08f, 0.14f, 0.22f, hazBlack, glm::vec3(tx, 1.20f, bz - dirZ * 0.35f));
        }

        // Two amber construction hazard flashers mounted cleanly atop the crossbar
        for (float fx : { -2.1f, 2.1f }) {
            addBBox(0.08f, 0.08f, 0.08f, hazBlack, glm::vec3(fx, 1.40f, bz));
            addBBox(0.12f, 0.10f, 0.12f, hazYellow, glm::vec3(fx, 1.49f, bz));
            // Glowing lens positioned above crossbar to prevent strip flickering
            rootNodes.push_back(SceneNode(CityBuilder::createBox(0.14f, 0.14f, 0.14f, amberBeaconCol),
                                          glm::translate(glm::mat4(1.0f), glm::vec3(fx, 1.61f, bz)), true));
            // Local amber warning point light
            pointLights.push_back({ glm::vec3(fx, 1.65f, bz + dirZ * 0.35f),
                                    amberBeaconCol * 0.04f, amberBeaconCol * 1.4f, amberBeaconCol * 1.2f,
                                    1.0f, 0.22f, 0.12f });
        }
    }

    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(barVerts, barIdxs), glm::mat4(1.0f), false));
}

void StreetProps::buildAlleyClutter(std::vector<SceneNode>& rootNodes) {
    std::vector<Vertex> clutterVerts;
    std::vector<unsigned int> clutterIdxs;

    auto addCBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        CityBuilder::appendBox(clutterVerts, clutterIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };

    // North perimeter wall along crossroad side street: Z = -14.0f, X in [-18.5f, -13.8f]
    glm::vec3 dumpGreen(0.14f, 0.22f, 0.16f);
    glm::vec3 lidBlack(0.08f, 0.08f, 0.09f);
    glm::vec3 crateCol1(0.44f, 0.32f, 0.20f), crateCol2(0.38f, 0.28f, 0.16f);

    // Large industrial commercial dumpster positioned against the new North wall
    float dX = -16.2f, dZ = -12.8f;
    // Dumpster steel tub with sloped front
    addCBox(1.90f, 1.15f, 1.30f, dumpGreen, glm::vec3(dX, 0.70f, dZ));
    // Reinforced upper rim
    addCBox(1.98f, 0.08f, 1.38f, glm::vec3(0.10f, 0.16f, 0.11f), glm::vec3(dX, 1.30f, dZ));
    // Dual sloped plastic lids
    addCBox(0.96f, 0.08f, 1.42f, lidBlack, glm::vec3(dX - 0.49f, 1.36f, dZ));
    addCBox(0.96f, 0.08f, 1.42f, lidBlack, glm::vec3(dX + 0.49f, 1.36f, dZ));
    // Side forklift pockets
    addCBox(0.14f, 0.18f, 1.25f, glm::vec3(0.10f, 0.15f, 0.11f), glm::vec3(dX - 1.02f, 0.75f, dZ));
    addCBox(0.14f, 0.18f, 1.25f, glm::vec3(0.10f, 0.15f, 0.11f), glm::vec3(dX + 1.02f, 0.75f, dZ));
    // 4 heavy steel caster wheel assemblies
    for (float wx : { -0.80f, 0.80f }) {
        for (float wz : { -0.45f, 0.45f }) {
            addCBox(0.12f, 0.12f, 0.12f, lidBlack, glm::vec3(dX + wx, 0.08f, dZ + wz));
        }
    }

    // Stacked sturdy wooden shipping crates next to the dumpster
    float crX = -14.4f, crZ = -13.2f;
    // Base large crate with corner reinforcements
    addCBox(0.95f, 0.85f, 0.95f, crateCol1, glm::vec3(crX, 0.425f, crZ));
    addCBox(0.99f, 0.08f, 0.99f, crateCol2, glm::vec3(crX, 0.08f, crZ));
    addCBox(0.99f, 0.08f, 0.99f, crateCol2, glm::vec3(crX, 0.81f, crZ));
    // Second tier crate
    addCBox(0.80f, 0.70f, 0.80f, crateCol2, glm::vec3(crX + 0.04f, 1.20f, crZ + 0.02f));
    // Third smaller crate
    addCBox(0.60f, 0.50f, 0.60f, crateCol1, glm::vec3(crX - 0.08f, 1.80f, crZ - 0.04f));

    // Cardboard boxes resting against the wall
    addCBox(0.70f, 0.60f, 0.70f, glm::vec3(0.48f, 0.38f, 0.24f), glm::vec3(-17.8f, 0.30f, -13.3f));
    addCBox(0.55f, 0.45f, 0.55f, glm::vec3(0.42f, 0.32f, 0.20f), glm::vec3(-17.8f, 0.80f, -13.3f));

    // Wall-mounted electrical breaker cabinet and conduit pipes on North brick wall
    addCBox(0.65f, 0.90f, 0.22f, glm::vec3(0.22f, 0.23f, 0.25f), glm::vec3(-15.1f, 2.20f, -13.86f));
    addCBox(0.14f, 0.18f, 0.04f, glm::vec3(0.85f, 0.40f, 0.10f), glm::vec3(-15.1f, 2.25f, -13.73f));
    // Vertical galvanized conduit pipe running down to ground
    addCBox(0.05f, 2.80f, 0.05f, glm::vec3(0.35f, 0.36f, 0.38f), glm::vec3(-14.7f, 1.40f, -13.88f));
    addCBox(0.05f, 2.80f, 0.05f, glm::vec3(0.35f, 0.36f, 0.38f), glm::vec3(-15.5f, 1.40f, -13.88f));

    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(clutterVerts, clutterIdxs), glm::mat4(1.0f), false));

    // Piles of discarded black garbage bags
    auto trashBagMesh = Model::loadOBJ("assets/models/trash_bag.obj", glm::vec3(0.08f, 0.08f, 0.09f));
    if (trashBagMesh) {
        auto addBag = [&](float x, float y, float z, float scale, float yaw) {
            glm::mat4 tr = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))
                         * glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f))
                         * glm::scale(glm::mat4(1.0f), glm::vec3(scale));
            rootNodes.push_back(SceneNode(trashBagMesh, tr));
        };
        addBag(-17.2f, 0.0f, -13.3f, 1.05f, 25.0f);
        addBag(-17.5f, 0.0f, -13.1f, 0.95f, 110.0f);
        addBag(-17.3f, 0.35f, -13.2f, 0.88f, 210.0f);
        addBag(-15.0f, 0.0f, -13.4f, 1.00f, 45.0f);
    }
}

void StreetProps::buildBuildingEntrances(std::vector<SceneNode>& rootNodes,
                                       std::vector<PointLight>& pointLights) {
    std::vector<Vertex> entVerts;
    std::vector<unsigned int> entIdxs;

    auto addEBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        CityBuilder::appendBox(entVerts, entIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };
    auto addRotEBox = [&](float w, float h, float d, const glm::vec3& col, const glm::mat4& tr) {
        CityBuilder::appendBox(entVerts, entIdxs, w, h, d, col, tr);
    };

    glm::vec3 frameDark(0.12f, 0.12f, 0.14f);
    glm::vec3 brassCol(0.85f, 0.72f, 0.28f);
    glm::vec3 steelCol(0.75f, 0.78f, 0.82f);
    glm::vec3 warmLobby(1.0f, 0.88f, 0.65f);
    glm::vec3 darkWood(0.24f, 0.14f, 0.10f);

    // 1. PENCIL BUILDING (West side, X = -6.45f, Z = 21.0f)
    float pX = -6.45f, pZ = 21.0f;
    // Entrance frame jambs and lintel
    addEBox(0.12f, 2.50f, 0.08f, frameDark, glm::vec3(pX, 1.25f, pZ - 0.76f));
    addEBox(0.12f, 2.50f, 0.08f, frameDark, glm::vec3(pX, 1.25f, pZ + 0.76f));
    addEBox(0.12f, 0.10f, 1.60f, frameDark, glm::vec3(pX, 2.45f, pZ));
    // Modern glass door slab with warm glowing interior
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.06f, 2.38f, 1.44f, warmLobby * 0.92f),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(pX + 0.02f, 1.20f, pZ)), true));
    // Vertical brushed aluminum pull handle
    addEBox(0.03f, 0.75f, 0.03f, steelCol, glm::vec3(pX + 0.06f, 1.25f, pZ - 0.40f));
    // Backlit electronic access panel
    addEBox(0.03f, 0.16f, 0.09f, frameDark, glm::vec3(pX + 0.05f, 1.30f, pZ + 0.70f));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.04f, 0.03f, 0.03f, glm::vec3(0.0f, 0.95f, 0.95f)),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(pX + 0.06f, 1.32f, pZ + 0.70f)), true));
    // Polished dark granite entrance threshold
    addEBox(0.24f, 0.04f, 1.80f, glm::vec3(0.18f, 0.18f, 0.20f), glm::vec3(pX + 0.08f, 0.02f, pZ));
    // Cantilevered glass canopy with 45-degree steel suspension tie rods
    addEBox(0.65f, 0.05f, 1.85f, glm::vec3(0.15f, 0.15f, 0.18f), glm::vec3(pX + 0.32f, 2.65f, pZ));
    addEBox(0.55f, 0.02f, 1.70f, glm::vec3(0.55f, 0.75f, 0.85f), glm::vec3(pX + 0.32f, 2.67f, pZ));
    for (float zArm : { pZ - 0.75f, pZ + 0.75f }) {
        addEBox(0.04f, 0.08f, 0.08f, steelCol, glm::vec3(pX + 0.02f, 3.23f, zArm));
        addEBox(0.06f, 0.04f, 0.06f, steelCol, glm::vec3(pX + 0.54f, 2.68f, zArm));
        glm::mat4 rodTr = glm::translate(glm::mat4(1.0f), glm::vec3(pX + 0.275f, 2.955f, zArm))
                        * glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        addRotEBox(0.03f, 0.78f, 0.03f, steelCol, rodTr);
    }
    // Recessed canopy downlight
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.25f, 0.02f, 0.80f, warmLobby),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(pX + 0.30f, 2.62f, pZ)), true));
    pointLights.push_back({ glm::vec3(pX + 0.60f, 2.45f, pZ), warmLobby * 0.04f, warmLobby * 1.6f, warmLobby * 1.2f, 1.0f, 0.20f, 0.10f });

    // 2. BOOKSTORE (East side, X = 6.45f, Z = 35.8f)
    float bX = 6.45f, bZ = 35.8f;
    // Dark frame jambs, lintel, and middle transom divider
    addEBox(0.12f, 2.45f, 0.09f, frameDark, glm::vec3(bX, 1.225f, bZ - 0.60f));
    addEBox(0.12f, 2.45f, 0.09f, frameDark, glm::vec3(bX, 1.225f, bZ + 0.60f));
    addEBox(0.12f, 0.08f, 1.11f, frameDark, glm::vec3(bX, 2.08f, bZ));
    addEBox(0.12f, 0.10f, 1.29f, frameDark, glm::vec3(bX, 2.40f, bZ));
    // Recessed dark mahogany wood door slab
    addEBox(0.06f, 2.02f, 1.11f, darkWood, glm::vec3(bX - 0.02f, 1.03f, bZ));
    // Brass handle and kickplate
    addEBox(0.04f, 0.16f, 0.04f, brassCol, glm::vec3(bX - 0.05f, 1.05f, bZ + 0.35f));
    addEBox(0.04f, 0.22f, 1.09f, brassCol, glm::vec3(bX - 0.04f, 0.13f, bZ));
    // Stained glass glowing transom window framed directly above door
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.05f, 0.24f, 1.11f, glm::vec3(1.0f, 0.82f, 0.40f)),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(bX - 0.02f, 2.24f, bZ)), true));
    // Striped green entrance awning sitting directly above transom
    auto bookAwning = CityBuilder::createBox(0.75f, 0.08f, 1.55f, glm::vec3(0.12f, 0.26f, 0.18f));
    glm::mat4 bAwnTr = glm::translate(glm::mat4(1.0f), glm::vec3(bX - 0.32f, 2.48f, bZ))
                     * glm::rotate(glm::mat4(1.0f), glm::radians(12.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rootNodes.push_back(SceneNode(bookAwning, bAwnTr));

    // 3. EAST END CAP BUILDING (East side, X = 6.45f, Z = -66.0f)
    float eX = 6.45f, eZ = -66.0f;
    // Heavy industrial steel door frame jambs and lintel
    addEBox(0.12f, 2.30f, 0.08f, frameDark, glm::vec3(eX, 1.15f, eZ - 0.58f));
    addEBox(0.12f, 2.30f, 0.08f, frameDark, glm::vec3(eX, 1.15f, eZ + 0.58f));
    addEBox(0.12f, 0.10f, 1.24f, frameDark, glm::vec3(eX, 2.25f, eZ));
    // Recessed steel door slab with protective kickplate
    addEBox(0.06f, 2.18f, 1.08f, glm::vec3(0.18f, 0.19f, 0.22f), glm::vec3(eX - 0.02f, 1.11f, eZ));
    addEBox(0.04f, 0.25f, 1.06f, glm::vec3(0.35f, 0.36f, 0.38f), glm::vec3(eX - 0.04f, 0.15f, eZ));
    addEBox(0.04f, 0.12f, 0.04f, steelCol, glm::vec3(eX - 0.05f, 1.10f, eZ + 0.32f));
    // Overhead protective cage sconce fixture
    addEBox(0.16f, 0.10f, 0.16f, frameDark, glm::vec3(eX - 0.10f, 2.45f, eZ));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.10f, 0.04f, 0.10f, glm::vec3(1.0f, 0.85f, 0.50f)),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(eX - 0.10f, 2.40f, eZ)), true));

    // 4. INDUSTRIAL ANNEX (West side, X = -6.45f, Z = -63.5f)
    float aX = -6.45f, aZ = -63.5f;
    addEBox(0.12f, 2.25f, 0.08f, frameDark, glm::vec3(aX, 1.125f, aZ - 0.56f));
    addEBox(0.12f, 2.25f, 0.08f, frameDark, glm::vec3(aX, 1.125f, aZ + 0.56f));
    addEBox(0.12f, 0.10f, 1.20f, frameDark, glm::vec3(aX, 2.20f, aZ));
    addEBox(0.06f, 2.13f, 1.04f, glm::vec3(0.20f, 0.21f, 0.24f), glm::vec3(aX + 0.02f, 1.085f, aZ));
    addEBox(0.03f, 0.14f, 0.03f, steelCol, glm::vec3(aX + 0.05f, 1.10f, aZ - 0.32f));
    addEBox(0.35f, 0.05f, 1.35f, frameDark, glm::vec3(aX + 0.16f, 2.30f, aZ));

    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(entVerts, entIdxs), glm::mat4(1.0f), false));
}

void StreetProps::buildUpperWindows(std::vector<SceneNode>& rootNodes) {
    std::vector<Vertex> winVerts;
    std::vector<unsigned int> winIdxs;
    std::vector<Vertex> litVerts;
    std::vector<unsigned int> litIdxs;

    auto addWBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        CityBuilder::appendBox(winVerts, winIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };

    auto addLitBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        CityBuilder::appendBox(litVerts, litIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };

    glm::vec3 frameDark(0.12f, 0.12f, 0.14f);
    glm::vec3 darkGlass(0.06f, 0.07f, 0.09f);
    glm::vec3 warmAmber(0.95f, 0.78f, 0.40f);
    glm::vec3 softCyan(0.40f, 0.75f, 0.85f);
    glm::vec3 blindAmber(0.70f, 0.55f, 0.30f);

    // Multi-floor window arrays for Pencil Building (X = -6.45f, Z in [18.0f, 24.0f])
    for (int floor = 0; floor < 3; ++floor) {
        float wy = 4.8f + floor * 3.0f;
        for (float wz : { 19.2f, 21.0f, 22.8f }) {
            addWBox(0.08f, 1.60f, 1.10f, frameDark, glm::vec3(-6.45f, wy, wz));
            bool isLit = ((floor + static_cast<int>(wz * 10)) % 3 == 0);
            if (isLit) {
                glm::vec3 litCol = (floor == 1) ? softCyan * 0.85f : warmAmber * 0.88f;
                addLitBox(0.06f, 1.45f, 0.96f, litCol, glm::vec3(-6.43f, wy, wz));
                // Horizontal window blind shadow
                addWBox(0.07f, 0.04f, 0.96f, frameDark, glm::vec3(-6.42f, wy + 0.30f, wz));
            } else {
                addWBox(0.06f, 1.45f, 0.96f, darkGlass, glm::vec3(-6.43f, wy, wz));
            }
        }
    }

    // Upper floor windows for Pharmacy Building (X = -6.45f, Z in [26.0f, 36.5f])
    for (float wz : { 27.2f, 29.8f, 32.5f, 35.2f }) {
        float wy = 5.6f;
        addWBox(0.08f, 1.50f, 1.20f, frameDark, glm::vec3(-6.45f, wy, wz));
        bool isLit = (wz == 29.8f || wz == 35.2f);
        if (isLit) {
            addLitBox(0.06f, 1.36f, 1.06f, blindAmber, glm::vec3(-6.43f, wy, wz));
        } else {
            addWBox(0.06f, 1.36f, 1.06f, darkGlass, glm::vec3(-6.43f, wy, wz));
        }
    }

    // Upper floor windows for East Commercial Towers (X = 6.45f, Z in [-68.0f, -60.0f])
    float wy = 5.2f;
    for (float wz : { -68.5f, -66.0f, -63.5f }) {
        addWBox(0.08f, 1.50f, 1.20f, frameDark, glm::vec3(6.45f, wy, wz));
        bool isLit = (wz == -66.0f);
        if (isLit) {
            addLitBox(0.06f, 1.36f, 1.06f, warmAmber * 0.85f, glm::vec3(6.43f, wy, wz));
        } else {
            addWBox(0.06f, 1.36f, 1.06f, darkGlass, glm::vec3(6.43f, wy, wz));
        }
    }

    if (!winVerts.empty()) {
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(winVerts, winIdxs), glm::mat4(1.0f), false));
    }
    if (!litVerts.empty()) {
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(litVerts, litIdxs), glm::mat4(1.0f), true));
    }
}
