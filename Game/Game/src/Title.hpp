
# pragma once
# include "Common.hpp"

// タイトルシーン
class Title : public MyApp::Scene {
private:
	Rect m_soloMatchButton;
	Transition m_soloMatchTransition;

	Rect m_twoPlayerMatchButton;
	Transition m_twoPlayerMatchTransition;

	Rect m_exitButton;
	Transition m_exitTransition;
public:
	Title(const InitData& init);

	void update() override;

	void draw() const override;
};


class Select : public MyApp::Scene {
private:
	size_t firstMoveIndx = 0;
	size_t handicapIndx = 0;
	size_t eleganceIndx = 0;
	size_t depthMaxIndx = 0;

	const Array<String> handicaps = {
		U"平手", U"角落ち", U"飛車落ち",
		U"2枚落ち", U"4枚落ち", U"6枚落ち",
		U"8枚落ち", U"10枚落ち"
	};
public:
	Select(const InitData& init)
	: IScene(init) {
		getData().firstMove = Turn::Player;
		getData().elegance = Elegance::Player;
		getData().handicap = Handicap::Hirate;
		getData().depthMax = 1;
	}

	void update() override {
		if (SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, Vec2(520, 40))) {
			getData().firstMove = static_cast<Turn>(firstMoveIndx);
		}

		if (SimpleGUI::RadioButtons(eleganceIndx, {U"自分", U"相手"}, Vec2(520, 140))) {
			getData().elegance = static_cast<Elegance>(eleganceIndx);
		}

		if (SimpleGUI::RadioButtons(handicapIndx, handicaps, Vec2(760, 40))) {
			getData().handicap = static_cast<Handicap>(handicapIndx);
		}

		if (getData().gameState == State::GameAI) {
			if (SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級"}, Vec2(520, 240))) {
				getData().depthMax = static_cast<uint32>(depthMaxIndx+1);
			}
		}

		if (SimpleGUI::Button(U"ゲームスタート", Vec2(800, 500), 200)) {
            changeScene(getData().gameState);
        }
	}

	/// <summary>
	/// フェードインの時に描画されないので仕方なく
	/// </summary>
	void updateFadeIn(double) override {
		SimpleGUI::RadioButtons(firstMoveIndx, {U"先手", U"後手"}, Vec2(520, 40));
		SimpleGUI::RadioButtons(eleganceIndx, {U"自分", U"相手"}, Vec2(520, 140));
		SimpleGUI::RadioButtons(handicapIndx, handicaps, Vec2(760, 40));
		SimpleGUI::RadioButtons(depthMaxIndx, {U"はじめて", U"初級", U"中級", U"上級"}, Vec2(520, 240));
		SimpleGUI::Button(U"ゲームスタート", Vec2(800, 500), 200);
	}
};
