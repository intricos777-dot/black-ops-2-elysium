#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>

namespace BO2 {
    struct PeerId {
        uint64_t value;
        bool operator==(const PeerId& other) const { return value == other.value; }
    };

    struct LobbyConfig {
        uint32_t maxPlayers;
        uint32_t regionId;
        bool dedicatedHost;
        std::string playlist;
    };

    struct LobbyMember {
        PeerId peer;
        std::string name;
        uint32_t pingMs;
        bool isHost;
    };

    struct LobbyState {
        PeerId host;
        std::vector<LobbyMember> members;
        LobbyConfig config;
        uint32_t memberCount;
    };
}
