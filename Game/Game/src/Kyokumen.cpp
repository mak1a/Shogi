#include "Kyokumen.hpp"

Kyokumen::Kyokumen(const uint32 tesu_, const array<array<uint32, 9>, 9>& board_, const array<uint32, 41>& motigoma_)
: m_kingSelfPos(0)
, m_kingEnemyPos(0)
, m_tesu(tesu_)
, m_value(0) {
    m_ban.fill(Wall);
    m_pin.fill(Wall);

    for (uint32 dan{1}; dan <= 9; ++dan) {
        for (uint32 suji{1}; suji <= 9; ++suji) {
            m_ban[suji + dan] = board_[dan - 1][9 - suji];

            if (m_ban[suji + dan] == Sou) {
                m_kingSelfPos = suji + dan;
            }
            if (m_ban[suji + dan] == Eou) {
                m_kingEnemyPos = suji + dan;
            }

            m_value += KomaValue[m_ban[suji + dan]];
        }
    }

    for (uint32 i{}; i <= Eou; ++i) {
        m_holdingKomas[i] = motigoma_[i];
        m_value += HandValue[i] * m_holdingKomas[i];
    }

    InitControl();
}

void Kyokumen::InitControl() {
    m_controlSelf.fill(0);
    m_controlEnemy.fill(0);

    for (uint32 suji{1}; suji <= 9; ++suji) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            if (m_ban[suji + dan] & Enemy) {
                for (uint32 i{}, b{1}, bj{1 << 16}; i <= 11; ++i, b <<= 1, bj <<= 1) {
                    if (CanJump[i][m_ban[dan + suji]]) {
                        uint32 j{dan + suji};

                        do {
                            j += Direct[i];
                            m_controlEnemy[j] |= bj;
                        }
                        while (m_ban[j] == Empty);
                    }
                    else if (CanMove[i][m_ban[dan + suji]]) {
                        m_controlEnemy[dan + suji + Direct[i]] |= b;
                    }
                }
            }
            else if (m_ban[suji + dan] & Self) {
                for (uint32 i{}, b{1}, bj{1 << 16}; i <= 11; ++i, b <<= 1, bj <<= 1) {
                    if (CanJump[i][m_ban[dan + suji]]) {
                        uint32 j{dan + suji};

                        do {
                            j += Direct[i];
                            m_controlSelf[j] |= bj;
                        }
                        while (m_ban[j] == Empty);
                    }
                    else if (CanMove[i][m_ban[dan + suji]]) {
                        m_controlSelf[dan + suji + Direct[i]] |= b;
                    }
                }
            }
        }
    }
}

void Kyokumen::Move(const uint32 isSelfOrEnemy_, const Te& te_) {
    if (te_.GetFrom() > 1) {
        for (uint32 dir{}, b{1}, bj{1 << 16}; dir <= 11; ++dir, b <<= 1, bj <<= 1) {
            if (te_.GetFrom() + Direct[dir] < 0) {
                continue;
            }

            if (isSelfOrEnemy_ == Self) {
                m_controlSelf[te_.GetFrom() + Direct[dir]] &= ~b;
            }
            else {
                m_controlEnemy[te_.GetFrom() + Direct[dir]] &= ~b;
            }
            if (CanJump[dir][te_.GetKoma()]) {
                int32 j{te_.GetFrom()};

                do {
                    j += Direct[dir];
                    if (isSelfOrEnemy_ == Self) {
                        m_controlSelf[j] &= ~bj;
                    }
                    else {
                        m_controlEnemy[j] &= ~bj;
                    }
                }
                while (m_ban[j] == Empty);
            }
        }

        m_ban[te_.GetFrom()] = Empty;

        for (uint32 i{}, bj{1 << 16}; i < 8; ++i, bj <<= 1) {
            int32 dir{Direct[i]};

            if (m_controlSelf[te_.GetFrom()] & bj) {
                int32 j{te_.GetFrom()};

                do {
                    j += dir;
                    m_controlSelf[j] |= bj;
                }
                while (m_ban[j] == Empty);
            }

            if (m_controlEnemy[te_.GetFrom()] & bj) {
                int32 j{te_.GetFrom()};

                do {
                    j += dir;
                    m_controlEnemy[j] |= bj;
                }
                while (m_ban[j] == Empty);
            }
        }
    }
    else {
        --m_holdingKomas[te_.GetKoma()];

        m_value -= HandValue[te_.GetKoma()];
        m_value += KomaValue[te_.GetKoma()];
    }

    if (m_ban[te_.GetTo()] != Empty) {
        m_value -= KomaValue[m_ban[te_.GetTo()]];
        m_value += HandValue[isSelfOrEnemy_ | (m_ban[te_.GetTo()] & ~Promote & ~Self & ~Enemy)];

        ++m_holdingKomas[isSelfOrEnemy_ | (m_ban[te_.GetTo()] & ~Promote & ~Self & ~Enemy)];

        if (isSelfOrEnemy_ | (m_ban[te_.GetTo()] & ~Promote & ~Self & ~Enemy) == 0) {
            ++m_holdingKomas[Eou];
            m_kingEnemyPos = 0;
        }

        for (uint32 i{}, b{1}, bj{1 << 16}; i < 12; ++i, b <<= 1, bj <<= 1) {
            int32 dir{Direct[i]};

            if (CanJump[i][m_ban[te_.GetTo()]]) {
                int32 j{te_.GetTo()};

                do {
                    j += dir;

                    if (isSelfOrEnemy_ == Self) {
                        m_controlEnemy[j] &= ~bj;
                    }
                    else {
                        m_controlSelf[j] &= ~bj;
                    }
                }
                while (m_ban[j] == Empty);
            }
            else {
                int32 j{te_.GetTo() + dir};

                if (j < 0) {
                    continue;
                }

                if (isSelfOrEnemy_ == Self) {
                    m_controlEnemy[j] &= ~b;
                }
                else {
                    m_controlSelf[j] &= ~b;
                }
            }
        }
    }
    else {
        for (uint32 i{}, bj{1 << 16}; i < 8; ++i, bj <<= 1) {
            int32 dir = Direct[i];

            if (m_controlSelf[te_.GetTo()] & bj) {
                int32 j{te_.GetTo()};

                do {
                    j += dir;
                    m_controlSelf[te_.GetTo()] &= ~bj;
                }
                while (m_ban[j] == Empty);
            }

            if (m_controlEnemy[te_.GetTo()] & bj) {
                int32 j{te_.GetTo()};

                do {
                    j += dir;
                    m_controlEnemy[j] &= ~bj;
                }
                while (m_ban[j] == Empty);
            }
        }
    }

    if (te_.GetPromote()) {
        m_value -= KomaValue[te_.GetKoma()];
        m_value += KomaValue[te_.GetKoma() | Promote];
        m_ban[te_.GetTo()] = te_.GetKoma() | Promote;
    }
    else {
        m_ban[te_.GetTo()] = te_.GetKoma();
    }

    for (uint32 i{}, b{1}, bj{1 << 16}; i < 12; ++i, b <<= 1, bj <<= 1) {
        if (CanJump[i][m_ban[te_.GetTo()]]) {
            int32 j{te_.GetTo()};

            do {
                j += Direct[i];

                if (isSelfOrEnemy_ == Self) {
                    m_controlSelf[j] |= bj;
                }
                else {
                    m_controlEnemy[j] |= bj;
                }
            }
            while (m_ban[i] == Empty);
        }
        else if (CanMove[i][m_ban[te_.GetTo()]]) {
            if (isSelfOrEnemy_ == Self) {
                m_controlSelf[te_.GetTo() + Direct[i]] |= b;
            }
            else {
                m_controlEnemy[te_.GetTo() + Direct[i]] |= b;
            }
        }
    }

    if (te_.GetKoma() == Sou) {
        m_kingSelfPos = te_.GetTo();
    }
    if (te_.GetKoma() == Eou) {
        m_kingEnemyPos = te_.GetTo();
    }

    ++m_tesu;
}

void Kyokumen::MakePinInfo() {
    for (uint32 i{11}; i <= 99; ++i) {
        m_pin[i] = 0;
    }

    if (m_kingSelfPos) {
        for (uint32 i{}; i < 8; ++i) {
            uint32 p{Search(m_kingSelfPos, -Direct[i])};

            if (m_ban[p] == Wall || m_ban[p] & Enemy) {
                continue;
            }

            if (m_controlEnemy[p] & (1 << (16 + i))) {
                m_pin[p] = Direct[i];
            }
        }
    }

    if (m_kingEnemyPos) {
        for (uint32 i{}; i < 8; ++i) {
            uint32 p{Search(m_kingEnemyPos, -Direct[i])};

            if (m_ban[p] == Wall || m_ban[p] & Self) {
                continue;
            }

            if (m_controlSelf[p] & (1 << (16 + i))) {
                m_pin[p] = Direct[i];
            }
        }
    }
}

uint32 Kyokumen::MakeLegalMoves(const uint32 isSelfOrEnemy_) {
    MakePinInfo();

    if (isSelfOrEnemy_ == Self && m_controlEnemy[m_kingSelfPos] != 0) {
        return AntiCheck(isSelfOrEnemy_, m_controlEnemy[m_kingSelfPos]);
    }

    if (isSelfOrEnemy_ == Enemy && m_controlSelf[m_kingEnemyPos] != 0) {
        return AntiCheck(isSelfOrEnemy_, m_controlSelf[m_kingEnemyPos]);
    }

    for (uint32 suji{1}; suji <= 9; ++suji) {
        for (uint32 dan{1}; dan <= 9; ++dan) {
            if (m_ban[suji + dan] & isSelfOrEnemy_) {
                AddMoves(isSelfOrEnemy_, suji + dan, m_pin[suji + dan]);
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Fu] > 0) {
        for (uint32 suji{1}; suji <= 9; ++suji) {
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

            uint32 startDan{(isSelfOrEnemy_ == Self) ? 2 : 1};
            uint32 endDan{(isSelfOrEnemy_ == Self) ? 9 : 8};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty && !Uchifudume(isSelfOrEnemy_, dan + suji)) {
                    m_teValid.emplace_back(0, suji + dan, isSelfOrEnemy_ | Fu, Empty);
                }
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ky] > 0) {
        for (uint32 suji{1}; suji <= 9; ++suji) {
            uint32 startDan{(isSelfOrEnemy_ == Self) ? 2 : 1};
            uint32 endDan{(isSelfOrEnemy_ == Self) ? 9 : 8};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty) {
                    m_teValid.emplace_back(0, suji + dan, isSelfOrEnemy_ | Ky, Empty);
                }
            }
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ke]) {
        for (uint32 suji{1}; suji <= 9; ++suji) {
            uint32 startDan{(isSelfOrEnemy_ == Self) ? 3 : 1};
            uint32 endDan{(isSelfOrEnemy_ == Self) ? 9 : 7};

            for (uint32 dan{startDan}; dan <= endDan; ++dan) {
                if (m_ban[dan + suji] == Empty) {
                    m_teValid.emplace_back(0, suji + dan, isSelfOrEnemy_ | Ke, Empty);
                }
            }
        }
    }

    for (uint32 koma{Gi}; koma <= Hi; ++koma) {
        if (m_holdingKomas[isSelfOrEnemy_ | koma] > 0) {
            for (uint32 suji{1}; suji <= 9; ++suji) {
                for (uint32 dan{1}; dan <= 9; ++dan) {
                    if (m_ban[dan + suji] == Empty) {
                        m_teValid.emplace_back(0, suji + dan, isSelfOrEnemy_ | koma, Empty);
                    }
                }
            }
        }
    }

    return m_teValid.size();
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
        AddMove(isSelfOrEnemy_, from_, 11, pin_, rPin_);
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

uint32 Kyokumen::CountControlSelf(const uint32 pos_) {
    uint32 ret{};
    
    for (uint32 i{}, b{1}, bj{1 << 16}; i < 12; ++i, b <<= 1, bj <<= 1) {
        if (pos_ - Direct[i] < 0 || pos_ - Direct[i] >= 121) {
            continue;
        }

        if (CanMove[i][m_ban[pos_ - Direct[i]]] && m_ban[pos_ - Direct[i]] & Self) {
            ret |= bj;
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]] && m_ban[Search(pos_, -Direct[i])] & Self) {
            ret |= bj;
        }
    }

    return ret;
}

uint32 Kyokumen::CountControlEnemy(const uint32 pos_) {
    uint32 ret{};
    
    for (uint32 i{}, b{1}, bj{1 << 16}; i < 12; ++i, b <<= 1, bj <<= 1) {
        if (pos_ - Direct[i] < 0 || pos_ - Direct[i] >= 121) {
            continue;
        }

        if (CanMove[i][m_ban[pos_ - Direct[i]]] && m_ban[pos_ - Direct[i]] & Enemy) {
            ret |= bj;
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]] && m_ban[Search(pos_, -Direct[i])] & Enemy) {
            ret |= bj;
        }
    }

    return ret;
}

uint32 Kyokumen::CountMove(const uint32 isSelfOrEnemy_, const uint32 pos_) {
    uint32 ret{};

    for (uint32 i{}, b{1}, bj{1 << 16}; i < 12; ++i, b <<= 1, bj <<= 1) {
        if (CanMove[i][m_ban[pos_ - Direct[i]]]
            && m_ban[pos_ - Direct[i]] & isSelfOrEnemy_
            && (m_pin[pos_ - Direct[i]] == 0
                || m_pin[pos_ - Direct[i]] == Direct[i]
                || m_pin[pos_ - Direct[i]] == -Direct[i])) {
            ret |= b;
        }
        else if (CanJump[i][m_ban[Search(pos_, -Direct[i])]]
            && m_ban[Search(pos_, -Direct[i])] & isSelfOrEnemy_
            && (m_pin[Search(pos_, -Direct[i])] == 0
                || m_pin[Search(pos_, -Direct[i])] == Direct[i]
                || m_pin[Search(pos_, -Direct[i])] == -Direct[i])) {
            ret |= bj;
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
        if (m_controlSelf[to_] && CountMove(Enemy, to_) == (1 << 1)) {
            for (uint32 i{}; i < 8; ++i) {
                uint32 koma{m_ban[m_kingEnemyPos + Direct[i]]};
                if (!(koma & Enemy) && !CountControlSelf(m_kingEnemyPos + Direct[i])) {
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
        if (m_controlEnemy[to_] && CountMove(Self, to_) == (1 << 6)) {
            for (uint32 i{}; i < 8; ++i) {
                uint32 koma{m_ban[m_kingSelfPos + Direct[i]]};
                if (!(koma & Self) && !CountControlEnemy(m_kingSelfPos + Direct[i])) {
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
    int32 dan{pos_ % 10};

    if (isSelfOrEnemy_ == Enemy) {
        dan = 10 - dan;
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Fu] > 0 && dan > 1) {
        int32 suji{pos_ / 10};
        bool nifu{false};

        for (uint32 d{1}; d <= 9; ++d) {
            if (m_ban[suji + d] == (isSelfOrEnemy_ | Fu)) {
                nifu = true;
                break;
            }
        }

        if (!nifu && !Uchifudume(isSelfOrEnemy_, pos_)) {
            m_teValid.emplace_back(0, pos_, (isSelfOrEnemy_ | Fu), Empty);
        }
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ky] > 0 && dan > 1) {
        m_teValid.emplace_back(0, pos_, (isSelfOrEnemy_ | Ky), Empty);
    }

    if (m_holdingKomas[isSelfOrEnemy_ | Ke] > 0 && dan > 2) {
        m_teValid.emplace_back(0, pos_, (isSelfOrEnemy_ | Ke), Empty);
    }

    for (uint32 koma{Gi}; koma <= Hi; ++koma) {
        if (m_holdingKomas[isSelfOrEnemy_ | koma] > 0) {
            m_teValid.emplace_back(0, pos_, (isSelfOrEnemy_ | koma), Empty);
        }
    }
}

uint32 Kyokumen::AntiCheck(const uint32 isSelfOrEnemy_, const uint32 control_) {
    if ((control_ & (control_ - 1)) != 0) {
        MoveKing(isSelfOrEnemy_, control_);
    }
    else {
        uint32 king{(isSelfOrEnemy_ == Self) ? m_kingSelfPos : m_kingEnemyPos};

        uint32 id{};

        for (; id <= 32; ++id) {
            if (control_ == (1u << id)) {
                break;
            }
        }

        uint32 check{(id < 16) ? king - Direct[id] : Search(king, -Direct[id - 16])};

        MoveTo(isSelfOrEnemy_, check);

        MoveKing(isSelfOrEnemy_, control_);

        if (id >= 16) {
            for (int32 i{king - Direct[id - 16]}; m_ban[i] == Empty; i -= Direct[id - 16]) {
                MoveTo(isSelfOrEnemy_, i);
            }

            for (int32 i{king - Direct[id - 16]}; m_ban[i] == Empty; i -= Direct[id - 16]) {
                PutTo(isSelfOrEnemy_, i);
            }
        }
    }

    return m_teValid.size();
}

void Kyokumen::MoveKing(const uint32 isSelfOrEnemy_, const uint32 kiki_) {
    int32 id{-1};

    for (uint32 i{}; i < 8; ++i) {
        if (kiki_ & (1 << i)) {
            id = i;
            break;
        }
    }

    if (id >= 0) {
        if (isSelfOrEnemy_ == Self) {
            uint32 koma{m_ban[m_kingSelfPos - Direct[id]]};

            if ((koma == Empty || koma & Enemy)
                && !CountControlEnemy(m_kingSelfPos - Direct[id])
                && !(kiki_ & (1 << (23 - id)))) {
                AddMove(isSelfOrEnemy_, m_kingSelfPos, -Direct[id], 0);
            }
        }
        else {
            uint32 koma{m_ban[m_kingEnemyPos - Direct[id]]};

            if ((koma == Empty || koma & Self)
                && !CountControlSelf(m_kingEnemyPos - Direct[id])
                && !(kiki_ & (1 << (23 - id)))) {
                AddMove(isSelfOrEnemy_, m_kingEnemyPos, -Direct[id], 0);
            }
        }

    }
    for (uint32 i{}; i < 8; ++i) {
        if (i == id) {
            continue;
        }
        if (isSelfOrEnemy_ == Self) {
            uint32 koma{m_ban[m_kingSelfPos - Direct[i]]};
            if ((koma == Empty || koma & Enemy)
                && !CountControlEnemy(m_kingSelfPos - Direct[i])
                && !(kiki_ & (1 << (23 - i)))) {
                AddMove(isSelfOrEnemy_, m_kingSelfPos, -Direct[i], 0);
            }
        }
        else {
            uint32 koma{m_ban[m_kingEnemyPos - Direct[i]]};
            if ((koma == Empty || koma & Self)
                && !CountControlSelf(m_kingEnemyPos - Direct[i])
                && !(kiki_ & (1 << 23 - i))) {
                AddMove(isSelfOrEnemy_, m_kingEnemyPos, -Direct[i], 0);
            }
        }
    }
}
