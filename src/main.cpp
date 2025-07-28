#include "util.hpp" // Importações do Open GL, FreeGLUT e métodos auxiliares para debug

#include <cstdlib> // Inclui algumas convenções do C
#include <cstring> // Manipulação de cadeias de caracteres
#include <random>

#include "facade/gui.hpp"

#include "cg/math.hpp"
#include "cg/geometry.hpp"


static constexpr cg::Vec2 FLAG_SIZE{20, 14};
static constexpr cg::Vec2 WINDOW_SIZE{FLAG_SIZE * 30}; // aspect ratio: 10:7

struct FlagColors {
    static const cg::Color GREEN, YELLOW, BLUE, WHITE;
};
const cg::Color FlagColors::GREEN{ 0, 148, 64 };
const cg::Color FlagColors::YELLOW{ 255, 203, 0 };
const cg::Color FlagColors::BLUE{ 48, 38, 129 };
const cg::Color FlagColors::WHITE{ 255, 255, 255 };


/* Inicialização do renderer */
int init(void)
{
    using namespace cg;

    auto [r, g, b] = FlagColors::GREEN.normalized();

    GLdebug {
        glClearColor(r, g, b, 1.0f); // cor de fundo
    }
    GLdebug {
        glMatrixMode(GL_PROJECTION);
    }

    constexpr bool _ENABLE_ZOOM = IS_DEBUG;

    if constexpr (_ENABLE_ZOOM) {
        Vec2 center = FLAG_SIZE / 2.0f;
        Vec2 halfSize = FLAG_SIZE / 4.0f; // metade da metade → zoom 2x

        GLdebug {
            glMatrixMode(GL_PROJECTION);
        }
        GLdebug {
            glLoadIdentity(); // sempre limpe antes de aplicar nova projeção
        }
        GLdebug {
            gluOrtho2D(
                center.x - halfSize.x,
                center.x + halfSize.x,
                center.y - halfSize.y,
                center.y + halfSize.y
            );
        }
    }
    else {
        GLdebug {
            gluOrtho2D(0.0, FLAG_SIZE.x, 0.0, FLAG_SIZE.y); // coordenadas limite do viewport normalizadas (em 2D)
        }
    }

    return EXIT_SUCCESS;
}

// Referência para a construção matemática da bandeira: <https://youtu.be/yBjX9jLuLSY>
/* Loop principal de desenho. */
void display()
{
    Gui::newFrame();

    using namespace cg;

    GLdebug {
        glClear(GL_COLOR_BUFFER_BIT); // Limpa o quadro do buffer de cor
    }

    GLdebug {
        glColor3ub(FlagColors::YELLOW.r, FlagColors::YELLOW.g, FlagColors::YELLOW.b);
    }

    // Desenha o diamante (losango) da bandeira do Brasil
    float diamondOffset = 1.7f;
    GLdebug {
        glBegin(GL_POLYGON);
            glVertex2f(FLAG_SIZE.x / 2.0f, FLAG_SIZE.y - diamondOffset);
            glVertex2f(FLAG_SIZE.x - diamondOffset, FLAG_SIZE.y / 2.0f);
            glVertex2f(FLAG_SIZE.x / 2.0f, diamondOffset);
            glVertex2f(diamondOffset, FLAG_SIZE.y / 2.0f);
        glEnd();
    }

    // Desenha o círculo da bandeira do Brasil
    static constexpr Vec2 CENTER{FLAG_SIZE / 2.0f};
    static constexpr float RADIUS = 3.5f;

    GLdebug {
        glColor3ub(FlagColors::BLUE.r, FlagColors::BLUE.g, FlagColors::BLUE.b);
    }
    GLdebug {
        genCircleAuto(CENTER, RADIUS, 10.0f);
    }

    // Desenha a faixa da bandeira, composta por arcos
    GLdebug {
        glColor3ub(FlagColors::WHITE.r, FlagColors::WHITE.g, FlagColors::WHITE.b);
    }
    const Vec2 arcCenter{CENTER.x - 2.0f, 0.0f};
    const float arcInnerRadius = 8.0f, arcOuterRadius = 8.5f;

    genSemiArcOverCircle( arcCenter,
        arcInnerRadius, arcOuterRadius, // radius: inner, outer
        CENTER, 3.5f,
        32        // número de segmentos (suavidade)
    );

    Vec2 starCoords = CENTER;
    starCoords.y -= 0.5;

    inline void randomizeColor();
    static constexpr float starSizes[] = {.21f, .17f, .13f, .1f, .08f};
    struct Star {
        const float CELL_SCALAR = RADIUS / 10.4f;
        float radius;
        Vec2 coords;

        /**
         * @param size Tamanho da estrela (1-5), referente às posições em starSizes.
         * @param radialCoords Coordenadas em relação ao centro do mapa, numa grade de 1/9.5 do raio por quadrante.
         * Referência: <https://upload.wikimedia.org/wikipedia/commons/b/be/Flag_of_Brazil_%28dimensions%29.svg>.
         */
        Star(uint8_t size, Vec2 radialCoords) : radius(starSizes[size - 1]), coords(radialCoords) {
            coords.y -= 0.5f;
            coords = CENTER + coords * CELL_SCALAR;
            genStar(coords, radius, 0.45f);
        }
    };
    /* Estrelas da bandeira do Brasil */
    // Referências: <https://pt.wikipedia.org/wiki/Bandeira_do_Brasil#/media/Ficheiro:Flag_of_Brazil_(sphere).svg> (estrelas)
    // <https://pt.wikipedia.org/wiki/Bandeira_do_Brasil#/media/Ficheiro:Brazil_flag_stars.svg> (constelações)

    //randomizeColor();
    // Virgo
    Star spica(1, {3, 4}); // Pará

    //randomizeColor();
    // Hydra
    Star dhanabAlShuja(3, {2.25f, 1.25f}); // Acre
    Star alphard(2, {-3.75f, -.75f}); // Mato Grosso do Sul

    //randomizeColor();
    // Scorpius
    Star graffias(3, {8.25f, -3.75f}); // Maranhão
    Star antares(1, {6.75f, -3.75f}); // Piauí
    Star wei(2, {7.25f, -4.75f}); // Ceará
    Star shaula(2, {6.75f, -5.4f}); // Rio Grande do Norte
    //randomizeColor();
    Star girtab(3, {5.75f, -5.75f}); // Paraiba
    Star denebakrab(3, {4.75f, -5.75f}); // Pernambuco
    Star sargas(2, {4.75f, -6.75f}); // Alagoas
    Star apollyon(3, {4.75f, -7.75f}); // Sergipe

    //randomizeColor();
    // Triangulum Australe
    Star deltaTrianguliAustralis(3, {3.75f, -6}); // Santa Catarina
    Star atria(2, {2.75f, -7}); // Rio Grande do Sul
    Star gamaTrianguliAustralis(3, {2, -5.75f}); // Paraná

    //randomizeColor();
    // Octans
    Star polarisAustralis(5, {0, -8}); // Distrito Federal

    //randomizeColor();
    // Crux
    Star acrux(1, {0, -4.8f}); // São Paulo
    Star mimosa(2, {1, -2.9f}); // Rio de Janeiro
    Star intrometida(4, {0.5f, -3.5f}); // Espirito Santo
    Star palida(3, {-1.25f, -3}); // Minas Gerais
    Star gacrux(2, {.15f, -2}); // Bahia

    //randomizeColor();
    // Carina
    Star canopus(1, {-4, -5.75f}); // Goiás

    //randomizeColor();
    // Canis Major
    Star adhara(3, {-5.25f, -4.75f}); // Tocantins
    Star wezen(2, {-5, -3.8f}); // Roraima
    Star mirzam(2, {-8, -3.6f}); // Amapá
    Star sirius(1, {-7, -2.8f}); // Mato Grosso
    //randomizeColor();
    Star muliphen(4, {-6.15f, -2.15f}); // Rondônia

    // Canis Minor
    Star procyon(1, {-7.8f, 1.2f}); // Amazonas

    /* Texto da faixa */ 
    GLdebug {
        glColor3ub(FlagColors::GREEN.r, FlagColors::GREEN.g, FlagColors::GREEN.b);
    }
    GLdebug {
        glPushMatrix();
    }

    const float midRadius = (arcInnerRadius + arcOuterRadius) / 2.0f; // raio médio da faixa
    // Calcular ângulos do arco visível
    auto [startAngle, endAngle] = computeArcAngles(arcCenter, midRadius, CENTER, RADIUS);

    GLdebug{
        glTranslatef(arcCenter.x, arcCenter.y, 0.0f); // Centralizar no centro do arco
    }

    // Desenhar texto na faixa
    drawArcText("ORDEM E PROGRESSO", midRadius - 0.22f, endAngle, startAngle, 0.004f);
    GLdebug{
        glPopMatrix();
    }

    // Mostra uma janela simples.
    // Cada janela ser criada num escopo separado para invocar
    // os construtores/ destrutores necessários
    {
        static int counter = 0;

        static bool check = false;
        static float f = 0.0f;
        static Vec3 color;

        Window test("Hello, world!");

        test.showText("Teste!");
        test.showCheckBox(&check, "Active");
        test.showSliderFloat(&f, "float");
        test.showColorEdit(&color, "color");
        
        if (test.showButton("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        test.sameLine();

        test.showText("counter = %d", counter);

        
        test.showText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Gui::getFps(), Gui::getFps());
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

    // Lógica personalizada de redimensionamento abaixo
}


int main(int argc, char** argv) {
    // 1. Inicialização do GLUT
    glutInit(&argc, argv);
    
    // modo de exibição: frame buffer, modelo de cor, e antialias ativado
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    // double buffering é necessário para tratamentos visuais dinâmicos
    // [requisitado pela GUI]

    //glEnable(GL_MULTISAMPLE);
    glutInitWindowSize(WINDOW_SIZE.x, WINDOW_SIZE.y); // tamanho da área interna da janela (coordenadas de tela)
    glutCreateWindow("Trabalho CG [Open GL]: Bandeira do Brasil");

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
