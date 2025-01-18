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

// At each iteration:
// If the queue is not empty, get its head: Get a random serving time, push it to the heap and increase the virtual
// time. Eitherway: Get a random arrival time, and push it to the heap(current time + time until arrival).
//
//

#define LoadBalancer
#ifndef LoadBalancer
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

    while (currentTime < simTime) {

        // Get the packet with the minimal time (Arrival or departure)
        auto [eventTime, eventType] = eventHeap.top();
        eventHeap.pop();

        currentTime = eventTime;

        if (eventType == Server::EventType::ARRIVAL) {
            // If the server is not busy, sample a serving time and put it to headp
            if (!isServerBusy) {
                isServerBusy = true;
                double serveTime = getExpNumber(m_serviceRate);
                double schedTime = currentTime + serveTime;
                eventHeap.push({schedTime, Server::EventType::DEPARTURE});
                totalServeTime += serveTime;
                LOG(std::format("Departure sched to {}", schedTime));

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
            eventHeap.push({schedTime, Server::EventType::ARRIVAL});
            LOG(std::format("Arrive sched to {}", schedTime));
        }

        // If the server finished serving the packet:
        if (eventType == Server::EventType::DEPARTURE) {
            totalServedNum++;
            lastQueryServerTime = currentTime;
            // If there are packets waiting get one and generate a serving time for it
            if (!waitQueue.empty()) {
                double arrivalTime = waitQueue.front();
                waitQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;

                // Serve the oldest query waiting in the queue
                double serveTime = getExpNumber(m_serviceRate);
                double schedTime = currentTime + serveTime;
                eventHeap.push({schedTime, Server::EventType::DEPARTURE});
                LOG(std::format("Departure sched to {}", schedTime));
                totalServeTime += serveTime;
            } else {
                isServerBusy = false;
            }
        }
    }

    return simResult;
}

#else

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

    while (currentTime < simTime && !eventHeap.empty()) {

        // Get the packet with the minimal time (Arrival or departure)
        auto [eventTime, eventType] = eventHeap.top();
        eventHeap.pop();

        currentTime = eventTime;

        if (eventType == Server::EventType::ARRIVAL) {
            // If the server is not busy, sample a serving time and put it to headp
            if (!isServerBusy) {
                isServerBusy = true;
                double serveTime = getExpNumber(m_serviceRate);
                double schedTime = currentTime + serveTime;
                eventHeap.push({schedTime, Server::EventType::DEPARTURE});
                totalServeTime += serveTime;
                LOG(std::format("Departure sched to {}", schedTime));

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
            if (schedTime < simTime) {
                eventHeap.push({schedTime, Server::EventType::ARRIVAL});
                LOG(std::format("Arrive sched to {}", schedTime));
            }
        }

        // If the server finished serving the packet:
        if (eventType == Server::EventType::DEPARTURE) {
            totalServedNum++;
            lastQueryServerTime = currentTime;
            // If there are packets waiting get one and generate a serving time for it
            if (!waitQueue.empty()) {
                double arrivalTime = waitQueue.front();
                waitQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;

                // Serve the oldest query waiting in the queue
                double serveTime = getExpNumber(m_serviceRate);
                double schedTime = currentTime + serveTime;
                eventHeap.push({schedTime, Server::EventType::DEPARTURE});
                LOG(std::format("Departure sched to {}", schedTime));
                totalServeTime += serveTime;
            } else {
                isServerBusy = false;
            }
        }
    }

    LOG("Finished simTime");

    // simTime ended, but there are still packets in the server
    if (!eventHeap.empty() || !waitQueue.empty()) {

        // In case that evenHeap is empty while packets are waiting, put the oldest in heap
        if (eventHeap.empty()) {
            double arrivalTime = waitQueue.front();
            waitQueue.pop();
            double waitTime = currentTime - arrivalTime;
            totalWaitTime += waitTime;

            // Serve the oldest query waiting in the queue
            double serveTime = getExpNumber(m_serviceRate);
            double schedTime = currentTime + serveTime;
            eventHeap.push({schedTime, Server::EventType::DEPARTURE});
            LOG(std::format("Departure sched to {}", schedTime));
            totalServeTime += serveTime;
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
                double arrivalTime = waitQueue.front();
                waitQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;

                // Serve the oldest query waiting in the queue
                double serveTime = getExpNumber(m_serviceRate);
                double schedTime = currentTime + serveTime;
                eventHeap.push({schedTime, Server::EventType::DEPARTURE});
                LOG(std::format("Departure sched to {}", schedTime));
                totalServeTime += serveTime;
            }
        }
    }

    simResult.totalServedQueries = totalServedNum;
    simResult.totalDroppedQueries = totalDroppedPackets;
    simResult.averageServeTime = totalServeTime / totalServedNum;
    simResult.averageWaitTime = totalWaitTime / totalServedNum;
    simResult.lastQueryServeTime = lastQueryServerTime;

    return simResult;
}
#endif
