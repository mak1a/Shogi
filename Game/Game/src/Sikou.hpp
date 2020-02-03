#pragma once

#include"Kyokumen.hpp"

class Sikou {
private:
    /// <summary>
    /// ある深さでの最善手を保持する。
    /// </summary>
    std::array<std::array<Te, 32>, 32> m_bestHands;
public:
    Sikou() noexcept;

    [[nodiscard]] Te Think(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_) noexcept;

    [[nodiscard]] int32 MinMax(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, const uint32 depth_, const uint32 depthMax_) noexcept;

    [[nodiscard]] int32 AlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const uint32 depthMax_) noexcept;

    [[nodiscard]] int32 NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const uint32 depthMax_) noexcept;
};
