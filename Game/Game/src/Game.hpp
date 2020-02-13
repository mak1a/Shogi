
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
    // 手番を交代する
    void ChangeCurrentTurn() noexcept {
        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
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
    
    void AddHoldKoma(KomaSquare& koma_);
public:
    Ban(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;
    
    void Update();
    
    void Draw() const;
    
    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }
};

// ゲームシーン
class Game : public MyApp::Scene
{
private:
    Ban m_ban;
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;
};
