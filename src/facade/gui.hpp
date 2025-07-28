#pragma once
// Fachada para a biblioteca de Interface Gráfica do Usuário.
// Declara um anova classe singleton Gui.
// Implementa o Dear Im Gui com GLUT e OpenGL3.

// WARN -> Neste caso é Header only pois está sendo usando apenas na main,
// Caso seja distribuído para outras unidades de código, por favor,
// mova a implementação para um arquivo gui.cpp.

#include <memory>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glut.h"
#include "util.hpp"

#include "cg/math.hpp" // Vector3


// Forward declarations
class Window;

class Gui {
    friend class Window;
public:
    // Obtém a instância singleton
    static Gui& instance() {
        static Gui inst;
        return inst;
    }

    inline static void initialize() {
        Gui::instance()._initialize();
    }

    inline static void newFrame() {
        Gui::instance()._newFrame();
    }

    inline static void render() {
        Gui::instance()._render();
    }

    inline static void shutdown() {
        Gui::instance()._shutdown();
    }

    inline static float getFps() {
        return Gui::instance().io->Framerate;
    }

private:
    // Inicialização (Criação de contexto ImGui, binding OpenGL3/GLUT etc)
    inline void _initialize() {
        // 1) Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        
        io = &ImGui::GetIO(); (void)*io;
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
        io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

        ImGui::StyleColorsDark(); // Setup Dear ImGui style

        // 2) ImGui + GLUT + OpenGL3 bindings
        ImGui_ImplGLUT_Init();
        ImGui_ImplOpenGL3_Init();

        // Configura o callback de reshape do GLUT para o ImGui
        ImGui_ImplGLUT_InstallFuncs();

#ifdef _DEBUG
        _initialized = true;
#endif
    }

    // Início de um novo frame ImGui
    inline void _newFrame() const {
#ifdef _DEBUG
        assert_err(_initialized); // Can't create a non initialized GUI context!
#endif  
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGLUT_NewFrame();
        ImGui::NewFrame();
    }

    // Renderiza tudo que foi enfileirado no ImGui
    inline void _render() const {
#ifdef _DEBUG
        assert_err(_initialized); // Tried to render a not initialized GUI context!
#endif  
        // 1. Processar dados da interface no frame buffer
        ImGui::Render();

        // 2. Preparar a viewport / limpar tela
        //int display_w, display_h;
        //glfwGetFramebufferSize(window, &display_w, &display_h);
        //glViewport(0, 0, display_w, display_h);
        //glClear(GL_COLOR_BUFFER_BIT);

        // 3. Desenhar o conteúdo do ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    // Limpa tudo ao encerrar
    inline void _shutdown() {
#ifdef _DEBUG
        assert_err(_initialized); // Tried to shutdown an already destroied GUI context!
#endif  
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGLUT_Shutdown();
        ImGui::DestroyContext();
#ifdef _DEBUG
        _initialized = false;
#endif
    }

private:
    Gui() = default;
    ~Gui() = default;
    Gui(const Gui&) = delete;
    Gui& operator=(const Gui&) = delete;

    ImGuiIO *io = nullptr;
#ifdef _DEBUG
    bool _initialized = false;
#endif
    Window *appendingWindow = nullptr;
};


class Window {
public:
    /* Create a window called by `name` and append into it.*/
    Window(const char* name) {
#ifdef _DEBUG
        Gui::instance().appendingWindow = this;
#endif
        ImGui::Begin(name);
    }
    ~Window() {
        ImGui::End();
#ifdef _DEBUG
        Gui::instance().appendingWindow = nullptr;
#endif
    }

    /* Next content will be displayed in the same line as the previous */
    inline void sameLine(float offsetFromStartX = 0.0f, float spacing = -1.0f) {
        ImGui::SameLine(offsetFromStartX, spacing);
    }

    /* Display some text (you can use a format string too) */
    template<typename... Args>
    void showText(const char *text, Args ...args) const {
        ImGui::Text(text, args...);
    }

    /** Displays a button inside the window
     * @param active Controls a bool to represent the active state.
     * Returns true if active.
     * @param label Associated label text.
     */
    inline bool showButton(const char* label = "", cg::Vector2 size = {}) const {
        return ImGui::Button(label, *(ImVec2 *)&size);
    }

    /** Displays a checkbox button inside the window
     * @param active Controls a bool to represent the active state.
     * Returns true if active.
     */ 
    inline bool showCheckBox(bool* active, const char* label = "") const {
        return ImGui::Checkbox(label, active);
    }

    inline void showSliderFloat(float *f, float min, float max, const char *label = "") const {
        ImGui::SliderFloat(label, f, 0.0f, 1.0f);
    }
    inline void showSliderFloat(float *f, float max = 1.0f, const char* label = "") const {
        showSliderFloat(f, 0.0f, max, label);
    }
    inline void showSliderFloat(float* f, const char* label) const {
        showSliderFloat(f, 0.0f, 1.0f, label);
    }

    inline void showColorEdit(cg::Vector3* color, const char *label = "") {
        ImGui::ColorEdit3(label, (float*)color); // Edit 3 floats representing a color
    }

private:
    inline void assertAppending() const {
#ifdef _DEBUG
        assert_err(Gui::instance().appendingWindow == this,
            "Can't append to current window, another instance is active please destroy it first [~Window]");
#endif
    }
};