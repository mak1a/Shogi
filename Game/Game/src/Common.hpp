
# pragma once
#include <Siv3D.hpp>
#include "Board.hpp"
using std::array;

// シーンの名前
enum class State {
	Title,  // タイトルシーン
	Select,	// 設定シーン
	Game,   // 二人用プレイのシーン
	GameAI  // 一人用プレイのシーン
};

/// <summary>
/// ターン
/// </summary>
enum class Turn {
    Player,
    Enemy,
	Tsumi
};

/// <summary>
/// 駒落ち
/// </summary>
enum class Handicap {
	Hirate,	// 平手
	Bishop,	// 角落ち
	Rook,	// 飛車落ち
	TwoPieces,	// 2枚落ち
	FourDrops,	// 4枚落ち
	SixDrops,	// 6枚落ち
	EightDrops,	// 8枚落ち
	TenDrops,	// 10枚落ち
};

/// <summary>
/// 駒落ちする方
/// </summary>
enum class Elegance {
	Player,
	Enemy
};

/// <summary>
/// ゲームデータ
/// </summary>
struct GameData {
	/// <summary>
	/// 先手
	/// </summary>
	Turn firstMove = Turn::Player;

	/// <summary>
	/// 一人用か二人用プレイの判定
	/// </summary>
	State gameState = State::GameAI;

	/// <summary>
	/// 駒落ち
	/// </summary>
	Handicap handicap = Handicap::Hirate;

	/// <summary>
	/// 駒落ちする方
	/// </summary>
	Elegance elegance = Elegance::Player;

	/// <summary>
	/// 選択した盤を出力する
	/// </summary>
	array<const array<const uint32, 9>, 9> GetBoard() const noexcept {
		if (elegance == Elegance::Player) {
			switch (handicap) {
			case Handicap::Hirate:
				return Board::Myself::Hirate;
			case Handicap::Bishop:
				return Board::Myself::Bishop;
			case Handicap::Rook:
				return Board::Myself::Rook;
			case Handicap::TwoPieces:
				return Board::Myself::TwoPieces;
			case Handicap::FourDrops:
				return Board::Myself::FourDrops;
			case Handicap::SixDrops:
				return Board::Myself::SixDrops;
			case Handicap::EightDrops:
				return Board::Myself::EightDrops;
			case Handicap::TenDrops:
				return Board::Myself::TenDrops;
			}
		}
		else {
			switch (handicap) {
			case Handicap::Hirate:
				return Board::Opponent::Hirate;
			case Handicap::Bishop:
				return Board::Opponent::Bishop;
			case Handicap::Rook:
				return Board::Opponent::Rook;
			case Handicap::TwoPieces:
				return Board::Opponent::TwoPieces;
			case Handicap::FourDrops:
				return Board::Opponent::FourDrops;
			case Handicap::SixDrops:
				return Board::Opponent::SixDrops;
			case Handicap::EightDrops:
				return Board::Opponent::EightDrops;
			case Handicap::TenDrops:
				return Board::Opponent::TenDrops;
			}
		}
	}
};

/// <summary>
/// シーン管理クラス
/// </summary>
using MyApp = SceneManager<State, GameData>;

/// <summary>
/// 勝者
/// </summary>
enum class Winner {
	Player,
	Enemy
};
