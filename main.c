
/*
 gcc -Wall -std=c99 -m64 -O2  -c main.c -o main.o
 windres.exe   -J rc -O coff -i resource.rc -o resource.res
 gcc -o colorinter main.o resource.res -static -m64 -lgdi32 -s -mwindows
*/

#define UNICODE
#include <windows.h>
#include "resource.h"
#include <stdio.h>

int WIDTH = 750;
int HEIGHT = 606;
int squarePosX = 107;
int squarePosY = 15;
int squareSize = 10;

int secondBox = 105;
int activateSecondBox = 0;
int whichButton = 1;
int ItemIndex = 0;
int allowAlpha = 0;
int isDelay = 1;

HWND hHexBtn, hRGBBtn, hRGBPercentBtn, hTAlphaBtn, hClearBtn, hComboBox, hTextBox1, hTextBox2, hTextBox3,
     hTextBox4, hTextBox5, hTextBox6, hTextBox7, hTextBox8, hTextBox9, hTextBox10, hTextBox11, hTextBox12;

HFONT hFont;
RECT clientRect, screenRect;
PAINTSTRUCT ps;
TCHAR ListItem[12];

#define buffed 30

struct _COLORCHART
{
    RECT colorSquare;
    unsigned int  red;
    unsigned int  green;
    unsigned int  blue;
    unsigned int  alpha;
};

struct _COLORCHART* clrChart;
struct _COLORCHART* txtBox1ColorChart;
struct _COLORCHART* txtBox2ColorChart;

int isUpdateColors = 0;
int isChanged = 0;
int isBx1 = 0;
int isBx2 = 0;
int lastfocus = 0;

#define BARRAYS 28

int backgroundArrays[BARRAYS];

int lerp(int a, int b, float t)
{
    return a + (int)(t * (float)(b-a));
}

void whichButtonChoice()
{
    wchar_t str1[buffed] = {'\0'};
    wchar_t str2[buffed] = {'\0'};
    if(whichButton == 1)
    {
        if(isBx1 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str1, buffed, L"#%02X%02X%02X%02X", txtBox1ColorChart->red, txtBox1ColorChart->green,
                                             txtBox1ColorChart->blue, txtBox1ColorChart->alpha);
            } else {
                swprintf(str1, buffed, L"#%02X%02X%02X", txtBox1ColorChart->red, txtBox1ColorChart->green,
                                             txtBox1ColorChart->blue);
            }
            SetWindowTextW(hTextBox1, str1);
        }
        if(isBx2 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str2, buffed, L"#%02X%02X%02X%02X", txtBox2ColorChart->red, txtBox2ColorChart->green,
                                             txtBox2ColorChart->blue, txtBox2ColorChart->alpha);
            } else {
                swprintf(str2, buffed, L"#%02X%02X%02X", txtBox2ColorChart->red, txtBox2ColorChart->green,
                                             txtBox2ColorChart->blue);
            }
            SetWindowTextW(hTextBox2, str2);
        }
    } else if(whichButton == 2)
    {
        if(isBx1 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str1, buffed, L"rgb(%d,%d,%d,%d)\n", txtBox1ColorChart->red, txtBox1ColorChart->green,
                                            txtBox1ColorChart->blue, txtBox1ColorChart->alpha);
            } else {
                swprintf(str1, buffed, L"rgb(%d,%d,%d)\n", txtBox1ColorChart->red, txtBox1ColorChart->green,
                                            txtBox1ColorChart->blue);
            }
            SetWindowTextW(hTextBox1, str1);
        }

        if(isBx2 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str2, buffed, L"rgb(%d,%d,%d,%d)\n", txtBox2ColorChart->red, txtBox2ColorChart->green,
                                             txtBox2ColorChart->blue, txtBox2ColorChart->alpha);
            } else {
                swprintf(str2, buffed, L"rgb(%d,%d,%d)\n", txtBox2ColorChart->red, txtBox2ColorChart->green,
                                             txtBox2ColorChart->blue);
            }
            SetWindowTextW(hTextBox2, str2);
        }
    } else if(whichButton == 3)
    {
        if(isBx1 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str1, buffed, L"rgb(%d%,%d%,%d%,%.001f)\n", ((txtBox1ColorChart->red   * 100) / 255),
                                                               ((txtBox1ColorChart->green * 100) / 255),
                                                               ((txtBox1ColorChart->blue  * 100) / 255),
                                                               (((txtBox1ColorChart->alpha * 0.1) / 255) * 10));
            } else {
                swprintf(str1, buffed, L"rgb(%d%,%d%,%d%)\n", ((txtBox1ColorChart->red   * 100) / 255),
                                                               ((txtBox1ColorChart->green * 100) / 255),
                                                               ((txtBox1ColorChart->blue  * 100) / 255));
            }
            SetWindowTextW(hTextBox1, str1);
        }

        if(isBx2 == 1)
        {
            if(allowAlpha == 1)
            {
                swprintf(str2, buffed, L"rgb(%d%,%d%,%d%,%.001f)\n", ((txtBox2ColorChart->red   * 100) / 255),
                                                               ((txtBox2ColorChart->green * 100) / 255),
                                                               ((txtBox2ColorChart->blue  * 100) / 255),
                                                               (((txtBox2ColorChart->alpha * 0.1) / 255) * 10));
            } else {
                swprintf(str2, buffed, L"rgb(%d%,%d%,%d%)\n", ((txtBox2ColorChart->red   * 100) / 255),
                                                               ((txtBox2ColorChart->green * 100) / 255),
                                                               ((txtBox2ColorChart->blue  * 100) / 255));
            }
            SetWindowTextW(hTextBox2, str2);
        }
    }
}

void clearTBs()
{
    if(isDelay == 1)
    {
        ShowWindow(hTextBox3, SW_HIDE);
        ShowWindow(hTextBox4, SW_HIDE);
        ShowWindow(hTextBox5, SW_HIDE);
        ShowWindow(hTextBox6, SW_HIDE);
        ShowWindow(hTextBox7, SW_HIDE);
        ShowWindow(hTextBox8, SW_HIDE);
        ShowWindow(hTextBox9, SW_HIDE);
        ShowWindow(hTextBox10, SW_HIDE);
        ShowWindow(hTextBox11, SW_HIDE);
        ShowWindow(hTextBox12, SW_HIDE);
    }
}

void renderBlending(HDC hdc)
{
    if(ItemIndex > 0)
    {
        activateSecondBox = 0;
        if(isUpdateColors > 0)
        {
            COLORREF currentColor;
            RECT tempRect;
            if(isBx1 == 1 && isBx2 == 1)
            {
                    wchar_t str[buffed] = {'\0'};
                    float steps = ItemIndex;
                    tempRect.bottom  = 92;
                    int loopy = tempRect.bottom - 25;
                    SetWindowPos(hTextBox2, 0, 175, (secondBox + (ItemIndex * 35)), 238, 27, 0);
                    int redstart = txtBox1ColorChart->red;
                    int greenstart = txtBox1ColorChart->green;
                    int bluestart = txtBox1ColorChart->blue;
                    int redend = txtBox2ColorChart->red;
                    int greenend = txtBox2ColorChart->green;
                    int blueend = txtBox2ColorChart->blue;
                    for(int g = 1; g <= (ItemIndex + 1); g++)
                    {
                        int redvalue = lerp(redstart, redend, (g / (steps + 1) ) );
                        int greenvalue = lerp(greenstart, greenend, (g / (steps + 1) ) );
                        int bluevalue = lerp(bluestart, blueend, (g / (steps + 1) ) );

                        tempRect.left    = 174;
                        tempRect.top     = loopy;
                        tempRect.right   = 414;
                        tempRect.bottom  = loopy + 25;
                        currentColor = SetBkColor(hdc, RGB(255, 255, 255));
                        ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                        SetBkColor(hdc, currentColor);

                        loopy += 35;
                        tempRect.left    = 425;
                        tempRect.top     = loopy;
                        tempRect.right   = 575;
                        tempRect.bottom  = loopy + 25;
                        currentColor = SetBkColor(hdc, RGB(redvalue, greenvalue, bluevalue));
                        ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                        SetBkColor(hdc, currentColor);

                        if(whichButton == 1)
                        {
                            if(allowAlpha == 1)
                            {
                                swprintf(str, buffed, L"#%02X%02X%02XFF", redvalue, greenvalue, bluevalue);
                            } else {
                                swprintf(str, buffed, L"#%02X%02X%02X", redvalue, greenvalue, bluevalue);
                            }
                        } else if(whichButton == 2)
                        {
                            if(allowAlpha == 1)
                            {
                                swprintf(str, buffed, L"rgb(%d,%d,%d,255)\n", redvalue, greenvalue, bluevalue);
                            } else {
                                swprintf(str, buffed, L"rgb(%d,%d,%d)\n", redvalue, greenvalue, bluevalue);
                            }
                        } else if(whichButton == 3)
                        {
                            if(allowAlpha == 1)
                            {
                                swprintf(str, buffed, L"rgb(%d%,%d%,%d%,%.001f)\n", ((redvalue   * 100) / 255),
                                                                               ((greenvalue * 100) / 255),
                                                                               ((bluevalue  * 100) / 255),
                                                                               (((255 * 0.1) / 255) * 10));
                            } else {
                                swprintf(str, buffed, L"rgb(%d%,%d%,%d%)\n", ((redvalue   * 100) / 255),
                                                                               ((greenvalue * 100) / 255),
                                                                               ((bluevalue  * 100) / 255));
                            }
                        }

                    if(isDelay == 1)
                    {
                            if(g == 1  && ItemIndex >= 1)  {ShowWindow(hTextBox3,  SW_SHOW); SetWindowTextW(hTextBox3, str);} else  {if(ItemIndex < 1){ShowWindow(hTextBox3,  SW_HIDE);}}
                            if(g == 2  && ItemIndex >= 2)  {ShowWindow(hTextBox4,  SW_SHOW); SetWindowTextW(hTextBox4, str);} else {if(ItemIndex < 2){ShowWindow(hTextBox4,  SW_HIDE);}}
                            if(g == 3  && ItemIndex >= 3)  {ShowWindow(hTextBox5,  SW_SHOW); SetWindowTextW(hTextBox5, str);} else {if(ItemIndex < 3){ShowWindow(hTextBox5,  SW_HIDE);}}
                            if(g == 4  && ItemIndex >= 4)  {ShowWindow(hTextBox6,  SW_SHOW); SetWindowTextW(hTextBox6, str);} else {if(ItemIndex < 4){ShowWindow(hTextBox6,  SW_HIDE);}}
                            if(g == 5  && ItemIndex >= 5)  {ShowWindow(hTextBox7,  SW_SHOW); SetWindowTextW(hTextBox7, str);} else {if(ItemIndex < 5){ShowWindow(hTextBox7,  SW_HIDE);}}
                            if(g == 6  && ItemIndex >= 6)  {ShowWindow(hTextBox8,  SW_SHOW); SetWindowTextW(hTextBox8, str);} else {if(ItemIndex < 6){ShowWindow(hTextBox8,  SW_HIDE);}}
                            if(g == 7  && ItemIndex >= 7)  {ShowWindow(hTextBox9,  SW_SHOW); SetWindowTextW(hTextBox9, str);} else {if(ItemIndex < 7){ShowWindow(hTextBox9,  SW_HIDE);}}
                            if(g == 8  && ItemIndex >= 8)  {ShowWindow(hTextBox10, SW_SHOW); SetWindowTextW(hTextBox10, str);} else {if(ItemIndex < 8){ShowWindow(hTextBox10,  SW_HIDE);}}
                            if(g == 9  && ItemIndex >= 9)  {ShowWindow(hTextBox11, SW_SHOW); SetWindowTextW(hTextBox11, str);} else {if(ItemIndex < 9){ShowWindow(hTextBox11,  SW_HIDE);}}
                            if(g == 10  && ItemIndex == 10) {ShowWindow(hTextBox12, SW_SHOW); SetWindowTextW(hTextBox12, str);} else {if(ItemIndex < 10){ShowWindow(hTextBox12,  SW_HIDE);}}
                    }
                }
                tempRect.left    = 174;
                tempRect.top     = loopy;
                tempRect.right   = 414;
                tempRect.bottom  = loopy + 25;
                currentColor = SetBkColor(hdc, RGB(255, 255, 255));
                ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                SetBkColor(hdc, currentColor);
                isChanged = 0;
            } else if(isBx1 == 1)
            {
                    tempRect.left    = 425;
                    tempRect.top     = 140;
                    tempRect.right   = 575;
                    tempRect.bottom  = 164;
                    currentColor = SetBkColor(hdc, RGB(txtBox1ColorChart->red, txtBox1ColorChart->green, txtBox1ColorChart->blue));
                    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                    SetBkColor(hdc, currentColor);
                    isChanged = 0;
            } else if(isBx2 == 1)
            {
                    tempRect.left    = 425;
                    tempRect.top     = 140;
                    tempRect.right   = 575;
                    tempRect.bottom  = 164;
                    currentColor = SetBkColor(hdc, RGB(txtBox2ColorChart->red, txtBox2ColorChart->green, txtBox2ColorChart->blue));
                    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                    SetBkColor(hdc, currentColor);
                    isChanged = 0;
            }
        }
    }
}

void updateChartColors()
{
    int green = 255;
    int red   = 255;
    int alpha = 255;
    int rc = 0;
    int totalChartNumber = 0;
    for(int j = 1; j < 37; j++)
    {
        int blue  = 255;
        for(int t = 1; t < 7; t++)
        {
            clrChart[totalChartNumber].colorSquare.left   = (t * 12) + (WIDTH - squarePosX);
            clrChart[totalChartNumber].colorSquare.top    = (j * 12) + squarePosY;
            clrChart[totalChartNumber].colorSquare.right  = (t * 12) + (WIDTH - (squarePosX - squareSize));
            clrChart[totalChartNumber].colorSquare.bottom = (j * 12) + (squarePosY + squareSize);
            clrChart[totalChartNumber].red   = red;
            clrChart[totalChartNumber].green = green;
            clrChart[totalChartNumber].blue  = blue;
            clrChart[totalChartNumber].alpha = alpha;
            blue -= 51;
            totalChartNumber++;
        }
        green -= 51;
        if(green < 0){green = 255;}
        rc++;
        if(rc >= 6)
        {
            rc = 0;
            red   -= 51;
        }
    }
}

void copyToClip(HWND hWn)
{
    wchar_t strData[24];
    SendMessage(hWn, WM_GETTEXT, (WPARAM)24, (LPARAM)strData);
    HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, sizeof(wchar_t) * (wcslen(strData) + 1));
    wchar_t* pchData;
    pchData = (wchar_t*)GlobalLock(hClipboardData);
    wcscpy(pchData, strData);
    GlobalUnlock(hClipboardData);
    OpenClipboard(0);
    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, hClipboardData);
    CloseClipboard();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO minSize = (LPMINMAXINFO)lParam;
            minSize->ptMinTrackSize.x = 750;
            minSize->ptMinTrackSize.y = 606;
            break;
        }
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case 1:
                {
                    whichButton = 1;
                    whichButtonChoice();
                    if(lastfocus == 2)
                    {
                        SetFocus(hTextBox2);
                    } else {
                        SetFocus(hTextBox1);
                    }
                    isChanged = 1;
                    InvalidateRect(hWnd, 0, 1);
                    break;
                }
                case 2:
                {
                    whichButton = 2;
                    whichButtonChoice();
                    if(lastfocus == 2)
                    {
                        SetFocus(hTextBox2);
                    } else {
                        SetFocus(hTextBox1);
                    }
                    isChanged = 1;
                    InvalidateRect(hWnd, 0, 1);
                    break;
                }
                case 3:
                {
                    whichButton = 3;
                    whichButtonChoice();
                    if(lastfocus == 2)
                    {
                        SetFocus(hTextBox2);
                    } else {
                        SetFocus(hTextBox1);
                    }
                    isChanged = 1;
                    InvalidateRect(hWnd, 0, 1);
                    break;
                }
                case 4:
                {
                    allowAlpha = !allowAlpha;
                    whichButtonChoice();
                    if(lastfocus == 2)
                    {
                        SetFocus(hTextBox2);
                    } else {
                        SetFocus(hTextBox1);
                    }
                    isChanged = 1;
                    InvalidateRect(hWnd, 0, 1);
                    break;
                }
                case 5:
                {
                    ItemIndex = 0;
                    EnableWindow(hComboBox, 0);
                    SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)ItemIndex, (LPARAM)ListItem);
                    activateSecondBox = 1;
                    isBx1 = 0;
                    isBx2 = 0;
                    SetFocus(hTextBox1);
                    SetWindowTextW(hTextBox1, L"");
                    SetWindowTextW(hTextBox2, L"");
                    clearTBs();
                    isChanged = 0;
                    InvalidateRect(hWnd, 0, 1);
                    break;
                }
                case 6:
                {
                    if(HIWORD(wParam) == CBN_SELCHANGE)
                    {
                        ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                        SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
                        if(ItemIndex == 0){clearTBs();}
                        if(lastfocus == 2)
                        {
                            SetFocus(hTextBox2);
                        } else {
                            SetFocus(hTextBox1);
                        }
                        isChanged = 1;
                        InvalidateRect(hWnd, 0, 1);
                    }
                    break;
                }
                case 8:
                {
                    copyToClip(hTextBox1);

                    HWND tHandle = (HWND)lParam;
                    HWND hResult = GetFocus();
                    if(tHandle == hTextBox1 && hResult == hTextBox1)
                    {
                        lastfocus = 1;
                        isChanged = 1;
                        InvalidateRect(hWnd, 0, 1);
                    }
                    break;
                }
                case 9:
                {
                    copyToClip(hTextBox2);

                    HWND tHandle = (HWND)lParam;
                    HWND hResult = GetFocus();
                    if(tHandle == hTextBox2 && hResult == hTextBox2)
                    {
                        lastfocus = 2;
                        isChanged = 1;
                        InvalidateRect(hWnd, 0, 1);
                    }
                    break;
                }
                case 60:
                {
                    copyToClip(hTextBox3);
                    break;
                }
                case 61:
                {
                    copyToClip(hTextBox4);
                    break;
                }
                case 62:
                {
                    copyToClip(hTextBox5);
                    break;
                }
                case 63:
                {
                    copyToClip(hTextBox6);
                    break;
                }
                case 64:
                {
                    copyToClip(hTextBox7);
                    break;
                }
                case 65:
                {
                    copyToClip(hTextBox8);
                    break;
                }
                case 66:
                {
                    copyToClip(hTextBox9);
                    break;
                }
                case 67:
                {
                    copyToClip(hTextBox10);
                    break;
                }
                case 68:
                {
                    copyToClip(hTextBox11);
                    break;
                }
                case 69:
                {
                    copyToClip(hTextBox12);
                    break;
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            POINT mp;
            GetCursorPos(&mp);
            ScreenToClient(hWnd, &mp);
            for(int t = 0; t < 216; t++)
            {
                if((mp.x >= (clrChart[t].colorSquare.left)) && (mp.x <= (clrChart[t].colorSquare.right))
                    && (mp.y >= clrChart[t].colorSquare.top) && (mp.y <= clrChart[t].colorSquare.bottom))
                {
                    HWND hResult = GetFocus();
                    if(hResult == hTextBox2)
                    {
                        txtBox2ColorChart->colorSquare.left   = 425;
                        txtBox2ColorChart->colorSquare.top    = 102;
                        txtBox2ColorChart->colorSquare.right  = 575;
                        txtBox2ColorChart->colorSquare.bottom = 127;
                        txtBox2ColorChart->red                = clrChart[t].red;
                        txtBox2ColorChart->green              = clrChart[t].green;
                        txtBox2ColorChart->blue               = clrChart[t].blue;
                        txtBox2ColorChart->alpha              = clrChart[t].alpha;
                        isBx2 = 1;
                        isChanged = 1;
                        isUpdateColors = 1;
                        InvalidateRect(hWnd, 0, 1);
                    } else if(hResult == hTextBox1) {
                        txtBox1ColorChart->colorSquare.left   = 425;
                        txtBox1ColorChart->colorSquare.top    = 65;
                        txtBox1ColorChart->colorSquare.right  = 575;
                        txtBox1ColorChart->colorSquare.bottom = 90;
                        txtBox1ColorChart->red                = clrChart[t].red;
                        txtBox1ColorChart->green              = clrChart[t].green;
                        txtBox1ColorChart->blue               = clrChart[t].blue;
                        txtBox1ColorChart->alpha              = clrChart[t].alpha;
                        isBx1 = 1;
                        isChanged = 1;
                        isUpdateColors = 1;
                        InvalidateRect(hWnd, 0, 1);
                    }
                    whichButtonChoice();
                }
            }
            break;
        }
        case WM_CREATE:
        {
            hFont = CreateFontW(20, 8, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
                                  CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, (LPCWSTR)(L"Arial"));

            backgroundArrays[0]  = 10;
            backgroundArrays[1]  = 10;
            backgroundArrays[2]  = 160;
            backgroundArrays[3]  = (HEIGHT - 10);
            backgroundArrays[4]  = 24;
            backgroundArrays[5]  = 24;
            backgroundArrays[6]  = 24;
            backgroundArrays[7]  = 161;
            backgroundArrays[8]  = 10;
            backgroundArrays[9]  = (WIDTH - 10);
            backgroundArrays[10] = (HEIGHT - 10);
            backgroundArrays[11] = 14;
            backgroundArrays[12] = 14;
            backgroundArrays[13] = 14;
            backgroundArrays[14] = (WIDTH - 117);
            backgroundArrays[15] = 20;
            backgroundArrays[16] = (WIDTH - 35);
            backgroundArrays[17] = 465;
            backgroundArrays[18] = 74;
            backgroundArrays[19] = 74;
            backgroundArrays[20] = 74;
            backgroundArrays[21] = (WIDTH - 114);
            backgroundArrays[22] = 23;
            backgroundArrays[23] = (WIDTH - 38);
            backgroundArrays[24] = 462;
            backgroundArrays[25] = 5;
            backgroundArrays[26] = 5;
            backgroundArrays[27] = 5;

            clrChart = malloc(sizeof(struct _COLORCHART) * 216);
            txtBox1ColorChart = malloc(sizeof(struct _COLORCHART));
            txtBox2ColorChart = malloc(sizeof(struct _COLORCHART));

            updateChartColors();

            HINSTANCE hInstance = GetModuleHandle(NULL);
            const TCHAR *items[] = {L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"10"};

            hTAlphaBtn     = CreateWindowExW(WS_EX_APPWINDOW, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 20, 30, 120, 21, hWnd,  (HMENU)4, hInstance, NULL);

            hHexBtn        = CreateWindowExW(WS_EX_APPWINDOW, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 178, 30, 55, 21, hWnd,  (HMENU)1, hInstance, NULL);
            hRGBBtn        = CreateWindowExW(WS_EX_APPWINDOW, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 267, 30, 55, 21, hWnd,  (HMENU)2, hInstance, NULL);
            hRGBPercentBtn = CreateWindowExW(WS_EX_APPWINDOW, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 355, 30, 55, 21, hWnd,  (HMENU)3, hInstance, NULL);
            hClearBtn      = CreateWindowExW(WS_EX_APPWINDOW, L"BUTTON", NULL, WS_CHILD | WS_VISIBLE, 465, 30, 70, 21, hWnd,  (HMENU)5, hInstance, NULL);

            SetWindowTextW(hTAlphaBtn, L"&TOGGLE ALPHA");
            SetWindowTextW(hHexBtn, L"&HEX");
            SetWindowTextW(hRGBBtn, L"&RGB");
            SetWindowTextW(hRGBPercentBtn, L"&PRGB");
            SetWindowTextW(hClearBtn, L"&CLEAR");

            hComboBox = CreateWindowExW(WS_EX_APPWINDOW, L"combobox", NULL, EM_SETREADONLY | WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | WS_DISABLED,
                                         20, 140, 50, 210, hWnd,  (HMENU)6, hInstance, NULL);

            int i;
            for (i = 0; i < 11; i++ )
            {
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)items[i]);
            }

            hTextBox1 = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_UPPERCASE, 175, 68, 238, 27, hWnd, (HMENU)8, hInstance, NULL);
            hTextBox2 = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, WS_CHILD | WS_VISIBLE | ES_UPPERCASE, 175, secondBox, 238, 27, hWnd, (HMENU)9, hInstance, NULL);

            hTextBox3  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 103, 238, 24, hWnd, (HMENU)60, hInstance, NULL);
            hTextBox4  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 138, 238, 24, hWnd, (HMENU)61, hInstance, NULL);
            hTextBox5  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 173, 238, 24, hWnd, (HMENU)62, hInstance, NULL);
            hTextBox6  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 208, 238, 24, hWnd, (HMENU)63, hInstance, NULL);
            hTextBox7  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 243, 238, 24, hWnd, (HMENU)64, hInstance, NULL);
            hTextBox8  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 278, 238, 24, hWnd, (HMENU)65, hInstance, NULL);
            hTextBox9  = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 313, 238, 24, hWnd, (HMENU)66, hInstance, NULL);
            hTextBox10 = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 348, 238, 24, hWnd, (HMENU)67, hInstance, NULL);
            hTextBox11 = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 383, 238, 24, hWnd, (HMENU)68, hInstance, NULL);
            hTextBox12 = CreateWindowExW(WS_EX_TRANSPARENT, L"EDIT", NULL, ES_READONLY | WS_CHILD, 175, 418, 238, 24, hWnd, (HMENU)69, hInstance, NULL);

            SendMessage (hTextBox1, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox2, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage (hTextBox3, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox4, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox5, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox6, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox7, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox8, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox9, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox10, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox11, WM_SETFONT, (WPARAM)hFont, TRUE);
            SendMessage (hTextBox12, WM_SETFONT, (WPARAM)hFont, TRUE);

            SendMessage(hComboBox, CB_SETCURSEL, (WPARAM)0, (LPARAM)ListItem);
            SetWindowTextW(hTextBox1, L"");
            SetWindowTextW(hTextBox2, L"");


            txtBox1ColorChart->red = 0;
            txtBox1ColorChart->green = 0;
            txtBox1ColorChart->blue = 0;
            txtBox2ColorChart->red = 0;
            txtBox2ColorChart->green = 0;
            txtBox2ColorChart->blue = 0;

            SetRect(&screenRect, 20, 30, 900, 800);
            break;
        }
        case WM_SIZE:
        {
            isDelay = 0;
            GetClientRect(hWnd, &clientRect);

            WIDTH  = clientRect.right;
            HEIGHT = clientRect.bottom;

            backgroundArrays[3]  = (HEIGHT -  10);
            backgroundArrays[9]  = (WIDTH  -  10);
            backgroundArrays[10] = (HEIGHT -  10);
            backgroundArrays[14] = (WIDTH  - 101);
            backgroundArrays[16] = (WIDTH  -  19);
            backgroundArrays[21] = (WIDTH  -  99);
            backgroundArrays[23] = (WIDTH  -  21);

            updateChartColors();
            isChanged = 1;
            break;
        }
        case WM_PAINT:
        {
            HDC hdc = BeginPaint(hWnd, &ps);
                RECT tempRect;
                COLORREF currentColor;
                for(int t = 0; t < BARRAYS; t++)
                {
                    tempRect.left    = backgroundArrays[t + 0];
                    tempRect.top     = backgroundArrays[t + 1];
                    tempRect.right   = backgroundArrays[t + 2];
                    tempRect.bottom  = backgroundArrays[t + 3];
                    currentColor = SetBkColor(hdc, RGB(backgroundArrays[t + 4], backgroundArrays[t + 5], backgroundArrays[t + 6]));
                    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                    SetBkColor(hdc, currentColor);
                    t += 6;
                }

                for(int j = 0; j < 216; j++)
                {
                    currentColor = SetBkColor(hdc, RGB(clrChart[j].red, clrChart[j].green, clrChart[j].blue));
                    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &clrChart[j].colorSquare, L"", 0, 0);
                    SetBkColor(hdc, currentColor);
                }

                if(isUpdateColors > 0)
                {
                    if(isBx1)
                    {
                        currentColor = SetBkColor(hdc, RGB(txtBox1ColorChart->red, txtBox1ColorChart->green, txtBox1ColorChart->blue));
                        ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &txtBox1ColorChart->colorSquare, L"", 0, 0);
                        SetBkColor(hdc, currentColor);
                    }
                    if(isBx2)
                    {
                        currentColor = SetBkColor(hdc, RGB(txtBox2ColorChart->red, txtBox2ColorChart->green, txtBox2ColorChart->blue));
                        ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &txtBox2ColorChart->colorSquare, L"", 0, 0);
                        SetBkColor(hdc, currentColor);
                    }
                }

                tempRect.left    = 174;
                tempRect.top     = 65;
                tempRect.right   = 414;
                tempRect.bottom  = 90;
                currentColor = SetBkColor(hdc, RGB(255, 255, 255));
                ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                SetBkColor(hdc, currentColor);

                if(ItemIndex > 0 && isChanged == 1)
                {
                    renderBlending(hdc);
                } else {
                    if(isBx1 == 1 && isBx2 == 1) {EnableWindow(hComboBox, 1);}
                    tempRect.left    = 174;
                    tempRect.top     = secondBox - 3;
                    tempRect.right   = 414;
                    tempRect.bottom  = secondBox + 22;
                    currentColor = SetBkColor(hdc, RGB(255, 255, 255));
                    ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &tempRect, L"", 0, 0);
                    SetBkColor(hdc, currentColor);
                    if(ItemIndex == 0) {SetWindowPos(hTextBox2, 0, 175, secondBox, 238, 27, 0); activateSecondBox = 0;}
                }

            EndPaint(hWnd, &ps);
            break;
        }
        case WM_CTLCOLOREDIT:
        {
            HWND tHandle = (HWND)lParam;
            HDC hdc = (HDC)wParam;
            if (tHandle == hTextBox1)
            {
                if(lastfocus == 1)
                {
                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkColor(hdc, RGB(200, 200, 0));
                } else {
                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkColor(hdc, RGB(255, 255, 255));
                }
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            if (tHandle == hTextBox2)
            {
                if(lastfocus == 2)
                {
                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkColor(hdc, RGB(200, 200, 0));
                } else {
                    SetTextColor(hdc, RGB(0, 0, 0));
                    SetBkColor(hdc, RGB(255, 255, 255));
                }
                return (LRESULT)GetStockObject(NULL_BRUSH);
            }
            break;
        }
        case WM_DESTROY:
        {
            DeleteObject(hFont);
            PostQuitMessage(0);
            break;
        }
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void CenterWindow(HWND hWnd)
{
   RECT rec1, rec2;
   GetWindowRect(hWnd, &rec1);
   GetWindowRect(GetDesktopWindow(), &rec2);
   MoveWindow(hWnd, ((rec2.right - rec2.left) - (rec1.right - rec1.left)) / 2,
                    ((rec2.bottom - rec2.top) - (rec1.bottom - rec1.top)) / 2,
                    (rec1.right - rec1.left), (rec1.bottom - rec1.top), 0);
}

int main()
{
    HMODULE hMod = GetModuleHandleW(0);
    WNDCLASSEXW wClass = {sizeof(WNDCLASSEXW), CS_VREDRAW|CS_HREDRAW, WndProc, 0, 0,
                         (HINSTANCE)hMod, LoadIconW(hMod, MAKEINTRESOURCE(IDI_ICON)),
                         LoadCursorW(NULL, MAKEINTRESOURCEW(IDC_ARROW)),
                         CreateSolidBrush(RGB(64, 64, 64)), NULL, L"window", NULL};

    if(!RegisterClassExW(&wClass))
    {
        return GetLastError();
    }

    HWND window = CreateWindowExW(0,L"window", L"Color Interpolator  v0.9",
                  WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                  WIDTH, HEIGHT, 0, 0, (HINSTANCE)hMod, 0);


    if(!window)
    {
        return GetLastError();
    }

    OSVERSIONINFOEXW info;
    ZeroMemory(&info, sizeof(OSVERSIONINFOEXW));
    info.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    GetVersionExW((LPOSVERSIONINFO)&info);
    if((info.dwMajorVersion > 5) && (info.dwMinorVersion >= 2))
    {
        CenterWindow(window);
    }

    SetFocus(hTextBox1);
    isBx1 = 1;

    MSG msg;
    int run = 1;
    int checkTimer = 0;

    while(run)
    {
        Sleep(isDelay);
        if(isDelay == 0)
        {
            checkTimer++;
            if(checkTimer > 10000){isDelay = 1; checkTimer = 0;}
        }
        while(PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) > 0)
        {
            if(WM_QUIT == msg.message) {run = 0;}
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    free(clrChart);
    free(txtBox1ColorChart);
    free(txtBox2ColorChart);
    DestroyWindow(window);
    return errno;
}


