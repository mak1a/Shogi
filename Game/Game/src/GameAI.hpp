
# pragma once
#include"Kyokumen.hpp"
#include"Sikou.hpp"

class BanSelf {
private:
    // 将棋盤
    RectF m_shogiBan;
    // 敵の駒台
    RectF m_komaDaiEnemy;
    // 自分の駒台
    RectF m_komaDaiSelf;

    /// <summary>
    /// ストップウォッチ
    /// AIが考えてる風を出すためのもの
    /// </summary>
    Stopwatch m_thinkingTimer;

    Kyokumen m_kyokumen;

    Sikou m_sikouEnemy;
    Sikou m_sikouSelf;

    // どちらの順番か
    Turn m_turn;

    // 手番を交代する
    void ChangeCurrentTurn() noexcept {
        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
        m_thinkingTimer.restart();
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
    BanSelf(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;
    
    void SelfUpdate();

    void SelfAIUpdate();

    void EnemyUpdate();
    
    void Draw() const;

    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }
};

// ゲームシーン
class GameAI : public MyApp::Scene
{
private:
    BanSelf m_ban;
public:

	GameAI(const InitData& init);

	void update() override;

	void draw() const override;
};
