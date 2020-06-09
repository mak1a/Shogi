
# include "Title.hpp"

Title::Title(const InitData& init)
: IScene(init)
, m_soloMatchButton(Arg::center(Scene::Center().movedBy(0, 0)), 300, 60)
, m_twoPlayerMatchButton(Arg::center(Scene::Center().movedBy(0, 100)), 300, 60)
, m_exitButton(Arg::center(Scene::Center().movedBy(0, 200)), 300, 60)
, m_soloMatchTransition(0.4s, 0.2s)
, m_twoPlayerMatchTransition(0.4s, 0.2s)
, m_exitTransition(0.4s, 0.2s) {}

void Title::update() {
    m_soloMatchTransition.update(m_soloMatchButton.mouseOver());
    m_twoPlayerMatchTransition.update(m_twoPlayerMatchButton.mouseOver());
    m_exitTransition.update(m_exitButton.mouseOver());

    if (m_soloMatchButton.mouseOver() || m_exitButton.mouseOver() || m_twoPlayerMatchButton.mouseOver()) {
        Cursor::RequestStyle(CursorStyle::Hand);
    }

    if (m_soloMatchButton.leftClicked()) {
        getData().gameState = State::GameAI;
        changeScene(State::Select);
    }

    if (m_twoPlayerMatchButton.leftClicked()) {
        getData().gameState = State::Game;
        changeScene(State::Select);
    }

    if (m_exitButton.leftClicked()) {
        System::Exit();
    }
}

void Title::draw() const {
    const String titleText = U"将棋";
    const Vec2 center(Scene::Center().x, 120);
    FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), ColorF(0.0, 0.5));
    FontAsset(U"Title")(titleText).drawAt(center);

    m_soloMatchButton.draw(ColorF(1.0, m_soloMatchTransition.value())).drawFrame(2);
    m_twoPlayerMatchButton.draw(ColorF(1.0, m_twoPlayerMatchTransition.value())).drawFrame(2);
    m_exitButton.draw(ColorF(1.0, m_exitTransition.value())).drawFrame(2);

    FontAsset(U"Menu")(U"ひとりで").drawAt(m_soloMatchButton.center(), ColorF(0.25));
    FontAsset(U"Menu")(U"ふたりで").drawAt(m_twoPlayerMatchButton.center(), ColorF(0.25));
    FontAsset(U"Menu")(U"おわる").drawAt(m_exitButton.center(), ColorF(0.25));

    Rect(0, 500, Scene::Width(), Scene::Height() - 500)
        .draw(Arg::top = ColorF(0.0, 0.0), Arg::bottom = ColorF(0.0, 0.5));

}


// 設定シーン
Select::Select(const InitData& init)
: IScene(init)
, m_isCustom(false)
, m_isNifu(false)
, m_isFirstOute(false)
, m_shogiBan(Arg::center(Scene::CenterF().movedBy(0, -100)), 450.f)
, m_enemyDai(Arg::center(345.f, 550.f), 420.f, 80.f)
, m_selfDai(Arg::center(935.f, 550.f), 420.f, 80.f)
, m_komaDaiSelf(Arg::center(Scene::CenterF()
    .movedBy(450.f/2+10+200.f/2, (450.f/2-300.f)+200.f/2)), 200.f)
, m_komaDaiEnemy(Arg::center(Scene::CenterF()
    .movedBy(-(450.f/2+10+200.f/2), -((450.f/2-100.f)+200.f/2))), 200.f) {
    getData().firstMove = Turn::Player;
    getData().elegance = Elegance::Player;
    getData().handicap = Handicap::Hirate;
    getData().depthMax = 1;
    getData().SetCustomBoard(Board::Custom);
    getData().motigomas.fill(0);

    // １マスの大きさ
    const double squareSize = 50.0;

    for (size_t x = 0; x < 7; ++x) {
        m_selectEnemyKomas << KomaSquare(
            m_enemyDai.tl().x + x * 60 + 5
            , 525.f
            , squareSize
            , static_cast<KomaType>(x+1)+KomaType::Enemy
            , Point(0, 0)
        );
    }

    for (size_t x = 0; x < 7; ++x) {
        m_selectSelfKomas << KomaSquare(
            m_selfDai.tl().x + x * 60 + 5
            , 525.f
            , squareSize
            , static_cast<KomaType>(x+1)+KomaType::Self
            , Point(0, 0)
        );
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);
}

void Select::SetUp() {
    SimpleGUI::Headline(U"手番", Vec2(480, 50));
    {
        if (SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, Vec2(480, 90))) {
            getData().firstMove = static_cast<Turn>(firstMoveIndx);
        }
    }
    SimpleGUI::Headline(U"駒落ち", Vec2(740, 50));
    {
        if (SimpleGUI::HorizontalRadioButtons(eleganceIndx, {U"自分", U"相手"}, Vec2(740, 90))) {
            getData().elegance = static_cast<Elegance>(eleganceIndx);
        }
    }
    //SimpleGUI::Headline(U"手合", Vec2(760, 100));
    {
        if (SimpleGUI::RadioButtons(handicapIndx, handicaps, Vec2(740, 135))) {
            getData().handicap = static_cast<Handicap>(handicapIndx);
        }
    }
    if (getData().gameState == State::GameAI) {
        SimpleGUI::Headline(U"強さ", Vec2(480, 240));
        {
            if (SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級"}, Vec2(480, 280))) {
                getData().depthMax = static_cast<uint32>(depthMaxIndx+1);
            }
        }
    }
    if (SimpleGUI::Button(U"ゲームスタート", Vec2(800, 530), 200)) {
        if (getData().handicap == Handicap::Custom) {
            m_isCustom = true;
            // １マスの大きさ
            const double squareSize = 50.0;
            
            // 盤面に駒を設置
            for (size_t y = 0; y < 9; ++y) {
                for (size_t x = 0; x < 9; ++x) {
                    m_boardSquares << KomaSquare(
                        m_shogiBan.tl().x + x * squareSize
                        , m_shogiBan.tl().y + y * squareSize
                        , squareSize
                        , getData().GetBoard()[y][x]
                        , Point(9-x, y+1)
                    );
                }
            }
            return;
        }
        changeScene(getData().gameState);
    }
}


namespace Custom {
    constexpr Vec2 selfFu = Vec2(875.0, 285.0);
    constexpr Vec2 enemyFu = Vec2(355.0, 35.0);
    
    constexpr Vec2 selfKyo = Vec2(875.0, 335.0);
    constexpr Vec2 enemyKyo = Vec2(355.0, 85.0);

    constexpr Vec2 selfKei = Vec2(975.0, 335.0);
    constexpr Vec2 enemyKei = Vec2(255.0, 85.0);

    constexpr Vec2 selfGin = Vec2(875.0, 385.0);
    constexpr Vec2 enemyGin = Vec2(355.0, 135.0);

    constexpr Vec2 selfKin = Vec2(975.0, 385.0);
    constexpr Vec2 enemyKin = Vec2(255.0, 135.0);

    constexpr Vec2 selfKaku = Vec2(875.0, 435.0);
    constexpr Vec2 enemyKaku = Vec2(355.0, 185.0);

    constexpr Vec2 selfHi = Vec2(975.0, 435.0);
    constexpr Vec2 enemyHi = Vec2(255.0, 185.0);

    /// <summary>
    /// 自分の持ち駒の場所
    /// </summary>
    constexpr array<const Vec2, 7> selfDaiPoses = {
        selfFu, selfKyo, selfKei, selfGin, selfKin, selfKaku, selfHi
    };
    /// <summary>
    /// 敵の持ち駒の場所
    /// </summary>
    constexpr array<const Vec2, 7> enemyDaiPoses = {
        enemyFu, enemyKyo, enemyKei, enemyGin, enemyKin, enemyKaku, enemyHi
    };
}

void Select::Custom() {
    // 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を消す処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                if (square.mouseOver() && square.GetKomaType() != Empty && square.GetKomaType() != Eou && square.GetKomaType() != Sou) {
                    Cursor::RequestStyle(CursorStyle::Hand);
                }
                continue;
            }
            if (square.GetKomaType() == Empty) {
                return;
            }

            m_isNifu = false;
            m_isFirstOute = false;

            if (square.GetKomaType() == Eou || square.GetKomaType() == Sou) {
                m_holdHand.emplace(square);
                square.ChangeKomaType(Emp);
                return;
            }
            
            square.ChangeKomaType(Emp);
            return;
        }
        
        // クリックしてない場合は次の処理に
        if (!square.leftClicked()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            continue;
        }
        if (square.GetKomaType() == Eou || square.GetKomaType() == Sou) {
            return;
        }
        
        // 持ってる駒を置く
        // ただし、動かせない部分には置けないようにする
        if (m_holdHand.value().GetKomaType() == Sfu && square.GetKomaCoodinate().y <= 1) {
            return;
        }
        if (m_holdHand.value().GetKomaType() == Sky && square.GetKomaCoodinate().y <= 1) {
            return;
        }
        if (m_holdHand.value().GetKomaType() == Ske && square.GetKomaCoodinate().y <= 2) {
            return;
        }
        if (m_holdHand.value().GetKomaType() == Efu && square.GetKomaCoodinate().y >= 9) {
            return;
        }
        if (m_holdHand.value().GetKomaType() == Eky && square.GetKomaCoodinate().y >= 9) {
            return;
        }
        if (m_holdHand.value().GetKomaType() == Eke && square.GetKomaCoodinate().y >= 8) {
            return;
        }
        if (KeySpace.pressed()) {
            m_holdHand.value().PromoteKoma();
        }
        square.ChangeKomaType(m_holdHand.value().GetKomaType());

        if (!KeyShift.pressed() || m_holdHand.value().GetKomaType() == Sou || m_holdHand.value().GetKomaType() == Eou) {
            m_holdHand.reset();
            return;
        }

        m_holdHand.value().ReturnPromoteKoma();
        return;
    }
    
    // 駒台に持ってる駒を置く処理
    if (m_holdHand.has_value()) {
        // 何もクリックしてなかったら、処理リターン
        if (!MouseL.down()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            return;
        }
        if (m_holdHand.value().GetKomaType() == Eou || m_holdHand.value().GetKomaType() == Sou) {
            m_holdHand.value().setCenter(Cursor::PosF());
            return;
        }

        m_holdHand.value().ReturnPromoteKoma();

        if (m_komaDaiSelf.leftClicked()) {
            if ((m_holdHand.value().GetKomaType() & Self) == 0) {
                return;
            }
            m_havingSelfKoma[m_holdHand.value().GetKomaType() - Self - 1] << KomaSquare(
                Custom::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1]
                , m_komaDaiSelf.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Point(0, 0)
            );
        }
        else if (m_komaDaiEnemy.leftClicked()) {
            if ((m_holdHand.value().GetKomaType() & Enemy) == 0) {
                return;
            }
            m_havingEnemyKoma[m_holdHand.value().GetKomaType() - Enemy - 1] << KomaSquare(
                Custom::enemyDaiPoses[m_holdHand.value().GetKomaType() - Enemy - 1]
                , m_komaDaiEnemy.w / 4
                , m_holdHand.value().GetKomaType()
                , KomaState::Dai
                , Point(0, 0)
            );
        }
        
        if (!KeyShift.pressed()) {
            m_holdHand.reset();
        }
        return;
    }
    
    // プレイヤーの駒選択台から駒を取る処理
    for (auto& selfKoma : m_selectSelfKomas) {
        if (!selfKoma.leftClicked()) {
            if (selfKoma.mouseOver()) {
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            continue;
        }
        m_holdHand.emplace(selfKoma);
        return;
    }
    
    // 敵の駒選択台から駒を取る処理
    for (auto& enemyKoma : m_selectEnemyKomas) {
        if (!enemyKoma.leftClicked()) {
            if (enemyKoma.mouseOver()) {
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            continue;
        }
        m_holdHand.emplace(enemyKoma);
        return;
    }

    // プレイヤーの駒台から駒を取る処理
    for (auto& havingSelfKoma : m_havingSelfKoma) {
        for (auto [i, koma] : IndexedRef(havingSelfKoma)) {
            if (koma.mouseOver()) {
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            if (!koma.leftClicked()) {
                continue;
            }

            havingSelfKoma.remove_at(i);
            return;
        }
    }
    
    // 敵の駒台から駒を取る処理
    for (auto& havingEnemyKoma : m_havingEnemyKoma) {
        for (auto [i, koma] : IndexedRef(havingEnemyKoma)) {
            if (koma.mouseOver()) {
                Cursor::RequestStyle(CursorStyle::Hand);
            }
            if (!koma.leftClicked()) {
                continue;
            }

            havingEnemyKoma.remove_at(i);
            return;
        }
    }
}

void Select::update() {
    if (m_isCustom) {
        if (SimpleGUI::Button(U"ゲームスタート", Vec2(900, 620), 200)) {
            array<array<uint32, 9>, 9> boardCustom;

            // 盤面に駒を設置
            for (size_t x{}; x < 9; ++x) {
                uint32 selfFuNum{}, enemyFuNum{};
                for (size_t y{}; y < 9; ++y) {
                    if (m_boardSquares[9*y+x].GetKomaType() == KomaType::Sfu) {
                        ++selfFuNum;
                    }
                    if (m_boardSquares[9*y+x].GetKomaType() == KomaType::Efu) {
                        ++enemyFuNum;
                    }
                    if (selfFuNum >= 2 || enemyFuNum >= 2) {
                        m_isNifu = true;
                        return;
                    }

                    boardCustom[y][x] = m_boardSquares[9*y+x].GetKomaType();
                }
            }
            // 初期状態で先手が王手の場合、ゲームを開始しない
            Kyokumen k(0, boardCustom);
            k.MakeLegalMoves((getData().firstMove==Turn::Player)?Enemy:Self);
            if (k.IsOute((getData().firstMove==Turn::Player)?Turn::Enemy:Turn::Player)) {
                m_isFirstOute = true;
                return;
            }

            getData().SetCustomBoard(boardCustom);

            // 持ち駒を渡す
            for (const auto [i, havingSelfKoma] : IndexedRef(m_havingSelfKoma)) {
                for (const auto& motigoma : havingSelfKoma) {
                    ++getData().motigomas[(i + 1) + Self];
                }
            }
            for (const auto [i, havingEnemyKoma] : IndexedRef(m_havingEnemyKoma)) {
                for (const auto& motigoma : havingEnemyKoma) {
                    ++getData().motigomas[(i + 1) + Enemy];
                }
            }

            changeScene(getData().gameState);
            return;
        }

        Custom();
        return;
    }
    
    SetUp();
}

/// <summary>
/// フェードインの時に描画されないので仕方なく
/// </summary>
void Select::updateFadeIn(double) {
    SimpleGUI::Headline(U"手番", Vec2(480, 50));
    {
        SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, Vec2(480, 90));
    }
    SimpleGUI::Headline(U"駒落ち", Vec2(740, 50));
    {
        SimpleGUI::HorizontalRadioButtons(eleganceIndx, {U"自分", U"相手"}, Vec2(740, 90));
    }
    //SimpleGUI::Headline(U"手合", Vec2(760, 100));
    {
        SimpleGUI::RadioButtons(handicapIndx, handicaps, Vec2(740, 135));
    }
    if (getData().gameState == State::GameAI) {
        SimpleGUI::Headline(U"強さ", Vec2(480, 240));
        {
            SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級"}, Vec2(480, 280));
        }
    }
    SimpleGUI::Button(U"ゲームスタート", Vec2(800, 530), 200);
}

void Select::draw() const {
    if (!m_isCustom) {
        return;
    }

    m_shogiBan.draw(Palette::Burlywood);
    m_enemyDai.draw(Palette::Burlywood);
    m_selfDai.draw(Palette::Burlywood);
    m_komaDaiEnemy.draw(Palette::Burlywood);
    m_komaDaiSelf.draw(Palette::Burlywood);

    SimpleGUI::Headline(U"敵駒", m_enemyDai.tl().movedBy(0, -40));
    SimpleGUI::Headline(U"自駒", m_selfDai.tr().movedBy(-60, -40));

    for (const auto& square : m_boardSquares) {
        square.drawFrame(2, Palette::Black);
        if (square.mouseOver()) {
            square.draw(ColorF(Palette::Red, 0.5f));
        }
        
        square.Draw();
    }

    for (const auto& enemyKoma : m_selectEnemyKomas) {
        enemyKoma.Draw();
    }

    for (const auto& selfKoma : m_selectSelfKomas) {
        selfKoma.Draw();
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

    FontAsset(U"Explain")(U"初期盤面を自由に制作できます。").draw(50, 280, Palette::Black);
    FontAsset(U"Explain")(U"Shiftキーを押したまま選択した駒を\n置くと連続で置く事ができ、\nSpaceキーを押したまま選択した駒を\n置くと駒を成る事ができます。").draw(50, 320, Palette::Black);

    if (m_isNifu) {
        FontAsset(U"Warning")(U"二歩です！置き直してください！").draw(320, 620, Palette::Red);
    }
    if (m_isFirstOute) {
        FontAsset(U"Warning")(U"先手が王手の状態で\n開始できません。").draw(915, 120, Palette::Red);
    }
}
