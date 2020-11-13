#include <Windows.h>

#define N 3 // размер столбцов матриц
#define M 6 // количество точек в фигуре

#define X 0
#define Y 1

class Edge
{
public:
    int yUpper; // верхн€€ граница y

    double xIntersect; // x-пересечение
    double dxPerScan; // 

    Edge* next;
};

void Fill_polygon(HDC, const double[M][N]); // 

void insertEdge(Edge*, Edge*); //
void makeEdgeRec(const double, const double, const double, const double, const int, Edge*, Edge* []); //
void buildEdgeList(const double[M][N], Edge* []); //
void buildActiveList(const int, Edge*, Edge* []); //
void fillScan(HDC, const int, const Edge*); //
void deleteAfter(Edge[]);
void updateActiveList(const int, Edge*); //
void resortActiveList(Edge[]); //

const int yNext(const int, const double[M][N]); //

void bresenhamline(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
void Line(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);

void Fill_polygon(HDC hdc, const double ppts[M][N])
{
    Edge* edges[520];
    Edge* active;

    double pppts[M+1][N];
    int count_1;
    for (count_1 = 0; count_1 < M; count_1++)
    {
        pppts[count_1][X] = (ppts[count_1][X]);
        pppts[count_1][Y] = (ppts[count_1][Y]);
    }
    pppts[count_1][X] = ppts[0][X];
    pppts[count_1][Y] = ppts[0][Y];

    for (int count_2 = 0; count_2 < 520; count_2++)
    {
        edges[count_2] = new Edge;
        edges[count_2]->next = NULL;
    }

    buildEdgeList(pppts, edges);

    active = new Edge;
    active->next = NULL;

    for (int count_3 = 0; count_3 < 520; count_3++)
    {
        buildActiveList(count_3, active, edges);

        if (active->next)
        {
            fillScan(hdc, count_3, active);
            updateActiveList(count_3, active);
            resortActiveList(active);
        }
    }
}

const int yNext(const int k, const double ppts[M][N])
{
    int j;

    if ((k + 1) > M)
    {
        j = 0;
    }
    else
    {
        j = k + 1;
    }

    while (ppts[k][Y] == ppts[j][Y])
    {
        if ((j + 1) > M)
        {
            j = 0;
        }
        else
        {
            j++;
        }
    }
    return (ppts[j][Y]);
}

void insertEdge(Edge* list, Edge* edge)
{
    Edge* p;
    Edge* q = list;

    p = q->next;

    while (p != NULL)
    {
        if (edge->xIntersect < p->xIntersect)
        {
            p = NULL;
        }
        else
        {
            q = p;
            p = p->next;
        }
    }

    edge->next = q->next;
    q->next = edge;
}

void makeEdgeRec(const double lowerx, const double lowery, const double upperx, const double uppery, const int yComp, Edge* edge, Edge* edges[])
{
    edge->dxPerScan = ((upperx - lowerx) / (uppery - lowery));
    edge->xIntersect = lowerx;

    if (uppery < yComp)
    {
        edge->yUpper = (uppery - 1);
    }
    else
    {
        edge->yUpper = uppery;
    }

    insertEdge(edges[(int)lowery], edge);
}

void buildEdgeList(const double ppts[M][N], Edge* edges[])
{
    Edge* edge;

    double v1x;
    double v1y;

    double v2x;
    double v2y;

    int yPrev = ppts[M-1][Y];

    v1x = ppts[M][X];
    v1y = ppts[M][Y];

    for (int count = 0; count < M+1; count++)
    {
        v2x = ppts[count][X];
        v2y = ppts[count][Y];

        if (v1y != v2y)
        {
            edge = new Edge;

            if (v1y < v2y)
            {
                makeEdgeRec(v1x, v1y, v2x, v2y, yNext(count, ppts), edge, edges);
            }
            else
            {
                makeEdgeRec(v2x, v2y, v1x, v1y, yPrev, edge, edges);
            }
        }
        yPrev = v1y;
        v1x = v2x;
        v1y = v2y;
    }
}

void buildActiveList(const int scan, Edge* active, Edge* edges[])
{
    Edge* p;
    Edge* q;

    p = edges[scan]->next;

    while (p)
    {
        q = p->next;

        insertEdge(active, p);

        p = q;
    }
}

void fillScan(HDC hdc, const int scan, const Edge* active)
{
    Edge* p1;
    Edge* p2;

    p1 = active->next;

    while (p1)
    {
        p2 = p1->next;

        Line(hdc, p1->xIntersect, scan, p2->xIntersect, scan, 0, 0, 255);

        p1 = p2->next;
    }
}

void deleteAfter(Edge* q)
{
    Edge* p = q->next;
    q->next = p->next;
    delete p;
}

void updateActiveList(const int scan, Edge* active)
{
    Edge* q = active;
    Edge* p = active->next;

    while (p)
    {
        if (scan >= p->yUpper)
        {
            p = p->next;
            deleteAfter(q);
        }
        else
        {
            p->xIntersect = (p->xIntersect + p->dxPerScan);
            q = p;
            p = p->next;
        }
    }
}

void resortActiveList(Edge* active)
{
    Edge* q;
    Edge* p = active->next;

    active->next = NULL;

    while (p)
    {
        q = p->next;
        insertEdge(active, p);
        p = q;
    }
}

void Line(HDC hdc, int x1, int y1, int x2, int y2, int r, int g, int b) // обычна€ лини€
{
	HPEN hPen; //ќбъ€вл€етс€ кисть
	hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b)); //—оздаЄтс€ объект
	SelectObject(hdc, hPen); //ќбъект делаетс€ текущим

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