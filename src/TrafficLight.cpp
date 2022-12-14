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
    std::unique_lock<std::mutex> uniqueLock(_mutex);
    _condition.wait(uniqueLock, [this] { return !_queue.empty(); });
    T message = std::move(_queue.back());
    _queue.pop_back();
    return message;
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
    while (true) {
        TrafficLightPhase trafficLightPhase = _messageQueue.receive();
        if (trafficLightPhase == TrafficLightPhase::green) return;
    }
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
    long cycleDuration = uniformIntDistribution(randomNumberGenerator) * 1000;

    std::chrono::time_point<std::chrono::system_clock> lastUpdate;
    lastUpdate = std::chrono::system_clock::now();
    while (true) {
        std::this_thread::sleep_for(1ms);

        long timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - lastUpdate
        ).count();
        if (timeSinceLastUpdate < cycleDuration) continue;

        _currentPhase = _currentPhase == TrafficLightPhase::red ? TrafficLightPhase::green : TrafficLightPhase::red;
        _messageQueue.send(std::move(_currentPhase));

        lastUpdate = std::chrono::system_clock::now();
    }
}