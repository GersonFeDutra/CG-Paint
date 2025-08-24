#pragma once
// Fachada para a biblioteca de Interface Gráfica do Usuário.
// Declara um anova classe singleton Gui.
// Implementa o Dear Im Gui com GLUT e OpenGL3.

// WARN -> Neste caso é Header only pois está sendo usando apenas na main,
// Caso seja distribuído para outras unidades de código, por favor,
// mova a implementação para um arquivo gui.cpp.

#include <memory>
#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glut.h>
#include <imgui_filedialog/ImGuiFileDialog.h>

#include <util.hpp>

#include <cg/math.hpp> // Vector3


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

    inline static void endFrame() {
        Gui::instance()._endFrame();
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

    inline static bool isUsingMouseInput() {
        return Gui::instance().io->WantCaptureMouse;
    }

    inline static bool isUsingKeyboardInput() {
        return Gui::instance().io->WantCaptureKeyboard;
    }

    inline static void openFileDialog(const char* key, const char* title, const char* filters, std::function<void(std::ifstream&)>&& callback) {
        Gui::instance()._openFileDialog(key, title, filters, std::move(callback));
    }
    inline static void saveFileDialog(const char* key, const char* title, const char* filters, std::function<void(std::ofstream&)>&& callback) {
        Gui::instance()._saveFileDialog(key, title, filters, std::move(callback));
    }

    inline static void closeDialog() {
		Gui::instance()._closeDialog();
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
        //ImGui_ImplGLUT_InstallFuncs(); // Usamos um input customizado na main.

        // Detecta e usa o último caminho usado via arquivo de cache
        std::string localdata_dir = get_localdata_dir_path();
        std::string app_cache_dir = localdata_dir + "/CGPaint";
        std::filesystem::create_directories(app_cache_dir); // Garante que a pasta existe
        std::string cache_file = app_cache_dir + "/last_path.cache";
        std::string last_path;

        if (std::filesystem::exists(cache_file)) {
            std::ifstream fin(cache_file);
            std::getline(fin, last_path);
            fin.close();
            if (!last_path.empty() && std::filesystem::exists(last_path)) {
                dialogConfigPath = last_path;
            }
            else {
                last_path = get_documents_dir_path();
                dialogConfigPath = last_path;
                std::ofstream fout(cache_file);
                fout << last_path << std::endl;
                fout.close();
            }
        }
        else {
            last_path = get_documents_dir_path();
            dialogConfigPath = last_path;
            std::ofstream fout(cache_file);
            fout << last_path << std::endl;
            fout.close();
        }

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
    inline void _endFrame() const {
        ImGui::EndFrame();
    }

    // Renderiza tudo que foi enfileirado no ImGui
    inline void _render() {
#ifdef _DEBUG
        assert_err(_initialized); // Tried to render a not initialized GUI context!
#endif
		_process();

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

    void cacheLastPath(const std::string& path) {
        // Salva o último caminho selecionado no arquivo de cache
        std::string localdata_dir = get_localdata_dir_path();
        std::string app_cache_dir = localdata_dir + "/CGPaint";
        std::filesystem::create_directories(app_cache_dir); // Garante que a pasta existe
        std::string cache_file = app_cache_dir + "/last_path.cache";
        std::ofstream fout(cache_file);
        if (fout.is_open()) {
            fout << path << std::endl;
            fout.close();
        }
	}

    inline void _process() {
        if (close) {
            if (ImGuiFileDialog::Instance()->IsOpened())
                ImGuiFileDialog::Instance()->Close();
			close = false;
        }

        // Processa o diálogo se estiver aberto
        if (dialogOpen != nullptr && ImGuiFileDialog::Instance()->Display(dialogOpen)) {
            // Verifica se o usuário selecionou um arquivo
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::cout << "Arquivo selecionado: " << filePathName << std::endl;

                //std::ifstream file(filePathName, std::ios::binary); // TODO -> Versão binária
                std::ifstream file(filePathName);
                if (file.is_open()) {
				    cacheLastPath(filePathName);
                    print_success("Arquivo aberto com sucesso: %s", filePathName.c_str());
                    if (openFileCallback) {
                        openFileCallback(file);
                        openFileCallback = nullptr;
                    }
                    else {
                        print_error("Callback de abertura de arquivo não definida!");
                    }
                    file.close();
                }
                else {
                    print_error("Falha ao abrir o arquivo: %s", filePathName.c_str());
                }
            }
            // Fecha o diálogo
            ImGuiFileDialog::Instance()->Close();
        }
        else if (dialogSave != nullptr && ImGuiFileDialog::Instance()->Display(dialogSave)) {
            // Verifica se o usuário selecionou um arquivo
            if (ImGuiFileDialog::Instance()->IsOk()) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                std::cout << "Arquivo selecionado: " << filePathName << std::endl;

                //std::ofstream file(filePathName, std::ios::binary); // TODO -> Versão binária
                std::ofstream file(filePathName);
                if (file.is_open()) {
                    print_success("Arquivo aberto com sucesso: %s", filePathName.c_str());
					cacheLastPath(filePathName);
                    if (saveFileCallback) {
                        saveFileCallback(file);
                        saveFileCallback = nullptr;
                    }
                    else {
                        print_error("Callback de salvamento de arquivo não definida!");
                    }
                    file.close();
                }
                else {
                    print_error("Falha ao abrir o arquivo: %s", filePathName.c_str());
                }
            }
            // Fecha o diálogo
            ImGuiFileDialog::Instance()->Close();
        }
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

    inline void _openFileDialog(const char* key, const char* title, const char* filters, std::function<void(std::ifstream&)>&& callback) {
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, { {}, {}, dialogConfigPath });
        dialogOpen = key;
        openFileCallback = std::move(callback);
    }

    inline void _saveFileDialog(const char* key, const char* title, const char* filters, std::function<void(std::ofstream&)>&& callback) {
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filters, { {}, {}, dialogConfigPath });
        dialogSave = key;
        saveFileCallback = std::move(callback);
    }

    inline void _closeDialog() {
		close = true;
    }

private:
    bool close = false;
    const char* dialogOpen = nullptr;
    const char* dialogSave = nullptr;
    std::function<void(std::ifstream&)> openFileCallback = nullptr;
    std::function<void(std::ofstream&)> saveFileCallback = nullptr;
    std::string dialogConfigPath{};

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
     * @param active: Controls a bool to represent the active state.
     * Returns true if active.
     */ 
    inline bool showCheckBox(bool* active, const char* label = "") const {
        return ImGui::Checkbox(label, active);
    }

    inline bool showRadioButton(int* choices, int choice = 0, const char* label = "") const {
        return ImGui::RadioButton(label, choices, choice);
    }

    // inline void showRadioButton(const char *label = "", bool active = false) const {
    //     ImGui::RadioButton(label, active);
    // }

    // inline void showRadioButton(const char *label = "", int *v, int v_button) const {
    //     ImGui::RadioButton(label, v, v_button);
    // }

    inline void showSliderFloat(float *f, float min, float max, const char *label = "") const {
        ImGui::SliderFloat(label, f, min, max);
    }
    inline void showSliderFloat(float *f, float max = 1.0f, const char* label = "") const {
        showSliderFloat(f, 0.0f, max, label);
    }
    inline void showSliderFloat(float* f, const char* label) const {
        showSliderFloat(f, 0.0f, 1.0f, label);
    }

    inline void showSliderVector2(cg::Vector2* vec, cg::Vector2 min, cg::Vector2 max, const char* labelX = "", const char* labelY = "") const {
		auto width = ImGui::GetContentRegionAvail().x;
        // divide o espaço disponível para dois sliders
		width = (width - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize(labelX).x - ImGui::CalcTextSize(labelY).x) / 2.0f;

        ImGui::PushItemWidth(width);
        ImGui::SliderFloat(labelX, &(vec->x), min.x, max.x);
        ImGui::SameLine();
        ImGui::SliderFloat(labelY, &(vec->y), min.y, max.y);
		ImGui::PopItemWidth();
	}

    inline void showSliderInt(int* value, int min, int max, const char* label = "", const char* format = "") {
        ImGui::SliderInt(label, value, min, max, format);
    }
    inline void showSliderInt(int* value, int max = INT_MAX, const char* label = "", const char* format = "") {
        ImGui::SliderInt(label, value, 0, max, format);
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
