
# pragma once
# include "Kyokumen.hpp"

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
		U"8枚落ち", U"10枚落ち", U"カスタム"
	};

	bool m_isCustom;

	bool m_isNifu;
	bool m_isFirstOute;

	// カスタムを選んだ時に使うメンバ変数

	// 将棋盤
    RectF m_shogiBan;

	RectF m_enemyDai, m_selfDai;

	// 盤上のマス目
    // RectFで保持する事でMouse判定できるようにする。
    Array<KomaSquare> m_boardSquares;

	// 選ぶ駒（プレイヤー側）
    Array<KomaSquare> m_selectSelfKomas;
    // 選ぶ駒（敵側）
    Array<KomaSquare> m_selectEnemyKomas;

	// マウスで駒を保持
    Optional<KomaSquare> m_holdHand;

	void SetUp();

	void Custom();

public:
	Select(const InitData& init);

	void update() override;

	/// <summary>
	/// フェードインの時に描画されないので仕方なく
	/// </summary>
	void updateFadeIn(double) override;

	void draw() const override;
};
