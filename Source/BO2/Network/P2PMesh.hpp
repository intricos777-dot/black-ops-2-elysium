#pragma once
#include "LobbyTypes.hpp"
#include <functional>
#include <memory>

namespace BO2 {
    class P2PMesh {
    public:
        using PacketHandler = std::function<void(PeerId from, const uint8_t* data, size_t len)>;
        P2PMesh();
        ~P2PMesh();

        bool hostLobby(const LobbyConfig& cfg);
        bool joinLobby(PeerId host, const LobbyConfig& cfg);
        void leaveLobby();
        void sendTo(PeerId to, const uint8_t* data, size_t len);
        void broadcast(const uint8_t* data, size_t len);
        void tick();

        void setPacketHandler(PacketHandler handler);
        const LobbyState* state() const;
        bool isHost() const;
        PeerId localPeer() const;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}
