#include "DecalBuilder.h"
#include "CityBuilder.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

static std::shared_ptr<Mesh> createQuad(float width, float height, const glm::vec3& normal) {
    std::vector<Vertex> verts;
    std::vector<unsigned int> idxs = { 0, 1, 2,  0, 2, 3 };
    glm::vec3 c(1.0f);

    if (normal.x > 0.5f) {
        // Facing positive x (west wall facing into street)
        verts.push_back({ glm::vec3(0.0f, -height*0.5f,  width*0.5f), normal, c, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ glm::vec3(0.0f, -height*0.5f, -width*0.5f), normal, c, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ glm::vec3(0.0f,  height*0.5f, -width*0.5f), normal, c, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ glm::vec3(0.0f,  height*0.5f,  width*0.5f), normal, c, glm::vec2(0.0f, 1.0f) });
    } else if (normal.x < -0.5f) {
        // Facing negative x (east wall facing into street)
        verts.push_back({ glm::vec3(0.0f, -height*0.5f, -width*0.5f), normal, c, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ glm::vec3(0.0f, -height*0.5f,  width*0.5f), normal, c, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ glm::vec3(0.0f,  height*0.5f,  width*0.5f), normal, c, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ glm::vec3(0.0f,  height*0.5f, -width*0.5f), normal, c, glm::vec2(0.0f, 1.0f) });
    } else if (normal.z > 0.5f) {
        // Facing positive z (north wall facing south)
        verts.push_back({ glm::vec3(-width*0.5f, -height*0.5f, 0.0f), normal, c, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ glm::vec3( width*0.5f, -height*0.5f, 0.0f), normal, c, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ glm::vec3( width*0.5f,  height*0.5f, 0.0f), normal, c, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ glm::vec3(-width*0.5f,  height*0.5f, 0.0f), normal, c, glm::vec2(0.0f, 1.0f) });
    } else if (normal.z < -0.5f) {
        // Facing negative z (south wall facing north)
        verts.push_back({ glm::vec3( width*0.5f, -height*0.5f, 0.0f), normal, c, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ glm::vec3(-width*0.5f, -height*0.5f, 0.0f), normal, c, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ glm::vec3(-width*0.5f,  height*0.5f, 0.0f), normal, c, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ glm::vec3( width*0.5f,  height*0.5f, 0.0f), normal, c, glm::vec2(0.0f, 1.0f) });
    } else {
        // Horizontal on ground facing positive y
        verts.push_back({ glm::vec3(-width*0.5f, 0.0f,  height*0.5f), glm::vec3(0.0f, 1.0f, 0.0f), c, glm::vec2(0.0f, 0.0f) });
        verts.push_back({ glm::vec3( width*0.5f, 0.0f,  height*0.5f), glm::vec3(0.0f, 1.0f, 0.0f), c, glm::vec2(1.0f, 0.0f) });
        verts.push_back({ glm::vec3( width*0.5f, 0.0f, -height*0.5f), glm::vec3(0.0f, 1.0f, 0.0f), c, glm::vec2(1.0f, 1.0f) });
        verts.push_back({ glm::vec3(-width*0.5f, 0.0f, -height*0.5f), glm::vec3(0.0f, 1.0f, 0.0f), c, glm::vec2(0.0f, 1.0f) });
    }
    return std::make_shared<Mesh>(verts, idxs);
}

void DecalBuilder::buildDecals(std::vector<SceneNode>& rootNodes,
                               std::vector<SceneNode>& decalNodes,
                               std::vector<PointLight>& pointLights) {
    auto texSkull   = std::make_shared<Texture>("assets/textures/graffiti_skull.png", true);
    auto texKanji   = std::make_shared<Texture>("assets/textures/graffiti_kanji.png", true);
    auto texCyber   = std::make_shared<Texture>("assets/textures/graffiti_cyber.png", true);
    auto texPost1   = std::make_shared<Texture>("assets/textures/poster_club.png", true);
    auto texPost2   = std::make_shared<Texture>("assets/textures/poster_warning.png", true);
    auto texLitter  = std::make_shared<Texture>("assets/textures/paper_litter.png", true);
    auto texBlade1  = std::make_shared<Texture>("assets/textures/blade_sign_capsule.png", true);
    auto texBlade2  = std::make_shared<Texture>("assets/textures/blade_sign_cyber.png", true);

    auto addDecal = [&](std::shared_ptr<Mesh> mesh, std::shared_ptr<Texture> tex, const glm::vec3& pos, float yaw = 0.0f, bool emissive = false) {
        glm::mat4 tr = glm::translate(glm::mat4(1.0f), pos);
        if (std::abs(yaw) > 0.001f) {
            tr = tr * glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        decalNodes.push_back(SceneNode(mesh, tr, emissive, false, tex));
    };

    // Transparent street graffiti decals placed flush against solid walls
    auto mSkullAlley = createQuad(1.8f, 1.8f, glm::vec3(0.0f, 0.0f, -1.0f));
    addDecal(mSkullAlley, texSkull, glm::vec3(8.5f, 2.1f, -41.98f));

    auto mSkullWest = createQuad(1.8f, 1.8f, glm::vec3(1.0f, 0.0f, 0.0f));
    addDecal(mSkullWest, texSkull, glm::vec3(-6.41f, 2.1f, -68.5f));

    auto mKanji = createQuad(1.85f, 1.85f, glm::vec3(1.0f, 0.0f, 0.0f));
    addDecal(mKanji, texKanji, glm::vec3(-6.41f, 2.2f, -40.5f));

    auto mKanjiNorth = createQuad(2.1f, 2.1f, glm::vec3(0.0f, 0.0f, 1.0f));
    addDecal(mKanjiNorth, texKanji, glm::vec3(-16.0f, 2.3f, -13.92f));

    auto mCyber = createQuad(2.5f, 1.5f, glm::vec3(1.0f, 0.0f, 0.0f));
    addDecal(mCyber, texCyber, glm::vec3(-18.42f, 2.1f, -3.0f));

    auto mCyberEast = createQuad(2.5f, 1.5f, glm::vec3(-1.0f, 0.0f, 0.0f));
    addDecal(mCyberEast, texCyber, glm::vec3(18.42f, 2.1f, -8.0f));

    // Weathered wall posters and notices
    auto mPostClub = createQuad(0.75f, 0.95f, glm::vec3(1.0f, 0.0f, 0.0f));
    addDecal(mPostClub, texPost1, glm::vec3(-6.41f, 1.7f, 7.3f));

    auto mPostClubEast = createQuad(0.75f, 0.95f, glm::vec3(-1.0f, 0.0f, 0.0f));
    addDecal(mPostClubEast, texPost1, glm::vec3(6.41f, 1.6f, 37.2f));

    auto mPostAlcove = createQuad(0.75f, 0.95f, glm::vec3(0.0f, 0.0f, 1.0f));
    addDecal(mPostAlcove, texPost1, glm::vec3(-7.5f, 1.8f, -2.46f));

    auto mPostWarnNorth = createQuad(0.70f, 0.90f, glm::vec3(0.0f, 0.0f, 1.0f));
    addDecal(mPostWarnNorth, texPost2, glm::vec3(-14.3f, 1.85f, -13.92f));

    auto mPostWarnEast = createQuad(0.70f, 0.90f, glm::vec3(-1.0f, 0.0f, 0.0f));
    addDecal(mPostWarnEast, texPost2, glm::vec3(6.41f, 1.75f, -63.2f));

    // Soaked paper street litter
    auto mLitter = createQuad(0.40f, 0.40f, glm::vec3(0.0f, 1.0f, 0.0f));
    addDecal(mLitter, texLitter, glm::vec3(-4.8f, 0.208f, -22.0f), 25.0f);
    addDecal(mLitter, texLitter, glm::vec3(-3.8f, 0.008f, -12.5f), -40.0f);
    addDecal(mLitter, texLitter, glm::vec3(4.8f, 0.208f, 5.0f), 65.0f);
    addDecal(mLitter, texLitter, glm::vec3(-8.5f, 0.208f, -2.8f), 15.0f);
    addDecal(mLitter, texLitter, glm::vec3(-16.5f, 0.008f, -11.5f), -80.0f);
    addDecal(mLitter, texLitter, glm::vec3(3.7f, 0.008f, -35.0f), 45.0f);

    // Vertical cyberpunk neon blade signs
    glm::vec3 trussCol(0.18f, 0.18f, 0.20f);
    glm::vec3 casingCol(0.12f, 0.12f, 0.14f);

    // Capsule Hotel blade sign mounted firmly to facade
    float b1X = 5.75f, b1Y = 6.40f, b1Z = -49.0f;
    // Structural steel cantilever wall mounting arms
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.85f, 0.10f, 0.10f, trussCol), glm::translate(glm::mat4(1.0f), glm::vec3(6.12f, b1Y + 1.4f, b1Z))));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.85f, 0.10f, 0.10f, trussCol), glm::translate(glm::mat4(1.0f), glm::vec3(6.12f, b1Y - 1.4f, b1Z))));
    // Main sign housing body
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.14f, 3.80f, 1.08f, casingCol), glm::translate(glm::mat4(1.0f), glm::vec3(b1X, b1Y, b1Z))));
    // Dual illuminated display faces
    auto mFaceEast = createQuad(1.02f, 3.72f, glm::vec3(-1.0f, 0.0f, 0.0f));
    auto mFaceWest = createQuad(1.02f, 3.72f, glm::vec3(1.0f, 0.0f, 0.0f));
    addDecal(mFaceEast, texBlade1, glm::vec3(b1X - 0.072f, b1Y, b1Z), 0.0f, true);
    addDecal(mFaceWest, texBlade1, glm::vec3(b1X + 0.072f, b1Y, b1Z), 0.0f, true);
    // Local intense magenta point light
    glm::vec3 magCol(1.0f, 0.15f, 0.65f);
    pointLights.push_back({ glm::vec3(5.15f, b1Y, b1Z), magCol * 0.05f, magCol * 1.6f, magCol * 1.2f, 1.0f, 0.16f, 0.08f });

    // Blade Sign 2: Cyber District (West side, mounted on solid corner pier at Z = 18.1f)
    float b2X = -5.75f, b2Y = 6.20f, b2Z = 18.10f;
    // Cantilever steel wall mounting arms
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.85f, 0.10f, 0.10f, trussCol), glm::translate(glm::mat4(1.0f), glm::vec3(-6.12f, b2Y + 1.3f, b2Z))));
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.85f, 0.10f, 0.10f, trussCol), glm::translate(glm::mat4(1.0f), glm::vec3(-6.12f, b2Y - 1.3f, b2Z))));
    // Main sign housing body
    rootNodes.push_back(SceneNode(CityBuilder::createBox(0.14f, 3.60f, 0.82f, casingCol), glm::translate(glm::mat4(1.0f), glm::vec3(b2X, b2Y, b2Z))));
    // Dual illuminated display faces
    auto mFaceWest2 = createQuad(0.78f, 3.52f, glm::vec3(1.0f, 0.0f, 0.0f));
    auto mFaceEast2 = createQuad(0.78f, 3.52f, glm::vec3(-1.0f, 0.0f, 0.0f));
    addDecal(mFaceWest2, texBlade2, glm::vec3(b2X + 0.072f, b2Y, b2Z), 0.0f, true);
    addDecal(mFaceEast2, texBlade2, glm::vec3(b2X - 0.072f, b2Y, b2Z), 0.0f, true);
    // Local intense electric cyan point light
    glm::vec3 cyanCol(0.0f, 0.90f, 1.0f);
    pointLights.push_back({ glm::vec3(-5.15f, b2Y, b2Z), cyanCol * 0.05f, cyanCol * 1.5f, cyanCol * 1.2f, 1.0f, 0.16f, 0.08f });
}
