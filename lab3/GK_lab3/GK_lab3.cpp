#define _USE_MATH_DEFINES
#include <windows.h>
#include <gl/gl.h>
#include <gl/glut.h>
#include <cmath>
#include <iostream>
#include <stdlib.h> 
#include <time.h>

using namespace std;

typedef double point3[3];
static GLdouble theta[] = { 0.0, 0.0, 0.0 }; // trzy kąty obrotu
point3* colours;
int n_points;
int model; 

void Axes(void)
{

	point3  x_min = { -5.0, 0.0, 0.0 };
	point3  x_max = { 5.0, 0.0, 0.0 };
	// początek u koniec obrazu osi x

	point3  y_min = { 0.0, -5.0, 0.0 };
	point3  y_max = { 0.0,  5.0, 0.0 };
	// początek u koniec obrazu osi y

	point3  z_min = { 0.0, 0.0, -5.0 };
	point3  z_max = { 0.0, 0.0,  5.0 };
	//  początek u koniec obrazu osi y
	glColor3f(1.0f, 0.0f, 0.0f);  // kolor rysowania osi - czerwony
	glBegin(GL_LINES); // rysowanie osi x
	glVertex3dv(x_min);
	glVertex3dv(x_max);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);  // kolor rysowania - zielony
	glBegin(GL_LINES);  // rysowanie osi y

	glVertex3dv(y_min);
	glVertex3dv(y_max);
	glEnd();

	glColor3f(0.0f, 0.0f, 1.0f);  // kolor rysowania - niebieski
	glBegin(GL_LINES); // rysowanie osi z

	glVertex3dv(z_min);
	glVertex3dv(z_max);
	glEnd();

}

void Egg(int n_points)
{

	glColor3f(0.0f, 1.0f, 0.0f);
	point3* points = new point3[n_points * n_points];
	double u, v;
	int id = 0;
	for (int i = 0; i < n_points; i++)
		for (int j = 0; j < n_points; j++)
		{
			u = i / ((double)(n_points - 1) * 2.0);
			v = j / ((double)n_points / 2.0);
			points[id][0] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * cos(M_PI * v);
			points[id][1] = (160 * pow(u, 4) - 320 * pow(u, 3) + 160 * pow(u, 2) - 5);
			points[id][2] = (-90 * pow(u, 5) + 225 * pow(u, 4) - 270 * pow(u, 3) + 180 * pow(u, 2) - 45 * u) * sin(M_PI * v);
			id++;
		}

	if (model == 1)
	{
		glBegin(GL_POINTS);
		for (int id = 0; id < n_points * n_points; id++)
			glVertex3d(points[id][0], points[id][1], points[id][2]);
		glEnd();
	}
	if (model == 2)
	{
		for (int i = 0; i < n_points * n_points; i += n_points)
		{
			for (int j = i; j < i + n_points - 1; j++)
			{
				glBegin(GL_LINES);
				glVertex3d(points[j][0], points[j][1], points[j][2]);
				glVertex3d(points[j + 1][0], points[j + 1][1], points[j + 1][2]);
				glEnd();
			}
			glBegin(GL_LINES);
			glVertex3d(points[i + n_points - 1][0], points[i + n_points - 1][1], points[i + n_points - 1][2]);
			glVertex3d(points[i][0], points[i][1], points[i][2]);
			glEnd();
		}

		for (int i = 0; i < n_points; i++)
		{
			for (int j = i; j < n_points * n_points - n_points; j += n_points)
			{
				glBegin(GL_LINES);
				glVertex3d(points[j][0], points[j][1], points[j][2]);
				glVertex3d(points[j + n_points][0], points[j + n_points][1], points[j + n_points][2]);
				glEnd();
			}
		}
	}
	if (model == 3)
	{
		int a, b, c, d;
		for (int i = 0; i < n_points; i++)
		{
			for (int j = i; j < n_points * n_points - n_points; j += n_points)
			{
				a = j;
				b = j + n_points;
				c = (j + 1) % (n_points * n_points);
				d = (j + 1) % (n_points * n_points) + n_points;

				glBegin(GL_TRIANGLES);
				glColor3d(colours[a][0], colours[a][1], colours[a][2]);
				glVertex3d(points[a][0], points[a][1], points[a][2]);
				glColor3d(colours[b][0], colours[b][1], colours[b][2]);
				glVertex3d(points[b][0], points[b][1], points[b][2]);
				glColor3d(colours[c][0], colours[c][1], colours[c][2]);
				glVertex3d(points[c][0], points[c][1], points[c][2]);
				glEnd();

				glBegin(GL_TRIANGLES);
				glColor3d(colours[b][0], colours[b][1], colours[b][2]);
				glVertex3d(points[b][0], points[b][1], points[b][2]);
				glColor3d(colours[c][0], colours[c][1], colours[c][2]);
				glVertex3d(points[c][0], points[c][1], points[c][2]);
				glColor3d(colours[d][0], colours[d][1], colours[d][2]);
				glVertex3d(points[d][0], points[d][1], points[d][2]);
				glEnd();
			}
		}
	}
	delete[] points;
	
	if (model == 4)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glutWireTeapot(3.0);
	}
}


void RenderScene(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Czyszczenie okna aktualnym kolorem czyszczącym

	glLoadIdentity();
	// Czyszczenie macierzy bieżącej

	// Narysowanie osi przy pomocy funkcji zdefiniowanej wyżej
	glRotated(theta[0], 1.0, 0.0, 0.0);
	glRotated(theta[1], 0.0, 1.0, 0.0);
	glRotated(theta[2], 0.0, 0.0, 1.0);
	//Axes();
	Egg(n_points);
	Sleep(25);
	glFlush();
	// Przekazanie poleceń rysujących do wykonania
		glutSwapBuffers();
}

void keys(unsigned char key, int x, int y)
{
	if (key == 'p')
		model = 1;
	if (key == 'w')
		model = 2;
	if (key == 's')
		model = 3;
	if (key == 't')
		model = 4;
	RenderScene(); // przerysowywanie obrazu sceny
}

void spinEgg()
{
	theta[0] = theta[0] - 0.5;
	if (theta[0] > 360)
		theta[0] = theta[0] - 360;

	theta[1] = theta[1] - 0.5;
	if (theta[1] > 360)
		theta[1] = theta[1] - 360;

	theta[2] = theta[2] - 0.5;
	if (theta[2] > 360)
		theta[2] = theta[2] - 360;

	glutPostRedisplay();
}


void MyInit(void)
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Kolor czyszcący (wypełnienia okna) ustawiono na czarny

}

void ChangeSize(GLsizei horizontal, GLsizei vertical)
{

	GLfloat AspectRatio;
	// Deklaracja zmiennej AspectRatio  określającej proporcję
	// wymiarów okna 
	if (vertical == 0)  // Zabezpieczenie przed dzieleniem przez 0
		vertical = 1;
	glViewport(0, 0, horizontal, vertical);
	// Ustawienie wielkościokna okna widoku (viewport)
	// W tym przypadku od (0,0) do (horizontal, vertical)  
	glMatrixMode(GL_PROJECTION);
	// Przełączenie macierzy bieżącej na macierz projekcji 
	glLoadIdentity();
	// Czyszcznie macierzy bieżącej            
	AspectRatio = (GLfloat)horizontal / (GLfloat)vertical;
	// Wyznaczenie współczynnika  proporcji okna
	// Gdy okno nie jest kwadratem wymagane jest określenie tak zwanej
	// przestrzeni ograniczającej pozwalającej zachować właściwe
	// proporcje rysowanego obiektu.
	// Do okreslenia przestrzeni ograniczjącej służy funkcja
	// glOrtho(...)            
	if (horizontal <= vertical)

		glOrtho(-7.5, 7.5, -7.5 / AspectRatio, 7.5 / AspectRatio, 10.0, -10.0);
	else

		glOrtho(-7.5 * AspectRatio, 7.5 * AspectRatio, -7.5, 7.5, 10.0, -10.0);
	glMatrixMode(GL_MODELVIEW);
	// Przełączenie macierzy bieżącej na macierz widoku modelu                                   

	glLoadIdentity();
	// Czyszcenie macierzy bieżącej
}

/*************************************************************************************/

// Główny punkt wejścia programu. Program działa w trybie konsoli

void menu()
{
	
	cout << "Podaj wartosc liczby N: ";
	cin >> n_points;
	while (n_points < 0)
	{
		cout << "Podana niepoprawna wartosc. Sprobuj ponownie: ";
		cin >> n_points;
	}
	cout << "Celem zmiany wyświetlanego modelu nacisnij klawisz:"<<endl
		<<" - p - dla modelu z punktow " << endl
		<< " - w - dla modelu siatkowego"<<endl
		<< " - s - dla modelu kolorowego"<<endl
		<<" - t - dla modelu czajniczka do herbaty";
	cout << endl;
}

int main()
{

	menu();
	colours = new point3[n_points * n_points];
	srand(time(NULL));
	for (int i = 0; i < n_points * n_points; i++)
	{
		colours[i][0] = (rand() % 101) * 0.01;
		colours[i][1] = (rand() % 101) * 0.01;
		colours[i][2] = (rand() % 101) * 0.01;
	}
	for (int i = 0; i < n_points; i++)
	{
		colours[i][0] = colours[0][0];
		colours[i][1] = colours[0][1];
		colours[i][2] = colours[0][2];
		colours[n_points * n_points - 1 - i][0] = colours[n_points * n_points - 1][0];
		colours[n_points * n_points - 1 - i][1] = colours[n_points * n_points - 1][1];
		colours[n_points * n_points - 1 - i][2] = colours[n_points * n_points - 1][2];
	}
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(300, 300);

	glutCreateWindow("Układ współrzędnych 3-D");

	glutDisplayFunc(RenderScene);
	// Określenie, że funkcja RenderScene będzie funkcją zwrotną
	// (callback function).  Bedzie ona wywoływana za każdym razem
	// gdy zajdzie potrzba przeryswania okna 
	glutReshapeFunc(ChangeSize);
	// Dla aktualnego okna ustala funkcję zwrotną odpowiedzialną
	// zazmiany rozmiaru okna      
	MyInit();
	// Funkcja MyInit() (zdefiniowana powyżej) wykonuje wszelkie
	// inicjalizacje konieczne  przed przystąpieniem do renderowania
	glEnable(GL_DEPTH_TEST);
	// Włączenie mechanizmu usuwania powierzchni niewidocznych

	glutKeyboardFunc(keys);
	glutIdleFunc(spinEgg);

	glutMainLoop();
	// Funkcja uruchamia szkielet biblioteki GLUT

	delete[] colours;
	return 0;
}