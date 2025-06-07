#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <cassert>

using namespace std;
using namespace std::chrono;

// ----------- ALGORITMOS -----------

void insertionSort(vector<int32_t>& arr) {
    int n = arr.size();
    for (int i = 1; i < n; i++) {
        int32_t key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = key;
    }
}

void merge(vector<int32_t>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int32_t> L(n1), R(n2);
    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int i = 0; i < n2; i++) R[i] = arr[mid + 1 + i];
    int i=0, j=0, k=left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<int32_t>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right-left)/2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid+1, right);
    merge(arr, left, mid, right);
}

void quickSort(vector<int32_t>& arr, int left, int right) {
    if (left >= right) return;
    int pivot = arr[right];
    int i = left - 1;
    for (int j = left; j < right; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i+1], arr[right]);
    int partitionIndex = i + 1;
    quickSort(arr, left, partitionIndex - 1);
    quickSort(arr, partitionIndex + 1, right);
}

void heapSort(vector<int32_t>& arr) {
    int n = arr.size();
    // Construir el heap (max-heap)
    for (int i = n / 2 - 1; i >= 0; i--) {
        int j = i;
        while (j < n) {
            int left = 2 * j + 1;
            int right = 2 * j + 2;
            int largest = j;
            if (left < n && arr[left] > arr[largest]) largest = left;
            if (right < n && arr[right] > arr[largest]) largest = right;
            if (largest == j) break;
            swap(arr[j], arr[largest]);
            j = largest;
        }
    }
    // Extraer elementos del heap
    for (int i = n - 1; i > 0; i--) {
        swap(arr[0], arr[i]);
        int j = 0;
        while (j < i) {
            int left = 2 * j + 1;
            int right = 2 * j + 2;
            int largest = j;
            if (left < i && arr[left] > arr[largest]) largest = left;
            if (right < i && arr[right] > arr[largest]) largest = right;
            if (largest == j) break;
            swap(arr[j], arr[largest]);
            j = largest;
        }
    }
}

void radixSort(vector<int32_t>& arr) {
    if (arr.empty()) return;
    int32_t maxVal = *max_element(arr.begin(), arr.end());
    for (int exp = 1; maxVal / exp > 0; exp *= 10) {
        vector<int32_t> output(arr.size());
        int count[10] = {0};
        for (int i = 0; i < arr.size(); i++) count[(arr[i] / exp) % 10]++;
        for (int i = 1; i < 10; i++) count[i] += count[i - 1];
        for (int i = arr.size() - 1; i >= 0; i--) {
            output[count[(arr[i] / exp) % 10] - 1] = arr[i];
            count[(arr[i] / exp) % 10]--;
        }
        arr = output;
    }
}

// ----------- FUNCIONES AUXILIARES -----------

bool readBinaryFile(const string& filename, vector<int32_t>& data) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error abriendo archivo: " << filename << endl;
        return false;
    }
    data.clear();
    int32_t value;
    while (file.read(reinterpret_cast<char*>(&value), sizeof(int32_t))) {
        data.push_back(value);
    }
    file.close();
    return true;
}

bool isSorted(const vector<int32_t>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i-1] > arr[i]) return false;
    }
    return true;
}

template<typename Func>
double measureAverageTime(Func sortingFunc, vector<int32_t> data, int runs=20) {
    vector<double> times;
    for (int i = 0; i < runs; i++) {
        vector<int32_t> copy = data; // Copia para cada corrida
        auto start = high_resolution_clock::now();
        sortingFunc(copy);
        auto stop = high_resolution_clock::now();
        double duration = duration_cast<microseconds>(stop - start).count()/1000.0; // ms
        assert(isSorted(copy) && "Error: arreglo no quedó ordenado");
        times.push_back(duration);
    }
    double sum = accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

// Wrappers para adaptar funciones con firma diferente
void wrapperInsertionSort(vector<int32_t>& arr) { insertionSort(arr); }
void wrapperMergeSort(vector<int32_t>& arr) { mergeSort(arr, 0, (int)arr.size()-1); }

// ----------- MAIN -----------

int main() {
    string filename = "einstein.en.100MB.1b.bwt";  // Aquí pones el nombre del archivo con extensión

    vector<int32_t> dataset;
    if (!readBinaryFile(filename, dataset)) {
        cerr << "Error leyendo archivo: " << filename << endl;
        return 1;
    }

    cout << "Número de elementos: " << dataset.size() << endl;
    cout << "Midiendo tiempos promedio (20 corridas):" << endl;

    double t_insertion = measureAverageTime(wrapperInsertionSort, dataset);
    cout << "Insertion Sort: " << t_insertion << " ms" << endl;

    double t_merge = measureAverageTime(wrapperMergeSort, dataset);
    cout << "Merge Sort: " << t_merge << " ms" << endl;

    return 0;
}
