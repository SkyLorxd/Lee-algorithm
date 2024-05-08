#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 10
#define WHITE 7
#define RED 4
#define GREEN 2
#define YELLOW 6
#define PURPLE 5
#define BLUE 9

int start_y = 0;
int start_x = 0;
int finish_y = 9;
int finish_x = 9;

int gen_rnd_int(int x, int y) // Геннератор случайного числа от x до y включительно
{
    int t = x + rand() % (y + 1 - x);
    return t;
}
void init_rng(void) // Применение зерна основанного на текущем времени
{
    time_t curtime;
    time(&curtime);
    srand((unsigned int)curtime);
}

void view_map(double arr[SIZE][SIZE]){
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 0; i < SIZE; i++) // Вывод карты
    {
        printf("\n-------------------------------------------------------------\n|");
        for (int j = 0; j < SIZE; j++)
        {
            if (arr[i][j] == -SIZE * SIZE)
            {
                SetConsoleTextAttribute(hStdout, RED);
                printf("  X");
                SetConsoleTextAttribute(hStdout, WHITE);
                printf("  |");
            }
            else if ((i == finish_y && j == finish_x) || (i == start_y && j == start_x))
            {
                SetConsoleTextAttribute(hStdout, PURPLE);
                if (arr[i][j] < 10) printf(" %3.1f", arr[i][j]); else printf("%3.1f", arr[i][j]);
                SetConsoleTextAttribute(hStdout, WHITE);
                printf(" |");
            }
            else if (arr[i][j] == SIZE*SIZE+1)
            {
                SetConsoleTextAttribute(hStdout, YELLOW);
                printf("  ?");
                SetConsoleTextAttribute(hStdout, WHITE);
                printf("  |");
            }
            else if (arr[i][j] < 0)
            {
                SetConsoleTextAttribute(hStdout, BLUE);
                if (-arr[i][j] < 10) printf(" %3.1f", -arr[i][j]); else printf("%3.1f", -arr[i][j]);
                SetConsoleTextAttribute(hStdout, WHITE);
                printf(" |");
            }
            else
            {
                SetConsoleTextAttribute(hStdout, GREEN);
                if (arr[i][j] < 10) printf(" %3.1f", arr[i][j]); else printf("%3.1f", arr[i][j]);
                SetConsoleTextAttribute(hStdout, WHITE);
                printf(" |");
            }
        }
    }
    printf("\n-------------------------------------------------------------");
}

int main()
{
    init_rng();

    int walls = gen_rnd_int(0, SIZE * SIZE - 2); // Генерация случайного количества стенок от 0 до SIZE*SIZE-2
    double arr[SIZE][SIZE];                         // Создаём матрицу(карту)
    double arr_weights[SIZE][SIZE];
    for (int z = 0; z < walls; z++)              // Наполняем карту случайным количеством стенок(-SIZE*SIZE) в случайных местах
    {
        int i;
        int j;
    m1:
        i = gen_rnd_int(0, SIZE - 1);
        j = gen_rnd_int(0, SIZE - 1);
        if ((i == start_y && j == start_x) || (i == finish_y && j == finish_x))
            goto m1;
        arr[i][j] = -SIZE * SIZE;
        arr_weights[i][j] = -SIZE * SIZE;
    }
    for (int l = 0; l < SIZE; l++) // Наполняем карту неисследованными клетками(SIZE*SIZE+1)
    {
        for (int l1 = 0; l1 < SIZE; l1++)
        {
            if (arr[l][l1] != -SIZE * SIZE)
                arr[l][l1] = SIZE*SIZE+1;
                arr_weights[l][l1] = gen_rnd_int(1,10);
        }
    }
    arr[start_y][start_x] = 0;             // Задаём начение в стартовой клетке (0)
    arr[finish_y][finish_x] = SIZE*SIZE; // Задаём значение в конечной клетке (SIZE*SIZE)
    arr_weights[start_y][start_x] = 0;
    arr_weights[finish_y][finish_x] = gen_rnd_int(1,10);

    double curent_step = 0;
    while (arr[finish_y][finish_x] == SIZE*SIZE) // Волновой алгоритм поиска кратчайшего пути
    {
        for (int y = 0; y < SIZE; y++)
        {
            for (int x = 0; x < SIZE; x++)
            {
                if (arr[y][x] == curent_step)
                {
                    for (int yy = y - 1; yy < y + 2; yy++)
                    {
                        for (int xx = x - 1; xx < x + 2; xx++)
                        {
                            if ((xx >= 0) && (xx <= SIZE - 1) && (yy >= 0) && (yy <= SIZE - 1)) {
                                if ((x - xx != 0) && (y - yy != 0) && (curent_step + sqrt(2)*arr_weights[yy][xx] < arr[yy][xx])) arr[yy][xx] = curent_step + sqrt(2)*arr_weights[yy][xx];
                                else if (((x - xx == 0) || (y - yy == 0)) && (curent_step + arr_weights[yy][xx] < arr[yy][xx])) arr[yy][xx] = curent_step + arr_weights[yy][xx];
                            }
                            continue;
                        }
                    }
                }
            }
        }
        double min = SIZE * SIZE; //Увеличение шага до минимального > текущего
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if ((arr[i][j] > curent_step) && (arr[i][j] < min) && (arr[i][j] != SIZE*SIZE+1) && (arr[i][j] != SIZE*SIZE)) min = arr[i][j];
            }
        }
        curent_step = min;
        if (curent_step == SIZE * SIZE) //Проверка на доступность финиша
        {
            printf("\nImposible to reach finish :(\n");
            break;
        }
    }

    int step_i, step_j, min_i, min_j; // Поиск оптимального маршрута
    double step_counter = arr[finish_y][finish_x]; // Значение в клетке текущего шага
    double min;
    step_i = finish_y;                      // Координата по вертикали последней клетки оптимального пути
    step_j = finish_x;                      // Координата по горизонтали последней клетки оптимального пути
    if (step_counter == SIZE*SIZE) goto m3;

m2:
    min = SIZE * SIZE + 1;
    for (int yy = step_i - 1; yy < step_i + 2; yy++) {
        for (int xx = step_j - 1; xx < step_j + 2; xx++) {
            if ((xx >= 0) && (xx <= SIZE - 1) && (yy >= 0) && (yy <= SIZE - 1)) {
                if ((arr[yy][xx]>=0)&&(arr[yy][xx] < step_counter) && (arr[yy][xx] < min) && (arr[yy][xx] != SIZE*SIZE+1)) {
                    min = arr[yy][xx];
                    min_i = yy;
                    min_j = xx;
                }
            }
        }
    }
    arr[min_i][min_j] -= 2 * min; // Записываем в клетку число с со знаком -
    step_counter = min;
    step_i = min_i;
    step_j = min_j;
    if ((step_counter == 0) || (step_counter == SIZE * SIZE + 1)) goto m3;
    goto m2;



m3:
    view_map(arr_weights);
    view_map(arr);
    return 0;
}