// Author: Adam Cobian
// CECS 325-01
// Prog 4
// Due: 11/2/26
//
//I certify that this program is my own original work. I did not copy any part of
//this program from any other source including inappropriate AI use. I did not share 
//this program with anyone. I further certify that I typed each and every line of code 
//in this program.

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex>
using namespace std;

long long totalSwaps = 0;
mutex mtx;  

void bubble(int arr[], int start, int size, int procNum)
{
    long long swaps = 0;
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - 1 - i; j++)
        {
            if (arr[start + j] > arr[start + j + 1])
            {
                int temp = arr[start + j];
                arr[start + j] = arr[start + j + 1];
                arr[start + j + 1] = temp;
                swaps = swaps + 1;
            }
        }
    }
    mtx.lock();
    cout << "Process " << procNum << " swap count:" << swaps << endl;
    totalSwaps = totalSwaps + swaps;
    mtx.unlock();
}

void merge(int arr[], int start, int mid, int end)
{
    int size = end - start;
    int *temp = new int[size];
    int i = start;
    int j = mid;
    int k = 0;

    while (i < mid && j < end)
    {
        if (arr[i] <= arr[j])
        {
            temp[k] = arr[i];
            i = i + 1;
        }
        else
        {
            temp[k] = arr[j];
            j = j + 1;
        }

        k = k + 1;
    }

    while (i < mid)
    {
        temp[k] = arr[i];
        i = i + 1;
        k = k + 1;
    }

    while (j < end)
    {
        temp[k] = arr[j];
        j = j + 1;
        k = k + 1;
    }

    for (int x = 0; x < size; x++)
    {
        arr[start + x] = temp[x];
    }
    delete[] temp;
}

int main(int argc, char *argv[])
{
    if (argc != 3 && argc != 4)
    {
        cout << "Error - wrong number of arguments" << endl;
        cout << "Usage: sort inputfile outputfile [-test]" << endl;
        return 1;
    }

    int count = 1000000;
    if (argc == 4)
    {
        string flag = argv[3];

        if (flag == "-test")
        {
            count = 10000;
        }
    }

    ifstream fin;
    fin.open(argv[1]);
    if (!fin)
    {
        cout << "Error - could not open " << argv[1] << endl;
        return 1;
    }

    int *numbers = new int[count];
    for (int i = 0; i < count; i++)
    {
        fin >> numbers[i];
    }
    fin.close(); 
    
    int sectionSize = count / 16;
    thread t[16];

    for (int i = 0; i < 16; i++)
    {
        t[i] = thread(bubble, numbers, i * sectionSize, sectionSize, i);
    }

    for (int i = 0; i < 16; i++)
    {
        t[i].join();
    }
    cout << "Total Swaps:" << totalSwaps << endl;

    int width = sectionSize;
    for (int round = 1; round <= 4; round++)
    {
        for (int start = 0; start < count; start = start + 2 * width)
        {
            merge(numbers, start, start + width, start + 2 * width);
        }
        width = width * 2;
    }

    ofstream fout;
    fout.open(argv[2]);
    if (!fout)
    {
        cout << "Error - could not open " << argv[2] << endl;
        delete[] numbers;
        return 1;
    }

    for (int i = 0; i < count; i++)
    {
        fout << numbers[i] << endl;
    }
    fout.close();
    delete[] numbers;

    return 0;
}
