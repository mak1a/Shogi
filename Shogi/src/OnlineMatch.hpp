﻿
#pragma once
#include "Kyokumen.hpp"

// ゲームシーン
class OnlineMatch : public MyApp::Scene {
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
    bool m_isUseMessageWindow;

    enum class MessageState {
        Promote,  // 成るかどうかのウィンドウ
        Waited,   // 待ったボタンを押した際のウィンドウ
        Quit,     // 投了ボタンを押した際のウィンドウ
        None
    } m_messageState;

    /// <summary>
    /// メッセージボックスだとserviceを呼び出せなくなるので代わりのもの
    /// </summary>
    MyMessageBox m_messageBox;

    /// <summary>
    /// 待ったの申請時に表示するウィンドウ
    /// </summary>
    MyMessageWindow m_messageWindow;

    /// <summary>
    /// 成るか判断した後に使う
    /// </summary>
    Te m_te;

    // 手番を交代する
    void ChangeCurrentTurn() noexcept {
        s3d::AudioAsset(U"Piece").playOneShot(0.2);
        s3d::ClearPrint();

        m_candidateHands.clear();

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

        m_stackKyokumens.emplace_back(m_kyokumen);
        m_stackBoradSquares.emplace_back(m_boardSquares);
        m_stackHavingSelf.emplace_back(m_havingSelfKoma);
        m_stackHavingEnemy.emplace_back(m_havingEnemyKoma);
        m_stackPlacedPart.emplace_back(m_placedPart);
    }

    // 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    s3d::Array<KomaSquare> m_boardSquares;

    // 持ち駒（プレイヤー側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingSelfKoma;
    // 持ち駒（敵側）
    s3d::Array<s3d::Array<KomaSquare>> m_havingEnemyKoma;

    // 候補手
    s3d::Array<KomaSquare> m_candidateHands;

    // マウスで駒を保持
    s3d::Optional<KomaSquare> m_holdHand;

    // 駒を置いた部分の色をちょっと赤くするための変数
    s3d::Optional<KomaSquare> m_placedPart;

    // 待ったの為に局面クラスを保持
    s3d::Array<Kyokumen> m_stackKyokumens;

    s3d::Array<s3d::Array<KomaSquare>> m_stackBoradSquares;

    s3d::Array<s3d::Array<s3d::Array<KomaSquare>>> m_stackHavingSelf;
    s3d::Array<s3d::Array<s3d::Array<KomaSquare>>> m_stackHavingEnemy;

    s3d::Array<s3d::Optional<KomaSquare>> m_stackPlacedPart;

    void AddHoldKoma(KomaSquare& koma_);

    void RetractingMove() {
        if (m_stackKyokumens.size() <= 2) {
            return;
        }

        m_stackKyokumens.pop_back();
        m_stackKyokumens.pop_back();
        m_kyokumen = m_stackKyokumens.back();

        m_stackBoradSquares.pop_back();
        m_stackBoradSquares.pop_back();
        m_boardSquares.assign(m_stackBoradSquares.back().begin(), m_stackBoradSquares.back().end());

        m_stackHavingSelf.pop_back();
        m_stackHavingSelf.pop_back();
        m_havingSelfKoma.assign(m_stackHavingSelf.back().begin(), m_stackHavingSelf.back().end());

        m_stackHavingEnemy.pop_back();
        m_stackHavingEnemy.pop_back();
        m_havingEnemyKoma.assign(m_stackHavingEnemy.back().begin(), m_stackHavingEnemy.back().end());

        m_stackPlacedPart.pop_back();
        m_stackPlacedPart.pop_back();
        m_placedPart = m_stackPlacedPart.back();
    }

    void AddCandidateHand() {
        auto candidateHands = m_kyokumen.GetCandidateHand(m_holdHand.value().GetKomaType(),
                                                          static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10));

        for (const auto& te : candidateHands) {
            m_candidateHands << m_boardSquares[(9 - te.GetTo() / 10) + ((te.GetTo() % 10) - 1) * 9];
        }
    }

    void SendOpponent(const Te& te_);

    void result();

    void CustomEventAction(int playerNr, nByte eventCode, const ExitGames::Common::Object& eventContent) override;

    void LeaveRoomEventAction(int playerNr, bool isInactive) override;

    void DisconnectReturn() override;

    void AskPromoteKoma();

    void AskQuitGame();

    void AskWaited();

public:
    OnlineMatch(const InitData& init, const double shogiBan_ = 540.f, const double komaDai_ = 240.f);

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
