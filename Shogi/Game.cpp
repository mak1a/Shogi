
# include "Game.hpp"

Ban::Ban(const array<array<uint32, 9>, 9>& iniKyokumen_, const Turn& turn_, const double shogiBan_, const double komaDai_) noexcept
    : m_shogiBan(Arg::center(Scene::CenterF()), shogiBan_)
    , m_komaDaiSelf(Arg::center(Scene::CenterF()
        .movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, (shogiBan_ / 2 - komaDai_) + komaDai_ / 2)), komaDai_)
    , m_komaDaiEnemy(Arg::center(Scene::CenterF()
        .movedBy(-(shogiBan_ / 2 + 10 + komaDai_ / 2), -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), komaDai_)
    , m_buttonWaited(Arg::center(Scene::CenterF()
        .movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2) - 1.5 * (shogiBan_ / 9))), Vec2(komaDai_, shogiBan_ / 12))
    , m_buttonQuit(Arg::center(Scene::CenterF()
        .movedBy(shogiBan_ / 2 + 10 + komaDai_ / 2, -((shogiBan_ / 2 - komaDai_) + komaDai_ / 2))), Vec2(komaDai_, shogiBan_ / 12))
    , m_turn(turn_)
    , m_isBehind((turn_ == Turn::Enemy))
    , m_kyokumen(0, iniKyokumen_) {
    // �P�}�X�̑傫��
    const double squareSize = shogiBan_ / 9;

    // �Ֆʂɋ��ݒu
    for (size_t y = 0; y < 9; ++y) {
        for (size_t x = 0; x < 9; ++x) {
            m_boardSquares << KomaSquare(
                m_shogiBan.tl().x + x * squareSize
                , m_shogiBan.tl().y + y * squareSize
                , squareSize
                , iniKyokumen_[y][x]
                , Point(9 - x, y + 1)
            );
        }
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);

    // �J�X�^���ݒ�̏ꍇ�A������Ԃŋl�݂̉\�������݂���̂ł��̊m�F
    if (m_kyokumen.MakeLegalMoves((turn_ == Turn::Player) ? Self : Enemy) <= 0) {
        m_turn = Turn::Tsumi;
        m_winner = (turn_ == Turn::Player) ? Winner::Enemy : Winner::Player;
        return;
    }

    m_stackKyokumens.emplace(m_kyokumen);
    m_stackBoradSquares.emplace(m_boardSquares);
    m_stackHavingSelf.emplace(m_havingSelfKoma);
    m_stackHavingEnemy.emplace(m_havingEnemyKoma);
    m_stackPlacedPart.emplace(m_placedPart);
}

// Game�N���X��Update()�ŌĂяo�������o�֐�
void Ban::Update() {
    if (m_buttonWaited.mouseOver() || m_buttonQuit.mouseOver()) {
        Cursor::RequestStyle(CursorStyle::Hand);
    }

    if (!m_holdHand.has_value() && m_buttonQuit.leftClicked()) {
        if (GetTurn() == Turn::Player) {
            m_winner = Winner::Enemy;
        }
        else {
            m_winner = Winner::Player;
        }
        m_turn = Turn::Tsumi;
        return;
    }

    if (!m_holdHand.has_value() && GetTurn() == Turn::Player && m_buttonWaited.leftClicked()) {
        RetractingMove();
        return;
    }

    // �Ֆʏ�̏���
    for (auto& square : m_boardSquares) {
        // �Ֆʂ�����I��Ŏ�Ɏ�����
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                continue;
            }
            if (square.GetKomaType() == Empty) {
                return;
            }

            // �����������̋�łȂ��������͎����̃^�[���łȂ��ꍇ�A��Ɏ����͂ł��Ȃ�
            if ((GetTurn() == Turn::Player && square.GetKomaType() > Enemy)
                || (GetTurn() == Turn::Enemy && square.GetKomaType() <= Enemy)) {
                return;
            }

            m_holdHand.emplace(square);
            square.ChangeKomaType(Emp);
            return;
        }

        // �N���b�N���ĂȂ��ꍇ�͎��̏�����
        if (!square.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            continue;
        }

        // �u���ꏊ�ɋ���݂��Ă��ꍇ�A
        // ���̋�����ɒu�����ǂ��������ɂ߂�
        if (square.GetKomaType() != Empty) {
            AddHoldKoma(square);
            return;
        }

        if (!m_holdHand.value().IsChangeCoodinate(square)) {
            square.ChangeKomaType(m_holdHand.value().GetKomaType());
            m_holdHand.reset();
            return;
        }

        // �u���ꏊ�ɉ����Ȃ�������A�����Ă���u��
        Te te{ static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10), static_cast<uint32>(square.GetKomaCoodinate().y + square.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType() };

        if (te.GetFrom() >= 11 && GetTurn() == Turn::Player && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() % 10) <= 3 || (te.GetTo() % 10) <= 3)) {
            if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() % 10) <= 2) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() % 10) <= 1) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else {
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                if (System::ShowMessageBox(U"����܂����H", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
                    te.SetPromote(1);
                    if (m_kyokumen.IsIllegal(te)) {
                        return;
                    }
                    m_holdHand.value().PromoteKoma();
                }
            }
        }
        if (te.GetFrom() >= 11 && GetTurn() == Turn::Enemy && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() % 10) >= 7 || (te.GetTo() % 10) >= 7)) {
            if (m_holdHand.value().GetKomaType() == Eke && (te.GetTo() % 10) >= 8) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if ((m_holdHand.value().GetKomaType() == Efu || m_holdHand.value().GetKomaType() == Eky) && (te.GetTo() % 10) >= 9) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else {
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                if (System::ShowMessageBox(U"����܂����H", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
                    te.SetPromote(1);
                    if (m_kyokumen.IsIllegal(te)) {
                        return;
                    }
                    m_holdHand.value().PromoteKoma();
                }
            }
        }
        if (m_kyokumen.IsIllegal(te)) {
            //Print << m_kyokumen.GetTeValids().size();
            return;
        }

        // �u���ꏊ�ɉ����Ȃ�������A�����Ă���u��
        square.ChangeKomaType(m_holdHand.value().GetKomaType());

        // �u������̏ꏊ���X�V����
        m_placedPart.reset(square);

        m_kyokumen.Move(((GetTurn() == Turn::Player) ? Self : Enemy), te);
        m_holdHand.reset();

        ChangeCurrentTurn();
        return;
    }

    // ��䂩������������̋��̈ʒu�ɖ߂�����
    // ������L�����Z������
    if (m_holdHand.has_value()) {
        // �����Ă��Ֆʂ���������Ȃ�A�������^�[��
        if (m_holdHand.value().GetKomaState() == KomaState::Ban) {
            return;
        }
        // �����N���b�N���ĂȂ�������A�������^�[��
        if (!m_komaDaiSelf.leftClicked()
            && !m_komaDaiEnemy.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            return;
        }

        if (m_komaDaiSelf.leftClicked()) {
            m_havingSelfKoma[m_holdHand.value().GetKomaType() - Self - 1] << KomaSquare(
                KomaPos::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1]
                , m_komaDaiSelf.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Point(0, 0)
            );
        }
        else {
            m_havingEnemyKoma[m_holdHand.value().GetKomaType() - Enemy - 1] << KomaSquare(
                KomaPos::enemyDaiPoses[m_holdHand.value().GetKomaType() - Enemy - 1]
                , m_komaDaiEnemy.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Point(0, 0)
            );
        }

        m_holdHand.reset();
        return;
    }

    // �v���C���[�̋�䂩������鏈��
    for (auto& havingSelfKoma : m_havingSelfKoma) {
        for (auto [i, koma] : IndexedRef(havingSelfKoma)) {
            if (!koma.leftClicked()) {
                continue;
            }
            m_holdHand.emplace(koma);
            havingSelfKoma.remove_at(i);
            return;
        }
    }

    // �G�̋�䂩������鏈��
    for (auto& havingEnemyKoma : m_havingEnemyKoma) {
        for (auto [i, koma] : IndexedRef(havingEnemyKoma)) {
            if (!koma.leftClicked()) {
                continue;
            }
            m_holdHand.emplace(koma);
            havingEnemyKoma.remove_at(i);
            return;
        }
    }
}

void Ban::Draw() const {
    m_shogiBan.draw(Palette::Burlywood);
    m_komaDaiSelf.draw(Palette::Burlywood);
    m_komaDaiEnemy.draw(Palette::Burlywood);
    m_buttonWaited.draw(Palette::White);
    m_buttonWaited.drawFrame(1.0, Palette::Black);
    m_buttonQuit.draw(Palette::White);
    m_buttonQuit.drawFrame(1.0, Palette::Black);

    FontAsset(U"Menu")(U"�҂���").drawAt(m_buttonWaited.center(), Palette::Black);
    FontAsset(U"Menu")(U"����").drawAt(m_buttonQuit.center(), Palette::Black);

    // ���u�����ꏊ�������Ԃ�����
    if (m_placedPart.has_value()) {
        m_placedPart.value().draw(ColorF(Palette::Red, 0.5f));
    }

    for (const auto& square : m_boardSquares) {
        square.drawFrame(2, Palette::Black);

        square.Draw();
    }

    for (const auto& selfDai : m_havingSelfKoma) {
        for (const auto [i, koma] : IndexedRefReversed(selfDai)) {
            koma.Draw(Vec2(i * 10, 0));
        }
    }
    for (const auto& enemyDai : m_havingEnemyKoma) {
        for (const auto [i, koma] : IndexedRefReversed(enemyDai)) {
            koma.Draw(Vec2(static_cast<int32>(i) * -10, 0));
        }
    }

    if (m_holdHand.has_value()) {
        m_holdHand.value().Draw();
    }
}

void Ban::AddHoldKoma(KomaSquare& koma_) {
    if ((koma_.GetKomaType() <= Enemy && m_holdHand.value().GetKomaType() <= Enemy)
        || (koma_.GetKomaType() > Enemy&& m_holdHand.value().GetKomaType() > Enemy)) {
        return;
    }

    Te te{ static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10), static_cast<uint32>(koma_.GetKomaCoodinate().y + koma_.GetKomaCoodinate().x * 10), m_holdHand.value().GetKomaType(), koma_.GetKomaType() };

    if (te.GetFrom() >= 11 && GetTurn() == Turn::Player && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() % 10) <= 3 || (te.GetTo() % 10) <= 3)) {
        if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() % 10) <= 2) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() % 10) <= 1) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else {
            if (m_kyokumen.IsIllegal(te)) {
                return;
            }
            if (System::ShowMessageBox(U"����܂����H", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
        }
    }
    if (te.GetFrom() >= 11 && GetTurn() == Turn::Enemy && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() % 10) >= 7 || (te.GetTo() % 10) >= 7)) {
        if (m_holdHand.value().GetKomaType() == Eke && (te.GetTo() % 10) >= 8) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if ((m_holdHand.value().GetKomaType() == Efu || m_holdHand.value().GetKomaType() == Eky) && (te.GetTo() % 10) >= 9) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else {
            if (m_kyokumen.IsIllegal(te)) {
                return;
            }
            if (System::ShowMessageBox(U"����܂����H", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
        }
    }
    if (m_kyokumen.IsIllegal(te)) {
        return;
    }

    m_kyokumen.Move(((GetTurn() == Turn::Player) ? Self : Enemy), te);

    uint32 komaType = koma_.GetKomaType();
    koma_.ChangeKomaType(m_holdHand.value().GetKomaType());

    // �u������̏ꏊ���X�V����
    m_placedPart.reset(koma_);

    m_holdHand.reset();
    ChangeCurrentTurn();

    // �G��ɂ���ꍇ
    if (komaType <= Enemy) {
        komaType = ((komaType - Self + Enemy) & ~Promote);
        m_havingEnemyKoma[komaType - Enemy - 1] << KomaSquare(
            KomaPos::enemyDaiPoses[komaType - Enemy - 1]
            , m_komaDaiEnemy.w / 4
            , static_cast<KomaType>(komaType)
            , KomaState::Dai
            , Point(0, 0)
        );
        return;
    }

    // ����ɂ���ꍇ
    komaType = ((komaType - Enemy + Self) & ~Promote);
    m_havingSelfKoma[komaType - Self - 1] << KomaSquare(
        KomaPos::selfDaiPoses[komaType - Self - 1]
        , m_komaDaiSelf.w / 4
        , static_cast<KomaType>(komaType)
        , KomaState::Dai
        , Point(0, 0)
    );
    return;
}

Game::Game(const InitData& init)
    : IScene(init)
    , m_ban(getData().GetBoard(), getData().firstMove) {}

void Game::update() {
    if (m_ban.GetTurn() != Turn::Tsumi) {
        m_ban.Update();
    }
    else {
        result();
    }
}

void Game::draw() const {
    m_ban.Draw();

    if (m_ban.GetTurn() == Turn::Tsumi) {
        if (m_ban.GetWinner() == Winner::Player) {
            FontAsset(U"Result")(U"����").drawAt(Scene::CenterF().movedBy(0, -100), Palette::Red);
        }
        else if (m_ban.GetWinner() == Winner::Enemy) {
            FontAsset(U"Result")(U"�s�k").drawAt(Scene::CenterF().movedBy(0, -100), Palette::Blue);
        }
        else {
            FontAsset(U"Result")(U"�����ł�").drawAt(Scene::CenterF().movedBy(0, -100), Palette::Black);
        }
        FontAsset(U"Explain")(U"��ʂ��N���b�N�Ń^�C�g���ɖ߂�").drawAt(Scene::CenterF().movedBy(0, 50), Palette::Darkred);
    }
}

void Game::result() {
    if (MouseL.down()) {
        changeScene(State::Title, 1s);
    }
}
