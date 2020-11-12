#include <Windows.h>

class Edge
{
public:
    int yUpper;

    float xIntersect;
    float dxPerScan;

    Edge* next;
};

class PointCoordinates
{
public:
    float x;
    float y;

    PointCoordinates()
    {
        x = 0;
        y = 0;
    }
};

class LineCoordinates
{
public:
    float x_1;
    float y_1;
    float x_2;
    float y_2;

    LineCoordinates()
    {
        x_1 = 0;
        y_1 = 0;
        x_2 = 0;
        y_2 = 0;
    }

    LineCoordinates(const float x1, const float y1, const float x2, const float y2)
    {
        x_1 = x1;
        y_1 = y1;
        x_2 = x2;
        y_2 = y2;
    }
};

//void show_screen();

void Fill_polygon(HDC, const int, const int[], const int);

void insertEdge(Edge*, Edge*);
void makeEdgeRec(const PointCoordinates, const PointCoordinates, const int, Edge*, Edge* []);
void buildEdgeList(const int, const PointCoordinates[], Edge* []);
void buildActiveList(const int, Edge*, Edge* []);
void fillScan(HDC, const int, const Edge*, const int);
void deleteAfter(Edge[]);
void updateActiveList(const int, Edge*);
void resortActiveList(Edge[]);

const int yNext(const int, const int, const PointCoordinates[]);

void Polygon(HDC, const int, const int[]);
void Lines(HDC, const int, const int, const int, const int);




void Fill_polygon(HDC hdc, const int n, const int ppts[], const int fill_color)
{
    Edge* edges[520];
    Edge* active;

    PointCoordinates* pts = new PointCoordinates[n];

    for (int count_1 = 0; count_1 < n; count_1++)
    {
        pts[count_1].x = (ppts[(count_1 * 2)]);
        pts[count_1].y = (ppts[((count_1 * 2) + 1)]);
    }

    for (int count_2 = 0; count_2 < 520; count_2++)
    {
        edges[count_2] = new Edge;
        edges[count_2]->next = NULL;
    }

    buildEdgeList(7, pts, edges);

    active = new Edge;
    active->next = NULL;

    for (int count_3 = 0; count_3 < 520; count_3++)
    {
        buildActiveList(count_3, active, edges);

        if (active->next)
        {
            fillScan(hdc, count_3, active, fill_color);
            updateActiveList(count_3, active);
            resortActiveList(active);
        }
    }

    Polygon(hdc, n, ppts);

    delete[] pts;
}

const int yNext(const int k, const int cnt, const PointCoordinates pts[])
{
    int j;

    if ((k + 1) > (cnt - 1))
        j = 0;

    else
        j = (k + 1);

    while (pts[k].y == pts[j].y)
    {
        if ((j + 1) > (cnt - 1))
            j = 0;

        else
            j++;
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
            p = NULL;

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
        edge->yUpper = (upper.y - 1);

    else
        edge->yUpper = upper.y;

    insertEdge(edges[(int)lower.y], edge);
}

void buildEdgeList(const int cnt, const PointCoordinates pts[], Edge* edges[])
{
    Edge* edge;
    PointCoordinates v1;
    PointCoordinates v2;

    int yPrev = (pts[cnt - 2].y);

    v1.x = pts[cnt - 1].x;
    v1.y = pts[cnt - 1].y;

    for (int count = 0; count < cnt; count++)
    {
        v2 = pts[count];

        if (v1.y != v2.y)
        {
            edge = new Edge;

            if (v1.y < v2.y)
                makeEdgeRec(v1, v2, yNext(count, cnt, pts), edge, edges);

            else
                makeEdgeRec(v2, v1, yPrev, edge, edges);
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

void fillScan(HDC hdc, const int scan, const Edge* active, const int fill_color)
{
    Edge* p1;
    Edge* p2;

    p1 = active->next;

    while (p1)
    {
        p2 = p1->next;

        for (int count = p1->xIntersect; count <= p2->xIntersect; count++)
        {
            SetPixel(hdc, count, scan, RGB(0, 0, 255));
        }

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

void Polygon(HDC hdc, const int n, const int coordinates[])
{
    if (n >= 2)
    {
        Lines(hdc, coordinates[0], coordinates[1], coordinates[2], coordinates[3]);

        for (int count = 1; count < (n - 1); count++)
            Lines(hdc, coordinates[(count * 2)], coordinates[((count * 2) + 1)], coordinates[((count + 1) * 2)], coordinates[(((count + 1) * 2) + 1)]);
    }
}

void Lines(HDC hdc, const int x_1, const int y_1, const int x_2, const int y_2)
{
    int color = 5/*getcolor()*/;

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
                p += two_dy;

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
                p += two_dx;

            else
            {
                x++;
                p += two_dx_dy;
            }

            SetPixel(hdc, x, y, RGB(0, 0, 255));
        }
    }
}