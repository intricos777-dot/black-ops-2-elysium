#include "Source/BO2/Network/P2PMesh.hpp"
#include <cassert>
#include <cstdint>

int main() {
    BO2::P2PMesh mesh;
    BO2::LobbyConfig cfg{50, 1, true, "team_deathmatch"};
    assert(mesh.hostLobby(cfg));
    assert(mesh.isHost());
    assert(mesh.state()->memberCount == 1);
    assert(mesh.state()->config.maxPlayers == 50);

    BO2::PeerId fakeHost{999};
    BO2::P2PMesh client;
    assert(client.joinLobby(fakeHost, cfg));
    assert(!client.isHost());
    assert(client.state()->memberCount == 2);

    client.leaveLobby();
    assert(client.state() == nullptr);

    return 0;
}
