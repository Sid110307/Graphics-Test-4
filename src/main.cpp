#include <iostream>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "include/shader.h"
#include "include/model.h"
#include "include/objects.h"
#include "include/camera.h"

constexpr GLint WIDTH = 1366, HEIGHT = 768;

GLdouble lastFrameTime = 0.0f;
glm::vec3 lightPosition = glm::vec3(0.0f), lightRotation = glm::vec3(0.0f), lightScale = glm::vec3(1.0f),
        lightColor = glm::vec3(1.0f);
const char* lightTypes[] = {"Point", "Directional", "Spot"};
GLint lightType = 0;
GLfloat spotLightAngle = 12.5f;
bool enableAmbientLight = true, enableDiffuseLight = true, enableSpecularLight = true;

ImGuiIO io;
Camera camera;

GLFWwindow* init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Graphics Test 4", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window!" << std::endl;
        glfwTerminate();

        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW!" << std::endl;
        glfwTerminate();

        exit(EXIT_FAILURE);
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO();
    static_cast<void>(io);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    return window;
}

namespace Callbacks
{
    bool wKeyHeld = false, sKeyHeld = false, aKeyHeld = false, dKeyHeld = false, eKeyHeld = false, qKeyHeld = false,
            lmbHeld = false;

    void keyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
    {
        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
        if (io.WantCaptureKeyboard) return;

        if (action == GLFW_PRESS)
        {
            if (key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, true);

            if (key == GLFW_KEY_W) wKeyHeld = true;
            if (key == GLFW_KEY_S) sKeyHeld = true;
            if (key == GLFW_KEY_A) aKeyHeld = true;
            if (key == GLFW_KEY_D) dKeyHeld = true;
            if (key == GLFW_KEY_E) eKeyHeld = true;
            if (key == GLFW_KEY_Q) qKeyHeld = true;
        } else if (action == GLFW_RELEASE)
        {
            if (key == GLFW_KEY_W) wKeyHeld = false;
            if (key == GLFW_KEY_S) sKeyHeld = false;
            if (key == GLFW_KEY_A) aKeyHeld = false;
            if (key == GLFW_KEY_D) dKeyHeld = false;
            if (key == GLFW_KEY_E) eKeyHeld = false;
            if (key == GLFW_KEY_Q) qKeyHeld = false;
        }

        if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) camera.resetProperties();
    }

    void cursorCallback(GLFWwindow* window, GLdouble xPos, GLdouble yPos)
    {
        ImGui_ImplGlfw_CursorPosCallback(window, xPos, yPos);
        if (io.WantCaptureMouse) return;

        static GLdouble lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
        static bool firstMouse = true;

        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        GLdouble xOffset = xPos - lastX;
        GLdouble yOffset = lastY - yPos;

        lastX = xPos;
        lastY = yPos;

        if (lmbHeld) camera.processMouseMovement(static_cast<GLfloat>(xOffset), static_cast<GLfloat>(yOffset));
    }

    void mouseButtonCallback(GLFWwindow* window, GLint button, GLint action, GLint mods)
    {
        ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
        if (io.WantCaptureMouse) return;

        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) lmbHeld = true;
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) lmbHeld = false;
    }

    void scrollCallback(GLFWwindow* window, GLdouble xOffset, GLdouble yOffset)
    {
        ImGui_ImplGlfw_ScrollCallback(window, xOffset, yOffset);
        if (io.WantCaptureMouse) return;

        camera.processMouseScroll(static_cast<GLfloat>(yOffset));
    }
}

void handleInput(GLFWwindow* window, GLdouble deltaTime)
{
    if (Callbacks::wKeyHeld) camera.processKeyboard(CameraMovement::FORWARD, deltaTime);
    if (Callbacks::sKeyHeld) camera.processKeyboard(CameraMovement::BACKWARD, deltaTime);
    if (Callbacks::aKeyHeld) camera.processKeyboard(CameraMovement::LEFT, deltaTime);
    if (Callbacks::dKeyHeld) camera.processKeyboard(CameraMovement::RIGHT, deltaTime);
    if (Callbacks::eKeyHeld) camera.processKeyboard(CameraMovement::UP, deltaTime);
    if (Callbacks::qKeyHeld) camera.processKeyboard(CameraMovement::DOWN, deltaTime);

    glfwSetKeyCallback(window, Callbacks::keyCallback);
    glfwSetCursorPosCallback(window, Callbacks::cursorCallback);
    glfwSetMouseButtonCallback(window, Callbacks::mouseButtonCallback);
    glfwSetScrollCallback(window, Callbacks::scrollCallback);
}

void renderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Graphics Test 4", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetWindowSize(ImVec2(WIDTH / 3.5f, HEIGHT));

    ImGui::SeparatorText("Controls");
    ImGui::Text("W: Move Forward");
    ImGui::Text("S: Move Backward");
    ImGui::Text("A: Move Left");
    ImGui::Text("D: Move Right");
    ImGui::Text("E: Move Up");
    ImGui::Text("Q: Move Down");
    ImGui::Text("LMB: Rotate Camera");
    ImGui::Text("Scroll Wheel: Zoom Camera");
    ImGui::Text("Space: Reset Camera");
    ImGui::Text("Escape: Exit");

    ImGui::SeparatorText("Camera Info");
    ImGui::Text("Camera Position: (%.1f, %.1f, %.1f)", camera.getPosition().x, camera.getPosition().y,
                camera.getPosition().z);
    ImGui::Text("Camera Orientation: (%.1f, %.1f, %.1f)", camera.getOrientation().x, camera.getOrientation().y,
                camera.getOrientation().z);
    ImGui::Text("Camera Up: (%.1f, %.1f, %.1f)", camera.getUp().x, camera.getUp().y, camera.getUp().z);
    ImGui::Text("Camera Pitch: %.1f", camera.pitch);
    ImGui::Text("Camera Yaw: %.1f", camera.yaw);

    ImGui::SeparatorText("Camera Settings");
    ImGui::SliderFloat("Mouse Sensitivity", &camera.mouseSensitivity, 0.0f, 1.0f);
    ImGui::SliderFloat("Movement Speed", &camera.movementSpeed, 0.0f, 100.0f);
    ImGui::SliderFloat("Field of View", &camera.fov, 0.0f, 180.0f);
    ImGui::SliderFloat("Near Plane", &camera.near, 0.0f, 1000.0f);
    ImGui::SliderFloat("Far Plane", &camera.far, 0.0f, 1000.0f);

    ImGui::SeparatorText("Lighting");
    ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPosition), -10.0f, 10.0f);
    ImGui::SliderFloat3("Light Rotation", glm::value_ptr(lightRotation), -360.0f, 360.0f);
    ImGui::SliderFloat3("Light Scale", glm::value_ptr(lightScale), 0.0f, 10.0f);
    ImGui::ColorEdit3("Light Color", glm::value_ptr(lightColor));
    ImGui::Checkbox("Enable Ambient Light", &enableAmbientLight);
    ImGui::Checkbox("Enable Diffuse Light", &enableDiffuseLight);
    ImGui::Checkbox("Enable Specular Light", &enableSpecularLight);
    ImGui::Combo("Light Type", &lightType, lightTypes, IM_ARRAYSIZE(lightTypes));
    if (lightType == 2) ImGui::SliderFloat("Spot Light Angle", &spotLightAngle, 0.0f, 90.0f);
    if (ImGui::Button("Reset Light"))
    {
        lightPosition = glm::vec3(0.0f);
        lightRotation = glm::vec3(0.0f);
        lightScale = glm::vec3(1.0f);
        lightColor = glm::vec3(1.0f);

        lightType = 0;
        spotLightAngle = 12.5f;

        enableAmbientLight = true;
        enableDiffuseLight = true;
        enableSpecularLight = true;
    }

    ImGui::SeparatorText("Info");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
    ImGui::Text("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
    ImGui::Text("ImGui Version: %s", IMGUI_VERSION);
    ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void renderGraphics(glm::mat4 &view, glm::mat4 &projection)
{
    {
        Shader defaultShader("lib/shaders/defaultVertex.glsl", "lib/shaders/defaultFragment.glsl");
        Model model("lib/models/cube.stl", defaultShader);

        defaultShader.use();
        defaultShader.setMatrices(view, projection);

        defaultShader.setVec3("lightColor", lightColor);
        defaultShader.setVec3("lightPos", lightPosition);
        defaultShader.setVec3("lightDirection", lightRotation);
        defaultShader.setVec3("viewPos", camera.getPosition());
        defaultShader.setVec3("objectColor", lightColor);
        defaultShader.setInt("lightType", lightType);
        defaultShader.setFloat("spotLightAngle", spotLightAngle);
        defaultShader.setBool("enableAmbientLight", enableAmbientLight);
        defaultShader.setBool("enableDiffuseLight", enableDiffuseLight);
        defaultShader.setBool("enableSpecularLight", enableSpecularLight);

        model.draw();
    }
    {
        Shader lightShader("lib/shaders/lightVertex.glsl", "lib/shaders/lightFragment.glsl");
        Cube light(lightShader);

        light.position = lightPosition;
        light.rotation = lightRotation;
        light.scale = lightScale;
        light.updateModel();

        lightShader.use();
        lightShader.setMatrices(view, projection);

        lightShader.setVec3("lightColor", lightColor);
        light.draw();
    }
    {
        Shader arrowShader("lib/shaders/defaultVertex.glsl", "lib/shaders/defaultFragment.glsl");

        arrowShader.use();
        arrowShader.setMatrices(view, projection);

        GLfloat vertices[] = {
                lightPosition.x, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x + 100.0f, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y + 100.0f, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y, lightPosition.z + 100.0f, lightColor.x, lightColor.y, lightColor.z,

                lightPosition.x + 100.0f, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x + 95.0f, lightPosition.y + 5.0f, lightPosition.z, lightColor.x, lightColor.y,
                lightColor.z,
                lightPosition.x + 100.0f, lightPosition.y, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x + 95.0f, lightPosition.y - 5.0f, lightPosition.z, lightColor.x, lightColor.y,
                lightColor.z,
                lightPosition.x, lightPosition.y + 100.0f, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x + 5.0f, lightPosition.y + 95.0f, lightPosition.z, lightColor.x, lightColor.y,
                lightColor.z,
                lightPosition.x, lightPosition.y + 100.0f, lightPosition.z, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x - 5.0f, lightPosition.y + 95.0f, lightPosition.z, lightColor.x, lightColor.y,
                lightColor.z,
                lightPosition.x, lightPosition.y, lightPosition.z + 100.0f, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y + 5.0f, lightPosition.z + 95.0f, lightColor.x, lightColor.y,
                lightColor.z,
                lightPosition.x, lightPosition.y, lightPosition.z + 100.0f, lightColor.x, lightColor.y, lightColor.z,
                lightPosition.x, lightPosition.y - 5.0f, lightPosition.z + 95.0f, lightColor.x, lightColor.y,
                lightColor.z,
        };

        GLuint VAO, VBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              reinterpret_cast<void*>(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);
        glLineWidth(5.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, 6);
        glBindVertexArray(0);

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

int main()
{
    auto window = init();

    #ifndef NDEBUG
    ImGui::GetIO().IniFilename = nullptr;
    #endif

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = camera.getProjectionMatrix(static_cast<GLfloat>(WIDTH) / static_cast<GLfloat>(HEIGHT),
                                                          camera.fov);

        GLdouble currentFrameTime = glfwGetTime();
        GLdouble deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        handleInput(window, deltaTime);
        renderGraphics(view, projection);
        renderGUI();

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return EXIT_SUCCESS;
}
