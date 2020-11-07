﻿#include <iostream>
#include <Windows.h>

using namespace std;

#define N 3 // размер столбцов матриц
#define M 2 // количество точек в фигуре
#define angle 0.0174444444444444

void mul_matrix(double fig[M][N], double mass[N][N])
{
	double res[M][N] = { 0,0,0 };
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
	for (int k = 0; k < 2; k++)
	{
		for (int i = 0; i < 3; i++)
		{
			fig[k][i] = res[k][i];
		}
	}
}

void rotate(double fig[M][N], double angl)
{
	double x1 = 0, y1 = 0;
	for (int i = 0; i < M; i++)
	{
		x1 += fig[i][0];
	}
	x1 = x1 / M;
	for (int i = 0; i < M; i++)
	{
		y1 += fig[i][1];
	}
	y1 = y1 / M;

	double Angle[N][N] = { {cos(angl), sin(angl), 0},{ -sin(angl), cos(angl), 0},{x1 * (1 - cos(angl)) + y1 * sin(angl), y1 * (1 - cos(angl)) - x1 * sin(angl), 1} };
	mul_matrix(fig, Angle);
}

void scale(double fig[M][N], double S)
{
	double x1 = 0.0, y1 = 0.0;
	for (int i = 0; i < M; i++)
	{
		x1 += fig[i][0];
	}
	x1 = x1 / M;

	for (int i = 0; i < M; i++)
	{
		y1 += fig[i][1];
	}
	y1 = y1 / M;
	double Sx_Sy[N][N] = { {S,0,0}, {0,S,0} , {x1 * (1 - S),y1 * (1 - S),1} };
	mul_matrix(fig, Sx_Sy);
}


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Создаём прототип функции окна, которая будет определена ниже

char szProgName[] = "Компьютерная графика ЛР №1"; // объявляем строку-имя программы

double mD_up[N][N] = { {1, 0, 0},
					   {0, 1, 0},
					   {0, -5, 1} };//матрица для перемещения наверх

double mD_down[N][N] = { {1, 0, 0},
						 {0, 1, 0},
						 {0, 5,1} };//матрица перемещения вниз

double mD_left[N][N] = { {1, 0, 0},
						 {0, 1, 0},
						 {-5,0, 1} };//матрица для перемещения налево

double mD_right[N][N] = { {1, 0, 0},
						  {0, 1, 0},
						  {5, 0, 1} };//матрица перемещения направо


double library_line[M][N] = { {400, 500, 1},
							  {300, 600, 1} };

double bresenham_line[M][N] = { {500, 600, 1},
								{200, 700, 1} };

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
	HPEN hPen; //Объявляется кисть
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); //Создаётся объект
	SelectObject(hdc, hPen); //Объект делается текущим

	MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
	return LineTo(hdc, x2, y2);
}

void bresenhamline(HDC hdc, int x0, int y0, int x1, int y1)
{
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = (dx > dy ? dx : -dy) / 2;
	int e2 = err;

	for (;;)
	{
		SetPixel(hdc, x0, y0, RGB(0, 0, 255));
		if (x0 == x1 && y0 == y1) break;
		e2 = err;
		if (e2 > -dx) { err -= dy; x0 += sx; }
		if (e2 < dy) { err += dx; y0 += sy; }
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	HWND hWnd;
	MSG lpMsg;
	WNDCLASS w; //создаём экземпляр структуры WNDCLASS

	//И начинаем её заполнять
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
		"Лабораторная работа №1", //Заголовок окна
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
		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		Line(hdc, library_line[0][0], library_line[0][1], library_line[1][0], library_line[1][1]); // библиотечная линия
		bresenhamline(hdc, bresenham_line[0][0], bresenham_line[0][1], bresenham_line[1][0], bresenham_line[1][1]); // брезенхем

		//закругляемся
		ValidateRect(hWnd, NULL); // обновляем окно
		EndPaint(hWnd, &ps); // заканчиваем рисовать
		break;

	case WM_KEYDOWN: // Обработка нажатия клавиши
		hdc = BeginPaint(hWnd, &ps); // начинаем рисовать

		switch (wParam)
		{
		case VK_LEFT: // Обрабатывает клавишу LEFT ARROW (Стрелка влево).	<
			mul_matrix(library_line, mD_left);
			break;

		case VK_RIGHT: // Обрабатывает клавишу RIGHT ARROW (Стрелка вправо).	>
			mul_matrix(library_line, mD_right);
			break;

		case VK_UP: // Обрабатывает клавишу UP ARROW (Стрелка вверх).	\/
			mul_matrix(library_line, mD_up);
			break;

		case VK_DOWN: // Обрабатывает клавишу DOWN ARROW (Стрелка вниз).	^
			mul_matrix(library_line, mD_down);
			break;

		case 0xba: // Обрабатывает клавишу ;.	уменьшение
			scale(library_line, 0.9);
			break;

		case 0xde: // Обрабатывает клавишу '.	увеличение
			scale(library_line, 1.1);
			break;

		case 0xbe: // Обрабатывает клавишу ..	поворот влево
			rotate(library_line, -angle);
			break;

		case 0xbf: // Обрабатывает клавишу /.	поворот вправо
			rotate(library_line, angle);
			break;

		case 0x41: // Обрабатывает клавишу A.	<
			mul_matrix(bresenham_line, mD_left);
			break;

		case 0x44: // Обрабатывает клавишу D.	>
			mul_matrix(bresenham_line, mD_right);
			break;

		case 0x57: // Обрабатывает клавишу W.	^
			mul_matrix(bresenham_line, mD_up);
			break;

		case 0x53: // Обрабатывает клавишу S.	\/
			mul_matrix(bresenham_line, mD_down);
			break;

		case 0x51: // Обрабатывает клавишу Q. 
			scale(bresenham_line, 0.9);
			break;

		case 0x45: // Обрабатывает клавишу E. 
			scale(bresenham_line, 1.1);
			break;

		case 0x5a: // Обрабатывает клавишу Z. 
			rotate(bresenham_line, -angle);
			break;

		case 0x58: // Обрабатывает клавишу X. 
			rotate(bresenham_line, angle);
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