
# pragma once
# include "Kyokumen.hpp"

// �^�C�g���V�[��
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
		U"����", U"�p����", U"��ԗ���",
		U"2������", U"4������", U"6������",
		U"8������", U"10������", U"�J�X�^��"
	};

	bool m_isCustom;

	bool m_isNifu;
	bool m_isFirstOute;

	// �J�X�^����I�񂾎��Ɏg�������o�ϐ�

	// ������
	RectF m_shogiBan;

	RectF m_enemyDai, m_selfDai;

	// �Տ�̃}�X��
	// RectF�ŕێ����鎖��Mouse����ł���悤�ɂ���B
	Array<KomaSquare> m_boardSquares;

	// �I�ԋ�i�v���C���[���j
	Array<KomaSquare> m_selectSelfKomas;
	// �I�ԋ�i�G���j
	Array<KomaSquare> m_selectEnemyKomas;

	// �}�E�X�ŋ��ێ�
	Optional<KomaSquare> m_holdHand;

	void SetUp();

	void Custom();

public:
	Select(const InitData& init);

	void update() override;

	/// <summary>
	/// �t�F�[�h�C���̎��ɕ`�悳��Ȃ��̂Ŏd���Ȃ�
	/// </summary>
	void updateFadeIn(double) override;

	void draw() const override;
};
