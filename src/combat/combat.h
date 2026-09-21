#pragma once
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "data/data_registries.h"

namespace bo2 {

struct PlayerState {
    float hp = 100;
    float max_hp = 100;
    uint32_t points = 500;
    uint32_t kills = 0;
    uint32_t deaths = 0;
    std::string weapon = "M16A1";
    std::string perks[3] = {"", "", ""};
    uint32_t perk_count = 0;
    bool alive = true;
    uint32_t x = 0, y = 0;
};

// ---- Zombies --------------------------------------------------------------

struct ZombieDef {
    std::string name;
    uint32_t hp;
    uint32_t damage;
    float speed;
    uint32_t reward;
};

enum class CombatState { idle, player_turn, zombie_turn, victory, defeat };

class ZombieCombat {
public:
    ZombieCombat(const DifficultyManager& diff);

    void engage(const ZombieDef& zombie, PlayerState& player);
    std::vector<std::string> attack();
    std::vector<std::string> reload();
    std::vector<std::string> use_equipment();
    std::vector<std::string> flee();

    bool is_over() const { return m_state == CombatState::victory || m_state == CombatState::defeat; }
    bool player_won() const { return m_state == CombatState::victory; }
    uint32_t enemy_hp() const { return m_enemy_hp; }
    uint32_t enemy_max_hp() const { return m_enemy_max_hp; }
    uint32_t player_hp() const { return m_player_hp; }
    CombatState state() const { return m_state; }

private:
    const DifficultyManager& m_diff;
    CombatState m_state = CombatState::idle;
    uint32_t m_enemy_hp = 0;
    uint32_t m_enemy_max_hp = 0;
    uint32_t m_player_hp = 0;
    uint32_t m_player_max_hp = 0;
    PlayerState* m_player = nullptr;
    uint32_t m_ammo_in_mag = 0;
    uint32_t m_mag_size = 30;
    uint32_t m_ammo_reserve = 90;
    uint32_t m_equipment_charges = 2;
    ZombieDef m_zombie;

    void enemy_attack();
    void check_victory();
};

// ---- Wave Manager ---------------------------------------------------------

struct WaveState {
    uint32_t wave = 0;
    uint32_t zombies_alive = 0;
    uint32_t zombies_total = 0;
    uint32_t points_reward = 0;
};

class WaveManager {
public:
    WaveManager(const DifficultyManager& diff);

    void start_wave(uint32_t wave);
    std::vector<ZombieDef> spawn_wave();
    void kill_one() { m_alive--; }
    bool wave_cleared() const { return m_alive == 0; }
    const WaveState& state() const { return m_state; }

private:
    const DifficultyManager& m_diff;
    WaveState m_state;
    uint32_t m_alive = 0;
};

// ---- NPC -------------------------------------------------------------------

struct NPCDef {
    std::string id;
    std::string name;
    std::string role;
    std::string current_map;
    std::vector<std::string> lines;
};

class NPCDb {
public:
    void register_defaults();
    const std::vector<NPCDef>& npcs() const { return m_npcs; }
    const NPCDef* find(const std::string& id) const;
    std::vector<const NPCDef*> in_map(const std::string& map) const;
private:
    std::vector<NPCDef> m_npcs;
};

} // namespace bo2
