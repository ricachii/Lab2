#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <numeric>
#include <cassert>

using namespace std;
using namespace std::chrono;

// ----------- ALGORITMOS DE ORDENAMIENTO -----------

void insertionSort(vector<int32_t>& arr) {
    // Ordenamiento in-place con complejidad O(n^2). Solo útil en arreglos pequeños o casi ordenados.
    for (int i = 1; i < arr.size(); i++) {
        int32_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key)
            arr[j+1] = arr[j--];
        arr[j+1] = key;
    }
}

void selectionSort(vector<int32_t>& arr) {
    // Selecciona iterativamente el mínimo y lo coloca en su lugar definitivo.
    for (int i = 0; i < arr.size() - 1; i++) {
        int minIdx = i;
        for (int j = i+1; j < arr.size(); j++)
            if (arr[j] < arr[minIdx]) minIdx = j;
        swap(arr[i], arr[minIdx]);
    }
}

void heapify(vector<int32_t>& arr, int n, int i) {
    // Mantiene la propiedad de heap maximizando desde el nodo i.
    int largest = i, l = 2*i + 1, r = 2*i + 2;
    if (l < n && arr[l] > arr[largest]) largest = l;
    if (r < n && arr[r] > arr[largest]) largest = r;
    if (largest != i) {
        swap(arr[i], arr[largest]);
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int32_t>& arr) {
    // Construye un heap y extrae el máximo repetidamente.
    for (int i = arr.size()/2 - 1; i >= 0; i--) heapify(arr, arr.size(), i);
    for (int i = arr.size() - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        heapify(arr, i, 0);
    }
}

int partition(vector<int32_t>& arr, int low, int high) {
    // Divide el arreglo respecto a un pivote.
    int pivot = arr[high], i = low - 1;
    for (int j = low; j < high; j++)
        if (arr[j] < pivot) swap(arr[++i], arr[j]);
    swap(arr[i+1], arr[high]);
    return i+1;
}

void quickSort(vector<int32_t>& arr, int low, int high) {
    // Ordenamiento eficiente promedio O(n log n) con recursividad.
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

void merge(vector<int32_t>& arr, int left, int mid, int right) {
    // Mezcla dos mitades ordenadas.
    int n1 = mid - left + 1, n2 = right - mid;
    vector<int32_t> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];
    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int32_t>& arr, int left, int right) {
    // Divide y mezcla hasta ordenar completamente.
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// ----------- FUNCIONES AUXILIARES -----------

bool readBinaryFile(const string& filename, vector<int32_t>& data) {
    // Carga los datos desde archivo binario a memoria.
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error abriendo archivo: " << filename << endl;
        return false;
    }
    data.clear();
    int32_t value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(int32_t)))
        data.push_back(value);
    file.close();
    return true;
}

bool isSorted(const vector<int32_t>& arr) {
    // Comprueba que el arreglo esté en orden no decreciente.
    for (size_t i = 1; i < arr.size(); i++)
        if (arr[i-1] > arr[i]) return false;
    return true;
}

template<typename Func>
double measureAverageTime(Func sortingFunc, vector<int32_t> data, int runs=3) {
    // Ejecuta múltiples corridas y promedia el tiempo de ejecución.
    vector<double> times;
    for (int i = 0; i < runs; i++) {
        vector<int32_t> copy = data;
        auto start = high_resolution_clock::now();
        sortingFunc(copy);
        auto stop = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(stop - start).count() / 1000.0;
        assert(isSorted(copy) && "Error: el arreglo no quedó ordenado.");
        times.push_back(duration);
    }
    return accumulate(times.begin(), times.end(), 0.0) / times.size();
}

// ----------- WRAPPERS -----------

void wrapperInsertionSort(vector<int32_t>& arr) { insertionSort(arr); }
void wrapperMergeSort(vector<int32_t>& arr)     { mergeSort(arr, 0, (int)arr.size() - 1); }
void wrapperHeapSort(vector<int32_t>& arr)      { heapSort(arr); }
void wrapperQuickSort(vector<int32_t>& arr)     { quickSort(arr, 0, (int)arr.size() - 1); }
void wrapperSelectionSort(vector<int32_t>& arr) { selectionSort(arr); }

// ----------- PROGRAMA PRINCIPAL -----------

int main() {
    string filename = "einstein.en.100MB.1b.bwt";

    cout << "Ruta actual de ejecución: " << filesystem::current_path() << endl;
    cout << "Intentando abrir: " << filename << endl;

    vector<int32_t> dataset;
    if (!readBinaryFile(filename, dataset)) {
        cerr << "Error leyendo archivo: " << filename << endl;
        return 1;
    }

    cout << "Número de elementos: " << dataset.size() << endl;
    cout << "Midiendo tiempos promedio (20 corridas):" << endl;

    // Ejecutar solo Insertion Sort si el dataset es pequeño
    if (dataset.size() <= 10000) {
        cout << "Insertion Sort:  " << measureAverageTime(wrapperInsertionSort, dataset)  << " ms" << endl;
    } else {
        cout << "Insertion Sort:  (omitido — dataset demasiado grande)" << endl;
    }

    cout << "Merge Sort:      " << measureAverageTime(wrapperMergeSort, dataset)      << " ms" << endl;
    cout << "Heap Sort:       " << measureAverageTime(wrapperHeapSort, dataset)       << " ms" << endl;
    cout << "Quick Sort:      " << measureAverageTime(wrapperQuickSort, dataset)      << " ms" << endl;
    cout << "Selection Sort:  " << measureAverageTime(wrapperSelectionSort, dataset)   << " ms" << endl;

    return 0;
}
