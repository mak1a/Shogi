#pragma once

#include"Kyokumen.hpp"

enum class SearchType {
    MinMax,
    AlphaBeta,
    NegaAlphaBeta
};

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

    [[nodiscard]] uint64 GetHashVal() const noexcept {
        return m_hashVal;
    }
    void SetHashVal(const uint64 val_) noexcept {
        m_hashVal = val_;
    }

    [[nodiscard]] int32 GetValue() const noexcept {
        return m_value;
    }

    [[nodiscard]] ValueKind GetValueKind() const noexcept {
        return m_valueKind;
    }

    [[nodiscard]] uint32 GetTesu() const noexcept {
        return m_tesu;
    }

    [[nodiscard]] uint32 GetDepth() const noexcept {
        return m_depth;
    }

    [[nodiscard]] uint32 GetRemainDepth() const noexcept {
        return m_remainDepth;
    }
};

class Sikou {
private:
    /// <summary>
    /// Ç†ÇÈê[Ç≥Ç≈ÇÃç≈ëPéËÇï€éùÇ∑ÇÈÅB
    /// </summary>
    Array<Array<Te>> m_bestHands;

    /// <summary>
    /// ê[Ç≥ÇÃç≈ëÂ
    /// </summary>
    uint32 m_depthMax;
public:
    Sikou(const uint32 depth_) noexcept
        : m_depthMax(depth_) {
        m_bestHands.resize(m_depthMax + 1);
        for (auto& hands : m_bestHands) {
            hands.resize(m_depthMax + 1);
        }
    }

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept;

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_, const SearchType type_) noexcept;

    [[nodiscard]] int32 MinMax(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, const uint32 depth_) noexcept;

    [[nodiscard]] int32 AlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept;

    [[nodiscard]] int32 NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept;
};
