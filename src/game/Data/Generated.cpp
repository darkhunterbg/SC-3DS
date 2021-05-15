#include "Generated.h"
#include "../Game.h"


SpriteFrameAtlas* SpriteDatabase::unit_terran_marine;
SpriteFrameAtlas* SpriteDatabase::unit_terran_tmashad;
SpriteFrameAtlas* SpriteDatabase::unit_terran_scv;
SpriteFrameAtlas* SpriteDatabase::unit_thingy_tbangs;

const SpriteFrameAtlas* SpriteDatabase::Load_unit_terran_marine() {
	const SpriteAtlas* atlas = Game::AssetLoader.LoadAtlas("unit_terran_marine.t3x");
	auto* a = unit_terran_marine = new SpriteFrameAtlas(atlas);
	a->FrameSize = Vector2Int(64, 64);

	a->SetOffset(0, Vector2Int(23,19));
	a->SetOffset(1, Vector2Int(23,19));
	a->SetOffset(2, Vector2Int(23,18));
	a->SetOffset(3, Vector2Int(23,18));
	a->SetOffset(4, Vector2Int(23,18));
	a->SetOffset(5, Vector2Int(23,18));
	a->SetOffset(6, Vector2Int(23,20));
	a->SetOffset(7, Vector2Int(23,20));
	a->SetOffset(8, Vector2Int(24,20));
	a->SetOffset(9, Vector2Int(24,20));
	a->SetOffset(10, Vector2Int(23,20));
	a->SetOffset(11, Vector2Int(23,20));
	a->SetOffset(12, Vector2Int(23,19));
	a->SetOffset(13, Vector2Int(23,19));
	a->SetOffset(14, Vector2Int(22,19));
	a->SetOffset(15, Vector2Int(22,19));
	a->SetOffset(16, Vector2Int(21,19));
	a->SetOffset(17, Vector2Int(24,18));
	a->SetOffset(18, Vector2Int(24,18));
	a->SetOffset(19, Vector2Int(24,18));
	a->SetOffset(20, Vector2Int(24,18));
	a->SetOffset(21, Vector2Int(23,20));
	a->SetOffset(22, Vector2Int(23,20));
	a->SetOffset(23, Vector2Int(24,20));
	a->SetOffset(24, Vector2Int(24,20));
	a->SetOffset(25, Vector2Int(23,20));
	a->SetOffset(26, Vector2Int(23,20));
	a->SetOffset(27, Vector2Int(23,19));
	a->SetOffset(28, Vector2Int(23,19));
	a->SetOffset(29, Vector2Int(23,19));
	a->SetOffset(30, Vector2Int(23,19));
	a->SetOffset(31, Vector2Int(21,19));
	a->SetOffset(32, Vector2Int(21,19));
	a->SetOffset(33, Vector2Int(22,19));
	a->SetOffset(34, Vector2Int(24,18));
	a->SetOffset(35, Vector2Int(24,18));
	a->SetOffset(36, Vector2Int(24,19));
	a->SetOffset(37, Vector2Int(24,19));
	a->SetOffset(38, Vector2Int(24,21));
	a->SetOffset(39, Vector2Int(24,21));
	a->SetOffset(40, Vector2Int(24,20));
	a->SetOffset(41, Vector2Int(24,20));
	a->SetOffset(42, Vector2Int(23,20));
	a->SetOffset(43, Vector2Int(23,20));
	a->SetOffset(44, Vector2Int(23,19));
	a->SetOffset(45, Vector2Int(23,19));
	a->SetOffset(46, Vector2Int(22,19));
	a->SetOffset(47, Vector2Int(22,19));
	a->SetOffset(48, Vector2Int(22,19));
	a->SetOffset(49, Vector2Int(22,19));
	a->SetOffset(50, Vector2Int(24,19));
	a->SetOffset(51, Vector2Int(24,14));
	a->SetOffset(52, Vector2Int(24,14));
	a->SetOffset(53, Vector2Int(24,14));
	a->SetOffset(54, Vector2Int(24,14));
	a->SetOffset(55, Vector2Int(24,18));
	a->SetOffset(56, Vector2Int(24,18));
	a->SetOffset(57, Vector2Int(24,20));
	a->SetOffset(58, Vector2Int(24,20));
	a->SetOffset(59, Vector2Int(23,20));
	a->SetOffset(60, Vector2Int(23,20));
	a->SetOffset(61, Vector2Int(23,19));
	a->SetOffset(62, Vector2Int(23,19));
	a->SetOffset(63, Vector2Int(22,19));
	a->SetOffset(64, Vector2Int(22,19));
	a->SetOffset(65, Vector2Int(22,19));
	a->SetOffset(66, Vector2Int(22,19));
	a->SetOffset(67, Vector2Int(24,19));
	a->SetOffset(68, Vector2Int(21,21));
	a->SetOffset(69, Vector2Int(21,21));
	a->SetOffset(70, Vector2Int(22,19));
	a->SetOffset(71, Vector2Int(22,19));
	a->SetOffset(72, Vector2Int(23,18));
	a->SetOffset(73, Vector2Int(23,18));
	a->SetOffset(74, Vector2Int(23,18));
	a->SetOffset(75, Vector2Int(23,18));
	a->SetOffset(76, Vector2Int(24,20));
	a->SetOffset(77, Vector2Int(24,20));
	a->SetOffset(78, Vector2Int(23,20));
	a->SetOffset(79, Vector2Int(23,20));
	a->SetOffset(80, Vector2Int(23,19));
	a->SetOffset(81, Vector2Int(23,19));
	a->SetOffset(82, Vector2Int(23,19));
	a->SetOffset(83, Vector2Int(23,19));
	a->SetOffset(84, Vector2Int(22,19));
	a->SetOffset(85, Vector2Int(22,21));
	a->SetOffset(86, Vector2Int(22,21));
	a->SetOffset(87, Vector2Int(22,19));
	a->SetOffset(88, Vector2Int(22,19));
	a->SetOffset(89, Vector2Int(23,19));
	a->SetOffset(90, Vector2Int(23,19));
	a->SetOffset(91, Vector2Int(23,19));
	a->SetOffset(92, Vector2Int(23,19));
	a->SetOffset(93, Vector2Int(24,20));
	a->SetOffset(94, Vector2Int(24,20));
	a->SetOffset(95, Vector2Int(23,19));
	a->SetOffset(96, Vector2Int(23,19));
	a->SetOffset(97, Vector2Int(23,19));
	a->SetOffset(98, Vector2Int(23,19));
	a->SetOffset(99, Vector2Int(22,19));
	a->SetOffset(100, Vector2Int(22,19));
	a->SetOffset(101, Vector2Int(21,19));
	a->SetOffset(102, Vector2Int(21,21));
	a->SetOffset(103, Vector2Int(21,21));
	a->SetOffset(104, Vector2Int(23,19));
	a->SetOffset(105, Vector2Int(23,19));
	a->SetOffset(106, Vector2Int(21,19));
	a->SetOffset(107, Vector2Int(21,19));
	a->SetOffset(108, Vector2Int(21,19));
	a->SetOffset(109, Vector2Int(21,19));
	a->SetOffset(110, Vector2Int(21,19));
	a->SetOffset(111, Vector2Int(21,19));
	a->SetOffset(112, Vector2Int(23,19));
	a->SetOffset(113, Vector2Int(23,19));
	a->SetOffset(114, Vector2Int(23,19));
	a->SetOffset(115, Vector2Int(23,19));
	a->SetOffset(116, Vector2Int(22,19));
	a->SetOffset(117, Vector2Int(22,19));
	a->SetOffset(118, Vector2Int(21,18));
	a->SetOffset(119, Vector2Int(21,21));
	a->SetOffset(120, Vector2Int(21,21));
	a->SetOffset(121, Vector2Int(23,20));
	a->SetOffset(122, Vector2Int(23,20));
	a->SetOffset(123, Vector2Int(22,19));
	a->SetOffset(124, Vector2Int(22,19));
	a->SetOffset(125, Vector2Int(22,19));
	a->SetOffset(126, Vector2Int(22,19));
	a->SetOffset(127, Vector2Int(22,19));
	a->SetOffset(128, Vector2Int(22,19));
	a->SetOffset(129, Vector2Int(23,19));
	a->SetOffset(130, Vector2Int(23,19));
	a->SetOffset(131, Vector2Int(23,19));
	a->SetOffset(132, Vector2Int(23,19));
	a->SetOffset(133, Vector2Int(23,19));
	a->SetOffset(134, Vector2Int(23,19));
	a->SetOffset(135, Vector2Int(21,19));
	a->SetOffset(136, Vector2Int(20,21));
	a->SetOffset(137, Vector2Int(20,21));
	a->SetOffset(138, Vector2Int(22,19));
	a->SetOffset(139, Vector2Int(22,19));
	a->SetOffset(140, Vector2Int(23,18));
	a->SetOffset(141, Vector2Int(23,18));
	a->SetOffset(142, Vector2Int(23,18));
	a->SetOffset(143, Vector2Int(23,18));
	a->SetOffset(144, Vector2Int(24,20));
	a->SetOffset(145, Vector2Int(24,20));
	a->SetOffset(146, Vector2Int(23,20));
	a->SetOffset(147, Vector2Int(23,20));
	a->SetOffset(148, Vector2Int(23,19));
	a->SetOffset(149, Vector2Int(23,19));
	a->SetOffset(150, Vector2Int(23,19));
	a->SetOffset(151, Vector2Int(23,19));
	a->SetOffset(152, Vector2Int(21,19));
	a->SetOffset(153, Vector2Int(20,21));
	a->SetOffset(154, Vector2Int(20,21));
	a->SetOffset(155, Vector2Int(22,19));
	a->SetOffset(156, Vector2Int(22,19));
	a->SetOffset(157, Vector2Int(23,18));
	a->SetOffset(158, Vector2Int(23,18));
	a->SetOffset(159, Vector2Int(23,18));
	a->SetOffset(160, Vector2Int(23,18));
	a->SetOffset(161, Vector2Int(24,19));
	a->SetOffset(162, Vector2Int(24,19));
	a->SetOffset(163, Vector2Int(23,19));
	a->SetOffset(164, Vector2Int(23,19));
	a->SetOffset(165, Vector2Int(23,19));
	a->SetOffset(166, Vector2Int(23,19));
	a->SetOffset(167, Vector2Int(23,19));
	a->SetOffset(168, Vector2Int(23,19));
	a->SetOffset(169, Vector2Int(21,19));
	a->SetOffset(170, Vector2Int(19,21));
	a->SetOffset(171, Vector2Int(19,21));
	a->SetOffset(172, Vector2Int(22,19));
	a->SetOffset(173, Vector2Int(22,19));
	a->SetOffset(174, Vector2Int(23,17));
	a->SetOffset(175, Vector2Int(23,17));
	a->SetOffset(176, Vector2Int(23,17));
	a->SetOffset(177, Vector2Int(23,17));
	a->SetOffset(178, Vector2Int(23,18));
	a->SetOffset(179, Vector2Int(23,18));
	a->SetOffset(180, Vector2Int(22,19));
	a->SetOffset(181, Vector2Int(22,19));
	a->SetOffset(182, Vector2Int(22,19));
	a->SetOffset(183, Vector2Int(22,19));
	a->SetOffset(184, Vector2Int(23,19));
	a->SetOffset(185, Vector2Int(23,19));
	a->SetOffset(186, Vector2Int(22,18));
	a->SetOffset(187, Vector2Int(20,20));
	a->SetOffset(188, Vector2Int(20,20));
	a->SetOffset(189, Vector2Int(22,18));
	a->SetOffset(190, Vector2Int(22,18));
	a->SetOffset(191, Vector2Int(23,17));
	a->SetOffset(192, Vector2Int(23,17));
	a->SetOffset(193, Vector2Int(23,17));
	a->SetOffset(194, Vector2Int(23,17));
	a->SetOffset(195, Vector2Int(21,18));
	a->SetOffset(196, Vector2Int(21,18));
	a->SetOffset(197, Vector2Int(20,19));
	a->SetOffset(198, Vector2Int(20,19));
	a->SetOffset(199, Vector2Int(21,19));
	a->SetOffset(200, Vector2Int(21,19));
	a->SetOffset(201, Vector2Int(23,19));
	a->SetOffset(202, Vector2Int(23,19));
	a->SetOffset(203, Vector2Int(22,19));
	a->SetOffset(204, Vector2Int(21,21));
	a->SetOffset(205, Vector2Int(21,21));
	a->SetOffset(206, Vector2Int(22,19));
	a->SetOffset(207, Vector2Int(22,19));
	a->SetOffset(208, Vector2Int(24,18));
	a->SetOffset(209, Vector2Int(24,18));
	a->SetOffset(210, Vector2Int(23,18));
	a->SetOffset(211, Vector2Int(23,18));
	a->SetOffset(212, Vector2Int(21,20));
	a->SetOffset(213, Vector2Int(21,20));
	a->SetOffset(214, Vector2Int(20,19));
	a->SetOffset(215, Vector2Int(20,19));
	a->SetOffset(216, Vector2Int(21,19));
	a->SetOffset(217, Vector2Int(21,19));
	a->SetOffset(218, Vector2Int(23,19));
	a->SetOffset(219, Vector2Int(23,19));
	a->SetOffset(220, Vector2Int(21,19));
	a->SetOffset(221, Vector2Int(22,19));
	a->SetOffset(222, Vector2Int(23,19));
	a->SetOffset(223, Vector2Int(17,17));
	a->SetOffset(224, Vector2Int(12,14));
	a->SetOffset(225, Vector2Int(7,15));
	a->SetOffset(226, Vector2Int(2,17));
	a->SetOffset(227, Vector2Int(0,19));
	a->SetOffset(228, Vector2Int(0,20));
	a->SetOffset(229, Vector2Int(23,19));
	a->SetOffset(230, Vector2Int(23,19));
	a->SetOffset(231, Vector2Int(23,18));
	a->SetOffset(232, Vector2Int(23,18));
	a->SetOffset(233, Vector2Int(23,18));
	a->SetOffset(234, Vector2Int(23,18));
	a->SetOffset(235, Vector2Int(23,20));
	a->SetOffset(236, Vector2Int(23,20));
	a->SetOffset(237, Vector2Int(24,20));
	a->SetOffset(238, Vector2Int(24,20));
	a->SetOffset(239, Vector2Int(23,20));
	a->SetOffset(240, Vector2Int(23,20));
	a->SetOffset(241, Vector2Int(23,19));
	a->SetOffset(242, Vector2Int(23,19));
	a->SetOffset(243, Vector2Int(22,19));
	a->SetOffset(244, Vector2Int(22,19));
	a->SetOffset(245, Vector2Int(21,19));
	a->SetOffset(246, Vector2Int(24,18));
	a->SetOffset(247, Vector2Int(24,18));
	a->SetOffset(248, Vector2Int(24,18));
	a->SetOffset(249, Vector2Int(24,18));
	a->SetOffset(250, Vector2Int(23,20));
	a->SetOffset(251, Vector2Int(23,20));
	a->SetOffset(252, Vector2Int(24,20));
	a->SetOffset(253, Vector2Int(24,20));
	a->SetOffset(254, Vector2Int(23,20));
	a->SetOffset(255, Vector2Int(23,20));
	a->SetOffset(256, Vector2Int(23,19));
	a->SetOffset(257, Vector2Int(23,19));
	a->SetOffset(258, Vector2Int(23,19));
	a->SetOffset(259, Vector2Int(23,19));
	a->SetOffset(260, Vector2Int(21,19));
	a->SetOffset(261, Vector2Int(21,19));
	a->SetOffset(262, Vector2Int(22,19));
	a->SetOffset(263, Vector2Int(24,18));
	a->SetOffset(264, Vector2Int(24,18));
	a->SetOffset(265, Vector2Int(24,19));
	a->SetOffset(266, Vector2Int(24,19));
	a->SetOffset(267, Vector2Int(24,21));
	a->SetOffset(268, Vector2Int(24,21));
	a->SetOffset(269, Vector2Int(24,20));
	a->SetOffset(270, Vector2Int(24,20));
	a->SetOffset(271, Vector2Int(23,20));
	a->SetOffset(272, Vector2Int(23,20));
	a->SetOffset(273, Vector2Int(23,19));
	a->SetOffset(274, Vector2Int(23,19));
	a->SetOffset(275, Vector2Int(22,19));
	a->SetOffset(276, Vector2Int(22,19));
	a->SetOffset(277, Vector2Int(22,19));
	a->SetOffset(278, Vector2Int(22,19));
	a->SetOffset(279, Vector2Int(24,19));
	a->SetOffset(280, Vector2Int(24,14));
	a->SetOffset(281, Vector2Int(24,14));
	a->SetOffset(282, Vector2Int(24,14));
	a->SetOffset(283, Vector2Int(24,14));
	a->SetOffset(284, Vector2Int(24,18));
	a->SetOffset(285, Vector2Int(24,18));
	a->SetOffset(286, Vector2Int(24,20));
	a->SetOffset(287, Vector2Int(24,20));
	a->SetOffset(288, Vector2Int(23,20));
	a->SetOffset(289, Vector2Int(23,20));
	a->SetOffset(290, Vector2Int(23,19));
	a->SetOffset(291, Vector2Int(23,19));
	a->SetOffset(292, Vector2Int(22,19));
	a->SetOffset(293, Vector2Int(22,19));
	a->SetOffset(294, Vector2Int(22,19));
	a->SetOffset(295, Vector2Int(22,19));
	a->SetOffset(296, Vector2Int(24,19));
	a->SetOffset(297, Vector2Int(21,21));
	a->SetOffset(298, Vector2Int(21,21));
	a->SetOffset(299, Vector2Int(22,19));
	a->SetOffset(300, Vector2Int(22,19));
	a->SetOffset(301, Vector2Int(23,18));
	a->SetOffset(302, Vector2Int(23,18));
	a->SetOffset(303, Vector2Int(23,18));
	a->SetOffset(304, Vector2Int(23,18));
	a->SetOffset(305, Vector2Int(24,20));
	a->SetOffset(306, Vector2Int(24,20));
	a->SetOffset(307, Vector2Int(23,20));
	a->SetOffset(308, Vector2Int(23,20));
	a->SetOffset(309, Vector2Int(23,19));
	a->SetOffset(310, Vector2Int(23,19));
	a->SetOffset(311, Vector2Int(23,19));
	a->SetOffset(312, Vector2Int(23,19));
	a->SetOffset(313, Vector2Int(22,19));
	a->SetOffset(314, Vector2Int(22,21));
	a->SetOffset(315, Vector2Int(22,21));
	a->SetOffset(316, Vector2Int(22,19));
	a->SetOffset(317, Vector2Int(22,19));
	a->SetOffset(318, Vector2Int(23,19));
	a->SetOffset(319, Vector2Int(23,19));
	a->SetOffset(320, Vector2Int(23,19));
	a->SetOffset(321, Vector2Int(23,19));
	a->SetOffset(322, Vector2Int(24,20));
	a->SetOffset(323, Vector2Int(24,20));
	a->SetOffset(324, Vector2Int(23,19));
	a->SetOffset(325, Vector2Int(23,19));
	a->SetOffset(326, Vector2Int(23,19));
	a->SetOffset(327, Vector2Int(23,19));
	a->SetOffset(328, Vector2Int(22,19));
	a->SetOffset(329, Vector2Int(22,19));
	a->SetOffset(330, Vector2Int(21,19));
	a->SetOffset(331, Vector2Int(21,21));
	a->SetOffset(332, Vector2Int(21,21));
	a->SetOffset(333, Vector2Int(23,19));
	a->SetOffset(334, Vector2Int(23,19));
	a->SetOffset(335, Vector2Int(21,19));
	a->SetOffset(336, Vector2Int(21,19));
	a->SetOffset(337, Vector2Int(21,19));
	a->SetOffset(338, Vector2Int(21,19));
	a->SetOffset(339, Vector2Int(21,19));
	a->SetOffset(340, Vector2Int(21,19));
	a->SetOffset(341, Vector2Int(23,19));
	a->SetOffset(342, Vector2Int(23,19));
	a->SetOffset(343, Vector2Int(23,19));
	a->SetOffset(344, Vector2Int(23,19));
	a->SetOffset(345, Vector2Int(22,19));
	a->SetOffset(346, Vector2Int(22,19));
	a->SetOffset(347, Vector2Int(21,18));
	a->SetOffset(348, Vector2Int(21,21));
	a->SetOffset(349, Vector2Int(21,21));
	a->SetOffset(350, Vector2Int(23,20));
	a->SetOffset(351, Vector2Int(23,20));
	a->SetOffset(352, Vector2Int(22,19));
	a->SetOffset(353, Vector2Int(22,19));
	a->SetOffset(354, Vector2Int(22,19));
	a->SetOffset(355, Vector2Int(22,19));
	a->SetOffset(356, Vector2Int(22,19));
	a->SetOffset(357, Vector2Int(22,19));
	a->SetOffset(358, Vector2Int(23,19));
	a->SetOffset(359, Vector2Int(23,19));
	a->SetOffset(360, Vector2Int(23,19));
	a->SetOffset(361, Vector2Int(23,19));
	a->SetOffset(362, Vector2Int(23,19));
	a->SetOffset(363, Vector2Int(23,19));
	a->SetOffset(364, Vector2Int(21,19));
	a->SetOffset(365, Vector2Int(20,21));
	a->SetOffset(366, Vector2Int(20,21));
	a->SetOffset(367, Vector2Int(22,19));
	a->SetOffset(368, Vector2Int(22,19));
	a->SetOffset(369, Vector2Int(23,18));
	a->SetOffset(370, Vector2Int(23,18));
	a->SetOffset(371, Vector2Int(23,18));
	a->SetOffset(372, Vector2Int(23,18));
	a->SetOffset(373, Vector2Int(24,20));
	a->SetOffset(374, Vector2Int(24,20));
	a->SetOffset(375, Vector2Int(23,20));
	a->SetOffset(376, Vector2Int(23,20));
	a->SetOffset(377, Vector2Int(23,19));
	a->SetOffset(378, Vector2Int(23,19));
	a->SetOffset(379, Vector2Int(23,19));
	a->SetOffset(380, Vector2Int(23,19));
	a->SetOffset(381, Vector2Int(21,19));
	a->SetOffset(382, Vector2Int(20,21));
	a->SetOffset(383, Vector2Int(20,21));
	a->SetOffset(384, Vector2Int(22,19));
	a->SetOffset(385, Vector2Int(22,19));
	a->SetOffset(386, Vector2Int(23,18));
	a->SetOffset(387, Vector2Int(23,18));
	a->SetOffset(388, Vector2Int(23,18));
	a->SetOffset(389, Vector2Int(23,18));
	a->SetOffset(390, Vector2Int(24,19));
	a->SetOffset(391, Vector2Int(24,19));
	a->SetOffset(392, Vector2Int(23,19));
	a->SetOffset(393, Vector2Int(23,19));
	a->SetOffset(394, Vector2Int(23,19));
	a->SetOffset(395, Vector2Int(23,19));
	a->SetOffset(396, Vector2Int(23,19));
	a->SetOffset(397, Vector2Int(23,19));
	a->SetOffset(398, Vector2Int(21,19));
	a->SetOffset(399, Vector2Int(19,21));
	a->SetOffset(400, Vector2Int(19,21));
	a->SetOffset(401, Vector2Int(22,19));
	a->SetOffset(402, Vector2Int(22,19));
	a->SetOffset(403, Vector2Int(23,17));
	a->SetOffset(404, Vector2Int(23,17));
	a->SetOffset(405, Vector2Int(23,17));
	a->SetOffset(406, Vector2Int(23,17));
	a->SetOffset(407, Vector2Int(23,18));
	a->SetOffset(408, Vector2Int(23,18));
	a->SetOffset(409, Vector2Int(22,19));
	a->SetOffset(410, Vector2Int(22,19));
	a->SetOffset(411, Vector2Int(22,19));
	a->SetOffset(412, Vector2Int(22,19));
	a->SetOffset(413, Vector2Int(23,19));
	a->SetOffset(414, Vector2Int(23,19));
	a->SetOffset(415, Vector2Int(22,18));
	a->SetOffset(416, Vector2Int(20,20));
	a->SetOffset(417, Vector2Int(20,20));
	a->SetOffset(418, Vector2Int(22,18));
	a->SetOffset(419, Vector2Int(22,18));
	a->SetOffset(420, Vector2Int(23,17));
	a->SetOffset(421, Vector2Int(23,17));
	a->SetOffset(422, Vector2Int(23,17));
	a->SetOffset(423, Vector2Int(23,17));
	a->SetOffset(424, Vector2Int(21,18));
	a->SetOffset(425, Vector2Int(21,18));
	a->SetOffset(426, Vector2Int(20,19));
	a->SetOffset(427, Vector2Int(20,19));
	a->SetOffset(428, Vector2Int(21,19));
	a->SetOffset(429, Vector2Int(21,19));
	a->SetOffset(430, Vector2Int(23,19));
	a->SetOffset(431, Vector2Int(23,19));
	a->SetOffset(432, Vector2Int(22,19));
	a->SetOffset(433, Vector2Int(21,21));
	a->SetOffset(434, Vector2Int(21,21));
	a->SetOffset(435, Vector2Int(22,19));
	a->SetOffset(436, Vector2Int(22,19));
	a->SetOffset(437, Vector2Int(24,18));
	a->SetOffset(438, Vector2Int(24,18));
	a->SetOffset(439, Vector2Int(23,18));
	a->SetOffset(440, Vector2Int(23,18));
	a->SetOffset(441, Vector2Int(21,20));
	a->SetOffset(442, Vector2Int(21,20));
	a->SetOffset(443, Vector2Int(20,19));
	a->SetOffset(444, Vector2Int(20,19));
	a->SetOffset(445, Vector2Int(21,19));
	a->SetOffset(446, Vector2Int(21,19));
	a->SetOffset(447, Vector2Int(23,19));
	a->SetOffset(448, Vector2Int(23,19));
	a->SetOffset(449, Vector2Int(21,19));
	a->SetOffset(450, Vector2Int(22,19));
	a->SetOffset(451, Vector2Int(23,19));
	a->SetOffset(452, Vector2Int(0,24));
	a->SetOffset(453, Vector2Int(0,25));
	a->SetOffset(454, Vector2Int(0,27));

	return a;
}

const SpriteFrameAtlas* SpriteDatabase::Load_unit_terran_tmashad() {
	const SpriteAtlas* atlas = Game::AssetLoader.LoadAtlas("unit_terran_tmashad.t3x");
	auto* a = unit_terran_tmashad = new SpriteFrameAtlas(atlas);
	a->FrameSize = Vector2Int(44, 44);

	a->SetOffset(0, Vector2Int(14,28));
	a->SetOffset(1, Vector2Int(14,28));
	a->SetOffset(2, Vector2Int(13,27));
	a->SetOffset(3, Vector2Int(13,27));
	a->SetOffset(4, Vector2Int(14,28));
	a->SetOffset(5, Vector2Int(14,28));
	a->SetOffset(6, Vector2Int(14,28));
	a->SetOffset(7, Vector2Int(14,28));
	a->SetOffset(8, Vector2Int(14,28));
	a->SetOffset(9, Vector2Int(14,28));
	a->SetOffset(10, Vector2Int(14,27));
	a->SetOffset(11, Vector2Int(14,27));
	a->SetOffset(12, Vector2Int(15,30));
	a->SetOffset(13, Vector2Int(15,30));
	a->SetOffset(14, Vector2Int(15,29));
	a->SetOffset(15, Vector2Int(15,29));
	a->SetOffset(16, Vector2Int(13,29));
	a->SetOffset(17, Vector2Int(14,28));
	a->SetOffset(18, Vector2Int(14,28));
	a->SetOffset(19, Vector2Int(13,27));
	a->SetOffset(20, Vector2Int(13,27));
	a->SetOffset(21, Vector2Int(14,28));
	a->SetOffset(22, Vector2Int(14,28));
	a->SetOffset(23, Vector2Int(14,28));
	a->SetOffset(24, Vector2Int(14,28));
	a->SetOffset(25, Vector2Int(14,28));
	a->SetOffset(26, Vector2Int(14,28));
	a->SetOffset(27, Vector2Int(14,27));
	a->SetOffset(28, Vector2Int(14,27));
	a->SetOffset(29, Vector2Int(15,30));
	a->SetOffset(30, Vector2Int(15,30));
	a->SetOffset(31, Vector2Int(15,29));
	a->SetOffset(32, Vector2Int(15,29));
	a->SetOffset(33, Vector2Int(13,29));
	a->SetOffset(34, Vector2Int(14,28));
	a->SetOffset(35, Vector2Int(14,28));
	a->SetOffset(36, Vector2Int(13,27));
	a->SetOffset(37, Vector2Int(13,27));
	a->SetOffset(38, Vector2Int(14,28));
	a->SetOffset(39, Vector2Int(14,28));
	a->SetOffset(40, Vector2Int(14,28));
	a->SetOffset(41, Vector2Int(14,28));
	a->SetOffset(42, Vector2Int(14,28));
	a->SetOffset(43, Vector2Int(14,28));
	a->SetOffset(44, Vector2Int(14,27));
	a->SetOffset(45, Vector2Int(14,27));
	a->SetOffset(46, Vector2Int(15,30));
	a->SetOffset(47, Vector2Int(15,30));
	a->SetOffset(48, Vector2Int(15,29));
	a->SetOffset(49, Vector2Int(15,29));
	a->SetOffset(50, Vector2Int(13,29));
	a->SetOffset(51, Vector2Int(14,28));
	a->SetOffset(52, Vector2Int(14,28));
	a->SetOffset(53, Vector2Int(13,27));
	a->SetOffset(54, Vector2Int(13,27));
	a->SetOffset(55, Vector2Int(14,28));
	a->SetOffset(56, Vector2Int(14,28));
	a->SetOffset(57, Vector2Int(14,28));
	a->SetOffset(58, Vector2Int(14,28));
	a->SetOffset(59, Vector2Int(14,28));
	a->SetOffset(60, Vector2Int(14,28));
	a->SetOffset(61, Vector2Int(14,27));
	a->SetOffset(62, Vector2Int(14,27));
	a->SetOffset(63, Vector2Int(15,30));
	a->SetOffset(64, Vector2Int(15,30));
	a->SetOffset(65, Vector2Int(15,29));
	a->SetOffset(66, Vector2Int(15,29));
	a->SetOffset(67, Vector2Int(13,29));
	a->SetOffset(68, Vector2Int(14,28));
	a->SetOffset(69, Vector2Int(14,28));
	a->SetOffset(70, Vector2Int(13,27));
	a->SetOffset(71, Vector2Int(13,27));
	a->SetOffset(72, Vector2Int(14,28));
	a->SetOffset(73, Vector2Int(14,28));
	a->SetOffset(74, Vector2Int(14,28));
	a->SetOffset(75, Vector2Int(14,28));
	a->SetOffset(76, Vector2Int(14,28));
	a->SetOffset(77, Vector2Int(14,28));
	a->SetOffset(78, Vector2Int(14,27));
	a->SetOffset(79, Vector2Int(14,27));
	a->SetOffset(80, Vector2Int(15,30));
	a->SetOffset(81, Vector2Int(15,30));
	a->SetOffset(82, Vector2Int(15,29));
	a->SetOffset(83, Vector2Int(15,29));
	a->SetOffset(84, Vector2Int(13,29));
	a->SetOffset(85, Vector2Int(14,27));
	a->SetOffset(86, Vector2Int(14,27));
	a->SetOffset(87, Vector2Int(13,28));
	a->SetOffset(88, Vector2Int(13,28));
	a->SetOffset(89, Vector2Int(12,29));
	a->SetOffset(90, Vector2Int(12,29));
	a->SetOffset(91, Vector2Int(13,28));
	a->SetOffset(92, Vector2Int(13,28));
	a->SetOffset(93, Vector2Int(13,29));
	a->SetOffset(94, Vector2Int(13,29));
	a->SetOffset(95, Vector2Int(15,28));
	a->SetOffset(96, Vector2Int(15,28));
	a->SetOffset(97, Vector2Int(14,27));
	a->SetOffset(98, Vector2Int(14,27));
	a->SetOffset(99, Vector2Int(14,26));
	a->SetOffset(100, Vector2Int(14,26));
	a->SetOffset(101, Vector2Int(14,26));
	a->SetOffset(102, Vector2Int(14,28));
	a->SetOffset(103, Vector2Int(14,28));
	a->SetOffset(104, Vector2Int(12,28));
	a->SetOffset(105, Vector2Int(12,28));
	a->SetOffset(106, Vector2Int(10,28));
	a->SetOffset(107, Vector2Int(10,28));
	a->SetOffset(108, Vector2Int(8,26));
	a->SetOffset(109, Vector2Int(8,26));
	a->SetOffset(110, Vector2Int(9,24));
	a->SetOffset(111, Vector2Int(9,24));
	a->SetOffset(112, Vector2Int(16,27));
	a->SetOffset(113, Vector2Int(16,27));
	a->SetOffset(114, Vector2Int(13,28));
	a->SetOffset(115, Vector2Int(13,28));
	a->SetOffset(116, Vector2Int(15,27));
	a->SetOffset(117, Vector2Int(15,27));
	a->SetOffset(118, Vector2Int(14,27));
	a->SetOffset(119, Vector2Int(13,30));
	a->SetOffset(120, Vector2Int(13,30));
	a->SetOffset(121, Vector2Int(12,28));
	a->SetOffset(122, Vector2Int(12,28));
	a->SetOffset(123, Vector2Int(11,26));
	a->SetOffset(124, Vector2Int(11,26));
	a->SetOffset(125, Vector2Int(10,27));
	a->SetOffset(126, Vector2Int(10,27));
	a->SetOffset(127, Vector2Int(11,25));
	a->SetOffset(128, Vector2Int(11,25));
	a->SetOffset(129, Vector2Int(15,27));
	a->SetOffset(130, Vector2Int(15,27));
	a->SetOffset(131, Vector2Int(14,27));
	a->SetOffset(132, Vector2Int(14,27));
	a->SetOffset(133, Vector2Int(16,27));
	a->SetOffset(134, Vector2Int(16,27));
	a->SetOffset(135, Vector2Int(14,27));
	a->SetOffset(136, Vector2Int(14,30));
	a->SetOffset(137, Vector2Int(14,30));
	a->SetOffset(138, Vector2Int(12,27));
	a->SetOffset(139, Vector2Int(12,27));
	a->SetOffset(140, Vector2Int(12,27));
	a->SetOffset(141, Vector2Int(12,27));
	a->SetOffset(142, Vector2Int(12,25));
	a->SetOffset(143, Vector2Int(12,25));
	a->SetOffset(144, Vector2Int(11,23));
	a->SetOffset(145, Vector2Int(11,23));
	a->SetOffset(146, Vector2Int(14,28));
	a->SetOffset(147, Vector2Int(14,28));
	a->SetOffset(148, Vector2Int(13,27));
	a->SetOffset(149, Vector2Int(13,27));
	a->SetOffset(150, Vector2Int(15,27));
	a->SetOffset(151, Vector2Int(15,27));
	a->SetOffset(152, Vector2Int(14,26));
	a->SetOffset(153, Vector2Int(14,28));
	a->SetOffset(154, Vector2Int(14,28));
	a->SetOffset(155, Vector2Int(14,26));
	a->SetOffset(156, Vector2Int(14,26));
	a->SetOffset(157, Vector2Int(14,29));
	a->SetOffset(158, Vector2Int(14,29));
	a->SetOffset(159, Vector2Int(14,28));
	a->SetOffset(160, Vector2Int(14,28));
	a->SetOffset(161, Vector2Int(13,26));
	a->SetOffset(162, Vector2Int(13,26));
	a->SetOffset(163, Vector2Int(12,26));
	a->SetOffset(164, Vector2Int(12,26));
	a->SetOffset(165, Vector2Int(12,27));
	a->SetOffset(166, Vector2Int(12,27));
	a->SetOffset(167, Vector2Int(13,28));
	a->SetOffset(168, Vector2Int(13,28));
	a->SetOffset(169, Vector2Int(14,26));
	a->SetOffset(170, Vector2Int(14,29));
	a->SetOffset(171, Vector2Int(14,29));
	a->SetOffset(172, Vector2Int(15,28));
	a->SetOffset(173, Vector2Int(15,28));
	a->SetOffset(174, Vector2Int(15,29));
	a->SetOffset(175, Vector2Int(15,29));
	a->SetOffset(176, Vector2Int(13,26));
	a->SetOffset(177, Vector2Int(13,26));
	a->SetOffset(178, Vector2Int(12,27));
	a->SetOffset(179, Vector2Int(12,27));
	a->SetOffset(180, Vector2Int(10,26));
	a->SetOffset(181, Vector2Int(10,26));
	a->SetOffset(182, Vector2Int(12,28));
	a->SetOffset(183, Vector2Int(12,28));
	a->SetOffset(184, Vector2Int(14,27));
	a->SetOffset(185, Vector2Int(14,27));
	a->SetOffset(186, Vector2Int(15,26));
	a->SetOffset(187, Vector2Int(14,29));
	a->SetOffset(188, Vector2Int(14,29));
	a->SetOffset(189, Vector2Int(15,29));
	a->SetOffset(190, Vector2Int(15,29));
	a->SetOffset(191, Vector2Int(14,30));
	a->SetOffset(192, Vector2Int(14,30));
	a->SetOffset(193, Vector2Int(12,30));
	a->SetOffset(194, Vector2Int(12,30));
	a->SetOffset(195, Vector2Int(10,28));
	a->SetOffset(196, Vector2Int(10,28));
	a->SetOffset(197, Vector2Int(9,27));
	a->SetOffset(198, Vector2Int(9,27));
	a->SetOffset(199, Vector2Int(11,28));
	a->SetOffset(200, Vector2Int(11,28));
	a->SetOffset(201, Vector2Int(13,27));
	a->SetOffset(202, Vector2Int(13,27));
	a->SetOffset(203, Vector2Int(15,26));
	a->SetOffset(204, Vector2Int(14,30));
	a->SetOffset(205, Vector2Int(14,30));
	a->SetOffset(206, Vector2Int(14,29));
	a->SetOffset(207, Vector2Int(14,29));
	a->SetOffset(208, Vector2Int(14,31));
	a->SetOffset(209, Vector2Int(14,31));
	a->SetOffset(210, Vector2Int(11,28));
	a->SetOffset(211, Vector2Int(11,28));
	a->SetOffset(212, Vector2Int(10,24));
	a->SetOffset(213, Vector2Int(10,24));
	a->SetOffset(214, Vector2Int(9,29));
	a->SetOffset(215, Vector2Int(9,29));
	a->SetOffset(216, Vector2Int(12,27));
	a->SetOffset(217, Vector2Int(12,27));
	a->SetOffset(218, Vector2Int(12,26));
	a->SetOffset(219, Vector2Int(12,26));
	a->SetOffset(220, Vector2Int(15,27));

	return a;
}

const SpriteFrameAtlas* SpriteDatabase::Load_unit_terran_scv() {
	const SpriteAtlas* atlas = Game::AssetLoader.LoadAtlas("unit_terran_scv.t3x");
	auto* a = unit_terran_scv = new SpriteFrameAtlas(atlas);
	a->FrameSize = Vector2Int(72, 72);

	a->SetOffset(0, Vector2Int(21,19));
	a->SetOffset(1, Vector2Int(21,19));
	a->SetOffset(2, Vector2Int(23,17));
	a->SetOffset(3, Vector2Int(23,17));
	a->SetOffset(4, Vector2Int(24,16));
	a->SetOffset(5, Vector2Int(24,16));
	a->SetOffset(6, Vector2Int(24,17));
	a->SetOffset(7, Vector2Int(24,17));
	a->SetOffset(8, Vector2Int(25,17));
	a->SetOffset(9, Vector2Int(25,17));
	a->SetOffset(10, Vector2Int(24,15));
	a->SetOffset(11, Vector2Int(24,15));
	a->SetOffset(12, Vector2Int(24,14));
	a->SetOffset(13, Vector2Int(24,14));
	a->SetOffset(14, Vector2Int(23,14));
	a->SetOffset(15, Vector2Int(23,14));
	a->SetOffset(16, Vector2Int(20,15));
	a->SetOffset(17, Vector2Int(13,14));
	a->SetOffset(18, Vector2Int(13,14));
	a->SetOffset(19, Vector2Int(16,18));
	a->SetOffset(20, Vector2Int(16,18));
	a->SetOffset(21, Vector2Int(22,17));
	a->SetOffset(22, Vector2Int(22,17));
	a->SetOffset(23, Vector2Int(24,16));
	a->SetOffset(24, Vector2Int(24,16));
	a->SetOffset(25, Vector2Int(25,17));
	a->SetOffset(26, Vector2Int(25,17));
	a->SetOffset(27, Vector2Int(24,15));
	a->SetOffset(28, Vector2Int(24,15));
	a->SetOffset(29, Vector2Int(24,14));
	a->SetOffset(30, Vector2Int(24,14));
	a->SetOffset(31, Vector2Int(25,14));
	a->SetOffset(32, Vector2Int(25,14));
	a->SetOffset(33, Vector2Int(23,15));
	a->SetOffset(34, Vector2Int(13,14));
	a->SetOffset(35, Vector2Int(13,14));
	a->SetOffset(36, Vector2Int(16,18));
	a->SetOffset(37, Vector2Int(16,18));
	a->SetOffset(38, Vector2Int(22,17));
	a->SetOffset(39, Vector2Int(22,17));
	a->SetOffset(40, Vector2Int(24,16));
	a->SetOffset(41, Vector2Int(24,16));
	a->SetOffset(42, Vector2Int(25,17));
	a->SetOffset(43, Vector2Int(25,17));
	a->SetOffset(44, Vector2Int(24,15));
	a->SetOffset(45, Vector2Int(24,15));
	a->SetOffset(46, Vector2Int(24,14));
	a->SetOffset(47, Vector2Int(24,14));
	a->SetOffset(48, Vector2Int(25,14));
	a->SetOffset(49, Vector2Int(25,14));
	a->SetOffset(50, Vector2Int(23,15));
	a->SetOffset(51, Vector2Int(21,19));
	a->SetOffset(52, Vector2Int(21,19));
	a->SetOffset(53, Vector2Int(23,17));
	a->SetOffset(54, Vector2Int(23,17));
	a->SetOffset(55, Vector2Int(24,16));
	a->SetOffset(56, Vector2Int(24,16));
	a->SetOffset(57, Vector2Int(24,17));
	a->SetOffset(58, Vector2Int(24,17));
	a->SetOffset(59, Vector2Int(25,17));
	a->SetOffset(60, Vector2Int(25,17));
	a->SetOffset(61, Vector2Int(24,15));
	a->SetOffset(62, Vector2Int(24,15));
	a->SetOffset(63, Vector2Int(24,14));
	a->SetOffset(64, Vector2Int(24,14));
	a->SetOffset(65, Vector2Int(23,14));
	a->SetOffset(66, Vector2Int(23,14));
	a->SetOffset(67, Vector2Int(20,15));
	a->SetOffset(68, Vector2Int(13,14));
	a->SetOffset(69, Vector2Int(13,14));
	a->SetOffset(70, Vector2Int(16,18));
	a->SetOffset(71, Vector2Int(16,18));
	a->SetOffset(72, Vector2Int(22,17));
	a->SetOffset(73, Vector2Int(22,17));
	a->SetOffset(74, Vector2Int(24,16));
	a->SetOffset(75, Vector2Int(24,16));
	a->SetOffset(76, Vector2Int(25,17));
	a->SetOffset(77, Vector2Int(25,17));
	a->SetOffset(78, Vector2Int(24,15));
	a->SetOffset(79, Vector2Int(24,15));
	a->SetOffset(80, Vector2Int(24,14));
	a->SetOffset(81, Vector2Int(24,14));
	a->SetOffset(82, Vector2Int(25,14));
	a->SetOffset(83, Vector2Int(25,14));
	a->SetOffset(84, Vector2Int(23,15));
	a->SetOffset(85, Vector2Int(13,14));
	a->SetOffset(86, Vector2Int(13,14));
	a->SetOffset(87, Vector2Int(16,18));
	a->SetOffset(88, Vector2Int(16,18));
	a->SetOffset(89, Vector2Int(22,17));
	a->SetOffset(90, Vector2Int(22,17));
	a->SetOffset(91, Vector2Int(24,16));
	a->SetOffset(92, Vector2Int(24,16));
	a->SetOffset(93, Vector2Int(25,17));
	a->SetOffset(94, Vector2Int(25,17));
	a->SetOffset(95, Vector2Int(24,15));
	a->SetOffset(96, Vector2Int(24,15));
	a->SetOffset(97, Vector2Int(24,14));
	a->SetOffset(98, Vector2Int(24,14));
	a->SetOffset(99, Vector2Int(25,14));
	a->SetOffset(100, Vector2Int(25,14));
	a->SetOffset(101, Vector2Int(23,15));

	return a;
}

const SpriteFrameAtlas* SpriteDatabase::Load_unit_thingy_tbangs() {
	const SpriteAtlas* atlas = Game::AssetLoader.LoadAtlas("unit_thingy_tbangs.t3x");
	auto* a = unit_thingy_tbangs = new SpriteFrameAtlas(atlas);
	a->FrameSize = Vector2Int(128, 128);

	a->SetOffset(0, Vector2Int(30,47));
	a->SetOffset(1, Vector2Int(19,32));
	a->SetOffset(2, Vector2Int(17,25));
	a->SetOffset(3, Vector2Int(16,17));
	a->SetOffset(4, Vector2Int(16,14));
	a->SetOffset(5, Vector2Int(17,14));
	a->SetOffset(6, Vector2Int(18,14));
	a->SetOffset(7, Vector2Int(16,15));
	a->SetOffset(8, Vector2Int(14,21));

	return a;
}

