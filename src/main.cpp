#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>

#include "Shader.h"
#include "Scene.h"
#include "Camera.h"
#include "Audio.h"
#include "Rain.h"
#include "Cinematic.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int currentFbWidth = SCR_WIDTH;
int currentFbHeight = SCR_HEIGHT;

Camera camera(glm::vec3(0.0f, PLAYER_HEIGHT, 15.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cursorLocked = true;
bool flashlightOn = false;
float flashlightBrightness = 1.0f;
bool fogEnabled = true;
int rainIntensity = 1;
static bool cullingEnabled = false;
static Scene* g_scene = nullptr;
static Cinematic* g_cinematic = nullptr;
static Rain* g_rain = nullptr;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    currentFbWidth = width;
    currentFbHeight = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    if (g_cinematic && g_cinematic->isCutsceneActive()) return;

    if (cursorLocked || glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;
    if (action == GLFW_PRESS && (key == GLFW_KEY_C || key == GLFW_KEY_TAB)) {
        cursorLocked = !cursorLocked;
        if (cursorLocked) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            firstMouse = true;
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_F) {
        flashlightOn = !flashlightOn;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_G) {
        fogEnabled = !fogEnabled;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_H) {
        if (g_scene) {
            g_scene->triggerFlickerSurge();
        }
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_B) {
        if (g_cinematic) {
            g_cinematic->toggleBlackout();
        }
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) {
        if (g_cinematic) {
            g_cinematic->toggleCutscene(camera.Position, camera.Yaw, camera.Pitch);
        }
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_R) {
        rainIntensity = (rainIntensity + 1) % 4;
        if (g_rain) {
            g_rain->setIntensity(rainIntensity);
        }
        float vol = (rainIntensity == 1) ? 0.35f : ((rainIntensity == 2) ? 0.55f : ((rainIntensity == 3) ? 0.80f : 0.0f));
        Audio::setRainVolume(vol);
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_K) {
        cullingEnabled = !cullingEnabled;
        if (cullingEnabled) {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        } else {
            glDisable(GL_CULL_FACE);
        }
    }
    if (action == GLFW_PRESS && (key == GLFW_KEY_LEFT_BRACKET || key == GLFW_KEY_MINUS)) {
        flashlightBrightness = std::max(0.4f, flashlightBrightness - 0.3f);
    }
    if (action == GLFW_PRESS && (key == GLFW_KEY_RIGHT_BRACKET || key == GLFW_KEY_EQUAL)) {
        flashlightBrightness = std::min(3.0f, flashlightBrightness + 0.3f);
    }
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (g_cinematic && g_cinematic->isCutsceneActive()) {
            g_cinematic->stopCutscene();
            camera.setPose(g_cinematic->getSavedPlayerPosition(),
                           g_cinematic->getSavedPlayerYaw(),
                           g_cinematic->getSavedPlayerPitch());
        } else {
            glfwSetWindowShouldClose(window, true);
        }
    }

    float flicker = g_scene ? g_scene->getNeonFlicker() : 1.0f;
    float barFlicker = g_scene ? g_scene->getBarFlicker() : 1.0f;
    if (g_cinematic && g_cinematic->isCutsceneActive()) {
        Audio::update(false, false, deltaTime, camera.Position, camera.Front, camera.Right, flicker, barFlicker);
        return;
    }

    bool isSprinting = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
    camera.MovementSpeed = isSprinting ? 8.5f : SPEED;

    bool isWalking = false;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
        isWalking = true;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        isWalking = true;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
        isWalking = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
        isWalking = true;
    }
    Audio::update(isWalking, isSprinting, deltaTime, camera.Position, camera.Front, camera.Right, flicker, barFlicker);
    camera.update(deltaTime);

    float rotSpeed = 75.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera.ProcessMouseMovement(-rotSpeed * 10.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera.ProcessMouseMovement(rotSpeed * 10.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, rotSpeed * 8.0f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camera.ProcessMouseMovement(0.0f, -rotSpeed * 8.0f);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, 
        "Neon Night Street", 
        nullptr, nullptr
    );
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vertical sync to match display refresh rate and prevent busy loop
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    std::cout << "[OpenGL] Renderer: " << (renderer ? reinterpret_cast<const char*>(renderer) : "Unknown") << std::endl;
    std::cout << "[OpenGL] Version:  " << (version  ? reinterpret_cast<const char*>(version)  : "Unknown") << std::endl;

    // Query physical pixel framebuffer dimensions (fixes Wayland / High-DPI scaling)
    glfwGetFramebufferSize(window, &currentFbWidth, &currentFbHeight);
    glViewport(0, 0, currentFbWidth, currentFbHeight);

    glEnable(GL_DEPTH_TEST);

    Shader sceneShader("shaders/scene.vert", "shaders/scene.frag");

    Cinematic cinematic;
    g_cinematic = &cinematic;

    Scene scene;
    scene.init();
    scene.setCinematic(&cinematic);
    g_scene = &scene;

    sceneShader.use();
    scene.setupLights(sceneShader);

    Audio::init();
    Rain rain(6000);
    g_rain = &rain;

    double lastFpsTime = glfwGetTime();
    int frameCount = 0;
    lastFrame = static_cast<float>(glfwGetTime());

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if (deltaTime > 0.05f) deltaTime = 0.05f;

        frameCount++;
        if (currentFrame - lastFpsTime >= 0.5) {
            double fps = frameCount / (currentFrame - lastFpsTime);
            std::string title = "Neon Night Street (Topic 21) | FPS: " + std::to_string(static_cast<int>(fps))
                              + (cullingEnabled ? " | CULL: ON" : " | CULL: OFF");
            glfwSetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastFpsTime = currentFrame;
        }

        cinematic.update(deltaTime);
        static bool s_wasCutscene = false;
        bool isCutscene = cinematic.isCutsceneActive();
        if (isCutscene) {
            camera.setPose(cinematic.getCameraPosition(),
                           cinematic.getCameraYaw(),
                           cinematic.getCameraPitch());
        } else if (s_wasCutscene) {
            camera.setPose(cinematic.getSavedPlayerPosition(),
                           cinematic.getSavedPlayerYaw(),
                           cinematic.getSavedPlayerPitch());
        }
        s_wasCutscene = isCutscene;

        processInput(window);

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sceneShader.use();
        sceneShader.setVec3("viewPos", camera.Position);
        sceneShader.setBool("fogEnabled", fogEnabled);

        sceneShader.setBool("spotLight.enabled", flashlightOn);
        if (flashlightOn) {
            sceneShader.setVec3("spotLight.position", camera.Position);
            sceneShader.setVec3("spotLight.direction", camera.Front);
            sceneShader.setVec3("spotLight.ambient", glm::vec3(0.0f));
            sceneShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 0.98f, 0.92f) * (3.8f * flashlightBrightness));
            sceneShader.setVec3("spotLight.specular", glm::vec3(1.0f, 0.98f, 0.92f) * (3.0f * flashlightBrightness));
            sceneShader.setFloat("spotLight.constant", 1.0f);
            sceneShader.setFloat("spotLight.linear", 0.09f);
            sceneShader.setFloat("spotLight.quadratic", 0.032f);
            sceneShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            sceneShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
        }

        glm::mat4 view = camera.GetViewMatrix();
        float aspect = (currentFbHeight > 0) 
            ? static_cast<float>(currentFbWidth) / static_cast<float>(currentFbHeight) 
            : 16.0f / 9.0f;
        glm::mat4 projection = glm::perspective(glm::radians(60.0f), aspect, 0.1f, 600.0f);

        sceneShader.setMat4("view", view);
        sceneShader.setMat4("projection", projection);

        bool isBlackout = cinematic.isBlackout();
        Audio::setBlackout(isBlackout);

        scene.update(deltaTime);
        scene.render(sceneShader, camera.Position, view, projection, 0.0f, fogEnabled, flashlightOn, camera.Front);

        if (rain.getIntensity() > 0) {
            rain.update(deltaTime, camera.Position);
            float rainLight = isBlackout ? cinematic.getDirLightMultiplier() : 1.0f;
            rain.render(view, projection, rainLight, flashlightOn, camera.Position, camera.Front, flashlightBrightness);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    Audio::cleanup();
    glfwTerminate();
    return 0;
}
