#include "Server.hpp"
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    // Make sure that it has at least the simulation time and servers num, internet sending rate
    if (argc < 4) {
        std::cerr << "Parsing Error: Usage <T> <M> <P_1> ... <P_M> lambda <Q_1> ... <Q_M> <mu_1> ... <mu_M>";
        exit(-1);
    }
    std::size_t simulationTime = std::stoi(argv[1]);
    LOG(simulationTime);
    std::size_t serversNum = std::stoi(argv[2]);
    LOG(serversNum);

    int currIdx = 3;
    std::vector<double> probVec;
    probVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        probVec.push_back(std::stod(argv[i]));
    }

    LOG("Prob Vec");
    std::for_each(probVec.begin(), probVec.end(), [](auto&& prob){
            LOG(std::format("{},", std::forward<decltype(prob)>(prob)));
            });

    currIdx += serversNum;

    double internetSendingRate = std::stod(argv[currIdx++]);
    
    LOG(internetSendingRate);

    
    std::vector<double> maxQueueSizesVec;
    maxQueueSizesVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        maxQueueSizesVec.push_back(std::stod(argv[i]));
    }

    LOG("Queue Sizes Vec");
    std::for_each(maxQueueSizesVec.begin(), maxQueueSizesVec.end(), [](auto&& size){
            LOG(std::format("{},", std::forward<decltype(size)>(size)));
            });



    currIdx += serversNum;


    std::vector<double> servingRateVec;
    servingRateVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        servingRateVec.push_back(std::stod(argv[i]));
    }

    LOG("Serve Rate Vec");
    std::for_each(servingRateVec.begin(), servingRateVec.end(), [](auto&& size){
            LOG(std::format("{},", std::forward<decltype(size)>(size)));
            });

    return 0;
}
