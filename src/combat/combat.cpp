#include "combat.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <algorithm>

namespace bo2 {

ZombieCombat::ZombieCombat(const DifficultyManager& diff) : m_diff(diff) {
    std::srand(std::time(nullptr));
}

void ZombieCombat::engage(const ZombieDef& zombie, PlayerState& player) {
    m_zombie = zombie;
    m_player = &player;
    m_player_hp = (uint32_t)player.hp;
    m_player_max_hp = (uint32_t)player.max_hp;
    const auto& d = m_diff.current();
    m_enemy_max_hp = (uint32_t)(zombie.hp * d.enemy_health_mult);
    m_enemy_hp = m_enemy_max_hp;
    m_ammo_in_mag = 30;
    m_ammo_reserve = 90;
    m_state = CombatState::player_turn;
    std::printf("[Combat] Engaged %s (hp=%u)\n", zombie.name.c_str(), m_enemy_hp);
}

std::vector<std::string> ZombieCombat::attack() {
    std::vector<std::string> log;
    if (m_state != CombatState::player_turn || !m_player) return log;
    if (m_ammo_in_mag == 0) { log.push_back("Click! Reload."); return log; }

    uint32_t dmg = 30 + std::rand() % 20;
    bool crit = (std::rand() % 100) < 15;
    if (crit) dmg = dmg * 2;

    m_enemy_hp = std::max(0u, m_enemy_hp - dmg);
    m_ammo_in_mag--;

    log.push_back("You shoot " + m_zombie.name + " for " + std::to_string(dmg) + (crit ? " (CRIT)" : ""));
    log.push_back("Ammo: " + std::to_string(m_ammo_in_mag) + "/" + std::to_string(m_mag_size));

    check_victory();
    if (m_state != CombatState::victory) {
        m_state = CombatState::zombie_turn;
        enemy_attack();
    }
    return log;
}

std::vector<std::string> ZombieCombat::reload() {
    uint32_t need = m_mag_size - m_ammo_in_mag;
    uint32_t take = std::min(need, m_ammo_reserve);
    m_ammo_in_mag += take;
    m_ammo_reserve -= take;
    return {"Reloaded."};
}

std::vector<std::string> ZombieCombat::use_equipment() {
    if (m_equipment_charges == 0) return {"No equipment charges left!"};
    m_equipment_charges--;
    uint32_t dmg = 500;
    m_enemy_hp = std::max(0u, m_enemy_hp - dmg);
    return {"Frag grenade! " + std::to_string(dmg) + " damage to " + m_zombie.name};
}

std::vector<std::string> ZombieCombat::flee() {
    if ((std::rand() % 100) < 30) {
        m_state = CombatState::idle;
        return {"You break line of sight."};
    }
    enemy_attack();
    return {"Can't escape!"};
}

void ZombieCombat::enemy_attack() {
    if (m_enemy_hp == 0) return;
    const auto& d = m_diff.current();
    if ((std::rand() % 100) / 100.0f > d.enemy_accuracy) {
        m_state = CombatState::player_turn;
        return;
    }
    uint32_t dmg = (uint32_t)(m_zombie.damage * d.enemy_damage_mult);
    dmg = std::max(1u, dmg);
    m_player_hp = std::max(0u, m_player_hp - dmg);
    std::printf("[Combat] %s hits you for %u\n", m_zombie.name.c_str(), dmg);
    if (m_player_hp == 0) {
        m_state = CombatState::defeat;
    } else {
        m_state = CombatState::player_turn;
    }
}

void ZombieCombat::check_victory() {
    if (m_enemy_hp == 0) {
        m_state = CombatState::victory;
        std::printf("[Combat] %s down.\n", m_zombie.name.c_str());
    }
}

// ---- WaveManager ---------------------------------------------------------

WaveManager::WaveManager(const DifficultyManager& diff) : m_diff(diff) {}

void WaveManager::start_wave(uint32_t wave) {
    m_state.wave = wave;
}

std::vector<ZombieDef> WaveManager::spawn_wave() {
    std::vector<ZombieDef> out;
    uint32_t count = 5 + m_state.wave * 2;
    m_state.zombies_total = count;
    m_state.zombies_alive = count;
    m_alive = count;
    for (uint32_t i = 0; i < count; ++i) {
        ZombieDef z;
        z.name = "Zombie";
        z.hp = 50 + m_state.wave * 10;
        z.damage = 15 + m_state.wave * 3;
        z.speed = 1.0f + m_state.wave * 0.1f;
        z.reward = 10 + m_state.wave * 5;
        out.push_back(std::move(z));
    }
    return out;
}

// ---- NPC ------------------------------------------------------------------

void NPCDb::register_defaults() {
    m_npcs = {
        {"doctor", "Dr. Richtofen", "scientist", "Tranzit",
         {"The test subjects are still alive, you know.",
          "I have seen the future. It is... red.",
          "Activate the towers! Now!"}},
        {"tank", "Tank Dempsey", "soldier", "Tranzit",
         {"I've punched bigger things before breakfast.",
          "The sharks are not the problem.",
          "Right here, right now, I am a god."}},
        {"nikolai", "Nikolai Belinski", "soldier", "Tranzit",
         {"Vodka and zombies. A good night.",
          "I have seen worse. My first wife.",
          "The maxis drone will help us."}},
        {"takeo", "Takeo Masaki", "samurai", "Tranzit",
         {"Honor demands we press on.",
          "The sword sings for battle.",
          "I see the reflection of my soul in this place."}},
    };
    std::printf("[NPC] %zu registered\n", m_npcs.size());
}

const NPCDef* NPCDb::find(const std::string& id) const {
    for (const auto& n : m_npcs) if (n.id == id) return &n;
    return nullptr;
}

std::vector<const NPCDef*> NPCDb::in_map(const std::string& map) const {
    std::vector<const NPCDef*> out;
    for (const auto& n : m_npcs)
        if (n.current_map == map || n.current_map == "any") out.push_back(&n);
    return out;
}

} // namespace bo2
