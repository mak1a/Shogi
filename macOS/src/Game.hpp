
# pragma once
#include"Kyokumen.hpp"

class Ban {
private:
    // 将棋盤
    RectF m_shogiBan;
    // 敵の駒台
    RectF m_komaDaiEnemy;
    // 自分の駒台
    RectF m_komaDaiSelf;

    Kyokumen m_kyokumen;

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
    RectF m_buttonWaited;

    /// <summary>
    /// 投了ボタン
    /// </summary>
    RectF m_buttonQuit;

    // 手番を交代する
    void ChangeCurrentTurn() noexcept {
        AudioAsset(U"Piece").playOneShot(0.2);
        ClearPrint();

        if (m_kyokumen.IsSennitite()) {
            m_winner = m_kyokumen.IsContinuous(m_turn);
            m_turn = Turn::Tsumi;
            return;
        }

        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
        if (GetTurn() == Turn::Player) {
            if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
                //Print << U"You Lose!";
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
                //Print << U"プレイヤーの勝利！";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Player;
            }
        }
    }
    
    // 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    Array<KomaSquare> m_boardSquares;
    
    // 持ち駒（プレイヤー側）
    Array<Array<KomaSquare>> m_havingSelfKoma;
    // 持ち駒（敵側）
    Array<Array<KomaSquare>> m_havingEnemyKoma;
    
    // マウスで駒を保持
    Optional<KomaSquare> m_holdHand;

    // 駒を置いた部分の色をちょっと赤くするための変数
    Optional<KomaSquare> m_placedPart;

    // 待ったの為に局面クラスを保持
    std::stack<Kyokumen> m_stackKyokumens;

    std::stack<Array<KomaSquare>> m_stackBoradSquares;

    std::stack<Array<Array<KomaSquare>>> m_stackHavingSelf;
    std::stack<Array<Array<KomaSquare>>> m_stackHavingEnemy;

    std::stack<Optional<KomaSquare>> m_stackPlacedPart;
    
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
    Ban(const array<array<uint32, 9>, 9>& iniKyokumen_, const Turn& turn_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;
    
    void Update();
    
    void Draw() const;
    
    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }

    [[nodiscard]] Winner GetWinner() const noexcept {
        return m_winner;
    }
};

// ゲームシーン
class Game : public MyApp::Scene {
private:
    Ban m_ban;

    void result();
public:

    Game(const InitData& init);

    void update() override;

    void draw() const override;
};
