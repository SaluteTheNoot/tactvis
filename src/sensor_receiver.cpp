#include "sensor_receiver.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <chrono>

// Platform sockets
#ifdef _WIN32
  #include <winsock2.h>
  #pragma comment(lib,"ws2_32.lib")
  #define CLOSE_SOCK(s) closesocket(s)
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <unistd.h>
  #define CLOSE_SOCK(s) ::close(s)
#endif

SensorReceiver::SensorReceiver(EntityManager& em, uint16_t port)
    : em_(em), port_(port) {}

SensorReceiver::~SensorReceiver() { stop(); }

void SensorReceiver::start() {
    running_ = true;
    thread_ = std::thread(&SensorReceiver::run, this);
}

void SensorReceiver::stop() {
    running_ = false;
    if (sock_fd_ >= 0) { CLOSE_SOCK(sock_fd_); sock_fd_ = -1; }
    if (thread_.joinable()) thread_.join();
}

void SensorReceiver::run() {
    sock_fd_ = static_cast<int>(::socket(AF_INET, SOCK_DGRAM, 0));
    if (sock_fd_ < 0) { std::cerr << "[SensorReceiver] socket() failed\n"; return; }

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port_);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(sock_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        std::cerr << "[SensorReceiver] bind() failed on port " << port_ << "\n";
        CLOSE_SOCK(sock_fd_); sock_fd_ = -1; return;
    }

    // Non-blocking timeout so we can check running_ flag
    struct timeval tv{ .tv_sec = 0, .tv_usec = 100000 };
    setsockopt(sock_fd_, SOL_SOCKET, SO_RCVTIMEO,
               reinterpret_cast<const char*>(&tv), sizeof(tv));

    char buf[4096];
    std::cout << "[SensorReceiver] Listening on UDP port " << port_ << "\n";

    while (running_) {
        int len = static_cast<int>(::recvfrom(sock_fd_, buf, sizeof(buf) - 1, 0, nullptr, nullptr));
        if (len <= 0) continue;
        buf[len] = '\0';
        parse_packet(buf, len);
    }
    CLOSE_SOCK(sock_fd_); sock_fd_ = -1;
}

// Minimal hand-rolled JSON field extractor (avoids external dep)
static bool extract_str(const char* json, const char* key, char* out, size_t out_len) {
    const char* p = std::strstr(json, key);
    if (!p) return false;
    p = std::strchr(p, ':');
    if (!p) return false;
    while (*++p == ' ');
    if (*p == '"') {
        p++;
        size_t i = 0;
        while (*p && *p != '"' && i < out_len - 1) out[i++] = *p++;
        out[i] = '\0';
        return true;
    }
    return false;
}

static bool extract_float(const char* json, const char* key, float& out) {
    const char* p = std::strstr(json, key);
    if (!p) return false;
    p = std::strchr(p, ':');
    if (!p) return false;
    while (*++p == ' ');
    out = static_cast<float>(std::atof(p));
    return true;
}

static bool extract_int(const char* json, const char* key, int& out) {
    const char* p = std::strstr(json, key);
    if (!p) return false;
    p = std::strchr(p, ':');
    if (!p) return false;
    while (*++p == ' ');
    out = std::atoi(p);
    return true;
}

bool SensorReceiver::parse_packet(const char* buf, int /*len*/) {
    Entity e{};

    int id = 0;
    if (!extract_int(buf, "\"id\"", id)) return false;
    e.id = static_cast<uint32_t>(id);

    char callsign[32] = {};
    extract_str(buf, "\"callsign\"", callsign, sizeof(callsign));
    e.callsign = callsign;

    float x = 0, y = 0, alt = 0, heading = 0, speed = 0;
    extract_float(buf, "\"x\"",       x);
    extract_float(buf, "\"y\"",       y);
    extract_float(buf, "\"alt\"",     alt);
    extract_float(buf, "\"heading\"", heading);
    extract_float(buf, "\"speed\"",   speed);

    e.position = { x, alt, y };
    e.heading  = heading;
    e.speed    = speed;

    char iff_str[16] = {};
    extract_str(buf, "\"iff\"", iff_str, sizeof(iff_str));
    if      (std::strcmp(iff_str, "friendly") == 0) e.iff = IFF::Friendly;
    else if (std::strcmp(iff_str, "hostile")  == 0) e.iff = IFF::Hostile;
    else                                             e.iff = IFF::Unknown;

    char type_str[16] = {};
    extract_str(buf, "\"type\"", type_str, sizeof(type_str));
    if      (std::strcmp(type_str, "aircraft") == 0)  e.type = EntityType::Aircraft;
    else if (std::strcmp(type_str, "vehicle")  == 0)  e.type = EntityType::GroundVehicle;
    else                                               e.type = EntityType::Waypoint;

    e.active = true;
    e.last_update = std::chrono::duration<double>(
        std::chrono::steady_clock::now().time_since_epoch()).count();

    em_.upsert(e);
    return true;
}
