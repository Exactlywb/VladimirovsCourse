#include <iostream>

int main(int argc, char **argv) {

    bool *arr = new bool [10000] {};

    for (int i = 0; i < 1000000; ++i) {
        int rand = std::rand() % 10000;
        if (arr[rand] == 0) {
            arr[rand] = 1;
            printf("k %d ", rand + 1);
        }
    }

    for (int i = 0; i < 10000; ++i) {
        if (arr[i] == 0) {
            printf("k %d ", i + 1);
        }
    }

}