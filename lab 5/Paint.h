#include <Windows.h>

#define N 3 // размер столбцов матриц
#define M 6 // количество точек в фигуре

void bresenhamline(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
void Line(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);

typedef struct EDGE // структура ребра
{
    double xIntersect;
    double dx, ymax;
    EDGE* next;
} AET, NET; // активные ребра / новая таблица ребер

struct point // структура точки
{
    double x;
    double y;
};

void PolyScan(HDC hdc, double hexagon[M][N])
{
    point polypoint[M]; // массив точек

    for (int count_1 = 0; count_1 < M; count_1++) // заполняем егр
    {
        polypoint[count_1].x = round(hexagon[count_1][0]);
        polypoint[count_1].y = round(hexagon[count_1][1]);
    }

    int MaxY = 0; // самая верхняя y-координата
    int i;
    for (i = 0; i < M; i++)
    {
        if (polypoint[i].y > MaxY)
        {
            MaxY = polypoint[i].y;
        }
    }

    int MinY = MaxY; // самая нижняя y-координата
    int k;
    for (k = 0; k < M; k++)
    {
        if (polypoint[k].y < MinY)
        {
            MinY = polypoint[k].y;
        }
    }

    AET* pAET = new AET; // таблица активных ребер
    pAET->next = NULL;

    NET** pNET = new NET * [MaxY - MinY]; // новая таблица ребер
    for (i = MinY; i <= MaxY; i++)
    {
        pNET[i - MinY] = new NET;
        pNET[i - MinY]->next = NULL;
    }

    for (i = MinY; i <= MaxY; i++) // сканируем и заполняем новую таблицу ребер
    {
        for (int j = 0; j < M; j++)
        {
            if (polypoint[j].y == i)
            {
                if (polypoint[(j - 1 + M) % M].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j - 1 + M) % M].y;
                    p->dx = (polypoint[(j - 1 + M) % M].x - polypoint[j].x) / (polypoint[(j - 1 + M) % M].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
                if (polypoint[(j + 1 + M) % M].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j + 1 + M) % M].y;
                    p->dx = (polypoint[(j + 1 + M) % M].x - polypoint[j].x) / (polypoint[(j + 1 + M) % M].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
            }
        }
    }

    for (i = MinY; i <= MaxY; i++) // устанавливаем и обновляем активную таблицу ребер
    {
        NET* p = pAET->next; // высчитываем новое X-пересечение, обновляем активные ребра
        while (p)
        {
            p->xIntersect = p->xIntersect + p->dx;
            p = p->next;
        }
        // обновить после первой сортировки новых активных ребер
        AET* tq = pAET;
        p = pAET->next;
        tq->next = NULL;
        while (p)
        {
            while (tq->next && p->xIntersect >= tq->next->xIntersect)
            {
                tq = tq->next;
            }
            NET* s = p->next;
            p->next = tq->next;
            tq->next = p;
            p = s;
            tq = pAET;
        }

        AET* q = pAET;
        p = q->next;
        while (p)
        {
            if (p->ymax == i)
            {
                q->next = p->next;
                delete p;
                p = q->next;
            }
            else
            {
                q = q->next;
                p = q->next;
            }
        }

        /* в новую точку NET добавляется активное ребро, значение X увеличивается путем сортировки*/
        p = pNET[i - MinY]->next;
        q = pAET;
        while (p)
        {
            while (q->next && p->xIntersect >= q->next->xIntersect)
            {
                q = q->next;
            }
            NET* s = p->next;
            p->next = q->next;
            q->next = p;
            p = s;
            q = pAET;
        }

        p = pAET->next;
        while (p && p->next) // закрашиваем фигуру
        {
            if (i >= 0)
            {
                Line(hdc, p->xIntersect, i, p->next->xIntersect + 1, i, 0, 0, 255);
            }
            p = p->next->next; // переходим к следующей точке
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