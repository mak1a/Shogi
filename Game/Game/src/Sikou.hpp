#pragma once

#include"Kyokumen.hpp"

class Sikou {
public:
    constexpr Sikou() noexcept {}

    [[nodiscard]] inline Te Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept {
        uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
        Array<Te> teValids{kyokumen_.GetTeValids()};

        for (const auto uint32 : step(20)) {
            teValids.shuffle();
        }

        uint32 best{};
        int32 bestVal{(isSelfOrEnemy_ == Self) ? -1000000 : +1000000};

        for (uint32 i{}; i < teNum; ++i) {
            Kyokumen k{kyokumen_};
            k.Move(isSelfOrEnemy_, teValids[i]);

            if (isSelfOrEnemy_ == Self && bestVal < k.GetValue() + k.BestEval(Enemy)) {
                bestVal = k.GetValue() + k.BestEval(Enemy);
                best = i;
            }
            if (isSelfOrEnemy_ == Enemy && bestVal > k.GetValue() + k.BestEval(Self)) {
                bestVal = k.GetValue() + k.BestEval(Self);
                best = i;
            }
        }

        return teValids[best];
    }
};
