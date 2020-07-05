
#pragma once
#include "SceneMaster.hpp"
#include "Board.hpp"
using std::array;

// シーンの名前
enum class State {
    Title,   // タイトルシーン
    Select,  // 設定シーン
    Game,    // 二人用プレイのシーン
    GameAI   // 一人用プレイのシーン
};

/// <summary>
/// ターン
/// </summary>
enum class Turn { Player, Enemy, Tsumi };

/// <summary>
/// 駒落ち
/// </summary>
enum class Handicap {
    Hirate,      // 平手
    Bishop,      // 角落ち
    Rook,        // 飛車落ち
    TwoPieces,   // 2枚落ち
    FourDrops,   // 4枚落ち
    SixDrops,    // 6枚落ち
    EightDrops,  // 8枚落ち
    TenDrops,    // 10枚落ち
    Custom       // カスタム
};

/// <summary>
/// 駒落ちする方
/// </summary>
enum class Elegance { Player, Enemy };

enum class PhotonState { Master, Join, None };

/// <summary>
/// ゲームデータ
/// </summary>
struct GameData {
    /// <summary>
    /// 先手
    /// </summary>
    Turn firstMove = Turn::Player;

    /// <summary>
    /// 一人用か二人用プレイの判定
    /// </summary>
    State gameState = State::GameAI;

    /// <summary>
    /// 駒落ち
    /// </summary>
    Handicap handicap = Handicap::Hirate;

    /// <summary>
    /// 駒落ちする方
    /// </summary>
    Elegance elegance = Elegance::Player;

    /// <summary>
    /// 思考の深さ
    /// </summary>
    uint32 depthMax = 0;

    /// <summary>
    /// 持ち駒
    /// </summary>
    array<uint32, 40> motigomas;

    PhotonState photonState = PhotonState::None;

private:
    /// <summary>
    /// 盤面がカスタムの場合、ここに格納する
    /// </summary>
    array<array<uint32, 9>, 9> m_customKomas = Board::Custom;

    ExitGames::Common::Hashtable m_hashTable;

public:
    GameData() noexcept {
        m_hashTable.put(L"version", L"1.0");

        for (size_t y{}; y < 9; ++y) {
            for (size_t x{}; x < 9; ++x) {
                m_customKomas[y][x] = Board::Custom[y][x];
            }
        }

        motigomas.fill(0);
    }

    /// <summary>
    /// 選択した盤を出力する
    /// </summary>
    array<array<uint32, 9>, 9> GetBoard() const noexcept {
        if (elegance == Elegance::Player) {
            switch (handicap) {
            case Handicap::Hirate:
                return Board::Myself::Hirate;
            case Handicap::Bishop:
                return Board::Myself::Bishop;
            case Handicap::Rook:
                return Board::Myself::Rook;
            case Handicap::TwoPieces:
                return Board::Myself::TwoPieces;
            case Handicap::FourDrops:
                return Board::Myself::FourDrops;
            case Handicap::SixDrops:
                return Board::Myself::SixDrops;
            case Handicap::EightDrops:
                return Board::Myself::EightDrops;
            case Handicap::TenDrops:
                return Board::Myself::TenDrops;
            case Handicap::Custom:
                return m_customKomas;
            }
        }
        else {
            switch (handicap) {
            case Handicap::Hirate:
                return Board::Opponent::Hirate;
            case Handicap::Bishop:
                return Board::Opponent::Bishop;
            case Handicap::Rook:
                return Board::Opponent::Rook;
            case Handicap::TwoPieces:
                return Board::Opponent::TwoPieces;
            case Handicap::FourDrops:
                return Board::Opponent::FourDrops;
            case Handicap::SixDrops:
                return Board::Opponent::SixDrops;
            case Handicap::EightDrops:
                return Board::Opponent::EightDrops;
            case Handicap::TenDrops:
                return Board::Opponent::TenDrops;
            case Handicap::Custom:
                return m_customKomas;
            }
        }
    }

    void SetCustomBoard(const array<array<uint32, 9>, 9>& custom_) {
        for (size_t y{}; y < 9; ++y) {
            for (size_t x{}; x < 9; ++x) {
                m_customKomas[y][x] = custom_[y][x];
            }
        }
    }

    ExitGames::Common::Hashtable& GetCustomProperties() {
        return m_hashTable;
    }
};

/// <summary>
/// シーン管理クラス
/// </summary>
using MyApp = shogi::SceneMaster<State, GameData>;


/// <summary>
/// 勝者
/// </summary>
enum class Winner { Player, Enemy, Sennitite };
