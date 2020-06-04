#pragma once
#include <Siv3D.hpp>
using std::array;

namespace Board {
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

    constexpr array<array<uint32, 9>, 9> Custom{
        {
            {Emp,Emp,Emp,Emp,Eou,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
            {Emp,Emp,Emp,Emp,Sou,Emp,Emp,Emp,Emp}
        }
    };

    namespace Myself {
        /// <summary>
        /// ����
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Hirate{
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
        /// �p����
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Bishop{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Shi,Emp},
                {Sky,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Sky}
            }
        };

        /// <summary>
        /// ��ԗ���
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Rook{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Ska,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sky,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Sky}
            }
        };

        /// <summary>
        /// 2������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> TwoPieces{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sky,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Sky}
            }
        };

        /// <summary>
        /// 4������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> FourDrops{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Emp}
            }
        };

        /// <summary>
        /// 6������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> SixDrops{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Sgi,Ski,Sou,Ski,Sgi,Emp,Emp}
            }
        };

        /// <summary>
        /// 8������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> EightDrops{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Ski,Sou,Ski,Emp,Emp,Emp}
            }
        };

        /// <summary>
        /// 10������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> TenDrops{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Sou,Emp,Emp,Emp,Emp}
            }
        };
    }

    namespace Opponent {
        /// <summary>
        /// ����
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Hirate{
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
        /// �p����
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Bishop{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Ehi,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
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
        /// ��ԗ���
        /// </summary>
        constexpr array<array<uint32, 9>, 9> Rook{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Eka,Emp},
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
        /// 2������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> TwoPieces{
            {
                {Eky,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Eky},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
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
        /// 4������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> FourDrops{
            {
                {Emp,Eke,Egi,Eki,Eou,Eki,Egi,Eke,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
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
        /// 6������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> SixDrops{
            {
                {Emp,Emp,Egi,Eki,Eou,Eki,Egi,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
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
        /// 8������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> EightDrops{
            {
                {Emp,Emp,Emp,Eki,Eou,Eki,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
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
        /// 10������
        /// </summary>
        constexpr array<array<uint32, 9>, 9> TenDrops{
            {
                {Emp,Emp,Emp,Emp,Eou,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu,Efu},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp,Emp},
                {Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu,Sfu},
                {Emp,Ska,Emp,Emp,Emp,Emp,Emp,Shi,Emp},
                {Sky,Ske,Sgi,Ski,Sou,Ski,Sgi,Ske,Sky}
            }
        };
    }
}
