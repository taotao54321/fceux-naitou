#include <array>
#include <cassert>
#include <tuple>
#include <type_traits>
#include <utility>

#include "naitou.hpp"
#include "prelude.hpp"
#include "util.hpp"

namespace {

constexpr u8 CELL_BYTE_EMPTY = 0;
constexpr u8 CELL_BYTE_WALL = 99;

Piece pcom2pt(u8 pcom) {
    return static_cast<Piece>(pcom - 15);
}

Piece phum2pt(u8 phum) {
    return static_cast<Piece>(phum);
}

Hand read_hand(Core& core, u16 addr) {
    std::array<u8, 7> buf;
    core.read_bytes(addr, buf);

    Hand hand;
    for (const auto i : IRANGE(7)) {
        const auto pt = pts_hand()[i];
        hand[pt] = buf[i];
    }

    return hand;
}

std::array<std::array<Buttons, 88>, 88> traveller_graph() {
    constexpr Buttons OP_UL = Buttons {}.U(true).L(true);
    constexpr Buttons OP_U = Buttons {}.U(true);
    constexpr Buttons OP_UR = Buttons {}.U(true).R(true);
    constexpr Buttons OP_L = Buttons {}.L(true);
    constexpr Buttons OP_R = Buttons {}.R(true);
    constexpr Buttons OP_DL = Buttons {}.D(true).L(true);
    constexpr Buttons OP_D = Buttons {}.D(true);
    constexpr Buttons OP_DR = Buttons {}.D(true).R(true);

    // (dy, dx, buttons)
    constexpr std::array<std::tuple<int, int, Buttons>, 8> NEIGHBORS = { {
        { -1, -1, OP_UL },
        { -1, 0, OP_U },
        { -1, 1, OP_UR },
        { 0, -1, OP_L },
        { 0, 1, OP_R },
        { 1, -1, OP_DL },
        { 1, 0, OP_D },
        { 1, 1, OP_DR },
    } };

    std::array<std::array<Buttons, 88>, 88> graph {};

    // 盤上のマス間の接続
    for (const auto y : IRANGE(1, 9 + 1)) {
        for (const auto x : IRANGE(1, 9 + 1)) {
            for (const auto [dy, dx, buttons] : NEIGHBORS) {
                const auto yy = y + dy;
                const auto xx = x + dx;
                if (yy < 1 || 9 < yy || xx < 1 || 9 < xx) continue;
                const auto i = Traveller::vertex_xy(x, y);
                const auto j = Traveller::vertex_xy(xx, yy);
                graph[i][j] = buttons;
            }
        }
    }

    // 盤から持駒エリアへの接続
    for (const auto y : IRANGE(1, 9 + 1)) {
        const auto i = Traveller::vertex_xy(9, y);
        graph[i][Traveller::vertex_hand(Piece::ROOK)] = OP_R;
        graph[i][Traveller::vertex_hand(Piece::SILVER)] = OP_DR;
    }

    // 持駒エリアから盤への接続
    {
        const auto i = Traveller::vertex_hand(Piece::ROOK);
        graph[i][Traveller::vertex_xy(9, 6)] = OP_UL;
        graph[i][Traveller::vertex_xy(9, 7)] = OP_L;
        graph[i][Traveller::vertex_xy(9, 8)] = OP_DL;
    }
    {
        const auto i = Traveller::vertex_hand(Piece::SILVER);
        graph[i][Traveller::vertex_xy(9, 7)] = OP_UL;
        graph[i][Traveller::vertex_xy(9, 8)] = OP_L;
        graph[i][Traveller::vertex_xy(9, 9)] = OP_DL;
    }
    {
        const auto i = Traveller::vertex_hand(Piece::PAWN);
        graph[i][Traveller::vertex_xy(9, 8)] = OP_UL;
        graph[i][Traveller::vertex_xy(9, 9)] = OP_L;
    }

    // 持駒エリア内の接続
    {
        const auto add_edge = [&graph](Piece pt1, Piece pt2, Buttons buttons) {
            const auto i = Traveller::vertex_hand(pt1);
            const auto j = Traveller::vertex_hand(pt2);
            graph[i][j] = buttons;
        };

        add_edge(Piece::ROOK, Piece::BISHOP, OP_R);
        add_edge(Piece::ROOK, Piece::SILVER, OP_D);
        add_edge(Piece::ROOK, Piece::KNIGHT, OP_DR);

        add_edge(Piece::BISHOP, Piece::ROOK, OP_L);
        add_edge(Piece::BISHOP, Piece::GOLD, OP_R);
        add_edge(Piece::BISHOP, Piece::SILVER, OP_DL);
        add_edge(Piece::BISHOP, Piece::KNIGHT, OP_D);
        add_edge(Piece::BISHOP, Piece::LANCE, OP_DR);

        add_edge(Piece::GOLD, Piece::BISHOP, OP_L);
        add_edge(Piece::GOLD, Piece::SILVER, OP_R);
        add_edge(Piece::GOLD, Piece::KNIGHT, OP_DL);
        add_edge(Piece::GOLD, Piece::LANCE, OP_D);

        add_edge(Piece::SILVER, Piece::ROOK, OP_U);
        add_edge(Piece::SILVER, Piece::BISHOP, OP_UR);
        add_edge(Piece::SILVER, Piece::KNIGHT, OP_R);
        add_edge(Piece::SILVER, Piece::PAWN, OP_D);

        add_edge(Piece::KNIGHT, Piece::ROOK, OP_UL);
        add_edge(Piece::KNIGHT, Piece::BISHOP, OP_U);
        add_edge(Piece::KNIGHT, Piece::GOLD, OP_UR);
        add_edge(Piece::KNIGHT, Piece::SILVER, OP_L);
        add_edge(Piece::KNIGHT, Piece::LANCE, OP_R);
        add_edge(Piece::KNIGHT, Piece::PAWN, OP_DL);

        add_edge(Piece::LANCE, Piece::BISHOP, OP_UL);
        add_edge(Piece::LANCE, Piece::GOLD, OP_U);
        add_edge(Piece::LANCE, Piece::KNIGHT, OP_L);
        add_edge(Piece::LANCE, Piece::PAWN, OP_R);

        add_edge(Piece::PAWN, Piece::SILVER, OP_U);
        add_edge(Piece::PAWN, Piece::KNIGHT, OP_UR);
    }

    return graph;
}

} // anonymous namespace

const Cell& Board::operator[](Sq sq) const {
    return cells_[sq.get()];
}

Cell& Board::operator[](Sq sq) {
    return cells_[sq.get()];
}

u8 Hand::operator[](Piece pt) const {
    return counts_[static_cast<std::underlying_type_t<Piece>>(pt) - 2];
}

u8& Hand::operator[](Piece pt) {
    return counts_[static_cast<std::underlying_type_t<Piece>>(pt) - 2];
}

Position::Position(Side side, const Board& board, const Hand& hand_com, const Hand& hand_hum)
    : side_(side)
    , board_(board)
    , hand_com_(hand_com)
    , hand_hum_(hand_hum) {}

Side read_side(Core& core) {
    return core.read_u8(0x77) == 0 ? Side::COM : Side::HUM;
}

Board read_board(Core& core) {
    std::array<u8, 11 * 11> buf_com;
    std::array<u8, 11 * 11> buf_hum;
    core.read_bytes(0x49B, buf_com);
    core.read_bytes(0x3A9, buf_hum);

    Board board;
    for (const auto sq : Sq::sqs_ok()) {
        u8 bcom = buf_com[sq.get()];
        u8 bhum = buf_hum[sq.get()];
        if (bcom == CELL_BYTE_WALL && bhum == CELL_BYTE_WALL) {
            board[sq] = CellWall {};
        }
        else if (bcom == CELL_BYTE_EMPTY && bhum == CELL_BYTE_EMPTY) {
            board[sq] = CellEmpty {};
        }
        else if (bcom != CELL_BYTE_WALL && bhum == CELL_BYTE_EMPTY) {
            board[sq] = CellCom { pcom2pt(bcom) };
        }
        else if (bcom == CELL_BYTE_EMPTY && bhum != CELL_BYTE_WALL) {
            board[sq] = CellHum { phum2pt(bhum) };
        }
        else {
            PANIC("read_board(): unreachable (bcom={}, bhum={})", bcom, bhum);
        }
    }

    return board;
}

Hand read_hand_com(Core& core) {
    return read_hand(core, 0x594);
}

Hand read_hand_hum(Core& core) {
    return read_hand(core, 0x58D);
}

Position read_position(Core& core) {
    const auto side = read_side(core);
    const auto board = read_board(core);
    const auto hand_com = read_hand_com(core);
    const auto hand_hum = read_hand_hum(core);

    return Position(side, board, hand_com, hand_hum);
}

Sq read_cursor(Core& core) {
    const auto x = core.read_u8(0xD6);
    const auto y = core.read_u8(0xD7);
    return Sq::from_xy(x, y);
}

Traveller Traveller::calc() {
    constexpr u8 INF = 100;

    const auto graph = traveller_graph();

    // i から j へ最短経路で行くときの (次に辿るべき点, 操作)
    // i==j なら (i, Buttons{})
    // 到達不能なら (-1, Buttons{})
    std::array<std::array<std::pair<int, Buttons>, 88>, 88> nxt;
    for (const auto i : IRANGE(88)) {
        for (const auto j : IRANGE(88)) {
            if (i == j) {
                nxt[i][j] = { i, Buttons {} };
                continue;
            }
            const auto buttons = graph[i][j];
            if (buttons.is_empty())
                nxt[i][j] = { -1, buttons };
            else
                nxt[i][j] = { j, buttons };
        }
    }

    std::array<std::array<u8, 88>, 88> dist;
    for (const auto i : IRANGE(88)) {
        for (const auto j : IRANGE(88)) {
            if (i == j)
                dist[i][j] = 0;
            else
                dist[i][j] = graph[i][j].is_empty() ? INF : 1;
        }
    }

    // Warshall-Floyd
    for (const auto k : IRANGE(88)) {
        for (const auto i : IRANGE(88)) {
            if (dist[i][k] == INF) continue;
            for (const auto j : IRANGE(88)) {
                if (dist[k][j] == INF) continue;
                if (CHMIN(dist[i][j], dist[i][k] + dist[k][j]))
                    nxt[i][j] = nxt[i][k];
            }
        }
    }

    // 経路復元
    Traveller tra;
    for (const auto src : IRANGE(88)) {
        for (const auto dst : IRANGE(88)) {
            if (dist[src][dst] == INF) continue;
            auto& entry = tra.entrys_[src][dst];
            for (auto v = src; v != dst;) {
                const auto [nxt_v, nxt_buttons] = nxt[v][dst];
                entry.seq[entry.size++] = nxt_buttons;
                v = nxt_v;
            }
        }
    }

    return tra;
}

std::pair<u8, const Buttons*> Traveller::query(int src, int dst) const {
    const auto& entry = entrys_[src][dst];
    return { entry.size, entry.seq.data() };
}
