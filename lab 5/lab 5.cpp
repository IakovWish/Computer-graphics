#include <iostream>
#include <math.h>
#include "Paint.h"

using namespace std;

#define N 4 // размер столбцов матриц
#define M 8 // количество точек в фигуре
#define angle 0.0174444444444444

struct square // структура точки
{
	point P1;
	point P2;
	point P3;
	point P4;
	double mid_z;

	int rt = 0;
	int gt = 0;
	int bt = 0;
};

void mul_matrix(double fig[M][N], double mass[N][N]);
void scale(double fig[M][N], double);
void rotateX(double fig[M][N], double);
void rotateY(double fig[M][N], double);
void rotateZ(double fig[M][N], double);
void move(double fig[M][N], int = 0, int = 0, int = 0);
void proek(double fig[M][N]);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Создаём прототип функции окна, которая будет определена ниже

char szProgName[] = "Компьютерная графика ЛР №5"; // объявляем строку-имя программы

double prism[M][N] = { {100, 400, 100, 1},
						 {100, 200, 100, 1},
						 {400, 200, 100, 1},
						 {400, 400, 100, 1},
						 {100, 400, 300, 1},
						 {100, 200, 300, 1},
						 {400, 200, 300, 1},
						 {400, 400, 300, 1} };

double COPY[M][N];
double lilfig[4][2];
square FIG[6];

int kontur = 0;
int figur = 1;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
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
		"Лабораторная работа №5", //Заголовок окна
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
	int s;

	//Цикл обработки сообщений
	switch (messg)
	{
	case WM_PAINT: // сообщение рисования

		for (int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY[i][j] = prism[i][j];
			}
		}

		proek(COPY);

		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		bresenhamline(hdc, COPY[0][0], COPY[0][1], COPY[1][0], COPY[1][1], 0, 0, 255);
		bresenhamline(hdc, COPY[1][0], COPY[1][1], COPY[2][0], COPY[2][1], 0, 0, 255);
		bresenhamline(hdc, COPY[2][0], COPY[2][1], COPY[3][0], COPY[3][1], 0, 0, 255);
		bresenhamline(hdc, COPY[3][0], COPY[3][1], COPY[0][0], COPY[0][1], 0, 0, 255);

		bresenhamline(hdc, COPY[4][0], COPY[4][1], COPY[5][0], COPY[5][1], 255, 0, 0);
		bresenhamline(hdc, COPY[5][0], COPY[5][1], COPY[6][0], COPY[6][1], 255, 0, 0);
		bresenhamline(hdc, COPY[6][0], COPY[6][1], COPY[7][0], COPY[7][1], 255, 0, 0);
		bresenhamline(hdc, COPY[7][0], COPY[7][1], COPY[4][0], COPY[4][1], 255, 0, 0);

		bresenhamline(hdc, COPY[1][0], COPY[1][1], COPY[5][0], COPY[5][1], 0, 255, 255);
		bresenhamline(hdc, COPY[4][0], COPY[4][1], COPY[0][0], COPY[0][1], 0, 255, 255);

		bresenhamline(hdc, COPY[2][0], COPY[2][1], COPY[6][0], COPY[6][1], 0, 255, 0);
		bresenhamline(hdc, COPY[7][0], COPY[7][1], COPY[3][0], COPY[3][1], 0, 255, 0);

		FIG[0].P1.x = COPY[0][0];
		FIG[0].P1.y = COPY[0][1];
		FIG[0].P1.z = prism[0][2];
		FIG[0].P2.x = COPY[1][0];
		FIG[0].P2.y = COPY[1][1];
		FIG[0].P2.z = prism[1][2];
		FIG[0].P3.x = COPY[2][0];
		FIG[0].P3.y = COPY[2][1];
		FIG[0].P3.z = prism[2][2];
		FIG[0].P4.x = COPY[3][0];
		FIG[0].P4.y = COPY[3][1];
		FIG[0].P4.z = prism[3][2];
		FIG[0].rt = 0;
		FIG[0].gt = 0;
		FIG[0].bt = 255;

		FIG[1].P1.x = COPY[4][0];
		FIG[1].P1.y = COPY[4][1];
		FIG[1].P1.z = prism[4][2];
		FIG[1].P2.x = COPY[5][0];
		FIG[1].P2.y = COPY[5][1];
		FIG[1].P2.z = prism[5][2];
		FIG[1].P3.x = COPY[6][0];
		FIG[1].P3.y = COPY[6][1];
		FIG[1].P3.z = prism[6][2];
		FIG[1].P4.x = COPY[7][0];
		FIG[1].P4.y = COPY[7][1];
		FIG[1].P4.z = prism[7][2];
		FIG[1].rt = 255;
		FIG[1].gt = 0;
		FIG[1].bt = 0;

		FIG[2].P1.x = COPY[0][0];
		FIG[2].P1.y = COPY[0][1];
		FIG[2].P1.z = prism[0][2];
		FIG[2].P2.x = COPY[1][0];
		FIG[2].P2.y = COPY[1][1];
		FIG[2].P2.z = prism[1][2];
		FIG[2].P3.x = COPY[5][0];
		FIG[2].P3.y = COPY[5][1];
		FIG[2].P3.z = prism[5][2];
		FIG[2].P4.x = COPY[4][0];
		FIG[2].P4.y = COPY[4][1];
		FIG[2].P4.z = prism[4][2];
		FIG[2].rt = 0;
		FIG[2].gt = 255;
		FIG[2].bt = 255;

		FIG[3].P1.x = COPY[2][0];
		FIG[3].P1.y = COPY[2][1];
		FIG[3].P1.z = prism[2][2];
		FIG[3].P2.x = COPY[3][0];
		FIG[3].P2.y = COPY[3][1];
		FIG[3].P2.z = prism[3][2];
		FIG[3].P3.x = COPY[7][0];
		FIG[3].P3.y = COPY[7][1];
		FIG[3].P3.z = prism[7][2];
		FIG[3].P4.x = COPY[6][0];
		FIG[3].P4.y = COPY[6][1];
		FIG[3].P4.z = prism[6][2];
		FIG[3].rt = 0;
		FIG[3].gt = 255;
		FIG[3].bt = 0;

		FIG[4].P1.x = COPY[1][0];
		FIG[4].P1.y = COPY[1][1];
		FIG[4].P1.z = prism[1][2];
		FIG[4].P2.x = COPY[2][0];
		FIG[4].P2.y = COPY[2][1];
		FIG[4].P2.z = prism[2][2];
		FIG[4].P3.x = COPY[6][0];
		FIG[4].P3.y = COPY[6][1];
		FIG[4].P3.z = prism[6][2];
		FIG[4].P4.x = COPY[5][0];
		FIG[4].P4.y = COPY[5][1];
		FIG[4].P4.z = prism[5][2];
		FIG[4].rt = 255;
		FIG[4].gt = 255;
		FIG[4].bt = 0;

		FIG[5].P1.x = COPY[4][0];
		FIG[5].P1.y = COPY[4][1];
		FIG[5].P1.z = prism[4][2];
		FIG[5].P2.x = COPY[0][0];
		FIG[5].P2.y = COPY[0][1];
		FIG[5].P2.z = prism[0][2];
		FIG[5].P3.x = COPY[3][0];
		FIG[5].P3.y = COPY[3][1];
		FIG[5].P3.z = prism[3][2];
		FIG[5].P4.x = COPY[7][0];
		FIG[5].P4.y = COPY[7][1];
		FIG[5].P4.z = prism[7][2];
		FIG[5].rt = 255;
		FIG[5].gt = 0;
		FIG[5].bt = 255;

		for (int q = 0; q < 6; q++)
		{

			FIG[q].mid_z = (FIG[q].P1.z + FIG[q].P2.z + FIG[q].P3.z + FIG[q].P4.z) / 4;
		}

		for (int q = 0; q < 5; q++)
		{
			for (int v = 0; v < 5 - q; v++)
			{
				if (FIG[v].mid_z > FIG[v + 1].mid_z)
				{
					square TMP = FIG[v];
					FIG[v] = FIG[v + 1];
					FIG[v + 1] = TMP;
				}
			}
		}

		for (int q = 0; q < 6; q++)
		{
			lilfig[0][0] = FIG[q].P1.x;
			lilfig[0][1] = FIG[q].P1.y;

			lilfig[1][0] = FIG[q].P2.x;
			lilfig[1][1] = FIG[q].P2.y;

			lilfig[2][0] = FIG[q].P3.x;
			lilfig[2][1] = FIG[q].P3.y;

			lilfig[3][0] = FIG[q].P4.x;
			lilfig[3][1] = FIG[q].P4.y;

			if (figur == 1)
			{
				PolyScan(hdc, lilfig, FIG[q].rt, FIG[q].gt, FIG[q].bt);
			}

			if (kontur == 1)
			{
				Line(hdc, lilfig[0][0], lilfig[0][1], lilfig[1][0], lilfig[1][1]);
				Line(hdc, lilfig[1][0], lilfig[1][1], lilfig[2][0], lilfig[2][1]);
				Line(hdc, lilfig[2][0], lilfig[2][1], lilfig[3][0], lilfig[3][1]);
				Line(hdc, lilfig[3][0], lilfig[3][1], lilfig[0][0], lilfig[0][1]);
			}
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
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_RIGHT: // Обрабатывает клавишу RIGHT ARROW (Стрелка вправо).	>
			move(prism, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_UP: // Обрабатывает клавишу UP ARROW (Стрелка вверх).	\/
			move(prism, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case VK_DOWN: // Обрабатывает клавишу DOWN ARROW (Стрелка вниз).	^
			move(prism, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x57: // Обрабатывает клавишу W.
			move(prism, 0, 0, 5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x53: // Обрабатывает клавишу S.
			move(prism, 0, 0, -5);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xba: // Обрабатывает клавишу ;.	уменьшение
			scale(prism, 0.9);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0xde: // Обрабатывает клавишу '.	увеличение
			scale(prism, 1.1);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x58: // Обрабатывает клавишу X.
			rotateX(prism, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x59: // Обрабатывает клавишу Y.
			rotateY(prism, angle);
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 0x5A: // Обрабатывает клавишу Z.
			rotateZ(prism, angle);
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

void mul_matrix(double fig[M][N], double mass[N][N])
{
	double res[M][N] = { 0,0,0,0 };
	for (int k = 0; k < M; k++)
	{
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				res[k][i] += fig[k][j] * mass[j][i];
			}
		}
	}
	for (int k = 0; k < M; k++)
	{
		for (int i = 0; i < N; i++)
		{
			fig[k][i] = res[k][i];
		}
	}
}

void proek(double fig[M][N])
{
	//double pro[N][N] = { {1, 0, 0, 0},
	//					 {0, 1, 0, 0},
	//					 {sqrt(2) / 4, sqrt(2) / 4, 0, 1},
	//					 {0, 0, 0, 0} }; // кабинетная проекция

	double pro[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 0, 0},
					     {0, 0, 0, 1} };
	mul_matrix(fig, pro);
}

double aver(double fig[M][N], int cnt)
{
	double average = 0;
	for (int i = 0; i < N; i++)
	{
		average += fig[i][cnt];
	}
	return average / N;
}

void move(double fig[M][N], int dx, int dy, int dz)
{
	double mover[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {0, 0, 1, 0},
						 {dx, dy, dz, 1} };
	mul_matrix(fig, mover);
}

void rotateX(double fig[M][N], double angl)
{
	double y = 0, z = 0;
	y = aver(fig, 1);
	z = aver(fig, 2);
	double Angle[N][N] = { {1, 0, 0, 0},
						   {0 , cos(angl), sin(angl), 0},
						   {0, -sin(angl), cos(angl), 0},
						   {0, y * (1 - cos(angl)) + z * sin(angl), z * (1 - cos(angl)) - y * sin(angl), 1} };
	mul_matrix(fig, Angle);
}

void rotateY(double fig[M][N], double angl)
{
	double x = 0, z = 0;
	x = aver(fig, 0);
	z = aver(fig, 2);
	double Angle[N][N] = { {cos(angl), 0, -sin(angl), 0},
						   {0, 1, 0, 0},
						   {sin(angl), 0, cos(angl), 0},
						   {x * (1 - cos(angl)) - z * sin(angl), 0, z * (1 - cos(angl)) + x * sin(angl), 1} };
	mul_matrix(fig, Angle);
}

void rotateZ(double fig[M][N], double angl)
{
	double x = 0, y = 0;
	x = aver(fig, 0);
	y = aver(fig, 1);
	double Angle[N][N] = { {cos(angl), sin(angl), 0, 0},
						   { -sin(angl), cos(angl), 0, 0},
						   {0, 0, 1, 0},
						   {x * (1 - cos(angl)) + y * sin(angl), y * (1 - cos(angl)) - x * sin(angl), 0, 1} };
	mul_matrix(fig, Angle);
}

void scale(double fig[M][N], double S)
{
	double x = 0, y = 0, z = 0;
	x = aver(fig, 0);
	y = aver(fig, 1);
	z = aver(fig, 2);
	double	Sx_Sy[N][N] = { {S,0,0,0},
			  {0,S,0,0},
			  {0,0,S,0},
			  {x * (1 - S),y * (1 - S),z * (1 - S),1} };
	mul_matrix(fig, Sx_Sy);
}