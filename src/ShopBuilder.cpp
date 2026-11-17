#include "ShopBuilder.h"
#include "CityBuilder.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static void appendBox(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                      float width, float height, float depth, const glm::vec3& col, const glm::mat4& tr) {
    float w = width * 0.5f, h = height * 0.5f, d = depth * 0.5f;
    glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(tr)));
    auto addFace = [&](const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n) {
        unsigned int b = static_cast<unsigned int>(verts.size());
        glm::vec3 tn = glm::normalize(normMat * n);
        verts.push_back({ glm::vec3(tr * glm::vec4(p0, 1.0f)), tn, col, {0.0f, 0.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p1, 1.0f)), tn, col, {1.0f, 0.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p2, 1.0f)), tn, col, {1.0f, 1.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p3, 1.0f)), tn, col, {0.0f, 1.0f} });
        idxs.push_back(b); idxs.push_back(b + 1); idxs.push_back(b + 2);
        idxs.push_back(b); idxs.push_back(b + 2); idxs.push_back(b + 3);
    };
    addFace({-w, -h,  d}, { w, -h,  d}, { w,  h,  d}, {-w,  h,  d}, { 0.0f,  0.0f,  1.0f});
    addFace({ w, -h, -d}, {-w, -h, -d}, {-w,  h, -d}, { w,  h, -d}, { 0.0f,  0.0f, -1.0f});
    addFace({-w,  h,  d}, { w,  h,  d}, { w,  h, -d}, {-w,  h, -d}, { 0.0f,  1.0f,  0.0f});
    addFace({-w, -h, -d}, { w, -h, -d}, { w, -h,  d}, {-w, -h,  d}, { 0.0f, -1.0f,  0.0f});
    addFace({ w, -h,  d}, { w, -h, -d}, { w,  h, -d}, { w,  h,  d}, { 1.0f,  0.0f,  0.0f});
    addFace({-w, -h, -d}, {-w, -h,  d}, {-w,  h,  d}, {-w,  h, -d}, {-1.0f,  0.0f,  0.0f});
}

static void appendCyl(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                      float radius, float height, int segs, const glm::vec3& col, const glm::mat4& tr) {
    float hh = height * 0.5f, step = 6.2831853f / segs;
    glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(tr)));
    unsigned int startIdx = static_cast<unsigned int>(verts.size());
    for (int i = 0; i <= segs; ++i) {
        float a = i * step, c = std::cos(a), s = std::sin(a);
        glm::vec3 tn = glm::normalize(normMat * glm::vec3(c, 0.0f, s));
        verts.push_back({ glm::vec3(tr * glm::vec4(c * radius, -hh, s * radius, 1.0f)), tn, col, {float(i)/segs, 0.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(c * radius,  hh, s * radius, 1.0f)), tn, col, {float(i)/segs, 1.0f} });
    }
    for (int i = 0; i < segs; ++i) {
        unsigned int b0 = startIdx + 2 * i, t0 = startIdx + 2 * i + 1;
        unsigned int b1 = startIdx + 2 * (i + 1), t1 = startIdx + 2 * (i + 1) + 1;
        idxs.push_back(b0); idxs.push_back(b1); idxs.push_back(t1);
        idxs.push_back(b0); idxs.push_back(t1); idxs.push_back(t0);
    }
    unsigned int topC = static_cast<unsigned int>(verts.size());
    verts.push_back({ glm::vec3(tr * glm::vec4(0.0f, hh, 0.0f, 1.0f)), glm::normalize(normMat * glm::vec3(0, 1, 0)), col, {0.5f, 0.5f} });
    unsigned int topR = static_cast<unsigned int>(verts.size());
    for (int i = 0; i <= segs; ++i) {
        float a = i * step;
        verts.push_back({ glm::vec3(tr * glm::vec4(std::cos(a) * radius, hh, std::sin(a) * radius, 1.0f)), glm::normalize(normMat * glm::vec3(0, 1, 0)), col, {0.5f, 0.5f} });
    }
    for (int i = 0; i < segs; ++i) {
        idxs.push_back(topC); idxs.push_back(topR + i); idxs.push_back(topR + i + 1);
    }
}

void ShopBuilder::buildRamenShop(std::vector<SceneNode>& rootNodes,
                                std::vector<PointLight>& pointLights) {
    std::vector<Vertex> rVerts;
    std::vector<unsigned int> rIdxs;

    auto addRamenBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos) {
        appendBox(rVerts, rIdxs, w, h, d, col, glm::translate(glm::mat4(1.0f), pos));
    };

    auto addRamenCyl = [&](float r, float h, int segs, const glm::vec3& col, const glm::vec3& pos) {
        appendCyl(rVerts, rIdxs, r, h, segs, col, glm::translate(glm::mat4(1.0f), pos));
    };

    glm::vec3 wallCol(0.22f, 0.17f, 0.16f);
    glm::vec3 woodCol(0.38f, 0.22f, 0.12f);
    glm::vec3 stoolCol(0.12f, 0.12f, 0.14f);

    // North & South building flanks
    addRamenBox(7.5f, 6.5f, 3.0f, wallCol, glm::vec3(10.25f, 3.25f, -1.0f));
    addRamenBox(7.5f, 6.5f, 3.0f, wallCol, glm::vec3(10.25f, 3.25f,  7.0f));
    // Header lintel over recessed nook
    addRamenBox(7.5f, 3.8f, 5.0f, wallCol, glm::vec3(10.25f, 4.60f,  3.0f));
    // Rear recessed alcove back wall
    addRamenBox(5.0f, 2.7f, 5.0f, wallCol, glm::vec3(10.70f, 1.35f,  3.0f));

    // Wooden timber floor decking in alcove
    addRamenBox(1.7f, 0.08f, 5.0f, glm::vec3(0.28f, 0.18f, 0.11f), glm::vec3(7.35f, 0.14f, 3.0f));

    // Wooden dining counter
    addRamenBox(0.55f, 0.95f, 3.6f, woodCol, glm::vec3(7.85f, 0.55f, 3.0f));
    addRamenBox(0.65f, 0.06f, 3.8f, glm::vec3(0.48f, 0.28f, 0.14f), glm::vec3(7.85f, 1.05f, 3.0f));

    // 3 dining stools
    for (float sz : { 1.8f, 3.0f, 4.2f }) {
        addRamenCyl(0.035f, 0.65f, 8, stoolCol, glm::vec3(7.15f, 0.42f, sz));
        addRamenCyl(0.20f, 0.06f, 12, glm::vec3(0.45f, 0.25f, 0.12f), glm::vec3(7.15f, 0.74f, sz));
    }

    // Ceramic ramen bowls with noodles & chopsticks on the counter
    for (float bz : { 1.8f, 3.0f, 4.2f }) {
        addRamenCyl(0.14f, 0.10f, 10, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(7.75f, 1.13f, bz));
        addRamenCyl(0.12f, 0.03f, 8, glm::vec3(0.92f, 0.78f, 0.35f), glm::vec3(7.75f, 1.17f, bz));
        addRamenBox(0.28f, 0.015f, 0.015f, glm::vec3(0.85f, 0.65f, 0.35f), glm::vec3(7.75f, 1.20f, bz));
    }

    // Indigo Noren fabric curtain panels hanging from the nook lintel
    glm::vec3 norenIndigo(0.10f, 0.14f, 0.28f);
    glm::vec3 norenCrest(0.90f, 0.88f, 0.78f);
    float norenX = 6.55f, norenY = 2.40f;
    for (int p = 0; p < 4; ++p) {
        float pz = 1.0f + p * 1.0f;
        addRamenBox(0.03f, 0.55f, 0.90f, norenIndigo, glm::vec3(norenX, norenY, pz));
        addRamenBox(0.04f, 0.22f, 0.24f, norenCrest, glm::vec3(norenX - 0.005f, norenY - 0.05f, pz));
    }
    addRamenBox(0.04f, 0.04f, 4.2f, glm::vec3(0.35f, 0.22f, 0.12f), glm::vec3(norenX, 2.70f, 2.5f));

    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(rVerts, rIdxs), glm::mat4(1.0f), false));

    // Exterior low pitched red awning above the nook
    auto ramenAwning = CityBuilder::createBox(0.85f, 0.12f, 4.8f, glm::vec3(0.28f, 0.08f, 0.08f));
    glm::mat4 awningTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.25f, 2.80f, 2.5f))
                       * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rootNodes.push_back(SceneNode(ramenAwning, awningTr));

    // 3D Chochin Paper Lantern mounted firmly against the south pillar wall
    auto lanternMesh = Model::loadOBJ("assets/models/chochin_lantern.obj", glm::vec3(0.95f, 0.18f, 0.10f));
    if (lanternMesh) {
        glm::mat4 lanternTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.48f, 2.30f, 5.65f))
                            * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(lanternMesh, lanternTr, true));
    }

    // 3D Katakana neon sign mounted above the awning
    auto ramenNeonMesh = Model::loadOBJ("assets/models/neon_sign_ramen.obj", glm::vec3(1.0f, 0.22f, 0.70f));
    if (ramenNeonMesh) {
        glm::mat4 ramenTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.42f, 3.75f, 2.5f))
                          * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(ramenNeonMesh, ramenTr, true));
    }

    // Warm atmospheric interior point light inside dining nook
    pointLights.push_back({ glm::vec3(7.2f, 2.1f, 3.0f), glm::vec3(1.0f, 0.72f, 0.35f) * 0.06f,
                            glm::vec3(1.0f, 0.72f, 0.35f) * 2.2f, glm::vec3(1.0f, 0.72f, 0.35f) * 1.8f,
                            1.0f, 0.16f, 0.08f });
}

void ShopBuilder::buildBarFacade(std::vector<SceneNode>& rootNodes,
                                std::vector<PointLight>& pointLights) {
    auto addBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive));
    };

    float bX = -6.45f, bZ = -22.0f;
    // Main building volume
    addBox(7.5f, 9.5f, 16.0f, glm::vec3(0.16f, 0.17f, 0.20f), glm::vec3(-10.25f, 4.75f, bZ));

    // Quilted entrance door frame
    addBox(0.10f, 2.45f, 1.55f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(bX, 1.25f, bZ));
    // Mahogany door slab
    addBox(0.06f, 2.35f, 1.40f, glm::vec3(0.20f, 0.11f, 0.09f), glm::vec3(bX + 0.01f, 1.25f, bZ));
    // Polished brass kickplate
    addBox(0.07f, 0.32f, 1.30f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(bX + 0.05f, 0.26f, bZ));
    // Vertical brass door pull handle
    auto handleMesh = CityBuilder::createBox(0.03f, 0.65f, 0.03f, glm::vec3(0.88f, 0.74f, 0.28f));
    rootNodes.push_back(SceneNode(handleMesh, glm::translate(glm::mat4(1.0f), glm::vec3(bX + 0.05f, 1.25f, bZ - 0.45f))));

    // Warm illuminated glass transom window above the door
    addBox(0.06f, 0.45f, 1.40f, glm::vec3(0.95f, 0.75f, 0.30f), glm::vec3(bX + 0.01f, 2.68f, bZ), true);

    // Sleek vertical cyan neon accent strips framing the portal
    auto stripMesh = CityBuilder::createBox(0.04f, 2.95f, 0.05f, glm::vec3(0.0f, 0.95f, 1.0f));
    rootNodes.push_back(SceneNode(stripMesh, glm::translate(glm::mat4(1.0f), glm::vec3(bX + 0.03f, 1.50f, bZ - 0.78f)), true, false, nullptr, true));
    rootNodes.push_back(SceneNode(stripMesh, glm::translate(glm::mat4(1.0f), glm::vec3(bX + 0.03f, 1.50f, bZ + 0.78f)), true, false, nullptr, true));

    // 3D Neon "BAR" sign overhead
    auto barNeonMesh = Model::loadOBJ("assets/models/neon_sign_bar.obj", glm::vec3(0.0f, 0.95f, 1.0f));
    if (barNeonMesh) {
        glm::mat4 barTr = glm::translate(glm::mat4(1.0f), glm::vec3(bX + 0.03f, 3.55f, bZ))
                        * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(barNeonMesh, barTr, true, false, nullptr, true));
    }

    pointLights.push_back({ glm::vec3(-5.5f, 3.4f, bZ), glm::vec3(0.0f, 0.95f, 1.0f) * 0.06f,
                            glm::vec3(0.0f, 0.95f, 1.0f) * 2.2f, glm::vec3(0.0f, 0.95f, 1.0f) * 2.0f,
                            1.0f, 0.18f, 0.09f });
}

void ShopBuilder::buildDetailedVendingMachine(std::vector<SceneNode>& rootNodes,
                                            const glm::vec3& pos,
                                            float rotYDeg,
                                            const glm::vec3& bodyCol,
                                            bool addSpotlight,
                                            std::vector<PointLight>* pointLights) {
    glm::mat4 baseTr = glm::translate(glm::mat4(1.0f), pos)
                     * glm::rotate(glm::mat4(1.0f), glm::radians(rotYDeg), glm::vec3(0.0f, 1.0f, 0.0f));

    std::vector<Vertex> staticVerts, emissiveVerts;
    std::vector<unsigned int> staticIdxs, emissiveIdxs;

    auto addPart = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& lPos, bool emissive = false) {
        glm::mat4 tr = baseTr * glm::translate(glm::mat4(1.0f), lPos);
        if (emissive) {
            appendBox(emissiveVerts, emissiveIdxs, w, h, d, col, tr);
        } else {
            appendBox(staticVerts, staticIdxs, w, h, d, col, tr);
        }
    };

    auto addCyl = [&](float r, float h, int segs, const glm::vec3& col, const glm::vec3& lPos) {
        glm::mat4 tr = baseTr * glm::translate(glm::mat4(1.0f), lPos);
        appendCyl(staticVerts, staticIdxs, r, h, segs, col, tr);
    };

    // Main machine housing
    addPart(0.92f, 2.05f, 0.78f, bodyCol, glm::vec3(0.0f, 1.025f, 0.0f));

    // Recessed illuminated display window with softened milk-acrylic glow
    addPart(0.78f, 0.95f, 0.04f, glm::vec3(0.55f, 0.60f, 0.66f), glm::vec3(0.0f, 1.40f, 0.38f), true);

    // Shelf dividing ledge widened to 0.86m so ends overlap into the housing frame
    addPart(0.86f, 0.03f, 0.06f, glm::vec3(0.20f, 0.20f, 0.22f), glm::vec3(0.0f, 1.38f, 0.40f));

    // Miniature beverage cans on upper and lower shelves
    glm::vec3 drinkCols[6] = {
        {0.20f, 0.75f, 0.30f}, {0.88f, 0.15f, 0.15f}, {0.15f, 0.55f, 0.95f},
        {0.95f, 0.65f, 0.15f}, {0.12f, 0.12f, 0.14f}, {0.90f, 0.85f, 0.70f}
    };
    for (int i = 0; i < 4; ++i) {
        float cx = -0.27f + i * 0.18f;
        addCyl(0.032f, 0.11f, 8, drinkCols[i % 6], glm::vec3(cx, 1.62f, 0.39f));
        addCyl(0.032f, 0.11f, 8, drinkCols[(i + 3) % 6], glm::vec3(cx, 1.20f, 0.39f));

        glm::vec3 btnCol1 = (i == 1) ? glm::vec3(0.65f, 0.15f, 0.15f) : glm::vec3(0.15f, 0.65f, 0.25f);
        glm::vec3 btnCol2 = (i == 3) ? glm::vec3(0.65f, 0.15f, 0.15f) : glm::vec3(0.15f, 0.65f, 0.25f);
        addPart(0.05f, 0.025f, 0.02f, btnCol1, glm::vec3(cx, 1.48f, 0.40f), true);
        addPart(0.05f, 0.025f, 0.02f, btnCol2, glm::vec3(cx, 1.06f, 0.40f), true);
    }

    // Payment interface panel
    addPart(0.18f, 0.28f, 0.03f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(0.26f, 0.75f, 0.39f));
    addPart(0.08f, 0.015f, 0.01f, glm::vec3(0.85f), glm::vec3(0.26f, 0.82f, 0.41f));
    addPart(0.10f, 0.025f, 0.01f, glm::vec3(0.15f, 0.65f, 0.35f), glm::vec3(0.26f, 0.72f, 0.41f), true);

    // Retrieval drop box push flap
    addPart(0.65f, 0.32f, 0.05f, glm::vec3(0.14f, 0.14f, 0.16f), glm::vec3(0.0f, 0.32f, 0.38f));
    addPart(0.55f, 0.22f, 0.02f, glm::vec3(0.25f, 0.25f, 0.28f), glm::vec3(0.0f, 0.32f, 0.40f));

    if (!staticVerts.empty())
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(staticVerts, staticIdxs), glm::mat4(1.0f), false));
    if (!emissiveVerts.empty())
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(emissiveVerts, emissiveIdxs), glm::mat4(1.0f), true));

    if (addSpotlight && pointLights) {
        glm::vec3 glowPos = pos + glm::vec3(baseTr * glm::vec4(0.0f, 1.4f, 0.6f, 0.0f));
        pointLights->push_back({ glowPos, glm::vec3(0.85f, 0.95f, 1.0f) * 0.04f,
                                 glm::vec3(0.85f, 0.95f, 1.0f) * 1.6f, glm::vec3(0.85f, 0.95f, 1.0f) * 1.4f,
                                 1.0f, 0.18f, 0.09f });
    }
}
