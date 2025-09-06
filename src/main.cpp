#include "util.hpp" // Importações do Open GL, FreeGLUT e métodos auxiliares para debug

#include <cstdlib> // Inclui algumas convenções do C
#include <cstring> // Manipulação de cadeias de caracteres

#include "facade/gui.hpp"

#include "cg/geometry.hpp"
#include "cg/canvas.hpp"
#include "cg/canvas_itens/flag.hpp"
#include "cg/canvas_itens/point.hpp"
#include "cg/canvas_itens/line.hpp"
#include "cg/canvas_itens/polygon.hpp"

static cg::Canvas canvas{ cg::Flag::SIZE * 30 };

cg::Flag *flag = nullptr;


/* Inicialização do renderer */
int init(void)
{
    //auto _flag_ptr = std::make_unique<cg::Flag>();

    auto half_size = canvas.getWindowSize() / 2.0f;
    glClearColor(0.1333f, 0.1333f, 0.1333f, 0.0f); // cor de fundo
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(-half_size.x, half_size.x, -half_size.y, half_size.y); // coordenadas limite do viewport normalizadas (em 2D)

    //flag = _flag_ptr.get();
    //canvas.insert(std::move(_flag_ptr));

    return EXIT_SUCCESS;
}


/* Loop principal de desenho. */
void display()
{
    Gui::newFrame();

    GLdebug() {
        glClear(GL_COLOR_BUFFER_BIT); // Limpa o quadro do buffer de cor
    }

    canvas.updateRender();

    Gui::render();
    Gui::endFrame();

    // Sincroniza comandos de desenho não executados,
    // em tempo finito [GLUT_DOUBLE buffering]
    glutSwapBuffers();

    // Força atualização contínua. Garantindo execução imediata.
    glutPostRedisplay(); // Recomendado para uso com a GUI
}


/* Chamada sempre que a janela for redimensionada */
static void reshape(int w, int h) {
    // 1. Atualiza o viewport
    GLdebug() {
        glViewport(0, 0, w, h);
    }

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); // Reseta a matriz de projeção

    cg::Vector2 window_size( w, h );
	cg::Vector2 half_size = window_size / 2.0f;
    gluOrtho2D(-half_size.x, half_size.x, -half_size.y, half_size.y); // coordenadas limite do viewport normalizadas (em 2D)

	// TODO -> Deslocar a origem do mundo usando o botão do meio do mouse
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity(); // Reseta a matriz de modelo

    // Envia o novo tamanho da janela para o canvas
    canvas.setWindowSize(window_size);

    // Repassa o evento para o ImGui
    ImGui_ImplGLUT_ReshapeFunc(w, h);

    // Lógica personalizada de redimensionamento abaixo
}


static void onMouseMoveEvent(int x, int y) {
    // Delegar entrada ao Dear Im Gui primeiro
    ImGui_ImplGLUT_MotionFunc(x, y);

    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return; // A Interface Gráfica do Usuário tem prioridade na entrada. "Camada superior"

    canvas.sendScreenInput<cg::io::MouseMove>(x, y);
}


static void onMouseDragEvent(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return;

    canvas.sendScreenInput<cg::io::MouseDrag>(x, y);
}


static void onMouseWheelEvent(int wheel, int direction, int x, int y) {
    ImGui_ImplGLUT_MouseWheelFunc(wheel, direction, x, y);
    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return;

    switch (wheel) {
    case 0:
        canvas.sendScreenInput<cg::io::MouseWheelV>(x, y, direction);
        break;
    case 1:
        canvas.sendScreenInput<cg::io::MouseWheelH>(x, y, direction);
        break;
    default: // ignore
        break;
    }
}


static void onMouseButtonEvent(int button, int state, int x, int y)
{
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return;

    switch (button) {
    case GLUT_LEFT_BUTTON:
        switch (state) {
        case GLUT_DOWN:
            canvas.sendScreenInput<cg::io::MouseLeftButtonPressed>(x, y);
            break;
        case GLUT_UP: {
            canvas.sendScreenInput<cg::io::MouseLeftButtonReleased>(x, y);
        } break;
        default: // ignore
            break;
        }
        break;
    case GLUT_RIGHT_BUTTON:
        switch (state) {
        case GLUT_DOWN:
            canvas.sendScreenInput<cg::io::MouseRightButtonPressed>(x, y);
            break;
        case GLUT_UP:
            canvas.sendScreenInput<cg::io::MouseRightButtonReleased>(x, y);
            break;
        default: // ignore
            break;
        }
        break;
    default: // ignore
        break;
    }
}

static void onSpecialKeyPressed(int key, int x, int y)
{
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
    canvas.sendScreenInput<cg::io::SpecialKeyInputEvent>(x, y, key, glutGetModifiers());
}


static void onKeyboardKeyPressed(unsigned char key, int x, int y)
{
    ImGui_ImplGLUT_KeyboardFunc(key, x, y);

    int mods = glutGetModifiers();
    unsigned char normalizedKey = key;

    if (mods & GLUT_ACTIVE_CTRL) {
        // Caso Linux/X11 tenha convertido em código de controle (< 32)
        if (key < 32) {
            // Reconstrói letra minúscula: Ctrl+A (0x01) → 'a'
            normalizedKey = static_cast<unsigned char>(key - 1 + 'a');

            // (key + '@') → Ctrl+A = 0x01 + '@' = 'A'
            //normalizedKey = static_cast<unsigned char>(key + '@');
        }
    }

    // Só deixa maiúsculo se o shift está ativo
    if ((mods & GLUT_ACTIVE_SHIFT) && normalizedKey >= 'a' && normalizedKey <= 'z') {
        normalizedKey = static_cast<unsigned char>(normalizedKey - 'a' + 'A');
    }

	unsigned char ESC = 27;

    std::cout << "[Key]: ";
    if (mods & GLUT_ACTIVE_CTRL)
		std::cout << "Ctrl + ";
	if(mods & GLUT_ACTIVE_SHIFT)
		std::cout << "Shift + ";
    if (mods & GLUT_ACTIVE_ALT)
        std::cout << "Alt + ";

    if (key == 127) {
        std::cout << "Delete [ASCII]\n";
        canvas.sendScreenInput<cg::io::SpecialKeyInputEvent>(x, y, GLUT_KEY_DELETE, mods);
        return;
    }

	if (normalizedKey == ESC)
		std::cout << "ESC\n";
	else
		std::cout << "Raw{" << (int)key << "}" << normalizedKey << '\n';

	canvas.sendScreenInput<cg::io::KeyboardInputEvent>(x, y, normalizedKey, mods);
}


static void onKeyboardKeyReleased(unsigned char key, int x, int y) {
    ImGui_ImplGLUT_KeyboardUpFunc(key, x, y);
}


static void onSpecialKeyReleased(int key, int x, int y) {
    ImGui_ImplGLUT_SpecialUpFunc(key, x, y);
}

static void onEntryEvent(int state) {
    if (state)
        canvas.sendScreenInput<cg::io::FocusIn>();
    else
        canvas.sendScreenInput<cg::io::FocusOut>();
}


#if defined(_WIN32) || defined(_WIN64)
    HANDLE _hConsole;
    WORD _saved_attributes;
#endif


void printGLInfo() {
    const char* version = (const char*)glGetString(GL_VERSION);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* vendor   = (const char*)glGetString(GL_VENDOR);

    print_success("OpenGL Version: [%s]", version);
    print_success("Renderer: [%s]", renderer);
    print_success("Vendor: [%s]", vendor);

#ifdef GL_SHADING_LANGUAGE_VERSION
    const char* shading  = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
    if (shading)
        print_success("GLSL Version: [%s]", shading);
#endif
}


int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(_WIN64)
    // Get the console handle
    _hConsole = GetStdHandle(STD_ERROR_HANDLE);

    // Save the current text attributes
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    GetConsoleScreenBufferInfo(_hConsole, &consoleInfo);
    _saved_attributes = consoleInfo.wAttributes;
#endif

    // 1. Inicialização do GLUT
    glutInit(&argc, argv);

    // modo de exibição: frame buffer, modelo de cor, e antialias ativado
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    // double buffering é necessário para tratamentos visuais dinâmicos
    // [requisitado pela GUI]

    //glEnable(GL_MULTISAMPLE);

    auto window_size = canvas.getWindowSize();
    glutInitWindowSize(window_size.x, window_size.y); // tamanho da área interna da janela (coordenadas de tela)
    glutCreateWindow("Paint CG");

    printGLInfo();

    // Glut Input Events -> Eventos de entrada customizados
    glutPassiveMotionFunc(onMouseMoveEvent);
    glutMotionFunc(onMouseDragEvent);
    glutMouseFunc(onMouseButtonEvent);
    glutMouseWheelFunc(onMouseWheelEvent);
	glutSpecialFunc(onSpecialKeyPressed); // Função de teclas especiais
	glutKeyboardFunc(onKeyboardKeyPressed); // Função de teclas normais
    glutKeyboardUpFunc(onKeyboardKeyReleased);
    glutSpecialUpFunc(onSpecialKeyReleased);
    glutEntryFunc(onEntryEvent);

    // Setup Gui Singleton
    Gui::initialize();

    int err = init();
    if (err != EXIT_SUCCESS)
        return err;
    // Estabelecer callbacks de exibição / redimensão
    glutDisplayFunc(display);
    glutReshapeFunc(reshape); // Necessário para tratamento da GUI

    glutMainLoop(); // Mostre tudo, e espere

    // Destroy o Singleton
    Gui::shutdown();

    return EXIT_SUCCESS;
}
