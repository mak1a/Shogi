﻿#include"Sikou.hpp"

/// <summary>
/// αβ法による探索。
/// </summary>
[[nodiscard]] int32 Sikou::MinMax(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, const uint32 depth_) noexcept {
	if (depth_ >= m_depthMax) {
		/// <summary>
		/// 現在の局面の評価値を返す。
		/// </summary>
		return kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_);
	}

	uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
	if (teNum == 0) {
		return ((isSelfOrEnemy_ == Self) ? -999999 : 999999);
	}

	Array<Te> teValids{kyokumen_.GetTeValids()};
	teValids.shuffle();

	int32 retVal{(isSelfOrEnemy_ == Self) ? -1000000 : 1000000};

	for (const uint32 i : step(teNum)) {
		Kyokumen k{kyokumen_};
		k.Move(isSelfOrEnemy_, teValids[i]);

		int32 value{MinMax(isSelfOrEnemy_==Self?Enemy:Self, k, depth_+1)};
		if ((isSelfOrEnemy_ == Self && value > retVal) || (isSelfOrEnemy_ == Enemy && value < retVal)) {
			retVal = value;
			m_bestHands[depth_][depth_] = teValids[i];

			for (uint32 i{depth_+1}; i < m_depthMax; ++i) {
				m_bestHands[depth_][i] = m_bestHands[depth_+1][i];
			}
		}
	}

	return retVal;
}

[[nodiscard]] int32 Sikou::AlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept {
	if (depth_ >= m_depthMax) {
		/// <summary>
		/// 現在の局面の評価値を返す。
		/// </summary>
		return kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_);
	}

	uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
	if (teNum == 0) {
		return ((isSelfOrEnemy_ == Self) ? -999999 : 999999);
	}

	Array<Te> teValids{kyokumen_.GetTeValids()};
	teValids.shuffle();

	int32 retVal{(isSelfOrEnemy_ == Self) ? -1000000 : 1000000};

	for (const uint32 i : step(teNum)) {
		Kyokumen k{kyokumen_};
		k.Move(isSelfOrEnemy_, teValids[i]);

		int32 value{AlphaBeta(isSelfOrEnemy_==Self?Enemy:Self, k, alpha_, beta_, depth_+1)};
		if ((isSelfOrEnemy_ == Self && value > retVal) || (isSelfOrEnemy_ == Enemy && value < retVal)) {
			retVal = value;
			m_bestHands[depth_][depth_] = teValids[i];

			for (uint32 i{depth_+1}; i < m_depthMax; ++i) {
				m_bestHands[depth_][i] = m_bestHands[depth_+1][i];
			}

			if (isSelfOrEnemy_ == Self && alpha_ < retVal) {
				alpha_ = retVal;
			}
			if (isSelfOrEnemy_ == Enemy && beta_ > retVal) {
				beta_ = retVal;
			}
			if ((isSelfOrEnemy_ == Self && retVal >= beta_) || (isSelfOrEnemy_ == Enemy && retVal <= alpha_)) {
				return retVal;
			}
		}
	}

	return retVal;
}

[[nodiscard]] int32 Sikou::NegaAlphaBeta(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, int32 alpha_, int32 beta_, const uint32 depth_) noexcept {
	if (depth_ >= m_depthMax) {
		return ((isSelfOrEnemy_ == Self)
			? kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_)
			: -(kyokumen_.GetValue() + kyokumen_.BestEval(isSelfOrEnemy_)));
	}

	uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
	if (teNum == 0) {
		return -999999;
	}

	Array<Te> teValids{kyokumen_.GetTeValids()};
	teValids.shuffle();

	int32 retVal{-1000000};

	for (const uint32 i : step(teNum)) {
		Kyokumen k{kyokumen_};
		k.Move(isSelfOrEnemy_, teValids[i]);

		int32 value{-NegaAlphaBeta(isSelfOrEnemy_==Self?Enemy:Self, k, -beta_, -alpha_, depth_+1)};
		if (value > retVal) {
			retVal = value;
			m_bestHands[depth_][depth_] = std::move(teValids[i]);

			for (uint32 j{depth_+1}; j < m_depthMax; ++j) {
				m_bestHands[depth_][j] = m_bestHands[depth_+1][j];
			}

			if (retVal > alpha_) {
				alpha_ = retVal;
			}
			if (retVal >= beta_) {
				return retVal;
			}
		}
	}

	return retVal;
}


/// <summary>
/// クソ雑魚探索
/// </summary>
[[nodiscard]] Te Sikou::Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_) noexcept {
	uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
	Array<Te> teValids{kyokumen_.GetTeValids()};
	teValids.shuffle();
	uint32 best{};
	int32 bestVal{(isSelfOrEnemy_ == Self) ? -1000000 : +1000000};
	for (uint32 i{}; i < teNum; ++i) {
		Kyokumen k{kyokumen_};
		k.Move(isSelfOrEnemy_, teValids[i]);
		if (isSelfOrEnemy_ == Self && bestVal < k.GetValue() + k.BestEval(Enemy)) {
			bestVal = k.GetValue() + k.BestEval(Enemy);
			best = i;
		}
		if (isSelfOrEnemy_ == Enemy && bestVal > k.GetValue() + k.BestEval(Self)) {
			bestVal = k.GetValue() + k.BestEval(Self);
			best = i;
		}
	}
	return teValids[best];
}

[[nodiscard]] Te Sikou::Think(const uint32 isSelfOrEnemy_, Kyokumen kyokumen_, const SearchType type_) noexcept {
	int32 bestVal{};

	switch (type_) {
	case SearchType::MinMax:
		bestVal={MinMax(isSelfOrEnemy_, kyokumen_, 0)};
		//Print << bestVal;
		break;
	case SearchType::AlphaBeta:
		bestVal={AlphaBeta(isSelfOrEnemy_, kyokumen_, -999999, 999999, 0)};
		//Print << bestVal;
		break;
	case SearchType::NegaAlphaBeta:
		bestVal={NegaAlphaBeta(isSelfOrEnemy_, kyokumen_, -999999, 999999, 0)};
		//Print << bestVal;
		break;
	default:
		break;
	}

	return m_bestHands[0][0];
}