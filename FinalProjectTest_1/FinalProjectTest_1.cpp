// FinalProjectTest_1.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "FinalProjectTest_1.h"
#include <windows.h>
#include <tchar.h>
#include <vector> 
#include <ctime>


#define MAX_LOADSTRING 100

// Глобальные переменные:
#define FIELD_WIDTH 13
#define FIELD_HEIGHT 13
#define CELL_SIZE 40

DWORD lastShotTime = 0; // время последнего выстрела
const DWORD SHOT_DELAY = 300; //задержка между выстрелами в миллисекундах

RECT player = { 5, 5, 6, 6 };// координаты танка на сетке
enum Direction { UP, DOWN, LEFT, RIGHT };
Direction playerDir = UP;

struct Bullet {
    int x, y;
    Direction dir;
};

struct Enemy {
    int x, y;
    Direction dir;
};

struct Explosion {
	int x, y;  
	int timer; // таймер для взрыва
};

std::vector<Bullet> bullets;
std::vector<Enemy> enemies;
std::vector<Explosion> explosions;



HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна



// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_FINALPROJECTTEST1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINALPROJECTTEST1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINALPROJECTTEST1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_FINALPROJECTTEST1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
 
    case WM_KEYDOWN:
        switch (wParam) {
        case 'W': //вверх;
            if (player.top > 0) {
                player.top--;
                player.bottom--;
            }
            playerDir = UP;
            break;

        case 'S': //вниз    
            if (player.bottom < FIELD_HEIGHT) {
                player.top++;
                player.bottom++;
            }
            playerDir = DOWN;
            break;
        case 'A': //влево
            if (player.left > 0) {
                player.left--;
                player.right--;
            }
            playerDir = LEFT;
            break;

        case 'D': //вправо
            if (player.right < FIELD_WIDTH) {
                player.left++;
                player.right++;
            }
            playerDir = RIGHT;
            break;

        case VK_SPACE: //выстрел
        {
			DWORD currentTime = GetTickCount();
            if (currentTime - lastShotTime >= SHOT_DELAY) {
                Bullet b;
                b.dir = playerDir;
                b.x = (player.left + player.right) / 2;
				b.y = (player.top + player.bottom) / 2;
                bullets.push_back(b);
				lastShotTime = currentTime; // обновляем время последнего выстрела
            }
          }
        break;
        }
        InvalidateRect(hWnd, NULL, TRUE); //перерисовка после движения
        break;


    case WM_TIMER:
        for (auto& b : bullets) {
            switch (b.dir) {
            case UP:      b.y--; break;
			case DOWN:    b.y++; break;
            case LEFT:    b.x--; break;
			case RIGHT:   b.x++; break;	
            }
          }
        for (auto& e : enemies) {
            switch (e.dir) {
            case UP: if (e.y > 0) e.y--; else e.dir = DOWN; break;
            case DOWN:if (e.y < FIELD_HEIGHT - 1) e.y++; else e.dir = UP; break;
            case LEFT: if (e.x > 0) e.x--; else e.dir = RIGHT; break;
            case RIGHT: if (e.x < FIELD_WIDTH - 1) e.x++; else e.dir = LEFT; break;
            }
        }

        for (auto bulletIt = bullets.begin(); bulletIt != bullets.end(); ) {
            bool hit = false;
            for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();) {
                if (bulletIt->x == enemyIt->x && bulletIt->y == enemyIt->y) {
                    //попадание
                    explosions.push_back({ enemyIt->x, enemyIt->y, 5 });//5 тиков по 100ьс = 500 мс;
                    enemyIt = enemies.erase(enemyIt);
                    hit = true;
                    break; //выходим из цикла, пуля тоже исчезает;
                }
                else {
                    ++enemyIt;
                }
            }
            if (hit) {
                bulletIt = bullets.erase(bulletIt);
			}
			else {
				++bulletIt;
			}
		}

        for (auto it = explosions.begin(); it != explosions.end();) {
            it->timer--;
            if (it->timer <= 0) {
                it = explosions.erase(it);
            }
            else {
                ++it;
            }
        }
		// Удаляем пули, которые вышли за пределы поля
        bullets.erase(
            std::remove_if(bullets.begin(), bullets.end(), [](Bullet& b) {
				return b.x < 0 || b.y < 0 || b.x >= FIELD_WIDTH || b.y >= FIELD_HEIGHT;
                }),
			bullets.end()
        );

        InvalidateRect(hWnd, NULL, TRUE); // Перерисовать всё окно
        break;

    case WM_CREATE:
    {
		srand((unsigned)time(NULL)); // инициализация генератора случайных чисел
		for (int i = 1; i < 5; i++) {
			Enemy e;
			e.x = rand() % FIELD_WIDTH;
			e.y = rand() % FIELD_HEIGHT;
			e.dir = static_cast<Direction>(rand() % 4);
			enemies.push_back(e);
		}
        SetTimer(hWnd, 1, 100, NULL);
        break;

    }
       
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // рисуем сетку
        for (int y = 0; y < FIELD_HEIGHT; y++) {
            for (int x = 0; x < FIELD_WIDTH; x++) {
                Rectangle(hdc, x * CELL_SIZE, y * CELL_SIZE,
                    (x + 1) * CELL_SIZE, (y + 1) * CELL_SIZE);
            }

            HBRUSH bulletBrush = CreateSolidBrush(RGB(255, 0, 0));
            for (auto& b : bullets) {
                RECT bulletRect = {
                    b.x * CELL_SIZE + CELL_SIZE / 4,
                    b.y * CELL_SIZE + CELL_SIZE / 4,
                    b.x * CELL_SIZE + 3 * CELL_SIZE / 4,
                    b.y * CELL_SIZE + 3 * CELL_SIZE / 4
                };
                FillRect(hdc, &bulletRect, bulletBrush);
            }
            DeleteObject(bulletBrush);

            HBRUSH enemyBrush = CreateSolidBrush(RGB(0, 0, 255));
            for (auto& e : enemies) {
                RECT enemyRect = {
                    e.x * CELL_SIZE,
                    e.y * CELL_SIZE,
                    (e.x + 1) * CELL_SIZE,
                    (e.y + 1) * CELL_SIZE
                };
                FillRect(hdc, &enemyRect, enemyBrush);
            }
            DeleteObject(enemyBrush);
        }

        HBRUSH hBrush = CreateSolidBrush(RGB(0, 128, 0));
        RECT tankRect = {
            player.left * CELL_SIZE,
            player.top * CELL_SIZE,
            player.right * CELL_SIZE,
            player.bottom * CELL_SIZE
        };
        FillRect(hdc, &tankRect, hBrush);
        DeleteObject(hBrush);

        // Рисуем взрывы
        HBRUSH explosionBrush = CreateSolidBrush(RGB(255, 165, 0));
        for (auto& e : explosions) {
            RECT explosionRect = {
                e.x * CELL_SIZE,
                e.y * CELL_SIZE,
                (e.x + 1) * CELL_SIZE,
                (e.y + 1) * CELL_SIZE
            };
            FillRect(hdc, &explosionRect, explosionBrush);
        }
        DeleteObject(explosionBrush);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
