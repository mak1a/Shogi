
# pragma once
#include <Siv3D.hpp>
using std::array;

// シーンの名前
enum class State
{
	Title,

	Game,

	GameAI
};

// ゲームデータ
struct GameData
{
	// ハイスコア
	int32 highScore = 0;
};

// シーン管理クラス
using MyApp = SceneManager<State, GameData>;

enum class Turn {
    Player,
    Enemy,
	Tsumi
};

enum class Winner {
	Player,
	Enemy
};
