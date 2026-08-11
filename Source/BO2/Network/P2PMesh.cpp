#include "P2PMesh.hpp"
#include <random>
#include <chrono>
#include <cassert>

namespace BO2 {
    namespace {
        PeerId generatePeerId() {
            static std::mt19937_64 rng(
                (uint64_t)std::chrono::high_resolution_clock::now().time_since_epoch().count()
            );
            return PeerId{rng()};
        }
    }

    struct P2PMesh::Impl {
        std::optional<LobbyState> lobby;
        PacketHandler handler;
        PeerId localPeer = generatePeerId();
        bool host = false;
    };

    P2PMesh::P2PMesh() : impl_(new Impl) {}
    P2PMesh::~P2PMesh() = default;

    bool P2PMesh::hostLobby(const LobbyConfig& cfg) {
        LobbyState state;
        state.config = cfg;
        state.host = impl_->localPeer;
        state.members.push_back({impl_->localPeer, "host", 0, true});
        state.memberCount = 1;
        impl_->lobby = state;
        impl_->host = true;
        return true;
    }

    bool P2PMesh::joinLobby(PeerId hostPeer, const LobbyConfig& cfg) {
        if (!impl_->lobby) {
            LobbyState state;
            state.config = cfg;
            state.host = hostPeer;
            state.members.push_back({hostPeer, "host", 0, true});
            state.members.push_back({impl_->localPeer, "peer", 42, false});
            state.memberCount = 2;
            impl_->lobby = state;
            impl_->host = false;
            return true;
        }
        return false;
    }

    void P2PMesh::leaveLobby() {
        impl_->lobby.reset();
        impl_->host = false;
    }

    void P2PMesh::sendTo(PeerId to, const uint8_t* data, size_t len) {
        (void)to; (void)data; (void)len;
        if (impl_->handler) {
            // stub delivery
        }
    }

    void P2PMesh::broadcast(const uint8_t* data, size_t len) {
        (void)data; (void)len;
        if (impl_->handler && impl_->lobby) {
            for (const auto& m : impl_->lobby->members) {
                if (m.peer != impl_->localPeer) {
                    impl_->handler(m.peer, nullptr, 0);
                }
            }
        }
    }

    void P2PMesh::tick() {
        // stub heartbeat
    }

    void P2PMesh::setPacketHandler(PacketHandler handler) {
        impl_->handler = std::move(handler);
    }

    const LobbyState* P2PMesh::state() const {
        return impl_->lobby ? &*impl_->lobby : nullptr;
    }

    bool P2PMesh::isHost() const { return impl_->host; }
    PeerId P2PMesh::localPeer() const { return impl_->localPeer; }
}
