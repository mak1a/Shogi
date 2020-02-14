
# pragma once
#include <Siv3D.hpp>
using std::array;

// シーンの名前
enum class State {
	Title,  // タイトルシーン
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
/// ゲームデータ
/// </summary>
struct GameData{
	/// <summary>
	/// 先手
	/// </summary>
	Turn firstMove = Turn::Player;
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
