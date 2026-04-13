#pragma once
#include <string>
#include <memory>
#include <iostream>

enum class Position {
    Goalkeeper,
    Outfield
};

class Player {
public:
    Player(std::string name, int rating, Position pos);
    virtual ~Player() = default;

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    const std::string& name() const noexcept { return m_name; }
    int rating() const noexcept { return m_rating; }
    Position position() const noexcept { return m_pos; }

    // Polymorphic "impact" in simulation
    virtual int attackImpact() const = 0;
    virtual int defenseImpact() const = 0;

    friend std::ostream& operator<<(std::ostream& os, const Player& p);

protected:
    std::string m_name;
    int m_rating;      // 1..100
    Position m_pos;
};

class Goalkeeper final : public Player {
public:
    Goalkeeper(std::string name, int rating, int reflex);
    int reflex() const noexcept { return m_reflex; }

    int attackImpact() const override;
    int defenseImpact() const override;

private:
    int m_reflex; // 1..100
};

class OutfieldPlayer final : public Player {
public:
    OutfieldPlayer(std::string name, int rating, int finishing, int tackling);

    int finishing() const noexcept { return m_finishing; }
    int tackling()  const noexcept { return m_tackling; }

    int attackImpact() const override;
    int defenseImpact() const override;

private:
    int m_finishing; // 1..100
    int m_tackling;  // 1..100
};
