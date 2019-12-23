#include "Kyokumen.hpp"

Kyokumen::Kyokumen(const uint32 tesu_, const array<array<uint32, 9>, 9>& board_, const array<uint32, 41>& motigoma_)
: m_kingSelfPos(0)
, m_kingEnemyPos(0)
, m_tesu(tesu_)
, m_value(0) {
    m_ban.fill(Wall);

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
                for (uint32 i{}, b{1}, bj{1 << 16}; i <= 11; ++i, b <= 1, bj <= 1) {
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
                for (uint32 i{}, b{1}, bj{1 << 16}; i <= 11; ++i, b <= 1, bj <= 1) {
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
