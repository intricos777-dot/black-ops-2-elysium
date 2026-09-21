#pragma once
#include <cstdint>
#include <string>
#include "data/data_registries.h"
#include "combat/combat.h"

namespace bo2 {

struct GameState {
    bool in_menu = true;
    bool game_over = false;
    uint32_t wave = 0;
    uint32_t total_kills = 0;
    uint32_t perks_unlocked = 0;
    std::string current_map;
    std::string difficulty = "casual";
};

class Game {
public:
    Game();
    bool initialize();
    void shutdown();

    void show_menu();
    void select_map();
    void run_wave();
    void visit_npc();
    void shop();
    void upgrade_weapon();

    GameState& state() { return m_state; }
    WeaponRegistry& weapons() { return m_weapons; }
    PerkRegistry& perks() { return m_perks; }
    MapRegistry& maps() { return m_maps; }
    DifficultyManager& diff() { return m_diff; }
    PlayerState& player() { return m_player; }
    NPCDb& npcs() { return m_npcs; }

private:
    GameState m_state;
    WeaponRegistry m_weapons;
    PerkRegistry m_perks;
    MapRegistry m_maps;
    DifficultyManager m_diff;
    PlayerState m_player;
    NPCDb m_npcs;

    void draw_hud();
};

} // namespace bo2
