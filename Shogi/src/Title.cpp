
#include "Title.hpp"

Title::Title(const InitData& init)
: IScene(init)
, m_soloMatchButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, -30)), 300, 60)
, m_twoPlayerMatchButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 70)), 300, 60)
, m_exitButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 270)), 300, 60)
, m_replayButton(s3d::Arg::center(s3d::Scene::Center().movedBy(0, 170)), 300, 60)
, m_soloMatchTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_twoPlayerMatchTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_exitTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_replayTransition(s3d::SecondsF(0.4), s3d::SecondsF(0.2))
, m_csv(U"") {
    getData().firstMove = Turn::Player;
    getData().elegance = Elegance::Player;
    getData().handicap = Handicap::Hirate;
    getData().depthMax = 1;
    getData().SetCustomBoard(Board::Custom);
    getData().motigomas.fill(0);
    getData().photonState = PhotonState::None;
    getData().stackBoradSquares.clear();
    getData().stackHavingSelf.clear();
    getData().stackHavingEnemy.clear();
    getData().stackPlacedPart.clear();

    s3d::ClearPrint();
}

void Title::update() {
    m_soloMatchTransition.update(m_soloMatchButton.mouseOver());
    m_twoPlayerMatchTransition.update(m_twoPlayerMatchButton.mouseOver());
    m_exitTransition.update(m_exitButton.mouseOver());
    m_replayTransition.update(m_replayButton.mouseOver());

    if (m_soloMatchButton.mouseOver() || m_exitButton.mouseOver() || m_twoPlayerMatchButton.mouseOver() || m_replayButton.mouseOver()) {
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

    if (m_replayButton.leftClicked()) {
        if (m_loadFileName = s3d::Dialog::OpenFile(); m_loadFileName.has_value()) {
            if (m_loadFileName.value().includes(U".csv")) {
                m_csv = s3d::CSVData(m_loadFileName.value());
            }
        }
        getData().gameState = State::Title;
        ReplayInit();
    }
}

void Title::ReplayInit() {
    if (!m_csv) {
        return;
    }

    // s3d::Print << m_csv.rows() << U", " << m_csv.columns(0);

    if (m_csv.rows() <= 10) {
        s3d::Print << U"行数不足 : " << m_csv.rows() + 1 << U"行しかありません。";
        return;
    }

    // テキストデータのString型をuint32型にするための変換器
    s3d::HashTable<s3d::String, uint32> parseData;

    parseData.emplace(U"Sfu", Sfu);
    parseData.emplace(U"Sto", Sto);
    parseData.emplace(U"Sky", Sky);
    parseData.emplace(U"Sny", Sny);
    parseData.emplace(U"Ske", Ske);
    parseData.emplace(U"Snk", Snk);
    parseData.emplace(U"Sgi", Sgi);
    parseData.emplace(U"Sng", Sng);
    parseData.emplace(U"Ski", Ski);
    parseData.emplace(U"Ska", Ska);
    parseData.emplace(U"Sum", Sum);
    parseData.emplace(U"Shi", Shi);
    parseData.emplace(U"Sry", Sry);
    parseData.emplace(U"Sou", Sou);

    parseData.emplace(U"Efu", Efu);
    parseData.emplace(U"Eto", Eto);
    parseData.emplace(U"Eky", Eky);
    parseData.emplace(U"Eny", Eny);
    parseData.emplace(U"Eke", Eke);
    parseData.emplace(U"Enk", Enk);
    parseData.emplace(U"Egi", Egi);
    parseData.emplace(U"Eng", Eng);
    parseData.emplace(U"Eki", Eki);
    parseData.emplace(U"Eka", Eka);
    parseData.emplace(U"Eum", Eum);
    parseData.emplace(U"Ehi", Ehi);
    parseData.emplace(U"Ery", Ery);
    parseData.emplace(U"Eou", Eou);

    constexpr double shogiBan = 540.f;
    constexpr double komaDai = 240.f;

    const s3d::RectF shogiBanRect(s3d::Arg::center(s3d::Scene::CenterF()), shogiBan);
    const s3d::RectF komaDaiSelf(s3d::Arg::center(s3d::Scene::CenterF().movedBy(shogiBan / 2 + 10 + komaDai / 2, (shogiBan / 2 - komaDai) + komaDai / 2)), komaDai);
    const s3d::RectF komaDaiEnemy(s3d::Arg::center(s3d::Scene::CenterF().movedBy(-(shogiBan / 2 + 10 + komaDai / 2), -((shogiBan / 2 - komaDai) + komaDai / 2))), komaDai);

    for (uint32 csvRow{}; csvRow < m_csv.rows(); csvRow += 11) {
        // 盤上のマス目
        // RectFで保持する事でMouse判定できるようにする。
        s3d::Array<KomaSquare> boardSquares;
        // 持ち駒（プレイヤー側）
        s3d::Array<s3d::Array<KomaSquare>> havingSelfKoma;
        // 持ち駒（敵側）
        s3d::Array<s3d::Array<KomaSquare>> havingEnemyKoma;
        // 駒を置いた部分の色をちょっと赤くするための変数
        s3d::Optional<KomaSquare> placedPart;

        array<array<uint32, 9>, 9> boardCustom;
        bool isExistSelfKing{false};
        bool isExistEnemyKing{false};

        for (uint32 y{}; y < 9; ++y) {
            if (m_csv.columns(y + csvRow) != 9) {
                s3d::Print << U"列数不足 : " << y + 1 << U"行目";
                return;
            }
            for (uint32 x{}; x < 9; ++x) {
                const auto koma = parseData.find(m_csv[y + csvRow][x]);
                if (koma == parseData.end()) {
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
            s3d::Print << U"玉が見つかりませんでした。";
            return;
        }

        array<uint32, 40> motigomas;
        motigomas.fill(0);
        if (m_csv[9 + csvRow].size() != 0) {
            s3d::Array<s3d::String> motigomaData = m_csv[9 + csvRow][0].split(U',');
            for (const auto& komaName : motigomaData) {
                const auto koma = parseData.find(komaName);
                if (koma == parseData.end()) {
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

            // 盤面に駒を設置
            for (size_t x{}; x < 9; ++x) {
                uint32 selfFuNum{}, enemyFuNum{};
                for (size_t y{}; y < 9; ++y) {
                    if (boardCustom[y][x] == KomaType::Sfu) {
                        ++selfFuNum;
                    }
                    if (boardCustom[y][x] == KomaType::Efu) {
                        ++enemyFuNum;
                    }
                    if (selfFuNum >= 2 || enemyFuNum >= 2) {
                        s3d::Print << U"二歩です。";
                        return;
                    }
                }
            }
        }

        // １マスの大きさ
        constexpr double squareSize = shogiBan / 9;

        // 盤面に駒を設置
        for (size_t y = 0; y < 9; ++y) {
            for (size_t x = 0; x < 9; ++x) {
                boardSquares << KomaSquare(shogiBanRect.tl().x + x * squareSize, shogiBanRect.tl().y + y * squareSize, squareSize, boardCustom[y][x], s3d::Point(9 - x, y + 1));
            }
        }

        havingSelfKoma.resize(7);
        havingEnemyKoma.resize(7);

        /// <summary>
        /// 持ち駒の初期化
        /// </summary>
        for (uint32 type{Sfu}; type <= Shi; ++type) {
            for (uint32 i{}; i < motigomas[type]; ++i) {
                havingSelfKoma[type - Self - 1] << KomaSquare(KomaPos::selfDaiPoses[type - Self - 1], komaDaiSelf.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
            }
        }
        for (uint32 type{Efu}; type <= Ehi; ++type) {
            for (uint32 i{}; i < motigomas[type]; ++i) {
                havingEnemyKoma[type - Enemy - 1] << KomaSquare(KomaPos::enemyDaiPoses[type - Enemy - 1], komaDaiEnemy.w / 4, type, KomaState::Dai, s3d::Point(0, 0));
            }
        }

        getData().stackBoradSquares.emplace_back(boardSquares);
        getData().stackHavingSelf.emplace_back(havingSelfKoma);
        getData().stackHavingEnemy.emplace_back(havingEnemyKoma);

        if (m_csv[10 + csvRow].size() != 0) {
            const uint32 x = s3d::Parse<uint32>(m_csv[10 + csvRow][0]);
            const uint32 y = s3d::Parse<uint32>(m_csv[10 + csvRow][1]);
            placedPart.emplace(boardSquares[y * 9 + x]);
        }
        getData().stackPlacedPart.emplace_back(placedPart);
    }

    changeScene(State::Replay);
}

void Title::draw() const {
    const s3d::String titleText = U"将棋";
    const s3d::Vec2 center(s3d::Scene::Center().x, 120);
    s3d::FontAsset(U"Title")(titleText).drawAt(center.movedBy(4, 6), s3d::ColorF(0.0, 0.5));
    s3d::FontAsset(U"Title")(titleText).drawAt(center);

    m_soloMatchButton.draw(s3d::ColorF(1.0, m_soloMatchTransition.value())).drawFrame(2);
    m_twoPlayerMatchButton.draw(s3d::ColorF(1.0, m_twoPlayerMatchTransition.value())).drawFrame(2);
    m_exitButton.draw(s3d::ColorF(1.0, m_exitTransition.value())).drawFrame(2);
    m_replayButton.draw(s3d::ColorF(1.0, m_replayTransition.value())).drawFrame(2);

    s3d::FontAsset(U"Menu")(U"ひとりで").drawAt(m_soloMatchButton.center(), s3d::ColorF(0.25));
    s3d::FontAsset(U"Menu")(U"ふたりで").drawAt(m_twoPlayerMatchButton.center(), s3d::ColorF(0.25));
    s3d::FontAsset(U"Menu")(U"おわる").drawAt(m_exitButton.center(), s3d::ColorF(0.25));
    s3d::FontAsset(U"Menu")(U"感想戦").drawAt(m_replayButton.center(), s3d::ColorF(0.25));

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
, m_csv(U"")
, m_isReadCsv(false)
, m_isSaveFile(false) {
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

    // テキストデータのString型をuint32型にするための変換器
    s3d::HashTable<s3d::String, uint32> parseData;

    parseData.emplace(U"Sfu", Sfu);
    parseData.emplace(U"Sto", Sto);
    parseData.emplace(U"Sky", Sky);
    parseData.emplace(U"Sny", Sny);
    parseData.emplace(U"Ske", Ske);
    parseData.emplace(U"Snk", Snk);
    parseData.emplace(U"Sgi", Sgi);
    parseData.emplace(U"Sng", Sng);
    parseData.emplace(U"Ski", Ski);
    parseData.emplace(U"Ska", Ska);
    parseData.emplace(U"Sum", Sum);
    parseData.emplace(U"Shi", Shi);
    parseData.emplace(U"Sry", Sry);
    parseData.emplace(U"Sou", Sou);

    parseData.emplace(U"Efu", Efu);
    parseData.emplace(U"Eto", Eto);
    parseData.emplace(U"Eky", Eky);
    parseData.emplace(U"Eny", Eny);
    parseData.emplace(U"Eke", Eke);
    parseData.emplace(U"Enk", Enk);
    parseData.emplace(U"Egi", Egi);
    parseData.emplace(U"Eng", Eng);
    parseData.emplace(U"Eki", Eki);
    parseData.emplace(U"Eka", Eka);
    parseData.emplace(U"Eum", Eum);
    parseData.emplace(U"Ehi", Ehi);
    parseData.emplace(U"Ery", Ery);
    parseData.emplace(U"Eou", Eou);

    array<array<uint32, 9>, 9> boardCustom;
    bool isExistSelfKing{false};
    bool isExistEnemyKing{false};

    for (uint32 y{}; y < 9; ++y) {
        if (m_csv.columns(y) != 9) {
            s3d::Print << U"列数不足 : " << y + 1 << U"行目";
            return;
        }
        for (uint32 x{}; x < 9; ++x) {
            const auto koma = parseData.find(m_csv[y][x]);
            if (koma == parseData.end()) {
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

    if (m_csv[9].size() == 0) {
        getData().SetCustomBoard(boardCustom);
        m_isReadCsv = true;
        return;
    }

    s3d::Array<s3d::String> motigomaData = m_csv[9][0].split(U',');
    array<uint32, 40> motigomas;
    motigomas.fill(0);
    for (const auto& komaName : motigomaData) {
        const auto koma = parseData.find(komaName);
        if (koma == parseData.end()) {
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

    // 盤面に駒を設置
    for (size_t x{}; x < 9; ++x) {
        uint32 selfFuNum{}, enemyFuNum{};
        for (size_t y{}; y < 9; ++y) {
            if (boardCustom[y][x] == KomaType::Sfu) {
                ++selfFuNum;
            }
            if (boardCustom[y][x] == KomaType::Efu) {
                ++enemyFuNum;
            }
            if (selfFuNum >= 2 || enemyFuNum >= 2) {
                m_isNifu = true;
                return;
            }
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

    if (getData().handicap == Handicap::Custom) {
        if (s3d::SimpleGUI::Button(U"ファイルを選択", s3d::Vec2(540, 530), 200)) {
            if (m_loadFileName = s3d::Dialog::OpenFile(); m_loadFileName.has_value()) {
                if (m_loadFileName.value().includes(U".csv")) {
                    m_csv = s3d::CSVData(m_loadFileName.value());
                }
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
            m_isSaveFile = false;

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

        m_isSaveFile = false;
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

        m_isSaveFile = false;
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

        m_isSaveFile = false;
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

        m_isSaveFile = false;
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

            m_isSaveFile = false;
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

            m_isSaveFile = false;
            havingEnemyKoma.remove_at(i);
            return;
        }
    }
}

void Select::update() {
    if (m_isCustom) {
        if (s3d::SimpleGUI::Button(U"ゲームスタート", s3d::Vec2(900, 620), 200) && !m_holdHand.has_value()) {
            m_isSaveFile = false;
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
                m_isSaveFile = false;
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

        if (s3d::SimpleGUI::Button(U"保存する", s3d::Vec2(100, 620), 200)) {
            // テキストデータのString型をuint32型にするための変換器
            s3d::HashTable<uint32, s3d::String> toStrData;

            toStrData.emplace(Sfu, U"Sfu");
            toStrData.emplace(Sto, U"Sto");
            toStrData.emplace(Sky, U"Sky");
            toStrData.emplace(Sny, U"Sny");
            toStrData.emplace(Ske, U"Ske");
            toStrData.emplace(Snk, U"Snk");
            toStrData.emplace(Sgi, U"Sgi");
            toStrData.emplace(Sng, U"Sng");
            toStrData.emplace(Ski, U"Ski");
            toStrData.emplace(Ska, U"Ska");
            toStrData.emplace(Sum, U"Sum");
            toStrData.emplace(Shi, U"Shi");
            toStrData.emplace(Sry, U"Sry");
            toStrData.emplace(Sou, U"Sou");

            toStrData.emplace(Efu, U"Efu");
            toStrData.emplace(Eto, U"Eto");
            toStrData.emplace(Eky, U"Eky");
            toStrData.emplace(Eny, U"Eny");
            toStrData.emplace(Eke, U"Eke");
            toStrData.emplace(Enk, U"Enk");
            toStrData.emplace(Egi, U"Egi");
            toStrData.emplace(Eng, U"Eng");
            toStrData.emplace(Eki, U"Eki");
            toStrData.emplace(Eka, U"Eka");
            toStrData.emplace(Eum, U"Eum");
            toStrData.emplace(Ehi, U"Ehi");
            toStrData.emplace(Ery, U"Ery");
            toStrData.emplace(Eou, U"Eou");

            m_csv = s3d::CSVData(m_saveFileName);
            if (!m_csv) {
                m_saveFileName = U"data/init/" + s3d::Date::Today().format(U"yyyy_M_d_") + s3d::ToString(1) + U".csv";
                for (uint32 num{2}; s3d::FileSystem::Exists(m_saveFileName); ++num) {
                    m_saveFileName = U"data/init/" + s3d::Date::Today().format(U"yyyy_M_d_") + s3d::ToString(num) + U".csv";
                }

                s3d::Serializer<s3d::BinaryWriter>{m_saveFileName}(0);
                m_csv = s3d::CSVData(m_saveFileName);
                if (!m_csv) {
                    s3d::Print << U"読み込めませんでした。";
                    return;
                }
            }

            m_csv.clear();

            for (uint32 y{}; y < 9; ++y) {
                for (uint32 x{}; x < 9; ++x) {
                    const auto komaStr = toStrData.find(m_boardSquares[9 * y + x].GetKomaType());
                    if (komaStr == toStrData.end()) {
                        m_csv.write(U"Emp");
                        continue;
                    }

                    m_csv.write(komaStr.value());
                }

                m_csv.newLine();
            }

            s3d::Array<s3d::String> motigomas;

            for (const auto [i, havingSelfKoma] : s3d::IndexedRef(m_havingSelfKoma)) {
                for (const auto& motigoma : havingSelfKoma) {
                    const auto komaStr = toStrData.find((i + 1) + Self);
                    if (komaStr == toStrData.end()) {
                        continue;
                    }
                    motigomas.emplace_back(komaStr.value());
                }
            }
            for (const auto [i, havingEnemyKoma] : s3d::IndexedRef(m_havingEnemyKoma)) {
                for (const auto& motigoma : havingEnemyKoma) {
                    const auto komaStr = toStrData.find((i + 1) + Enemy);
                    if (komaStr == toStrData.end()) {
                        continue;
                    }
                    motigomas.emplace_back(komaStr.value());
                }
            }

            s3d::String writeStr;
            for (const auto [i, str] : s3d::IndexedRef(motigomas)) {
                writeStr += str;
                if (i < motigomas.size() - 1) {
                    writeStr += U",";
                }
            }
            m_csv.write(writeStr);
            m_csv.save(m_saveFileName);
            m_isSaveFile = true;
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
    // s3d::SimpleGUI::Button(U"ファイルを選択", s3d::Vec2(540, 530), 200);
}

void Select::draw() const {
    if (!m_isCustom) {
        if (!m_loadFileName.has_value() || getData().handicap != Handicap::Custom) {
            return;
        }

        s3d::FontAsset(U"Explain")(U"ファイルパス : " + m_loadFileName.value()).drawAt(s3d::Scene::CenterF().movedBy(0.0, 275.0), s3d::Palette::Black);
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
    if (m_isSaveFile) {
        s3d::FontAsset(U"Explain")(U"保存しました。").draw(350.0, 625.0, s3d::Palette::Black);
    }
}
