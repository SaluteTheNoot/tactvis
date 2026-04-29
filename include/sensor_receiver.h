#pragma once
#include "entity_manager.h"
#include <atomic>
#include <thread>

// Listens on a UDP socket for JSON sensor packets and writes into EntityManager.
// Runs on its own thread — call start() once, stop() on shutdown.
class SensorReceiver {
public:
    explicit SensorReceiver(EntityManager& em, uint16_t port = 5005);
    ~SensorReceiver();

    void start();
    void stop();

private:
    void run();
    bool parse_packet(const char* buf, int len);

    EntityManager&    em_;
    uint16_t          port_;
    int               sock_fd_{-1};
    std::atomic<bool> running_{false};
    std::thread       thread_;
};
