#pragma once

#include "Server.hpp"
#include <cstddef>
#include <vector>

class LoadBalancer {
public:
    LoadBalancer(double internetSendingRate, const std::vector<double>& probVec,
                 const std::vector<std::size_t>& maxQueueVec, const std::vector<double>& servingRateVec);

    void simulate() const;

private:
    double m_internetSendingRate;
    std::vector<Server> m_servers;
};
