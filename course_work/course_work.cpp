#include <iostream>
#include <math.h>
#include "Paint.h"

using namespace std;

#define N 4 // размер столбцов матриц
#define M_prism 8 // количество точек в фигуре
#define M_pyramid 4 // количество точек в фигуре
#define angle 0.0174444444444444

struct square // структура грани призмы
{
	point P1;
	point P2;
	point P3;
	point P4;
	double mid_z = 0;

	COLORREF clr = rgbYellow;
};

struct triangle // структура грани пирамиды
{
	point P1;
	point P2;
	point P3;
	double mid_z = 0;

	COLORREF clr = rgbYellow;
};

void mul_matrix_prism(double fig[M_prism][N], double mass[N][N]);
void mul_matrix_pyramid(double fig[M_pyramid][N], double mass[N][N]);
void scale(double fig[M_prism][N], double);
void rotateX(double fig[M_prism][N], double);
void rotateY(double fig[M_prism][N], double);
void rotateZ(double fig[M_prism][N], double);
void move(double fig[M_prism][N], double = 0, double = 0, double = 0);

void scale_pyramid(double fig[M_pyramid][N], double);
void rotateX_pyramid(double fig[M_pyramid][N], double);
void rotateY_pyramid(double fig[M_pyramid][N], double);
void rotateZ_pyramid(double fig[M_pyramid][N], double);
void move_pyramid(double fig[M_pyramid][N], double = 0, double = 0, double = 0);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Создаём прототип функции окна, которая будет определена ниже

char szProgName[] = "Курсовая работа"; // объявляем строку-имя программы

double kabinet[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {sqrt(2) / 4, sqrt(2) / 4, 0, 1},
						 {0, 0, 0, 0} }; // кабинетная проекция

double prism[M_prism][N] = { {100, 400, 100, 1}, // левый низ близ
							 {100, 320, 100, 1}, // левый верх близ
							 {180, 320, 100, 1}, // правый верз близ
							 {180, 400, 100, 1}, // правый низ близ
							 {100, 400, 180, 1}, // левый низ дал
							 {100, 320, 180, 1}, // левый верх дал
							 {180, 320, 180, 1}, // правый верх дал
							 {180, 400, 180, 1} }; // правый низ дал

double pyramid[M_pyramid][N] = { {600, 400, 100, 1}, // левый низ близ
								 {680, 400, 100, 1}, // правый низ близ
								 {640, 400, 140, 1}, // центр низ дал
								 {640, 320, 120, 1}, }; // вершина

point light{1.0f, -100.0f, 1.0f};

double COPY_prism[M_prism][N];
double lilfig_prism[4][2];
square FIG_prism[6];

double COPY_pyramid[M_pyramid][N];
double lilfig_pyramid[3][2];
triangle FIG_pyramid[4];

int kontur = 0;
int figur = 0;
int speed = 1;

int WINAPI WinMain (HINSTANCE _In_ hInstance, HINSTANCE _In_opt_ hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS w; //создаём экземпляр структуры WNDCLASS

	// И начинаем её заполнять
	w.lpszClassName = szProgName; //имя программы - объявлено выше
	w.hInstance = hInstance; //идентификатор текущего приложения
	w.lpfnWndProc = WndProc; //указатель на функцию окна
	w.hCursor = LoadCursor(NULL, IDC_ARROW); //загружаем курсор
	w.hIcon = 0; //иконки у нас не будет пока
	w.lpszMenuName = 0; //и меню пока не будет
	w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH); //цвет фона окна
	w.style = CS_HREDRAW | CS_VREDRAW; //стиль - перерисовываемое по х и по у
	w.cbClsExtra = 0;
	w.cbWndExtra = 0;

	//Если не удалось зарегистрировать класс окна - выходим
	if (!RegisterClass(&w))
	{
		return 0;
	}

	//Создадим окно в памяти, заполнив аргументы CreateWindow
	hWnd = CreateWindow(szProgName, //Имя программы
		"Курсовая работа", //Заголовок окна
		WS_OVERLAPPEDWINDOW, //Стиль окна - перекрывающееся
		0, //положение окна на экране по х
		0, //положение по у
		1500, //ширина
		800, //высота
		(HWND)NULL, //идентификатор родительского окна
		(HMENU)NULL, //идентификатор меню
		(HINSTANCE)hInstance, //идентификатор экземпляра программы
		(HINSTANCE)NULL); //отсутствие дополнительных параметров

	ShowWindow(hWnd, nCmdShow); // Выводим окно из памяти на экран
	UpdateWindow(hWnd); // Обновим содержимое окна

	//Цикл обработки сообщений
	while (GetMessage(&lpMsg, NULL, 0, 0)) // Получаем сообщение из очереди
	{
		TranslateMessage(&lpMsg); // Преобразует сообщения клавиш в символы
		DispatchMessage(&lpMsg); // Передаёт сообщение соответствующей функции окна
	}
	return(lpMsg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) // Функция окна
{
	HDC hdc; //создаём контекст устройства
	PAINTSTRUCT ps; //создаём экземпляр структуры графического вывода

	//Цикл обработки сообщений
	switch (messg)
	{
	case WM_PAINT: // сообщение рисования

		for (int i = 0; i < M_prism; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY_prism[i][j] = prism[i][j];
			}
		}

		for (int i = 0; i < M_pyramid; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY_pyramid[i][j] = pyramid[i][j];
			}
		}

		mul_matrix_prism(COPY_prism, kabinet);
		mul_matrix_pyramid(COPY_pyramid, kabinet);

		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		// призму
		Line(hdc, (int)COPY_prism[0][0], (int)COPY_prism[0][1], (int)COPY_prism[1][0], (int)COPY_prism[1][1], rgbBlue);
		Line(hdc, (int)COPY_prism[1][0], (int)COPY_prism[1][1], (int)COPY_prism[2][0], (int)COPY_prism[2][1], rgbBlue);
		Line(hdc, (int)COPY_prism[2][0], (int)COPY_prism[2][1], (int)COPY_prism[3][0], (int)COPY_prism[3][1], rgbBlue);
		Line(hdc, (int)COPY_prism[3][0], (int)COPY_prism[3][1], (int)COPY_prism[0][0], (int)COPY_prism[0][1], rgbBlue);

		Line(hdc, (int)COPY_prism[4][0], (int)COPY_prism[4][1], (int)COPY_prism[5][0], (int)COPY_prism[5][1], rgbRed);
		Line(hdc, (int)COPY_prism[5][0], (int)COPY_prism[5][1], (int)COPY_prism[6][0], (int)COPY_prism[6][1], rgbRed);
		Line(hdc, (int)COPY_prism[6][0], (int)COPY_prism[6][1], (int)COPY_prism[7][0], (int)COPY_prism[7][1], rgbRed);
		Line(hdc, (int)COPY_prism[7][0], (int)COPY_prism[7][1], (int)COPY_prism[4][0], (int)COPY_prism[4][1], rgbRed);

		Line(hdc, (int)COPY_prism[1][0], (int)COPY_prism[1][1], (int)COPY_prism[5][0], (int)COPY_prism[5][1], rgbCyan);
		Line(hdc, (int)COPY_prism[4][0], (int)COPY_prism[4][1], (int)COPY_prism[0][0], (int)COPY_prism[0][1], rgbCyan);

		Line(hdc, (int)COPY_prism[2][0], (int)COPY_prism[2][1], (int)COPY_prism[6][0], (int)COPY_prism[6][1], rgbGreen);
		Line(hdc, (int)COPY_prism[7][0], (int)COPY_prism[7][1], (int)COPY_prism[3][0], (int)COPY_prism[3][1], rgbGreen);

		FIG_prism[0].P1.x = COPY_prism[0][0];
		FIG_prism[0].P1.y = COPY_prism[0][1];
		FIG_prism[0].P1.z = prism[0][2];
		FIG_prism[0].P2.x = COPY_prism[1][0];
		FIG_prism[0].P2.y = COPY_prism[1][1];
		FIG_prism[0].P2.z = prism[1][2];
		FIG_prism[0].P3.x = COPY_prism[2][0];
		FIG_prism[0].P3.y = COPY_prism[2][1];
		FIG_prism[0].P3.z = prism[2][2];
		FIG_prism[0].P4.x = COPY_prism[3][0];
		FIG_prism[0].P4.y = COPY_prism[3][1];
		FIG_prism[0].P4.z = prism[3][2];
		FIG_prism[0].clr = rgbBlue;

		FIG_prism[1].P1.x = COPY_prism[4][0];
		FIG_prism[1].P1.y = COPY_prism[4][1];
		FIG_prism[1].P1.z = prism[4][2];
		FIG_prism[1].P2.x = COPY_prism[5][0];
		FIG_prism[1].P2.y = COPY_prism[5][1];
		FIG_prism[1].P2.z = prism[5][2];
		FIG_prism[1].P3.x = COPY_prism[6][0];
		FIG_prism[1].P3.y = COPY_prism[6][1];
		FIG_prism[1].P3.z = prism[6][2];
		FIG_prism[1].P4.x = COPY_prism[7][0];
		FIG_prism[1].P4.y = COPY_prism[7][1];
		FIG_prism[1].P4.z = prism[7][2];
		FIG_prism[1].clr = rgbRed;

		FIG_prism[2].P1.x = COPY_prism[0][0];
		FIG_prism[2].P1.y = COPY_prism[0][1];
		FIG_prism[2].P1.z = prism[0][2];
		FIG_prism[2].P2.x = COPY_prism[1][0];
		FIG_prism[2].P2.y = COPY_prism[1][1];
		FIG_prism[2].P2.z = prism[1][2];
		FIG_prism[2].P3.x = COPY_prism[5][0];
		FIG_prism[2].P3.y = COPY_prism[5][1];
		FIG_prism[2].P3.z = prism[5][2];
		FIG_prism[2].P4.x = COPY_prism[4][0];
		FIG_prism[2].P4.y = COPY_prism[4][1];
		FIG_prism[2].P4.z = prism[4][2];
		FIG_prism[2].clr = rgbCyan;

		FIG_prism[3].P1.x = COPY_prism[2][0];
		FIG_prism[3].P1.y = COPY_prism[2][1];
		FIG_prism[3].P1.z = prism[2][2];
		FIG_prism[3].P2.x = COPY_prism[3][0];
		FIG_prism[3].P2.y = COPY_prism[3][1];
		FIG_prism[3].P2.z = prism[3][2];
		FIG_prism[3].P3.x = COPY_prism[7][0];
		FIG_prism[3].P3.y = COPY_prism[7][1];
		FIG_prism[3].P3.z = prism[7][2];
		FIG_prism[3].P4.x = COPY_prism[6][0];
		FIG_prism[3].P4.y = COPY_prism[6][1];
		FIG_prism[3].P4.z = prism[6][2];
		FIG_prism[3].clr = rgbGreen;

		FIG_prism[4].P1.x = COPY_prism[1][0];
		FIG_prism[4].P1.y = COPY_prism[1][1];
		FIG_prism[4].P1.z = prism[1][2];
		FIG_prism[4].P2.x = COPY_prism[2][0];
		FIG_prism[4].P2.y = COPY_prism[2][1];
		FIG_prism[4].P2.z = prism[2][2];
		FIG_prism[4].P3.x = COPY_prism[6][0];
		FIG_prism[4].P3.y = COPY_prism[6][1];
		FIG_prism[4].P3.z = prism[6][2];
		FIG_prism[4].P4.x = COPY_prism[5][0];
		FIG_prism[4].P4.y = COPY_prism[5][1];
		FIG_prism[4].P4.z = prism[5][2];
		FIG_prism[4].clr = rgbOlive;

		FIG_prism[5].P1.x = COPY_prism[4][0];
		FIG_prism[5].P1.y = COPY_prism[4][1];
		FIG_prism[5].P1.z = prism[4][2];
		FIG_prism[5].P2.x = COPY_prism[0][0];

		FIG_prism[5].P2.y = COPY_prism[0][1];
		FIG_prism[5].P2.z = prism[0][2];
		FIG_prism[5].P3.x = COPY_prism[3][0];
		FIG_prism[5].P3.y = COPY_prism[3][1];
		FIG_prism[5].P3.z = prism[3][2];
		FIG_prism[5].P4.x = COPY_prism[7][0];
		FIG_prism[5].P4.y = COPY_prism[7][1];
		FIG_prism[5].P4.z = prism[7][2];
		FIG_prism[5].clr = rgbMagenta;

		for (int q = 0; q < 6; q++)
		{
			FIG_prism[q].mid_z = (FIG_prism[q].P1.z + FIG_prism[q].P2.z + FIG_prism[q].P3.z + FIG_prism[q].P4.z) / 4;
		}

		for (int q = 0; q < 5; q++)
		{
			for (int v = 0; v < 5 - q; v++)
			{
				if (FIG_prism[v].mid_z > FIG_prism[v + 1].mid_z)
				{
					square TMP = FIG_prism[v];
					FIG_prism[v] = FIG_prism[v + 1];
					FIG_prism[v + 1] = TMP;
				}
			}
		}

		for (int q = 0; q < 6; q++)
		{
			lilfig_prism[0][0] = FIG_prism[q].P1.x;
			lilfig_prism[0][1] = FIG_prism[q].P1.y;

			lilfig_prism[1][0] = FIG_prism[q].P2.x;
			lilfig_prism[1][1] = FIG_prism[q].P2.y;

			lilfig_prism[2][0] = FIG_prism[q].P3.x;
			lilfig_prism[2][1] = FIG_prism[q].P3.y;

			lilfig_prism[3][0] = FIG_prism[q].P4.x;
			lilfig_prism[3][1] = FIG_prism[q].P4.y;

			if (figur == 1)
			{
				Line(hdc, (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], rgbBorder);

				if (speed == 1)
				{
					PolyScan(hdc, lilfig_prism, FIG_prism[q].clr);
				}
				else
				{
					Zatravrka(hdc, lilfig_prism, FIG_prism[q].clr);
				}
			}

			if (kontur == 1)
			{
				Line(hdc, (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], (int)lilfig_prism[1][0], (int)lilfig_prism[1][1]);
				Line(hdc, (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], (int)lilfig_prism[2][0], (int)lilfig_prism[2][1]);
				Line(hdc, (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], (int)lilfig_prism[3][0], (int)lilfig_prism[3][1]);
				Line(hdc, (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], (int)lilfig_prism[0][0], (int)lilfig_prism[0][1]);
			}
			else
			{
				Line(hdc, (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], FIG_prism[q].clr);
				Line(hdc, (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], FIG_prism[q].clr);
				Line(hdc, (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], FIG_prism[q].clr);
				Line(hdc, (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], FIG_prism[q].clr);
			}
		}

		for (int i = 0; i < M_prism; i++)
		{
			COPY_prism[i][0] -= light.x * (COPY_prism[i][1] / light.y);
			COPY_prism[i][2] = prism[i][2] - light.z * (COPY_prism[i][1] / light.y);
			COPY_prism[i][1] = 0.90f * 700 + COPY_prism[i][2] * 0.1f;
		}

		FIG_prism[0].P1.x = COPY_prism[0][0];
		FIG_prism[0].P1.y = COPY_prism[0][1];
		FIG_prism[0].P2.x = COPY_prism[1][0];
		FIG_prism[0].P2.y = COPY_prism[1][1];
		FIG_prism[0].P3.x = COPY_prism[2][0];
		FIG_prism[0].P3.y = COPY_prism[2][1];
		FIG_prism[0].P4.x = COPY_prism[3][0];
		FIG_prism[0].P4.y = COPY_prism[3][1];

		FIG_prism[1].P1.x = COPY_prism[4][0];
		FIG_prism[1].P1.y = COPY_prism[4][1];
		FIG_prism[1].P2.x = COPY_prism[5][0];
		FIG_prism[1].P2.y = COPY_prism[5][1];
		FIG_prism[1].P3.x = COPY_prism[6][0];
		FIG_prism[1].P3.y = COPY_prism[6][1];
		FIG_prism[1].P4.x = COPY_prism[7][0];
		FIG_prism[1].P4.y = COPY_prism[7][1];

		FIG_prism[2].P1.x = COPY_prism[0][0];
		FIG_prism[2].P1.y = COPY_prism[0][1];
		FIG_prism[2].P2.x = COPY_prism[1][0];
		FIG_prism[2].P2.y = COPY_prism[1][1];
		FIG_prism[2].P3.x = COPY_prism[5][0];
		FIG_prism[2].P3.y = COPY_prism[5][1];
		FIG_prism[2].P4.x = COPY_prism[4][0];
		FIG_prism[2].P4.y = COPY_prism[4][1];

		FIG_prism[3].P1.x = COPY_prism[2][0];
		FIG_prism[3].P1.y = COPY_prism[2][1];
		FIG_prism[3].P2.x = COPY_prism[3][0];
		FIG_prism[3].P2.y = COPY_prism[3][1];
		FIG_prism[3].P3.x = COPY_prism[7][0];
		FIG_prism[3].P3.y = COPY_prism[7][1];
		FIG_prism[3].P4.x = COPY_prism[6][0];
		FIG_prism[3].P4.y = COPY_prism[6][1];

		FIG_prism[4].P1.x = COPY_prism[1][0];
		FIG_prism[4].P1.y = COPY_prism[1][1];
		FIG_prism[4].P2.x = COPY_prism[2][0];
		FIG_prism[4].P2.y = COPY_prism[2][1];
		FIG_prism[4].P3.x = COPY_prism[6][0];
		FIG_prism[4].P3.y = COPY_prism[6][1];
		FIG_prism[4].P4.x = COPY_prism[5][0];
		FIG_prism[4].P4.y = COPY_prism[5][1];

		FIG_prism[5].P1.x = COPY_prism[4][0];
		FIG_prism[5].P1.y = COPY_prism[4][1];
		FIG_prism[5].P2.x = COPY_prism[0][0];
		FIG_prism[5].P2.y = COPY_prism[0][1];
		FIG_prism[5].P3.x = COPY_prism[3][0];
		FIG_prism[5].P3.y = COPY_prism[3][1];
		FIG_prism[5].P4.x = COPY_prism[7][0];
		FIG_prism[5].P4.y = COPY_prism[7][1];

		for (int q = 0; q < 6; q++)
		{
			lilfig_prism[0][0] = FIG_prism[q].P1.x;
			lilfig_prism[0][1] = FIG_prism[q].P1.y;

			lilfig_prism[1][0] = FIG_prism[q].P2.x;
			lilfig_prism[1][1] = FIG_prism[q].P2.y;

			lilfig_prism[2][0] = FIG_prism[q].P3.x;
			lilfig_prism[2][1] = FIG_prism[q].P3.y;

			lilfig_prism[3][0] = FIG_prism[q].P4.x;
			lilfig_prism[3][1] = FIG_prism[q].P4.y;

			if (speed == 1)
			{
				PolyScan(hdc, lilfig_prism);
			}
			else
			{
				Line(hdc, (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], rgbBorder);
				Line(hdc, (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], rgbBorder);

				Zatravrka(hdc, lilfig_prism);
			}

			Line(hdc, (int)lilfig_prism[0][0], (int)lilfig_prism[0][1], (int)lilfig_prism[1][0], (int)lilfig_prism[1][1]);
			Line(hdc, (int)lilfig_prism[1][0], (int)lilfig_prism[1][1], (int)lilfig_prism[2][0], (int)lilfig_prism[2][1]);
			Line(hdc, (int)lilfig_prism[2][0], (int)lilfig_prism[2][1], (int)lilfig_prism[3][0], (int)lilfig_prism[3][1]);
			Line(hdc, (int)lilfig_prism[3][0], (int)lilfig_prism[3][1], (int)lilfig_prism[0][0], (int)lilfig_prism[0][1]);
		}

		// пирамиду
		Line(hdc, (int)COPY_pyramid[0][0], (int)COPY_pyramid[0][1], (int)COPY_pyramid[1][0], (int)COPY_pyramid[1][1], rgbBlue);
		Line(hdc, (int)COPY_pyramid[1][0], (int)COPY_pyramid[1][1], (int)COPY_pyramid[2][0], (int)COPY_pyramid[2][1], rgbBlue);
		Line(hdc, (int)COPY_pyramid[2][0], (int)COPY_pyramid[2][1], (int)COPY_pyramid[0][0], (int)COPY_pyramid[0][1], rgbBlue);

		Line(hdc, (int)COPY_pyramid[2][0], (int)COPY_pyramid[2][1], (int)COPY_pyramid[3][0], (int)COPY_pyramid[3][1], rgbRed);
		Line(hdc, (int)COPY_pyramid[3][0], (int)COPY_pyramid[3][1], (int)COPY_pyramid[1][0], (int)COPY_pyramid[1][1], rgbRed);

		Line(hdc, (int)COPY_pyramid[3][0], (int)COPY_pyramid[3][1], (int)COPY_pyramid[0][0], (int)COPY_pyramid[0][1], rgbCyan);

		FIG_pyramid[0].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[0].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[0].P1.z = pyramid[0][2];
		FIG_pyramid[0].P2.x = COPY_pyramid[1][0];
		FIG_pyramid[0].P2.y = COPY_pyramid[1][1];
		FIG_pyramid[0].P2.z = pyramid[1][2];
		FIG_pyramid[0].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[0].P3.y = COPY_pyramid[2][1];
		FIG_pyramid[0].P3.z = pyramid[2][2];
		FIG_pyramid[0].clr = rgbBlue;

		FIG_pyramid[1].P1.x = COPY_pyramid[1][0];
		FIG_pyramid[1].P1.y = COPY_pyramid[1][1];
		FIG_pyramid[1].P1.z = pyramid[1][2];
		FIG_pyramid[1].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[1].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[1].P2.z = pyramid[3][2];
		FIG_pyramid[1].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[1].P3.y = COPY_pyramid[2][1];
		FIG_pyramid[1].P3.z = pyramid[2][2];
		FIG_pyramid[1].clr = rgbRed;

		FIG_pyramid[2].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[2].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[2].P1.z = pyramid[0][2];
		FIG_pyramid[2].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[2].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[2].P2.z = pyramid[3][2];
		FIG_pyramid[2].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[2].P3.y = COPY_pyramid[2][1];
		FIG_pyramid[2].P3.z = pyramid[2][2];
		FIG_pyramid[2].clr = rgbCyan;

		FIG_pyramid[3].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[3].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[3].P1.z = pyramid[0][2];
		FIG_pyramid[3].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[3].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[3].P2.z = pyramid[3][2];
		FIG_pyramid[3].P3.x = COPY_pyramid[1][0];
		FIG_pyramid[3].P3.y = COPY_pyramid[1][1];
		FIG_pyramid[3].P3.z = pyramid[1][2];
		FIG_pyramid[3].clr = rgbGreen;

		for (int q = 0; q < 4; q++)
		{
			FIG_pyramid[q].mid_z = (FIG_pyramid[q].P1.z + FIG_pyramid[q].P2.z + FIG_pyramid[q].P3.z) / 3;
		}

		for (int q = 0; q < 3; q++)
		{
			for (int v = 0; v < 3 - q; v++)
			{
				if (FIG_pyramid[v].mid_z > FIG_pyramid[v + 1].mid_z)
				{
					triangle TMP_pyramid = FIG_pyramid[v];
					FIG_pyramid[v] = FIG_pyramid[v + 1];
					FIG_pyramid[v + 1] = TMP_pyramid;
				}
			}
		}

		for (int q = 0; q < 4; q++)
		{
			lilfig_pyramid[0][0] = FIG_pyramid[q].P1.x;
			lilfig_pyramid[0][1] = FIG_pyramid[q].P1.y;

			lilfig_pyramid[1][0] = FIG_pyramid[q].P2.x;
			lilfig_pyramid[1][1] = FIG_pyramid[q].P2.y;

			lilfig_pyramid[2][0] = FIG_pyramid[q].P3.x;
			lilfig_pyramid[2][1] = FIG_pyramid[q].P3.y;

			if (figur == 1)
			{
				Line(hdc, (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], rgbBorder);
				Line(hdc, (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], rgbBorder);
				Line(hdc, (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], rgbBorder);

				if (speed == 1)
				{
					PolyScan_P(hdc, lilfig_pyramid, FIG_pyramid[q].clr);
				}
				else
				{
					Zatravrka_triangle(hdc, lilfig_pyramid, FIG_pyramid[q].clr);
				}
			}

			if (kontur == 1)
			{
				Line(hdc, (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1]);
				Line(hdc, (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1]);
				Line(hdc, (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1]);
			}
			else
			{
				Line(hdc, (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], FIG_pyramid[q].clr);
				Line(hdc, (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], FIG_pyramid[q].clr);
				Line(hdc, (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], FIG_pyramid[q].clr);
			}
		}

		for (int i = 0; i < M_pyramid; i++)
		{
			COPY_pyramid[i][0] -= light.x * (COPY_pyramid[i][1] / light.y);
			COPY_pyramid[i][2] = pyramid[i][2] - light.z * (COPY_pyramid[i][1] / light.y);
			COPY_pyramid[i][1] = 0.90f * 700 + COPY_pyramid[i][2] * 0.1f;
		}

		FIG_pyramid[0].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[0].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[0].P2.x = COPY_pyramid[1][0];
		FIG_pyramid[0].P2.y = COPY_pyramid[1][1];
		FIG_pyramid[0].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[0].P3.y = COPY_pyramid[2][1];

		FIG_pyramid[1].P1.x = COPY_pyramid[1][0];
		FIG_pyramid[1].P1.y = COPY_pyramid[1][1];
		FIG_pyramid[1].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[1].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[1].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[1].P3.y = COPY_pyramid[2][1];

		FIG_pyramid[2].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[2].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[2].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[2].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[2].P3.x = COPY_pyramid[2][0];
		FIG_pyramid[2].P3.y = COPY_pyramid[2][1];

		FIG_pyramid[3].P1.x = COPY_pyramid[0][0];
		FIG_pyramid[3].P1.y = COPY_pyramid[0][1];
		FIG_pyramid[3].P2.x = COPY_pyramid[3][0];
		FIG_pyramid[3].P2.y = COPY_pyramid[3][1];
		FIG_pyramid[3].P3.x = COPY_pyramid[1][0];
		FIG_pyramid[3].P3.y = COPY_pyramid[1][1];

		for (int q = 0; q < 4; q++)
		{
			lilfig_pyramid[0][0] = FIG_pyramid[q].P1.x;
			lilfig_pyramid[0][1] = FIG_pyramid[q].P1.y;

			lilfig_pyramid[1][0] = FIG_pyramid[q].P2.x;
			lilfig_pyramid[1][1] = FIG_pyramid[q].P2.y;

			lilfig_pyramid[2][0] = FIG_pyramid[q].P3.x;
			lilfig_pyramid[2][1] = FIG_pyramid[q].P3.y;

			if (speed == 1)
			{
				PolyScan_P(hdc, lilfig_pyramid);
			}
			else
			{
				Line(hdc, (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], rgbBorder);
				Line(hdc, (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], rgbBorder);
				Line(hdc, (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], rgbBorder);

				Zatravrka_triangle(hdc, lilfig_pyramid);
			}

			Line(hdc, (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1], (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1]);
			Line(hdc, (int)lilfig_pyramid[1][0], (int)lilfig_pyramid[1][1], (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1]);
			Line(hdc, (int)lilfig_pyramid[2][0], (int)lilfig_pyramid[2][1], (int)lilfig_pyramid[0][0], (int)lilfig_pyramid[0][1]);
		}

		//закругляемся
		ValidateRect(hWnd, NULL); // обновляем окно
		EndPaint(hWnd, &ps); // заканчиваем рисовать
		break;

	case WM_KEYDOWN: // Обработка нажатия клавиши
		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		switch (wParam)
		{
		case VK_LEFT: // Обрабатывает клавишу LEFT ARROW (Стрелка влево).	<
			move(prism, -5);
			move_pyramid(pyramid, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_RIGHT: // Обрабатывает клавишу RIGHT ARROW (Стрелка вправо).	>
			move(prism, 5);
			move_pyramid(pyramid, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_UP: // Обрабатывает клавишу UP ARROW (Стрелка вверх).	\/
			move(prism, 0, -5);
			move_pyramid(pyramid, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DOWN: // Обрабатывает клавишу DOWN ARROW (Стрелка вниз).	^
			move(prism, 0, 5);
			move_pyramid(pyramid, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x57: // Обрабатывает клавишу W.
			move(prism, 0, 0, 5);
			move_pyramid(pyramid, 0, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x53: // Обрабатывает клавишу S.
			move(prism, 0, 0, -5);
			move_pyramid(pyramid, 0, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xba: // Обрабатывает клавишу ;.	уменьшение
			scale(prism, 0.9);
			scale_pyramid(pyramid, 0.9);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xde: // Обрабатывает клавишу '.	увеличение
			scale(prism, 1.1);
			scale_pyramid(pyramid, 1.1);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x58: // Обрабатывает клавишу X.
			rotateX(prism, angle);
			rotateX_pyramid(pyramid, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x59: // Обрабатывает клавишу Y.
			rotateY(prism, angle);
			rotateY_pyramid(pyramid, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x5A: // Обрабатывает клавишу Z.
			rotateZ(prism, angle);
			rotateZ_pyramid(pyramid, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_SPACE: // Обрабатывает пробел
			if (kontur == 0)
			{
				kontur = 1;
			}
			else
			{
				kontur = 0;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x46: // Обрабатывает F
			if (figur == 0)
			{
				figur = 1;
			}
			else
			{
				figur = 0;
			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DELETE:
			if (speed == 0)
			{
				speed = 1;
			}
			else
			{
				speed = 0;
			}
			InvalidateRect(hWnd, NULL, TRUE);

		default:
			break;
		}

		/*закругляемся*/
		EndPaint(hWnd, &ps); // заканчиваем рисовать

		break;

	case WM_DESTROY: // сообщение выхода - разрушение окна
		PostQuitMessage(0); // Посылаем сообщение выхода с кодом 0 - нормальное завершение
		break;

	default:
		return(DefWindowProc(hWnd, messg, wParam, lParam)); //освобождаем очередь приложения от нераспознаных
	}
	return 0;
}

void mul_matrix_prism(double fig[M_prism][N], double mass[N][N])
{
	double res[M_prism][N] = { 0,0,0,0 };
	for (int k = 0; k < M_prism; k++)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res[k][i] += fig[k][j] * mass[j][i];
			}
		}
	}
	for (int k = 0; k < M_prism; k++)
	{
		for (int i = 0; i < N; i++)
		{
			fig[k][i] = res[k][i];
		}
	}
}

void mul_matrix_pyramid(double fig[M_pyramid][N], double mass[N][N])
{
	double res[M_pyramid][N] = { 0,0,0,0 };
	for (int k = 0; k < M_pyramid; k++)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res[k][i] += fig[k][j] * mass[j][i];
			}
		}
	}
	for (int k = 0; k < M_pyramid; k++)
	{
		for (int i = 0; i < 3; i++)
		{
			fig[k][i] = res[k][i];
		}
	}
}

double aver_prism(double fig[M_prism][N], int cnt)
{
	double average = 0;
	for (int i = 0; i < 4; i++)
	{
		average += fig[i][cnt];
	}
	return average / 4;
}

double aver_pyramid(double fig[M_pyramid][N], int cnt)
{
	double average = 0;
	for (int i = 0; i < 3; i++)
	{
		average += fig[i][cnt];
	}
	return average / 3;
}

void move(double fig[M_prism][N], double dx, double dy, double dz)
{
	double mover[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 1, 0},
						 {dx, dy, dz, 1} };
	mul_matrix_prism(fig, mover);
}

void move_pyramid(double fig[M_pyramid][N], double dx, double dy, double dz)
{
	double mover[N][N] = { {1, 0, 0, 0},
						   {0, 1, 0, 0},
						   {0, 0, 1, 0},
						   {dx, dy, dz, 1} };
	mul_matrix_pyramid(fig, mover);
}

void rotateX(double fig[M_prism][N], double angl)
{
	double y = 0, z = 0;
	y = aver_prism(fig, 1);
	z = aver_prism(fig, 2);
	double Angle[N][N] = { {1, 0, 0, 0},
						   {0 , cos(angl), sin(angl), 0},
						   {0, -sin(angl), cos(angl), 0},
						   {0, y * (1 - cos(angl)) + z * sin(angl), z * (1 - cos(angl)) - y * sin(angl), 1} };
	mul_matrix_prism(fig, Angle);
}

void rotateX_pyramid(double fig[M_pyramid][N], double angl)
{
	double y = 0, z = 0;
	y = aver_pyramid(fig, 1);
	z = aver_pyramid(fig, 2);
	double Angle[N][N] = { {1, 0, 0, 0},
						   {0 , cos(angl), sin(angl), 0},
						   {0, -sin(angl), cos(angl), 0},
						   {0, y * (1 - cos(angl)) + z * sin(angl), z * (1 - cos(angl)) - y * sin(angl), 1} };
	mul_matrix_pyramid(fig, Angle);
}

void rotateY(double fig[M_prism][N], double angl)
{
	double x = 0, z = 0;
	x = aver_prism(fig, 0);
	z = aver_prism(fig, 2);
	double Angle[N][N] = { {cos(angl), 0, -sin(angl), 0},
						   {0, 1, 0, 0},
						   {sin(angl), 0, cos(angl), 0},
						   {x * (1 - cos(angl)) - z * sin(angl), 0, z * (1 - cos(angl)) + x * sin(angl), 1} };
	mul_matrix_prism(fig, Angle);
}

void rotateY_pyramid(double fig[M_pyramid][N], double angl)
{
	double x = 0, z = 0;
	x = aver_pyramid(fig, 0);
	z = aver_pyramid(fig, 2);
	double Angle[N][N] = { {cos(angl), 0, -sin(angl), 0},
						   {0, 1, 0, 0},
						   {sin(angl), 0, cos(angl), 0},
						   {x * (1 - cos(angl)) - z * sin(angl), 0, z * (1 - cos(angl)) + x * sin(angl), 1} };
	mul_matrix_pyramid(fig, Angle);
}

void rotateZ(double fig[M_prism][N], double angl)
{
	double x = 0, y = 0;
	x = aver_prism(fig, 0);
	y = aver_prism(fig, 1);
	double Angle[N][N] = { {cos(angl), sin(angl), 0, 0},
						   { -sin(angl), cos(angl), 0, 0},
						   {0, 0, 1, 0},
						   {x * (1 - cos(angl)) + y * sin(angl), y * (1 - cos(angl)) - x * sin(angl), 0, 1} };
	mul_matrix_prism(fig, Angle);
}

void rotateZ_pyramid(double fig[M_pyramid][N], double angl)
{
	double x = 0, y = 0;
	x = aver_pyramid(fig, 0);
	y = aver_pyramid(fig, 1);
	double Angle[N][N] = { {cos(angl), sin(angl), 0, 0},
						   { -sin(angl), cos(angl), 0, 0},
						   {0, 0, 1, 0},
						   {x * (1 - cos(angl)) + y * sin(angl), y * (1 - cos(angl)) - x * sin(angl), 0, 1} };
	mul_matrix_pyramid(fig, Angle);
}

void scale(double fig[M_prism][N], double S)
{
	double x = 0, y = 0, z = 0;
	x = aver_prism(fig, 0);
	y = aver_prism(fig, 1);
	z = aver_prism(fig, 2);
	double	Sx_Sy[N][N] = { {S,0,0,0},
							{0,S,0,0},
							{0,0,S,0},
							{x * (1 - S),y * (1 - S),z * (1 - S),1} };
	mul_matrix_prism(fig, Sx_Sy);
}

void scale_pyramid(double fig[M_pyramid][N], double S)
{
	double x = 0, y = 0, z = 0;
	x = aver_pyramid(fig, 0);
	y = aver_pyramid(fig, 1);
	z = aver_pyramid(fig, 2);
	double	Sx_Sy[N][N] = { {S,0,0,0},
							{0,S,0,0},
							{0,0,S,0},
							{x * (1 - S),y * (1 - S),z * (1 - S),1} };
	mul_matrix_pyramid(fig, Sx_Sy);
}