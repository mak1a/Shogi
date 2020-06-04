
# pragma once
#include <Siv3D.hpp>
#include "Board.hpp"
using std::array;

// �V�[���̖��O
enum class State {
	Title,  // �^�C�g���V�[��
	Select,	// �ݒ�V�[��
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
/// ���
/// </summary>
enum class Handicap {
	Hirate,	// ����
	Bishop,	// �p����
	Rook,	// ��ԗ���
	TwoPieces,	// 2������
	FourDrops,	// 4������
	SixDrops,	// 6������
	EightDrops,	// 8������
	TenDrops,	// 10������
	Custom	// �J�X�^��
};

/// <summary>
/// ��������
/// </summary>
enum class Elegance {
	Player,
	Enemy
};

/// <summary>
/// �Q�[���f�[�^
/// </summary>
struct GameData {
	/// <summary>
	/// ���
	/// </summary>
	Turn firstMove = Turn::Player;

	/// <summary>
	/// ��l�p����l�p�v���C�̔���
	/// </summary>
	State gameState = State::GameAI;

	/// <summary>
	/// ���
	/// </summary>
	Handicap handicap = Handicap::Hirate;

	/// <summary>
	/// ��������
	/// </summary>
	Elegance elegance = Elegance::Player;

	/// <summary>
	/// �v�l�̐[��
	/// </summary>
	uint32 depthMax = 0;

private:
	/// <summary>
	/// �Ֆʂ��J�X�^���̏ꍇ�A�����Ɋi�[����
	/// </summary>
	array<array<uint32, 9>, 9> m_customKomas = Board::Custom;

public:
	GameData() noexcept {
		for (size_t y{}; y < 9; ++y) {
			for (size_t x{}; x < 9; ++x) {
				m_customKomas[y][x] = Board::Custom[y][x];
			}
		}
	}

	/// <summary>
	/// �I�������Ղ��o�͂���
	/// </summary>
	array<array<uint32, 9>, 9> GetBoard() const noexcept {
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
			case Handicap::Custom:
				return m_customKomas;
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
			case Handicap::Custom:
				return m_customKomas;
			}
		}
	}

	void SetCustomBoard(const array<array<uint32, 9>, 9>& custom_) {
		for (size_t y{}; y < 9; ++y) {
			for (size_t x{}; x < 9; ++x) {
				m_customKomas[y][x] = custom_[y][x];
			}
		}
	}
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
	Enemy,
	Sennitite
};
