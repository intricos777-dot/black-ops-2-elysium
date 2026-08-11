# BO2 Lobby Design

- 50-player localized lobbies per system
- Peer-to-peer session mesh with host scaffolding
- Deterministic backend API stub
- Fallback host promotion if host drops

## Build
```bash
mkdir build && cd build
cmake ..
cmake --build .
ctest --output-on-failure
```
