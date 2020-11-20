#include <Windows.h>

#define N 3 // ������ �������� ������
#define M 6 // ���������� ����� � ������

void bresenhamline(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);
void Line(HDC, int, int, int, int, int r = 0, int g = 0, int b = 0);

// ******definition of the structure and an active edge list AET new edge table NET * **********************************/
typedef struct EDGE // ��������� �����
{
    double xIntersect;
    double dx, ymax;
    EDGE* next;
} AET, NET; // �������� ����� / ����� ������� �����

struct point // ��������� �����
{
    double x;
    double y;
};

void PolyScan(HDC hdc, double hexagon[M][N])
{
    point polypoint[M]; // ������ �����

    for (int count_1 = 0; count_1 < M; count_1++) // ��������� ���
    {
        polypoint[count_1].x = round(hexagon[count_1][0]);
        polypoint[count_1].y = round(hexagon[count_1][1]);
    }

    int MaxY = 0; // ����� ������� y-����������
    int i;
    for (i = 0; i < M; i++)
    {
        if (polypoint[i].y > MaxY)
        {
            MaxY = polypoint[i].y;
        }
    }

    AET* pAET = new AET; // ������� �������� �����
    pAET->next = NULL;

    NET* pNET[1024]; // ����� ������� �����
    for (i = 0; i <= MaxY; i++)
    {
        pNET[i] = new NET;
        pNET[i]->next = NULL;
    }

    for (i = 0; i <= MaxY; i++) // ��������� � ��������� ����� ������� �����
    {
        for (int j = 0; j < M; j++)
        {
            if (polypoint[j].y == i)
            {
                // forming a segment with a point in front of a point, the point is also formed with the back segment  
                if (polypoint[(j - 1 + M) % M].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j - 1 + M) % M].y;
                    p->dx = (polypoint[(j - 1 + M) % M].x - polypoint[j].x) / (polypoint[(j - 1 + M) % M].y - polypoint[j].y);
                    p->next = pNET[i]->next;
                    pNET[i]->next = p;
                }
                if (polypoint[(j + 1 + M) % M].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j + 1 + M) % M].y;
                    p->dx = (polypoint[(j + 1 + M) % M].x - polypoint[j].x) / (polypoint[(j + 1 + M) % M].y - polypoint[j].y);
                    p->next = pNET[i]->next;
                    pNET[i]->next = p;
                }
            }
        }
    }

    for (i = 0; i <= MaxY; i++) // ������������� � ��������� �������� ������� �����
    {
        // Calculate the new intersection x, update AET  
        NET* p = pAET->next; // ����������� ����� X-�����������, ��������� �������� �����
        while (p)
        {
            p->xIntersect = p->xIntersect + p->dx;
            p = p->next;
        }
        // update after the new AET first sort ****************************************** ******************* /  
        // off the table sorting, no longer open space  
        AET* tq = pAET;
        p = pAET->next;
        tq->next = NULL;
        while (p)
        {
            while (tq->next && p->xIntersect >= tq->next->xIntersect)
                tq = tq->next;
            NET* s = p->next;
            p->next = tq->next;
            tq->next = p;
            p = s;
            tq = pAET;
        }
        // (Improved Algorithm) deleting the node ymax == i Table AET start ******************************* ********* /  
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
        // new point NET added AET, and by interpolation value X is incremented by sorting ******************************* *** /  
        p = pNET[i]->next;
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
        while (p && p->next) // ����������� ������
        {
            Line(hdc, p->xIntersect, i, p->next->xIntersect+1, i, 0, 0, 255);
            p = p->next->next;// consider the situation endpoint  
        }
    }
}

void Line(HDC hdc, int x1, int y1, int x2, int y2, int r, int g, int b) // ������� �����
{
    HPEN hPen; //����������� �����
    hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b)); //�������� ������
    SelectObject(hdc, hPen); //������ �������� �������

    MoveToEx(hdc, x1, y1, NULL); //������� �������� ���������� x1, y1
    LineTo(hdc, x2, y2);
    DeleteObject(hPen);
}

void bresenhamline(HDC hdc, int x0, int y0, int x1, int y1, int r, int g, int b) // ���������
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