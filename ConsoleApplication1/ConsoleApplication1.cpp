#include <iostream>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <string>
#include <random>
#include <iomanip>
#include <limits>
#include <exception>
#include <cctype>
#include <Windows.h>

// Отключаем макросы min и max из Windows.h
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace std;
using namespace chrono;

// Класс для пользовательских исключений
class SortException : public exception 
{
private:
    string message;
public:
    SortException(const string& msg) : message(msg) {}
    const char* what() const noexcept override 
    {
        return message.c_str();
    }
};

// Структура для хранения информации о сортировке
struct SortInfo 
{
    string name;
    void (*func)(int*, int);
    string description;
};

// Прототипы функций сортировок
void bubbleSort(int* arr, int n);
void insertionSort(int* arr, int n);
void selectionSort(int* arr, int n);
void quickSort(int* arr, int left, int right);
void quickSortWrapper(int* arr, int n);
void mergeSort(int* arr, int left, int right);
void mergeSortWrapper(int* arr, int n);
void heapSort(int* arr, int n);
void countingSort(int* arr, int n);
void shellSort(int* arr, int n);

// Вспомогательные функции
int partition(int* arr, int left, int right);
void merge(int* arr, int left, int mid, int right);
void heapify(int* arr, int n, int i);
void printArray(int* arr, int n);
int* createArray(int size);
void generateRandomArray(int* arr, int size);
bool readArrayFromFile(int*& arr, int& size, const string& filename);
bool writeArrayToFile(int* arr, int n, const string& filename);
void copyArray(int* dest, int* src, int n);
bool isSorted(int* arr, int n);
int getValidIntInput(const string& prompt, int minVal = numeric_limits<int>::min(), int maxVal = numeric_limits<int>::max());
char getValidYesNoInput(const string& prompt);
string getValidStringInput(const string& prompt);
void clearInputStream();
void performSort(int* arr, int size, SortInfo sortInfo, bool showResult = true);

// Функция для установки кодировки консоли
void setConsoleEncoding() 
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);
    setlocale(LC_ALL, "Russian");
}

// Функция для очистки входного потока
void clearInputStream()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Функция для получения корректного целочисленного ввода
int getValidIntInput(const string& prompt, int minVal, int maxVal) 
{
    int value;
    bool valid = false;

    do {
        cout << prompt;
        cin >> value;

        if (cin.fail()) 
        {
            cout << "Ошибка: введите целое число!" << endl;
            clearInputStream();
            valid = false;
        }
        else if (value < minVal || value > maxVal) 
        {
            cout << "Ошибка: число должно быть от " << minVal << " до " << maxVal << endl;
            valid = false;
            clearInputStream();
        }
        else {
            valid = true;
        }
    } while (!valid);

    clearInputStream();
    return value;
}

// Функция для получения корректного ввода Yes/No с поддержкой русских букв
char getValidYesNoInput(const string& prompt) 
{
    string input;
    char result = 'n';
    bool valid = false;

    do 
    {
        cout << prompt;
        getline(cin, input);

        if (input.empty())
        {
            cout << "Ошибка: введите ответ!" << endl;
            continue;
        }

        // Приводим к нижнему регистру для сравнения
        for (char& c : input)
        {
            c = tolower(c);
        }

        // Проверяем все возможные варианты
        if (input == "y" || input == "yes" || input == "ye" || input == "1" ||
            input == "да" || input == "д" || input == "lf" || input == "l") 
        {
            result = 'y';
            valid = true;
        }
        else if (input == "n" || input == "no" || input == "0" ||
            input == "нет" || input == "н" || input == "ytn" ||
            input == "yt" || input == "nt" || input == "т") {
            result = 'n';
            valid = true;
        }
        else
        {
            cout << "Ошибка: введите Y/N, Д/Н, Yes/No, Да/Нет" << endl;
            valid = false;
        }

    } while (!valid);

    return result;
}

// Функция для получения корректного строкового ввода
string getValidStringInput(const string& prompt)
{
    string value;
    bool valid = false;

    do
    {
        cout << prompt;
        getline(cin, value);

        if (cin.fail())
        {
            cout << "Ошибка ввода!" << endl;
            clearInputStream();
            valid = false;
        }
        else if (value.empty()) 
        {
            cout << "Ошибка: строка не может быть пустой!" << endl;
            valid = false;
        }
        else 
        {
            valid = true;
        }
    } while (!valid);

    return value;
}

// Сортировка пузырьком
void bubbleSort(int* arr, int n) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    for (int i = 0; i < n - 1; i++) 
    {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// Сортировка вставками
void insertionSort(int* arr, int n) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    for (int i = 1; i < n; i++) 
    {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) 
        {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Сортировка выбором
void selectionSort(int* arr, int n)
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    for (int i = 0; i < n - 1; i++) 
    {
        int minIndex = i;
        for (int j = i + 1; j < n; j++) 
        {
            if (arr[j] < arr[minIndex]) 
            {
                minIndex = j;
            }
        }
        swap(arr[i], arr[minIndex]);
    }
}

// Вспомогательная функция для быстрой сортировки
int partition(int* arr, int left, int right) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    int pivot = arr[left + (right - left) / 2];
    int i = left - 1;
    int j = right + 1;

    while (true) 
    {
        do
        {
            i++;
        } while (arr[i] < pivot);

        do 
        {
            j--;
        } while (arr[j] > pivot);

        if (i >= j) return j;
        swap(arr[i], arr[j]);
    }
}

// Быстрая сортировка
void quickSort(int* arr, int left, int right) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    if (left < right)
    {
        int pi = partition(arr, left, right);
        quickSort(arr, left, pi);
        quickSort(arr, pi + 1, right);
    }
}

// Обертка для быстрой сортировки
void quickSortWrapper(int* arr, int n) 
{
    if (n > 0)
    {
        quickSort(arr, 0, n - 1);
    }
}

// Вспомогательная функция для сортировки слиянием
void merge(int* arr, int left, int mid, int right) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    int n1 = mid - left + 1;
    int n2 = right - mid;

    // Создаем временные массивы
    int* L = nullptr;
    int* R = nullptr;

    try 
    {
        L = new int[n1];
        R = new int[n2];
    }
    catch (const bad_alloc& e) 
    {
        throw SortException("Ошибка выделения памяти для временных массивов");
    }

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    while (i < n1 && j < n2) 
    {
        if (L[i] <= R[j]) 
        {
            arr[k] = L[i];
            i++;
        }
        else 
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) 
    {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }

    delete[] L;
    delete[] R;
}

// Реализация сортировки слиянием
void mergeSort(int* arr, int left, int right) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    if (left < right) 
    {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Обертка для сортировки слиянием
void mergeSortWrapper(int* arr, int n) 
{
    if (n > 0)
    {
        mergeSort(arr, 0, n - 1);
    }
}

// Вспомогательная функция для пирамидальной сортировки
void heapify(int* arr, int n, int i) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest])
        largest = left;

    if (right < n && arr[right] > arr[largest])
        largest = right;

    if (largest != i) 
    {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

// Реализация пирамидальной сортировки
void heapSort(int* arr, int n) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    // Построение кучи
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // Извлечение элементов из кучи
    for (int i = n - 1; i > 0; i--)
    {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

// Сортировка подсчетом
void countingSort(int* arr, int n) {
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");
    if (n <= 0) return;

    // Находим максимальный и минимальный элементы
    int maxVal = arr[0], minVal = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > maxVal) maxVal = arr[i];
        if (arr[i] < minVal) minVal = arr[i];
    }

    int range = maxVal - minVal + 1;

    // Проверяем, не слишком ли большой диапазон
    if (range > 1000000)
    {
        throw SortException("Диапазон значений слишком большой для сортировки подсчетом");
    }

    // Создаем счетчик и выходной массив
    int* count = nullptr;
    int* output = nullptr;

    try {
        count = new int[range]();
        output = new int[n];
    }
    catch (const bad_alloc& e) 
    {
        throw SortException("Ошибка выделения памяти для сортировки подсчетом");
    }

    // Подсчет элементов
    for (int i = 0; i < n; i++)
        count[arr[i] - minVal]++;

    // Накопление сумм
    for (int i = 1; i < range; i++)
        count[i] += count[i - 1];

    // Построение отсортированного массива
    for (int i = n - 1; i >= 0; i--) 
    {
        output[count[arr[i] - minVal] - 1] = arr[i];
        count[arr[i] - minVal]--;
    }

    // Копируем обратно
    for (int i = 0; i < n; i++)
        arr[i] = output[i];

    delete[] count;
    delete[] output;
}

// Сортировка Шелла
void shellSort(int* arr, int n) 
{
    if (arr == nullptr) throw SortException("Указатель на массив равен nullptr");

    for (int gap = n / 2; gap > 0; gap /= 2) 
    {
        for (int i = gap; i < n; i++) 
        {
            int temp = arr[i];
            int j;
            for (j = i; j >= gap && arr[j - gap] > temp; j -= gap) 
            {
                arr[j] = arr[j - gap];
            }
            arr[j] = temp;
        }
    }
}

// Функция для создания массива
int* createArray(int size) 
{
    if (size <= 0) 
    {
        throw SortException("Размер массива должен быть положительным");
    }

    int* arr = nullptr;
    try {
        arr = new int[size];
    }
    catch (const bad_alloc& e) 
    {
        throw SortException("Не удалось выделить память для массива");
    }

    return arr;
}

// Функция для освобождения памяти
void deleteArray(int* arr) 
{
    delete[] arr;
}

// Функция для копирования массива
void copyArray(int* dest, int* src, int n)
{
    if (dest == nullptr || src == nullptr) 
    {
        throw SortException("Указатели на массивы не могут быть nullptr");
    }

    for (int i = 0; i < n; i++) 
    {
        dest[i] = src[i];
    }
}

// Функция для проверки отсортированности массива
bool isSorted(int* arr, int n) 
{
    if (arr == nullptr) return false;

    for (int i = 1; i < n; i++) 
    {
        if (arr[i] < arr[i - 1]) return false;
    }
    return true;
}

// Функция для вывода массива
void printArray(int* arr, int n) 
{
    if (n <= 0 || arr == nullptr) 
    {
        cout << "Массив пуст" << endl;
        return;
    }

    cout << "[";
    int printCount = n < 20 ? n : 20; // Показываем максимум 20 элементов
    for (int i = 0; i < printCount; i++) 
    {
        cout << arr[i];
        if (i < printCount - 1) cout << ", ";
    }
    if (n > 20) cout << ", ...";
    cout << "]" << endl;
}

// Функция для генерации случайного массива
void generateRandomArray(int* arr, int size) 
{
    if (arr == nullptr) 
    {
        throw SortException("Указатель на массив равен nullptr");
    }

    try {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1, 1000);

        for (int i = 0; i < size; i++) 
        {
            arr[i] = dis(gen);
        }
    }
    catch (const exception& e) 
    {
        throw SortException("Ошибка при генерации случайных чисел");
    }
}

// Функция для чтения массива из файла
bool readArrayFromFile(int*& arr, int& size, const string& filename) 
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Ошибка: Не удалось открыть файл " << filename << endl;
        return false;
    }

    try 
    {
        // Сначала подсчитываем количество чисел
        int count = 0;
        int num;
        while (file >> num) 
        {
            count++;
        }

        if (count == 0) 
        {
            cout << "Файл пуст" << endl;
            file.close();
            return false;
        }

        // Освобождаем старый массив если есть
        if (arr != nullptr) 
        {
            deleteArray(arr);
            arr = nullptr;
        }

        // Создаем новый массив нужного размера
        size = count;
        arr = createArray(size);

        // Возвращаемся в начало файла и читаем числа
        file.clear();
        file.seekg(0);

        for (int i = 0; i < size; i++)
        {
            if (!(file >> arr[i]))
            {
                throw SortException("Ошибка чтения данных из файла");
            }
        }

        file.close();
        cout << "Массив успешно загружен из файла. Размер: " << size << endl;
        return true;

    }
    catch (const exception& e) 
    {
        cout << "Ошибка при чтении файла: " << e.what() << endl;
        file.close();
        return false;
    }
}

// Функция для записи массива в файл
bool writeArrayToFile(int* arr, int n, const string& filename) 
{
    if (arr == nullptr) 
    {
        cout << "Ошибка: массив не существует" << endl;
        return false;
    }

    ofstream file(filename);
    if (!file.is_open()) 
    {
        cout << "Ошибка: Не удалось создать файл " << filename << endl;
        return false;
    }

    try 
    {
        for (int i = 0; i < n; i++) 
        {
            file << arr[i];
            if (i < n - 1) file << " ";

            // Проверяем состояние потока
            if (!file.good())
            {
                throw SortException("Ошибка при записи в файл");
            }
        }

        file.close();
        cout << "Массив успешно сохранен в файл " << filename << endl;
        return true;

    }
    catch (const exception& e) 
    {
        cout << "Ошибка при записи в файл: " << e.what() << endl;
        file.close();
        return false;
    }
}

// Функция для выполнения сортировки с измерением времени
void performSort(int* arr, int size, SortInfo sortInfo, bool showResult) 
{
    if (size <= 0 || arr == nullptr) 
    {
        cout << "Массив пуст! Сначала создайте или загрузите массив." << endl;
        return;
    }

    cout << "\n" << string(50, '=') << endl;
    cout << "Выполняется: " << sortInfo.name << endl;
    cout << sortInfo.description << endl;
    cout << string(50, '-') << endl;

    cout << "Исходный массив: ";
    printArray(arr, size);

    // Создаем копию массива для сортировки
    int* arrCopy = nullptr;
    try 
    {
        arrCopy = createArray(size);
        copyArray(arrCopy, arr, size);
    }
    catch (const SortException& e)
    {
        cout << "Ошибка при подготовке к сортировке: " << e.what() << endl;
        return;
    }

    double timeMs = 0;
    bool success = false;

    try 
    {
        // Измеряем время сортировки
        auto start = high_resolution_clock::now();
        sortInfo.func(arrCopy, size);
        auto end = high_resolution_clock::now();
        timeMs = duration<double, milli>(end - start).count();
        success = true;
    }
    catch (const SortException& e) 
    {
        cout << "Ошибка сортировки: " << e.what() << endl;
    }
    catch (const exception& e) 
    {
        cout << "Неизвестная ошибка: " << e.what() << endl;
    }

    if (success) 
    {
        cout << "Отсортированный массив: ";
        printArray(arrCopy, size);

        // Проверяем, что массив действительно отсортирован
        if (isSorted(arrCopy, size)) 
        {
            cout << "Массив успешно отсортирован" << endl;
        }
        else 
        {
            cout << "ОШИБКА: Массив не отсортирован правильно!" << endl;
        }

        cout << "Время сортировки: " << fixed << setprecision(3) << timeMs << " мс" << endl;

        // Спрашиваем, хочет ли пользователь сохранить результат
        if (showResult)
        {
            char save = getValidYesNoInput("\nСохранить отсортированный массив в файл? (y/n/да/нет): ");
            if (save == 'y') {
                string filename = getValidStringInput("Введите имя файла для сохранения: ");
                writeArrayToFile(arrCopy, size, filename);
            }
            else
            {
                cout << "Сохранение отменено." << endl;
            }
        }
    }

    deleteArray(arrCopy);
}

int main() 
{
    // Устанавливаем кодировку консоли
    setConsoleEncoding();

    // Инициализация списка сортировок
    SortInfo sorts[] = 
    {
        {"Сортировка пузырьком", bubbleSort,
         "Простой алгоритм, сравнивает соседние элементы и меняет их местами. O(n^2)"},

        {"Сортировка вставками", insertionSort,
         "Строит отсортированный массив, вставляя элементы в правильную позицию. O(n^2)"},

        {"Сортировка выбором", selectionSort,
         "Находит минимальный элемент и ставит его в начало. O(n^2)"},

        {"Быстрая сортировка", quickSortWrapper,
         "Разделяет массив на части относительно опорного элемента. O(n log n)"},

        {"Сортировка слиянием", mergeSortWrapper,
         "Рекурсивно делит массив и сливает отсортированные части. O(n log n)"},

        {"Пирамидальная сортировка", heapSort,
         "Использует структуру данных 'куча'. O(n log n)"},

        {"Сортировка подсчетом", countingSort,
         "Подходит для целых чисел в ограниченном диапазоне. O(n + k)"},

        {"Сортировка Шелла", shellSort,
         "Улучшенная версия сортировки вставками. O(n log n)"}
    };
    int numSorts = sizeof(sorts) / sizeof(sorts[0]);

    int* arr = nullptr;
    int size = 0;
    bool running = true;

    while (running) 
    {
        try 
        {
            cout << "\n" << string(50, '=') << endl;
            cout << "ПРОГРАММА ДЛЯ ТЕСТИРОВАНИЯ АЛГОРИТМОВ СОРТИРОВКИ" << endl;
            cout << string(50, '=') << endl;

            cout << "\nТекущий массив: ";
            if (size <= 0 || arr == nullptr) 
            {
                cout << "не задан (пуст)" << endl;
            }
            else 
            {
                cout << "размер = " << size << ", первые 10 элементов: ";
                int printCount = size < 10 ? size : 10;
                for (int i = 0; i < printCount; i++) {
                    cout << arr[i] << " ";
                }
                if (size > 10) cout << "...";
                cout << endl;
            }

            cout << "\nМЕНЮ:" << endl;
            cout << "1. Создать новый массив" << endl;
            cout << "2. Загрузить массив из файла" << endl;
            cout << "3. Сохранить текущий массив в файл" << endl;
            cout << "4. Выбрать метод сортировки" << endl;
            cout << "5. Сравнить все методы сортировки" << endl;
            cout << "6. Показать текущий массив" << endl;
            cout << "0. Выход" << endl;

            int choice = getValidIntInput("\nВаш выбор: ", 0, 6);

            switch (choice) 
            {
            case 1: {
                int newSize = getValidIntInput("Введите размер массива: ", 1, 1000000);

                // Освобождаем старый массив
                if (arr != nullptr) 
                {
                    deleteArray(arr);
                    arr = nullptr;
                }

                // Создаем новый массив
                arr = createArray(newSize);
                size = newSize;

                cout << "Выберите способ заполнения:" << endl;
                cout << "1. Случайные числа" << endl;
                cout << "2. Ввести вручную" << endl;

                int fillChoice = getValidIntInput("Ваш выбор: ", 1, 2);

                if (fillChoice == 1) 
                {
                    generateRandomArray(arr, size);
                    cout << "Сгенерирован случайный массив из " << size << " элементов." << endl;
                }
                else if (fillChoice == 2)
                {
                    cout << "Введите " << size << " целых чисел: ";
                    for (int i = 0; i < size; i++) 
                    {
                        arr[i] = getValidIntInput("", numeric_limits<int>::min(), numeric_limits<int>::max());
                    }
                }
                break;
            }

            case 2: 
            {
                string filename = getValidStringInput("Введите имя файла для загрузки: ");
                readArrayFromFile(arr, size, filename);
                break;
            }

            case 3: 
            {
                if (size <= 0 || arr == nullptr) 
                {
                    cout << "Массив пуст! Нечего сохранять." << endl;
                    break;
                }
                string filename = getValidStringInput("Введите имя файла для сохранения: ");
                writeArrayToFile(arr, size, filename);
                break;
            }

            case 4: 
            {
                if (size <= 0 || arr == nullptr) 
                {
                    cout << "Массив пуст! Сначала создайте или загрузите массив." << endl;
                    break;
                }

                cout << "\nДоступные методы сортировки:" << endl;
                for (int i = 0; i < numSorts; i++) 
                {
                    cout << i + 1 << ". " << sorts[i].name << endl;
                }

                int sortChoice = getValidIntInput("Выберите метод (1-" + to_string(numSorts) + "): ", 1, numSorts);
                performSort(arr, size, sorts[sortChoice - 1], true);
                break;
            }

            case 5: 
            {
                if (size <= 0 || arr == nullptr)
                {
                    cout << "Массив пуст! Сначала создайте или загрузите массив." << endl;
                    break;
                }

                cout << "\n" << string(60, '=') << endl;
                cout << "СРАВНЕНИЕ ВСЕХ МЕТОДОВ СОРТИРОВКИ" << endl;
                cout << string(60, '=') << endl;

                cout << "Исходный массив: ";
                printArray(arr, size);
                cout << "\nСортировка " << size << " элементов...\n" << endl;

                // Массив для хранения результатов
                struct Result 
                {
                    string name;
                    double time;
                };
                Result results[8];
                int resultCount = 0;

                for (int i = 0; i < numSorts; i++) 
                {
                    // Создаем копию массива
                    int* arrCopy = nullptr;
                    try 
                    {
                        arrCopy = createArray(size);
                        copyArray(arrCopy, arr, size);
                    }
                    catch (const SortException& e)
                    {
                        cout << left << setw(25) << sorts[i].name
                            << ": ОШИБКА - " << e.what() << endl;
                        continue;
                    }

                    auto start = high_resolution_clock::now();

                    try {
                        sorts[i].func(arrCopy, size);

                        auto end = high_resolution_clock::now();
                        double timeMs = duration<double, milli>(end - start).count();

                        // Проверка, что массив действительно отсортирован
                        if (isSorted(arrCopy, size)) 
                        {
                            results[resultCount].name = sorts[i].name;
                            results[resultCount].time = timeMs;
                            resultCount++;
                            cout << left << setw(25) << sorts[i].name
                                << ": " << fixed << setprecision(3) << timeMs << " мс" << endl;
                        }
                        else 
                        {
                            cout << left << setw(25) << sorts[i].name
                                << ": ОШИБКА - массив не отсортирован!" << endl;
                        }
                    }
                    catch (const exception& e)
                    {
                        cout << left << setw(25) << sorts[i].name
                            << ": ОШИБКА - " << e.what() << endl;
                    }

                    deleteArray(arrCopy);
                }

                // Находим самую быструю сортировку
                if (resultCount > 0) 
                {
                    int fastestIndex = 0;
                    for (int i = 1; i < resultCount; i++) 
                    {
                        if (results[i].time < results[fastestIndex].time)
                        {
                            fastestIndex = i;
                        }
                    }

                    cout << "\n" << string(60, '-') << endl;
                    cout << "Самый быстрый метод: " << results[fastestIndex].name
                        << " (" << fixed << setprecision(3) << results[fastestIndex].time << " мс)" << endl;
                }
                break;
            }

            case 6: 
            {
                cout << "Текущий массив: ";
                printArray(arr, size);
                break;
            }

            case 0:
            {
                running = false;
                if (arr != nullptr) 
                {
                    deleteArray(arr);
                    arr = nullptr;
                }
                cout << "Программа завершена." << endl;
                break;
            }
            }
        }
        catch (const SortException& e)
        {
            cout << "\n!!! Ошибка: " << e.what() << endl;
            cout << "Нажмите Enter чтобы продолжить..." << endl;
            clearInputStream();
            cin.get();
        }
        catch (const exception& e) 
        {
            cout << "\n!!! Непредвиденная ошибка: " << e.what() << endl;
            cout << "Нажмите Enter чтобы продолжить..." << endl;
            clearInputStream();
            cin.get();
        }
    }

    return 0;
}