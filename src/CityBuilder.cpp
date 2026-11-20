#include "CityBuilder.h"
#include "ShopBuilder.h"
#include "StreetProps.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

void CityBuilder::appendBox(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                           float width, float height, float depth, const glm::vec3& color,
                           const glm::mat4& tr) {
    float w = width * 0.5f, h = height * 0.5f, d = depth * 0.5f;
    glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(tr)));
    auto addFace = [&](const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n) {
        unsigned int b = static_cast<unsigned int>(verts.size());
        glm::vec3 tn = glm::normalize(normMat * n);
        verts.push_back({ glm::vec3(tr * glm::vec4(p0, 1.0f)), tn, color, {0.0f, 0.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p1, 1.0f)), tn, color, {1.0f, 0.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p2, 1.0f)), tn, color, {1.0f, 1.0f} });
        verts.push_back({ glm::vec3(tr * glm::vec4(p3, 1.0f)), tn, color, {0.0f, 1.0f} });
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

void CityBuilder::appendPlane(std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                            float width, float depth, const glm::vec3& color,
                            const glm::mat4& tr, float tileX, float tileY) {
    float hw = width * 0.5f, hd = depth * 0.5f;
    glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(tr)));
    glm::vec3 n = glm::normalize(normMat * glm::vec3(0.0f, 1.0f, 0.0f));
    unsigned int b = static_cast<unsigned int>(verts.size());
    verts.push_back({ glm::vec3(tr * glm::vec4(-hw, 0.0f, -hd, 1.0f)), n, color, {0.0f, tileY} });
    verts.push_back({ glm::vec3(tr * glm::vec4( hw, 0.0f, -hd, 1.0f)), n, color, {tileX, tileY} });
    verts.push_back({ glm::vec3(tr * glm::vec4( hw, 0.0f,  hd, 1.0f)), n, color, {tileX, 0.0f} });
    verts.push_back({ glm::vec3(tr * glm::vec4(-hw, 0.0f,  hd, 1.0f)), n, color, {0.0f, 0.0f} });
    idxs.push_back(b); idxs.push_back(b + 2); idxs.push_back(b + 1);
    idxs.push_back(b); idxs.push_back(b + 3); idxs.push_back(b + 2);
}

std::shared_ptr<Mesh> CityBuilder::createBox(float width, float height, float depth, const glm::vec3& color) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    appendBox(vertices, indices, width, height, depth, color);
    return std::make_shared<Mesh>(vertices, indices);
}

std::shared_ptr<Mesh> CityBuilder::createPlane(float width, float depth, const glm::vec3& color, float tileX, float tileY) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    appendPlane(vertices, indices, width, depth, color, glm::mat4(1.0f), tileX, tileY);
    return std::make_shared<Mesh>(vertices, indices);
}

// Industrial roll-up security shutter with reinforced guide rails and corrugated alternating slats
static void addRibbedShutter(std::vector<SceneNode>& rootNodes, float x, float y, float z, float w, float h, int slats = 12) {
    glm::vec3 frameCol(0.18f, 0.18f, 0.20f);
    glm::vec3 slatCol1(0.24f, 0.25f, 0.27f);
    glm::vec3 slatCol2(0.20f, 0.21f, 0.23f);
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.08f, h, 0.08f, frameCol), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z - w * 0.5f))));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.08f, h, 0.08f, frameCol), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z + w * 0.5f))));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.16f, 0.22f, w + 0.12f, frameCol), glm::translate(glm::mat4(1.0f), glm::vec3(x, y + h * 0.5f, z))));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.10f, 0.12f, w, frameCol), glm::translate(glm::mat4(1.0f), glm::vec3(x, y - h * 0.5f + 0.06f, z))));
    float slatH = (h - 0.12f) / slats, startY = y - h * 0.5f + 0.12f + slatH * 0.5f;
    for (int i = 0; i < slats; ++i) {
        float sy = startY + i * slatH, xOff = (i % 2 == 0) ? 0.012f : -0.012f;
        glm::vec3 sCol = (i % 2 == 0) ? slatCol1 : slatCol2;
        rootNodes.push_back(SceneNode(CityBuilder::createBox(0.04f, slatH * 0.92f, w - 0.04f, sCol), glm::translate(glm::mat4(1.0f), glm::vec3(x + xOff, sy, z))));
    }
}

void CityBuilder::buildSkyElements(std::vector<SceneNode>& rootNodes, std::shared_ptr<Texture> moonTex) {
    // Textured celestial lunar sphere generated via UV spherical coordinate parameterization
    std::vector<Vertex> moonVerts;
    std::vector<unsigned int> moonIdxs;
    int rings = 24, segs = 32;
    float mRadius = 16.0f;
    glm::vec3 mCenter(-80.0f, 210.0f, -360.0f);

    for (int r = 0; r <= rings; ++r) {
        float phi = 3.14159265f * float(r) / rings;
        float y = std::cos(phi), r_sin = std::sin(phi);
        for (int s = 0; s <= segs; ++s) {
            float theta = 6.2831853f * float(s) / segs;
            float x = r_sin * std::cos(theta), z = r_sin * std::sin(theta);
            float u = float(s) / segs;
            float v = float(r) / rings;
            moonVerts.push_back({ glm::vec3(x, y, z) * mRadius, glm::vec3(x, y, z), glm::vec3(1.0f), {u, v} });
        }
    }
    for (int r = 0; r < rings; ++r) {
        for (int s = 0; s < segs; ++s) {
            unsigned int i0 = r * (segs + 1) + s, i1 = i0 + 1;
            unsigned int i2 = (r + 1) * (segs + 1) + s, i3 = i2 + 1;
            moonIdxs.push_back(i0); moonIdxs.push_back(i2); moonIdxs.push_back(i1);
            moonIdxs.push_back(i1); moonIdxs.push_back(i2); moonIdxs.push_back(i3);
        }
    }
    glm::mat4 moonTr = glm::translate(glm::mat4(1.0f), mCenter)
                     * glm::rotate(glm::mat4(1.0f), glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rootNodes.push_back(SceneNode(std::make_shared<Mesh>(moonVerts, moonIdxs), moonTr, true, false, moonTex));

    // Distant randomized celestial starfield dome with spectral color temperature distribution
    std::vector<Vertex> starVerts;
    std::vector<unsigned int> starIdxs;
    for (int i = 0; i < 350; ++i) {
        float fi = float(i);
        float rx = std::fmod(std::abs(std::sin(fi * 12.9898f + 78.233f) * 43758.5453f), 1.0f);
        float ry = std::fmod(std::abs(std::sin(fi * 37.719f + 14.123f) * 28461.1234f), 1.0f);
        float rz = std::fmod(std::abs(std::sin(fi * 93.185f + 49.813f) * 51324.9876f), 1.0f);

        float theta = rx * 6.2831853f;
        float phi = 0.20f + ry * 1.10f;
        float rDist = 420.0f + rz * 100.0f;

        float sx = rDist * std::sin(phi) * std::cos(theta);
        float sy = rDist * std::cos(phi);
        float sz = rDist * std::sin(phi) * std::sin(theta);

        if (glm::distance(glm::vec3(sx, sy, sz), mCenter) < 35.0f) continue;

        glm::vec3 sCol = (i % 3 == 0) ? glm::vec3(0.75f, 0.88f, 1.0f) :
                         ((i % 3 == 1) ? glm::vec3(1.0f, 0.98f, 0.92f) : glm::vec3(1.0f, 0.88f, 0.65f));
        float brightness = 0.5f + 0.5f * std::fmod(std::abs(std::sin(fi * 45.123f) * 12345.0f), 1.0f);
        sCol *= (0.7f + 0.5f * brightness);

        float szStar = 0.40f + 0.20f * brightness;
        unsigned int b = static_cast<unsigned int>(starVerts.size());
        starVerts.push_back({ {sx - szStar, sy, sz}, {0, 0, 1}, sCol, {0,0} });
        starVerts.push_back({ {sx + szStar, sy, sz}, {0, 0, 1}, sCol, {1,0} });
        starVerts.push_back({ {sx, sy + szStar, sz}, {0, 0, 1}, sCol, {0.5f,1} });
        starIdxs.push_back(b); starIdxs.push_back(b + 1); starIdxs.push_back(b + 2);
        starIdxs.push_back(b); starIdxs.push_back(b + 2); starIdxs.push_back(b + 1);
    }
    if (!starVerts.empty()) {
        rootNodes.push_back(SceneNode(std::make_shared<Mesh>(starVerts, starIdxs), glm::mat4(1.0f), true));
    }
}

void CityBuilder::buildSkyscrapers(std::vector<SceneNode>& rootNodes, std::shared_ptr<Texture> moonTex) {
    buildSkyElements(rootNodes, moonTex);

    // Perimeter skyscraper tower layouts bounding the street volume (0: north, 1: south, 2: east, 3: west)
    struct TowerDef {
        glm::vec3 pos;
        float w, h, d;
        int dir;
    };

    std::vector<TowerDef> towers = {
        { {-26.0f, 24.0f, -86.0f}, 18.0f, 48.0f, 16.0f, 0 }, { { -8.0f, 30.0f, -92.0f}, 16.0f, 60.0f, 16.0f, 0 },
        { { 10.0f, 26.0f, -88.0f}, 18.0f, 52.0f, 18.0f, 0 }, { { 28.0f, 22.0f, -84.0f}, 16.0f, 44.0f, 16.0f, 0 },
        { {-24.0f, 25.0f,  58.0f}, 16.0f, 50.0f, 16.0f, 1 }, { { -6.0f, 22.0f,  54.0f}, 18.0f, 44.0f, 16.0f, 1 },
        { { 12.0f, 28.0f,  60.0f}, 16.0f, 56.0f, 18.0f, 1 }, { { 30.0f, 23.0f,  56.0f}, 18.0f, 46.0f, 16.0f, 1 },
        { { 36.0f, 22.0f, -24.0f}, 16.0f, 44.0f, 18.0f, 2 }, { { 38.0f, 28.0f,  -8.0f}, 16.0f, 56.0f, 16.0f, 2 },
        { { 35.0f, 19.0f,   8.0f}, 16.0f, 38.0f, 18.0f, 2 }, { {-36.0f, 23.0f, -24.0f}, 16.0f, 46.0f, 18.0f, 3 },
        { {-38.0f, 28.0f,  -8.0f}, 16.0f, 56.0f, 16.0f, 3 }, { {-34.0f, 18.0f,   8.0f}, 16.0f, 36.0f, 18.0f, 3 }
    };

    // Monolithic dark tower massing and flashing red rooftop aviation warning beacons
    glm::vec3 towerColor(0.12f, 0.13f, 0.16f);
    glm::vec3 redBeacon(1.0f, 0.15f, 0.15f);
    auto beaconMesh = createBox(0.6f, 0.6f, 0.6f, redBeacon);

    // Procedural facade window arrays with pseudo-random illumination states
    std::vector<Vertex> litVerts, unlitVerts;
    std::vector<unsigned int> litIdxs, unlitIdxs;

    auto addQuad = [](std::vector<Vertex>& verts, std::vector<unsigned int>& idxs,
                      const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3,
                      const glm::vec3& norm, const glm::vec3& col) {
        unsigned int b = static_cast<unsigned int>(verts.size());
        verts.push_back({ p0, norm, col, {0.0f, 0.0f} });
        verts.push_back({ p1, norm, col, {1.0f, 0.0f} });
        verts.push_back({ p2, norm, col, {1.0f, 1.0f} });
        verts.push_back({ p3, norm, col, {0.0f, 1.0f} });
        idxs.push_back(b); idxs.push_back(b + 1); idxs.push_back(b + 2);
        idxs.push_back(b); idxs.push_back(b + 2); idxs.push_back(b + 3);
    };

    for (size_t i = 0; i < towers.size(); ++i) {
        const auto& t = towers[i];
        rootNodes.push_back(SceneNode(createBox(t.w, t.h, t.d, towerColor), glm::translate(glm::mat4(1.0f), t.pos)));
        rootNodes.push_back(SceneNode(beaconMesh, glm::translate(glm::mat4(1.0f), glm::vec3(t.pos.x, t.h + 0.3f, t.pos.z)), true));

        // Generate street-facing window grid based on tower facade dimensions
        float span = (t.dir < 2) ? t.w : t.d;
        int cols = static_cast<int>(span / 2.6f);
        int floors = static_cast<int>((t.h - 8.0f) / 2.8f);

        for (int c = 0; c < cols; ++c) {
            float u = -span * 0.5f + 1.6f + c * 2.6f;
            for (int r = 0; r < floors; ++r) {
                float y = 5.0f + r * 2.8f;
                // Sinusoidal hash to distribute lit office suites vs dark glass
                float val = std::fmod(std::abs(std::sin((c + 1) * 12.9898f + (r + 1) * 78.233f + (i + 1) * 37.719f) * 43758.5453f), 1.0f);

                bool isLit = (val > 0.68f);
                glm::vec3 col = isLit 
                    ? (val > 0.88f ? glm::vec3(0.32f, 0.35f, 0.38f) : (val > 0.78f ? glm::vec3(0.38f, 0.32f, 0.22f) : glm::vec3(0.40f, 0.36f, 0.28f)))
                    : glm::vec3(0.04f, 0.04f, 0.05f);

                auto& targetV = isLit ? litVerts : unlitVerts;
                auto& targetI = isLit ? litIdxs : unlitIdxs;

                // Align window quads according to the tower's street-facing perimeter direction
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

    // Batch draw calls for glowing windows and non-emissive skyscraper surfaces
    if (!litVerts.empty()) rootNodes.push_back(SceneNode(std::make_shared<Mesh>(litVerts, litIdxs), glm::mat4(1.0f), true));
    if (!unlitVerts.empty()) rootNodes.push_back(SceneNode(std::make_shared<Mesh>(unlitVerts, unlitIdxs), glm::mat4(1.0f), false));
}

void CityBuilder::buildExtendedStreet(std::vector<SceneNode>& rootNodes,
                                     std::vector<PointLight>& pointLights,
                                     std::shared_ptr<Texture> asphaltTex) {
    auto addPoint = [&](const glm::vec3& p, const glm::vec3& c, float amb, float diff, float spec, float lin, float quad) {
        pointLights.push_back({ p, c * amb, c * diff, c * spec, 1.0f, lin, quad });
    };

    auto addNeonBox = [&](float w, float h, float d, const glm::vec3& pos, const glm::vec3& col) {
        rootNodes.push_back(SceneNode(createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), true, false, nullptr, false, 2.85f));
    };

    auto addWindow = [&](float x, float y, float z, float w, float h, bool isLit, const glm::vec3& litCol) {
        rootNodes.push_back(SceneNode(createBox(0.06f, h + 0.16f, w + 0.16f, glm::vec3(0.12f, 0.12f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))));
        if (isLit) {
            rootNodes.push_back(SceneNode(createBox(0.08f, h, w, litCol), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)), true));
        } else {
            rootNodes.push_back(SceneNode(createBox(0.07f, h, w, glm::vec3(0.06f, 0.07f, 0.09f)), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))));
        }
    };

    auto addDoorHandle = [&](float x, float y, float z, const glm::vec3& col) {
        rootNodes.push_back(SceneNode(createBox(0.03f, 0.55f, 0.03f, col), glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z))));
    };

    // Ground foundation and stripes
    auto cityGround = createPlane(220.0f, 260.0f, glm::vec3(0.08f, 0.08f, 0.09f), 60.0f, 70.0f);
    rootNodes.push_back(SceneNode(cityGround, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.01f, -10.0f)), false, true, asphaltTex));

    auto northGround = createPlane(40.0f, 38.0f, glm::vec3(0.12f), 16.0f, 15.0f);
    rootNodes.push_back(SceneNode(northGround, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -57.0f)), false, true, asphaltTex));
    auto southGround = createPlane(40.0f, 28.0f, glm::vec3(0.12f), 16.0f, 11.0f);
    rootNodes.push_back(SceneNode(southGround, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 32.0f)), false, true, asphaltTex));

    auto stripeMesh = createPlane(0.2f, 2.0f, glm::vec3(0.9f, 0.8f, 0.2f));
    for (float z = -72.0f; z <= -41.0f; z += 5.0f)
        rootNodes.push_back(SceneNode(stripeMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z))));
    for (float z = 21.0f; z <= 42.0f; z += 5.0f)
        rootNodes.push_back(SceneNode(stripeMesh, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.005f, z))));

    glm::vec3 curbColor(0.22f, 0.22f, 0.24f);
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 36.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.1f, 0.1f, -57.0f))));
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 36.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3( 5.1f, 0.1f, -57.0f))));
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 26.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3(-5.1f, 0.1f,  31.0f))));
    rootNodes.push_back(SceneNode(createBox(2.2f, 0.2f, 26.0f, curbColor), glm::translate(glm::mat4(1.0f), glm::vec3( 5.1f, 0.1f,  31.0f))));

    // Streetlamps
    auto poleMesh = createBox(0.2f, 4.5f, 0.2f, glm::vec3(0.2f, 0.2f, 0.22f));
    auto armMesh  = createBox(1.2f, 0.15f, 0.15f, glm::vec3(0.25f, 0.25f, 0.27f));
    glm::vec3 cyanColor(0.0f, 0.9f, 1.0f), amberColor(1.0f, 0.65f, 0.15f);
    auto cyanFixture  = createBox(0.4f, 0.2f, 0.3f, cyanColor);
    auto amberFixture = createBox(0.4f, 0.2f, 0.3f, amberColor);

    for (float z : { -48.0f, -64.0f, 27.0f, 39.0f }) {
        glm::mat4 poleL = glm::translate(glm::mat4(1.0f), glm::vec3(-4.2f, 2.25f, z));
        SceneNode poleLNode(poleMesh, poleL);
        glm::mat4 armL = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.1f, 0.0f));
        SceneNode armLNode(armMesh, armL);
        armLNode.addChild(SceneNode(cyanFixture, glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, -0.15f, 0.0f)), true));
        poleLNode.addChild(armLNode);
        rootNodes.push_back(poleLNode);
        addPoint(glm::vec3(-3.2f, 4.2f, z), cyanColor,  0.05f, 1.3f, 1.2f, 0.14f, 0.07f);

        glm::mat4 poleR = glm::translate(glm::mat4(1.0f), glm::vec3(4.2f, 2.25f, z));
        SceneNode poleRNode(poleMesh, poleR);
        glm::mat4 armR = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 2.1f, 0.0f));
        SceneNode armRNode(armMesh, armR);
        armRNode.addChild(SceneNode(amberFixture, glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.15f, 0.0f)), true));
        poleRNode.addChild(armRNode);
        rootNodes.push_back(poleRNode);
        addPoint(glm::vec3( 3.2f, 4.2f, z), amberColor, 0.05f, 1.3f, 1.2f, 0.14f, 0.07f);
    }

    // Industrial annex
    rootNodes.push_back(SceneNode(createBox(8.0f, 7.5f, 16.0f, glm::vec3(0.17f, 0.17f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 3.75f, -66.0f))));
    addRibbedShutter(rootNodes, -6.45f, 1.2f, -66.0f, 3.2f, 2.4f, 12);
    addWindow(-6.45f, 5.2f, -63.5f, 1.6f, 1.1f, false, glm::vec3(0.0f));
    addWindow(-6.45f, 5.2f, -68.5f, 1.6f, 1.1f, true, glm::vec3(0.35f, 0.30f, 0.22f));

    // Izakaya
    rootNodes.push_back(SceneNode(createBox(8.0f, 8.5f, 12.0f, glm::vec3(0.19f, 0.17f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 4.25f, -52.0f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.35f, 12.2f, glm::vec3(0.14f, 0.13f, 0.12f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 8.65f, -52.0f))));
    auto izakayaAwning = createBox(0.9f, 0.12f, 4.4f, glm::vec3(0.32f, 0.08f, 0.08f));
    rootNodes.push_back(SceneNode(izakayaAwning, glm::translate(glm::mat4(1.0f), glm::vec3(-6.25f, 2.5f, -51.5f)) * glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(0.0f, 0.0f, 1.0f))));
    rootNodes.push_back(SceneNode(createBox(0.08f, 2.1f, 1.3f, glm::vec3(0.95f, 0.65f, 0.22f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.15f, -50.5f)), true));
    addDoorHandle(-6.40f, 1.15f, -50.0f, glm::vec3(0.18f, 0.16f, 0.15f));
    rootNodes.push_back(SceneNode(createBox(0.08f, 1.8f, 2.2f, glm::vec3(0.85f, 0.58f, 0.20f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.35f, -53.0f)), true));

    addWindow(-6.45f, 5.5f, -49.0f, 1.4f, 1.2f, true, glm::vec3(0.38f, 0.28f, 0.18f));
    addWindow(-6.45f, 5.5f, -52.0f, 1.4f, 1.2f, false, glm::vec3(0.0f));
    addWindow(-6.45f, 5.5f, -55.0f, 1.4f, 1.2f, true, glm::vec3(0.32f, 0.26f, 0.16f));

    rootNodes.push_back(SceneNode(createBox(0.12f, 1.0f, 3.2f, glm::vec3(0.18f, 0.10f, 0.08f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.44f, 3.7f, -51.5f))));
    addNeonBox(0.05f, 0.08f, 2.8f, glm::vec3(-6.40f, 4.05f, -51.5f), glm::vec3(1.0f, 0.25f, 0.15f));
    addNeonBox(0.05f, 0.08f, 2.8f, glm::vec3(-6.40f, 3.35f, -51.5f), glm::vec3(1.0f, 0.25f, 0.15f));
    addNeonBox(0.05f, 0.45f, 0.6f, glm::vec3(-6.40f, 3.7f, -52.4f), glm::vec3(1.0f, 0.75f, 0.2f));
    addNeonBox(0.05f, 0.45f, 0.6f, glm::vec3(-6.40f, 3.7f, -51.5f), glm::vec3(1.0f, 0.75f, 0.2f));
    addNeonBox(0.05f, 0.45f, 0.6f, glm::vec3(-6.40f, 3.7f, -50.6f), glm::vec3(1.0f, 0.75f, 0.2f));

    auto izakayaLantern = Model::loadOBJ("assets/models/chochin_lantern.obj", glm::vec3(0.95f, 0.18f, 0.10f));
    if (izakayaLantern) {
        for (float lz : { -49.4f, -51.5f, -54.4f }) {
            glm::mat4 lTr = glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.95f, lz));
            rootNodes.push_back(SceneNode(izakayaLantern, lTr, true, false, nullptr, false, 2.2f));
        }
    }
    addPoint(glm::vec3(-5.5f, 2.2f, -51.5f), glm::vec3(1.0f, 0.65f, 0.22f), 0.05f, 1.8f, 1.5f, 0.18f, 0.09f);

    // Apartment block
    rootNodes.push_back(SceneNode(createBox(8.0f, 13.5f, 7.5f, glm::vec3(0.18f, 0.19f, 0.21f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 6.75f, -39.75f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.35f, 7.7f, glm::vec3(0.13f, 0.14f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 13.65f, -39.75f))));
    rootNodes.push_back(SceneNode(createBox(0.08f, 2.2f, 1.4f, glm::vec3(0.22f, 0.23f, 0.25f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.1f, -38.0f))));
    addDoorHandle(-6.40f, 1.10f, -37.5f, glm::vec3(0.85f, 0.88f, 0.90f));
    addNeonBox(0.06f, 0.15f, 0.25f, glm::vec3(-6.42f, 2.4f, -38.0f), glm::vec3(0.9f, 0.95f, 1.0f));
    addWindow(-6.45f, 4.8f, -38.2f, 1.5f, 1.2f, true, glm::vec3(0.35f, 0.28f, 0.18f));
    addWindow(-6.45f, 4.8f, -41.2f, 1.5f, 1.2f, false, glm::vec3(0.0f));
    addWindow(-6.45f, 7.8f, -38.2f, 1.5f, 1.2f, false, glm::vec3(0.0f));
    addWindow(-6.45f, 7.8f, -41.2f, 1.5f, 1.2f, true, glm::vec3(0.22f, 0.28f, 0.32f));
    addWindow(-6.45f, 10.8f, -38.2f, 1.5f, 1.2f, true, glm::vec3(0.32f, 0.25f, 0.20f));
    addWindow(-6.45f, 10.8f, -41.2f, 1.5f, 1.2f, false, glm::vec3(0.0f));

    // Parking alcove
    rootNodes.push_back(SceneNode(createBox(5.0f, 5.5f, 6.0f, glm::vec3(0.17f, 0.17f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(-9.0f, 2.75f, -33.0f))));
    addRibbedShutter(rootNodes, -6.48f, 1.2f, -33.0f, 2.8f, 2.4f, 12);
    auto alcoveBayLine = createPlane(2.2f, 0.10f, glm::vec3(0.9f, 0.8f, 0.2f));
    rootNodes.push_back(SceneNode(alcoveBayLine, glm::translate(glm::mat4(1.0f), glm::vec3(-5.2f, 0.007f, -31.6f))));
    rootNodes.push_back(SceneNode(alcoveBayLine, glm::translate(glm::mat4(1.0f), glm::vec3(-5.2f, 0.007f, -34.4f))));
    rootNodes.push_back(SceneNode(createBox(0.22f, 0.12f, 1.8f, glm::vec3(0.9f, 0.75f, 0.12f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.1f, 0.06f, -33.0f))));

    // Pencil building
    rootNodes.push_back(SceneNode(createBox(8.0f, 14.0f, 7.0f, glm::vec3(0.15f, 0.16f, 0.18f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 7.0f, 21.0f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.35f, 7.2f, glm::vec3(0.12f, 0.13f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 14.15f, 21.0f))));

    // Pharmacy
    rootNodes.push_back(SceneNode(createBox(8.0f, 8.5f, 13.5f, glm::vec3(0.17f, 0.18f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 4.25f, 31.25f))));
    rootNodes.push_back(SceneNode(createBox(0.7f, 0.1f, 4.5f, glm::vec3(0.22f, 0.22f, 0.24f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.25f, 2.6f, 31.0f))));
    rootNodes.push_back(SceneNode(createBox(0.08f, 2.1f, 1.4f, glm::vec3(0.25f, 0.92f, 0.55f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.15f, 30.0f)), true));
    addDoorHandle(-6.40f, 1.15f, 30.5f, glm::vec3(0.88f, 0.90f, 0.92f));
    rootNodes.push_back(SceneNode(createBox(0.08f, 1.9f, 2.6f, glm::vec3(0.80f, 0.90f, 0.85f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.45f, 1.35f, 33.0f)), true));

    // Pharmacy sign
    rootNodes.push_back(SceneNode(createBox(0.10f, 1.5f, 1.5f, glm::vec3(0.12f, 0.12f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(-6.43f, 3.6f, 31.0f))));
    addNeonBox(0.06f, 1.1f, 0.34f, glm::vec3(-6.39f, 3.6f, 31.0f), glm::vec3(0.15f, 1.0f, 0.45f));
    addNeonBox(0.06f, 0.34f, 1.1f, glm::vec3(-6.39f, 3.6f, 31.0f), glm::vec3(0.15f, 1.0f, 0.45f));

    ShopBuilder::buildDetailedVendingMachine(rootNodes, glm::vec3(-6.02f, 0.0f, 38.0f), 90.0f, glm::vec3(0.38f, 0.09f, 0.12f));
    addPoint(glm::vec3(-5.5f, 2.2f, 31.0f), glm::vec3(0.25f, 0.90f, 0.55f), 0.05f, 1.8f, 1.5f, 0.18f, 0.09f);

    // End cap
    rootNodes.push_back(SceneNode(createBox(8.0f, 7.0f, 6.0f, glm::vec3(0.16f, 0.16f, 0.18f)), glm::translate(glm::mat4(1.0f), glm::vec3(-10.5f, 3.5f, 41.0f))));

    // End cap
    rootNodes.push_back(SceneNode(createBox(8.0f, 8.0f, 16.0f, glm::vec3(0.18f, 0.18f, 0.20f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 4.0f, -66.0f))));

    // Hotel tower
    rootNodes.push_back(SceneNode(createBox(8.0f, 22.0f, 12.0f, glm::vec3(0.16f, 0.17f, 0.20f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 11.0f, -52.0f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.40f, 12.2f, glm::vec3(0.13f, 0.14f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 22.2f, -52.0f))));
    rootNodes.push_back(SceneNode(createBox(0.6f, 0.6f, 0.6f, glm::vec3(1.0f, 0.15f, 0.15f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 22.5f, -52.0f)), true));

    rootNodes.push_back(SceneNode(createBox(0.08f, 2.2f, 1.6f, glm::vec3(0.75f, 0.88f, 0.98f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.2f, -53.0f)), true));
    addDoorHandle(6.40f, 1.20f, -52.85f, glm::vec3(0.88f, 0.74f, 0.28f));
    addDoorHandle(6.40f, 1.20f, -53.15f, glm::vec3(0.88f, 0.74f, 0.28f));
    rootNodes.push_back(SceneNode(createBox(0.08f, 1.8f, 2.4f, glm::vec3(0.45f, 0.70f, 0.85f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.35f, -55.8f)), true));
    ShopBuilder::buildDetailedVendingMachine(rootNodes, glm::vec3(6.02f, 0.0f, -49.0f), -90.0f, glm::vec3(0.08f, 0.14f, 0.28f));
    addPoint(glm::vec3(5.5f, 2.2f, -53.0f), glm::vec3(0.5f, 0.85f, 1.0f), 0.05f, 1.8f, 1.5f, 0.18f, 0.09f);

    for (float wy : { 5.5f, 9.5f, 13.5f, 17.5f }) {
        addWindow(6.45f, wy, -51.0f, 1.5f, 1.3f, (wy == 9.5f || wy == 17.5f), glm::vec3(0.20f, 0.32f, 0.38f));
        addWindow(6.45f, wy, -54.5f, 1.5f, 1.3f, (wy == 5.5f || wy == 13.5f), glm::vec3(0.35f, 0.30f, 0.22f));
    }

    // Hotel neon sign
    float hX = 5.86f, hZ = -47.0f;
    for (float sy : { 12.5f, 16.0f, 19.5f }) {
        rootNodes.push_back(SceneNode(createBox(0.50f, 0.08f, 0.08f, glm::vec3(0.20f, 0.20f, 0.22f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.20f, sy, hZ))));
    }
    rootNodes.push_back(SceneNode(createBox(0.08f, 8.2f, 1.3f, glm::vec3(0.08f, 0.08f, 0.10f)), glm::translate(glm::mat4(1.0f), glm::vec3(5.95f, 16.0f, hZ))));

    glm::vec3 hotelNeonCyan(0.05f, 1.0f, 0.95f);
    addNeonBox(0.04f, 8.2f, 0.06f, glm::vec3(5.90f, 16.0f, hZ - 0.62f), hotelNeonCyan * 0.85f);
    addNeonBox(0.04f, 8.2f, 0.06f, glm::vec3(5.90f, 16.0f, hZ + 0.62f), hotelNeonCyan * 0.85f);
    addNeonBox(0.04f, 0.06f, 1.3f,  glm::vec3(5.90f, 20.1f, hZ),          hotelNeonCyan * 0.85f);
    addNeonBox(0.04f, 0.06f, 1.3f,  glm::vec3(5.90f, 11.9f, hZ),          hotelNeonCyan * 0.85f);

    float lThick = 0.08f, lSpan = 0.15f;
    // H
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 19.2f, hZ - 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 19.2f, hZ + 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 19.2f, hZ), hotelNeonCyan);
    // O
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 17.6f, hZ - 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 17.6f, hZ + 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 18.04f, hZ), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 17.16f, hZ), hotelNeonCyan);
    // T
    addNeonBox(lThick, 0.14f, 0.72f, glm::vec3(hX, 16.44f, hZ), hotelNeonCyan);
    addNeonBox(lThick, 0.88f, lSpan, glm::vec3(hX, 15.94f, hZ), hotelNeonCyan);
    // E
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 14.4f, hZ - 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 14.84f, hZ - 0.06f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.44f, glm::vec3(hX, 14.40f, hZ - 0.10f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 13.96f, hZ - 0.06f), hotelNeonCyan);
    // L
    addNeonBox(lThick, 1.0f, lSpan, glm::vec3(hX, 12.8f, hZ - 0.32f), hotelNeonCyan);
    addNeonBox(lThick, 0.14f, 0.52f, glm::vec3(hX, 12.36f, hZ - 0.06f), hotelNeonCyan);

    addPoint(glm::vec3(5.2f, 16.0f, hZ), hotelNeonCyan, 0.05f, 1.8f, 1.5f, 0.16f, 0.08f);

    // Office tower
    rootNodes.push_back(SceneNode(createBox(8.0f, 16.5f, 13.5f, glm::vec3(0.14f, 0.15f, 0.18f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 8.25f, -36.75f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.40f, 13.7f, glm::vec3(0.11f, 0.12f, 0.14f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 16.65f, -36.75f))));
    rootNodes.push_back(SceneNode(createBox(0.08f, 2.3f, 1.6f, glm::vec3(0.20f, 0.22f, 0.25f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.15f, -33.5f))));
    addDoorHandle(6.40f, 1.15f, -33.0f, glm::vec3(0.85f, 0.88f, 0.90f));
    for (float wy : { 5.0f, 8.2f, 11.4f, 14.6f }) {
        addWindow(6.45f, wy, -35.0f, 1.5f, 1.2f, (wy == 8.2f || wy == 14.6f), glm::vec3(0.35f, 0.30f, 0.22f));
        addWindow(6.45f, wy, -38.5f, 1.5f, 1.2f, (wy == 5.0f || wy == 11.4f), glm::vec3(0.22f, 0.28f, 0.32f));
        addWindow(6.45f, wy, -41.5f, 1.5f, 1.2f, false, glm::vec3(0.0f));
    }

    // Commercial arcade
    rootNodes.push_back(SceneNode(createBox(8.0f, 11.5f, 13.3f, glm::vec3(0.20f, 0.19f, 0.22f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 5.75f, 17.85f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.35f, 13.5f, glm::vec3(0.14f, 0.14f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 11.65f, 17.85f))));
    // Double glass entrance portal and door leaves
    rootNodes.push_back(SceneNode(createBox(0.12f, 2.4f, 2.8f, glm::vec3(0.12f, 0.13f, 0.15f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.25f, 17.85f))));
    for (float dz : { 17.2f, 18.5f }) {
        rootNodes.push_back(SceneNode(createBox(0.08f, 2.2f, 1.25f, glm::vec3(0.18f, 0.20f, 0.23f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.15f, dz))));
        rootNodes.push_back(SceneNode(createBox(0.05f, 1.6f, 0.85f, glm::vec3(0.25f, 0.32f, 0.38f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.41f, 1.45f, dz))));
    }
    addDoorHandle(6.38f, 1.15f, 17.75f, glm::vec3(0.85f, 0.88f, 0.90f));
    addDoorHandle(6.38f, 1.15f, 17.95f, glm::vec3(0.85f, 0.88f, 0.90f));
    for (float wy : { 5.5f, 8.8f }) {
        addWindow(6.45f, wy, 15.0f, 1.8f, 1.3f, true, glm::vec3(0.32f, 0.28f, 0.18f));
        addWindow(6.45f, wy, 18.0f, 1.8f, 1.3f, false, glm::vec3(0.0f));
        addWindow(6.45f, wy, 21.0f, 1.8f, 1.3f, true, glm::vec3(0.25f, 0.30f, 0.35f));
    }

    // Bookstore building
    rootNodes.push_back(SceneNode(createBox(8.0f, 9.5f, 13.5f, glm::vec3(0.20f, 0.16f, 0.15f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 4.75f, 31.25f))));
    rootNodes.push_back(SceneNode(createBox(8.2f, 0.35f, 13.7f, glm::vec3(0.14f, 0.12f, 0.11f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 9.65f, 31.25f))));
    // Street entrance awning, glowing wood door, and illuminated display windows
    auto bookAwning = createBox(0.9f, 0.12f, 4.5f, glm::vec3(0.14f, 0.14f, 0.16f));
    rootNodes.push_back(SceneNode(bookAwning, glm::translate(glm::mat4(1.0f), glm::vec3(6.25f, 2.5f, 31.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(15.0f), glm::vec3(0.0f, 0.0f, 1.0f))));
    rootNodes.push_back(SceneNode(createBox(0.08f, 2.1f, 1.4f, glm::vec3(0.92f, 0.80f, 0.55f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.45f, 1.15f, 28.0f)), true));
    addDoorHandle(6.40f, 1.15f, 28.5f, glm::vec3(0.45f, 0.32f, 0.18f));
    addWindow(6.45f, 5.8f, 29.0f, 1.5f, 1.2f, true, glm::vec3(0.35f, 0.28f, 0.18f));
    addWindow(6.45f, 5.8f, 33.5f, 1.5f, 1.2f, false, glm::vec3(0.0f));

    // Fascia neon sign backplate mounted above awning
    rootNodes.push_back(SceneNode(createBox(0.10f, 1.1f, 4.2f, glm::vec3(0.14f, 0.14f, 0.16f)), glm::translate(glm::mat4(1.0f), glm::vec3(6.43f, 3.65f, 31.0f))));

    // End cap
    rootNodes.push_back(SceneNode(createBox(8.0f, 7.5f, 6.0f, glm::vec3(0.17f, 0.17f, 0.19f)), glm::translate(glm::mat4(1.0f), glm::vec3(10.5f, 3.75f, 41.0f))));

    // Street-level props: construction road closures, building entrances, alley clutter, upper windows
    StreetProps::buildConstructionBarriers(rootNodes, pointLights);
    StreetProps::buildAlleyClutter(rootNodes);
    StreetProps::buildBuildingEntrances(rootNodes, pointLights);
    StreetProps::buildUpperWindows(rootNodes);
}
