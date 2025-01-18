#pragma once

#include "Utils.hpp"
#include <cstddef>
#include <random>

class Server {
public:
    enum class EventType {
        ARRIVAL = 0,
        DEPARTURE
    };

    struct SimResult {
        std::size_t totalServedQueries;
        std::size_t totalDroppedQueries;
        double lastQueryServeTime;
        double averageWaitTime;
        double averageServeTime;
    };

    struct Event {
        double time;
        EventType eventType;
    };

    Server(double arrivalRate, double serviceRate, std::size_t maxQueueSize);
    SimResult simulate(double simTime) const;

private:
    double m_arrivalRate;
    double m_serviceRate;
    std::size_t m_maxQueueSize;
};
