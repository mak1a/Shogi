
# pragma once
#include"Kyokumen.hpp"

class Ban {
private:
    // ������
    RectF m_shogiBan;
    // �G�̋��
    RectF m_komaDaiEnemy;
    // �����̋��
    RectF m_komaDaiSelf;

    Kyokumen m_kyokumen;

    // �ǂ���̏��Ԃ�
    Turn m_turn;

    /// <summary>
    /// �ǂ��炪��������
    /// </summary>
    Winner m_winner = Winner::Player;

    // ��Ԃ���シ��
    void ChangeCurrentTurn() noexcept {
        ClearPrint();
        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
        if (GetTurn() == Turn::Player) {
            if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
                //Print << U"You Lose!";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Enemy;
            }
        }
        else {
            if (m_kyokumen.MakeLegalMoves(Enemy) <= 0) {
                //Print << U"�v���C���[�̏����I";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Player;
            }
        }
    }

    // �Տ�̃}�X��
    // RectF�ŕێ����鎖��Mouse����ł���悤�ɂ���B
    Array<KomaSquare> m_boardSquares;

    // ������i�v���C���[���j
    Array<Array<KomaSquare>> m_havingSelfKoma;
    // ������i�G���j
    Array<Array<KomaSquare>> m_havingEnemyKoma;

    // �}�E�X�ŋ��ێ�
    Optional<KomaSquare> m_holdHand;

    // ���u���������̐F��������ƐԂ����邽�߂̕ϐ�
    Optional<KomaSquare> m_placedPart;

    void AddHoldKoma(KomaSquare& koma_);
public:
    Ban(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const Turn& turn_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;

    void Update();

    void Draw() const;

    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }

    [[nodiscard]] Winner GetWinner() const noexcept {
        return m_winner;
    }
};

// �Q�[���V�[��
class Game : public MyApp::Scene {
private:
    Ban m_ban;

    void result();
public:

    Game(const InitData& init);

    void update() override;

    void draw() const override;
};
