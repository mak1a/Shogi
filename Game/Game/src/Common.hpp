
# pragma once
#include <Siv3D.hpp>
using std::array;

// シーンの名前
enum class State
{
	Title,

	Game
};

// ゲームデータ
struct GameData
{
	// ハイスコア
	int32 highScore = 0;
};

// シーン管理クラス
using MyApp = SceneManager<State, GameData>;
