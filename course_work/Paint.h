#include <Windows.h>

#define Nn 2 // ������ �������� ������
#define Mm 4 // ���������� ����� � ������
#define Mm_pyr 3 // ���������� ����� � ������

// Color constants.
const COLORREF rgbRed = 0x000000FF; // 1
const COLORREF rgbGreen = 0x0000FF00; // 2
const COLORREF rgbBlue = 0x00FF0000; // 3
const COLORREF rgbCyan = 0xFFFF00; // 4
const COLORREF rgbYellow = 0x00FFFF; // 5
const COLORREF rgbMagenta = 0xFF00FF; // 6
const COLORREF rgbOlive = 0x808000; // 7

const COLORREF rgbBlack = 0x00000000;
const COLORREF rgbWhite = 0x00FFFFFF;
const COLORREF rgbBorder = 0x323232;

void Line(HDC, int, int, int, int, COLORREF = rgbBlack);

struct point // ��������� �����
{
    double x = 0;
    double y = 0;
    double z = 0;
};

typedef struct EDGE // ��������� �����
{
    double xIntersect;
    double dx, ymax;
    EDGE* next;
} AET, NET; // �������� ����� / ����� ������� �����

void Fill(HDC hdc, int x0, int y0, COLORREF clr)
{
    point zatr; // ����������� ������
    zatr.x = x0;
    zatr.y = y0;

    COLORREF color = GetPixel(hdc, (int)zatr.x, (int)zatr.y);

    if ((color != rgbBorder) && (color != clr))
    {
        SetPixel(hdc, (int)zatr.x, (int)zatr.y, clr);

        //Fill(hdc, zatr.x - 1, zatr.y - 1, clr);
        //Fill(hdc, zatr.x + 1, zatr.y + 1, clr);
        //Fill(hdc, zatr.x - 1, zatr.y + 1, clr);
        //Fill(hdc, zatr.x + 1, zatr.y - 1, clr);

        Fill(hdc, (int)zatr.x - 1, (int)zatr.y, clr);
        Fill(hdc, (int)zatr.x + 1, (int)zatr.y, clr);
        Fill(hdc, (int)zatr.x, (int)zatr.y - 1, clr);
        Fill(hdc, (int)zatr.x, (int)zatr.y + 1, clr);
    }
}

void Zatravrka(HDC hdc, double hexagon[Mm][Nn], COLORREF clr = rgbBlack)
{
    point zatr{ 0,0 };

    zatr.x = (hexagon[0][0] + hexagon[1][0] + hexagon[2][0] + hexagon[3][0]) / 4;
    zatr.y = (hexagon[0][1] + hexagon[1][1] + hexagon[2][1] + hexagon[3][1]) / 4;

    Fill(hdc, (int)zatr.x, (int)zatr.y, clr);
}

void Zatravrka_triangle(HDC hdc, double hexagon[Mm][Nn], COLORREF clr = rgbBlack)
{
    point zatr{ 0,0 };

    zatr.x = (hexagon[0][0] + hexagon[1][0] + hexagon[2][0]) / 3;
    zatr.y = (hexagon[0][1] + hexagon[1][1] + hexagon[2][1]) / 3;

    Fill(hdc, (int)zatr.x, (int)zatr.y, clr);
}

void Line(HDC hdc, int x1, int y1, int x2, int y2, COLORREF clr) // ������� �����
{
    HPEN hPen; //����������� �����
    hPen = CreatePen(PS_SOLID, 1, clr); //�������� ������
    SelectObject(hdc, hPen); //������ �������� �������

    MoveToEx(hdc, x1, y1, NULL); //������� �������� ���������� x1, y1
    LineTo(hdc, x2, y2);
    DeleteObject(hPen);
}

void PolyScan_P(HDC hdc, double hexagon[Mm_pyr][Nn], COLORREF clr = rgbBlack)
{
    point polypoint[Mm_pyr]; // ������ �����

    for (int count_1 = 0; count_1 < Mm_pyr; count_1++) // ��������� ���
    {
        polypoint[count_1].x = round(hexagon[count_1][0]);
        polypoint[count_1].y = round(hexagon[count_1][1]);
    }

    int MaxY = 0; // ����� ������� y-����������
    int i;
    for (i = 0; i < Mm_pyr; i++)
    {
        if (polypoint[i].y > MaxY)
        {
            MaxY = (int)polypoint[i].y;
        }
    }

    int MinY = MaxY; // ����� ������ y-����������
    int k;
    for (k = 0; k < Mm_pyr; k++)
    {
        if (polypoint[k].y < MinY)
        {
            MinY = (int)polypoint[k].y;
        }
    }

    AET* pAET = new AET; // ������� �������� �����
    pAET->next = NULL;

    NET** pNET = new NET *[MaxY - MinY]; // ����� ������� �����
    for (i = MinY; i <= MaxY; i++)
    {
        pNET[i - MinY] = new NET;
        pNET[i - MinY]->next = NULL;
    }

    for (i = MinY; i <= MaxY; i++) // ��������� � ��������� ����� ������� �����
    {
        for (int j = 0; j < Mm_pyr; j++)
        {
            if (polypoint[j].y == i)
            {
                if (polypoint[(j - 1 + Mm_pyr) % Mm_pyr].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j - 1 + Mm_pyr) % Mm_pyr].y;
                    p->dx = (polypoint[(j - 1 + Mm_pyr) % Mm_pyr].x - polypoint[j].x) / (polypoint[(j - 1 + Mm_pyr) % Mm_pyr].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
                if (polypoint[(j + 1 + Mm_pyr) % Mm_pyr].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j + 1 + Mm_pyr) % Mm_pyr].y;
                    p->dx = (polypoint[(j + 1 + Mm_pyr) % Mm_pyr].x - polypoint[j].x) / (polypoint[(j + 1 + Mm_pyr) % Mm_pyr].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
            }
        }
    }

    for (i = MinY; i <= MaxY; i++) // ������������� � ��������� �������� ������� �����
    {
        NET* p = pAET->next; // ����������� ����� X-�����������, ��������� �������� �����
        while (p)
        {
            p->xIntersect = p->xIntersect + p->dx;
            p = p->next;
        }
        // �������� ����� ������ ���������� ����� �������� �����
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

        /* � ����� ����� NET ����������� �������� �����, �������� X ������������� ����� ����������*/
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
        while (p && p->next) // ����������� ������
        {
            if (i >= 0)
            {
                Line(hdc, (int)p->xIntersect, i, (int)p->next->xIntersect + 1, i, clr);
            }
            p = p->next->next; // ��������� � ��������� �����
        }
    }
}

void PolyScan(HDC hdc, double hexagon[Mm][Nn], COLORREF clr = rgbBlack)
{
    point polypoint[Mm]; // ������ �����

    for (int count_1 = 0; count_1 < Mm; count_1++) // ��������� ���
    {
        polypoint[count_1].x = round(hexagon[count_1][0]);
        polypoint[count_1].y = round(hexagon[count_1][1]);
    }

    int MaxY = 0; // ����� ������� y-����������
    int i;
    for (i = 0; i < Mm; i++)
    {
        if (polypoint[i].y > MaxY)
        {
            MaxY = (int)polypoint[i].y;
        }
    }

    int MinY = MaxY; // ����� ������ y-����������
    int k;
    for (k = 0; k < Mm; k++)
    {
        if (polypoint[k].y < MinY)
        {
            MinY = (int)polypoint[k].y;
        }
    }

    AET* pAET = new AET; // ������� �������� �����
    pAET->next = NULL;

    NET** pNET = new NET * [MaxY - MinY]; // ����� ������� �����
    for (i = MinY; i <= MaxY; i++)
    {
        pNET[i - MinY] = new NET;
        pNET[i - MinY]->next = NULL;
    }

    for (i = MinY; i <= MaxY; i++) // ��������� � ��������� ����� ������� �����
    {
        for (int j = 0; j < Mm; j++)
        {
            if (polypoint[j].y == i)
            {
                if (polypoint[(j - 1 + Mm) % Mm].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j - 1 + Mm) % Mm].y;
                    p->dx = (polypoint[(j - 1 + Mm) % Mm].x - polypoint[j].x) / (polypoint[(j - 1 + Mm) % Mm].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
                if (polypoint[(j + 1 + Mm) % Mm].y > polypoint[j].y)
                {
                    NET* p = new NET;
                    p->xIntersect = polypoint[j].x;
                    p->ymax = polypoint[(j + 1 + Mm) % Mm].y;
                    p->dx = (polypoint[(j + 1 + Mm) % Mm].x - polypoint[j].x) / (polypoint[(j + 1 + Mm) % Mm].y - polypoint[j].y);
                    p->next = pNET[i - MinY]->next;
                    pNET[i - MinY]->next = p;
                }
            }
        }
    }

    for (i = MinY; i <= MaxY; i++) // ������������� � ��������� �������� ������� �����
    {
        NET* p = pAET->next; // ����������� ����� X-�����������, ��������� �������� �����
        while (p)
        {
            p->xIntersect = p->xIntersect + p->dx;
            p = p->next;
        }
        // �������� ����� ������ ���������� ����� �������� �����
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

        /* � ����� ����� NET ����������� �������� �����, �������� X ������������� ����� ����������*/
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
        while (p && p->next) // ����������� ������
        {
            if (i >= 0)
            {
                Line(hdc, (int)p->xIntersect, i, (int)p->next->xIntersect + 1, i, clr);
            }
            p = p->next->next; // ��������� � ��������� �����
        }
    }
}