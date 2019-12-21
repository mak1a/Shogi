#pragma once

#include<Siv3D.hpp>
#include"Koma.hpp"


// 手のクラス
class Te {
private:
    // どこからどこへはそれぞれ1バイトで表せる
    // 詳しくはKyokumenクラスを参照
    uint32 m_from, m_to;
    
    // 動かした駒
    uint32 m_koma;
    
    // 取った駒
    uint32 m_capture;
    
    // 成/不成
    uint32 m_promote;
    
    // 将来の拡張用に使うらしい
    // 何に使うかは今のところわかりません()
    uint32 m_kind;
    
    // その手の仮評価
    int32 m_value;
    
public:
    [[nodiscard]] inline bool IsNull() const noexcept {
        return m_from == 0 && m_to == 0;
    }
    
    Te() = default;
    
    constexpr Te(const uint32 from_, const uint32 to_, const uint32 koma_, const uint32 capture_ = Emp,
                 const uint32 promote_ = 0, const uint32 kind_ = 0, const int32 value_ = 0) noexcept
        : m_from(from_)
        , m_to(to_)
        , m_koma(koma_)
        , m_capture(capture_)
        , m_promote(promote_)
        , m_kind(kind_)
        , m_value(value_) {}
    
    [[nodiscard]] uint32 GetFrom() const noexcept {
        return m_from;
    }
    [[nodiscard]] uint32 GetTo() const noexcept {
        return m_to;
    }
    [[nodiscard]] uint32 GetKoma() const noexcept {
        return m_koma;
    }
    [[nodiscard]] uint32 GetPromote() const noexcept {
        return m_capture;
    }
    
    // 手の同一性を比較したい時に使う
    // kindやvalueが違くても同じ手とみなす
    bool operator==(const Te& te_) const noexcept {
        return te_.GetFrom() == m_from && te_.GetTo() == m_to
            && te_.GetKoma() == m_koma && te_.GetPromote() == m_promote;
    }
};


class Kyokumen {
private:
    // ２次元配列だと遅いので、1次元配列を使う
    array<uint32, 11 * 11> m_ban;
    
    // 駒の利きを保持する
    array<uint32, 11 * 11> m_controlSelf, m_controlEnemy;
    
    // 持ち駒の数。
    array<uint32, 41> m_holdingKomas;
    
    // この局面の手数
    uint32 m_tesu;
    
    // 互いの王の位置
    uint32 m_kingSelfPos, m_kingEnemyPos;
    
    // 局面の評価値
    int32 m_value;
    
    Array<Te> m_teValid;
    
    array<uint32, 11 * 11> m_pin;
    
    void InitControl();
    
    [[nodiscard]] bool Uchifudume(const uint32 isSelfOrEnemy_, const uint32 to_);
    
    // 王の動く手の生成
    void MoveKing(const uint32 isSelfOrEnemy_, const uint32 kiki_);
    
    void MoveTo(const uint32 isSelfOrEnemy_, const uint32 to_);
    
    void PutTo(const uint32 isSelfOrEnemy_, const uint32 pos_);

    [[nodiscard]] uint32 CountControlSelf(const uint32 pos_);

    [[nodiscard]] uint32 CountControlEnemy(const uint32 pos_);

    [[nodiscard]] uint32 CountMove(const uint32 isSelfOrEnemy_, const uint32 pos_);

    void AddMoves(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 pin_, const int32 rPin_);
};
