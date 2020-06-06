#pragma once

#include"Kyokumen.hpp"

enum class ValueKind {
    Exactly,
    Lower,
    Upper
};

class HashEntry {
private:
    uint64 m_hashVal;

    Te m_best;

    Te m_second;

    int32 m_value;

    ValueKind m_valueKind;

    uint32 m_tesu;

    uint32 m_depth;

    uint32 m_remainDepth;

public:
    constexpr HashEntry() noexcept
    : m_hashVal(0)
    , m_best(0)
    , m_second(0)
    , m_value(0)
    , m_valueKind(ValueKind::Exactly)
    , m_tesu(0)
    , m_depth(0)
    , m_remainDepth(0) {}

    [[nodiscard]] Te GetBest() const noexcept {
        return m_best;
    }

    [[nodiscard]] Te GetSecond() const noexcept {
        return m_second;
    }
    void InitSecond() noexcept {
        m_second = Te(0);
    }
    void ChangeSecondFromBest() noexcept {
        m_second = m_best;
    }
    void InitBest() noexcept {
        m_best = Te(0);
    }
    void ChangeBestFromBestHand(const Te& te_) noexcept {
        m_best = te_;
    }

    [[nodiscard]] uint64 GetHashVal() const noexcept {
        return m_hashVal;
    }
    void SetHashVal(const uint64 val_) noexcept {
        m_hashVal = val_;
    }

    [[nodiscard]] int32 GetValue() const noexcept {
        return m_value;
    }
    void SetValue(const int32 val_) noexcept {
        m_value = val_;
    }

    [[nodiscard]] ValueKind GetValueKind() const noexcept {
        return m_valueKind;
    }
    void SetValueKind(const ValueKind val_) noexcept {
        m_valueKind = val_;
    }

    [[nodiscard]] uint32 GetTesu() const noexcept {
        return m_tesu;
    }
    void SetTesu(const uint32 val_) noexcept {
        m_tesu = val_;
    }

    [[nodiscard]] uint32 GetDepth() const noexcept {
        return m_depth;
    }
    void SetDepth(const uint32 val_) noexcept {
        m_depth = val_;
    }

    [[nodiscard]] uint32 GetRemainDepth() const noexcept {
        return m_remainDepth;
    }
    void SetRemainDepth(const uint32 val_) noexcept {
        m_remainDepth = val_;
    }
};

class Sikou {
private:
    /// <summary>
    /// ある深さでの最善手を保持する。
    /// </summary>
    Array<Array<Te>> m_bestHands;

    /// <summary>
    /// 深さの最大
    /// </summary>
    uint32 m_depthMax;

    HashTable<uint64, HashEntry> m_hashTable;

    [[nodiscard]] uint32 MakeMoveFirst(const uint32 isSelfOrEnemy_, const uint32 depth_, Array<Te>& teValids_, Kyokumen k_);

    [[nodiscard]] int32 HashAdd(const int32 retVal_, const Kyokumen& kyokumen_, const int32 alpha_, const int32 beta_, const uint32 depth_, const uint32 depthMax_);
public:
    Sikou(const uint32 depth_) noexcept
    : m_depthMax(depth_) {
        m_bestHands.resize(m_depthMax+1);
        for (auto& hands : m_bestHands) {
            hands.resize(m_depthMax+1);
        }
    }

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept;

    //[[nodiscard]] int32 NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const bool itDeep_ = true) noexcept;
    [[nodiscard]] int32 NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const uint32 depthMax_ , const bool itDeep_ = true) noexcept;

    [[nodiscard]] int32 ITDeep(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept;
};
