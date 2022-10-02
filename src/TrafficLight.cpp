#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include "TrafficLight.h"

using namespace std::chrono_literals;

/* Implementation of class "MessageQueue" */

template<typename T>
T MessageQueue<T>::receive() {
    // FP.5a : The method receive should use std::unique_lock<std::mutex> and _condition.wait() 
    // to wait for and receive new messages and pull them from the queue using move semantics. 
    // The received object should then be returned by the receive function. 
}

template<typename T>
void MessageQueue<T>::send(T &&message) {
    std::lock_guard<std::mutex> lockGuard(_mutex);
    _queue.push_front(std::move(message));
    _condition.notify_one();
}

/* Implementation of class "TrafficLight" */

TrafficLight::TrafficLight() : _currentPhase(TrafficLightPhase::red) {}

void TrafficLight::waitForGreen() {
    // FP.5b : add the implementation of the method waitForGreen, in which an infinite while-loop 
    // runs and repeatedly calls the receive function on the message queue. 
    // Once it receives TrafficLightPhase::green, the method returns.
}

TrafficLightPhase TrafficLight::getCurrentPhase() { return _currentPhase; }

void TrafficLight::simulate() {
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases() {
    std::random_device randomDevice;
    std::mt19937 randomNumberGenerator(randomDevice());
    std::uniform_int_distribution<long> uniformIntDistribution(4, 6);
    long cycleDuration = uniformIntDistribution(randomNumberGenerator);

    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(1ms);

        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - lastUpdate
        ).count();
        if (timeSinceLastUpdate < cycleDuration) continue;

        _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
        lastUpdate = std::chrono::system_clock::now();
    }
}