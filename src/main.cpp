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

#include "cg/fileSystem/fileHandler.hpp"

static cg::Canvas canvas{ cg::Flag::SIZE * 30 };

cg::Flag *flag = nullptr;

cg::FileHandler fileHandler;

// static int selectedShape = 0; // useless line

// static files::FileHandler fileManager


/* Inicialização do renderer */
int init(void)
{
    //auto _flag_ptr = std::make_unique<cg::Flag>();

    //flag = _flag_ptr.get();
    //canvas.insert(std::move(_flag_ptr));

    return EXIT_SUCCESS;
}

// get all polygons from canvas
ArrayList<cg::Polygon *> getPolygons() {
    ArrayList<cg::Polygon *> returnList;

    for (auto& item : canvas.getItens()) {
        if (auto derivedClass = dynamic_cast<cg::Polygon *>(item.get())) {
            returnList.push_back(derivedClass);
        } else continue;
    }
    
    return returnList;
}
// get all lines from canvas
ArrayList<cg::Line *> getLines() {
    ArrayList<cg::Line *> returnList;

    for (auto& item : canvas.getItens()) {
        if (auto derivedClass = dynamic_cast<cg::Line *>(item.get())) {
            returnList.push_back(derivedClass);
        } else continue;
    }
    
    return returnList;
}
// get all points from canvas
ArrayList<cg::Point *> getPoints() {
    ArrayList<cg::Point *> returnList;

    for (auto& item : canvas.getItens()) {
        if (auto derivedClass = dynamic_cast<cg::Point *>(item.get())) {
            returnList.push_back(derivedClass);
        } else continue;
    }
    
    return returnList;
}




/* Loop principal de desenho. */
void display()
{
    Gui::newFrame();

    GLdebug{
        glClear(GL_COLOR_BUFFER_BIT); // Limpa o quadro do buffer de cor
    }

    canvas.updateRender();

    // Mostra uma janela simples.
    // Cada janela ser criada num escopo separado para invocar
    // os construtores/ destrutores necessários
    {
        static int counter = 0;
        static float f = 0.0f;

        Window toolBox("Controls");

        // toolBox.showSliderFloat(&f, "float");
        // toolBox.showCheckBox(&check, "Active");

        if (toolBox.showRadioButton(&canvas.toolBox.currentPrimitive, cg::ToolBox::Primitives::POINT, "Point")); // use point
        if (toolBox.showRadioButton(&canvas.toolBox.currentPrimitive, cg::ToolBox::Primitives::LINE, "Line")); // use line
        if (toolBox.showRadioButton(&canvas.toolBox.currentPrimitive, cg::ToolBox::Primitives::POLYGON, "Polygon")); // use polygon

        // TODO [Extra] -> Polígono regular
        // switch (canvas.toolBox.currentPrimitive) {
        // case cg::ToolBox::Primitives::REGULAR_POLYGON: {
        //     toolBox.sameLine();
        //     toolBox.showSliderInt(&canvas.toolBox.polygonEdges, 1, 255, "Edges");
        //     toolBox.sameLine();
        //     toolBox.showText("[%d]", canvas.toolBox.polygonEdges);
        // } break;
        // default: break;
        // }
        
        toolBox.showColorEdit((cg::Vector3 *)&canvas.toolBox.currentColor.r, "color");
        
        if (toolBox.showButton("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        toolBox.sameLine();

        toolBox.showText("counter = %d", counter);

        if (toolBox.showButton("Save to file")) {
            ArrayList<cg::Polygon *> polyList = getPolygons();
            ArrayList<cg::Line *> lineList = getLines();
            ArrayList<cg::Point *> pntList = getPoints();

            fileHandler.saveFile(pntList, lineList, polyList);
        };

        if (toolBox.showButton("Load from file")) {
            std::tuple<ArrayList<cg::Point *>, ArrayList<cg::Line *>, ArrayList<cg::Polygon *>> objects = fileHandler.loadFile("");

            for (auto& obj : canvas.getItens()) {
                canvas.remove(obj.get());
            }

            for (auto& point : std::get<0>(objects)) {
                canvas.insert(std::make_unique<cg::Point>(* point));
            }
            for (auto& line : std::get<1>(objects)) {
                canvas.insert(std::make_unique<cg::Line>(* line));
            }
            for (auto& poly : std::get<2>(objects)) {
                canvas.insert(std::make_unique<cg::Polygon>(* poly));
            }
        }
        
        toolBox.showText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Gui::getFps(), Gui::getFps());
    }

    Gui::render();

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

    // Envia o novo tamanho da janela para o canvas
    canvas.setWindowSize(w, h);

    // Lógica personalizada de redimensionamento abaixo
}


void onMouseMoveEvent(int x, int y) {
    // Delegar entrada ao Dear Im Gui primeiro
    ImGui_ImplGLUT_MotionFunc(x, y);

    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return; // A Interface Gráfica do Usuário tem prioridade na entrada. "Camada superior"

    canvas.sendScreenInput<cg::io::MouseMove>(x, y);
}


void onMouseDragEvent(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
    if (canvas.toolBox.isInsideGui = Gui::isUsingMouseInput())
        return;

    canvas.sendScreenInput<cg::io::MouseDrag>(x, y);
}


void onMouseWheelEvent(int wheel, int direction, int x, int y) {
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


void onMouseButtonEvent(int button, int state, int x, int y)
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

    auto window_size = canvas.getWindowSize();
    glutInitWindowSize(window_size.x, window_size.y); // tamanho da área interna da janela (coordenadas de tela)
    glutCreateWindow("Paint CG");

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
