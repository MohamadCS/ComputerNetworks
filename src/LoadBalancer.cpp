#include "LoadBalancer.hpp"
#include "Server.hpp"
#include <cstddef>
#include <iostream>
#include <format>

LoadBalancer::LoadBalancer(double internetSendingRate, const std::vector<double>& probVec,
                           const std::vector<std::size_t>& maxQueueVec, const std::vector<double>& servingRateVec)
    : m_internetSendingRate(internetSendingRate),
      m_servers() {
    std::size_t serversNum = probVec.size();
    m_servers.reserve(serversNum);

    // Create servers
    for (int i = 0; i < probVec.size(); ++i) {
        double arrivalRate = probVec.at(i) * internetSendingRate;
        Server server(arrivalRate, servingRateVec.at(i), maxQueueVec.at(i));
        LOG(std::format("Created server with {} {} {}", arrivalRate, servingRateVec.at(i),maxQueueVec.at(i)));
        m_servers.emplace_back(std::move(server));
    }
}

void LoadBalancer::simulate(double simTime) const {
    double  averageWaitTime = 0, averageServeTime = 0, lastQueryServerTime = 0;
    std::size_t totalServedQueries = 0, totalDroppedQueries = 0;

    std::size_t serversNum = m_servers.size();

    for (const auto& server : m_servers) {
        Server::SimResult result = server.simulate(simTime);
        totalDroppedQueries += result.totalDroppedQueries;
        totalServedQueries += result.totalServedQueries;
        averageWaitTime += result.averageWaitTime/ serversNum;
        averageServeTime += result.averageServeTime/ serversNum;
        lastQueryServerTime = std::max(lastQueryServerTime, result.lastQueryServeTime);
    }

    std::cout << std::format("{} {} {} {} {}",totalServedQueries,totalDroppedQueries,lastQueryServerTime,averageWaitTime,averageServeTime);
}
