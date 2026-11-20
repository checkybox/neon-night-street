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
    glm::vec3 darkOak(0.25f, 0.14f, 0.08f);
    glm::vec3 stoolCol(0.12f, 0.12f, 0.14f);
    glm::vec3 steelCol(0.72f, 0.74f, 0.76f);
    glm::vec3 tileCol(0.16f, 0.18f, 0.20f);

    // North & South building flanks (widened carving on the left side)
    addRamenBox(7.5f, 6.5f, 3.0f, wallCol, glm::vec3(10.25f, 3.25f, -2.5f));
    addRamenBox(7.5f, 6.5f, 3.0f, wallCol, glm::vec3(10.25f, 3.25f,  7.0f));
    // Header lintel over wide dining nook & service corridor opening
    addRamenBox(7.5f, 3.8f, 6.5f, wallCol, glm::vec3(10.25f, 4.60f,  2.25f));

    // Wooden timber floor decking covering the entire ramen shop interior
    glm::vec3 woodDeck(0.28f, 0.18f, 0.11f);
    addRamenBox(3.40f, 0.08f, 4.30f, woodDeck, glm::vec3(8.15f, 0.14f, 3.35f));
    addRamenBox(8.10f, 0.08f, 2.20f, woodDeck, glm::vec3(10.50f, 0.14f, 0.10f));
    addRamenBox(0.20f, 0.09f, 6.50f, glm::vec3(0.20f, 0.12f, 0.07f), glm::vec3(6.45f, 0.145f, 2.25f));

    // Customer wooden dining counter
    addRamenBox(0.55f, 0.95f, 3.6f, woodCol, glm::vec3(7.85f, 0.55f, 3.3f));
    addRamenBox(0.65f, 0.06f, 3.8f, glm::vec3(0.48f, 0.28f, 0.14f), glm::vec3(7.85f, 1.05f, 3.3f));
    addRamenBox(0.55f, 0.95f, 0.06f, woodCol, glm::vec3(7.85f, 0.55f, 1.45f));

    // Customer dining stools
    for (float sz : { 2.1f, 3.3f, 4.5f }) {
        addRamenCyl(0.035f, 0.65f, 8, stoolCol, glm::vec3(7.15f, 0.42f, sz));
        addRamenCyl(0.20f, 0.06f, 12, glm::vec3(0.45f, 0.25f, 0.12f), glm::vec3(7.15f, 0.74f, sz));
    }

    // Ceramic ramen bowls with noodles & chopsticks on the counter
    for (float bz : { 2.1f, 3.3f, 4.5f }) {
        addRamenCyl(0.14f, 0.10f, 10, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(7.75f, 1.13f, bz));
        addRamenCyl(0.12f, 0.03f, 8, glm::vec3(0.92f, 0.78f, 0.35f), glm::vec3(7.75f, 1.17f, bz));
        addRamenBox(0.28f, 0.015f, 0.015f, glm::vec3(0.85f, 0.65f, 0.35f), glm::vec3(7.75f, 1.20f, bz));
    }

    // Kitchen prep counter with stainless steel top behind dining counter
    addRamenBox(0.55f, 0.90f, 3.8f, darkOak,  glm::vec3(9.10f, 0.55f, 3.3f));
    addRamenBox(0.60f, 0.04f, 3.9f, steelCol, glm::vec3(9.10f, 1.02f, 3.3f));
    addRamenBox(0.55f, 0.90f, 0.06f, darkOak, glm::vec3(9.10f, 0.55f, 1.40f));

    // Bar entry: Staff flip-up counter flap connecting front counter to prep counter
    addRamenBox(0.70f, 0.05f, 0.18f, darkOak, glm::vec3(8.475f, 1.05f, 1.45f));

    // Multi-tier wooden backbar shelving against partition wall
    float shX = 9.65f;
    addRamenBox(0.25f, 1.90f, 3.8f, darkOak, glm::vec3(shX, 1.65f, 3.3f));
    for (float sy : { 1.10f, 1.50f, 1.90f, 2.30f }) {
        addRamenBox(0.32f, 0.04f, 3.7f, woodCol, glm::vec3(shX - 0.04f, sy, 3.3f));
    }
    // Rows of sake / whisky bottles and condiment jars on shelves
    glm::vec3 botGreen(0.12f, 0.32f, 0.18f), botAmber(0.55f, 0.32f, 0.10f), botClear(0.70f, 0.78f, 0.82f);
    for (int b = 0; b < 8; ++b) {
        float bz = 1.7f + b * 0.45f;
        glm::vec3 bCol = (b % 3 == 0) ? botGreen : ((b % 3 == 1) ? botAmber : botClear);
        addRamenCyl(0.045f, 0.22f, 8, bCol, glm::vec3(shX - 0.08f, 1.24f, bz));
        addRamenCyl(0.018f, 0.06f, 6, glm::vec3(0.12f), glm::vec3(shX - 0.08f, 1.37f, bz));
    }
    for (float bz : { 2.0f, 2.6f, 3.2f, 3.8f, 4.4f }) {
        addRamenCyl(0.10f, 0.08f, 8, glm::vec3(0.85f, 0.82f, 0.78f), glm::vec3(shX - 0.06f, 1.56f, bz));
        addRamenCyl(0.08f, 0.08f, 8, glm::vec3(0.80f, 0.78f, 0.75f), glm::vec3(shX - 0.06f, 1.63f, bz));
    }

    // Corridor walls & ceiling (walkway floor is unified with woodDeck)
    addRamenBox(6.5f, 2.80f, 0.15f, wallCol, glm::vec3(11.25f, 1.50f, -1.05f));
    addRamenBox(4.5f, 2.80f, 0.15f, wallCol, glm::vec3(12.25f, 1.50f,  1.25f));
    addRamenBox(6.5f, 0.15f, 2.2f, wallCol, glm::vec3(11.25f, 2.85f, 0.1f));

    // Side Door 1 (Right wall, Kitchen prep door parallel to wall at Z = 1.25m)
    addRamenBox(1.10f, 2.20f, 0.05f, glm::vec3(0.18f), glm::vec3(11.5f, 1.20f, 1.20f));
    addRamenBox(0.98f, 2.10f, 0.04f, steelCol,         glm::vec3(11.5f, 1.20f, 1.18f));
    addRamenBox(0.96f, 0.35f, 0.02f, glm::vec3(0.45f), glm::vec3(11.5f, 0.28f, 1.16f));
    addRamenBox(0.06f, 0.28f, 0.03f, steelCol * 1.3f,  glm::vec3(11.1f, 1.15f, 1.16f));

    // Side Door 2 (Left wall, Staff / restroom door parallel to wall at Z = -1.05m)
    addRamenBox(1.10f, 2.20f, 0.05f, glm::vec3(0.18f), glm::vec3(11.5f, 1.20f, -1.00f));
    addRamenBox(0.98f, 2.10f, 0.04f, darkOak,          glm::vec3(11.5f, 1.20f, -0.98f));
    addRamenBox(0.12f, 0.04f, 0.03f, glm::vec3(0.85f, 0.72f, 0.28f), glm::vec3(11.85f, 1.15f, -0.96f));

    // End of Corridor: Heavy industrial emergency exit door with silver panic bar
    addRamenBox(0.10f, 2.80f, 2.2f, wallCol, glm::vec3(14.55f, 1.50f, 0.1f));
    addRamenBox(0.08f, 2.25f, 1.20f, glm::vec3(0.15f, 0.16f, 0.18f), glm::vec3(14.48f, 1.20f, 0.1f));
    addRamenBox(0.06f, 2.15f, 1.08f, steelCol * 0.45f, glm::vec3(14.46f, 1.20f, 0.1f));
    addRamenBox(0.04f, 0.08f, 0.95f, steelCol, glm::vec3(14.40f, 1.10f, 0.1f));
    addRamenBox(0.08f, 0.24f, 0.48f, glm::vec3(0.12f), glm::vec3(14.42f, 2.45f, 0.1f));

    // Indigo Noren fabric curtain panels hanging from front nook lintel
    glm::vec3 norenIndigo(0.10f, 0.14f, 0.28f), norenCrest(0.90f, 0.88f, 0.78f);
    float norenX = 6.55f, norenY = 2.40f;
    for (int p = 0; p < 5; ++p) {
        float pz = 0.5f + p * 1.0f;
        addRamenBox(0.03f, 0.55f, 0.90f, norenIndigo, glm::vec3(norenX, norenY, pz));
        addRamenBox(0.04f, 0.22f, 0.24f, norenCrest, glm::vec3(norenX - 0.005f, norenY - 0.05f, pz));
    }
    addRamenBox(0.04f, 0.04f, 5.2f, glm::vec3(0.35f, 0.22f, 0.12f), glm::vec3(norenX, 2.70f, 2.5f));

    // Single static batch draw call for the entire detailed ramen shop, backbar & corridor
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(rVerts, rIdxs), glm::mat4(1.0f), false));

    // Glowing green lens for emergency exit sign at end of corridor
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.04f, 0.18f, 0.42f, glm::vec3(0.12f, 1.0f, 0.35f)),
                                  glm::translate(glm::mat4(1.0f), glm::vec3(14.37f, 2.45f, 0.1f)), true));

    // Exterior low pitched red awning above the nook
    auto ramenAwning = CityBuilder::createBox(0.85f, 0.12f, 5.8f, glm::vec3(0.28f, 0.08f, 0.08f));
    glm::mat4 awningTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.25f, 2.80f, 2.25f))
                       * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rootNodes.push_back(SceneNode(ramenAwning, awningTr));

    // Chochin paper lantern mounted on south pillar wall
    auto lanternMesh = Model::loadOBJ("assets/models/chochin_lantern.obj", glm::vec3(0.95f, 0.18f, 0.10f));
    if (lanternMesh) {
        glm::mat4 lanternTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.48f, 2.30f, 5.65f))
                            * glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(lanternMesh, lanternTr, true));
    }

    // Katakana neon sign mounted above the awning
    auto ramenNeonMesh = Model::loadOBJ("assets/models/neon_sign_ramen.obj", glm::vec3(1.0f, 0.22f, 0.70f));
    if (ramenNeonMesh) {
        glm::mat4 ramenTr = glm::translate(glm::mat4(1.0f), glm::vec3(6.42f, 3.75f, 2.25f))
                          * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(ramenNeonMesh, ramenTr, true));
    }

    // Warm atmospheric interior point light inside dining nook
    pointLights.push_back({ glm::vec3(7.2f, 2.1f, 3.0f), glm::vec3(1.0f, 0.72f, 0.35f) * 0.06f,
                            glm::vec3(1.0f, 0.72f, 0.35f) * 2.2f, glm::vec3(1.0f, 0.72f, 0.35f) * 1.8f,
                            1.0f, 0.16f, 0.08f });

    // Subtle atmospheric green emergency corridor light
    glm::vec3 exitGreen(0.12f, 0.95f, 0.35f);
    pointLights.push_back({ glm::vec3(13.5f, 2.3f, 0.1f), exitGreen * 0.02f, exitGreen * 1.1f, exitGreen * 0.8f,
                            1.0f, 0.22f, 0.12f });
}

void ShopBuilder::buildBarFacade(std::vector<SceneNode>& rootNodes,
                                std::vector<PointLight>& pointLights) {
    auto addBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false, bool isNeon = false) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive, false, nullptr, isNeon));
    };

    float bX = -6.45f, bZ = -22.0f;
    float vipZ = -26.2f;
    // Main building volume
    addBox(7.5f, 9.5f, 16.0f, glm::vec3(0.16f, 0.17f, 0.20f), glm::vec3(-10.25f, 4.75f, bZ));

    // Dark polished granite base wainscot divided into piers avoiding both door openings
    glm::vec3 plinthCol(0.08f, 0.08f, 0.09f);
    glm::vec3 trimCol(0.65f, 0.52f, 0.25f);
    // Plinth 1: South of secondary door
    addBox(0.12f, 0.85f, 3.10f, plinthCol, glm::vec3(bX + 0.03f, 0.425f, -28.45f));
    addBox(0.14f, 0.04f, 3.10f, trimCol, glm::vec3(bX + 0.04f, 0.87f, -28.45f));
    // Plinth 2: Center pier between secondary door and main entrance
    addBox(0.12f, 0.85f, 2.65f, plinthCol, glm::vec3(bX + 0.03f, 0.425f, -24.175f));
    addBox(0.14f, 0.04f, 2.65f, trimCol, glm::vec3(bX + 0.04f, 0.87f, -24.175f));
    // Plinth 3: North of main entrance under lounge display windows
    addBox(0.12f, 0.85f, 7.15f, plinthCol, glm::vec3(bX + 0.03f, 0.425f, -17.575f));
    addBox(0.14f, 0.04f, 7.15f, trimCol, glm::vec3(bX + 0.04f, 0.87f, -17.575f));

    // Clean stone door thresholds on the ground plane
    addBox(0.16f, 0.04f, 1.30f, glm::vec3(0.22f, 0.22f, 0.24f), glm::vec3(bX + 0.04f, 0.02f, vipZ));
    addBox(0.18f, 0.04f, 1.60f, glm::vec3(0.28f, 0.26f, 0.22f), glm::vec3(bX + 0.05f, 0.02f, bZ));

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

    // Compact art-deco entrance canopy over main portal positioned to keep bar neon sign fully visible
    addBox(0.42f, 0.06f, 1.70f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(bX + 0.21f, 2.82f, bZ));
    addBox(0.03f, 0.08f, 1.74f, glm::vec3(0.85f, 0.70f, 0.28f), glm::vec3(bX + 0.42f, 2.82f, bZ));
    addBox(0.42f, 0.08f, 0.03f, glm::vec3(0.85f, 0.70f, 0.28f), glm::vec3(bX + 0.21f, 2.82f, bZ - 0.85f));
    addBox(0.42f, 0.08f, 0.03f, glm::vec3(0.85f, 0.70f, 0.28f), glm::vec3(bX + 0.21f, 2.82f, bZ + 0.85f));
    // Warm recessed canopy downlight
    addBox(0.24f, 0.015f, 0.80f, glm::vec3(1.0f, 0.88f, 0.62f), glm::vec3(bX + 0.21f, 2.78f, bZ), true);

    // Secondary lounge and service door on the left side
    addBox(0.08f, 2.30f, 1.25f, glm::vec3(0.10f, 0.10f, 0.12f), glm::vec3(bX, 1.15f, vipZ));
    addBox(0.06f, 2.22f, 1.12f, glm::vec3(0.14f, 0.14f, 0.16f), glm::vec3(bX + 0.01f, 1.15f, vipZ));
    addBox(0.04f, 0.03f, 0.14f, glm::vec3(0.75f, 0.62f, 0.28f), glm::vec3(bX + 0.05f, 1.05f, vipZ - 0.35f));
    addBox(0.06f, 0.22f, 1.05f, glm::vec3(0.55f, 0.45f, 0.22f), glm::vec3(bX + 0.03f, 0.20f, vipZ));
    // Electronic keycard access pad with illuminated indicator
    addBox(0.03f, 0.18f, 0.10f, glm::vec3(0.15f, 0.15f, 0.16f), glm::vec3(bX + 0.04f, 1.35f, vipZ + 0.75f));
    addBox(0.04f, 0.02f, 0.02f, glm::vec3(0.0f, 0.95f, 0.95f), glm::vec3(bX + 0.05f, 1.38f, vipZ + 0.75f), true);
    // Overhead shielded sconce light fixture
    addBox(0.12f, 0.08f, 0.12f, glm::vec3(0.65f, 0.52f, 0.25f), glm::vec3(bX + 0.06f, 2.45f, vipZ));
    addBox(0.08f, 0.02f, 0.08f, glm::vec3(1.0f, 0.85f, 0.55f), glm::vec3(bX + 0.06f, 2.40f, vipZ), true);

    // Warm backlit architectural lounge display windows
    float winZs[2] = { -18.2f, -15.8f };
    for (float wz : winZs) {
        addBox(0.08f, 1.80f, 1.50f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(bX + 0.02f, 1.85f, wz));
        addBox(0.05f, 1.66f, 1.36f, glm::vec3(0.95f, 0.72f, 0.35f) * 0.85f, glm::vec3(bX + 0.03f, 1.85f, wz), true);
        addBox(0.06f, 1.66f, 0.04f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(bX + 0.04f, 1.85f, wz));
        addBox(0.06f, 0.04f, 1.36f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(bX + 0.04f, 1.85f, wz));
        addBox(0.08f, 0.04f, 1.30f, glm::vec3(0.18f, 0.15f, 0.12f), glm::vec3(bX - 0.02f, 1.50f, wz));
        addBox(0.08f, 0.04f, 1.30f, glm::vec3(0.18f, 0.15f, 0.12f), glm::vec3(bX - 0.02f, 2.10f, wz));
    }

    // Illuminated brass cocktail menu display case
    addBox(0.05f, 0.65f, 0.50f, glm::vec3(0.75f, 0.60f, 0.25f), glm::vec3(bX + 0.03f, 1.60f, -20.6f));
    addBox(0.06f, 0.55f, 0.40f, glm::vec3(0.98f, 0.90f, 0.65f), glm::vec3(bX + 0.04f, 1.60f, -20.6f), true);

    // Neon bar sign overhead
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

void ShopBuilder::buildHotel(std::vector<SceneNode>& rootNodes,
                            std::vector<PointLight>& pointLights) {
    auto addBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive));
    };

    float hX = 10.25f, hY = 4.5f, hZ = -22.0f;
    float frontX = 6.45f;
    float sideZ = -14.0f;

    // Main hotel mass
    addBox(7.5f, 9.0f, 16.0f, glm::vec3(0.20f, 0.18f, 0.16f), glm::vec3(hX, hY, hZ));

    // Dark polished granite base with brass cap molding divided into piers avoiding both door openings
    glm::vec3 hPlinthCol(0.08f, 0.08f, 0.09f);
    glm::vec3 hTrimCol(0.65f, 0.52f, 0.25f);
    // West face - South pier
    addBox(0.12f, 0.85f, 6.65f, hPlinthCol, glm::vec3(frontX + 0.03f, 0.425f, -26.675f));
    addBox(0.14f, 0.04f, 6.65f, hTrimCol,   glm::vec3(frontX - 0.02f, 0.87f,  -26.675f));
    // West face - North pier
    addBox(0.12f, 0.85f, 6.65f, hPlinthCol, glm::vec3(frontX + 0.03f, 0.425f, -17.325f));
    addBox(0.14f, 0.04f, 6.65f, hTrimCol,   glm::vec3(frontX - 0.02f, 0.87f,  -17.325f));

    // South face - West pier
    addBox(2.80f, 0.85f, 0.12f, hPlinthCol, glm::vec3(7.90f,  0.425f, sideZ + 0.03f));
    addBox(2.80f, 0.04f, 0.14f, hTrimCol,   glm::vec3(7.90f,  0.87f,  sideZ - 0.02f));
    // South face - East pier
    addBox(3.30f, 0.85f, 0.12f, hPlinthCol, glm::vec3(12.35f, 0.425f, sideZ + 0.03f));
    addBox(3.30f, 0.04f, 0.14f, hTrimCol,   glm::vec3(12.35f, 0.87f,  sideZ - 0.02f));

    // Stone threshold slabs at sidewalk level
    addBox(0.20f, 0.04f, 2.70f, glm::vec3(0.28f, 0.26f, 0.22f), glm::vec3(frontX - 0.05f, 0.02f, hZ));
    addBox(1.50f, 0.04f, 0.20f, glm::vec3(0.24f, 0.24f, 0.26f), glm::vec3(10.0f, 0.02f, sideZ - 0.05f));

    // Main street grand entrance (West face, directly opposite the Bar):
    glm::vec3 lobbyGlow(1.0f, 0.88f, 0.65f);
    // Dark bronze double door portal frame
    addBox(0.12f, 2.70f, 2.60f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(frontX, 1.35f, hZ));
    // Glowing warm interior lobby double doors
    addBox(0.06f, 2.45f, 1.10f, lobbyGlow * 0.90f, glm::vec3(frontX - 0.02f, 1.25f, hZ - 0.60f), true);
    addBox(0.06f, 2.45f, 1.10f, lobbyGlow * 0.90f, glm::vec3(frontX - 0.02f, 1.25f, hZ + 0.60f), true);
    // Vertical polished brass pull handles on both doors
    addBox(0.04f, 0.70f, 0.03f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(frontX - 0.06f, 1.25f, hZ - 0.10f));
    addBox(0.04f, 0.70f, 0.03f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(frontX - 0.06f, 1.25f, hZ + 0.10f));
    // Warm glowing transom window above double doors
    addBox(0.06f, 0.40f, 2.30f, lobbyGlow, glm::vec3(frontX - 0.02f, 2.70f, hZ), true);

    // Cantilevered entrance marquee canopy over main double doors
    addBox(0.55f, 0.08f, 3.00f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(frontX - 0.275f, 2.95f, hZ));
    addBox(0.03f, 0.10f, 3.04f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.55f, 2.95f, hZ));
    addBox(0.55f, 0.10f, 0.03f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.275f, 2.95f, hZ - 1.50f));
    addBox(0.55f, 0.10f, 0.03f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.275f, 2.95f, hZ + 1.50f));
    // Warm recessed canopy downlight
    addBox(0.35f, 0.02f, 1.00f, glm::vec3(1.0f, 0.90f, 0.70f), glm::vec3(frontX - 0.275f, 2.90f, hZ), true);

    // Side street entrance door (South face at Z = sideZ):
    addBox(1.30f, 2.45f, 0.10f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(10.0f, 1.25f, sideZ));
    addBox(1.10f, 2.35f, 0.06f, glm::vec3(0.18f, 0.14f, 0.12f), glm::vec3(10.0f, 1.25f, sideZ - 0.02f));
    addBox(0.60f, 0.90f, 0.07f, lobbyGlow * 0.85f, glm::vec3(10.0f, 1.65f, sideZ - 0.03f), true);
    addBox(0.12f, 0.03f, 0.04f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(9.60f, 1.15f, sideZ - 0.06f));
    // Overhead illuminated sconce above side door
    addBox(0.20f, 0.08f, 0.15f, glm::vec3(0.65f, 0.52f, 0.25f), glm::vec3(10.0f, 2.60f, sideZ - 0.08f));
    addBox(0.14f, 0.03f, 0.10f, glm::vec3(1.0f, 0.85f, 0.55f), glm::vec3(10.0f, 2.55f, sideZ - 0.08f), true);

    // Ground floor glowing windows (Lobby / Lounge):
    float groundWinZs[] = { -26.2f, -17.8f, -15.4f };
    for (float wz : groundWinZs) {
        addBox(0.08f, 1.80f, 1.60f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 1.85f, wz));
        addBox(0.05f, 1.66f, 1.46f, lobbyGlow * 0.88f, glm::vec3(frontX - 0.02f, 1.85f, wz), true);
        addBox(0.06f, 1.66f, 0.04f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(frontX - 0.03f, 1.85f, wz));
        addBox(0.06f, 0.04f, 1.46f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(frontX - 0.03f, 1.85f, wz));
    }
    // Side street ground windows (South face)
    float sideWinXs[] = { 7.8f, 12.2f };
    for (float wx : sideWinXs) {
        addBox(1.50f, 1.80f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 1.85f, sideZ));
        addBox(1.36f, 1.66f, 0.05f, lobbyGlow * 0.85f, glm::vec3(wx, 1.85f, sideZ - 0.02f), true);
        addBox(0.04f, 1.66f, 0.06f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(wx, 1.85f, sideZ - 0.03f));
        addBox(1.36f, 0.04f, 0.06f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(wx, 1.85f, sideZ - 0.03f));
    }

    // Upper floors (Floor 2 at Y = 4.6f, Floor 3 at Y = 7.0f): mostly dark with some warmly lit
    glm::vec3 darkWinCol(0.08f, 0.09f, 0.11f);
    glm::vec3 litWinCol(0.95f, 0.80f, 0.45f);

    float upperZs[] = { -27.5f, -24.8f, -22.0f, -19.2f, -16.5f };
    for (float wz : upperZs) {
        bool litF2 = (wz == -24.8f || wz == -19.2f);
        addBox(0.08f, 1.40f, 1.20f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 4.6f, wz));
        addBox(0.05f, 1.26f, 1.06f, litF2 ? litWinCol : darkWinCol, glm::vec3(frontX - 0.02f, 4.6f, wz), litF2);
        addBox(0.06f, 1.26f, 0.03f, glm::vec3(0.25f, 0.22f, 0.20f), glm::vec3(frontX - 0.03f, 4.6f, wz));

        bool litF3 = (wz == -27.5f || wz == -16.5f);
        addBox(0.08f, 1.40f, 1.20f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 7.0f, wz));
        addBox(0.05f, 1.26f, 1.06f, litF3 ? litWinCol : darkWinCol, glm::vec3(frontX - 0.02f, 7.0f, wz), litF3);
        addBox(0.06f, 1.26f, 0.03f, glm::vec3(0.25f, 0.22f, 0.20f), glm::vec3(frontX - 0.03f, 7.0f, wz));
    }

    // Upper floor side windows (South face)
    for (float wx : sideWinXs) {
        bool litS2 = (wx == 7.8f);
        addBox(1.20f, 1.40f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 4.6f, sideZ));
        addBox(1.06f, 1.26f, 0.05f, litS2 ? litWinCol : darkWinCol, glm::vec3(wx, 4.6f, sideZ - 0.02f), litS2);

        bool litS3 = (wx == 12.2f);
        addBox(1.20f, 1.40f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 7.0f, sideZ));
        addBox(1.06f, 1.26f, 0.05f, litS3 ? litWinCol : darkWinCol, glm::vec3(wx, 7.0f, sideZ - 0.02f), litS3);
    }

    // Neon hotel sign overhead
    auto hotelNeonMesh = Model::loadOBJ("assets/models/neon_sign_hotel.obj", glm::vec3(1.0f, 0.65f, 0.15f));
    if (hotelNeonMesh) {
        glm::mat4 hotelTr = glm::translate(glm::mat4(1.0f), glm::vec3(frontX - 0.08f, 3.55f, hZ))
                          * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(hotelNeonMesh, hotelTr, true));
    }

    // Warm hotel lobby entrance point light
    pointLights.push_back({ glm::vec3(5.5f, 2.2f, hZ), glm::vec3(1.0f, 0.85f, 0.55f) * 0.06f,
                            glm::vec3(1.0f, 0.85f, 0.55f) * 2.2f, glm::vec3(1.0f, 0.85f, 0.55f) * 1.8f,
                            1.0f, 0.18f, 0.09f });
}
