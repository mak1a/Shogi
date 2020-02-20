#pragma once

#include"Koma.hpp"


/// <summary>
/// ��̃N���X
/// </summary>
class Te {
private:
	/// <summary>
	/// �ǂ�����ǂ��ւ͂��ꂼ��1�o�C�g�ŕ\����
	/// </summary>
	/// <summary>
	/// �ڂ�����Kyokumen�N���X���Q��
	/// </summary>
	uint32 m_from, m_to;

	/// <summary>
	/// ����������
	/// </summary>
	uint32 m_koma;

	/// <summary>
	/// �������
	/// </summary>
	uint32 m_capture;

	// ��/�s��
	uint32 m_promote;

	// �����̊g���p�Ɏg���炵��
	// ���Ɏg�����͍��̂Ƃ���킩��܂���()
	uint32 m_kind;

	// ���̎�̉��]��
	int32 m_value;

public:
	[[nodiscard]] bool IsNull () const noexcept {
		return m_from == 0 && m_to == 0;
	}

	Te () = default;

	constexpr Te (const uint32 from_, const uint32 to_, const uint32 koma_, const uint32 capture_ = Emp,
		const uint32 promote_ = 0, const uint32 kind_ = 0, const int32 value_ = 0) noexcept
		: m_from (from_)
		, m_to (to_)
		, m_capture (capture_)
		, m_promote (promote_)
		, m_koma ((promote_ == 0) ? koma_ : koma_ | Promote)
		, m_kind (kind_)
		, m_value (value_) {}

	[[nodiscard]] uint32 GetFrom () const noexcept {
		return m_from;
	}
	[[nodiscard]] uint32 GetTo () const noexcept {
		return m_to;
	}
	[[nodiscard]] uint32 GetKoma () const noexcept {
		return m_koma;
	}
	[[nodiscard]] uint32 GetPromote () const noexcept {
		return m_promote;
	}
	void SetPromote (const uint32 promote_) noexcept {
		m_promote = promote_;
		m_koma = (m_promote == 0) ? m_koma : m_koma | Promote;
	}
	[[nodiscard]] uint32 GetCapture () const noexcept {
		return m_capture;
	}
	void SetCapture (const uint32 capture_) noexcept {
		m_capture = capture_;
	}
	[[nodiscard]] uint32 GetKind () const noexcept {
		return m_kind;
	}
	[[nodiscard]] uint32 GetValue () const noexcept {
		return m_value;
	}

	// ��̓��ꐫ���r���������Ɏg��
	// kind��value���Ⴍ�Ă�������Ƃ݂Ȃ�
	bool operator==(const Te& te_) const noexcept {
		return te_.GetFrom () == m_from && te_.GetTo () == m_to
			&& te_.GetKoma () == m_koma && te_.GetPromote () == m_promote;
	}

};


class Kyokumen {
private:
	/// <summary>
	/// �j�n�̗�����ban����͂ݏo���̂ŁA�͂ݏo�������m�ۂ��Ă���
	/// C++�ł́A�\���̂̓����̕ϐ��̕��я��͐錾�������ɂȂ邱�Ƃ𗘗p���Ă���
	/// ���ʂł͂��܂�g��Ȃ��u�����v�e�N�j�b�N
	/// </summary>
	array<uint32, 16> m_banPadding;

	/// <summary>
	/// �Q�����z�񂾂ƒx���̂ŁA1�����z����g��
	/// </summary>
	array<uint32, 16 * 11> m_ban;

	// ��̗�����ێ�����
	array<uint32, 16 * 11> m_controlSelf, m_controlEnemy;

	// ������̐��B
	array<uint32, 41> m_holdingKomas;

	// ���̋ǖʂ̎萔
	uint32 m_tesu;

	// �݂��̉��̈ʒu
	uint32 m_kingSelfPos, m_kingEnemyPos;

	// �ǖʂ̕]���l
	int32 m_value;

	Array<Te> m_teValids;

	array<uint32, 16 * 11> m_pin;

	void InitControl ();

	[[nodiscard]] bool Uchifudume (const uint32 isSelfOrEnemy_, const uint32 to_);

	// ���̓�����̐���
	void MoveKing (const uint32 isSelfOrEnemy_, const uint32 kiki_);

	void MoveTo (const uint32 isSelfOrEnemy_, const uint32 to_);

	void PutTo (const uint32 isSelfOrEnemy_, const uint32 pos_);

	[[nodiscard]] uint32 CountControlSelf (const uint32 pos_);

	[[nodiscard]] uint32 CountControlEnemy (const uint32 pos_);

	[[nodiscard]] uint32 CountMove (const uint32 isSelfOrEnemy_, const uint32 pos_);

	void AddMoves (const uint32 isSelfOrEnemy_, const uint32 from_, const int32 pin_, const int32 rPin_ = 0);

	void AddStraight (const uint32 isSelfOrEnemy_, const uint32 from_, const int32 dir_, const int32 pin_, const int32 rPin_ = 0);

	void AddMove (const uint32 isSelfOrEnemy_, const uint32 from_, const int32 diff_, const int32 pin_, const int32 rPin_ = 0);

	bool IsCorrectMove (Te* te_);

	int32 EvalMin (Array<Te>& moveSelf_, Array<Te>& moveEnemy_);

	int32 EvalMax (Array<Te>& moveSelf_, Array<Te>& moveEnemy_);

	int32 Eval (const uint32 pos_);

public:
	Kyokumen () = default;

	Kyokumen (const uint32 tesu_, const array<const array<const uint32, 9>, 9>& board_, const array<uint32, 41>& motigoma_ = array<uint32, 41> ()) noexcept;

	[[nodiscard]] inline uint32 Search (uint32 pos_, int32 dir_) const noexcept {
		do {
			if (static_cast<int32>(pos_) + dir_ < 0) {
				break;
			}
			pos_ += dir_;
		}
		while (m_ban[pos_] == Empty);

		return pos_;
	}

	[[nodiscard]] bool IsIllegal (const Te& te_) const noexcept {
		for (const auto& te : m_teValids) {
			if (te_ == te) {
				return false;
			}
		}

		return true;
	}

	[[nodiscard]] Array<Te> GetTeValids () const noexcept {
		return m_teValids;
	}
	[[nodiscard]] int32 GetValue () const noexcept {
		return m_value;
	}
	[[nodiscard]] array<uint32, 41> GetHoldingKomas () const noexcept {
		return m_holdingKomas;
	}

	void MakePinInfo ();

	uint32 MakeLegalMoves (const uint32 isSelfOrEnemy_);

	// uint32 MakeLegalMoves(const uint32 isSelfOrEnemy_);

	uint32 AntiCheck (const uint32 isSelfOrEnemy_, const uint32 control_);

	void Move (const uint32 isSelfOrEnemy_, const Te& te_);

	int32 BestEval (const uint32 isSelfOrEnemy_);
};
