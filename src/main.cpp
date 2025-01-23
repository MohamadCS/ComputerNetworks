#include "LoadBalancer.hpp"
#include "Server.hpp"
#include "Utils.hpp"
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iostream>

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

    std::size_t serversNum = std::stoi(argv[2]);
    if (serversNum == 0) {
        std::cerr << "Invalid servers num" << '\n';
        exit(-1);
    }

    int currIdx = 3;
    std::vector<double> probVec;
    probVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        probVec.push_back(std::stod(argv[i]));
    }

    currIdx += serversNum;

    double internetSendingRate = std::stod(argv[currIdx++]);

    std::vector<std::size_t> maxQueueSizesVec;
    maxQueueSizesVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        maxQueueSizesVec.push_back(std::stoi(argv[i]));
    }

    currIdx += serversNum;

    std::vector<double> servingRateVec;
    servingRateVec.reserve(serversNum);

    for (int i = currIdx; i < currIdx + serversNum; ++i) {
        servingRateVec.push_back(std::stod(argv[i]));
    }

    double probSum = 0;
    for(auto prob : probVec) {
        probSum += prob;
    }

    if (probVec.size() != serversNum || maxQueueSizesVec.size() != serversNum ||
        maxQueueSizesVec.size() != serversNum || probSum != 1) {
        std::cerr << "Wrong parameters\n";
    }

    LoadBalancer loadBalancer{internetSendingRate, std::move(probVec), std::move(maxQueueSizesVec),
                              std::move(servingRateVec)};

    return {loadBalancer, simTime};
}

#else

int main() {
    Server server(1, 2, 1000);

    std::vector<int> vec;
    for(int i = 10000; i< 100000 ;i+=1000){
        vec.push_back(i);
    }

    for (int simTime : vec) {
        double averageWaitTime = 0;
        double serviceError = 0;
        double waitTimeError = 0;

        for (int i = 0; i < 20; ++i) {
            Server::SimResult result = server.simulate(simTime);
             averageWaitTime = result.waitTime / result.totalServedQueries;
            // std::cout << "Served " << result.totalServedQueries << " Average wait time " << averageWaitTime << '\n';

             serviceError += double(std::abs(simTime - double(result.waitTime)) * 100) / simTime;
             waitTimeError += std::abs(1 - averageWaitTime) * 100;

        }
        std::cout << "Service Error " << serviceError/20 << ", waitTime Error " << waitTimeError/20 << '\n';
    }

    return 0;
}

#endif
