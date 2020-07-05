
#pragma once
#include "Kyokumen.hpp"
#include "Sikou.hpp"

class BanSelf {
private:
    // 将棋盤
    s3d::RectF m_shogiBan;
    // 敵の駒台
    s3d::RectF m_komaDaiEnemy;
    // 自分の駒台
    s3d::RectF m_komaDaiSelf;

    /// <summary>
    /// ストップウォッチ
    /// AIが考えてる風を出すためのもの
    /// </summary>
    s3d::Stopwatch m_thinkingTimer;

    Kyokumen m_kyokumen;

    Sikou m_sikouEnemy;
    Sikou m_sikouSelf;

    // どちらの順番か
    Turn m_turn;

    /// <summary>
    /// どちらが勝ったか
    /// </summary>
    Winner m_winner = Winner::Player;

    // プレイヤーが後手ならtrue
    bool m_isBehind = false;

    /// <summary>
    /// 待ったボタン
    /// </summary>
    s3d::RectF m_buttonWaited;

    /// <summary>
    /// 投了ボタン
    /// </summary>
    s3d::RectF m_buttonQuit;

    // 手番を交代する
    void ChangeCurrentTurn() noexcept {
        s3d::AudioAsset(U"Piece").playOneShot(0.2);

        s3d::ClearPrint();

        if (m_kyokumen.IsSennitite()) {
            m_winner = m_kyokumen.IsContinuous(m_turn);
            m_turn = Turn::Tsumi;
            return;
        }

        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
        if (GetTurn() == Turn::Player) {
            if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
                // Print << U"You Lose!";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Enemy;
            }

            m_stackKyokumens.emplace(m_kyokumen);
            m_stackBoradSquares.emplace(m_boardSquares);
            m_stackHavingSelf.emplace(m_havingSelfKoma);
            m_stackHavingEnemy.emplace(m_havingEnemyKoma);
            m_stackPlacedPart.emplace(m_placedPart);
        }
        else {
            if (m_kyokumen.MakeLegalMoves(Enemy) <= 0) {
                // Print << U"プレイヤーの勝利！";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Player;
            }
        }
        m_thinkingTimer.restart();
    }

    // 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    s3d::Array<KomaSquare> m_boardSquares;

    // 持ち駒（プレイヤー側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingSelfKoma;
    // 持ち駒（敵側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingEnemyKoma;

    // マウスで駒を保持
    s3d::Optional<KomaSquare> m_holdHand;

    // 駒を置いた部分の色をちょっと赤くするための変数
    s3d::Optional<KomaSquare> m_placedPart;

    // 待ったの為に局面クラスを保持
    std::stack<Kyokumen> m_stackKyokumens;

    std::stack<s3d::Array<KomaSquare>> m_stackBoradSquares;

    std::stack<s3d::Array<s3d::Array<KomaSquare>>> m_stackHavingSelf;
    std::stack<s3d::Array<s3d::Array<KomaSquare>>> m_stackHavingEnemy;

    std::stack<s3d::Optional<KomaSquare>> m_stackPlacedPart;

    void AddHoldKoma(KomaSquare& koma_);

    void RetractingMove() {
        if (m_stackKyokumens.size() <= 1) {
            return;
        }
        if (m_stackKyokumens.size() == 2 && m_isBehind) {
            return;
        }

        m_stackKyokumens.pop();
        m_kyokumen = m_stackKyokumens.top();

        m_stackBoradSquares.pop();
        m_boardSquares.assign(m_stackBoradSquares.top().begin(), m_stackBoradSquares.top().end());

        m_stackHavingSelf.pop();
        m_havingSelfKoma.assign(m_stackHavingSelf.top().begin(), m_stackHavingSelf.top().end());

        m_stackHavingEnemy.pop();
        m_havingEnemyKoma.assign(m_stackHavingEnemy.top().begin(), m_stackHavingEnemy.top().end());

        m_stackPlacedPart.pop();
        m_placedPart = m_stackPlacedPart.top();
    }

public:
    BanSelf(const array<array<uint32, 9>, 9>& iniKyokumen_,
            const array<uint32, 40>& motigomas_,
            const Turn& turn_,
            const uint32 sikouDepth_,
            const double shogiBan_ = 540.f,
            const double komaDai_ = 240.f) noexcept;

    void SelfUpdate();

    void SelfAIUpdate();

    void EnemyUpdate();

    void Draw() const;

    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }

    [[nodiscard]] Winner GetWinner() const noexcept {
        return m_winner;
    }
};

// ゲームシーン
class GameAI : public MyApp::Scene {
private:
    BanSelf m_ban;

    void result();

public:
    GameAI(const InitData& init);

    void update() override;

    void draw() const override;
};
