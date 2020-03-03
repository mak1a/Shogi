
# pragma once
#include"Kyokumen.hpp"
#include"Sikou.hpp"

class BanSelf {
private:
    // ������
    RectF m_shogiBan;
    // �G�̋��
    RectF m_komaDaiEnemy;
    // �����̋��
    RectF m_komaDaiSelf;

    /// <summary>
    /// �X�g�b�v�E�H�b�`
    /// AI���l���Ă镗���o�����߂̂���
    /// </summary>
    Stopwatch m_thinkingTimer;

    Kyokumen m_kyokumen;

    Sikou m_sikouEnemy;
    Sikou m_sikouSelf;

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
                Print << U"You Lose!";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Enemy;
            }
        }
        else {
            if (m_kyokumen.MakeLegalMoves(Enemy) <= 0) {
                Print << U"�v���C���[�̏����I";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Player;
            }
        }
        m_thinkingTimer.restart();
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

    void AddHoldKoma(KomaSquare& koma_);
public:
    BanSelf(const array<const array<const uint32, 9>, 9>& iniKyokumen_, const Turn& turn_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;

    void SelfUpdate();

    void SelfAIUpdate();

    void EnemyUpdate();

    void Draw() const;

    [[nodiscard]] Turn GetTurn() const noexcept {
        return m_turn;
    }

    [[nodiscard]] Winner GetWinner() const noexcept {
        return m_winner;
    }
};

// �Q�[���V�[��
class GameAI : public MyApp::Scene {
private:
    BanSelf m_ban;

    void result();
public:

    GameAI(const InitData& init);

    void update() override;

    void draw() const override;
};
