#pragma once
// Fachada para a biblioteca de Interface Gráfica do Usuário.
// Declara um anova classe singleton Gui.
// Implementa o Dear Im Gui com GLUT e OpenGL3.

// WARN -> Neste caso é Header only pois está sendo usando apenas na main,
// Caso seja distribuído para outras unidades de código, por favor,
// mova a implementação para um arquivo gui.cpp.

#include <memory>
#include <filesystem>
#include <string>
#include <cstdint>

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

    inline static bool isDialogOpen() {
        return Gui::instance()._isDialogOpen();
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

    inline bool _isDialogOpen() {
        return ImGuiFileDialog::Instance()->IsOpened();
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
        return ImGui::Button(label, *(ImVec2*)&size);
    }

    enum Increment { DEC_PRESSED = -1, NONE = 0, INC_PRESSED = +1 };

    inline Increment showIncrementalFloatSlider(float* f, float min, float max, float by = 1.0f,
            const char* label = "", const char* dec_label = "", const char* inc_label = "",
            const char* dec_description = "", const char* inc_description = "")
    {
        ImGuiStyle& style = ImGui::GetStyle();

        const char* lbl_slider = label ? label : "";
        const char* lbl_dec = dec_label ? dec_label : "";
        const char* lbl_inc = inc_label ? inc_label : "";
        const char* txt_dec = dec_description ? dec_description : "";
        const char* txt_inc = inc_description ? inc_description : "";

        // calcule larguras mínimas dos elementos que não devem expandir (inclui o label do slider, agora no fim)
        ImVec2 sz_dec_btn = ImGui::CalcTextSize(lbl_dec);
        float dec_btn_w = sz_dec_btn.x + style.FramePadding.x * 2.0f;
        if (dec_btn_w <= 0.0f) dec_btn_w = 0.0f;

        ImVec2 sz_inc_btn = ImGui::CalcTextSize(lbl_inc);
        float inc_btn_w = sz_inc_btn.x + style.FramePadding.x * 2.0f;
        if (inc_btn_w <= 0.0f) inc_btn_w = 0.0f;

        ImVec2 sz_dec_txt = ImGui::CalcTextSize(txt_dec);
        float dec_txt_w = sz_dec_txt.x;
        if (dec_txt_w <= 0.0f) dec_txt_w = 0.0f;

        ImVec2 sz_inc_txt = ImGui::CalcTextSize(txt_inc);
        float inc_txt_w = sz_inc_txt.x;
        if (inc_txt_w <= 0.0f) inc_txt_w = 0.0f;

        ImVec2 sz_label = ImGui::CalcTextSize(lbl_slider);
        float label_w = sz_label.x;
        if (label_w <= 0.0f) label_w = 0.0f;

        // Disponível total (em px) para todos os elementos nesta linha
        float avail = ImGui::GetContentRegionAvail().x;

        // elementos na linha (ordem): dec_btn, dec_txt, slider, inc_txt, inc_btn, label
        const int gaps = 5; // gaps entre os 6 elementos
        float total_gaps_w = style.ItemSpacing.x * gaps;

        // largura mínima total que NÃO é do slider (inclui label no fim)
        float min_fixed_w = dec_btn_w + dec_txt_w + inc_txt_w + inc_btn_w + label_w + total_gaps_w;

        // largura que iremos dar ao slider
        float slider_w = avail - min_fixed_w;
        const float slider_min_w = 50.0f; // mínimo defensivo
        if (slider_w < slider_min_w) {
            slider_w = slider_min_w;
        }

        // --- layout e render ---
        // dec button
        bool dec_pressed = false;
        if (dec_btn_w > 0.0f) {
            dec_pressed = ImGui::Button(lbl_dec, ImVec2(dec_btn_w, 0.0f));
        }

        ImGui::SameLine();

        // dec description (texto mínimo)
        if (dec_txt_w > 0.0f) {
            ImGui::TextUnformatted(txt_dec);
        }

        ImGui::SameLine();

        // slider (ocupa slider_w). Usamos uma ID derivada do ponteiro para evitar colisões e não mostrar label aqui.
        std::string slider_id = std::string("##slider_") + std::to_string(reinterpret_cast<std::uintptr_t>(f));
        ImGui::PushItemWidth(slider_w);
        bool changed = ImGui::SliderFloat(slider_id.c_str(), f, min, max);
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // inc description
        if (inc_txt_w > 0.0f) {
            ImGui::TextUnformatted(txt_inc);
        }

        ImGui::SameLine();

        // inc button
        bool inc_pressed = false;
        if (inc_btn_w > 0.0f) {
            inc_pressed = ImGui::Button(lbl_inc, ImVec2(inc_btn_w, 0.0f));
        }

        ImGui::SameLine();

        // finalmente, o label do slider (exibe após o último botão de incremento)
        if (label_w > 0.0f) {
            ImGui::TextUnformatted(lbl_slider);
        }

        // retorno
        if (dec_pressed)
            return DEC_PRESSED;
        if (inc_pressed)
            return INC_PRESSED;
        return NONE;
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

    inline cg::Vector2 showIncrementalSliderVector2(
        cg::Vector2* vec, cg::Vector2 min, cg::Vector2 max, float by = 1.0f,
        // X axis: slider label, dec button label, inc button label
        const char* labelX = "X", const char* dec_labelX = "<", const char* inc_labelX = ">",
        // Y axis
        const char* labelY = "Y", const char* dec_labelY = "V", const char* inc_labelY = "^"
    ) {
        ImGuiStyle& style = ImGui::GetStyle();

        // normalize strings (evita null)
        const char* lx = labelX ? labelX : "";
        const char* dx = dec_labelX ? dec_labelX : "";
        const char* ix = inc_labelX ? inc_labelX : "";
        const char* ly = labelY ? labelY : "";
        const char* dy = dec_labelY ? dec_labelY : "";
        const char* iy = inc_labelY ? inc_labelY : "";

        // calcula larguras mínimas (botões consideram FramePadding)
        ImVec2 sz;
        sz = ImGui::CalcTextSize(dx); float dec_btn_x_w = (dx[0] ? (sz.x + style.FramePadding.x * 2.0f) : 0.0f);
        sz = ImGui::CalcTextSize(ix); float inc_btn_x_w = (ix[0] ? (sz.x + style.FramePadding.x * 2.0f) : 0.0f);
        sz = ImGui::CalcTextSize(lx);    float label_x_w = (lx[0] ? sz.x : 0.0f);

        sz = ImGui::CalcTextSize(dy); float dec_btn_y_w = (dy[0] ? (sz.x + style.FramePadding.x * 2.0f) : 0.0f);
        sz = ImGui::CalcTextSize(iy); float inc_btn_y_w = (iy[0] ? (sz.x + style.FramePadding.x * 2.0f) : 0.0f);
        sz = ImGui::CalcTextSize(ly);    float label_y_w = (ly[0] ? sz.x : 0.0f);

        // espaço disponível
        float avail = ImGui::GetContentRegionAvail().x;

        // estimativa de gaps: cada cluster tem 3 gaps entre seus 4 itens (dec, slider, inc, label).
        // Além disso, deixamos 1 gap extra entre os clusters.
        const float gaps_count = 3.0f + 3.0f + 1.0f;
        float total_gaps_w = style.ItemSpacing.x * gaps_count;

        // largura mínima fixa (tudo exceto sliders)
        float min_fixed = dec_btn_x_w + inc_btn_x_w + label_x_w
            + dec_btn_y_w + inc_btn_y_w + label_y_w
            + total_gaps_w;

        // espaço restante para os 2 sliders (dividido igualmente)
        float slider_total = avail - min_fixed;
        const float slider_min_each = 50.0f;
        float slider_w_each = slider_min_each;
        if (slider_total > 2.0f * slider_min_each) slider_w_each = slider_total * 0.5f;

        // rendering em uma só linha: cluster X então cluster Y
        bool dec_x_pressed = false, inc_x_pressed = false;
        bool dec_y_pressed = false, inc_y_pressed = false;

        // --- X cluster ---
        if (dec_btn_x_w > 0.0f) dec_x_pressed = ImGui::Button(dx, ImVec2(dec_btn_x_w, 0.0f));
        ImGui::SameLine();

        {
            // slider X (ID único baseado no ponteiro)
            std::string id_x = std::string("##slider_x_") + std::to_string(reinterpret_cast<std::uintptr_t>(vec));
            ImGui::PushItemWidth(slider_w_each);
            ImGui::SliderFloat(id_x.c_str(), &(vec->x), min.x, max.x);
            ImGui::PopItemWidth();
        }
        ImGui::SameLine();

        if (inc_btn_x_w > 0.0f) inc_x_pressed = ImGui::Button(ix, ImVec2(inc_btn_x_w, 0.0f));
        ImGui::SameLine();

        if (label_x_w > 0.0f) ImGui::TextUnformatted(lx);

        // separador pequeno entre clusters
        ImGui::SameLine();

        // --- Y cluster ---
        if (dec_btn_y_w > 0.0f) dec_y_pressed = ImGui::Button(dy, ImVec2(dec_btn_y_w, 0.0f));
        ImGui::SameLine();

        {
            std::string id_y = std::string("##slider_y_") + std::to_string(reinterpret_cast<std::uintptr_t>(vec));
            ImGui::PushItemWidth(slider_w_each);
            ImGui::SliderFloat(id_y.c_str(), &(vec->y), min.y, max.y);
            ImGui::PopItemWidth();
        }
        ImGui::SameLine();

        if (inc_btn_y_w > 0.0f) inc_y_pressed = ImGui::Button(iy, ImVec2(inc_btn_y_w, 0.0f));
        ImGui::SameLine();

        if (label_y_w > 0.0f) ImGui::TextUnformatted(ly);

        // monta o vetor de incremento já multiplicado por 'by'
        cg::Vector2 increment{ 0.0f, 0.0f };
        if (dec_x_pressed) increment.x = -by;
        else if (inc_x_pressed) increment.x = +by;
        if (dec_y_pressed) increment.y = -by;
        else if (inc_y_pressed) increment.y = +by;

        return increment;
    }

    inline void showSliderVector2(cg::Vector2* vec, float min, float max, const char* labelX = "", const char* labelY = "") const {
        showSliderVector2(vec, { min, min }, { max, max }, labelX, labelY);
    }

    inline void showSliderVector2(cg::Vector2* vec, float max = 1.0f, const char* labelX = "", const char* labelY = "") const {
        showSliderVector2(vec, {}, { max, max }, labelX, labelY);
    }

    inline void showSliderInt(int* value, int min, int max, const char* label = "", const char* format = "") {
        ImGui::SliderInt(label, value, min, max, format);
    }
    inline void showSliderInt(int* value, int max = INT_MAX, const char* label = "", const char* format = "") {
        ImGui::SliderInt(label, value, 0, max, format);
    }

    inline void showColorEdit(cg::Color* color, const char *label = "") {
        ImGui::ColorEdit3(label, (float*)&color->r); // Edit 3 floats representing a color
    }

inline void show2ColorEdit(cg::Color* primary, cg::Color* secondary, const char *label = "") {
    ImGuiStyle& style = ImGui::GetStyle();

    // largura disponível total
    float avail = ImGui::GetContentRegionAvail().x;

    // largura mínima defensiva para cada ColorEdit
    const float color_min_w = 80.0f;

    // largura do label final (ocupando o mínimo)
    float label_w = (label && label[0]) ? ImGui::CalcTextSize(label).x : 0.0f;

    // número de gaps: sempre 1 gap entre os dois color edits; se houver label, mais 1 gap até o label
    int gaps = 1 + (label_w > 0.0f ? 1 : 0);
    float total_gaps_w = style.ItemSpacing.x * (float)gaps;

    // espaço restante que ficará para os dois color edits
    float remaining = avail - label_w - total_gaps_w;
    float each_w = remaining * 0.5f;

    // garante um mínimo razoável
    if (each_w < color_min_w) each_w = color_min_w;

    // desenha os widgets na mesma linha: ColorEdit1 | ColorEdit2 | Label
    ImGui::PushItemWidth(each_w);
    ImGui::ColorEdit3("##primary_color", (float*)&primary->r, ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();

    ImGui::PushItemWidth(each_w);
    ImGui::ColorEdit3("##secondary_color", (float*)&secondary->r, ImGuiColorEditFlags_NoLabel);
    ImGui::SameLine();

    // texto final ocupando o mínimo possível
    if (label && label[0]) ImGui::TextUnformatted(label);

    // limpar os PushItemWidth (duas pushes => duas pops)
    ImGui::PopItemWidth();
    ImGui::PopItemWidth();
}


private:

    inline void assertAppending() const {
#ifdef _DEBUG
        assert_err(Gui::instance().appendingWindow == this,
            "Can't append to current window, another instance is active please destroy it first [~Window]");
#endif
    }
};
