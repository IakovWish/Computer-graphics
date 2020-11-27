#include <iostream>
#include <Windows.h>
#include <math.h>

using namespace std;

#define N 4 // размер столбцов матриц
#define M 8 // количество точек в фигуре
#define angle 0.0174444444444444

void mul_matrix(double fig[M][N], double mass[N][N]);
void scale(double fig[M][N], double);
void rotateX(double fig[M][N], double);
void rotateY(double fig[M][N], double);
void rotateZ(double fig[M][N], double);
void Line(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
void bresenhamline(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
void move(double fig[M][N], int = 0, int = 0, int = 0);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Создаём прототип функции окна, которая будет определена ниже

char szProgName[] = "Компьютерная графика ЛР №4"; // объявляем строку-имя программы

double prism[M][N] = { {100, 400, 100, 1},
						 {100, 200, 100, 1},
						 {400, 200, 100, 1},
						 {400, 400, 100, 1},
						 {100, 400, 300, 1},
						 {100, 200, 300, 1},
						 {400, 200, 300, 1},
						 {400, 400, 300, 1} };

double pro[N][N] = { {1, 0, 0, 0},
						 {0, 1, 0, 0},
						 {sqrt(2) / 4, sqrt(2) / 4, 0, 1},
						 {0, 0, 0, 0} }; // кабинетная проекция

double COPY[M][N];

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
		"Лабораторная работа №4", //Заголовок окна
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

		for(int i = 0; i < M; i++)
		{
			for (int j = 0; j < N; j++)
			{
				COPY[i][j] = prism[i][j];
			}
		}
		
		mul_matrix(COPY, pro);

		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		bresenhamline(hdc, COPY[0][0], COPY[0][1], COPY[1][0], COPY[1][1], 0, 0, 255);
		bresenhamline(hdc, COPY[1][0], COPY[1][1], COPY[2][0], COPY[2][1], 0, 0, 255);
		bresenhamline(hdc, COPY[2][0], COPY[2][1], COPY[3][0], COPY[3][1], 0, 0, 255);
		bresenhamline(hdc, COPY[3][0], COPY[3][1], COPY[0][0], COPY[0][1], 0, 0, 255);
		
		bresenhamline(hdc, COPY[4][0], COPY[4][1], COPY[5][0], COPY[5][1], 255, 0, 0);
		bresenhamline(hdc, COPY[5][0], COPY[5][1], COPY[6][0], COPY[6][1], 255, 0, 0);
		bresenhamline(hdc, COPY[6][0], COPY[6][1], COPY[7][0], COPY[7][1], 255, 0, 0);
		bresenhamline(hdc, COPY[7][0], COPY[7][1], COPY[4][0], COPY[4][1], 255, 0, 0);
		
		bresenhamline(hdc, COPY[0][0], COPY[0][1], COPY[4][0], COPY[4][1], 0, 255, 255);
		bresenhamline(hdc, COPY[1][0], COPY[1][1], COPY[5][0], COPY[5][1], 0, 255, 255);

		bresenhamline(hdc, COPY[2][0], COPY[2][1], COPY[6][0], COPY[6][1], 0, 255, 0);
		bresenhamline(hdc, COPY[3][0], COPY[3][1], COPY[7][0], COPY[7][1], 0, 255, 0);

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
			break;

		case VK_RIGHT: // Обрабатывает клавишу RIGHT ARROW (Стрелка вправо).	>
			move(prism, 5);
			break;

		case VK_UP: // Обрабатывает клавишу UP ARROW (Стрелка вверх).	\/
			move(prism, 0, -5);
			break;

		case VK_DOWN: // Обрабатывает клавишу DOWN ARROW (Стрелка вниз).	^
			move(prism, 0, 5);
			break;

		case 0x57: // Обрабатывает клавишу W.
			move(prism, 0, 0, 5);
			break;

		case 0x53: // Обрабатывает клавишу S.
			move(prism, 0, 0, -5);
			break;

		case 0xba: // Обрабатывает клавишу ;.	уменьшение
			scale(prism, 0.9);
			break;

		case 0xde: // Обрабатывает клавишу '.	увеличение
			scale(prism, 1.1);
			break;

		case 0x58: // Обрабатывает клавишу X.
			rotateX(prism, angle);
			break;

		case 0x59: // Обрабатывает клавишу Y.
			rotateY(prism, angle);
			break;

		case 0x5A: // Обрабатывает клавишу Z.
			rotateZ(prism, angle);
			break;

		default:
			break;
		}

		/*закругляемся*/
		InvalidateRect(hWnd, NULL, TRUE);
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

void bresenhamline(HDC hdc, int x0, int y0, int x1, int y1, int r, int g, int b) // брезенхем
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2 = err;

	for (;;)
	{
		SetPixel(hdc, x0, y0, RGB(r, g, b));

		if (x0 == x1 && y0 == y1)
		{
			break;
		}
		e2 = err;
		if (e2 > -dx)
		{
			err -= dy; x0 += sx;
		}
		if (e2 < dy)
		{
			err += dx; y0 += sy;
		}
	}
}

void Line(HDC hdc, int x1, int y1, int x2, int y2, int r, int g, int b) // обычная линия
{
	HPEN hPen; //Объявляется кисть
	hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b)); //Создаётся объект
	SelectObject(hdc, hPen); //Объект делается текущим

	MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
	LineTo(hdc, x2, y2);
	DeleteObject(hPen);
}