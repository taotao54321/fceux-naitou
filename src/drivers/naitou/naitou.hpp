#pragma once

#include <array>
#include <type_traits>
#include <utility>
#include <variant>

#include <boost/operators.hpp>

#include "core.hpp"
#include "prelude.hpp"

enum class Side {
    COM = 0,
    HUM = 1,
};

enum class Piece {
    KING = 1,
    ROOK = 2,
    BISHOP = 3,
    GOLD = 4,
    SILVER = 5,
    KNIGHT = 6,
    LANCE = 7,
    PAWN = 8,
    DRAGON = 9,
    HORSE = 10,
    PRO_SILVER = 12,
    PRO_KNIGHT = 13,
    PRO_LANCE = 14,
    PRO_PAWN = 15,
};

constexpr std::array<Piece, 7> pts_hand() {
    return { Piece::ROOK, Piece::BISHOP, Piece::GOLD, Piece::SILVER, Piece::KNIGHT, Piece::LANCE, Piece::PAWN };
}

class Sq : private boost::equality_comparable<Sq> {
private:
    int value_;

    friend bool operator==(Sq lhs, Sq rhs) {
        return lhs.value_ == rhs.value_;
    }

public:
    constexpr explicit Sq(const int value)
        : value_(value) {}

    [[nodiscard]] static constexpr Sq from_xy(const int x, const int y) {
        return Sq(11 * y + x);
    }

    [[nodiscard]] constexpr int get() const { return value_; }

    [[nodiscard]] constexpr int x() const { return value_ % 11; }
    [[nodiscard]] constexpr int y() const { return value_ / 11; }
    [[nodiscard]] constexpr std::pair<int, int> xy() const { return { x(), y() }; }

    [[nodiscard]] constexpr bool is_ok() const {
        return 0 <= value_ && value_ < 11 * 11;
    }

    [[nodiscard]] constexpr bool is_valid() const {
        return 1 <= x() && x() <= 9 && 1 <= y() && y() <= 9;
    }

    [[nodiscard]] static constexpr std::array<Sq, 11 * 11> sqs_ok() {
        return {
            // {{{
            Sq(0),
            Sq(1),
            Sq(2),
            Sq(3),
            Sq(4),
            Sq(5),
            Sq(6),
            Sq(7),
            Sq(8),
            Sq(9),
            Sq(10),
            Sq(11),
            Sq(12),
            Sq(13),
            Sq(14),
            Sq(15),
            Sq(16),
            Sq(17),
            Sq(18),
            Sq(19),
            Sq(20),
            Sq(21),
            Sq(22),
            Sq(23),
            Sq(24),
            Sq(25),
            Sq(26),
            Sq(27),
            Sq(28),
            Sq(29),
            Sq(30),
            Sq(31),
            Sq(32),
            Sq(33),
            Sq(34),
            Sq(35),
            Sq(36),
            Sq(37),
            Sq(38),
            Sq(39),
            Sq(40),
            Sq(41),
            Sq(42),
            Sq(43),
            Sq(44),
            Sq(45),
            Sq(46),
            Sq(47),
            Sq(48),
            Sq(49),
            Sq(50),
            Sq(51),
            Sq(52),
            Sq(53),
            Sq(54),
            Sq(55),
            Sq(56),
            Sq(57),
            Sq(58),
            Sq(59),
            Sq(60),
            Sq(61),
            Sq(62),
            Sq(63),
            Sq(64),
            Sq(65),
            Sq(66),
            Sq(67),
            Sq(68),
            Sq(69),
            Sq(70),
            Sq(71),
            Sq(72),
            Sq(73),
            Sq(74),
            Sq(75),
            Sq(76),
            Sq(77),
            Sq(78),
            Sq(79),
            Sq(80),
            Sq(81),
            Sq(82),
            Sq(83),
            Sq(84),
            Sq(85),
            Sq(86),
            Sq(87),
            Sq(88),
            Sq(89),
            Sq(90),
            Sq(91),
            Sq(92),
            Sq(93),
            Sq(94),
            Sq(95),
            Sq(96),
            Sq(97),
            Sq(98),
            Sq(99),
            Sq(100),
            Sq(101),
            Sq(102),
            Sq(103),
            Sq(104),
            Sq(105),
            Sq(106),
            Sq(107),
            Sq(108),
            Sq(109),
            Sq(110),
            Sq(111),
            Sq(112),
            Sq(113),
            Sq(114),
            Sq(115),
            Sq(116),
            Sq(117),
            Sq(118),
            Sq(119),
            Sq(120),
            // }}}
        };
    }

    [[nodiscard]] static constexpr std::array<Sq, 9 * 9> sqs_valid() {
        return {
            // {{{
            Sq(12),
            Sq(13),
            Sq(14),
            Sq(15),
            Sq(16),
            Sq(17),
            Sq(18),
            Sq(19),
            Sq(20),
            Sq(23),
            Sq(24),
            Sq(25),
            Sq(26),
            Sq(27),
            Sq(28),
            Sq(29),
            Sq(30),
            Sq(31),
            Sq(34),
            Sq(35),
            Sq(36),
            Sq(37),
            Sq(38),
            Sq(39),
            Sq(40),
            Sq(41),
            Sq(42),
            Sq(45),
            Sq(46),
            Sq(47),
            Sq(48),
            Sq(49),
            Sq(50),
            Sq(51),
            Sq(52),
            Sq(53),
            Sq(56),
            Sq(57),
            Sq(58),
            Sq(59),
            Sq(60),
            Sq(61),
            Sq(62),
            Sq(63),
            Sq(64),
            Sq(67),
            Sq(68),
            Sq(69),
            Sq(70),
            Sq(71),
            Sq(72),
            Sq(73),
            Sq(74),
            Sq(75),
            Sq(78),
            Sq(79),
            Sq(80),
            Sq(81),
            Sq(82),
            Sq(83),
            Sq(84),
            Sq(85),
            Sq(86),
            Sq(89),
            Sq(90),
            Sq(91),
            Sq(92),
            Sq(93),
            Sq(94),
            Sq(95),
            Sq(96),
            Sq(97),
            Sq(100),
            Sq(101),
            Sq(102),
            Sq(103),
            Sq(104),
            Sq(105),
            Sq(106),
            Sq(107),
            Sq(108),
            // }}}
        };
    }

    [[nodiscard]] constexpr bool can_promote_hum() const {
        return 1 <= y() && y() <= 3;
    }

    [[nodiscard]] constexpr bool can_put_hum(const Piece pt) const {
        switch (pt) {
        case Piece::PAWN:
        case Piece::LANCE:
            return 2 <= y() && y() <= 9;
        case Piece::KNIGHT:
            return 3 <= y() && y() <= 9;
        default:
            return true;
        }
    }
};

struct CellWall {};
struct CellEmpty {};
struct CellCom {
    Piece pt;
};
struct CellHum {
    Piece pt;
};
using Cell = std::variant<CellWall, CellEmpty, CellCom, CellHum>;

class Board {
private:
    std::array<Cell, 11 * 11> cells_ {};

public:
    [[nodiscard]] const Cell& operator[](Sq sq) const;
    [[nodiscard]] Cell& operator[](Sq sq);
};

class Hand {
private:
    std::array<u8, 7> counts_ {};

public:
    Hand() = default;

    [[nodiscard]] u8 operator[](Piece pt) const;
    [[nodiscard]] u8& operator[](Piece pt);
};

class Position {
private:
    Side side_;
    Board board_;
    Hand hand_com_;
    Hand hand_hum_;

public:
    Position(Side side, const Board& board, const Hand& hand_com, const Hand& hand_hum);

    [[nodiscard]] Side side() const;
    [[nodiscard]] const Board& board() const;
    [[nodiscard]] const Hand& hand_com() const;
    [[nodiscard]] const Hand& hand_hum() const;
};

[[nodiscard]] Side read_side(Core& core);
[[nodiscard]] Board read_board(Core& core);
[[nodiscard]] Hand read_hand_com(Core& core);
[[nodiscard]] Hand read_hand_hum(Core& core);
[[nodiscard]] Position read_position(Core& core);

[[nodiscard]] Sq read_cursor(Core& core);

// ゲーム画面上の盤面マスおよび持駒マスを頂点とみなし、
// 全頂点間最短経路(操作列)を計算してキャッシュする
class Traveller {
private:
    struct Entry {
        u8 size { 0 };
        std::array<Buttons, 11> seq {}; // 最も離れた頂点間でも 11 回で移動可能
    };

    // 盤上 81 マス + 持駒 7 種
    std::array<std::array<Entry, 88>, 88> entrys_ {};

    Traveller() = default;

public:
    [[nodiscard]] static Traveller calc();

    // (操作回数, 操作列) を返す。
    [[nodiscard]] std::pair<u8, const Buttons*> query(int src, int dst) const;

    // x, y は valid でなければならない
    [[nodiscard]] static constexpr int vertex_xy(const int x, const int y) {
        return 9 * (y - 1) + (x - 1);
    }

    // sq は valid でなければならない
    [[nodiscard]] static constexpr int vertex_sq(const Sq sq) {
        return vertex_xy(sq.x(), sq.y());
    }

    // pt は持駒となりうる駒でなければならない
    [[nodiscard]] static constexpr int vertex_hand(const Piece pt) {
        return 81 + (static_cast<std::underlying_type_t<Piece>>(pt) - 2);
    }
};
