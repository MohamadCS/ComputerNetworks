#include <iostream>
#include <random>

int main(int argc, char* argv[]) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<int> poissonDist(4);
    int arrivals = poissonDist(gen);
    std::cout << arrivals;
    return 0;
}
