#pragma once

#include <iostream>
#include <random>

class Server {
public:
    enum class EventType {
        ARRIVAL = 0,
        DEPARTURE
    };

    struct Event {
        double time;
        EventType eventType;
    };

    Server(double arrivalRate, double serviceRate, std::size_t msgNum);
    void simulate(std::size_t msgNum, double simTime);

private:
    std::default_random_engine generator;
    std::exponential_distribution<double> arrivalDist;
    std::poisson_distribution<int> poissonDist;
    double m_arrivalRate;
    double m_serviceRate;
    std::size_t m_msgNum;
};
