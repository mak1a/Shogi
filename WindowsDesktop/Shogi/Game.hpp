
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

    // �v���C���[�����Ȃ�true
    bool m_isBehind = false;

    /// <summary>
    /// �҂����{�^��
    /// </summary>
    RectF m_buttonWaited;

    /// <summary>
    /// �����{�^��
    /// </summary>
    RectF m_buttonQuit;

    // ��Ԃ���シ��
    void ChangeCurrentTurn() noexcept {
        AudioAsset(U"Piece").playOneShot(0.2);
        ClearPrint();

        if (m_kyokumen.IsSennitite()) {
            m_winner = m_kyokumen.IsContinuous(m_turn);
            m_turn = Turn::Tsumi;
            return;
        }

        m_turn = (m_turn == Turn::Player) ? Turn::Enemy : Turn::Player;
        if (GetTurn() == Turn::Player) {
            if (m_kyokumen.MakeLegalMoves(Self) <= 0) {
                //Print << U"You Lose!";
                m_turn = Turn::Tsumi;
                m_winner = Winner::Enemy;
            }

            m_stackKyokumens.emplace(m_kyokumen);
            m_stackBoradSquares.emplace(m_boardSquares);
            m_stackHavingSelf.emplace(m_havingSelfKoma);
            m_stackHavingEnemy.emplace(m_havingEnemyKoma);
            m_stackPlacedPart.emplace(m_placedPart);
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

    // �҂����ׂ̈ɋǖʃN���X��ێ�
    std::stack<Kyokumen> m_stackKyokumens;

    std::stack<Array<KomaSquare>> m_stackBoradSquares;

    std::stack<Array<Array<KomaSquare>>> m_stackHavingSelf;
    std::stack<Array<Array<KomaSquare>>> m_stackHavingEnemy;

    std::stack<Optional<KomaSquare>> m_stackPlacedPart;

    void AddHoldKoma(KomaSquare& koma_);

    void RetractingMove() {
        if (m_stackKyokumens.size() <= 1) {
            return;
        }
        if (m_stackKyokumens.size() == 2 && m_isBehind) {
            return;
        }

        m_stackKyokumens.pop();
        m_kyokumen = m_stackKyokumens.top();

        m_stackBoradSquares.pop();
        m_boardSquares.assign(m_stackBoradSquares.top().begin(), m_stackBoradSquares.top().end());

        m_stackHavingSelf.pop();
        m_havingSelfKoma.assign(m_stackHavingSelf.top().begin(), m_stackHavingSelf.top().end());

        m_stackHavingEnemy.pop();
        m_havingEnemyKoma.assign(m_stackHavingEnemy.top().begin(), m_stackHavingEnemy.top().end());

        m_stackPlacedPart.pop();
        m_placedPart = m_stackPlacedPart.top();
    }
public:
    Ban(const array<array<uint32, 9>, 9>& iniKyokumen_, const array<uint32, 40>& motigomas_, const Turn& turn_, const double shogiBan_ = 540.f, const double komaDai_ = 240.f) noexcept;

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
