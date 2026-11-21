#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <windows.h>
#include <locale>

using namespace std;

void clear_input() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

double read_double(const string& prompt) {
    double x;
    while (true) {
        cout << prompt;
        if (cin >> x) return x;
        cout << "Невірне введення. Спробуйте ще раз.\n";
        clear_input();
    }
}

int read_int(const string& prompt, int minv = INT_MIN, int maxv = INT_MAX) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= minv && x <= maxv) return x;
        cout << "Невірне введення або поза допустимим діапазоном. Спробуйте ще раз.\n";
        clear_input();
    }
}


class GridTask {
private:
    int rows, cols;
    double* data;

public:
    GridTask() : rows(0), cols(0), data(nullptr) {}
    ~GridTask() { clear(); }

    void clear() {
        if (data) delete[] data;
        data = nullptr;
        rows = cols = 0;
    }

    void create(int r, int c) {
        if (data) delete[] data;
        rows = r; cols = c;
        data = new double[rows * cols];
        for (int i = 0; i < rows * cols; ++i) data[i] = 0.0;
    }

    void setCell(int r, int c, double val) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return;
        data[r * cols + c] = val;
    }

    double getCell(int r, int c) const {
        if (r < 0 || r >= rows || c < 0 || c >= cols) return 0;
        return data[r * cols + c];
    }

    void inputField() {
        int r = read_int("Введіть кількість рядків ( >0 ): ", 1);
        int c = read_int("Введіть кількість стовпців ( >0 ): ", 1);
        create(r, c);
        cout << "Введіть вартість за кожну позицію (построково):\n";
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                double v = read_double("  cell[" + to_string(i) + "][" + to_string(j) + "] = ");
                setCell(i, j, v);
            }
        }
    }

    void printField() const {
        if (!data) {
            cout << "Поле не задано.\n";
            return;
        }
        cout << "Поле (" << rows << "x" << cols << "):\n";
        cout << fixed << setprecision(2);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                cout << setw(8) << getCell(i, j);
            }
            cout << "\n";
        }
    }

    double solvePath(int r1, int c1, int r2, int c2) {
        if (!data) {
            cout << "Поле не задано.\n";
            return -1;
        }
        if (r1 < 0 || r1 >= rows || r2 < 0 || r2 >= rows || c1 < 0 || c1 >= cols || c2 < 0 || c2 >= cols) {
            cout << "Координати поза полем.\n";
            return -1;
        }
        if (r2 < r1 || c2 < c1) {
            cout << "Друга позиція повинна бути правіше та нижче (або на тій самій позиції).\n";
            return -1;
        }
        int R = r2 - r1 + 1;
        int C = c2 - c1 + 1;
        double* dp = new double[R * C];
        for (int i = 0; i < R * C; ++i) dp[i] = 1e300;

        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) {
                int gi = r1 + i;
                int gj = c1 + j;
                double cost = getCell(gi, gj);
                if (i == 0 && j == 0) {
                    dp[i * C + j] = cost;
                }
                else {
                    double best = 1e300;
                    if (i > 0) best = min(best, dp[(i - 1) * C + j]);
                    if (j > 0) best = min(best, dp[i * C + (j - 1)]);
                    if (i > 0 && j > 0) best = min(best, dp[(i - 1) * C + (j - 1)]);
                    dp[i * C + j] = cost + best;
                }
            }
        }
        double ans = dp[(R - 1) * C + (C - 1)];
        cout << "Матриця затрат від (" << r1 << "," << c1 << ") до (" << r2 << "," << c2 << "):\n";
        cout << fixed << setprecision(2);
        for (int i = 0; i < R; ++i) {
            for (int j = 0; j < C; ++j) cout << setw(10) << dp[i * C + j];
            cout << "\n";
        }
        delete[] dp;
        return ans;
    }
};

struct Sector {
    double p0;
    int m;
    double delta;
    Sector() : p0(0.0), m(0), delta(0.0) {}
};

class SearchTask {
private:
    Sector* sectors;
    int n;
public:
    SearchTask() : sectors(nullptr), n(0) {}
    ~SearchTask() { clear(); }

    void clear() {
        if (sectors) delete[] sectors;
        sectors = nullptr;
        n = 0;
    }

    void create(int N) {
        if (sectors) delete[] sectors;
        n = N;
        sectors = new Sector[n];
    }

    void inputSectors() {
        int N = read_int("Введіть кількість секторів (N > 0): ", 1);
        create(N);
        for (int i = 0; i < n; ++i) {
            cout << "Сектор " << i << ":\n";
            double p0 = read_double("  Базова ймовірність p0 (0..1): ");
            while (p0 < 0.0 || p0 > 1.0) {
                cout << "  p0 має бути в [0,1].\n";
                p0 = read_double("  Базова ймовірність p0 (0..1): ");
            }
            int m = read_int("  Максимум додаткових команд m (>=0): ", 0);
            double delta = read_double("  Приріст ймовірності за 1 команду (delta, >=0): ");
            while (delta < 0.0) {
                cout << "  delta має бути >= 0.\n";
                delta = read_double("  Приріст ймовірності за 1 команду (delta, >=0): ");
            }
            sectors[i].p0 = p0;
            sectors[i].m = m;
            sectors[i].delta = delta;
        }
    }

    void printSectors() const {
        if (!sectors) {
            cout << "Сектори не задані.\n";
            return;
        }
        cout << "Сектори:\n";
        cout << fixed << setprecision(4);
        for (int i = 0; i < n; ++i) {
            cout << "  " << i << ": p0=" << sectors[i].p0 << "  m=" << sectors[i].m << "  delta=" << sectors[i].delta << "\n";
        }
    }

    void solveAllocation(int T) const {
        if (!sectors) {
            cout << "Дані секторів відсутні.\n";
            return;
        }
        if (T < 0) {
            cout << "Невірна кількість команд.\n";
            return;
        }
        int maxT = T;
        const double INF = 1e300;
        double* dp_prev = new double[maxT + 1];
        double* dp_curr = new double[maxT + 1];
        int** parent = new int* [n];
        for (int i = 0; i < n; ++i) {
            parent[i] = new int[maxT + 1];
            for (int t = 0; t <= maxT; ++t) parent[i][t] = -1;
        }

        for (int t = 0; t <= maxT; ++t) dp_prev[t] = INF;
        dp_prev[0] = 1.0;

        for (int i = 0; i < n; ++i) {
            for (int t = 0; t <= maxT; ++t) dp_curr[t] = INF;
            for (int s = 0; s <= sectors[i].m; ++s) {
                double p = sectors[i].p0 + s * sectors[i].delta;
                if (p > 1.0) p = 1.0;
                double factor = 1.0 - p;
                for (int used = 0; used <= maxT; ++used) {
                    if (dp_prev[used] >= INF) continue;
                    int newt = used + s;
                    if (newt > maxT) break;
                    double candidate = dp_prev[used] * factor;
                    if (candidate < dp_curr[newt]) {
                        dp_curr[newt] = candidate;
                        parent[i][newt] = s;
                    }
                }
            }
            for (int t = 0; t <= maxT; ++t) dp_prev[t] = dp_curr[t];
        }

        double bestProd = INF;
        int bestT = 0;
        for (int t = 0; t <= maxT; ++t) {
            if (dp_prev[t] < bestProd) {
                bestProd = dp_prev[t];
                bestT = t;
            }
        }
        double bestProbability = 1.0 - bestProd;
        cout << "Найкраща ймовірність знайти людину: " << fixed << setprecision(6) << bestProbability
            << " (використано команд: " << bestT << " з " << T << ")\n";

        int remainingT = bestT;
        int* alloc = new int[n];
        for (int i = n - 1; i >= 0; --i) {
            int s = parent[i][remainingT];
            if (s < 0) s = 0;
            alloc[i] = s;
            remainingT -= s;
            if (remainingT < 0) remainingT = 0;
        }

        cout << "Розподіл команд по секторах:\n";
        for (int i = 0; i < n; ++i) {
            double p = sectors[i].p0 + alloc[i] * sectors[i].delta;
            if (p > 1.0) p = 1.0;
            cout << "  Сектор " << i << ": команд = " << alloc[i] << " -> p=" << fixed << setprecision(6) << p << "\n";
        }

        delete[] dp_prev;
        delete[] dp_curr;
        delete[] alloc;
        for (int i = 0; i < n; ++i) delete[] parent[i];
        delete[] parent;
    }
};

int main() {
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, "ukr");

    GridTask grid;
    SearchTask search;
    bool running = true;
    while (running) {
        cout << "\n=== Меню ===\n";
        cout << "1) Робота з ігровим полем (Задача 1)\n";
        cout << "2) Робота з пошуком людини (Задача 2)\n";
        cout << "0) Вихід\n";
        int choice = read_int("Виберіть опцію: ", 0, 2);
        if (choice == 0) { running = false; break; }

        if (choice == 1) {
            bool sub = true;
            while (sub) {
                cout << "\n-- Задача 1: Меню --\n";
                cout << "1) Ввести поле\n";
                cout << "2) Вивести поле\n";
                cout << "3) Змінити значення клітинки\n";
                cout << "4) Розв'язати: знайти мінімальний шлях між двома позиціями\n";
                cout << "0) Повернутись в головне меню\n";
                int c = read_int("Ваш вибір: ", 0, 4);
                if (c == 0) { sub = false; break; }
                if (c == 1) { grid.inputField(); }
                else if (c == 2) { grid.printField(); }
                else if (c == 3) {
                    int r = read_int("Введіть індекс рядка: ", 0);
                    int col = read_int("Введіть індекс стовпця: ", 0);
                    double v = read_double("Введіть нове значення: ");
                    grid.setCell(r, col, v);
                    cout << "Значення змінено.\n";
                }
                else if (c == 4) {
                    int r1 = read_int("Введіть r1 (початковий рядок): ", 0);
                    int c1 = read_int("Введіть c1 (початковий стовпець): ", 0);
                    int r2 = read_int("Введіть r2 (кінцевий рядок, повинен бути >= r1): ", 0);
                    int c2 = read_int("Введіть c2 (кінцевий стовпець, повинен бути >= c1): ", 0);
                    double ans = grid.solvePath(r1, c1, r2, c2);
                    if (ans >= 0) cout << "Мінімальні витрати: " << fixed << setprecision(6) << ans << "\n";
                }
            }
        }
        else if (choice == 2) {
            bool sub = true;
            while (sub) {
                cout << "\n-- Задача 2: Меню --\n";
                cout << "1) Ввести дані секторів\n";
                cout << "2) Вивести дані секторів\n";
                cout << "3) Розв'язати: оптимальний розподіл команд\n";
                cout << "0) Повернутись в головне меню\n";
                int c = read_int("Ваш вибір: ", 0, 3);
                if (c == 0) { sub = false; break; }
                if (c == 1) { search.inputSectors(); }
                else if (c == 2) { search.printSectors(); }
                else if (c == 3) {
                    int T = read_int("Введіть загальну кількість доступних команд (T): ", 0);
                    search.solveAllocation(T);
                }
            }
        }
    }
    return 0;
}
