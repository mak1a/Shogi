#include "Kyokumen.hpp"

Kyokumen::Kyokumen(const uint32 tesu_, const array<array<uint32, 9>, 9>& board_, const array<uint32, 41>& motigoma_) noexcept
: m_kingSelfPos(0)
, m_kingEnemyPos(0)
, m_tesu(tesu_)
, m_value(0)
, m_kyokumenHashVal(0)
, m_handHashVal(0)
, m_hashVal(0) {
    m_ban.fill(Wall);
    m_pin.fill(Wall);

    for (uint32 suji{10}; suji <= 90; suji += 10) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            m_ban[suji + dan] = board_[dan - 1][9-(suji/10)];
            m_kyokumenHashVal ^= HashSeeds[m_ban[suji+dan]][suji+dan];

            if (m_ban[suji + dan] == Sou) {
                m_kingSelfPos = suji + dan;
            }
            if (m_ban[suji + dan] == Eou) {
                m_kingEnemyPos = suji + dan;
            }

            m_value += KomaValue[m_ban[suji + dan]];
        }
    }

    for (uint32 i{}; i <= Ehi; ++i) {
        m_holdingKomas[i] = motigoma_[i];
        m_value += HandValue[i] * m_holdingKomas[i];

        for (uint32 j{1}; j <= m_holdingKomas[i]; ++j) {
            m_handHashVal ^= HashHandSeeds[i][j];
        }
    }

    m_hashVal = m_kyokumenHashVal ^ m_handHashVal;
    ++m_hashHistory[m_hashVal];
    m_outeHistory.emplace_back(0, m_hashVal);

    InitControl();
}

void Kyokumen::InitControl() {
    m_controlSelf.fill(0);
    m_controlEnemy.fill(0);

    for (uint32 suji{10}; suji <= 90; suji += 10) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            if (m_ban[suji + dan] & Enemy) {
                for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
                    if (CanJump[i][m_ban[dan + suji]]) {
                        uint32 j{dan + suji};

                        do {
                            j += Direct[i];
                            m_controlEnemy[j].set(jumpDir);
                        }
                        while (m_ban[j] == Empty);
                    }
                    else if (CanMove[i][m_ban[dan + suji]] && (static_cast<int32>(dan + suji) + Direct[i] >= 0)) {
                        m_controlEnemy[dan + suji + Direct[i]].set(moveDir);
                    }
                }
            }
            else if (m_ban[suji + dan] & Self) {
                for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
                    if (CanJump[i][m_ban[dan + suji]]) {
                        uint32 j{dan + suji};

                        do {
                            j += Direct[i];
                            m_controlSelf[j].set(jumpDir);
                        }
                        while (m_ban[j] == Empty);
                    }
                    else if (CanMove[i][m_ban[dan + suji]] && (static_cast<int32>(dan + suji) + Direct[i] >= 0)) {
                        m_controlSelf[dan + suji + Direct[i]].set(moveDir);
                    }
                }
            }
        }
    }
}

void Kyokumen::Move(const uint32 isSelfOrEnemy_, const Te& te_) {
    if (te_.GetFrom() > 10) {
        for (uint32 dir{}, moveDir{}, jumpDir{16}; dir < 12; ++dir, ++moveDir, ++jumpDir) {
            if (static_cast<int32>(te_.GetFrom()) + Direct[dir] < 0) {
                continue;
            }

            if (isSelfOrEnemy_ == Self) {
                m_controlSelf[te_.GetFrom() + Direct[dir]].reset(moveDir);
            }
            else {
                m_controlEnemy[te_.GetFrom() + Direct[dir]].reset(moveDir);
            }
            if (CanJump[dir][te_.GetKoma()]) {
                int32 j{static_cast<int32>(te_.GetFrom())};

                do {
                    j += Direct[dir];
                    if (isSelfOrEnemy_ == Self) {
                        m_controlSelf[j].reset(jumpDir);
                    }
                    else {
                        m_controlEnemy[j].reset(jumpDir);
                    }
                }
                while (m_ban[j] == Empty);
            }
        }

        m_ban[te_.GetFrom()] = Empty;

        m_kyokumenHashVal ^= HashSeeds[te_.GetKoma()][te_.GetFrom()];
        m_kyokumenHashVal ^= HashSeeds[Empty][te_.GetFrom()];

        for (uint32 i{}, jumpDir{16}; i < 8; ++i, ++jumpDir) {
            int32 dir{Direct[i]};

            if (m_controlSelf[te_.GetFrom()].test(jumpDir)) {
                int32 j{static_cast<int32>(te_.GetFrom())};

                do {
                    j += dir;
                    m_controlSelf[j].set(jumpDir);
                }
                while (m_ban[j] == Empty);
            }

            if (m_controlEnemy[te_.GetFrom()].test(jumpDir)) {
                int32 j{static_cast<int32>(te_.GetFrom())};

                do {
                    j += dir;
                    m_controlEnemy[j].set(jumpDir);
                }
                while (m_ban[j] == Empty);
            }
        }
    }
    else {
        m_handHashVal ^= HashHandSeeds[te_.GetKoma()][m_holdingKomas[te_.GetKoma()]];

        --m_holdingKomas[te_.GetKoma()];

        m_value -= HandValue[te_.GetKoma()];
        m_value += KomaValue[te_.GetKoma()];
    }

    if (m_ban[te_.GetTo()] != Empty) {
        m_value -= KomaValue[m_ban[te_.GetTo()]];

        uint32 koma{isSelfOrEnemy_ | (m_ban[te_.GetTo()] & ~Promote & ~Self & ~Enemy)};
        m_value += HandValue[koma];

        ++m_holdingKomas[koma];

        m_handHashVal ^= HashHandSeeds[koma][m_holdingKomas[koma]];

        for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
            int32 dir{Direct[i]};

            if (CanJump[i][m_ban[te_.GetTo()]]) {
                int32 j{static_cast<int32>(te_.GetTo())};

                do {
                    j += dir;

                    if (isSelfOrEnemy_ == Self) {
                        m_controlEnemy[j].reset(jumpDir);
                    }
                    else {
                        m_controlSelf[j].reset(jumpDir);
                    }
                }
                while (m_ban[j] == Empty);
            }
            else {
                int32 j{static_cast<int32>(te_.GetTo()) + dir};

                if (j < 0) {
                    continue;
                }

                if (isSelfOrEnemy_ == Self) {
                    m_controlEnemy[j].reset(moveDir);
                }
                else {
                    m_controlSelf[j].reset(moveDir);
                }
            }
        }
    }
    else {
        for (uint32 i{}, jumpDir{16}; i < 8; ++i, ++jumpDir) {
            int32 dir{Direct[i]};

            if (m_controlSelf[te_.GetTo()].test(jumpDir)) {
                int32 j{static_cast<int32>(te_.GetTo())};

                do {
                    j += dir;
                    m_controlSelf[j].reset(jumpDir);
                }
                while (m_ban[j] == Empty);
            }

            if (m_controlEnemy[te_.GetTo()].test(jumpDir)) {
                int32 j{static_cast<int32>(te_.GetTo())};

                do {
                    j += dir;
                    m_controlEnemy[j].reset(jumpDir);
                }
                while (m_ban[j] == Empty);
            }
        }
    }

    m_kyokumenHashVal ^= HashSeeds[m_ban[te_.GetTo()]][te_.GetTo()];

    if (te_.GetPromote()) {
        m_value -= KomaValue[te_.GetKoma()];
        m_value += KomaValue[te_.GetKoma() | Promote];
        m_ban[te_.GetTo()] = te_.GetKoma() | Promote;
    }
    else {
        m_ban[te_.GetTo()] = te_.GetKoma();
    }

    m_kyokumenHashVal ^= HashSeeds[m_ban[te_.GetTo()]][te_.GetTo()];

    for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
        if (CanJump[i][m_ban[te_.GetTo()]]) {
            int32 j{static_cast<int32>(te_.GetTo())};

            do {
                j += Direct[i];

                if (isSelfOrEnemy_ == Self) {
                    m_controlSelf[j].set(jumpDir);
                }
                else {
                    m_controlEnemy[j].set(jumpDir);
                }
            }
            while (m_ban[j] == Empty);
        }
        else if (CanMove[i][m_ban[te_.GetTo()]]) {
            if (isSelfOrEnemy_ == Self) {
                m_controlSelf[te_.GetTo() + Direct[i]].set(moveDir);
            }
            else {
                m_controlEnemy[te_.GetTo() + Direct[i]].set(moveDir);
            }
        }
    }

    if (te_.GetKoma() == Sou) {
        m_kingSelfPos = te_.GetTo();
    }
    if (te_.GetKoma() == Eou) {
        m_kingEnemyPos = te_.GetTo();
    }

    m_hashVal = m_kyokumenHashVal ^ m_handHashVal;
    ++m_tesu;
    ++m_hashHistory[m_hashVal];
    m_outeHistory.emplace_back((isSelfOrEnemy_==Self)?m_controlSelf[m_kingEnemyPos]:m_controlEnemy[m_kingSelfPos], m_hashVal);
}

void Kyokumen::MakePinInfo() {
    for (uint32 i{11}; i <= 99; ++i) {
        m_pin[i] = 0;
    }
    m_teValids.clear();

    if (m_kingSelfPos) {
        for (uint32 i{}; i < 8; ++i) {
            uint32 p{Search(m_kingSelfPos, -Direct[i])};

            if (m_ban[p] == Wall || (m_ban[p] & Enemy)) {
                continue;
            }

            if (m_controlEnemy[p].test(16 + i)) {
                m_pin[p] = Direct[i];
            }
        }
    }

    if (m_kingEnemyPos) {
        for (uint32 i{}; i < 8; ++i) {
            uint32 p{Search(m_kingEnemyPos, -Direct[i])};

            if (m_ban[p] == Wall || !(m_ban[p] & Enemy)) {
                continue;
            }

            if (m_controlSelf[p].test(16 + i)) {
                m_pin[p] = Direct[i];
            }
        }
    }
}

uint32 Kyokumen::MakeLegalMoves(const uint32 isSelfOrEnemy_) {
    MakePinInfo();

    if (isSelfOrEnemy_ == Self && m_controlEnemy[m_kingSelfPos].any()) {
        return AntiCheck(isSelfOrEnemy_, m_controlEnemy[m_kingSelfPos]);
    }

    if (isSelfOrEnemy_ == Enemy && m_controlSelf[m_kingEnemyPos].any()) {
        return AntiCheck(isSelfOrEnemy_, m_controlSelf[m_kingEnemyPos]);
    }

    for (uint32 suji{10}; suji <= 90; suji += 10) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            if (m_ban[suji + dan] & isSelfOrEnemy_) {
                AddMoves(isSelfOrEnemy_, suji + dan, m_pin[suji + dan]);
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Fu] > 0) {
        for (uint32 suji{10}; suji <= 90; suji += 10) {
            bool nifu{false};
            for (uint32 dan{1}; dan <= 9; ++dan) {
                if (m_ban[suji + dan] == (isSelfOrEnemy_ | Fu)) {
                    nifu = true;
                    break;
                }
            }

            if (nifu) {
                continue;
            }

            uint32 startDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 2 : 1)};
            uint32 endDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 9 : 8)};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty && !Uchifudume(isSelfOrEnemy_, dan + suji)) {
                    m_teValids.emplace_back(0, suji + dan, isSelfOrEnemy_ | Fu);
                }
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ky] > 0) {
        for (uint32 suji{10}; suji <= 90; suji += 10) {
            uint32 startDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 2 : 1)};
            uint32 endDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 9 : 8)};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty) {
                    m_teValids.emplace_back(0, suji + dan, isSelfOrEnemy_ | Ky);
                }
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ke]) {
        for (uint32 suji{10}; suji <= 90; suji += 10) {
            uint32 startDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 3 : 1)};
            uint32 endDan{static_cast<uint32>((isSelfOrEnemy_ == Self) ? 9 : 7)};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty) {
                    m_teValids.emplace_back(0, suji + dan, isSelfOrEnemy_ | Ke);
                }
            }
        }
    }

    for (uint32 koma{Gi}; koma <= Hi; ++koma) {
        if (m_holdingKomas[isSelfOrEnemy_ | koma] > 0) {
            for (uint32 suji{10}; suji <= 90; suji += 10) {
                for (uint32 dan{1}; dan <= 9; ++dan) {
                    if (m_ban[dan + suji] == Empty) {
                        m_teValids.emplace_back(0, suji + dan, isSelfOrEnemy_ | koma);
                    }
                }
            }
        }
    }

    return m_teValids.size();
}

void Kyokumen::AddMoves(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 pin_, const int32 rPin_) {
    switch (m_ban[from_]) {
    case Sfu:
        AddMove(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        break;
    case Efu:
        AddMove(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        break;
    case Sky:
        AddStraight(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        break;
    case Eky:
        AddStraight(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        break;
    case Ske:
        AddMove(isSelfOrEnemy_, from_, +8, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -12, pin_, rPin_);
        break;
    case Eke:
        AddMove(isSelfOrEnemy_, from_, -8, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +12, pin_, rPin_);
        break;
    case Sgi:
        AddMove(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +9, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -9, pin_, rPin_);
        break;
    case Egi:
        AddMove(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -9, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +9, pin_, rPin_);
        break;
    case Ski:case Sto:case Sny:case Snk:case Sng:
        AddMove(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +9, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -10, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +10, pin_, rPin_);
        break;
    case Eki:case Eto:case Eny:case Enk:case Eng:
        AddMove(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -9, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +10, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -10, pin_, rPin_);
        break;
    case Sry:case Ery:
        AddMove(isSelfOrEnemy_, from_, +11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -9, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -11, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +9, pin_, rPin_);
    case Shi:case Ehi:
        AddStraight(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, -10, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, +10, pin_, rPin_);
        break;
    case Sum:case Eum:
        AddMove(isSelfOrEnemy_, from_, +1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -1, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, -10, pin_, rPin_);
        AddMove(isSelfOrEnemy_, from_, +10, pin_, rPin_);
    case Ska:case Eka:
        AddStraight(isSelfOrEnemy_, from_, +11, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, -9, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, -11, pin_, rPin_);
        AddStraight(isSelfOrEnemy_, from_, +9, pin_, rPin_);
        break;
    case Sou:case Eou:
        MoveKing(isSelfOrEnemy_, 0);
        break;
    default:
        break;
    }
}

std::bitset<28> Kyokumen::CountControlSelf(const uint32 pos_) {
    std::bitset<28> ret{};
    
    for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
        if (static_cast<int32>(pos_) - Direct[i] < 0 || static_cast<int32>(pos_) - Direct[i] >= 121) {
            continue;
        }

        if (CanMove[i][m_ban[pos_ - Direct[i]]] && (m_ban[pos_ - Direct[i]] & Self)) {
            ret.set(moveDir);
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]] && (m_ban[Search(pos_, -Direct[i])] & Self)) {
            ret.set(jumpDir);
        }
    }

    return ret;
}

std::bitset<28> Kyokumen::CountControlEnemy(const uint32 pos_) {
    std::bitset<28> ret{};
    
    for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
        if (static_cast<int32>(pos_) - Direct[i] < 0 || static_cast<int32>(pos_) - Direct[i] >= 121) {
            continue;
        }

        if (CanMove[i][m_ban[pos_ - Direct[i]]] && (m_ban[pos_ - Direct[i]] & Enemy)) {
            ret.set(moveDir);
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]] && (m_ban[Search(pos_, -Direct[i])] & Enemy)) {
            ret.set(jumpDir);
        }
    }

    return ret;
}

std::bitset<28> Kyokumen::CountMove(const uint32 isSelfOrEnemy_, const uint32 pos_) {
    std::bitset<28> ret{};

    for (uint32 i{}, moveDir{}, jumpDir{16}; i < 12; ++i, ++moveDir, ++jumpDir) {
        if (CanMove[i][m_ban[pos_ - Direct[i]]]
            && (m_ban[pos_ - Direct[i]] & isSelfOrEnemy_)
            && (m_pin[pos_ - Direct[i]] == 0
                || m_pin[pos_ - Direct[i]] == Direct[i]
                || m_pin[pos_ - Direct[i]] == -Direct[i])) {
            ret.set(moveDir);
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]]
            && (m_ban[Search(pos_, -Direct[i])] & isSelfOrEnemy_)
            && (m_pin[Search(pos_, -Direct[i])] == 0
                || m_pin[Search(pos_, -Direct[i])] == Direct[i]
                || m_pin[Search(pos_, -Direct[i])] == -Direct[i])) {
            ret.set(jumpDir);
        }
    }

    return ret;
}

bool Kyokumen::Uchifudume(const uint32 isSelfOrEnemy_, const uint32 to_) {
    if (isSelfOrEnemy_ == Self) {
		if (m_kingEnemyPos + 1 != to_) {
			return false;
		}
	}
    else {
        if (m_kingSelfPos - 1 != to_) {
            return false;
        }
    }

    m_ban[to_] = Fu | isSelfOrEnemy_;
    if (isSelfOrEnemy_ == Self) {
        if (m_controlSelf[to_].any() && CountMove(Enemy, to_).test(1)) {
            for (uint32 i{}; i < 8; ++i) {
                uint32 koma{m_ban[m_kingEnemyPos + Direct[i]]};
                if (!(koma & Enemy) && CountControlSelf(m_kingEnemyPos + Direct[i]).none()) {
                    m_ban[to_] = Empty;
                    return false;
                }
            }

            m_ban[to_] = Empty;
            return true;
        }

        m_ban[to_] = Empty;
        return false;
    }
    else {
        if (m_controlEnemy[to_].any() && CountMove(Self, to_).test(6)) {
            for (uint32 i{}; i < 8; ++i) {
                uint32 koma{m_ban[m_kingSelfPos + Direct[i]]};
                if (!(koma & Self) && CountControlEnemy(m_kingSelfPos + Direct[i]).none()) {
                    m_ban[to_] = Empty;
                    return false;
                }
            }

            m_ban[to_] = Empty;
            return true;
        }

        m_ban[to_] = Empty;
        return false;
    }
}

void Kyokumen::PutTo(const uint32 isSelfOrEnemy_, const uint32 pos_) {
    int32 dan{static_cast<int32>(pos_ % 10)};

    if (isSelfOrEnemy_ == Enemy) {
        dan = 10 - dan;
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Fu] > 0 && dan > 1) {
        int32 suji{static_cast<int32>(pos_ / 10)};
        suji *= 10;
        bool nifu{false};

        for (uint32 d{1}; d <= 9; ++d) {
            if (m_ban[suji + d] == (isSelfOrEnemy_ | Fu)) {
                nifu = true;
                break;
            }
        }

        if (!nifu && !Uchifudume(isSelfOrEnemy_, pos_)) {
            m_teValids.emplace_back(0, pos_, (isSelfOrEnemy_ | Fu));
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ky] > 0 && dan > 1) {
        m_teValids.emplace_back(0, pos_, (isSelfOrEnemy_ | Ky));
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ke] > 0 && dan > 2) {
        m_teValids.emplace_back(0, pos_, (isSelfOrEnemy_ | Ke));
    }

    for (uint32 koma{Gi}; koma <= Hi; ++koma) {
        if (m_holdingKomas[isSelfOrEnemy_ | koma] > 0) {
            m_teValids.emplace_back(0, pos_, (isSelfOrEnemy_ | koma));
        }
    }
}

uint32 Kyokumen::AntiCheck(const uint32 isSelfOrEnemy_, const std::bitset<28>& control_) {
    if (control_.count() >= 2) {
        MoveKing(isSelfOrEnemy_, control_);
    }
    else {
        uint32 king{(isSelfOrEnemy_ == Self) ? m_kingSelfPos : m_kingEnemyPos};

        uint32 id{};

        for (; id < 28; ++id) {
            if (control_.test(id)) {
                break;
            }
        }

        uint32 check{(id < 16) ? king - Direct[id] : Search(king, -Direct[id - 16])};

        MoveTo(isSelfOrEnemy_, check);

        MoveKing(isSelfOrEnemy_, control_);

        if (id >= 16) {
            for (int32 i{static_cast<int32>(king) - Direct[id - 16]}; m_ban[i] == Empty; i -= Direct[id - 16]) {
                MoveTo(isSelfOrEnemy_, i);
            }

            for (int32 i{static_cast<int32>(king) - Direct[id - 16]}; m_ban[i] == Empty; i -= Direct[id - 16]) {
                PutTo(isSelfOrEnemy_, i);
            }
        }
    }
    return m_teValids.size();
}

void Kyokumen::MoveKing(const uint32 isSelfOrEnemy_, const std::bitset<28>& kiki_) {
    int32 id{-1};

    for (uint32 i{}; i < 8; ++i) {
        if (kiki_.test(i)) {
            id = i;
            break;
        }
    }

    if (id >= 0) {
        if (isSelfOrEnemy_ == Self) {
            uint32 koma{m_ban[m_kingSelfPos - Direct[id]]};

            if ((koma == Empty || (koma & Enemy))
                && CountControlEnemy(m_kingSelfPos - Direct[id]).none()
                && !(kiki_.test(23 - id))
                ) {
                AddMove(isSelfOrEnemy_, m_kingSelfPos, -Direct[id], 0);
            }
        }
        else {
            uint32 koma{m_ban[m_kingEnemyPos - Direct[id]]};

            if ((koma == Empty || (koma & Self))
                && CountControlSelf(m_kingEnemyPos - Direct[id]).none()
                && !(kiki_.test(23 - id))
                ) {
                AddMove(isSelfOrEnemy_, m_kingEnemyPos, -Direct[id], 0);
            }
        }

    }
    for (uint32 i{}; i < 8; ++i) {
        if (i == id) {
            continue;
        }
        if (isSelfOrEnemy_ == Self) {
            if (static_cast<int32>(m_kingSelfPos) - Direct[i] < 0) {
                continue;
            }
            uint32 koma{m_ban[m_kingSelfPos - Direct[i]]};
            if ((koma == Empty || (koma & Enemy))
                && CountControlEnemy(m_kingSelfPos - Direct[i]).none()
                && !(kiki_.test(23 - i))
                ) {
                AddMove(isSelfOrEnemy_, m_kingSelfPos, -Direct[i], 0);
            }
        }
        else {
            if (static_cast<int32>(m_kingEnemyPos) - Direct[i] < 0) {
                continue;
            }
            uint32 koma{m_ban[m_kingEnemyPos - Direct[i]]};
            if ((koma == Empty || (koma & Self))
                && CountControlSelf(m_kingEnemyPos - Direct[i]).none()
                && !(kiki_.test(23 - i))
                ) {
                AddMove(isSelfOrEnemy_, m_kingEnemyPos, -Direct[i], 0);
            }
        }
    }
}

void Kyokumen::AddMove(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 diff_, const int32 pin_, const int32 rPin_) {
    if (rPin_ == diff_ || rPin_ == -diff_) {
        return;
    }

    const int32 to{static_cast<int32>(from_ + diff_)};
    const int32 dan{to % 10};
    const int32 fromDan{static_cast<int32>(from_ % 10)};

    if ((pin_ != 0 && pin_ != diff_ && pin_ != -diff_) || (m_ban[to] & isSelfOrEnemy_)) {
        return;
    }

    if (m_ban[from_] == Ske && dan <= 2) {
        m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
    }
    else if ((m_ban[from_] == Sfu || m_ban[from_] == Sky) && dan <= 1) {
        m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
    }
    else if (m_ban[from_] == Eke && dan >= 8) {
        m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
    }
    else if ((m_ban[from_] == Efu || m_ban[from_] == Eky) && dan >= 9) {
        m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
    }
    else {
        if (isSelfOrEnemy_ == Self && (fromDan <= 3 || dan <= 3) && CanPromote[m_ban[from_]]) {
            m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
        }
        else if (isSelfOrEnemy_ == Enemy && (fromDan >= 7 || dan >= 7) && CanPromote[m_ban[from_]]) {
            m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 1);
        }

        m_teValids.emplace_back(from_, to, m_ban[from_], m_ban[to], 0);
    }
}

void Kyokumen::AddStraight(const uint32 isSelfOrEnemy_, const uint32 from_, const int32 dir_, const int32 pin_, const int32 rPin_) {
    if (dir_ == rPin_ || dir_ == -rPin_) {
        return;
    }

    if (pin_ == 0 || pin_ == dir_ || pin_ == -dir_) {
        int32 i{};

        for (i = dir_; m_ban[from_ + i] == Empty; i += dir_) {
            AddMove(isSelfOrEnemy_, from_, i, 0);
        }

        if (!(m_ban[from_ + i] & isSelfOrEnemy_)) {
            AddMove(isSelfOrEnemy_, from_, i, 0);
        }
    }
}

void Kyokumen::MoveTo(const uint32 isSelfOrEnemy_, const uint32 to_) {
    uint32 p{};
    uint32 koma{};

    for (uint32 i{}; i < 12; ++i) {
        if (static_cast<int32>(to_) - Direct[i] < 0) {
            continue;
        }

        if ((koma = m_ban[to_ - Direct[i]]) == Empty) {
            p = Search(to_, -Direct[i]);
            if ((m_ban[p] & isSelfOrEnemy_) && CanJump[i][m_ban[p]]) {
                AddMove(isSelfOrEnemy_, p, to_ - p, m_pin[p]);
            }
        }
        else {
            if ((koma & ~isSelfOrEnemy_) != Ou && (koma & isSelfOrEnemy_)
                && (CanMove[i][koma] || CanJump[i][koma])) {
                AddMove(isSelfOrEnemy_, to_ - Direct[i], Direct[i], m_pin[to_ - Direct[i]]);
            }
        }
    }
}

[[nodiscard]] constexpr uint32 Kyori(const uint32 p1_, const uint32 p2_) noexcept {
    return Max<uint32>(Abs(p1_ / 10 - p2_ / 10), Abs((p1_ % 10) - (p2_ % 10)));
}

bool Kyokumen::IsCorrectMove(Te& te_) {
    if (m_ban[te_.GetFrom()] == Sou || m_ban[te_.GetFrom()] == Eou) {
        if (m_controlEnemy[te_.GetTo()].any()) {
            return false;
        }

        te_.SetCapture(m_ban[te_.GetTo()]);
        return true;
    }

    if (m_ban[te_.GetFrom()] == Eou) {
        if (m_controlSelf[te_.GetTo()].any()) {
            return false;
        }

        te_.SetCapture(m_ban[te_.GetTo()]);
        return true;
    }

    if (m_ban[te_.GetFrom()] == Ske || m_ban[te_.GetFrom()] == Eke) {
        te_.SetCapture(m_ban[te_.GetTo()]);
        return true;
    }

    uint32 d{Kyori(te_.GetFrom(), te_.GetTo())};
    if (d == 0) {
        return false;
    }

    if (d == 1) {
        te_.SetCapture(m_ban[te_.GetTo()]);
        return true;
    }

    int32 dir{static_cast<int32>((te_.GetTo() - te_.GetFrom()) / d)};

    for (uint32 i{1}, pos{te_.GetFrom() + dir}; i < d; ++i, pos += dir) {
        if (pos < 0 || pos >= 121) {
            continue;
        }

        if (m_ban[pos] != Empty) {
            return false;
        }
    }

    te_.SetCapture(m_ban[te_.GetTo()]);
    return true;
}

int32 Kyokumen::EvalMin(Array<Te>& moveSelf_, Array<Te>& moveEnemy_) {
    if (moveEnemy_.isEmpty()) {
        return m_value;
    }

    int32 v{m_value};
    uint32 k{};

    for (; k < moveEnemy_.size(); ++k) {
        if (IsCorrectMove(moveEnemy_[k])) {
            break;
        }
    }

    if (k > 0) {
        if (k >= moveEnemy_.size()) {
            return v;
        }

        Te te{moveEnemy_[k]};
        for (uint32 i{k}; i > 0; --i) {
            moveEnemy_[i] = std::move(moveEnemy_[i - 1]);
        }

        moveEnemy_[0] = std::move(te);
    }

    moveEnemy_[0].SetCapture(m_ban[moveEnemy_[0].GetTo()]);
    Move(Enemy, moveEnemy_[0]);

    moveEnemy_.pop_front();
    return Min<int32>(v, EvalMax(moveSelf_, moveEnemy_));
}

int32 Kyokumen::EvalMax(Array<Te>& moveSelf_, Array<Te>& moveEnemy_) {
    if (moveSelf_.isEmpty()) {
        return m_value;
    }

    int32 v{m_value};
    uint32 k{};

    for (; k < moveSelf_.size(); ++k) {
        if (IsCorrectMove(moveSelf_[k])) {
            break;
        }
    }

    if (k > 0) {
        if (k >= moveSelf_.size()) {
            return v;
        }

        Te t{moveSelf_[k]};
        for (uint32 i{k}; i > 0; --i) {
            moveSelf_[i] = std::move(moveSelf_[i - 1]);
        }
        moveSelf_[0] = std::move(t);
    }

    moveSelf_[0].SetCapture(m_ban[moveSelf_[0].GetTo()]);
    Move(Self, moveSelf_[0]);

    moveSelf_.pop_front();
    return Max<int32>(v, EvalMin(moveSelf_, moveEnemy_));
}

int32 Kyokumen::Eval(const uint32 pos_) {
    if (m_ban[pos_] == Empty) {
        return 0;
    }
    if ((m_ban[pos_] & Self) && m_controlEnemy[pos_].none()) {
        return 0;
    }
    if ((m_ban[pos_] & Enemy) && m_controlSelf[pos_].none()) {
        return 0;
    }

    uint32 toPos{pos_};

    Array<Te> attackSelfTes;
    Array<Te> attackEnemyTes;

    uint32 pos2{};

    bool promoteSelf{(toPos % 10) <= 3}, promoteEnemy{(toPos % 10) >= 7};
    uint32 pos{toPos};

    for (uint32 i{}, moveDir{}, jumpDir{16}; i < 8; ++i, ++moveDir, ++jumpDir) {
        pos2 = pos;
        if (m_controlSelf[pos].test(moveDir)) {
            pos2 -= Direct[i];
            attackSelfTes.emplace_back(pos2, pos, m_ban[pos2]);

            if ((promoteSelf || (pos2 % 10) <= 3) && CanPromote[attackSelfTes.back().GetKoma()]) {
                attackSelfTes.back().SetPromote(1);
            }
            else {
                attackSelfTes.back().SetPromote(0);
            }
        }
        else if (m_controlEnemy[pos].test(moveDir)) {
            pos2 -= Direct[i];
            attackEnemyTes.emplace_back(pos2, pos, m_ban[pos2]);

            if ((promoteEnemy || (pos2 % 10) >= 7) && CanPromote[attackEnemyTes.back().GetKoma()]) {
                attackEnemyTes.back().SetPromote(1);
            }
            else {
                attackEnemyTes.back().SetPromote(0);
            }
        }

        if (static_cast<int32>(pos) - Direct[i] < 0) {
            continue;
        }

        if (m_ban[pos - Direct[i]] != Sou && m_ban[pos - Direct[i]] != Eou) {
            while ((m_controlSelf[pos2].test(jumpDir)) || (m_controlEnemy[pos2].test(jumpDir))) {
                do {
                    if (static_cast<int32>(pos2) - Direct[i] < 0) {
                        break;
                    }
                    pos2 -= Direct[i];
                }
                while (m_ban[pos2] == Empty);

                if (m_ban[pos2] & Enemy) {
                    attackEnemyTes.emplace_back(pos2, pos, m_ban[pos2]);
                    if ((promoteEnemy || (pos2 % 10) >= 7) && CanPromote[attackEnemyTes.back().GetKoma()]) {
                        attackEnemyTes.back().SetPromote(1);
                    }
                    else {
                        attackEnemyTes.back().SetPromote(0);
                    }
                }
                else if (m_ban[pos2] & Self) {
                    attackSelfTes.emplace_back(pos2, pos, m_ban[pos2]);
                    if ((promoteSelf || (pos2 % 10) <= 3) && CanPromote[attackSelfTes.back().GetKoma()]) {
                        attackSelfTes.back().SetPromote(1);
                    }
                    else {
                        attackSelfTes.back().SetPromote(0);
                    }
                }
            }
        }
    }

    for (uint32 i{8}, moveDir{8}; i < 12; ++i, ++moveDir) {
        if (static_cast<int32>(pos) - Direct[i] < 0) {
            continue;
        }

        if (m_controlSelf[pos].test(moveDir)) {
            pos2 = pos - Direct[i];
            attackSelfTes.emplace_back(pos2, pos, m_ban[pos2]);

            if (promoteSelf && CanPromote[attackSelfTes.back().GetKoma()]) {
                attackSelfTes.back().SetPromote(1);
            }
            else {
                attackSelfTes.back().SetPromote(0);
            }
        }
        
        if (m_controlEnemy[pos].test(moveDir)) {
            pos2 = pos - Direct[i];
            attackEnemyTes.emplace_back(pos2, pos, m_ban[pos2]);

            if (promoteEnemy && CanPromote[attackEnemyTes.back().GetKoma()]) {
                attackEnemyTes.back().SetPromote(1);
            }
            else {
                attackEnemyTes.back().SetPromote(0);
            }
        }
    }

    if (!attackSelfTes.isEmpty()) {
        for (uint32 i{}; i < attackSelfTes.size() - 1; ++i) {
            uint32 max_id{i};
            int32 max_val{KomaValue[attackSelfTes[i].GetKoma()]};

            for (uint32 j{i + 1}; j < attackSelfTes.size(); ++j) {
                int32 v{KomaValue[attackSelfTes[j].GetKoma()]};
                if (v < max_val) {
                    max_id = j;
                    max_val = v;
                }
                else if (v == max_val) {
                    if (KomaValue[attackSelfTes[j].GetKoma()] < KomaValue[attackSelfTes[max_id].GetKoma()]) {
                        max_id = j;
                    }
                }
            }

            if (i != max_id) {
                std::swap<Te>(attackSelfTes[i], attackSelfTes[max_id]);
            }
        }
    }

    if (!attackEnemyTes.isEmpty()) {
        for (uint32 i{}; i < attackEnemyTes.size() - 1; ++i) {
            uint32 max_id{i};
            int32 max_val{KomaValue[attackEnemyTes[i].GetKoma()]};

            for (uint32 j{i + 1}; j < attackEnemyTes.size(); ++j) {
                int32 v{KomaValue[attackEnemyTes[j].GetKoma()]};
                if (v > max_val) {
                    max_id = j;
                    max_val = v;
                }
                else if (v == max_val) {
                    if (KomaValue[attackEnemyTes[j].GetKoma()] > KomaValue[attackEnemyTes[max_id].GetKoma()]) {
                        max_id = j;
                    }
                }
            }

            if (i != max_id) {
                std::swap<Te>(attackEnemyTes[i], attackEnemyTes[max_id]);
            }
        }
    }

    bool isEnemy{(m_ban[pos_] & Enemy) != 0};
    bool isSelf{!isEnemy && (m_ban[pos_] != Enemy)};
    int32 val{m_value};

    if (isEnemy && !attackSelfTes.isEmpty()) {
        return EvalMax(attackSelfTes, attackEnemyTes) - val;
    }
    if (isSelf && !attackEnemyTes.isEmpty()) {
        return val - EvalMin(attackSelfTes, attackEnemyTes);
    }

    return 0;
}

int32 Kyokumen::BestEval(const uint32 isSelfOrEnemy_) {
    int32 best{};

    for (uint32 suji{10}; suji <= 90; suji += 10) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            if ((m_ban[suji + dan] & isSelfOrEnemy_) == 0) {
                int32 value{Eval(suji + dan)};
                if (value > best) {
                    best = value;
                }
            }
        }
    }

    if (isSelfOrEnemy_ == Enemy) {
        return -best;
    }

    return best;
}
