#include"Sikou.hpp"

Sikou::Sikou() noexcept {}

/// <summary>
/// αβ法による探索。
/// </summary>
[[nodiscard]] int32 Sikou::MinMax(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_, const uint32 depth_, const uint32 depthMax_) noexcept {
	if (depth_ >= depthMax_) {
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
	int32 retVal = (isSelfOrEnemy_ == Self) ? -1000000 : 1000000;

	for (const uint32 i : step(teNum)) {
		Kyokumen k{kyokumen_};
		k.Move(isSelfOrEnemy_, teValids[i]);

		int32 value{MinMax(isSelfOrEnemy_==Self?Enemy:Self, k, depth_+1, depthMax_)};
		if ((isSelfOrEnemy_ == Self && value > retVal) || (isSelfOrEnemy_ == Enemy && value < retVal)) {
			retVal = value;
			m_bestHands[depth_][depth_] = teValids[i];

			for (uint32 i{depth_+1}; i < depthMax_; ++i) {
				m_bestHands[depth_][i] = m_bestHands[depth_+1][i];
			}
		}
	}

	return retVal;
}


[[nodiscard]] Te Sikou::Think(const uint32 isSelfOrEnemy_, Kyokumen& kyokumen_) noexcept {
	uint32 teNum{kyokumen_.MakeLegalMoves(isSelfOrEnemy_)};
	Array<Te> teValids{kyokumen_.GetTeValids()};
	for (const auto i : step(20)) {
		teValids.shuffle();
	}
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
