#include "util.hpp" // Importações do Open GL, FreeGLUT e métodos auxiliares para debug

#include <cstdlib> // Inclui algumas convenções do C
#include <cstring> // Manipulação de cadeias de caracteres

#include "facade/gui.hpp"

#include "cg/geometry.hpp"
#include "cg/canvas.hpp"
#include "cg/canvas_itens/flag.hpp"
#include "cg/canvas_itens/point.hpp"


static constexpr cg::Vector2 WINDOW_SIZE{ cg::Flag::SIZE * 30 }; // aspect ratio: 10:7

static constexpr cg::Transform2D SCREEN_TO_NDC{
    cg::Vector3{ 2.0f / WINDOW_SIZE.x, 0.0f, 0.0f },
    cg::Vector3{ 0.0f, -2.0f / WINDOW_SIZE.y, 0.0f },
    cg::Vector3{ -1.0f, 1.0f, 1.0f },
}; // Screen coordinates to Normalized Display Coordinates

enum {
    POINT_CHECK = 0,
    LINE_CHECK = 1,
    POLY_CHECK = 2,
};
static int current_tool_primitive;

static cg::Canvas canvas;

cg::Flag *flag = nullptr;

static cg::Color global_color = cg::colors::WHITE; // define a cor selecionada na janela separada
static int selectedShape = 0; // point


/* Inicialização do renderer */
int init(void)
{
    //auto _flag_ptr = std::make_unique<cg::Flag>();

    //flag = _flag_ptr.get();
    //canvas.insert(std::move(_flag_ptr));

    return EXIT_SUCCESS;
}


/* Loop principal de desenho. */
void display()
{
    //Gui::newFrame();

    GLdebug{
        glClear(GL_COLOR_BUFFER_BIT); // Limpa o quadro do buffer de cor
    }

    canvas.updateRender();

    // Mostra uma janela simples.
    // Cada janela ser criada num escopo separado para invocar
    // os construtores/ destrutores necessários
    {
        goto down;
        static int counter = 0;
        static float f = 0.0f;
        // static cg::Vector3 color;

        // testes
        // Window test("Hello, world!");

        // test.showText("Teste!");
        // test.showCheckBox(&check, "Active");
        // test.showSliderFloat(&f, "float");
        // test.showColorEdit(&global_color, "color");

        Window test("Controls");

        //switch (currentPrimitive)
        //{
        //case POINT:
        //    insert(std::make_unique<Point>(input_event.position));
        //    break;
        //default:
        //    break;
        //}

        if (test.showRadioButton(&current_tool_primitive, POINT_CHECK, "Point")) {
            // use point
        }
        if (test.showRadioButton(&current_tool_primitive, LINE_CHECK, "Line")) {
            // use line
        }
        if (test.showRadioButton(&current_tool_primitive, POLY_CHECK, "Polygon")) {
            // use polygon
        }
        
        test.showColorEdit((cg::Vector3 *)&global_color.r, "color");
        
        if (test.showButton("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        test.sameLine();

        test.showText("counter = %d", counter);
        
        test.showText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Gui::getFps(), Gui::getFps());
    }
down:

    //Gui::render();

    // Sincroniza comandos de desenho não executados,
    // em tempo finito [GLUT_DOUBLE buffering]
    glutSwapBuffers();
    
    // Força atualização contínua. Garantindo execução imediata.
    glutPostRedisplay(); // Recomendado para uso com a GUI
}


/* Chamada sempre que a janela for redimensionada */
void reshape(int w, int h) {
    // 1. Atualiza o viewport
    GLdebug {
        glViewport(0, 0, w, h);
    }

    // Repassa o evento para o ImGui
    ImGui_ImplGLUT_ReshapeFunc(w, h);

    // Lógica personalizada de redimensionamento abaixo
}


void onMouseMoveEvent(int x, int y) {
    // Delegar entrada ao Dear Im Gui primeiro
    ImGui_ImplGLUT_MotionFunc(x, y);

    if (Gui::isUsingMouseInput())
        return; // A Interface Gráfica do Usuário tem prioridade na entrada. "Camada superior"

    canvas.sendInput(cg::io::MouseMove{ cg::Vector2i{x, y} });
}


void onMouseDragEvent(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    if (Gui::isUsingMouseInput())
        return;

    canvas.sendInput(cg::io::MouseDrag{ cg::Vector2i{x, y} });
}


void onMouseWheelEvent(int wheel, int direction, int x, int y) {
    ImGui_ImplGLUT_MouseWheelFunc(wheel, direction, x, y);
    if (Gui::isUsingMouseInput())
        return;

    switch (wheel) {
    case 0:
        canvas.sendInput(cg::io::MouseWheelV{cg::Vector2i{x, y}, direction });
        break;
    case 1:
        canvas.sendInput(cg::io::MouseWheelH{cg::Vector2i{x, y}, direction});
    break;
    default: // ignore
        break;
    }
}


void onMouseButtonEvent(int button, int state, int x, int y)
{
    ImGui_ImplGLUT_MouseFunc(button, state, x, y);
    if (Gui::isUsingMouseInput())
        return;

    switch (button) {
    case GLUT_LEFT_BUTTON:
        switch (state) {
        case GLUT_DOWN:
            canvas.sendInput(cg::io::MouseLeftButtonPressed{ cg::Vector2i{x, y} });
            break;
        case GLUT_UP:

            switch (current_tool_primitive)
            {
            case POINT_CHECK: {
                cg::ColorRgb color = cg::Color(global_color);
                cg::Vector2 position = SCREEN_TO_NDC.transform(x, y);
                canvas.insert(std::make_unique<cg::Point>(position, color));
            } break;
            default:
                break;
            }

            canvas.sendInput(cg::io::MouseLeftButtonReleased{ cg::Vector2i{x, y} });
            break;
        default: // ignore
            break;
        }
        break;
    case GLUT_RIGHT_BUTTON:
        switch (state) {
        case GLUT_DOWN:
            canvas.sendInput(cg::io::MouseRightButtonPressed{ cg::Vector2i{x, y} });
            break;
        case GLUT_UP:
            canvas.sendInput(cg::io::MouseRightButtonReleased{ cg::Vector2i{x, y} });
            break;
        default: // ignore
            break;
        }
        break;
    default: // ignore
        break;
    }
}


#if defined(_WIN32) || defined(_WIN64)
    HANDLE _hConsole;
    WORD _saved_attributes;
#endif


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

    glutInitWindowSize(WINDOW_SIZE.x, WINDOW_SIZE.y); // tamanho da área interna da janela (coordenadas de tela)
    glutCreateWindow("Trabalho CG [Open GL]: Bandeira do Brasil");

    // Glut Input Events -> Eventos de entrada customizados
    glutPassiveMotionFunc(onMouseMoveEvent);
    glutMotionFunc(onMouseDragEvent);
    glutMouseFunc(onMouseButtonEvent);
    glutMouseWheelFunc(onMouseWheelEvent);

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
