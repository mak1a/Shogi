#pragma once
#include "Common.hpp"

/// <summary>
/// 駒の種類
/// </summary>
enum KomaType {
    // 何もない
    Empty = 0,
    Emp = 0,
    // 成駒につける目印
    Promote = 8,
    // 駒を表す
    Fu = 1,
    Ky = 2,
    Ke = 3,
    Gi = 4,
    Ki = 5,
    Ka = 6,
    Hi = 7,
    Ou = 8,
    To = Fu + Promote,
    Ny = Ky + Promote,
    Nk = Ke + Promote,
    Ng = Gi + Promote,
    Um = Ka + Promote,
    Ry = Hi + Promote,

    // 自分自身の駒につける目印
    Self = 16,
    // 敵の駒につける目印
    Enemy = 32,
    // 盤外
    Wall = Self + Enemy,

    // 実際の駒
    Sfu = Self + Fu,
    Sto = Self + To,
    Sky = Self + Ky,
    Sny = Self + Ny,
    Ske = Self + Ke,
    Snk = Self + Nk,
    Sgi = Self + Gi,
    Sng = Self + Ng,
    Ski = Self + Ki,
    Ska = Self + Ka,
    Sum = Self + Um,
    Shi = Self + Hi,
    Sry = Self + Ry,
    Sou = Self + Ou,

    Efu = Enemy + Fu,
    Eto = Enemy + To,
    Eky = Enemy + Ky,
    Eny = Enemy + Ny,
    Eke = Enemy + Ke,
    Enk = Enemy + Nk,
    Egi = Enemy + Gi,
    Eng = Enemy + Ng,
    Eki = Enemy + Ki,
    Eka = Enemy + Ka,
    Eum = Enemy + Um,
    Ehi = Enemy + Hi,
    Ery = Enemy + Ry,
    Eou = Enemy + Ou,
};

/// <summary>
/// 駒がどこにあるか
/// </summary>
enum class KomaState { Ban, Dai };

/// <summary>
/// 台に置く際の駒の場所
/// </summary>
namespace KomaPos {
    constexpr s3d::Vec2 selfFu = s3d::Vec2(920.0, 570.0);
    constexpr s3d::Vec2 enemyFu = s3d::Vec2(300.0, 90.0);

    constexpr s3d::Vec2 selfKyo = s3d::Vec2(920.0, 510.0);
    constexpr s3d::Vec2 enemyKyo = s3d::Vec2(300.0, 150.0);

    constexpr s3d::Vec2 selfKei = s3d::Vec2(1040.0, 510.0);
    constexpr s3d::Vec2 enemyKei = s3d::Vec2(180.0, 150.0);

    constexpr s3d::Vec2 selfGin = s3d::Vec2(920.0, 450.0);
    constexpr s3d::Vec2 enemyGin = s3d::Vec2(300.0, 210.0);

    constexpr s3d::Vec2 selfKin = s3d::Vec2(1040.0, 450.0);
    constexpr s3d::Vec2 enemyKin = s3d::Vec2(180.0, 210.0);

    constexpr s3d::Vec2 selfKaku = s3d::Vec2(920.0, 390.0);
    constexpr s3d::Vec2 enemyKaku = s3d::Vec2(300.0, 270.0);

    constexpr s3d::Vec2 selfHi = s3d::Vec2(1040.0, 390.0);
    constexpr s3d::Vec2 enemyHi = s3d::Vec2(180.0, 270.0);

    /// <summary>
    /// 自分の持ち駒の場所
    /// </summary>
    constexpr array<const s3d::Vec2, 7> selfDaiPoses = {selfFu, selfKyo, selfKei, selfGin, selfKin, selfKaku, selfHi};
    /// <summary>
    /// 敵の持ち駒の場所
    /// </summary>
    constexpr array<const s3d::Vec2, 7> enemyDaiPoses = {enemyFu, enemyKyo, enemyKei, enemyGin, enemyKin, enemyKaku, enemyHi};
}  // namespace KomaPos

// 方向を表す
constexpr array<const int32, 12> Direct = {
    // 17,1,-15,16,-16,15,-1,-17,14,-18,18,-14
    11, 1, -9, 10, -10, 9, -1, -11, 8, -12, 12, -8};

// 成る事ができる駒か？
constexpr array<const bool, 64> CanPromote = {
    //  空歩香桂銀金角飛王と杏圭全金馬龍
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
    //  空歩香桂銀金角飛王と杏圭全金馬龍
    0, 1, 1, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// その方向に動けるか？その方向に飛んで行ける駒は入れない。
// 例えば角
constexpr array<const array<const bool, 64>, 12> CanMove{{{//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

// その方向に飛んで動く事っができるか？
// ちなみに飛角香と龍と馬しかそういう駒はない
constexpr array<const array<const bool, 64>, 12> CanJump{{{//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                                          {//  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                                           //  空歩香桂銀金角飛王と杏圭全金馬龍
                                                           0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};

constexpr array<const int32, 40> HandValue{
    //  歩香桂銀金角飛王
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105, 630, 735, 1050, 1260, 1890, 2100, 0, 0, 0, 0, 0, 0, 0, 0, 0, -105, -630, -735, -1050, -1260, -1890, -2100};

constexpr array<const int32, 49> KomaValue{
    //  空歩香桂銀金角飛王と杏圭全金馬龍
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 100, 600, 700, 1000, 1200, 1800, 2000, 10000, 1200, 1200, 1200, 1200, 1200, 2000, 2200,
    //  空歩香桂銀金角飛王と杏圭全金馬龍
    0, -100, -600, -700, -1000, -1200, -1800, -2000, -10000, -1200, -1200, -1200, -1200, -1200, -2000, -2200, 0};

inline array<array<uint64, 100>, 48> HashSeeds;
inline array<array<uint64, 19>, 40> HashHandSeeds;

class KomaSquare : public s3d::RectF {
private:
    uint32 m_komaType;
    KomaState m_komaState;

    s3d::Point m_coodinate;

public:
    constexpr KomaSquare(const double x_, const double y_, const double size_, const uint32 koma_, const s3d::Point& coodinate_) noexcept
    : s3d::RectF(x_, y_, size_), m_komaType(koma_), m_komaState(KomaState::Ban), m_coodinate(coodinate_) {}

    constexpr KomaSquare(const s3d::Vec2 pos_, const double size_, const uint32 koma_, const s3d::Point& coodinate_) noexcept
    : s3d::RectF(pos_, size_), m_komaType(koma_), m_komaState(KomaState::Ban), m_coodinate(coodinate_) {}

    constexpr KomaSquare(const s3d::Vec2 pos_, const double size_, const uint32 koma_, const KomaState state_, const s3d::Point& coodinate_) noexcept
    : s3d::RectF(pos_, size_), m_komaType(koma_), m_komaState(state_), m_coodinate(coodinate_) {}

    [[nodiscard]] uint32 GetKomaType() const noexcept {
        return m_komaType;
    }
    void ChangeKomaType(const uint32 komaType_) {
        m_komaType = komaType_;
    }

    [[nodiscard]] KomaState GetKomaState() const noexcept {
        return m_komaState;
    }
    void ChangeKomaState(const KomaState& komaState_) {
        m_komaState = komaState_;
    }

    [[nodiscard]] s3d::Point GetKomaCoodinate() const noexcept {
        return m_coodinate;
    }
    constexpr void ChangeKomaCoodinate(const s3d::Point& komaCoodinate_) noexcept {
        m_coodinate.set(komaCoodinate_);
    }
    [[nodiscard]] bool IsChangeCoodinate(const KomaSquare& willPutPlace_) const noexcept {
        return m_coodinate != willPutPlace_.GetKomaCoodinate();
    }

    constexpr void PromoteKoma() noexcept {
        if (m_komaType != Eou && m_komaType != Sou && m_komaType != Eki && m_komaType != Ski) {
            m_komaType |= Promote;
        }
    }

    constexpr void ReturnPromoteKoma() noexcept {
        m_komaType &= ~Promote;
    }

    // 駒を描画する
    void Draw(const s3d::Vec2& difference_ = s3d::Vec2::Zero()) const {
        switch (GetKomaType()) {
        case Sfu:
            s3d::TextureAsset(U"Fu").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sto:
            s3d::TextureAsset(U"To").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sky:
            s3d::TextureAsset(U"Kyosha").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sny:
            s3d::TextureAsset(U"NariKyosha").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ske:
            s3d::TextureAsset(U"Keima").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Snk:
            s3d::TextureAsset(U"NariKeima").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sgi:
            s3d::TextureAsset(U"Gin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sng:
            s3d::TextureAsset(U"NariGin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ski:
            s3d::TextureAsset(U"Kin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ska:
            s3d::TextureAsset(U"Kaku").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sum:
            s3d::TextureAsset(U"Uma").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Shi:
            s3d::TextureAsset(U"Hi").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sry:
            s3d::TextureAsset(U"Ryu").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sou:
            s3d::TextureAsset(U"Ou").resized(size).drawAt(center().movedBy(difference_));
            break;

        // ここから敵の駒
        case Efu:
            s3d::TextureAsset(U"Fu").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eto:
            s3d::TextureAsset(U"To").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eky:
            s3d::TextureAsset(U"Kyosha").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eny:
            s3d::TextureAsset(U"NariKyosha").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eke:
            s3d::TextureAsset(U"Keima").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Enk:
            s3d::TextureAsset(U"NariKeima").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Egi:
            s3d::TextureAsset(U"Gin").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eng:
            s3d::TextureAsset(U"NariGin").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eki:
            s3d::TextureAsset(U"Kin").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eka:
            s3d::TextureAsset(U"Kaku").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eum:
            s3d::TextureAsset(U"Uma").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Ehi:
            s3d::TextureAsset(U"Hi").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Ery:
            s3d::TextureAsset(U"Ryu").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        case Eou:
            s3d::TextureAsset(U"Gyoku").resized(size).rotated(s3d::ToRadians(180)).drawAt(center().movedBy(difference_));
            break;
        default:
            break;
        }
    }
};

enum class YesNoSelection {
    Yes,  // Yes
    No,   // No
    None  // None
};

class MyMessageBox {
private:
    s3d::RectF m_messageBox;
    s3d::RectF m_yesButton;
    s3d::RectF m_noButton;

    YesNoSelection m_selection;

public:
    MyMessageBox() noexcept
    : m_messageBox(s3d::Arg::center(s3d::Scene::CenterF().movedBy(0.0, -100.0)), 280.0, 250.0)
    , m_yesButton(s3d::Arg::center(m_messageBox.center().movedBy(-50.0, 50)), 80.0, 30.0)
    , m_noButton(s3d::Arg::center(m_messageBox.center().movedBy(50.0, 50)), 80.0, 30.0)
    , m_selection(YesNoSelection::None) {}

    YesNoSelection Select() {
        m_selection = YesNoSelection::None;

        if (m_yesButton.leftClicked()) {
            m_selection = YesNoSelection::Yes;
        }
        if (m_noButton.leftClicked()) {
            m_selection = YesNoSelection::No;
        }

        return m_selection;
    }

    void Draw(s3d::StringView title_) const {
        m_messageBox.draw(s3d::ColorF(s3d::Palette::Whitesmoke, 0.94));
        s3d::FontAsset(U"Menu")(title_).drawAt(m_messageBox.center().movedBy(0.0, -50.0), s3d::Palette::Black);

        m_yesButton.drawFrame(1.0, s3d::ColorF(s3d::Palette::Black, 0.94));
        s3d::FontAsset(U"YesNo")(U"Yes").drawAt(m_yesButton.center(), s3d::Palette::Black);
        m_noButton.drawFrame(1.0, s3d::ColorF(s3d::Palette::Black, 0.94));
        s3d::FontAsset(U"YesNo")(U"No").drawAt(m_noButton.center(), s3d::Palette::Black);

        if (m_yesButton.mouseOver()) {
            m_yesButton.drawFrame(1.6, s3d::ColorF(s3d::Palette::Aqua, 0.6f));
        }
        if (m_noButton.mouseOver()) {
            m_noButton.drawFrame(1.6, s3d::ColorF(s3d::Palette::Aqua, 0.6f));
        }
    }
};

class MyMessageWindow {
private:
    s3d::RectF m_messageBox;

    s3d::RectF m_okButton;

    s3d::String m_label;

    bool m_isUseSelectButton;

public:
    MyMessageWindow() noexcept
    : m_messageBox(s3d::Arg::center(s3d::Scene::CenterF().movedBy(0.0, -100.0)), 280.0, 250.0)
    , m_okButton(s3d::Arg::center(m_messageBox.center().movedBy(0.0, 50)), 80.0, 30.0)
    , m_isUseSelectButton(false) {}

    bool SelectOK() {
        return m_okButton.leftClicked();
    }

    void SetLabel(const s3d::StringView label_, const bool isUseSelect_ = false) {
        m_label = label_;
        m_isUseSelectButton = isUseSelect_;
    }

    void Draw() const {
        m_messageBox.draw(s3d::ColorF(s3d::Palette::Whitesmoke, 0.94));
        s3d::FontAsset(U"Explain")(m_label).drawAt(m_messageBox.center().movedBy(0.0, -20.0), s3d::Palette::Black);

        if (!m_isUseSelectButton) {
            return;
        }

        m_okButton.drawFrame(1.0, s3d::ColorF(s3d::Palette::Black, 0.94));
        s3d::FontAsset(U"YesNo")(U"Yes").drawAt(m_okButton.center(), s3d::Palette::Black);

        if (m_okButton.mouseOver()) {
            m_okButton.drawFrame(1.6, s3d::ColorF(s3d::Palette::Aqua, 0.6f));
        }
    }
};
