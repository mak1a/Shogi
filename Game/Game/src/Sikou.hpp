#pragma once

#include"Kyokumen.hpp"

enum class SearchType {
    MinMax,
    AlphaBeta,
    NegaAlphaBeta
};

class Sikou {
private:
    /// <summary>
    /// ある深さでの最善手を保持する。
    /// </summary>
    std::array<std::array<Te, 32>, 32> m_bestHands;

    /// <summary>
    /// 深さの最大
    /// </summary>
    uint32 m_depthMax;
public:
    Sikou(const uint32 depth_) noexcept
    : m_depthMax(depth_) {}

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept;

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_, const SearchType type_) noexcept;

    [[nodiscard]] int32 MinMax(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, const uint32 depth_) noexcept;

    [[nodiscard]] int32 AlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept;

    [[nodiscard]] int32 NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept;
};
