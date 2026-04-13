#include "Player.h"
#include <algorithm>

static int clamp100(int v) { return std::max(1, std::min(100, v)); }

Player::Player(std::string name, int rating, Position pos)
    : m_name(std::move(name)), m_rating(clamp100(rating)), m_pos(pos) {
}

std::ostream& operator<<(std::ostream& os, const Player& p) {
    os << p.m_name << " ("
        << (p.m_pos == Position::Goalkeeper ? "GK" : "OUT")
        << ", rating " << p.m_rating << ")";
    return os;
}

Goalkeeper::Goalkeeper(std::string name, int rating, int reflex)
    : Player(std::move(name), rating, Position::Goalkeeper),
    m_reflex(clamp100(reflex)) {
}

int Goalkeeper::attackImpact() const {
    // GK contributes a little to build-up
    return (m_rating * 10 + m_reflex * 2) / 100;
}

int Goalkeeper::defenseImpact() const {
    // GK is huge in defense
    return (m_rating * 40 + m_reflex * 80) / 100;
}

OutfieldPlayer::OutfieldPlayer(std::string name, int rating, int finishing, int tackling)
    : Player(std::move(name), rating, Position::Outfield),
    m_finishing(clamp100(finishing)),
    m_tackling(clamp100(tackling)) {
}

int OutfieldPlayer::attackImpact() const {
    return (m_rating * 25 + m_finishing * 60) / 100;
}

int OutfieldPlayer::defenseImpact() const {
    return (m_rating * 20 + m_tackling * 60) / 100;
}
