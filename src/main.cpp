#include "LoadBalancer.hpp"
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>



std::pair<LoadBalancer,double> parseArguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    auto [loadBalancer,simTime] = parseArguments(argc, argv);
    loadBalancer.simulate(simTime);
    return 0;
}

std::pair<LoadBalancer,double> parseArguments(int argc, char* argv[]) {
    // Make sure that it has at least the simulation time and servers num, internet sending rate
    if (argc < 4) {
        std::cerr << "Parsing Error: Usage <T> <M> <P_1> ... <P_M> lambda <Q_1> ... <Q_M> <mu_1> ... <mu_M>";
        exit(-1);
    }
    std::size_t simTime = std::stoi(argv[1]);
    LOG(simTime);
    std::size_t serversNum = std::stoi(argv[2]);
    LOG(serversNum);

    int currIdx = 3;
    std::vector<double> probVec;
    probVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        probVec.push_back(std::stod(argv[i]));
    }

    LOG("Prob Vec");
    std::for_each(probVec.begin(), probVec.end(),
                  [](auto&& prob) { LOG(std::format("{},", std::forward<decltype(prob)>(prob))); });

    currIdx += serversNum;

    double internetSendingRate = std::stod(argv[currIdx++]);

    LOG(internetSendingRate);

    std::vector<std::size_t> maxQueueSizesVec;
    maxQueueSizesVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        maxQueueSizesVec.push_back(std::stoi(argv[i]));
    }

    LOG("Queue Sizes Vec");
    std::for_each(maxQueueSizesVec.begin(), maxQueueSizesVec.end(),
                  [](auto&& size) { LOG(std::format("{},", std::forward<decltype(size)>(size))); });

    currIdx += serversNum;

    std::vector<double> servingRateVec;
    servingRateVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        servingRateVec.push_back(std::stod(argv[i]));
    }

    LOG("Serve Rate Vec");
    std::for_each(servingRateVec.begin(), servingRateVec.end(),
                  [](auto&& size) { LOG(std::format("{},", std::forward<decltype(size)>(size))); });

    LoadBalancer loadBalancer{internetSendingRate, std::move(probVec), std::move(maxQueueSizesVec),
                              std::move(servingRateVec)};

    return {loadBalancer,simTime};
}
