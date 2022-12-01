#include <windows.h>
#include<iostream>
#include <gl/gl.h>
#include <gl/glut.h>
#include <chrono>
#include <thread>
using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;
double def_level;
int stopien;
double x = 0, y = 0, width;
void init() 
{
    cout<<"Podaj liczbe powtorzen algorytmu: ";
    cin >> stopien;
    cout <<endl << "Podaj dlugosc boku kwadratu: ";
    cin >> width;
    cout << endl << "Podaj stopien deformacji (0-1): ";
    cin >> def_level;
}
void rysuj(double x, double y, double width, int stopien)
{
    if (stopien == 0)
    {
        float deformation_x = 0.f;
        float deformation_y = 0.f;

        glBegin(GL_POLYGON);
        glColor3f(((rand() % 101) * 0.01), ((rand() % 101) * 0.01), ((rand() % 101) * 0.01));
        deformation_x = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        deformation_y = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        glVertex2f(x + deformation_x, y + width + deformation_y);
        glColor3f(((rand() % 101) * 0.01), ((rand() % 101) * 0.01), ((rand() % 101) * 0.01));
        deformation_x = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        deformation_y = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        glVertex2f(x +  width + deformation_x, y +  width + deformation_y);
        glColor3f(((rand() % 101) * 0.01), ((rand() % 101) * 0.01), ((rand() % 101) * 0.01));
        deformation_x = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        deformation_y = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        glVertex2f(x +  width + deformation_x, y + deformation_y);
        glColor3f(((rand() % 101) * 0.01), ((rand() % 101) * 0.01), ((rand() % 101) * 0.01));
        deformation_x = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        deformation_y = ((float)rand() / RAND_MAX - 0.5f) * width * def_level;
        glVertex2f(x + deformation_x, y + deformation_y);
        glEnd();

    }

    else
    {
        width = width / 3.0f;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++)
                if (i != 1 or j != 1)
                {
                    rysuj(x + i * width, y + j * width, width, stopien - 1);
                }
        }
    }
}
void RenderScene()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
    for (int i = 0; i <= stopien; i++)
    { 
        glClear(GL_COLOR_BUFFER_BIT);
        rysuj(x - width / 2., y - width / 2., width, i);
        glFlush();
        sleep_for(500ms);
    }
}

void MyInit(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

}

void ChangeSize(GLsizei horizontal, GLsizei vertical)

// Parametry horizontal i vertical (szerokość i wysokość okna) są
// przekazywane do funkcji za każdym razem, gdy zmieni się rozmiar okna

{

    GLfloat AspectRatio;

    // Deklaracja zmiennej AspectRatio określającej proporcję wymiarów okna



    if (vertical == 0)
        // Zabezpieczenie pzred dzieleniem przez 0

        vertical = 1;


    glViewport(0, 0, horizontal, vertical);
    // Ustawienie wielkościokna okna urządzenia (Viewport)
    // W tym przypadku od (0,0) do (horizontal, vertical)


    glMatrixMode(GL_PROJECTION);
    // Określenie układu współrzędnych obserwatora

    glLoadIdentity();
    // Określenie przestrzeni ograniczającej

    AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
    // Wyznaczenie współczynnika proporcji okna

    // Gdy okno na ekranie nie jest kwadratem wymagane jest
    // określenie okna obserwatora.
    // Pozwala to zachować właściwe proporcje rysowanego obiektu
    // Do określenia okna obserwatora służy funkcja glOrtho(...)



    if (horizontal <= vertical)

        glOrtho(-100.0, 100.0, -100.0 / AspectRatio, 100.0 / AspectRatio, 1.0, -1.0);

    else

        glOrtho(-100.0 * AspectRatio, 100.0 * AspectRatio, -100.0, 100.0, 1.0, -1.0);

    glMatrixMode(GL_MODELVIEW);
    // Określenie układu współrzędnych    

    glLoadIdentity();

}

int main()
{
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);

    glutCreateWindow("Dywan Sierpińskiego");
    init();

    glutDisplayFunc(RenderScene);

    glutReshapeFunc(ChangeSize);

    MyInit();

    glutMainLoop();

    return 0;
}
