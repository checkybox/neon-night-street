#include "HotelBuilder.h"
#include "CityBuilder.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.hpp>

void HotelBuilder::buildHotel(std::vector<SceneNode>& rootNodes,
                             std::vector<PointLight>& pointLights) {
    auto addBox = [&](float w, float h, float d, const glm::vec3& col, const glm::vec3& pos, bool emissive = false, float boost = 1.0f) {
        rootNodes.push_back(SceneNode(CityBuilder::createBox(w, h, d, col), glm::translate(glm::mat4(1.0f), pos), emissive, false, nullptr, false, boost));
    };

    float hX = 10.25f, hY = 4.5f, hZ = -22.0f;
    float frontX = 6.45f;
    float sideZ = -14.0f;

    // Main hotel mass
    addBox(7.5f, 9.0f, 16.0f, glm::vec3(0.20f, 0.18f, 0.16f), glm::vec3(hX, hY, hZ));

    // Dark polished granite base with brass cap molding divided into piers avoiding both door openings
    glm::vec3 hPlinthCol(0.08f, 0.08f, 0.09f);
    glm::vec3 hTrimCol(0.65f, 0.52f, 0.25f);
    // West face plinths
    addBox(0.12f, 0.85f, 6.65f, hPlinthCol, glm::vec3(frontX + 0.03f, 0.425f, -26.675f));
    addBox(0.14f, 0.04f, 6.65f, hTrimCol,   glm::vec3(frontX - 0.02f, 0.87f,  -26.675f));
    addBox(0.12f, 0.85f, 6.65f, hPlinthCol, glm::vec3(frontX + 0.03f, 0.425f, -17.325f));
    addBox(0.14f, 0.04f, 6.65f, hTrimCol,   glm::vec3(frontX - 0.02f, 0.87f,  -17.325f));

    // South face plinths
    addBox(2.80f, 0.85f, 0.12f, hPlinthCol, glm::vec3(7.90f,  0.425f, sideZ + 0.03f));
    addBox(2.80f, 0.04f, 0.14f, hTrimCol,   glm::vec3(7.90f,  0.87f,  sideZ - 0.02f));
    addBox(3.30f, 0.85f, 0.12f, hPlinthCol, glm::vec3(12.35f, 0.425f, sideZ + 0.03f));
    addBox(3.30f, 0.04f, 0.14f, hTrimCol,   glm::vec3(12.35f, 0.87f,  sideZ - 0.02f));

    // Stone threshold slabs at sidewalk level
    addBox(0.20f, 0.04f, 2.70f, glm::vec3(0.28f, 0.26f, 0.22f), glm::vec3(frontX - 0.05f, 0.02f, hZ));
    addBox(1.50f, 0.04f, 0.20f, glm::vec3(0.24f, 0.24f, 0.26f), glm::vec3(10.0f, 0.02f, sideZ - 0.05f));

    // Main street entrance opposite the bar
    glm::vec3 lobbyGlow(1.0f, 0.88f, 0.65f);
    // Dark bronze double door portal frame
    addBox(0.12f, 2.70f, 2.60f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(frontX, 1.35f, hZ));
    // Glowing warm interior lobby double doors with standard lobby illumination
    addBox(0.06f, 2.45f, 1.10f, lobbyGlow * 0.90f, glm::vec3(frontX - 0.02f, 1.25f, hZ - 0.60f), true, 1.0f);
    addBox(0.06f, 2.45f, 1.10f, lobbyGlow * 0.90f, glm::vec3(frontX - 0.02f, 1.25f, hZ + 0.60f), true, 1.0f);
    // Vertical polished brass pull handles on both doors
    addBox(0.04f, 0.70f, 0.03f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(frontX - 0.06f, 1.25f, hZ - 0.10f));
    addBox(0.04f, 0.70f, 0.03f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(frontX - 0.06f, 1.25f, hZ + 0.10f));
    // Warm glowing transom window above double doors
    addBox(0.06f, 0.40f, 2.30f, lobbyGlow, glm::vec3(frontX - 0.02f, 2.70f, hZ), true, 1.0f);

    // Cantilevered entrance marquee canopy over main double doors
    addBox(0.55f, 0.08f, 3.00f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(frontX - 0.275f, 2.95f, hZ));
    addBox(0.03f, 0.10f, 3.04f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.55f, 2.95f, hZ));
    addBox(0.55f, 0.10f, 0.03f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.275f, 2.95f, hZ - 1.50f));
    addBox(0.55f, 0.10f, 0.03f, glm::vec3(0.85f, 0.70f, 0.25f), glm::vec3(frontX - 0.275f, 2.95f, hZ + 1.50f));
    // Warm recessed canopy downlight
    addBox(0.35f, 0.02f, 1.00f, glm::vec3(1.0f, 0.90f, 0.70f), glm::vec3(frontX - 0.275f, 2.90f, hZ), true, 1.2f);

    // Side street entrance door
    addBox(1.30f, 2.45f, 0.10f, glm::vec3(0.12f, 0.12f, 0.14f), glm::vec3(10.0f, 1.25f, sideZ));
    addBox(1.10f, 2.35f, 0.06f, glm::vec3(0.18f, 0.14f, 0.12f), glm::vec3(10.0f, 1.25f, sideZ - 0.02f));
    addBox(0.60f, 0.90f, 0.07f, lobbyGlow * 0.85f, glm::vec3(10.0f, 1.65f, sideZ - 0.03f), true, 1.0f);
    addBox(0.12f, 0.03f, 0.04f, glm::vec3(0.85f, 0.72f, 0.25f), glm::vec3(9.60f, 1.15f, sideZ - 0.06f));
    // Overhead illuminated sconce above side door
    addBox(0.20f, 0.08f, 0.15f, glm::vec3(0.65f, 0.52f, 0.25f), glm::vec3(10.0f, 2.60f, sideZ - 0.08f));
    addBox(0.14f, 0.03f, 0.10f, glm::vec3(1.0f, 0.85f, 0.55f), glm::vec3(10.0f, 2.55f, sideZ - 0.08f), true, 1.2f);

    // Ground floor glowing windows
    float groundWinZs[] = { -26.2f, -17.8f, -15.4f };
    for (float wz : groundWinZs) {
        addBox(0.08f, 1.80f, 1.60f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 1.85f, wz));
        addBox(0.05f, 1.66f, 1.46f, lobbyGlow * 0.88f, glm::vec3(frontX - 0.02f, 1.85f, wz), true, 1.0f);
        addBox(0.06f, 1.66f, 0.04f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(frontX - 0.03f, 1.85f, wz));
        addBox(0.06f, 0.04f, 1.46f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(frontX - 0.03f, 1.85f, wz));
    }
    // Side street ground windows
    float sideWinXs[] = { 7.8f, 12.2f };
    for (float wx : sideWinXs) {
        addBox(1.50f, 1.80f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 1.85f, sideZ));
        addBox(1.36f, 1.66f, 0.05f, lobbyGlow * 0.85f, glm::vec3(wx, 1.85f, sideZ - 0.02f), true, 1.0f);
        addBox(0.04f, 1.66f, 0.06f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(wx, 1.85f, sideZ - 0.03f));
        addBox(1.36f, 0.04f, 0.06f, glm::vec3(0.45f, 0.38f, 0.22f), glm::vec3(wx, 1.85f, sideZ - 0.03f));
    }

    // Upper floor windows with selective illumination
    glm::vec3 darkWinCol(0.08f, 0.09f, 0.11f);
    glm::vec3 litWinCol(0.95f, 0.80f, 0.45f);

    float upperZs[] = { -27.5f, -24.8f, -22.0f, -19.2f, -16.5f };
    for (float wz : upperZs) {
        bool litF2 = (wz == -24.8f || wz == -19.2f);
        addBox(0.08f, 1.40f, 1.20f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 4.6f, wz));
        addBox(0.05f, 1.26f, 1.06f, litF2 ? litWinCol : darkWinCol, glm::vec3(frontX - 0.02f, 4.6f, wz), litF2, 1.0f);
        addBox(0.06f, 1.26f, 0.03f, glm::vec3(0.25f, 0.22f, 0.20f), glm::vec3(frontX - 0.03f, 4.6f, wz));

        bool litF3 = (wz == -27.5f || wz == -16.5f);
        addBox(0.08f, 1.40f, 1.20f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(frontX, 7.0f, wz));
        addBox(0.05f, 1.26f, 1.06f, litF3 ? litWinCol : darkWinCol, glm::vec3(frontX - 0.02f, 7.0f, wz), litF3, 1.0f);
        addBox(0.06f, 1.26f, 0.03f, glm::vec3(0.25f, 0.22f, 0.20f), glm::vec3(frontX - 0.03f, 7.0f, wz));
    }

    // Upper floor side windows
    for (float wx : sideWinXs) {
        bool litS2 = (wx == 7.8f);
        addBox(1.20f, 1.40f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 4.6f, sideZ));
        addBox(1.06f, 1.26f, 0.05f, litS2 ? litWinCol : darkWinCol, glm::vec3(wx, 4.6f, sideZ - 0.02f), litS2, 1.0f);

        bool litS3 = (wx == 12.2f);
        addBox(1.20f, 1.40f, 0.08f, glm::vec3(0.14f, 0.13f, 0.12f), glm::vec3(wx, 7.0f, sideZ));
        addBox(1.06f, 1.26f, 0.05f, litS3 ? litWinCol : darkWinCol, glm::vec3(wx, 7.0f, sideZ - 0.02f), litS3, 1.0f);
    }

    // Neon hotel sign overhead with vivid emissive boost
    auto hotelNeonMesh = Model::loadOBJ("assets/models/neon_sign_hotel.obj", glm::vec3(1.0f, 0.65f, 0.15f));
    if (hotelNeonMesh) {
        glm::mat4 hotelTr = glm::translate(glm::mat4(1.0f), glm::vec3(frontX - 0.08f, 3.55f, hZ))
                          * glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        rootNodes.push_back(SceneNode(hotelNeonMesh, hotelTr, true, false, nullptr, false, 2.85f));
    }

    // Warm hotel lobby entrance point light
    pointLights.push_back({ glm::vec3(5.5f, 2.2f, hZ), glm::vec3(1.0f, 0.85f, 0.55f) * 0.06f,
                            glm::vec3(1.0f, 0.85f, 0.55f) * 2.2f, glm::vec3(1.0f, 0.85f, 0.55f) * 1.8f,
                            1.0f, 0.18f, 0.09f });
}
