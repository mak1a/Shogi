#include"Sikou.hpp"


uint32 Sikou::MakeMoveFirst(const uint32 isSelfOrEnemy_, const uint32 depth_, Array<Te>& teValids_, Kyokumen k_) {
    if (m_hashTable[k_.GetHashVal()].GetHashVal() != k_.GetHashVal()) {
        /// <summary>
        /// ‹Ç–Ê‚ªˆê’v‚µ‚Ä‚È‚¢
        /// </summary>
        return 0;
    }

    if (m_hashTable[k_.GetHashVal()].GetTesu() % 2 != k_.GetTesu() % 2) {
        /// <summary>
        /// Žè”Ô‚ªˆá‚¤
        /// </summary>
        return 0;
    }

    Te te{ m_hashTable[k_.GetHashVal()].GetBest() };
    if (!te.IsNull()) {
        if (k_.IsLegalMove(isSelfOrEnemy_, te)) {
            teValids_.emplace_back(te);
        }
    }

    te = m_hashTable[k_.GetHashVal()].GetSecond();
    if (!te.IsNull()) {
        if (k_.IsLegalMove(isSelfOrEnemy_, te)) {
            teValids_.emplace_back(te);
        }
    }

    if (depth_ > 1) {
        te = m_bestHands[depth_ - 1][depth_];
        if (!te.IsNull() && k_.IsLegalMove(isSelfOrEnemy_, te)) {
            teValids_.emplace_back(te);
        }
    }

    return static_cast<uint32>(teValids_.size());
}

int32 Sikou::NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const uint32 depthMax_, const bool itDeep_) noexcept {
    if (depth_ == 1) {
        if (kyokumen_.IsSennitite()) {
            if (kyokumen_.IsContinuous(Turn::Enemy) == Winner::Player) {
                return 999999;
            }
            if (kyokumen_.IsContinuous(Turn::Player) == Winner::Enemy) {
                return -999999;
            }

            return 0;
        }
    }

    if (depth_ >= depthMax_) {
        return ((isSelfOrEnemy_ == Self)
            ? kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_)
            : -(kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_)));
    }

    if (m_hashTable[kyokumen_.GetHashVal()].GetHashVal() == kyokumen_.GetHashVal()) {
        HashEntry e{ m_hashTable[kyokumen_.GetHashVal()] };
        if (e.GetValue() >= beta_ && e.GetTesu() >= kyokumen_.GetTesu() && e.GetTesu() % 2 == kyokumen_.GetTesu() % 2
            && e.GetDepth() <= depth_ && e.GetRemainDepth() >= depthMax_ - depth_ && e.GetValueKind() != ValueKind::Upper) {
            return e.GetValue();
        }

        if (e.GetValue() <= alpha_ && e.GetTesu() >= kyokumen_.GetTesu() && e.GetTesu() % 2 == kyokumen_.GetTesu() % 2
            && e.GetDepth() <= depth_ && e.GetRemainDepth() >= depthMax_ - depth_ && e.GetValueKind() != ValueKind::Lower) {
            return e.GetValue();
        }
    }
    else if (depthMax_ - depth_ > 2 && itDeep_) {
        return ITDeep(isSelfOrEnemy_, kyokumen_, alpha_, beta_, depth_, depthMax_);
    }

    int32 retVal{ -1000000 };
    {
        Array<Te> teValids;
        uint32 teNum{ MakeMoveFirst(isSelfOrEnemy_, depth_, teValids, kyokumen_) };
        for (const uint32 i : step(teNum)) {
            Kyokumen k{ kyokumen_ };
            if (teValids[i].IsNull()) {
                continue;
            }

            k.Move(isSelfOrEnemy_, teValids[i]);
            int32 value{ -NegaAlphaBeta(isSelfOrEnemy_ == Self ? Enemy : Self, k, -beta_, -Max(alpha_, retVal), depth_ + 1, depthMax_) };
            if (value > retVal) {
                retVal = value;
                m_bestHands[depth_][depth_] = std::move(teValids[i]);

                for (uint32 j{ depth_ + 1 }; j < depthMax_; ++j) {
                    m_bestHands[depth_][j] = m_bestHands[depth_ + 1][j];
                }

                if (retVal >= beta_) {
                    return HashAdd(retVal, kyokumen_, alpha_, beta_, depth_, depthMax_);
                }
            }
        }
    }

    uint32 teNum{ kyokumen_.MakeLegalMoves(isSelfOrEnemy_) };
    if (teNum == 0) {
        return -999999;
    }

    Array<Te> teValids{ kyokumen_.GetTeValids() };
    teValids.shuffle();


    for (const uint32 i : step(teNum)) {
        Kyokumen k{ kyokumen_ };
        k.Move(isSelfOrEnemy_, teValids[i]);

        int32 value{ -NegaAlphaBeta(isSelfOrEnemy_ == Self ? Enemy : Self, k, -beta_, -Max(alpha_, retVal), depth_ + 1, depthMax_) };
        if (value > retVal) {
            retVal = value;
            m_bestHands[depth_][depth_] = std::move(teValids[i]);

            for (uint32 j{ depth_ + 1 }; j < depthMax_; ++j) {
                m_bestHands[depth_][j] = m_bestHands[depth_ + 1][j];
            }

            if (retVal >= beta_) {
                return HashAdd(retVal, kyokumen_, alpha_, beta_, depth_, depthMax_);
            }
        }
    }

    return retVal;
}

int32 Sikou::HashAdd(const int32 retVal_, const Kyokumen& kyokumen_, const int32 alpha_, const int32 beta_, const uint32 depth_, const uint32 depthMax_) {
    HashEntry e{ m_hashTable[kyokumen_.GetHashVal()] };
    if (e.GetHashVal() == kyokumen_.GetHashVal()) {
        e.ChangeSecondFromBest();
    }
    else {
        if (e.GetTesu() - e.GetDepth() == kyokumen_.GetTesu() - depth_ && e.GetRemainDepth() > depthMax_ - depth_) {
            return retVal_;
        }

        e.SetHashVal(kyokumen_.GetHashVal());
        e.InitSecond();
    }

    if (retVal_ > alpha_) {
        e.ChangeBestFromBestHand(m_bestHands[depth_][depth_]);
    }
    else {
        e.InitBest();
    }

    e.SetValue(retVal_);
    if (retVal_ <= alpha_) {
        e.SetValueKind(ValueKind::Upper);
    }
    else if (retVal_ >= beta_) {
        e.SetValueKind(ValueKind::Lower);
    }
    else {
        e.SetValueKind(ValueKind::Exactly);
    }

    e.SetDepth(depth_);
    e.SetRemainDepth(depthMax_ - depth_);
    e.SetTesu(kyokumen_.GetTesu());
    m_hashTable[kyokumen_.GetHashVal()] = e;

    return retVal_;
}

int32 Sikou::ITDeep(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_, const uint32 depthMax_) noexcept {
    int32 retVal{};
    for (uint32 i{ depth_ + 1 }; i <= depthMax_; ++i) {
        retVal = NegaAlphaBeta(isSelfOrEnemy_, kyokumen_, alpha_, beta_, depth_, i, false);
    }

    return retVal;
}


Te Sikou::Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept {
    for (const uint32 i : step(m_depthMax)) {
        for (const uint32 j : step(m_depthMax)) {
            m_bestHands[i][j] = Te(0);
        }
    }

    int32 bestVal{ ITDeep(isSelfOrEnemy_, kyokumen_, -999998, 999998, 0, m_depthMax) };

    return m_bestHands[0][0];
}
