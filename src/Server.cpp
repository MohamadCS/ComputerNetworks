#include "Server.hpp"
#include <functional>
#include <queue>
#include <vector>
#include <random>

Server::Server(double arrivalRate, double serviceRate, std::size_t msgNum)
    : m_arrivalRate(arrivalRate),
      m_serviceRate(serviceRate),
      m_msgNum(msgNum) {
}

static double getPoisonNumber(double rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::poisson_distribution<int> dist(rate);
    double arrivals = dist(gen);
    return arrivals;
}

static double getExpNumber(double rate) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::exponential_distribution<double> dist(rate);
    double arrivals = dist(gen);
    return arrivals;
}

struct EventGreater : public std::greater<Server::Event> {
    bool operator()(const Server::Event& lhs, const Server::Event& rhs) const {
        return lhs.time <= rhs.time;
    }
};

void Server::simulate(std::size_t msgNum, double simTime) {
    double currentTime = 0;
    std::queue<double> arrivalTimeQueue{};
    std::priority_queue<Server::Event, std::vector<Server::Event>, EventGreater> eventHeap{};
    bool isServerBusy = false;

    std::size_t totalWaitTime = 0, servedNum = 0;

    while (currentTime < simTime) {
        auto& [eventTime, eventType] = eventHeap.top();
        eventHeap.pop();
        currentTime = eventTime;


        if (eventType == Server::EventType::ARRIVAL) {
            if (!isServerBusy) {
                isServerBusy = true;
                double serviceTime = getExpNumber(m_serviceRate);
                eventHeap.push({currentTime + serviceTime, Server::EventType::DEPARTURE});
            } else {
                arrivalTimeQueue.push(currentTime);
            }

            double nextArrival = currentTime + getExpNumber(m_arrivalRate);

            if (arrivalTimeQueue.size() < msgNum) {
                eventHeap.push({nextArrival, Server::EventType::ARRIVAL});
            }

        } else if (eventType == Server::EventType::DEPARTURE) {
            servedNum++;
            if (!arrivalTimeQueue.empty()) {
                double arrivalTime = arrivalTimeQueue.front();
                arrivalTimeQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;
                double serviceTime = getExpNumber(m_serviceRate);
                eventHeap.push({currentTime + serviceTime, Server::EventType::DEPARTURE});
            } else {
                isServerBusy = false;
            }
        }
    }
}
