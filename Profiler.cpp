#include <iostream>
#include <set>
#include <ctime>
#include <fstream>
#include <chrono>
#include <random>

#include "Tree.cpp"
#include "RBTree.h"

// memory leaks
#define __CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW


class Timer
{
private:
    std::chrono::steady_clock::time_point start;

public:
    Timer() : start(std::chrono::steady_clock::now()) { }

    void reset()
    {
        start = std::chrono::steady_clock::now();
    }

    double elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1> >>(std::chrono::steady_clock::now() - start).count();
    }

    ~Timer() {

    }
};


int main() {
    // detecting memory leaks
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    // start generating of random unsigned int (4 bites)
    std::random_device rd;
    std::mt19937 mersenne(rd());

    // choose testing variant of set
    using My_set = std::set<int>;

    bool methods_correctness(1);

    // check insert + size correctness
    std::cout << "Enter the number of inserted elements in tests: ";
    int n_0;
    std::cin >> n_0;
    const int N_0(n_0);

    {
        std::set<int> std_set;
        My_set our_set;

        int temp;
        for (int i = 0; i < N_0; i += 1) {
            temp = mersenne();
            our_set.insert(temp);
            std_set.insert(temp);
            if (our_set.size() != std_set.size()) {
                methods_correctness = 0;
                std::cout << "Errors were found in one of the following methods: insert, size " << std::endl;
            }
        }
    }

    // check insert + find correctness
    {
        My_set our_set;
        int temp;
        for (int i = 0; i < N_0; i += 1) {
            temp = mersenne();
            our_set.insert(temp);
            if (our_set.find(temp) != our_set.end()) {
                if (*(our_set.find(temp)) != temp) {
                    methods_correctness = 0;
                    std::cout << "Errors were found in one of the following methods: insert, find " << std::endl;
                }
            }
            else {
                methods_correctness = 0;
                std::cout << "Errors were found in one of the following methods: insert, find " << std::endl;
            }
        }
    }


    // check erase + find correctness
    std::cout << "Enter the number of removed elements: ";
    int n_1;
    std::cin >> n_1;
    const int N_1(n_1);

    {
        My_set our_set;
        int temp_insert;
        int temp_erase;
        for (int i = 0; i < N_0; i += 1) {
            temp_insert = mersenne();
            our_set.insert(temp_insert);
        }
        for (int i = 0; i < N_1; i += 1) {
            temp_erase = mersenne();
            our_set.erase(temp_erase);
            if (our_set.find(temp_erase) != our_set.end()) {
                methods_correctness = 0;
                std::cout << "Errors were found in one of the following methods: erase, find " << std::endl;
            }
        }
    }

    {
        if (methods_correctness) {
            std::cout << "Methods seem to work correctly" << std::endl;
        }
        else {
            std::cout << "Methods are incorrect" << std::endl;
        }
    }

    
    // asymptotics

    std::ofstream f_insert;
    std::ofstream f_find;
    std::ofstream f_erase;
    f_insert.open("D:\\Study\\Tree\\Asymp_Insert.txt");
    f_find.open("D:\\Study\\Tree\\Asymp_Find.txt");
    f_erase.open("D:\\Study\\Tree\\Asymp_Erase.txt");


    if (f_insert.is_open() && f_find.is_open() && f_erase.is_open()) {
        My_set our_set;
        int N_2;
        std::cout << "Enter the number of steps in asymptotic test: ";
        std::cin >> N_2;
        int elements_per_step;
        std::cout << "enter tne number of elements per step: ";
        std::cin >> elements_per_step;

        double prev, pres;
        {
            for (int i = 0; i < N_2; i += 1) {
                {
                    Timer t;
                    for (int j = 0; j < elements_per_step; j += 1) {
                        our_set.insert(i * elements_per_step + j);
                        //rand variant
                        //our_set.insert(mersenne());
                    }
                    pres = t.elapsed();
                }
                f_insert << pres << ' ' << i * elements_per_step + elements_per_step / 2 << '\n';

                {
                    Timer t;
                    for (int j = 0; j < elements_per_step; j += 1) {
                        our_set.find(mersenne());
                    }
                    f_find << t.elapsed() << ' ' << i * elements_per_step + elements_per_step / 2 << '\n';
                }
            }

            /*
            for (int i = 0; i < N_2; i += 1) {
                if (!(i % 10000)) {
                    if (i) {
                        f_insert << i << ' ' << t.elapsed() << '\n';
                    }


                    // find asymp
                    t.reset();
                    for (int j = 0; j < 10000; j += 1) {
                        our_set.find(rand());
                    }
                    f_find << i << ' ' << t.elapsed() << '\n';


                    t.reset();
                }
                our_set.insert(i);
            }
            */
        }

        // erase asymp
        {
            Timer t;
            for (int i = 0; i < N_2; i += 1) {
                t.reset();
                for (int j = 0; j < elements_per_step; j += 1) {
                    our_set.erase(i * elements_per_step + j);
                }
                f_erase << t.elapsed() << ' ' << our_set.size() + elements_per_step / 2 << '\n';
            }
        }
    }
    else {
        std::cout << "files are not opened";
    }




    // deliberately created memory leak
    int* ptr = new int;
    ptr = new int;

    // another method to detect memory leaks
    /*
    _CrtDumpMemoryLeaks();
    */

    return 0;
}