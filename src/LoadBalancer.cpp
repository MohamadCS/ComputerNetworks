#include "LoadBalancer.hpp"
#include <cstddef>

LoadBalancer::LoadBalancer(double internetSendingRate, const std::vector<double>& probVec,
                           const std::vector<std::size_t>& maxQueueVec, const std::vector<double>& servingRateVec)
    : m_internetSendingRate(internetSendingRate),
      m_servers() {
          std::size_t serversNum = probVec.size();
          m_servers.reserve(serversNum);

          // Create servers
          for(int i = 0 ; i < probVec.size(); ++i){
              double arrivalRate = probVec.at(i) * internetSendingRate;
              Server server(arrivalRate,servingRateVec.at(i),maxQueueVec.at(i));
              m_servers.emplace_back(std::move(server));
          }
}
