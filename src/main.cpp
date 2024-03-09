#define STB_IMAGE_IMPLEMENTATION
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

GLint WIDTH = 1366, HEIGHT = 768;

GLdouble lastFrameTime = 0.0f;
const GLchar* lightTypes[] = {"Point", "Directional", "Spot"};
glm::vec3 lightPosition(0.0f), lightRotation(0.0f), lightScale(1.0f);
glm::vec4 lightColor(1.0f);
GLint lightType = 0;
GLfloat spotLightAngle = 25.0f;
bool enableAmbientLight = true, enableDiffuseLight = true, enableSpecularLight = true;

ImGuiIO io;
Camera camera;

void debugLog(GLenum source, GLenum type, GLuint id, GLenum severity, GLint, const GLchar* message, const void*)
{
    if (type == GL_DEBUG_TYPE_OTHER) return;

    std::cerr << "Source: ";
    switch (source)
    {
        case GL_DEBUG_SOURCE_API:
            std::cerr << "API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cerr << "Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cerr << "Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cerr << "Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cerr << "Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cerr << "Other";
            break;
        default:
            std::cerr << "Unknown";
            break;
    }

    std::cerr << "\nType: ";
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            std::cerr << "Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cerr << "Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cerr << "Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cerr << "Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cerr << "Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cerr << "Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cerr << "Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cerr << "Pop Group";
            break;
        default:
            std::cerr << "Unknown";
            break;
    }

    std::cerr << "\nID: " << id << "\nSeverity: ";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cerr << "High";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cerr << "Medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cerr << "Low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cerr << "Notification";
            break;
        default:
            std::cerr << "Unknown";
            break;
    }

    std::cerr << "\nMessage: " << message << "\n\n";
}

GLFWwindow* init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

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

    #ifdef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(debugLog, nullptr);
    #endif

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.4f, 0.4f, 0.4f, 1.0f);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    io = ImGui::GetIO();
    static_cast<void>(io);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, GLint width, GLint height)
    {
        WIDTH = width;
        HEIGHT = height;

        glViewport(0, 0, width, height);
    });

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

        static GLdouble lastX = static_cast<GLfloat>(WIDTH) / 2.0f, lastY = static_cast<GLfloat>(HEIGHT) / 2.0f;
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

    ImGui::Begin("Graphics Test 4", nullptr, ImGuiWindowFlags_NoMove);
    ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
    ImGui::SetWindowSize(ImVec2(static_cast<GLfloat>(WIDTH ) / 3.5f, static_cast<GLfloat>(HEIGHT)));

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
    ImGui::SliderFloat3("Light Position", glm::value_ptr(lightPosition), -100.0f, 100.0f);
    ImGui::SliderFloat3("Light Rotation", glm::value_ptr(lightRotation), -360.0f, 360.0f);
    ImGui::SliderFloat3("Light Scale", glm::value_ptr(lightScale), 0.0f, 10.0f);
    ImGui::ColorEdit4("Light Color", glm::value_ptr(lightColor));
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
        lightColor = glm::vec4(1.0f);

        lightType = 0;
        spotLightAngle = 25.0f;

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
    auto setupShader = [&](Shader &shader)
    {
        shader.use();
        shader.setMatrices(view, projection);

        shader.setVec3("lightColor", lightColor);
        shader.setVec3("lightPos", lightPosition);
        shader.setVec3("lightDirection", lightRotation);
        shader.setVec3("viewPos", camera.getPosition());
        shader.setVec3("objectColor", lightColor);
        shader.setInt("lightType", lightType);
        shader.setFloat("spotLightAngle", spotLightAngle);
        shader.setBool("enableAmbientLight", enableAmbientLight);
        shader.setBool("enableDiffuseLight", enableDiffuseLight);
        shader.setBool("enableSpecularLight", enableSpecularLight);
    };

    {
        Shader defaultShader("lib/shaders/defaultVertex.glsl", "lib/shaders/defaultFragment.glsl");
        Model model("lib/models/cube.stl", defaultShader);

        setupShader(defaultShader);
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

        lightShader.setVec4("lightColor", lightColor);
        light.draw();
    }
    {
        Shader sphereShader("lib/shaders/defaultVertex.glsl", "lib/shaders/defaultFragment.glsl");
        Sphere sphere(sphereShader);

        sphere.position = glm::vec3(0.0f, 0.0f, -5.0f);
        sphere.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        sphere.scale = glm::vec3(1.0f);
        sphere.updateModel();

        Texture texDiffuse("lib/textures/Bricks086_1K-PNG_Color.png", "diffuse");
        texDiffuse.bind(GL_TEXTURE0);

        Texture texSpecular("lib/textures/Bricks086_1K-PNG_Roughness.png", "specular");
        texSpecular.bind(GL_TEXTURE1);

        setupShader(sphereShader);
        sphereShader.setBool("hasTexture", true);
        sphere.draw();
    }
    {
        Shader planeShader("lib/shaders/defaultVertex.glsl", "lib/shaders/defaultFragment.glsl");
        Plane plane(planeShader);

        plane.position = glm::vec3(0.0f, -1.0f, 0.0f);
        plane.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        plane.scale = glm::vec3(10.0f);
        plane.updateModel();

        Texture texDiffuse("lib/textures/Bricks086_1K-PNG_Color.png", "diffuse");
        texDiffuse.bind(GL_TEXTURE0);

        Texture texSpecular("lib/textures/Bricks086_1K-PNG_Roughness.png", "specular");
        texSpecular.bind(GL_TEXTURE1);

        setupShader(planeShader);
        planeShader.setBool("hasTexture", true);
        plane.draw();
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
