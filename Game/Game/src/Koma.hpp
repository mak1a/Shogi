# pragma once
#include"Common.hpp"

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
enum class KomaState {
    Ban,
    Dai
};

/// <summary>
/// 平手盤
/// </summary>
constexpr array<const array<const uint32, 9>, 9> HirateBan {
    {
        {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
        {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
        {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
        {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
        {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
        {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
        {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
        {Emp,Ska,Emp,Emp,Emp,Emp,Emp,Shi,Emp},
        {Sky,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Sky}
    }
};

/// <summary>
/// 台に置く際の駒の場所
/// </summary>
namespace KomaPos {
    constexpr Vec2 selfFu = Vec2(920.0, 570.0);
    constexpr Vec2 enemyFu = Vec2(300.0, 90.0);
    
    constexpr Vec2 selfKyo = Vec2(920.0, 510.0);
    constexpr Vec2 enemyKyo = Vec2(300.0, 150.0);

    constexpr Vec2 selfKei = Vec2(1040.0, 510.0);
    constexpr Vec2 enemyKei = Vec2(180.0, 150.0);

    constexpr Vec2 selfGin = Vec2(920.0, 450.0);
    constexpr Vec2 enemyGin = Vec2(300.0, 210.0);

    constexpr Vec2 selfKin = Vec2(1040.0, 450.0);
    constexpr Vec2 enemyKin = Vec2(180.0, 210.0);

    constexpr Vec2 selfKaku = Vec2(920.0, 390.0);
    constexpr Vec2 enemyKaku = Vec2(300.0, 270.0);

    constexpr Vec2 selfHi = Vec2(1040.0, 390.0);
    constexpr Vec2 enemyHi = Vec2(180.0, 270.0);

    /// <summary>
    /// 自分の持ち駒の場所
    /// </summary>
    constexpr array<const Vec2, 7> selfDaiPoses = {
        selfFu, selfKyo, selfKei, selfGin, selfKin, selfKaku, selfHi
    };
    /// <summary>
    /// 敵の持ち駒の場所
    /// </summary>
    constexpr array<const Vec2, 7> enemyDaiPoses = {
        enemyFu, enemyKyo, enemyKei, enemyGin, enemyKin, enemyKaku, enemyHi
    };
}

// 方向を表す
constexpr array<const int32, 12> Direct = {
    11, 1, -9, 10, -10, 9, -1, -11, 8, -12, 12, -8
};

// 成る事ができる駒か？
constexpr array<const bool, 32> CanPromote = {
//  空歩香桂銀金角飛王と杏圭全金馬龍
    0,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,
//  空歩香桂銀金角飛王と杏圭全金馬龍
    0,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,
};

// その方向に動けるか？その方向に飛んで行ける駒は入れない。
// 例えば角
constexpr array<const array<const bool, 32>, 12> CanMove {
    {
        {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0
        }
    }
};

// その方向に飛んで動く事っができるか？
// ちなみに飛角香と龍と馬しかそういう駒はない
constexpr array<const array<const bool, 32>, 12> CanJump {
    {
        {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }, {
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        //  空歩香桂銀金角飛王と杏圭全金馬龍
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
        }
    }
};

constexpr array<const int32, Ou> HandValue {
//  歩香桂銀金角飛王
    0,120,550,660,880,990,1400,1650
};

constexpr array<const int32, 32> KomaValue {
    //  空歩香桂銀金角飛王と杏圭全金馬龍
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    //  空歩香桂銀金角飛王と杏圭全金馬龍
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

class KomaSquare : public RectF {
private:
    KomaType m_komaType;
    KomaState m_komaState;

    Vec2 m_coodinate;
public:
    constexpr KomaSquare(const double x_, const double y_, const double size_, const KomaType koma_, const Vec2& coodinate_) noexcept
        : RectF(x_, y_, size_)
        , m_komaType(koma_)
        , m_komaState(KomaState::Ban)
        , m_coodinate(coodinate_) {}
    
    constexpr KomaSquare(const Vec2 pos_, const double size_, const KomaType koma_, const Vec2& coodinate_) noexcept
    : RectF(pos_, size_)
    , m_komaType(koma_)
    , m_komaState(KomaState::Ban)
    , m_coodinate(coodinate_) {}
    
    constexpr KomaSquare(const Vec2 pos_, const double size_, const KomaType koma_, const KomaState state_, const Vec2& coodinate_) noexcept
    : RectF(pos_, size_)
    , m_komaType(koma_)
    , m_komaState(state_)
    , m_coodinate(coodinate_) {}
    
    [[nodiscard]] KomaType GetKomaType() const noexcept {
        return m_komaType;
    }
    void ChangeKomaType(const KomaType komaType_) {
        m_komaType = komaType_;
    }
    
    [[nodiscard]] KomaState GetKomaState() const noexcept {
        return m_komaState;
    }
    void ChangeKomaState(const KomaState& komaState_) {
        m_komaState = komaState_;
    }

    [[nodiscard]] Vec2 GetKomaCoodinate() const noexcept {
        return m_coodinate;
    }
    constexpr void ChangeKomaCoodinate(const Vec2& komaCoodinate_) noexcept  {
        m_coodinate.set(komaCoodinate_);
    }
    [[nodiscard]] bool IsChangeCoodinate(const KomaSquare& willPutPlace_) const noexcept {
        return m_coodinate != willPutPlace_.GetKomaCoodinate();
    }
    
    // 駒を描画する
    void Draw() const {
        switch (GetKomaType()) {
            case Sfu:
                TextureAsset(U"Fu").resized(size).drawAt(center());
                break;
            case Sto:
                TextureAsset(U"To").resized(size).drawAt(center());
                break;
            case Sky:
                TextureAsset(U"Kyosha").resized(size).drawAt(center());
                break;
            case Sny:
                TextureAsset(U"NariKyosha").resized(size).drawAt(center());
                break;
            case Ske:
                TextureAsset(U"Keima").resized(size).drawAt(center());
                break;
            case Snk:
                TextureAsset(U"NariKeima").resized(size).drawAt(center());
                break;
            case Sgi:
                TextureAsset(U"Gin").resized(size).drawAt(center());
                break;
            case Sng:
                TextureAsset(U"NariGin").resized(size).drawAt(center());
                break;
            case Ski:
                TextureAsset(U"Kin").resized(size).drawAt(center());
                break;
            case Ska:
                TextureAsset(U"Kaku").resized(size).drawAt(center());
                break;
            case Sum:
                TextureAsset(U"Uma").resized(size).drawAt(center());
                break;
            case Shi:
                TextureAsset(U"Hi").resized(size).drawAt(center());
                break;
            case Sry:
                TextureAsset(U"Ryu").resized(size).drawAt(center());
                break;
            case Sou:
                TextureAsset(U"Ou").resized(size).drawAt(center());
                break;
                
            // ここから敵の駒
            case Efu:
                TextureAsset(U"Fu").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eto:
                TextureAsset(U"To").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eky:
                TextureAsset(U"Kyosha").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eny:
                TextureAsset(U"NariKyosha").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eke:
                TextureAsset(U"Keima").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Enk:
                TextureAsset(U"NariKeima").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Egi:
                TextureAsset(U"Gin").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eng:
                TextureAsset(U"NariGin").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eki:
                TextureAsset(U"Kin").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eka:
                TextureAsset(U"Kaku").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eum:
                TextureAsset(U"Uma").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Ehi:
                TextureAsset(U"Hi").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Ery:
                TextureAsset(U"Ryu").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            case Eou:
                TextureAsset(U"Gyoku").resized(size)
                .rotated(180_deg).drawAt(center());
                break;
            default:
                break;
        }
    }
};
