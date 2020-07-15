
#pragma once
#include "Kyokumen.hpp"

// ゲームシーン
class Game : public MyApp::Scene {
private:
    // 将棋盤
    s3d::RectF m_shogiBan;
    // 敵の駒台
    s3d::RectF m_komaDaiEnemy;
    // 自分の駒台
    s3d::RectF m_komaDaiSelf;

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
    s3d::RectF m_buttonWaited;

    /// <summary>
    /// 投了ボタン
    /// </summary>
    s3d::RectF m_buttonQuit;

    bool m_isUseMessageBox;

    /// <summary>
    /// メッセージボックスだとserviceを呼び出せなくなるので代わりのもの
    /// </summary>
    MyMessageBox m_promoteMessage;

    /// <summary>
    /// 成るか判断した後に使う
    /// </summary>
    Te m_te;

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
        }
        else {
            if (m_kyokumen.MakeLegalMoves(Enemy) <= 0) {
                // Print << U"プレイヤーの勝利！";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Player;
            }
        }

        m_stackKyokumens.emplace(m_kyokumen);
        m_stackBoradSquares.emplace(m_boardSquares);
        m_stackHavingSelf.emplace(m_havingSelfKoma);
        m_stackHavingEnemy.emplace(m_havingEnemyKoma);
        m_stackPlacedPart.emplace(m_placedPart);
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
        if (m_stackKyokumens.size() <= 2) {
            return;
        }

        m_stackKyokumens.pop();
        m_stackKyokumens.pop();
        m_kyokumen = m_stackKyokumens.top();

        m_stackBoradSquares.pop();
        m_stackBoradSquares.pop();
        m_boardSquares.assign(m_stackBoradSquares.top().begin(), m_stackBoradSquares.top().end());

        m_stackHavingSelf.pop();
        m_stackHavingSelf.pop();
        m_havingSelfKoma.assign(m_stackHavingSelf.top().begin(), m_stackHavingSelf.top().end());

        m_stackHavingEnemy.pop();
        m_stackHavingEnemy.pop();
        m_havingEnemyKoma.assign(m_stackHavingEnemy.top().begin(), m_stackHavingEnemy.top().end());

        m_stackPlacedPart.pop();
        m_stackPlacedPart.pop();
        m_placedPart = m_stackPlacedPart.top();
    }

    void SendOpponent(const Te& te_);

    void result();

    void CustomEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

    void LeaveRoomEventAction(int playerNr, bool isInactive) override;

    void DisconnectReturn() override;

    void ShowMessageBox();

public:
    Game(const InitData& init, const double shogiBan_ = 540.f, const double komaDai_ = 240.f);

    void SelfUpdate();

    void EnemyUpdate(const Te& te_);

    void Draw() const;

    void update() override;

    void draw() const override;

    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }

    [[nodiscard]] Winner GetWinner() const noexcept {
        return m_winner;
    }
};
