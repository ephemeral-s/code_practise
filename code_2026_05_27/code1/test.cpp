#include <iostream>

void func() {
    int* p1 = new int[10]{0};

    int t = 1;
    for(int i = 0; i < 10; i++) {
        p1[i] = t++;
        std::cout << p1[i] << std::endl;
    }

    delete[] p1;
}

int main() {
    func();
    return 0;
}

// void func() {
//     int* p1 = new int[10]{0};

//     int t = 1;
//     for(int i = 0; i < 10; i++) {
//         p1[i] = t++;
//         std::cout << p1[i] << std::endl;
//     }

//     if(p1[9] == 10) throw p1[9];

//     delete[] p1;
// }

// int main() {
//     try {
//         func();
//     } catch (const std::exception& e) {
//         std::cout << e.what() << std::endl;
//     }
//     return 0;
// }