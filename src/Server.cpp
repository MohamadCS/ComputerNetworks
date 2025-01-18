#include "Server.hpp"
#include <format>
#include <functional>
#include <iostream>
#include <queue>
#include <random>
#include <unistd.h>
#include <vector>

Server::Server(double arrivalRate, double serviceRate, std::size_t msgNum)
    : m_arrivalRate(arrivalRate),
      m_serviceRate(serviceRate),
      m_maxQueueSize(msgNum) {
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
        return lhs.time >= rhs.time;
    }
};

// Undefine if you want to solve question 1.
#define Q4

Server::SimResult Server::simulate(double simTime) const {

    double currentTime = 0;

    // Stores all events that arrived, with currentTime > arrivingTime.
    std::queue<double> waitQueue{};

    // Stores all events that are going to happen (packets arriving, or packets finishing).
    std::priority_queue<Server::Event, std::vector<Server::Event>, EventGreater> eventHeap{};

    // First packet is arrival, prepare one.
    eventHeap.push({getExpNumber(m_arrivalRate), Server::EventType::ARRIVAL});

    bool isServerBusy = false;
    std::size_t totalDroppedPackets = 0, totalServedNum = 0;
    double totalWaitTime = 0, totalServeTime = 0, lastQueryServerTime = 0;

    SimResult simResult;

    LOG("Started Server Sim");

    // Add a departue event to the heap, change the variables state. 
    auto addDeparture = [&]() -> void {
        double serveTime = getExpNumber(m_serviceRate);
        double schedTime = currentTime + serveTime;
        eventHeap.push({schedTime, Server::EventType::DEPARTURE});
        totalServeTime += serveTime;
        LOG(std::format("Departure sched to {}", schedTime));
    };

    // Schedule the head of waiting queue, change the variables state.
    auto schedFromWaitQueue = [&]() -> void {
        double arrivalTime = waitQueue.front();
        waitQueue.pop();
        double waitTime = currentTime - arrivalTime;
        totalWaitTime += waitTime;

        // Serve the oldest query waiting in the queue
        addDeparture();
    };

    // Event handling loop
    while (currentTime < simTime
#ifdef Q4
           && !eventHeap.empty()
#endif
    ) {

        // Get the packet with the minimal time (Arrival or departure)
        auto [eventTime, eventType] = eventHeap.top();
        eventHeap.pop();

        currentTime = eventTime;

        if (eventType == Server::EventType::ARRIVAL) {
            // If the server is not busy, sample a serving time and put it to headp
            if (!isServerBusy) {
                isServerBusy = true;
                addDeparture();
            } else {
                // If there server is busy, put the packet in the waiting queue, as long
                // as there is space, otherwise, drop the packet
                if (waitQueue.size() < m_maxQueueSize) {
                    waitQueue.push(eventTime);
                } else {
                    totalDroppedPackets++;
                }
            }

            // Prepare the next arrival
            double arriveTime = getExpNumber(m_arrivalRate);
            double schedTime = arriveTime + currentTime;

#ifdef Q4
            if (schedTime < simTime) {
                eventHeap.push({schedTime, Server::EventType::ARRIVAL});
                LOG(std::format("Arrive sched to {}", schedTime));
            }
#else
            eventHeap.push({schedTime, Server::EventType::ARRIVAL});
            LOG(std::format("Arrive sched to {}", schedTime));
#endif
        }

        // If the server finished serving the packet:
        if (eventType == Server::EventType::DEPARTURE) {
            totalServedNum++;
            lastQueryServerTime = currentTime;
            // If there are packets waiting get one and generate a serving time for it
            if (!waitQueue.empty()) {
                schedFromWaitQueue();
            } else {
                isServerBusy = false;
            }
        }
    }

    LOG("Finished simTime");

#ifdef Q4
    /*
     * currentTime >= simTime, in Q4, we need to serve remaining packets.
     */

    if (!eventHeap.empty() || !waitQueue.empty()) {

        // In case that evenHeap is empty while packets are waiting, put the oldest in heap
        if (eventHeap.empty()) {
            schedFromWaitQueue();
        }

        // Last if
        while (!eventHeap.empty()) {
            auto [eventTime, _] = eventHeap.top();
            eventHeap.pop();
            currentTime = eventTime;
            LOG(std::format("Current Time is {}", currentTime));
            lastQueryServerTime = currentTime;
            totalServedNum++;
            if (!waitQueue.empty()) {
                schedFromWaitQueue();
            }
        }
    }

    simResult.totalServedQueries = totalServedNum;
    simResult.totalDroppedQueries = totalDroppedPackets;
    simResult.averageServeTime = totalServeTime / totalServedNum;
    simResult.averageWaitTime = totalWaitTime / totalServedNum;
    simResult.lastQueryServeTime = lastQueryServerTime;
#endif

    return simResult;
}
