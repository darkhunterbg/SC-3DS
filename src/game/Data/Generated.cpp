#include "Generated.h"
#include "../Platform.h"

std::vector<const SpriteFrameAtlas*> SpriteDatabase::Units;
SpriteFrameAtlas* SpriteDatabase::unit_terran_marine;
SpriteFrameAtlas* SpriteDatabase::unit_terran_tmashad;

const SpriteFrameAtlas* SpriteDatabase::Load_unit_terran_marine() {
	const SpriteAtlas* atlas = Platform::LoadAtlas("unit_terran_marine.t3x");
	auto* a = unit_terran_marine = new SpriteFrameAtlas(atlas);
	Units.push_back(a);
	a->FrameSize = Vector2Int(64, 64);

	a->SetOffset(0, Vector2Int(-9,-13));
	a->SetOffset(1, Vector2Int(-9,-13));
	a->SetOffset(2, Vector2Int(-9,-14));
	a->SetOffset(3, Vector2Int(-9,-14));
	a->SetOffset(4, Vector2Int(-9,-14));
	a->SetOffset(5, Vector2Int(-9,-14));
	a->SetOffset(6, Vector2Int(-9,-12));
	a->SetOffset(7, Vector2Int(-9,-12));
	a->SetOffset(8, Vector2Int(-8,-12));
	a->SetOffset(9, Vector2Int(-8,-12));
	a->SetOffset(10, Vector2Int(-9,-12));
	a->SetOffset(11, Vector2Int(-9,-12));
	a->SetOffset(12, Vector2Int(-9,-13));
	a->SetOffset(13, Vector2Int(-9,-13));
	a->SetOffset(14, Vector2Int(-10,-13));
	a->SetOffset(15, Vector2Int(-10,-13));
	a->SetOffset(16, Vector2Int(-11,-13));
	a->SetOffset(17, Vector2Int(-8,-14));
	a->SetOffset(18, Vector2Int(-8,-14));
	a->SetOffset(19, Vector2Int(-8,-14));
	a->SetOffset(20, Vector2Int(-8,-14));
	a->SetOffset(21, Vector2Int(-9,-12));
	a->SetOffset(22, Vector2Int(-9,-12));
	a->SetOffset(23, Vector2Int(-8,-12));
	a->SetOffset(24, Vector2Int(-8,-12));
	a->SetOffset(25, Vector2Int(-9,-12));
	a->SetOffset(26, Vector2Int(-9,-12));
	a->SetOffset(27, Vector2Int(-9,-13));
	a->SetOffset(28, Vector2Int(-9,-13));
	a->SetOffset(29, Vector2Int(-9,-13));
	a->SetOffset(30, Vector2Int(-9,-13));
	a->SetOffset(31, Vector2Int(-11,-13));
	a->SetOffset(32, Vector2Int(-11,-13));
	a->SetOffset(33, Vector2Int(-10,-13));
	a->SetOffset(34, Vector2Int(-8,-14));
	a->SetOffset(35, Vector2Int(-8,-14));
	a->SetOffset(36, Vector2Int(-8,-13));
	a->SetOffset(37, Vector2Int(-8,-13));
	a->SetOffset(38, Vector2Int(-8,-11));
	a->SetOffset(39, Vector2Int(-8,-11));
	a->SetOffset(40, Vector2Int(-8,-12));
	a->SetOffset(41, Vector2Int(-8,-12));
	a->SetOffset(42, Vector2Int(-9,-12));
	a->SetOffset(43, Vector2Int(-9,-12));
	a->SetOffset(44, Vector2Int(-9,-13));
	a->SetOffset(45, Vector2Int(-9,-13));
	a->SetOffset(46, Vector2Int(-10,-13));
	a->SetOffset(47, Vector2Int(-10,-13));
	a->SetOffset(48, Vector2Int(-10,-13));
	a->SetOffset(49, Vector2Int(-10,-13));
	a->SetOffset(50, Vector2Int(-8,-13));
	a->SetOffset(51, Vector2Int(-8,-18));
	a->SetOffset(52, Vector2Int(-8,-18));
	a->SetOffset(53, Vector2Int(-8,-18));
	a->SetOffset(54, Vector2Int(-8,-18));
	a->SetOffset(55, Vector2Int(-8,-14));
	a->SetOffset(56, Vector2Int(-8,-14));
	a->SetOffset(57, Vector2Int(-8,-12));
	a->SetOffset(58, Vector2Int(-8,-12));
	a->SetOffset(59, Vector2Int(-9,-12));
	a->SetOffset(60, Vector2Int(-9,-12));
	a->SetOffset(61, Vector2Int(-9,-13));
	a->SetOffset(62, Vector2Int(-9,-13));
	a->SetOffset(63, Vector2Int(-10,-13));
	a->SetOffset(64, Vector2Int(-10,-13));
	a->SetOffset(65, Vector2Int(-10,-13));
	a->SetOffset(66, Vector2Int(-10,-13));
	a->SetOffset(67, Vector2Int(-8,-13));
	a->SetOffset(68, Vector2Int(-11,-11));
	a->SetOffset(69, Vector2Int(-11,-11));
	a->SetOffset(70, Vector2Int(-10,-13));
	a->SetOffset(71, Vector2Int(-10,-13));
	a->SetOffset(72, Vector2Int(-9,-14));
	a->SetOffset(73, Vector2Int(-9,-14));
	a->SetOffset(74, Vector2Int(-9,-14));
	a->SetOffset(75, Vector2Int(-9,-14));
	a->SetOffset(76, Vector2Int(-8,-12));
	a->SetOffset(77, Vector2Int(-8,-12));
	a->SetOffset(78, Vector2Int(-9,-12));
	a->SetOffset(79, Vector2Int(-9,-12));
	a->SetOffset(80, Vector2Int(-9,-13));
	a->SetOffset(81, Vector2Int(-9,-13));
	a->SetOffset(82, Vector2Int(-9,-13));
	a->SetOffset(83, Vector2Int(-9,-13));
	a->SetOffset(84, Vector2Int(-10,-13));
	a->SetOffset(85, Vector2Int(-10,-11));
	a->SetOffset(86, Vector2Int(-10,-11));
	a->SetOffset(87, Vector2Int(-10,-13));
	a->SetOffset(88, Vector2Int(-10,-13));
	a->SetOffset(89, Vector2Int(-9,-13));
	a->SetOffset(90, Vector2Int(-9,-13));
	a->SetOffset(91, Vector2Int(-9,-13));
	a->SetOffset(92, Vector2Int(-9,-13));
	a->SetOffset(93, Vector2Int(-8,-12));
	a->SetOffset(94, Vector2Int(-8,-12));
	a->SetOffset(95, Vector2Int(-9,-13));
	a->SetOffset(96, Vector2Int(-9,-13));
	a->SetOffset(97, Vector2Int(-9,-13));
	a->SetOffset(98, Vector2Int(-9,-13));
	a->SetOffset(99, Vector2Int(-10,-13));
	a->SetOffset(100, Vector2Int(-10,-13));
	a->SetOffset(101, Vector2Int(-11,-13));
	a->SetOffset(102, Vector2Int(-11,-11));
	a->SetOffset(103, Vector2Int(-11,-11));
	a->SetOffset(104, Vector2Int(-9,-13));
	a->SetOffset(105, Vector2Int(-9,-13));
	a->SetOffset(106, Vector2Int(-11,-13));
	a->SetOffset(107, Vector2Int(-11,-13));
	a->SetOffset(108, Vector2Int(-11,-13));
	a->SetOffset(109, Vector2Int(-11,-13));
	a->SetOffset(110, Vector2Int(-11,-13));
	a->SetOffset(111, Vector2Int(-11,-13));
	a->SetOffset(112, Vector2Int(-9,-13));
	a->SetOffset(113, Vector2Int(-9,-13));
	a->SetOffset(114, Vector2Int(-9,-13));
	a->SetOffset(115, Vector2Int(-9,-13));
	a->SetOffset(116, Vector2Int(-10,-13));
	a->SetOffset(117, Vector2Int(-10,-13));
	a->SetOffset(118, Vector2Int(-11,-14));
	a->SetOffset(119, Vector2Int(-11,-11));
	a->SetOffset(120, Vector2Int(-11,-11));
	a->SetOffset(121, Vector2Int(-9,-12));
	a->SetOffset(122, Vector2Int(-9,-12));
	a->SetOffset(123, Vector2Int(-10,-13));
	a->SetOffset(124, Vector2Int(-10,-13));
	a->SetOffset(125, Vector2Int(-10,-13));
	a->SetOffset(126, Vector2Int(-10,-13));
	a->SetOffset(127, Vector2Int(-10,-13));
	a->SetOffset(128, Vector2Int(-10,-13));
	a->SetOffset(129, Vector2Int(-9,-13));
	a->SetOffset(130, Vector2Int(-9,-13));
	a->SetOffset(131, Vector2Int(-9,-13));
	a->SetOffset(132, Vector2Int(-9,-13));
	a->SetOffset(133, Vector2Int(-9,-13));
	a->SetOffset(134, Vector2Int(-9,-13));
	a->SetOffset(135, Vector2Int(-11,-13));
	a->SetOffset(136, Vector2Int(-12,-11));
	a->SetOffset(137, Vector2Int(-12,-11));
	a->SetOffset(138, Vector2Int(-10,-13));
	a->SetOffset(139, Vector2Int(-10,-13));
	a->SetOffset(140, Vector2Int(-9,-14));
	a->SetOffset(141, Vector2Int(-9,-14));
	a->SetOffset(142, Vector2Int(-9,-14));
	a->SetOffset(143, Vector2Int(-9,-14));
	a->SetOffset(144, Vector2Int(-8,-12));
	a->SetOffset(145, Vector2Int(-8,-12));
	a->SetOffset(146, Vector2Int(-9,-12));
	a->SetOffset(147, Vector2Int(-9,-12));
	a->SetOffset(148, Vector2Int(-9,-13));
	a->SetOffset(149, Vector2Int(-9,-13));
	a->SetOffset(150, Vector2Int(-9,-13));
	a->SetOffset(151, Vector2Int(-9,-13));
	a->SetOffset(152, Vector2Int(-11,-13));
	a->SetOffset(153, Vector2Int(-12,-11));
	a->SetOffset(154, Vector2Int(-12,-11));
	a->SetOffset(155, Vector2Int(-10,-13));
	a->SetOffset(156, Vector2Int(-10,-13));
	a->SetOffset(157, Vector2Int(-9,-14));
	a->SetOffset(158, Vector2Int(-9,-14));
	a->SetOffset(159, Vector2Int(-9,-14));
	a->SetOffset(160, Vector2Int(-9,-14));
	a->SetOffset(161, Vector2Int(-8,-13));
	a->SetOffset(162, Vector2Int(-8,-13));
	a->SetOffset(163, Vector2Int(-9,-13));
	a->SetOffset(164, Vector2Int(-9,-13));
	a->SetOffset(165, Vector2Int(-9,-13));
	a->SetOffset(166, Vector2Int(-9,-13));
	a->SetOffset(167, Vector2Int(-9,-13));
	a->SetOffset(168, Vector2Int(-9,-13));
	a->SetOffset(169, Vector2Int(-11,-13));
	a->SetOffset(170, Vector2Int(-13,-11));
	a->SetOffset(171, Vector2Int(-13,-11));
	a->SetOffset(172, Vector2Int(-10,-13));
	a->SetOffset(173, Vector2Int(-10,-13));
	a->SetOffset(174, Vector2Int(-9,-15));
	a->SetOffset(175, Vector2Int(-9,-15));
	a->SetOffset(176, Vector2Int(-9,-15));
	a->SetOffset(177, Vector2Int(-9,-15));
	a->SetOffset(178, Vector2Int(-9,-14));
	a->SetOffset(179, Vector2Int(-9,-14));
	a->SetOffset(180, Vector2Int(-10,-13));
	a->SetOffset(181, Vector2Int(-10,-13));
	a->SetOffset(182, Vector2Int(-10,-13));
	a->SetOffset(183, Vector2Int(-10,-13));
	a->SetOffset(184, Vector2Int(-9,-13));
	a->SetOffset(185, Vector2Int(-9,-13));
	a->SetOffset(186, Vector2Int(-10,-14));
	a->SetOffset(187, Vector2Int(-12,-12));
	a->SetOffset(188, Vector2Int(-12,-12));
	a->SetOffset(189, Vector2Int(-10,-14));
	a->SetOffset(190, Vector2Int(-10,-14));
	a->SetOffset(191, Vector2Int(-9,-15));
	a->SetOffset(192, Vector2Int(-9,-15));
	a->SetOffset(193, Vector2Int(-9,-15));
	a->SetOffset(194, Vector2Int(-9,-15));
	a->SetOffset(195, Vector2Int(-11,-14));
	a->SetOffset(196, Vector2Int(-11,-14));
	a->SetOffset(197, Vector2Int(-12,-13));
	a->SetOffset(198, Vector2Int(-12,-13));
	a->SetOffset(199, Vector2Int(-11,-13));
	a->SetOffset(200, Vector2Int(-11,-13));
	a->SetOffset(201, Vector2Int(-9,-13));
	a->SetOffset(202, Vector2Int(-9,-13));
	a->SetOffset(203, Vector2Int(-10,-13));
	a->SetOffset(204, Vector2Int(-11,-11));
	a->SetOffset(205, Vector2Int(-11,-11));
	a->SetOffset(206, Vector2Int(-10,-13));
	a->SetOffset(207, Vector2Int(-10,-13));
	a->SetOffset(208, Vector2Int(-8,-14));
	a->SetOffset(209, Vector2Int(-8,-14));
	a->SetOffset(210, Vector2Int(-9,-14));
	a->SetOffset(211, Vector2Int(-9,-14));
	a->SetOffset(212, Vector2Int(-11,-12));
	a->SetOffset(213, Vector2Int(-11,-12));
	a->SetOffset(214, Vector2Int(-12,-13));
	a->SetOffset(215, Vector2Int(-12,-13));
	a->SetOffset(216, Vector2Int(-11,-13));
	a->SetOffset(217, Vector2Int(-11,-13));
	a->SetOffset(218, Vector2Int(-9,-13));
	a->SetOffset(219, Vector2Int(-9,-13));
	a->SetOffset(220, Vector2Int(-11,-13));
	a->SetOffset(221, Vector2Int(-10,-13));
	a->SetOffset(222, Vector2Int(-9,-13));
	a->SetOffset(223, Vector2Int(-15,-15));
	a->SetOffset(224, Vector2Int(-20,-18));
	a->SetOffset(225, Vector2Int(-25,-17));
	a->SetOffset(226, Vector2Int(-30,-15));
	a->SetOffset(227, Vector2Int(-32,-13));
	a->SetOffset(228, Vector2Int(-32,-12));

	return a;
}

const SpriteFrameAtlas* SpriteDatabase::Load_unit_terran_tmashad() {
	const SpriteAtlas* atlas = Platform::LoadAtlas("unit_terran_tmashad.t3x");
	auto* a = unit_terran_tmashad = new SpriteFrameAtlas(atlas);
	Units.push_back(a);
	a->FrameSize = Vector2Int(44, 44);

	a->SetOffset(0, Vector2Int(-8,6));
	a->SetOffset(1, Vector2Int(-8,6));
	a->SetOffset(2, Vector2Int(-9,5));
	a->SetOffset(3, Vector2Int(-9,5));
	a->SetOffset(4, Vector2Int(-8,6));
	a->SetOffset(5, Vector2Int(-8,6));
	a->SetOffset(6, Vector2Int(-8,6));
	a->SetOffset(7, Vector2Int(-8,6));
	a->SetOffset(8, Vector2Int(-8,6));
	a->SetOffset(9, Vector2Int(-8,6));
	a->SetOffset(10, Vector2Int(-8,5));
	a->SetOffset(11, Vector2Int(-8,5));
	a->SetOffset(12, Vector2Int(-7,8));
	a->SetOffset(13, Vector2Int(-7,8));
	a->SetOffset(14, Vector2Int(-7,7));
	a->SetOffset(15, Vector2Int(-7,7));
	a->SetOffset(16, Vector2Int(-9,7));
	a->SetOffset(17, Vector2Int(-8,6));
	a->SetOffset(18, Vector2Int(-8,6));
	a->SetOffset(19, Vector2Int(-9,5));
	a->SetOffset(20, Vector2Int(-9,5));
	a->SetOffset(21, Vector2Int(-8,6));
	a->SetOffset(22, Vector2Int(-8,6));
	a->SetOffset(23, Vector2Int(-8,6));
	a->SetOffset(24, Vector2Int(-8,6));
	a->SetOffset(25, Vector2Int(-8,6));
	a->SetOffset(26, Vector2Int(-8,6));
	a->SetOffset(27, Vector2Int(-8,5));
	a->SetOffset(28, Vector2Int(-8,5));
	a->SetOffset(29, Vector2Int(-7,8));
	a->SetOffset(30, Vector2Int(-7,8));
	a->SetOffset(31, Vector2Int(-7,7));
	a->SetOffset(32, Vector2Int(-7,7));
	a->SetOffset(33, Vector2Int(-9,7));
	a->SetOffset(34, Vector2Int(-8,6));
	a->SetOffset(35, Vector2Int(-8,6));
	a->SetOffset(36, Vector2Int(-9,5));
	a->SetOffset(37, Vector2Int(-9,5));
	a->SetOffset(38, Vector2Int(-8,6));
	a->SetOffset(39, Vector2Int(-8,6));
	a->SetOffset(40, Vector2Int(-8,6));
	a->SetOffset(41, Vector2Int(-8,6));
	a->SetOffset(42, Vector2Int(-8,6));
	a->SetOffset(43, Vector2Int(-8,6));
	a->SetOffset(44, Vector2Int(-8,5));
	a->SetOffset(45, Vector2Int(-8,5));
	a->SetOffset(46, Vector2Int(-7,8));
	a->SetOffset(47, Vector2Int(-7,8));
	a->SetOffset(48, Vector2Int(-7,7));
	a->SetOffset(49, Vector2Int(-7,7));
	a->SetOffset(50, Vector2Int(-9,7));
	a->SetOffset(51, Vector2Int(-8,6));
	a->SetOffset(52, Vector2Int(-8,6));
	a->SetOffset(53, Vector2Int(-9,5));
	a->SetOffset(54, Vector2Int(-9,5));
	a->SetOffset(55, Vector2Int(-8,6));
	a->SetOffset(56, Vector2Int(-8,6));
	a->SetOffset(57, Vector2Int(-8,6));
	a->SetOffset(58, Vector2Int(-8,6));
	a->SetOffset(59, Vector2Int(-8,6));
	a->SetOffset(60, Vector2Int(-8,6));
	a->SetOffset(61, Vector2Int(-8,5));
	a->SetOffset(62, Vector2Int(-8,5));
	a->SetOffset(63, Vector2Int(-7,8));
	a->SetOffset(64, Vector2Int(-7,8));
	a->SetOffset(65, Vector2Int(-7,7));
	a->SetOffset(66, Vector2Int(-7,7));
	a->SetOffset(67, Vector2Int(-9,7));
	a->SetOffset(68, Vector2Int(-8,6));
	a->SetOffset(69, Vector2Int(-8,6));
	a->SetOffset(70, Vector2Int(-9,5));
	a->SetOffset(71, Vector2Int(-9,5));
	a->SetOffset(72, Vector2Int(-8,6));
	a->SetOffset(73, Vector2Int(-8,6));
	a->SetOffset(74, Vector2Int(-8,6));
	a->SetOffset(75, Vector2Int(-8,6));
	a->SetOffset(76, Vector2Int(-8,6));
	a->SetOffset(77, Vector2Int(-8,6));
	a->SetOffset(78, Vector2Int(-8,5));
	a->SetOffset(79, Vector2Int(-8,5));
	a->SetOffset(80, Vector2Int(-7,8));
	a->SetOffset(81, Vector2Int(-7,8));
	a->SetOffset(82, Vector2Int(-7,7));
	a->SetOffset(83, Vector2Int(-7,7));
	a->SetOffset(84, Vector2Int(-9,7));
	a->SetOffset(85, Vector2Int(-8,5));
	a->SetOffset(86, Vector2Int(-8,5));
	a->SetOffset(87, Vector2Int(-9,6));
	a->SetOffset(88, Vector2Int(-9,6));
	a->SetOffset(89, Vector2Int(-10,7));
	a->SetOffset(90, Vector2Int(-10,7));
	a->SetOffset(91, Vector2Int(-9,6));
	a->SetOffset(92, Vector2Int(-9,6));
	a->SetOffset(93, Vector2Int(-9,7));
	a->SetOffset(94, Vector2Int(-9,7));
	a->SetOffset(95, Vector2Int(-7,6));
	a->SetOffset(96, Vector2Int(-7,6));
	a->SetOffset(97, Vector2Int(-8,5));
	a->SetOffset(98, Vector2Int(-8,5));
	a->SetOffset(99, Vector2Int(-8,4));
	a->SetOffset(100, Vector2Int(-8,4));
	a->SetOffset(101, Vector2Int(-8,4));
	a->SetOffset(102, Vector2Int(-8,6));
	a->SetOffset(103, Vector2Int(-8,6));
	a->SetOffset(104, Vector2Int(-10,6));
	a->SetOffset(105, Vector2Int(-10,6));
	a->SetOffset(106, Vector2Int(-12,6));
	a->SetOffset(107, Vector2Int(-12,6));
	a->SetOffset(108, Vector2Int(-14,4));
	a->SetOffset(109, Vector2Int(-14,4));
	a->SetOffset(110, Vector2Int(-13,2));
	a->SetOffset(111, Vector2Int(-13,2));
	a->SetOffset(112, Vector2Int(-6,5));
	a->SetOffset(113, Vector2Int(-6,5));
	a->SetOffset(114, Vector2Int(-9,6));
	a->SetOffset(115, Vector2Int(-9,6));
	a->SetOffset(116, Vector2Int(-7,5));
	a->SetOffset(117, Vector2Int(-7,5));
	a->SetOffset(118, Vector2Int(-8,5));
	a->SetOffset(119, Vector2Int(-9,8));
	a->SetOffset(120, Vector2Int(-9,8));
	a->SetOffset(121, Vector2Int(-10,6));
	a->SetOffset(122, Vector2Int(-10,6));
	a->SetOffset(123, Vector2Int(-11,4));
	a->SetOffset(124, Vector2Int(-11,4));
	a->SetOffset(125, Vector2Int(-12,5));
	a->SetOffset(126, Vector2Int(-12,5));
	a->SetOffset(127, Vector2Int(-11,3));
	a->SetOffset(128, Vector2Int(-11,3));
	a->SetOffset(129, Vector2Int(-7,5));
	a->SetOffset(130, Vector2Int(-7,5));
	a->SetOffset(131, Vector2Int(-8,5));
	a->SetOffset(132, Vector2Int(-8,5));
	a->SetOffset(133, Vector2Int(-6,5));
	a->SetOffset(134, Vector2Int(-6,5));
	a->SetOffset(135, Vector2Int(-8,5));
	a->SetOffset(136, Vector2Int(-8,8));
	a->SetOffset(137, Vector2Int(-8,8));
	a->SetOffset(138, Vector2Int(-10,5));
	a->SetOffset(139, Vector2Int(-10,5));
	a->SetOffset(140, Vector2Int(-10,5));
	a->SetOffset(141, Vector2Int(-10,5));
	a->SetOffset(142, Vector2Int(-10,3));
	a->SetOffset(143, Vector2Int(-10,3));
	a->SetOffset(144, Vector2Int(-11,1));
	a->SetOffset(145, Vector2Int(-11,1));
	a->SetOffset(146, Vector2Int(-8,6));
	a->SetOffset(147, Vector2Int(-8,6));
	a->SetOffset(148, Vector2Int(-9,5));
	a->SetOffset(149, Vector2Int(-9,5));
	a->SetOffset(150, Vector2Int(-7,5));
	a->SetOffset(151, Vector2Int(-7,5));
	a->SetOffset(152, Vector2Int(-8,4));
	a->SetOffset(153, Vector2Int(-8,6));
	a->SetOffset(154, Vector2Int(-8,6));
	a->SetOffset(155, Vector2Int(-8,4));
	a->SetOffset(156, Vector2Int(-8,4));
	a->SetOffset(157, Vector2Int(-8,7));
	a->SetOffset(158, Vector2Int(-8,7));
	a->SetOffset(159, Vector2Int(-8,6));
	a->SetOffset(160, Vector2Int(-8,6));
	a->SetOffset(161, Vector2Int(-9,4));
	a->SetOffset(162, Vector2Int(-9,4));
	a->SetOffset(163, Vector2Int(-10,4));
	a->SetOffset(164, Vector2Int(-10,4));
	a->SetOffset(165, Vector2Int(-10,5));
	a->SetOffset(166, Vector2Int(-10,5));
	a->SetOffset(167, Vector2Int(-9,6));
	a->SetOffset(168, Vector2Int(-9,6));
	a->SetOffset(169, Vector2Int(-8,4));
	a->SetOffset(170, Vector2Int(-8,7));
	a->SetOffset(171, Vector2Int(-8,7));
	a->SetOffset(172, Vector2Int(-7,6));
	a->SetOffset(173, Vector2Int(-7,6));
	a->SetOffset(174, Vector2Int(-7,7));
	a->SetOffset(175, Vector2Int(-7,7));
	a->SetOffset(176, Vector2Int(-9,4));
	a->SetOffset(177, Vector2Int(-9,4));
	a->SetOffset(178, Vector2Int(-10,5));
	a->SetOffset(179, Vector2Int(-10,5));
	a->SetOffset(180, Vector2Int(-12,4));
	a->SetOffset(181, Vector2Int(-12,4));
	a->SetOffset(182, Vector2Int(-10,6));
	a->SetOffset(183, Vector2Int(-10,6));
	a->SetOffset(184, Vector2Int(-8,5));
	a->SetOffset(185, Vector2Int(-8,5));
	a->SetOffset(186, Vector2Int(-7,4));
	a->SetOffset(187, Vector2Int(-8,7));
	a->SetOffset(188, Vector2Int(-8,7));
	a->SetOffset(189, Vector2Int(-7,7));
	a->SetOffset(190, Vector2Int(-7,7));
	a->SetOffset(191, Vector2Int(-8,8));
	a->SetOffset(192, Vector2Int(-8,8));
	a->SetOffset(193, Vector2Int(-10,8));
	a->SetOffset(194, Vector2Int(-10,8));
	a->SetOffset(195, Vector2Int(-12,6));
	a->SetOffset(196, Vector2Int(-12,6));
	a->SetOffset(197, Vector2Int(-13,5));
	a->SetOffset(198, Vector2Int(-13,5));
	a->SetOffset(199, Vector2Int(-11,6));
	a->SetOffset(200, Vector2Int(-11,6));
	a->SetOffset(201, Vector2Int(-9,5));
	a->SetOffset(202, Vector2Int(-9,5));
	a->SetOffset(203, Vector2Int(-7,4));
	a->SetOffset(204, Vector2Int(-8,8));
	a->SetOffset(205, Vector2Int(-8,8));
	a->SetOffset(206, Vector2Int(-8,7));
	a->SetOffset(207, Vector2Int(-8,7));
	a->SetOffset(208, Vector2Int(-8,9));
	a->SetOffset(209, Vector2Int(-8,9));
	a->SetOffset(210, Vector2Int(-11,6));
	a->SetOffset(211, Vector2Int(-11,6));
	a->SetOffset(212, Vector2Int(-12,2));
	a->SetOffset(213, Vector2Int(-12,2));
	a->SetOffset(214, Vector2Int(-13,7));
	a->SetOffset(215, Vector2Int(-13,7));
	a->SetOffset(216, Vector2Int(-10,5));
	a->SetOffset(217, Vector2Int(-10,5));
	a->SetOffset(218, Vector2Int(-10,4));
	a->SetOffset(219, Vector2Int(-10,4));
	a->SetOffset(220, Vector2Int(-7,5));

	return a;
}

