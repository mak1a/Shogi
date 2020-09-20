
#include "Title.hpp"

Title::Title(const InitData& init)
: IScene(init)
, m_soloMatchButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 0)), 300, 60)
, m_twoPlayerMatchButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 100)), 300, 60)
, m_exitButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 200)), 300, 60)
, m_soloMatchTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_twoPlayerMatchTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_exitTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2)) {
    getData().firstMove = Turn::Player;
    getData().elegance = Elegance::Player;
    getData().handicap = Handicap::Hirate;
    getData().depthMax = 1;
    getData().SetCustomBoard(Board::Custom);
    getData().motigomas.fill(0);
    getData().photonState = PhotonState::None;

    s3d::ClearPrint();
}

void Title::update() {
    m_soloMatchTransition.update(m_soloMatchButton.mouseOver());
    m_twoPlayerMatchTransition.update(m_twoPlayerMatchButton.mouseOver());
    m_exitTransition.update(m_exitButton.mouseOver());

    if (m_soloMatchButton.mouseOver() || m_exitButton.mouseOver() || m_twoPlayerMatchButton.mouseOver()) {
        s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
    }

    if (m_soloMatchButton.leftClicked()) {
        getData().gameState = State::PlayAlone;
        changeScene(State::Select);
    }

    if (m_twoPlayerMatchButton.leftClicked()) {
        getData().gameState = State::OnlineMatch;
        changeScene(State::Match);
    }

    if (m_exitButton.leftClicked()) {
        s3d::System::Exit();
    }
}

void Title::draw() const {
    const s3d::String titleText = U"将棋";
    const s3d::Vec2 center(s3d::Scene::Center().x, 120);
    s3d::FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), s3d::ColorF(0.0, 0.5));
    s3d::FontAsset(U"Title")(titleText).drawAt(center);

    m_soloMatchButton.draw(s3d::ColorF(1.0, m_soloMatchTransition.value())).drawFrame(2);
    m_twoPlayerMatchButton.draw(s3d::ColorF(1.0, m_twoPlayerMatchTransition.value())).drawFrame(2);
    m_exitButton.draw(s3d::ColorF(1.0, m_exitTransition.value())).drawFrame(2);

    s3d::FontAsset(U"Menu")(U"ひとりで").drawAt(m_soloMatchButton.center(), s3d::ColorF(0.25));
    s3d::FontAsset(U"Menu")(U"ふたりで").drawAt(m_twoPlayerMatchButton.center(), s3d::ColorF(0.25));
    s3d::FontAsset(U"Menu")(U"おわる").drawAt(m_exitButton.center(), s3d::ColorF(0.25));

    s3d::Rect(0, 500, s3d::Scene::Width(), s3d::Scene::Height() - 500).draw(s3d::Arg::top = s3d::ColorF(0.0, 0.0), s3d::Arg::bottom = s3d::ColorF(0.0, 0.5));
}


// 設定シーン
Select::Select(const InitData& init)
: IScene(init)
, m_isCustom(false)
, m_isNifu(false)
, m_isFirstOute(false)
, m_shogiBan(s3d::Arg::center(s3d::Scene::CenterF().movedBy(0, -100)), 450.f)
, m_enemyDai(s3d::Arg::center(345.f, 550.f), 420.f, 80.f)
, m_selfDai(s3d::Arg::center(935.f, 550.f), 420.f, 80.f)
, m_komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(450.f / 2 + 10 + 200.f / 2, (450.f / 2 - 300.f) + 200.f / 2)), 200.f)
, m_komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(450.f / 2 + 10 + 200.f / 2), -((450.f / 2 - 100.f) + 200.f / 2))), 200.f)
, m_csv(s3d::Resource(U""))
//, m_csv(s3d::Dialog::OpenFile().value())
, m_isReadCsv(false) {
    // １マスの大きさ
    const double squareSize = 50.0;

    for (size_t x = 0; x < 7; ++x) {
        m_selectEnemyKomas << KomaSquare(m_enemyDai.tl().x + x * 60 + 5, 525.f, squareSize, static_cast<KomaType>(x + 1) + KomaType::Enemy, s3d::Point(0, 0));
    }

    for (size_t x = 0; x < 7; ++x) {
        m_selectSelfKomas << KomaSquare(m_selfDai.tl().x + x * 60 + 5, 525.f, squareSize, static_cast<KomaType>(x + 1) + KomaType::Self, s3d::Point(0, 0));
    }

    m_havingSelfKoma.resize(7);
    m_havingEnemyKoma.resize(7);
}

void Select::ParseInit() {
    if (!m_csv) {
        return;
    }

    // s3d::Print << m_csv.rows() << U", " << m_csv.columns(0);

    if (m_csv.rows() < 9) {
        s3d::Print << U"行数不足 : " << m_csv.rows() + 1 << U"行しかありません。";
        return;
    }

    m_parseData.emplace(U"Sfu", Sfu);
    m_parseData.emplace(U"Sto", Sto);
    m_parseData.emplace(U"Sky", Sky);
    m_parseData.emplace(U"Sny", Sny);
    m_parseData.emplace(U"Ske", Ske);
    m_parseData.emplace(U"Snk", Snk);
    m_parseData.emplace(U"Sgi", Sgi);
    m_parseData.emplace(U"Sng", Sng);
    m_parseData.emplace(U"Ski", Ski);
    m_parseData.emplace(U"Ska", Ska);
    m_parseData.emplace(U"Sum", Sum);
    m_parseData.emplace(U"Shi", Shi);
    m_parseData.emplace(U"Sry", Sry);
    m_parseData.emplace(U"Sou", Sou);

    m_parseData.emplace(U"Efu", Efu);
    m_parseData.emplace(U"Eto", Eto);
    m_parseData.emplace(U"Eky", Eky);
    m_parseData.emplace(U"Eny", Eny);
    m_parseData.emplace(U"Eke", Eke);
    m_parseData.emplace(U"Enk", Enk);
    m_parseData.emplace(U"Egi", Egi);
    m_parseData.emplace(U"Eng", Eng);
    m_parseData.emplace(U"Eki", Eki);
    m_parseData.emplace(U"Eka", Eka);
    m_parseData.emplace(U"Eum", Eum);
    m_parseData.emplace(U"Ehi", Ehi);
    m_parseData.emplace(U"Ery", Ery);
    m_parseData.emplace(U"Eou", Eou);

    array<array<uint32, 9>, 9> boardCustom;
    bool isExistSelfKing{false};
    bool isExistEnemyKing{false};

    for (uint32 y{}; y < 9; ++y) {
        if (m_csv.columns(y) != 9) {
            s3d::Print << U"列数不足 : " << y + 1 << U"行目";
            return;
        }
        for (uint32 x{}; x < 9; ++x) {
            const auto koma = m_parseData.find(m_csv[y][x]);
            if (koma == m_parseData.end()) {
                boardCustom[y][x] = Emp;
                continue;
            }

            boardCustom[y][x] = koma.value();

            if (koma.value() == Sou) {
                isExistSelfKing = true;
            }
            if (koma.value() == Eou) {
                isExistEnemyKing = true;
            }
        }
    }

    if (!isExistSelfKing || !isExistEnemyKing) {
        return;
    }

    if (m_csv.rows() == 9) {
        getData().SetCustomBoard(boardCustom);
        m_isReadCsv = true;
        return;
    }

    s3d::Array<s3d::String> motigomaData = m_csv[9][0].split(U',');
    array<uint32, 40> motigomas;
    motigomas.fill(0);
    for (const auto& komaName : motigomaData) {
        const auto koma = m_parseData.find(komaName);
        if (koma == m_parseData.end()) {
            continue;
        }

        if (koma.value() == Sou || koma.value() == Eou) {
            continue;
        }
        if ((koma.value() & Promote) > 0) {
            s3d::Print << U"成駒を持ち駒にできません。";
            return;
        }

        ++motigomas[koma.value()];
    }

    getData().SetCustomBoard(boardCustom);
    for (uint32 i{}; i < 40; ++i) {
        getData().motigomas[i] = motigomas[i];
    }
    m_isReadCsv = true;
}

void Select::SetUp() {
    s3d::SimpleGUI::Headline(U"手番", s3d::Vec2(480, 50));
    {
        if (s3d::SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, s3d::Vec2(480, 90))) {
            getData().firstMove = static_cast<Turn>(firstMoveIndx);
        }
    }
    s3d::SimpleGUI::Headline(U"駒落ち", s3d::Vec2(740, 50));
    {
        if (s3d::SimpleGUI::HorizontalRadioButtons(eleganceIndx, {U"自分", U"相手"}, s3d::Vec2(740, 90))) {
            getData().elegance = static_cast<Elegance>(eleganceIndx);
        }
    }
    // SimpleGUI::Headline(U"手合", Vec2(760, 100));
    {
        if (s3d::SimpleGUI::RadioButtons(handicapIndx, handicaps, s3d::Vec2(740, 135))) {
            getData().handicap = static_cast<Handicap>(handicapIndx);
        }
    }
    if (getData().gameState == State::PlayAlone) {
        s3d::SimpleGUI::Headline(U"強さ", s3d::Vec2(480, 240));
        {
            if (s3d::SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級", U"検討モード"}, s3d::Vec2(480, 280))) {
                getData().depthMax = static_cast<uint32>(depthMaxIndx + 1);
            }
        }
    }
    if (s3d::SimpleGUI::Button(U"ゲームスタート", s3d::Vec2(800, 530), 200)) {
        if (getData().handicap == Handicap::Custom) {
            ParseInit();
            if (m_isReadCsv) {
                changeScene(getData().gameState);
                return;
            }

            m_isCustom = true;
            // １マスの大きさ
            const double squareSize = 50.0;

            // 盤面に駒を設置
            for (size_t y = 0; y < 9; ++y) {
                for (size_t x = 0; x < 9; ++x) {
                    m_boardSquares << KomaSquare(m_shogiBan.tl().x + x * squareSize, m_shogiBan.tl().y + y * squareSize, squareSize, getData().GetBoard()[y][x],
                                                 s3d::Point(9 - x, y + 1));
                }
            }
            return;
        }

        if (getData().gameState == State::OnlineMatch) {
            ExitGames::Common::Dictionary<nByte, int32> dic;
            dic.put(1, static_cast<int32>(getData().firstMove));
            dic.put(2, static_cast<int32>(getData().elegance));
            dic.put(3, static_cast<int32>(getData().handicap));
            GetClient().opRaiseEvent(true, dic, 2);
        }

        changeScene(getData().gameState);
    }

    if (s3d::SimpleGUI::Button(U"タイトルに戻る", s3d::Vec2(280, 530), 200)) {
        if (getData().gameState == State::OnlineMatch) {
            Disconnect();
            return;
        }

        changeScene(State::Title);
    }

    if (s3d::SimpleGUI::Button(U"ファイルを選択", s3d::Vec2(540, 530), 200)) {
        if (s3d::Optional<s3d::String> filePath = s3d::Dialog::OpenFile(); filePath.has_value()) {
            if (filePath.value().includes(U".csv")) {
                m_csv = s3d::CSVData(filePath.value());
            }
        }
    }
}


namespace Custom {
    constexpr s3d::Vec2 selfFu = s3d::Vec2(875.0, 285.0);
    constexpr s3d::Vec2 enemyFu = s3d::Vec2(355.0, 35.0);

    constexpr s3d::Vec2 selfKyo = s3d::Vec2(875.0, 335.0);
    constexpr s3d::Vec2 enemyKyo = s3d::Vec2(355.0, 85.0);

    constexpr s3d::Vec2 selfKei = s3d::Vec2(975.0, 335.0);
    constexpr s3d::Vec2 enemyKei = s3d::Vec2(255.0, 85.0);

    constexpr s3d::Vec2 selfGin = s3d::Vec2(875.0, 385.0);
    constexpr s3d::Vec2 enemyGin = s3d::Vec2(355.0, 135.0);

    constexpr s3d::Vec2 selfKin = s3d::Vec2(975.0, 385.0);
    constexpr s3d::Vec2 enemyKin = s3d::Vec2(255.0, 135.0);

    constexpr s3d::Vec2 selfKaku = s3d::Vec2(875.0, 435.0);
    constexpr s3d::Vec2 enemyKaku = s3d::Vec2(355.0, 185.0);

    constexpr s3d::Vec2 selfHi = s3d::Vec2(975.0, 435.0);
    constexpr s3d::Vec2 enemyHi = s3d::Vec2(255.0, 185.0);

    /// <summary>
    /// 自分の持ち駒の場所
    /// </summary>
    constexpr array<const s3d::Vec2, 7> selfDaiPoses = {selfFu, selfKyo, selfKei, selfGin, selfKin, selfKaku, selfHi};
    /// <summary>
    /// 敵の持ち駒の場所
    /// </summary>
    constexpr array<const s3d::Vec2, 7> enemyDaiPoses = {enemyFu, enemyKyo, enemyKei, enemyGin, enemyKin, enemyKaku, enemyHi};
}  // namespace Custom

void Select::Custom() {
    // 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を消す処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                if (square.mouseOver() && square.GetKomaType() != Empty && square.GetKomaType() != Eou && square.GetKomaType() != Sou) {
                    s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
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
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            continue;
        }
        if (square.GetKomaType() == Eou || square.GetKomaType() == Sou) {
            return;
        }

        if (s3d::KeySpace.pressed()) {
            m_holdHand.value().PromoteKoma();
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

        square.ChangeKomaType(m_holdHand.value().GetKomaType());

        if (!s3d::KeyShift.pressed() || m_holdHand.value().GetKomaType() == Sou || m_holdHand.value().GetKomaType() == Eou) {
            m_holdHand.reset();
            return;
        }

        m_holdHand.value().ReturnPromoteKoma();
        return;
    }

    // 駒台に持ってる駒を置く処理
    if (m_holdHand.has_value()) {
        // 何もクリックしてなかったら、処理リターン
        if (!s3d::MouseL.down()) {
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            return;
        }
        if (m_holdHand.value().GetKomaType() == Eou || m_holdHand.value().GetKomaType() == Sou) {
            m_holdHand.value().setCenter(s3d::Cursor::PosF());
            return;
        }

        m_holdHand.value().ReturnPromoteKoma();

        if (m_komaDaiSelf.leftClicked()) {
            if ((m_holdHand.value().GetKomaType() & Self) == 0) {
                return;
            }
            m_havingSelfKoma[m_holdHand.value().GetKomaType() - Self - 1] << KomaSquare(Custom::selfDaiPoses[m_holdHand.value().GetKomaType() - Self - 1], m_komaDaiSelf.w / 4,
                                                                                        m_holdHand.value().GetKomaType(), KomaState::Dai, s3d::Point(0, 0));
        }
        else if (m_komaDaiEnemy.leftClicked()) {
            if ((m_holdHand.value().GetKomaType() & Enemy) == 0) {
                return;
            }
            m_havingEnemyKoma[m_holdHand.value().GetKomaType() - Enemy - 1] << KomaSquare(Custom::enemyDaiPoses[m_holdHand.value().GetKomaType() - Enemy - 1], m_komaDaiEnemy.w / 4,
                                                                                          m_holdHand.value().GetKomaType(), KomaState::Dai, s3d::Point(0, 0));
        }

        if (!s3d::KeyShift.pressed()) {
            m_holdHand.reset();
        }
        return;
    }

    // プレイヤーの駒選択台から駒を取る処理
    for (auto& selfKoma : m_selectSelfKomas) {
        if (!selfKoma.leftClicked()) {
            if (selfKoma.mouseOver()) {
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
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
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
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
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
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
                s3d::Cursor::RequestStyle(s3d::CursorStyle::Hand);
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
        if (s3d::SimpleGUI::Button(U"ゲームスタート", s3d::Vec2(900, 620), 200)) {
            array<array<uint32, 9>, 9> boardCustom;

            // 盤面に駒を設置
            for (size_t x{}; x < 9; ++x) {
                uint32 selfFuNum{}, enemyFuNum{};
                for (size_t y{}; y < 9; ++y) {
                    if (m_boardSquares[9 * y + x].GetKomaType() == KomaType::Sfu) {
                        ++selfFuNum;
                    }
                    if (m_boardSquares[9 * y + x].GetKomaType() == KomaType::Efu) {
                        ++enemyFuNum;
                    }
                    if (selfFuNum >= 2 || enemyFuNum >= 2) {
                        m_isNifu = true;
                        return;
                    }

                    boardCustom[y][x] = m_boardSquares[9 * y + x].GetKomaType();
                }
            }
            // 初期状態で先手が王手の場合、ゲームを開始しない
            Kyokumen k(0, boardCustom);
            k.MakeLegalMoves((getData().firstMove == Turn::Player) ? Enemy : Self);
            if (k.IsOute((getData().firstMove == Turn::Player) ? Turn::Enemy : Turn::Player)) {
                m_isFirstOute = true;
                return;
            }

            getData().SetCustomBoard(boardCustom);

            // 持ち駒を渡す
            for (const auto [i, havingSelfKoma] : s3d::IndexedRef(m_havingSelfKoma)) {
                for (const auto& motigoma : havingSelfKoma) {
                    ++getData().motigomas[(i + 1) + Self];
                }
            }
            for (const auto [i, havingEnemyKoma] : s3d::IndexedRef(m_havingEnemyKoma)) {
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
    s3d::SimpleGUI::Headline(U"手番", s3d::Vec2(480, 50));
    { s3d::SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, s3d::Vec2(480, 90)); }
    s3d::SimpleGUI::Headline(U"駒落ち", s3d::Vec2(740, 50));
    { s3d::SimpleGUI::HorizontalRadioButtons(eleganceIndx, {U"自分", U"相手"}, s3d::Vec2(740, 90)); }
    // SimpleGUI::Headline(U"手合", Vec2(760, 100));
    { s3d::SimpleGUI::RadioButtons(handicapIndx, handicaps, s3d::Vec2(740, 135)); }
    if (getData().gameState == State::PlayAlone) {
        s3d::SimpleGUI::Headline(U"強さ", s3d::Vec2(480, 240));
        { s3d::SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級", U"検討モード"}, s3d::Vec2(480, 280)); }
    }
    s3d::SimpleGUI::Button(U"ゲームスタート", s3d::Vec2(800, 530), 200);
    s3d::SimpleGUI::Button(U"タイトルに戻る", s3d::Vec2(280, 530), 200);
    s3d::SimpleGUI::Button(U"ファイルを選択", s3d::Vec2(540, 530), 200);
}

void Select::draw() const {
    if (!m_isCustom) {
        return;
    }

    m_shogiBan.draw(s3d::Palette::Burlywood);
    m_enemyDai.draw(s3d::Palette::Burlywood);
    m_selfDai.draw(s3d::Palette::Burlywood);
    m_komaDaiEnemy.draw(s3d::Palette::Burlywood);
    m_komaDaiSelf.draw(s3d::Palette::Burlywood);

    s3d::SimpleGUI::Headline(U"敵駒", m_enemyDai.tl().movedBy(0, -40));
    s3d::SimpleGUI::Headline(U"自駒", m_selfDai.tr().movedBy(-60, -40));

    for (const auto& square : m_boardSquares) {
        square.drawFrame(2, s3d::Palette::Black);
        if (square.mouseOver()) {
            square.draw(s3d::ColorF(s3d::Palette::Red, 0.5f));
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
        for (const auto [i, koma] : s3d::IndexedRefReversed(selfDai)) {
            koma.Draw(s3d::Vec2(i * 10, 0));
        }
    }
    for (const auto& enemyDai : m_havingEnemyKoma) {
        for (const auto [i, koma] : s3d::IndexedRefReversed(enemyDai)) {
            koma.Draw(s3d::Vec2(static_cast<int32>(i) * -10, 0));
        }
    }

    if (m_holdHand.has_value()) {
        m_holdHand.value().Draw();
    }

    s3d::FontAsset(U"Explain")(U"初期盤面を自由に制作できます。").draw(50, 280, s3d::Palette::Black);
    s3d::FontAsset(U"Explain")(U"Shiftキーを押したまま選択した駒を\n置くと連続で置く事ができ、\nSpaceキーを押したまま選択した駒を\n置くと駒を成る事ができます。")
        .draw(50, 320, s3d::Palette::Black);

    if (m_isNifu) {
        s3d::FontAsset(U"Warning")(U"二歩です！置き直してください！").draw(320, 620, s3d::Palette::Red);
    }
    if (m_isFirstOute) {
        s3d::FontAsset(U"Warning")(U"先手が王手の状態で\n開始できません。").draw(915, 120, s3d::Palette::Red);
    }
}
