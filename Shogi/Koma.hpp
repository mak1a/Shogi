# pragma once
#include"Common.hpp"

/// <summary>
/// ��̎��
/// </summary>
enum KomaType {
    // �����Ȃ�
    Empty = 0,
    Emp = 0,
    // ����ɂ���ڈ�
    Promote = 8,
    // ���\��
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

    // �������g�̋�ɂ���ڈ�
    Self = 16,
    // �G�̋�ɂ���ڈ�
    Enemy = 32,
    // �ՊO
    Wall = Self + Enemy,

    // ���ۂ̋�
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
/// ��ǂ��ɂ��邩
/// </summary>
enum class KomaState {
    Ban,
    Dai
};

/// <summary>
/// ��ɒu���ۂ̋�̏ꏊ
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
    /// �����̎�����̏ꏊ
    /// </summary>
    constexpr array<const Vec2, 7> selfDaiPoses = {
        selfFu, selfKyo, selfKei, selfGin, selfKin, selfKaku, selfHi
    };
    /// <summary>
    /// �G�̎�����̏ꏊ
    /// </summary>
    constexpr array<const Vec2, 7> enemyDaiPoses = {
        enemyFu, enemyKyo, enemyKei, enemyGin, enemyKin, enemyKaku, enemyHi
    };
}

// ������\��
constexpr array<const int32, 12> Direct = {
    //17,1,-15,16,-16,15,-1,-17,14,-18,18,-14
    11, 1, -9, 10, -10, 9, -1, -11, 8, -12, 12, -8
};

// ���鎖���ł����H
constexpr array<const bool, 64> CanPromote = {
    //  ������j����p�򉤂ƈǌ\�S���n��
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,
        //  ������j����p�򉤂ƈǌ\�S���n��
            0,1,1,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// ���̕����ɓ����邩�H���̕����ɔ��ōs�����͓���Ȃ��B
// �Ⴆ�Ίp
constexpr array<const array<const bool, 64>, 12> CanMove{
    {
        {
            //  ������j����p�򉤂ƈǌ\�S���n��
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
                //  ������j����p�򉤂ƈǌ\�S���n��
                    0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                }, {
                    //  ������j����p�򉤂ƈǌ\�S���n��
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
                        //  ������j����p�򉤂ƈǌ\�S���n��
                            0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                        }, {
                            //  ������j����p�򉤂ƈǌ\�S���n��
                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,
                                //  ������j����p�򉤂ƈǌ\�S���n��
                                    0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                }, {
                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                            0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                        }, {
                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,
                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                    0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                }, {
                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,
                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                        }, {
                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,1,0,0,1,1,1,1,1,1,1,0,
                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                    0,0,0,0,0,1,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                }, {
                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,1,1,1,1,1,1,0,1,
                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                            0,0,0,0,1,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                        }, {
                                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                }, {
                                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                        }, {
                                                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                    0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                                }, {
                                                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                            0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                                        }
                                                                                                    }
};

// ���̕����ɔ��œ����������ł��邩�H
// ���Ȃ݂ɔ�p���Ɨ��Ɣn��������������͂Ȃ�
constexpr array<const array<const bool, 64>, 12> CanJump{
    {
        {
            //  ������j����p�򉤂ƈǌ\�S���n��
                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
                //  ������j����p�򉤂ƈǌ\�S���n��
                    0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                }, {
                    //  ������j����p�򉤂ƈǌ\�S���n��
                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
                        //  ������j����p�򉤂ƈǌ\�S���n��
                            0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                        }, {
                            //  ������j����p�򉤂ƈǌ\�S���n��
                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
                                //  ������j����p�򉤂ƈǌ\�S���n��
                                    0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                }, {
                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                            0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                        }, {
                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,
                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                }, {
                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                        }, {
                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,1,
                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                    0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                }, {
                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,
                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                            0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                        }, {
                                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                }, {
                                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                        }, {
                                                                                            //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                                //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                                }, {
                                                                                                    //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
                                                                                                        //  ������j����p�򉤂ƈǌ\�S���n��
                                                                                                            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
                                                                                                        }
                                                                                                    }
};

constexpr array<const int32, 40> HandValue{
    //  �����j����p��
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,105,630,735,1050,1260,1890,2100,0,0,0,0,0,0,0,0,
        0,-105,-630,-735,-1050,-1260,-1890,-2100
};

constexpr array<const int32, 49> KomaValue{
    //  ������j����p�򉤂ƈǌ\�S���n��
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,600,700,1000,1200,1800,2000,10000,1200,1200,1200,1200,1200,2000,2200,
        //  ������j����p�򉤂ƈǌ\�S���n��
            0,-100,-600,-700,-1000,-1200,-1800,-2000,-10000,-1200,-1200,-1200,-1200,-1200,-2000,-2200,0
};

inline array<array<uint64, 100>, 48> HashSeeds;
inline array<array<uint64, 19>, 40> HashHandSeeds;

class KomaSquare : public RectF {
private:
    uint32 m_komaType;
    KomaState m_komaState;

    Point m_coodinate;
public:
    constexpr KomaSquare(const double x_, const double y_, const double size_, const uint32 koma_, const Point& coodinate_) noexcept
        : RectF(x_, y_, size_)
        , m_komaType(koma_)
        , m_komaState(KomaState::Ban)
        , m_coodinate(coodinate_) {}

    constexpr KomaSquare(const Vec2 pos_, const double size_, const uint32 koma_, const Point& coodinate_) noexcept
        : RectF(pos_, size_)
        , m_komaType(koma_)
        , m_komaState(KomaState::Ban)
        , m_coodinate(coodinate_) {}

    constexpr KomaSquare(const Vec2 pos_, const double size_, const uint32 koma_, const KomaState state_, const Point& coodinate_) noexcept
        : RectF(pos_, size_)
        , m_komaType(koma_)
        , m_komaState(state_)
        , m_coodinate(coodinate_) {}

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

    [[nodiscard]] Point GetKomaCoodinate() const noexcept {
        return m_coodinate;
    }
    constexpr void ChangeKomaCoodinate(const Point& komaCoodinate_) noexcept {
        m_coodinate.set(komaCoodinate_);
    }
    [[nodiscard]] bool IsChangeCoodinate(const KomaSquare& willPutPlace_) const noexcept {
        return m_coodinate != willPutPlace_.GetKomaCoodinate();
    }

    constexpr void PromoteKoma() noexcept {
        m_komaType |= Promote;
    }

    // ���`�悷��
    void Draw(const Vec2& difference_ = Vec2::Zero()) const {
        switch (GetKomaType()) {
        case Sfu:
            TextureAsset(U"Fu").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sto:
            TextureAsset(U"To").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sky:
            TextureAsset(U"Kyosha").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sny:
            TextureAsset(U"NariKyosha").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ske:
            TextureAsset(U"Keima").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Snk:
            TextureAsset(U"NariKeima").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sgi:
            TextureAsset(U"Gin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sng:
            TextureAsset(U"NariGin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ski:
            TextureAsset(U"Kin").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Ska:
            TextureAsset(U"Kaku").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sum:
            TextureAsset(U"Uma").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Shi:
            TextureAsset(U"Hi").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sry:
            TextureAsset(U"Ryu").resized(size).drawAt(center().movedBy(difference_));
            break;
        case Sou:
            TextureAsset(U"Ou").resized(size).drawAt(center().movedBy(difference_));
            break;

            // ��������G�̋�
        case Efu:
            TextureAsset(U"Fu").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eto:
            TextureAsset(U"To").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eky:
            TextureAsset(U"Kyosha").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eny:
            TextureAsset(U"NariKyosha").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eke:
            TextureAsset(U"Keima").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Enk:
            TextureAsset(U"NariKeima").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Egi:
            TextureAsset(U"Gin").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eng:
            TextureAsset(U"NariGin").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eki:
            TextureAsset(U"Kin").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eka:
            TextureAsset(U"Kaku").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eum:
            TextureAsset(U"Uma").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Ehi:
            TextureAsset(U"Hi").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Ery:
            TextureAsset(U"Ryu").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        case Eou:
            TextureAsset(U"Gyoku").resized(size)
                .rotated(180_deg).drawAt(center().movedBy(difference_));
            break;
        default:
            break;
        }
    }
};
