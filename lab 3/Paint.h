#include <Windows.h>

#define N 3 // размер столбцов матриц
#define M 6 // количество точек в фигуре

class Edge
{
public:
    int yUpper; // верхн€€ граница y

    double xIntersect; // x-пересечение
    double dxPerScan; // 

    Edge* next; 
};

class PointCoordinates
{
public:
    double x;
    double y;

    PointCoordinates()
    {
        x = 0;
        y = 0;
    }
};

class LineCoordinates
{
public:
    double x_1;
    double y_1;
    double x_2;
    double y_2;

    LineCoordinates()
    {
        x_1 = 0;
        y_1 = 0;
        x_2 = 0;
        y_2 = 0;
    }

    LineCoordinates(const double x1, const double y1, const double x2, const double y2)
    {
        x_1 = x1;
        y_1 = y1;
        x_2 = x2;
        y_2 = y2;
    }
};

void Fill_polygon(HDC, /*const int, */const double[M][N]); // 

void insertEdge(Edge*, Edge*); //
void makeEdgeRec(const PointCoordinates, const PointCoordinates, const int, Edge*, Edge* []); //
void buildEdgeList(/*const int, */const PointCoordinates[], Edge* []); //
void buildActiveList(const int, Edge*, Edge* []); //
void fillScan(HDC, const int, const Edge*); //
void deleteAfter(Edge[]);
void updateActiveList(const int, Edge*); //
void resortActiveList(Edge[]); //

const int yNext(const int, /*const int, */const PointCoordinates[]); //

void Polygon(HDC, /*const int, */const double[M][N]); //
void Lines(HDC, const int, const int, const int, const int);

void bresenhamline(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
//BOOL Line(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);

void Fill_polygon(HDC hdc/*, const int n*/, const double ppts[M][N])
{
    Edge* edges[520];
    Edge* active;

    PointCoordinates* pts = new PointCoordinates[M+1];

    int count_1;
    for (count_1 = 0; count_1 < M; count_1++)
    {
        pts[count_1].x = (ppts[count_1][0]);
        pts[count_1].y = (ppts[count_1][1]);
    }
    pts[count_1].x = (ppts[0][0]);
    pts[count_1].y = (ppts[0][1]);

    for (int count_2 = 0; count_2 < 520; count_2++)
    {
        edges[count_2] = new Edge;
        edges[count_2]->next = NULL;
    }

    buildEdgeList(/*n, */pts, edges);

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

    Polygon(hdc, /*n, */ppts);

    delete[] pts;
}

const int yNext(const int k, /*const int cnt,*/ const PointCoordinates pts[])
{
    int j;

    if ((k + 1) > M)
    {
        j = 0;
    }
    else
    {
        j = (k + 1);
    }

    while (pts[k].y == pts[j].y)
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

    return (pts[j].y);
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

void makeEdgeRec(const PointCoordinates lower, const PointCoordinates upper, const int yComp, Edge* edge, Edge* edges[])
{
    edge->dxPerScan = ((upper.x - lower.x) / (upper.y - lower.y));
    edge->xIntersect = lower.x;

    if (upper.y < yComp)
    {
        edge->yUpper = (upper.y - 1);
    }
    else
    {
        edge->yUpper = upper.y;
    }

    insertEdge(edges[(int)lower.y], edge);
}

void buildEdgeList(/*const int cnt,*/ const PointCoordinates pts[], Edge* edges[])
{
    Edge* edge;
    PointCoordinates v1;
    PointCoordinates v2;

    int yPrev = (pts[M-1].y);

    v1.x = pts[M].x;
    v1.y = pts[M].y;

    for (int count = 0; count < M+1; count++)
    {
        v2 = pts[count];

        if (v1.y != v2.y)
        {
            edge = new Edge;

            if (v1.y < v2.y)
            {
                makeEdgeRec(v1, v2, yNext(count/*, M+1*/, pts), edge, edges);
            }
            else
            {
                makeEdgeRec(v2, v1, yPrev, edge, edges);
            }
        }
        yPrev = v1.y;
        v1 = v2;
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

        /*for (int count = p1->xIntersect; count <= p2->xIntersect; count++)
        {
            SetPixel(hdc, count, scan, RGB(0, 0, 255));
        }*/

        bresenhamline(hdc, p1->xIntersect, scan, p2->xIntersect, scan, 0,0,255); //////////////////////////////////////

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

void Polygon(HDC hdc, /*const int n, */const double coordinates[M][N])
{
    if ((M+1) >= 2)
    {
        Lines(hdc, coordinates[0][0], coordinates[0][1], coordinates[1][0], coordinates[1][1]);

        for (int count = 1; count < (M - 1); count++)//?
        {
            Lines(hdc, coordinates[count][0], coordinates[count][1], coordinates[(count+1)][0], coordinates[(count+1)][1]);
        }
    }
}

void Lines(HDC hdc, const int x_1, const int y_1, const int x_2, const int y_2)
{
    int x1 = x_1;
    int y1 = y_1;

    int x2 = x_2;
    int y2 = y_2;

    if (x_1 > x_2)
    {
        x1 = x_2;
        y1 = y_2;

        x2 = x_1;
        y2 = y_1;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int inc_dec = ((y2 >= y1) ? 1 : -1);

    if (dx > dy)
    {
        int two_dy = (2 * dy);
        int two_dy_dx = (2 * (dy - dx));
        int p = ((2 * dy) - dx);

        int x = x1;
        int y = y1;

        SetPixel(hdc, x, y, RGB(0, 0, 255));

        while (x < x2)
        {
            x++;
            if (p < 0)
            {
                p += two_dy;
            }
            else
            {
                y += inc_dec;
                p += two_dy_dx;
            }
            SetPixel(hdc, x, y, RGB(0, 0, 255));
        }
    }
    else
    {
        int two_dx = (2 * dx);
        int two_dx_dy = (2 * (dx - dy));
        int p = ((2 * dx) - dy);

        int x = x1;
        int y = y1;

        SetPixel(hdc, x, y, RGB(0, 0, 255));

        while (y != y2)
        {
            y += inc_dec;

            if (p < 0)
            {
                p += two_dx;
            }
            else
            {
                x++;
                p += two_dx_dy;
            }

            SetPixel(hdc, x, y, RGB(0, 0, 255));
        }
    }
}

//BOOL Line(HDC hdc, int x1, int y1, int x2, int y2, int r, int g, int b) // обычна€ лини€
//{
//	HPEN hPen; //ќбъ€вл€етс€ кисть
//	hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b)); //—оздаЄтс€ объект
//	SelectObject(hdc, hPen); //ќбъект делаетс€ текущим
//
//	MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
//	return LineTo(hdc, x2, y2);
//}

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