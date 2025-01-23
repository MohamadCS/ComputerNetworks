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
        std::size_t totalServedQueries = 0;
        std::size_t totalDroppedQueries = 0;
        double lastQueryServeTime = 0;
        double waitTime = 0 ;
        double serveTime = 0;
    };

    struct Event {
        double time;
        EventType eventType;
    };

    Server(double arrivalRate, double serviceRate, std::size_t maxQueueSize);
    SimResult simulate(double simTime) const;

    double getArrivalRate() const{
        return m_arrivalRate;
    }

private:
    double m_arrivalRate;
    double m_serviceRate;
    std::size_t m_maxQueueSize;
};
