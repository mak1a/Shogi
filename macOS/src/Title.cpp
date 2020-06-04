
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
, m_enemyDai(Arg::center(370.f, 550.f), 420.f, 80.f)
, m_selfDai(Arg::center(910.f, 550.f), 420.f, 80.f) {
	getData().firstMove = Turn::Player;
	getData().elegance = Elegance::Player;
	getData().handicap = Handicap::Hirate;
	getData().depthMax = 1;
	getData().SetCustomBoard(Board::Custom);

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

void Select::Custom() {
	// 盤面上の処理
    for (auto& square : m_boardSquares) {
        // 盤面から駒を消す処理
        if (!m_holdHand.has_value()) {
            if (!square.leftClicked()) {
                continue;
            }
            if (square.GetKomaType() == Empty
				|| square.GetKomaType() == Eou
				|| square.GetKomaType() == Sou) {
                return;
            }
			Cursor::RequestStyle(CursorStyle::Hand);
            
            square.ChangeKomaType(Emp);
			m_isNifu = false;
			m_isFirstOute = false;
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
        square.ChangeKomaType(m_holdHand.value().GetKomaType());

		if (!KeyShift.pressed()) {
			m_holdHand.reset();
		}
        return;
    }
    
    // 駒台から取った駒を元の駒台の位置に戻す処理
    // いわゆるキャンセル処理
    if (m_holdHand.has_value()) {
        // 何もクリックしてなかったら、処理リターン
        if (!MouseL.down()) {
            m_holdHand.value().setCenter(Cursor::PosF());
            return;
        }
        
        m_holdHand.reset();
        return;
    }
    
    // プレイヤーの駒台から駒を取る処理
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
    
    // 敵の駒台から駒を取る処理
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

	if (m_holdHand.has_value()) {
        m_holdHand.value().Draw();
    }

	FontAsset(U"Explain")(U"初期盤面を自由に\n制作できます。").draw(50, 80, Palette::Black);
	FontAsset(U"Explain")(U"Shiftキーを押した\nまま選択した駒を\n置くと連続で\n置けます。").draw(50, 230, Palette::Black);

	if (m_isNifu) {
		FontAsset(U"Explain")(U"二歩です！置き直してください！").draw(320, 620, Palette::Red);
	}
	if (m_isFirstOute) {
		FontAsset(U"Explain")(U"先手が王手の状態で\n開始できません。").draw(915, 120, Palette::Red);
	}
}
