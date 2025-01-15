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

#undef Submission
#ifndef Submission
Server::SimResult Server::simulate(double simTime) {

    double currentTime = 0;

    // Stores all events that arrived, with currentTime > arrivingTime.
    std::queue<double> arrivalTimeQueue{};

    // Stores all events that are going to happen (packets arriving, or packets finishing).
    std::priority_queue<Server::Event, std::vector<Server::Event>, EventGreater> eventHeap{};

    bool isServerBusy = false;

    std::size_t totalWaitTime = 0, servedNum = 0;

    eventHeap.push({getExpNumber(m_arrivalRate), Server::EventType::ARRIVAL});

    while (currentTime < simTime) {

        LOG(" ---Start---");
        auto [eventTime, eventType] = eventHeap.top();
        LOG(std::format("Time is {}, type is {}", eventTime, int(eventType)));
        eventHeap.pop();
        currentTime = eventTime;

        DEBUG(sleep(1));

        if (eventType == Server::EventType::ARRIVAL) {
            LOG("ARRIVAL");
            LOG(std::format("Message arrived at time {}", currentTime));
            if (!isServerBusy) {
                // Generate a serving time and the packet to the heap.
                isServerBusy = true;
                double serveTime = currentTime + getExpNumber(m_serviceRate);
                eventHeap.push({serveTime, Server::EventType::DEPARTURE});
                LOG(std::format("Message scheduled to end at time {}", serveTime));

            } else {
                LOG("Server is busy");
                // If there server is busy, put the packet in the waiting queue, as long
                // as there is space, otherwise, drop the packet
                if (arrivalTimeQueue.size() < m_maxQueueSize) {
                    arrivalTimeQueue.push(eventTime);
                    LOG(std::format("Server is busy, added to waiting queue"));
                }
            }

            // Prepare the next arrival
            double nextArrival = currentTime + getExpNumber(m_arrivalRate);
            eventHeap.push({nextArrival, Server::EventType::ARRIVAL});
            LOG(std::format("Message scheduled to arrive at time {}", nextArrival));
        }

        // If the server finished serving the packet:
        if (eventType == Server::EventType::DEPARTURE) {
            LOG("DEPARTURE");
            LOG(std::format("Message departured at time {}", currentTime));
            servedNum++;
            // If there are packets waiting get one and generate a serving time for it
            if (!arrivalTimeQueue.empty()) {
                double arrivalTime = arrivalTimeQueue.front();
                arrivalTimeQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;

                // Serve the oldest query waiting in the queue
                double serveTime = currentTime + getExpNumber(m_serviceRate);
                eventHeap.push({serveTime, Server::EventType::DEPARTURE});
                LOG(std::format("Message scheduled to end at time {}", serveTime));
            } else {
                LOG("Server is not busy");
                isServerBusy = false;
            }
        }
        LOG(" ---END---\n");
    }
    std::cout << (std::format("Average Wait time is {}, Served Num is {}\n", totalWaitTime / servedNum, servedNum));
    return SimResult();
}

#else
Server::SimResult Server::simulate(double simTime) {
    double currentTime = 0;
    std::queue<double> arrivalTimeQueue{};
    std::priority_queue<Server::Event, std::vector<Server::Event>, EventGreater> eventHeap{};
    bool isServerBusy = false;

    std::size_t totalWaitTime = 0, servedNum = 0;

    eventHeap.push({getExpNumber(m_arrivalRate), Server::EventType::ARRIVAL});

    while (currentTime < simTime) {

        auto& [eventTime, eventType] = eventHeap.top();
        LOG(std::format("Time is {}, type is {}", eventTime, int(eventType)) << '\n');
        eventHeap.pop();
        currentTime = eventTime;

        sleep(1);

        if (eventType == Server::EventType::ARRIVAL) {
            LOG(std::format("Message arrived at time {}\n", currentTime));
            if (!isServerBusy) {
                isServerBusy = true;
                double serviceTime = getExpNumber(m_serviceRate);
                eventHeap.push({currentTime + serviceTime, Server::EventType::DEPARTURE});
                LOG(std::format("Message scheduled to end at time {}\n", currentTime + serviceTime));

            } else {
                if (arrivalTimeQueue.size() < m_msgNum) {
                    arrivalTimeQueue.push(eventTime);
                }
            }

            double nextArrival = currentTime + getExpNumber(m_arrivalRate);
            eventHeap.push({nextArrival, Server::EventType::ARRIVAL});
            LOG(std::format("Message scheduled to end at time {}\n", nextArrival));

        } else if (eventType == Server::EventType::DEPARTURE) {
            LOG(std::format("Message departured at time {}\n", currentTime));
            servedNum++;
            if (!arrivalTimeQueue.empty()) {
                double arrivalTime = arrivalTimeQueue.front();
                arrivalTimeQueue.pop();
                double waitTime = currentTime - arrivalTime;
                totalWaitTime += waitTime;
                double serviceTime = getExpNumber(m_serviceRate);
                eventHeap.push({currentTime + serviceTime, Server::EventType::DEPARTURE});
                LOG(std::format("Message scheduled to end at time {}\n", currentTime + serviceTime));
            } else {
                isServerBusy = false;
            }
        }
    }

    return SimResult();
}
#endif
