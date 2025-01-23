#include "LoadBalancer.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#ifdef Q4
std::pair<LoadBalancer, double> parseArguments(int argc, char* argv[]);

int main(int argc, char* argv[]) {
    auto [loadBalancer, simTime] = parseArguments(argc, argv);
    loadBalancer.simulate(simTime);
    return 0;
}

std::pair<LoadBalancer, double> parseArguments(int argc, char* argv[]) {
    // Make sure that it has at least the simulation time and servers num, internet sending rate
    if (argc < 4) {
        std::cerr << "Parsing Error: Usage <T> <M> <P_1> ... <P_M> lambda <Q_1> ... <Q_M> <mu_1> ... <mu_M>";
        exit(-1);
    }

    std::size_t simTime = std::stoi(argv[1]);
    if (simTime == 0) {
        std::cerr << "Invalid simulation time" << '\n';
        exit(-1);
    }
    LOG(std::format("Sim time is {}", simTime));

    std::size_t serversNum = std::stoi(argv[2]);
    if (serversNum == 0) {
        std::cerr << "Invalid servers num" << '\n';
    }
    LOG(std::format("Servers Num is {}", serversNum));

    int currIdx = 3;
    std::vector<double> probVec;
    probVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        probVec.push_back(std::stod(argv[i]));
    }

    LOG("Prob Vec");
    std::for_each(probVec.begin(), probVec.end(),
                  [](auto&& prob) { std::cout << (std::format("{},", std::forward<decltype(prob)>(prob))); });

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

    return {loadBalancer, simTime};
}

#else

int main() {
    Server server(1, 2, 1000);
    for (int i = 0; i < 5; ++i) {
        Server::SimResult result = server.simulate(5);
        std::cout << std::format("Served {}, Average wait time {}\n", result.totalServedQueries,
                                 result.averageWaitTime);
    }

    return 0;
}

#endif
