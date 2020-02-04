
# include "GameAI.hpp"

BanSelf::BanSelf(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const double shogiBan_, const double komaDai_) noexcept
    : m_shogiBan(Arg::center(Scene::CenterF()), shogiBan_)
    , m_komaDaiSelf(Arg::center
        (Scene::CenterF()
            .movedBy(shogiBan_/2+10+komaDai_/2, (shogiBan_/2-komaDai_)+komaDai_/2))
        , komaDai_)
    , m_komaDaiEnemy(Arg::center
        (Scene::CenterF()
            .movedBy(-(shogiBan_/2+10+komaDai_/2), -((shogiBan_/2-komaDai_)+komaDai_/2)))
        , komaDai_)
    , m_turn(Turn::Player)
    , m_kyokumen(0, HirateBan)
{
    // １マスの大きさ
    const double squareSize = shogiBan_ / 9;
    
    // 盤面に駒を設置
    for (size_t y = 0; y < 9; ++y) {
        for (size_t x = 0; x < 9; ++x) {
            m_boardSquares << KomaSquare(
                m_shogiBan.tl().x + x * squareSize
                , m_shogiBan.tl().y + y * squareSize
                , squareSize
                , iniKyokumen_[y][x]
                , Point(x+1, y+1)
            );
        }
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);
    
    m_thinkingTimer.start();
}

void BanSelf::EnemyUpdate() {
    if (m_kyokumen.MakeLegalMoves(Enemy) <= 0 || m_thinkingTimer <= 0.5s) {
        return;
    }

    Te te{m_sikou.Think(Enemy, m_kyokumen, SearchType::NegaAlphaBeta)};
    m_kyokumen.Move(Enemy, te);

    if (te.GetFrom() > 0x10) {
        m_boardSquares[(te.GetFrom()/16)-1 + (te.GetFrom()%16-1)*9].ChangeKomaType(Empty);
    }

    if (m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].GetKomaType() != Empty) {
        uint32 komaType = m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].GetKomaType();
        m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].ChangeKomaType(te.GetKoma());
        
        komaType = ((komaType - Self + Enemy) & ~Promote);
        m_havingEnemyKoma[komaType - Enemy - 1] << KomaSquare(
            KomaPos::enemyDaiPoses[komaType - Enemy - 1]
            , m_komaDaiEnemy.w / 4
            , static_cast<KomaType>(komaType)
            , KomaState::Dai
            , Point(0, 0)
        );
    }
    else {
        m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].ChangeKomaType(te.GetKoma());
    }

    //Print << (te.GetFrom()/16)*10+(te.GetFrom()%16) << U"->" << (te.GetTo()/16)*10+(te.GetTo()%16);
    ChangeCurrentTurn();
}

void BanSelf::SelfAIUpdate() {
    if (m_kyokumen.MakeLegalMoves(Self) <= 0 || m_thinkingTimer <= 0.5s) {
        return;
    }

    Te te{m_sikou.Think(Self, m_kyokumen)};
    m_kyokumen.Move(Self, te);

    if (te.GetFrom() > 0x10) {
        m_boardSquares[(te.GetFrom()/16)-1 + (te.GetFrom()%16-1)*9].ChangeKomaType(Empty);
    }

    if (m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].GetKomaType() != Empty) {
        uint32 komaType = m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].GetKomaType();
        m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].ChangeKomaType(te.GetKoma());
        
        komaType = ((komaType - Enemy + Self) & ~Promote);
        m_havingSelfKoma[komaType - Self - 1] << KomaSquare(
            KomaPos::selfDaiPoses[komaType - Self - 1]
            , m_komaDaiSelf.w / 4
            , static_cast<KomaType>(komaType)
            , KomaState::Dai
            , Point(0, 0)
        );
    }
    else {
        m_boardSquares[(te.GetTo()/16)-1 + (te.GetTo()%16-1)*9].ChangeKomaType(te.GetKoma());
    }
    ChangeCurrentTurn();
    //Print << (te.GetFrom()/16)*10+(te.GetFrom()%16) << U"->" << (te.GetTo()/16)*10+(te.GetTo()%16);
}

// GameクラスのUpdate()で呼び出すメンバ関数
void BanSelf::SelfUpdate() {
    // 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を選んで手に持つ処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                continue;
            }
            if (square.GetKomaType() == Empty) {
                return;
            }
            
            // もしも自分の駒でないもしくは自分のターンでない場合、手に持つ事はできない
            if ((GetTurn() == Turn::Player && square.GetKomaType() > Enemy)
                || (GetTurn() == Turn::Enemy)) {
                    return;
            }

            m_holdHand.emplace(square);
            square.ChangeKomaType(Emp);
            return;
        }
        
        // クリックしてない場合は次の処理に
        if (!square.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            continue;
        }
        
        // 置く場所に駒が存在してた場合、
        // その駒を駒台に置くかどうかを見極める
        if (square.GetKomaType() != Empty) {
            AddHoldKoma(square);
            return;
        }
        
        if (!m_holdHand.value().IsChangeCoodinate(square)) {
            square.ChangeKomaType(m_holdHand.value().GetKomaType());
            m_holdHand.reset();
            return;
        }
        
        // 置く場所に何もなかったら、持ってる駒を置く
        Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 16), static_cast<uint32>(square.GetKomaCoodinate().y + square.GetKomaCoodinate().x * 16), m_holdHand.value().GetKomaType()};
        
        if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
            return;
        }

        if (te.GetFrom() >= 0x11 && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() & 0x0f) <= 3 || (te.GetTo() & 0x0f) <= 3)) {
            if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() & 0x0f) <= 2) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() & 0x0f) <= 1) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
            else if (System::ShowMessageBox(U"成りますか？", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
                te.SetPromote(1);
                if (m_kyokumen.IsIllegal(te)) {
                    //Print << m_kyokumen.GetTeValids().size();
                    return;
                }
                m_holdHand.value().PromoteKoma();
            }
        }
        if (m_kyokumen.IsIllegal(te)) {
            //Print << m_kyokumen.GetTeValids().size();
            return;
        }
        
        ChangeCurrentTurn();
        square.ChangeKomaType(m_holdHand.value().GetKomaType());
        m_kyokumen.Move(Self, te);
        //Print << m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 10 << U"->" << square.GetKomaCoodinate().y + square.GetKomaCoodinate().x * 10;
        m_holdHand.reset();
        return;
    }
    
    // 駒台から取った駒を元の駒台の位置に戻す処理
    // いわゆるキャンセル処理
    if (m_holdHand.has_value()) {
        // 持ってる駒が盤面から取った駒なら、早期リターン
        if (m_holdHand.value().GetKomaState() == KomaState::Ban) {
            return;
        }
        // 何もクリックしてなかったら、処理リターン
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
    
    // プレイヤーの駒台から駒を取る処理
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
    
    // 敵の駒台から駒を取る処理
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

void BanSelf::Draw() const {
    m_shogiBan.draw(Palette::Burlywood);
    m_komaDaiSelf.draw(Palette::Burlywood);
    m_komaDaiEnemy.draw(Palette::Burlywood);
    
    for (const auto &square : m_boardSquares) {
        square.drawFrame(2, Palette::Black);
        if (square.mouseOver()) {
            square.draw(ColorF(Palette::White, 0.5f));
        }
        
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

void BanSelf::AddHoldKoma(KomaSquare& koma_) {
    if ((koma_.GetKomaType() <= Enemy && m_holdHand.value().GetKomaType() <= Enemy)
        || (koma_.GetKomaType() > Enemy && m_holdHand.value().GetKomaType() > Enemy)) {
        return;
    }

    Te te{static_cast<uint32>(m_holdHand.value().GetKomaCoodinate().y + m_holdHand.value().GetKomaCoodinate().x * 16), static_cast<uint32>(koma_.GetKomaCoodinate().y + koma_.GetKomaCoodinate().x * 16), m_holdHand.value().GetKomaType(), koma_.GetKomaType()};
        
    if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
        return;
    }

    if (te.GetFrom() >= 0x11 && (m_holdHand.value().GetKomaType() & Promote) == 0 && CanPromote[m_holdHand.value().GetKomaType()] && ((te.GetFrom() & 0x0f) <= 3 || (te.GetTo() & 0x0f) <= 3)) {
        if (m_holdHand.value().GetKomaType() == Ske && (te.GetTo() & 0x0f) <= 2) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if ((m_holdHand.value().GetKomaType() == Sfu || m_holdHand.value().GetKomaType() == Sky) && (te.GetTo() & 0x0f) <= 1) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
        else if (System::ShowMessageBox(U"成りますか？", MessageBoxButtons::YesNo) == MessageBoxSelection::Yes) {
            te.SetPromote(1);
            if (m_kyokumen.IsIllegal(te)) {
                //Print << m_kyokumen.GetTeValids().size();
                return;
            }
            m_holdHand.value().PromoteKoma();
        }
    }
    if (m_kyokumen.IsIllegal(te)) {
        //Print << m_kyokumen.GetTeValids().size();
        return;
    }

    m_kyokumen.Move(Self, te);
    
    uint32 komaType = koma_.GetKomaType();
    koma_.ChangeKomaType(m_holdHand.value().GetKomaType());
    m_holdHand.reset();
    ChangeCurrentTurn();
    
    // 敵駒にする場合
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
    
    // 自駒にする場合
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

GameAI::GameAI(const InitData& init)
	: IScene(init)
    , m_ban(HirateBan) {}

void GameAI::update()
{
    switch (m_ban.GetTurn()) {
    case Turn::Player:
        m_ban.SelfUpdate();
        //m_ban.SelfAIUpdate();
        break;
    case Turn::Enemy:
        ClearPrint();
        m_ban.EnemyUpdate();
        break;
    }
}

void GameAI::draw() const
{
    m_ban.Draw();
}
