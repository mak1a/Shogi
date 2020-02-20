
# pragma once
#include <Siv3D.hpp>
using std::array;

// �V�[���̖��O
enum class State {
	Title,  // �^�C�g���V�[��
	Game,   // ��l�p�v���C�̃V�[��
	GameAI  // ��l�p�v���C�̃V�[��
};

/// <summary>
/// �^�[��
/// </summary>
enum class Turn {
	Player,
	Enemy,
	Tsumi
};

/// <summary>
/// �Q�[���f�[�^
/// </summary>
struct GameData {
	/// <summary>
	/// ���
	/// </summary>
	Turn firstMove = Turn::Player;
};

/// <summary>
/// �V�[���Ǘ��N���X
/// </summary>
using MyApp = SceneManager<State, GameData>;

/// <summary>
/// ����
/// </summary>
enum class Winner {
	Player,
	Enemy
};
