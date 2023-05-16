#ifndef VH_PONC_STYLE_TAILWIND_COLORS_H_
#define VH_PONC_STYLE_TAILWIND_COLORS_H_

#include <imgui.h>

#include <array>

#include "style_utils.h"

namespace vh::ponc::style {
///
constexpr auto kTailwindColors = std::array{std::array{
                                                FromRgb(248, 250, 252),
                                                FromRgb(241, 245, 249),
                                                FromRgb(226, 232, 240),
                                                FromRgb(203, 213, 225),
                                                FromRgb(148, 163, 184),
                                                FromRgb(100, 116, 139),
                                                FromRgb(71, 85, 105),
                                                FromRgb(51, 65, 85),
                                                FromRgb(30, 41, 59),
                                                FromRgb(15, 23, 42),
                                                FromRgb(2, 6, 23),
                                            },
                                            std::array{
                                                FromRgb(249, 250, 251),
                                                FromRgb(243, 244, 246),
                                                FromRgb(229, 231, 235),
                                                FromRgb(209, 213, 219),
                                                FromRgb(156, 163, 175),
                                                FromRgb(107, 114, 128),
                                                FromRgb(75, 85, 99),
                                                FromRgb(55, 65, 81),
                                                FromRgb(31, 41, 55),
                                                FromRgb(17, 24, 39),
                                                FromRgb(3, 7, 18),
                                            },
                                            std::array{
                                                FromRgb(250, 250, 250),
                                                FromRgb(244, 244, 245),
                                                FromRgb(228, 228, 231),
                                                FromRgb(212, 212, 216),
                                                FromRgb(161, 161, 170),
                                                FromRgb(113, 113, 122),
                                                FromRgb(82, 82, 91),
                                                FromRgb(63, 63, 70),
                                                FromRgb(39, 39, 42),
                                                FromRgb(24, 24, 27),
                                                FromRgb(9, 9, 11),
                                            },
                                            std::array{
                                                FromRgb(250, 250, 250),
                                                FromRgb(245, 245, 245),
                                                FromRgb(229, 229, 229),
                                                FromRgb(212, 212, 212),
                                                FromRgb(163, 163, 163),
                                                FromRgb(115, 115, 115),
                                                FromRgb(82, 82, 82),
                                                FromRgb(64, 64, 64),
                                                FromRgb(38, 38, 38),
                                                FromRgb(23, 23, 23),
                                                FromRgb(10, 10, 10),
                                            },
                                            std::array{
                                                FromRgb(250, 250, 249),
                                                FromRgb(245, 245, 244),
                                                FromRgb(231, 229, 228),
                                                FromRgb(214, 211, 209),
                                                FromRgb(168, 162, 158),
                                                FromRgb(120, 113, 108),
                                                FromRgb(87, 83, 78),
                                                FromRgb(68, 64, 60),
                                                FromRgb(41, 37, 36),
                                                FromRgb(28, 25, 23),
                                                FromRgb(12, 10, 9),
                                            },
                                            std::array{
                                                FromRgb(254, 242, 242),
                                                FromRgb(254, 226, 226),
                                                FromRgb(254, 202, 202),
                                                FromRgb(252, 165, 165),
                                                FromRgb(248, 113, 113),
                                                FromRgb(239, 68, 68),
                                                FromRgb(220, 38, 38),
                                                FromRgb(185, 28, 28),
                                                FromRgb(153, 27, 27),
                                                FromRgb(127, 29, 29),
                                                FromRgb(69, 10, 10),
                                            },
                                            std::array{
                                                FromRgb(255, 247, 237),
                                                FromRgb(255, 237, 213),
                                                FromRgb(254, 215, 170),
                                                FromRgb(253, 186, 116),
                                                FromRgb(251, 146, 60),
                                                FromRgb(249, 115, 22),
                                                FromRgb(234, 88, 12),
                                                FromRgb(194, 65, 12),
                                                FromRgb(154, 52, 18),
                                                FromRgb(124, 45, 18),
                                                FromRgb(67, 20, 7),
                                            },
                                            std::array{
                                                FromRgb(255, 251, 235),
                                                FromRgb(254, 243, 199),
                                                FromRgb(253, 230, 138),
                                                FromRgb(252, 211, 77),
                                                FromRgb(251, 191, 36),
                                                FromRgb(245, 158, 11),
                                                FromRgb(217, 119, 6),
                                                FromRgb(180, 83, 9),
                                                FromRgb(146, 64, 14),
                                                FromRgb(120, 53, 15),
                                                FromRgb(69, 26, 3),
                                            },
                                            std::array{
                                                FromRgb(254, 252, 232),
                                                FromRgb(254, 249, 195),
                                                FromRgb(254, 240, 138),
                                                FromRgb(253, 224, 71),
                                                FromRgb(250, 204, 21),
                                                FromRgb(234, 179, 8),
                                                FromRgb(202, 138, 4),
                                                FromRgb(161, 98, 7),
                                                FromRgb(133, 77, 14),
                                                FromRgb(113, 63, 18),
                                                FromRgb(66, 32, 6),
                                            },
                                            std::array{
                                                FromRgb(247, 254, 231),
                                                FromRgb(236, 252, 203),
                                                FromRgb(217, 249, 157),
                                                FromRgb(190, 242, 100),
                                                FromRgb(163, 230, 53),
                                                FromRgb(132, 204, 22),
                                                FromRgb(101, 163, 13),
                                                FromRgb(77, 124, 15),
                                                FromRgb(63, 98, 18),
                                                FromRgb(54, 83, 20),
                                                FromRgb(26, 46, 5),
                                            },
                                            std::array{
                                                FromRgb(240, 253, 244),
                                                FromRgb(220, 252, 231),
                                                FromRgb(187, 247, 208),
                                                FromRgb(134, 239, 172),
                                                FromRgb(74, 222, 128),
                                                FromRgb(34, 197, 94),
                                                FromRgb(22, 163, 74),
                                                FromRgb(21, 128, 61),
                                                FromRgb(22, 101, 52),
                                                FromRgb(20, 83, 45),
                                                FromRgb(5, 46, 22),
                                            },
                                            std::array{
                                                FromRgb(236, 253, 245),
                                                FromRgb(209, 250, 229),
                                                FromRgb(167, 243, 208),
                                                FromRgb(110, 231, 183),
                                                FromRgb(52, 211, 153),
                                                FromRgb(16, 185, 129),
                                                FromRgb(5, 150, 105),
                                                FromRgb(4, 120, 87),
                                                FromRgb(6, 95, 70),
                                                FromRgb(6, 78, 59),
                                                FromRgb(2, 44, 34),
                                            },
                                            std::array{
                                                FromRgb(240, 253, 250),
                                                FromRgb(204, 251, 241),
                                                FromRgb(153, 246, 228),
                                                FromRgb(94, 234, 212),
                                                FromRgb(45, 212, 191),
                                                FromRgb(20, 184, 166),
                                                FromRgb(13, 148, 136),
                                                FromRgb(15, 118, 110),
                                                FromRgb(17, 94, 89),
                                                FromRgb(19, 78, 74),
                                                FromRgb(4, 47, 46),
                                            },
                                            std::array{
                                                FromRgb(236, 254, 255),
                                                FromRgb(207, 250, 254),
                                                FromRgb(165, 243, 252),
                                                FromRgb(103, 232, 249),
                                                FromRgb(34, 211, 238),
                                                FromRgb(6, 182, 212),
                                                FromRgb(8, 145, 178),
                                                FromRgb(14, 116, 144),
                                                FromRgb(21, 94, 117),
                                                FromRgb(22, 78, 99),
                                                FromRgb(8, 51, 68),
                                            },
                                            std::array{
                                                FromRgb(240, 249, 255),
                                                FromRgb(224, 242, 254),
                                                FromRgb(186, 230, 253),
                                                FromRgb(125, 211, 252),
                                                FromRgb(56, 189, 248),
                                                FromRgb(14, 165, 233),
                                                FromRgb(2, 132, 199),
                                                FromRgb(3, 105, 161),
                                                FromRgb(7, 89, 133),
                                                FromRgb(12, 74, 110),
                                                FromRgb(8, 47, 73),
                                            },
                                            std::array{
                                                FromRgb(239, 246, 255),
                                                FromRgb(219, 234, 254),
                                                FromRgb(191, 219, 254),
                                                FromRgb(147, 197, 253),
                                                FromRgb(96, 165, 250),
                                                FromRgb(59, 130, 246),
                                                FromRgb(37, 99, 235),
                                                FromRgb(29, 78, 216),
                                                FromRgb(30, 64, 175),
                                                FromRgb(30, 58, 138),
                                                FromRgb(23, 37, 84),
                                            },
                                            std::array{
                                                FromRgb(238, 242, 255),
                                                FromRgb(224, 231, 255),
                                                FromRgb(199, 210, 254),
                                                FromRgb(165, 180, 252),
                                                FromRgb(129, 140, 248),
                                                FromRgb(99, 102, 241),
                                                FromRgb(79, 70, 229),
                                                FromRgb(67, 56, 202),
                                                FromRgb(55, 48, 163),
                                                FromRgb(49, 46, 129),
                                                FromRgb(30, 27, 75),
                                            },
                                            std::array{
                                                FromRgb(245, 243, 255),
                                                FromRgb(237, 233, 254),
                                                FromRgb(221, 214, 254),
                                                FromRgb(196, 181, 253),
                                                FromRgb(167, 139, 250),
                                                FromRgb(139, 92, 246),
                                                FromRgb(124, 58, 237),
                                                FromRgb(109, 40, 217),
                                                FromRgb(91, 33, 182),
                                                FromRgb(76, 29, 149),
                                                FromRgb(46, 16, 101),
                                            },
                                            std::array{
                                                FromRgb(250, 245, 255),
                                                FromRgb(243, 232, 255),
                                                FromRgb(233, 213, 255),
                                                FromRgb(216, 180, 254),
                                                FromRgb(192, 132, 252),
                                                FromRgb(168, 85, 247),
                                                FromRgb(147, 51, 234),
                                                FromRgb(126, 34, 206),
                                                FromRgb(107, 33, 168),
                                                FromRgb(88, 28, 135),
                                                FromRgb(59, 7, 100),
                                            },
                                            std::array{
                                                FromRgb(253, 244, 255),
                                                FromRgb(250, 232, 255),
                                                FromRgb(245, 208, 254),
                                                FromRgb(240, 171, 252),
                                                FromRgb(232, 121, 249),
                                                FromRgb(217, 70, 239),
                                                FromRgb(192, 38, 211),
                                                FromRgb(162, 28, 175),
                                                FromRgb(134, 25, 143),
                                                FromRgb(112, 26, 117),
                                                FromRgb(74, 4, 78),
                                            },
                                            std::array{
                                                FromRgb(253, 242, 248),
                                                FromRgb(252, 231, 243),
                                                FromRgb(251, 207, 232),
                                                FromRgb(249, 168, 212),
                                                FromRgb(244, 114, 182),
                                                FromRgb(236, 72, 153),
                                                FromRgb(219, 39, 119),
                                                FromRgb(190, 24, 93),
                                                FromRgb(157, 23, 77),
                                                FromRgb(131, 24, 67),
                                                FromRgb(80, 7, 36),
                                            },
                                            std::array{
                                                FromRgb(255, 241, 242),
                                                FromRgb(255, 228, 230),
                                                FromRgb(254, 205, 211),
                                                FromRgb(253, 164, 175),
                                                FromRgb(251, 113, 133),
                                                FromRgb(244, 63, 94),
                                                FromRgb(225, 29, 72),
                                                FromRgb(190, 18, 60),
                                                FromRgb(159, 18, 57),
                                                FromRgb(136, 19, 55),
                                                FromRgb(76, 5, 25),
                                            }};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_TAILWIND_COLORS_H_