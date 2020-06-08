#pragma once

#include"Koma.hpp"
#include<bitset>


/// <summary>
/// 手のクラス
/// </summary>
class Te {
private:
    /// <summary>
    /// どこからどこへはそれぞれ1バイトで表せる
    /// </summary>
    /// <summary>
    /// 詳しくはKyokumenクラスを参照
    /// </summary>
    uint32 m_from, m_to;
    
    /// <summary>
    /// 動かした駒
    /// </summary>
    uint32 m_koma;
    
    /// <summary>
    /// 取った駒
    /// </summary>
    uint32 m_capture;
    
    // 成/不成
    uint32 m_promote;
    
    // 将来の拡張用に使うらしい
    // 何に使うかは今のところわかりません()
    uint32 m_kind;
    
    // その手の仮評価
    int32 m_value;
    
public:
    [[nodiscard]] bool IsNull() const noexcept {
        return m_from == 0 && m_to == 0;
    }
    
    Te() = default;
    
    constexpr Te(const uint32 from_, const uint32 to_, const uint32 koma_, const uint32 capture_ = Emp,
                 const uint32 promote_ = 0, const uint32 kind_ = 0, const int32 value_ = 0) noexcept
        : m_from(from_)
        , m_to(to_)
        , m_capture(capture_)
        , m_promote(promote_)
        , m_koma((promote_ == 0) ? koma_ : koma_ | Promote)
        , m_kind(kind_)
        , m_value(value_) {}
    
    constexpr Te(int32 i) noexcept
    : Te(0, 0, 0) {}
    
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
        return m_promote;
    }
    void SetPromote(const uint32 promote_) noexcept {
        m_promote = promote_;
        m_koma = (m_promote == 0) ? m_koma : m_koma | Promote;
    }
    [[nodiscard]] uint32 GetCapture() const noexcept {
        return m_capture;
    }
    void SetCapture(const uint32 capture_) noexcept {
        m_capture = capture_;
    }
    [[nodiscard]] uint32 GetKind() const noexcept {
        return m_kind;
    }
    [[nodiscard]] uint32 GetValue() const noexcept {
        return m_value;
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
    /// <summary>
    /// ２次元配列だと遅いので、1次元配列を使う
    /// </summary>
    array<uint32, 11 * 11> m_ban;
    
    // 駒の利きを保持する
    array<std::bitset<28>, 11 * 11> m_controlSelf, m_controlEnemy;
    
    // 持ち駒の数。
    array<uint32, 40> m_holdingKomas;
    
    // この局面の手数
    uint32 m_tesu;
    
    // 互いの王の位置
    uint32 m_kingSelfPos, m_kingEnemyPos;
    
    // 局面の評価値
    int32 m_value;
    
    Array<Te> m_teValids;
    
    array<uint32, 11 * 11> m_pin;
    
    uint64 m_kyokumenHashVal;
    uint64 m_handHashVal;
    uint64 m_hashVal;

    HashTable<uint64, uint32> m_hashHistory;
    Array<std::pair<std::bitset<28>, uint64>> m_outeHistory;
    
    void InitControl();
    
    /// <summary>
    /// 打ち歩詰めの判定
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=to_>駒を置く場所</param>
    /// <returns>打ち歩詰めか</returns>
    [[nodiscard]] bool Uchifudume(const uint32 isSelfOrEnemy_, const uint32 to_);

    /// <summary>
    /// 打ち歩詰めの判定
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=to_>駒を置く場所</param>
    /// <param name=pin_>ピン</param>
    /// <returns>打ち歩詰めか</returns>
    [[nodiscard]] bool Uchifudume(const uint32 isSelfOrEnemy_, const uint32 to_, array<uint32, 11 * 11>& pin_);
    
    /// <summary>
    /// 王を利きがない場所に動かす手の生成
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=kiki_>利き</param>
    void MoveKing(const uint32 isSelfOrEnemy_, const std::bitset<28>& kiki_);
    
    /// <summary>
    /// 指定の場所に動く手の生成
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=to_>指定した場所</param>
    void MoveTo(const uint32 isSelfOrEnemy_, const uint32 to_);
    
    /// <summary>
    /// 指定した場所に駒を打つ手の生成
    /// </summary>
    /// <param name=isSelfOrEnemy>手番</param>
    /// <param name=pos_>指定した場所</param>
    void PutTo(const uint32 isSelfOrEnemy_, const uint32 pos_);

    /// <summary>
    /// 引数で指定した場所の利き情報を作成
    /// 打ち歩詰めのチェック等、駒を仮に置いてみて何かする時に使用
    /// </summary>
    /// <param name=pos_>指定した場所</param>
    /// <returns>利き情報</returns>
    [[nodiscard]] std::bitset<28> CountControlSelf(const uint32 pos_);

    /// <summary>
    /// 引数で指定した場所の利き情報を作成
    /// 打ち歩詰めのチェック等、駒を仮に置いてみて何かする時に使用
    /// </summary>
    /// <param name=pos_>指定した場所</param>
    /// <returns>利き情報</returns>
    [[nodiscard]] std::bitset<28> CountControlEnemy(const uint32 pos_);

    /// <summary>
    /// 指定した場所に移動できる駒を利き情報にして返す
    /// ピンされてる駒はピンの方向にしか動けない
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=pos_>指定した場所</param>
    /// <returns>利き情報</returns>
    [[nodiscard]] std::bitset<28> CountMove(const uint32 isSelfOrEnemy_, const uint32 pos_);

    /// <summary>
    /// 指定した場所に移動できる駒を利き情報にして返す
    /// ピンされてる駒はピンの方向にしか動けない
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=pos_>指定した場所</param>
    /// <param name=pin_>ピン</param>
    /// <returns>利き情報</returns>
    [[nodiscard]] std::bitset<28> CountMove(const uint32 isSelfOrEnemy_, const uint32 pos_, array<uint32, 11 * 11>& pin_);

    /// <summary>
    /// 盤面のfrom_ に設定した駒を動かす手を生成する
    /// </summary>
    void AddMoves(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 pin_, const int32 rPin_ = 0);

    /// <summary>
    /// 飛車、角、香車の動く手を生成
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=from_>動かす前の場所</param>
    /// <param name=diff_>動かす方向との差分</param>
    /// <param name=pin_>利きの方向</param>
    /// <param name=rPin_>利きの方向</param>
    void AddStraight(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 dir_, const int32 pin_, const int32 rPin_ = 0);

    /// <summary>
    /// 手の生成
    /// 成り、不成も考慮する
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=from_>動かす前の場所</param>
    /// <param name=diff_>動かす方向との差分</param>
    /// <param name=pin_>利きの方向</param>
    /// <param name=rPin_>利きの方向</param>
    void AddMove(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 diff_, const int32 pin_, const int32 rPin_ = 0);

    /// <summary>
    /// 有効手かどうか
    /// </summary>
    /// <param name=te_>手</param>
    /// <returns>有効手か</returns>
    bool IsCorrectMove(Te& te_);

    int32 EvalMin(Array<Te>& moveSelf_, Array<Te>& moveEnemy_);

    int32 EvalMax(Array<Te>& moveSelf_, Array<Te>& moveEnemy_);

    int32 Eval(const uint32 pos_);

    [[nodiscard]] inline uint32 Search(uint32 pos_, int32 dir_) const noexcept {
        do {
            if (static_cast<int32>(pos_) + dir_ < 0) {
                break;
            }
            pos_ += dir_;
        } 
        while (m_ban[pos_] == Empty);
        
        return pos_;
    }

    /// <summary>
    /// ピン(動かすと王を取られるので動きが制限される駒)の状態の設定
    /// </summary>
    void MakePinInfo();

    /// <summary>
    /// ピン(動かすと王を取られるので動きが制限される駒)の状態の設定
    /// ピンは引数で指定
    /// </summary>
    /// <param name=pin_>ピン</param>
    void MakePinInfo(array<uint32, 11 * 11>& pin_);

    /// <summary>
    /// 王手を受ける手を生成
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=control_>利き</param>
    /// <returns>有効手の数</returns>
    uint32 AntiCheck(const uint32 isSelfOrEnemy_, const std::bitset<28>& control_);

public:
    Kyokumen() = default;
    
    Kyokumen(const uint32 tesu_, const array<array<uint32, 9>, 9>& board_, const array<uint32, 40>& motigoma_ = array<uint32, 40>()) noexcept;

    /// <summary>
    /// 有効"ではない"手かどうか判定
    /// </summary>
    /// <param name=te_>手</param>
    /// <returns>有効"ではない"かどうか</returns>
    [[nodiscard]] inline bool IsIllegal(const Te& te_) const noexcept {
        for (const auto& te : m_teValids) {
            if (te_ == te) {
                return false;
            }
        }

        return true;
    }

    [[nodiscard]] Array<Te> GetTeValids() const noexcept {
        return m_teValids;
    }
    [[nodiscard]] int32 GetValue() const noexcept {
        return m_value;
    }
    [[nodiscard]] uint64 GetHashVal() const noexcept {
        return m_hashVal;
    }
    [[nodiscard]] uint32 GetTesu() const noexcept {
        return m_tesu;
    }

    [[nodiscard]] bool IsOute(const Turn& turn_) const noexcept {
        return ((turn_== Turn::Player)
                    ? m_controlEnemy[m_kingSelfPos].any()
                    : m_controlSelf[m_kingEnemyPos].any());
    }

    /// <summary>
    /// 駒の動ける場所を全て生成する
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <returns>動ける駒の数</returns>
    uint32 MakeLegalMoves(const uint32 isSelfOrEnemy_);

    /// <summary>
    /// 手を指す
    /// </summary>
    /// <param name=isSelfOrEnemy_>手番</param>
    /// <param name=te_>動かす駒</param>
    void Move(const uint32 isSelfOrEnemy_, const Te& te_);

    int32 BestEval(const uint32 isSelfOrEnemy_);

    [[nodiscard]] bool IsLegalMove(const uint32 isSelfOrEnemy_, Te& te_);

    /// <summary>
    /// 千日手かどうかを判断する
    /// </summary>
    /// <returns>千日手かどうか</returns>
    [[nodiscard]] bool IsSennitite() const noexcept {
        if (m_hashHistory.empty()) {
            return false;
        }

        return (std::max_element(m_hashHistory.begin(), m_hashHistory.end(),
            [](const auto& a, const auto& b) -> bool {
                return (a.second < b.second);
            }
        )->second >= 4);
    }

    [[nodiscard]] Winner IsContinuous(const Turn& turn_) {
        uint32 sennitite{};

        for (uint32 i{m_tesu}; (i > 0 && sennitite <= 3); i -= 2) {
            if (m_outeHistory[i].first.none()) {
                break;
            }

            if (m_outeHistory[i].second == m_hashVal) {
                ++sennitite;
            }
        }

        if (sennitite < 4) {
            return Winner::Sennitite;
        }

        return ((turn_==Turn::Player)?Winner::Enemy:Winner::Player);
    }
};
