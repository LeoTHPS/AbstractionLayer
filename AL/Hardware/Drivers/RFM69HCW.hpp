#pragma once
#include "AL/Common.hpp"

#include "IDriver.hpp"

#include "AL/OS/Timer.hpp"

#include "AL/Hardware/SPI.hpp"

#include "AL/Collections/Array.hpp"

namespace AL::Hardware::Drivers
{
	enum class RFM69HCWLnaGains : uint8
	{
		Auto = 0b000,
		G1   = 0b001, // 0 dB
		G2   = 0b010, // 6 dB
		G3   = 0b011, // 12 dB
		G4   = 0b100, // 24 dB
		G5   = 0b101, // 36 dB
		G6   = 0b110  // 48 dB
	};

	enum class RFM69HCWLnaImpedances : uint8
	{
		OHM_50  = 50,
		OHM_200 = 200
	};

	enum class RFM69HCWOutputPowerModes : uint8
	{
		PA_0     = 0b1000,
		PA_1     = 0b0100,
		PA_2     = 0b0110,
		PA_BOOST = 0b0111
	};

	typedef uint8 RFM69HCWOutputPowerLevel;

	enum RFM69HCW_OUTPUT_POWER_LEVELS : RFM69HCWOutputPowerLevel
	{
		RFM69HCW_OUTPUT_POWER_LEVEL_0       = 0,
		RFM69HCW_OUTPUT_POWER_LEVEL_1       = 1,
		RFM69HCW_OUTPUT_POWER_LEVEL_2       = 2,
		RFM69HCW_OUTPUT_POWER_LEVEL_3       = 3,
		RFM69HCW_OUTPUT_POWER_LEVEL_4       = 4,
		RFM69HCW_OUTPUT_POWER_LEVEL_5       = 5,
		RFM69HCW_OUTPUT_POWER_LEVEL_6       = 6,
		RFM69HCW_OUTPUT_POWER_LEVEL_7       = 7,
		RFM69HCW_OUTPUT_POWER_LEVEL_8       = 8,
		RFM69HCW_OUTPUT_POWER_LEVEL_9       = 9,
		RFM69HCW_OUTPUT_POWER_LEVEL_10      = 10,
		RFM69HCW_OUTPUT_POWER_LEVEL_11      = 11,
		RFM69HCW_OUTPUT_POWER_LEVEL_12      = 12,
		RFM69HCW_OUTPUT_POWER_LEVEL_13      = 13,
		RFM69HCW_OUTPUT_POWER_LEVEL_14      = 14,
		RFM69HCW_OUTPUT_POWER_LEVEL_15      = 15,
		RFM69HCW_OUTPUT_POWER_LEVEL_16      = 16,
		RFM69HCW_OUTPUT_POWER_LEVEL_17      = 17,
		RFM69HCW_OUTPUT_POWER_LEVEL_18      = 18,
		RFM69HCW_OUTPUT_POWER_LEVEL_19      = 19,
		RFM69HCW_OUTPUT_POWER_LEVEL_20      = 20,
		RFM69HCW_OUTPUT_POWER_LEVEL_21      = 21,
		RFM69HCW_OUTPUT_POWER_LEVEL_22      = 22,
		RFM69HCW_OUTPUT_POWER_LEVEL_23      = 23,
		RFM69HCW_OUTPUT_POWER_LEVEL_24      = 24,
		RFM69HCW_OUTPUT_POWER_LEVEL_25      = 25,
		RFM69HCW_OUTPUT_POWER_LEVEL_26      = 26,
		RFM69HCW_OUTPUT_POWER_LEVEL_27      = 27,
		RFM69HCW_OUTPUT_POWER_LEVEL_28      = 28,
		RFM69HCW_OUTPUT_POWER_LEVEL_29      = 29,
		RFM69HCW_OUTPUT_POWER_LEVEL_30      = 30,
		RFM69HCW_OUTPUT_POWER_LEVEL_31      = 31,

		RFM69HCW_OUTPUT_POWER_LEVEL_MINIMUM = RFM69HCW_OUTPUT_POWER_LEVEL_0,
		RFM69HCW_OUTPUT_POWER_LEVEL_MAXIMUM = RFM69HCW_OUTPUT_POWER_LEVEL_31
	};

	typedef uint8 RFM69HCWNetwork;
	typedef uint8 RFM69HCWNetworkAddress;

	enum RFM69HCW_NETWORKS : RFM69HCWNetwork
	{
		RFM69HCW_NETWORK_0   = 0,
		RFM69HCW_NETWORK_1   = 1,
		RFM69HCW_NETWORK_2   = 2,
		RFM69HCW_NETWORK_3   = 3,
		RFM69HCW_NETWORK_4   = 4,
		RFM69HCW_NETWORK_5   = 5,
		RFM69HCW_NETWORK_6   = 6,
		RFM69HCW_NETWORK_7   = 7,
		RFM69HCW_NETWORK_8   = 8,
		RFM69HCW_NETWORK_9   = 9,
		RFM69HCW_NETWORK_10  = 10,
		RFM69HCW_NETWORK_11  = 11,
		RFM69HCW_NETWORK_12  = 12,
		RFM69HCW_NETWORK_13  = 13,
		RFM69HCW_NETWORK_14  = 14,
		RFM69HCW_NETWORK_15  = 15,
		RFM69HCW_NETWORK_16  = 16,
		RFM69HCW_NETWORK_17  = 17,
		RFM69HCW_NETWORK_18  = 18,
		RFM69HCW_NETWORK_19  = 19,
		RFM69HCW_NETWORK_20  = 20,
		RFM69HCW_NETWORK_21  = 21,
		RFM69HCW_NETWORK_22  = 22,
		RFM69HCW_NETWORK_23  = 23,
		RFM69HCW_NETWORK_24  = 24,
		RFM69HCW_NETWORK_25  = 25,
		RFM69HCW_NETWORK_26  = 26,
		RFM69HCW_NETWORK_27  = 27,
		RFM69HCW_NETWORK_28  = 28,
		RFM69HCW_NETWORK_29  = 29,
		RFM69HCW_NETWORK_30  = 30,
		RFM69HCW_NETWORK_31  = 31,
		RFM69HCW_NETWORK_32  = 32,
		RFM69HCW_NETWORK_33  = 33,
		RFM69HCW_NETWORK_34  = 34,
		RFM69HCW_NETWORK_35  = 35,
		RFM69HCW_NETWORK_36  = 36,
		RFM69HCW_NETWORK_37  = 37,
		RFM69HCW_NETWORK_38  = 38,
		RFM69HCW_NETWORK_39  = 39,
		RFM69HCW_NETWORK_40  = 40,
		RFM69HCW_NETWORK_41  = 41,
		RFM69HCW_NETWORK_42  = 42,
		RFM69HCW_NETWORK_43  = 43,
		RFM69HCW_NETWORK_44  = 44,
		RFM69HCW_NETWORK_45  = 45,
		RFM69HCW_NETWORK_46  = 46,
		RFM69HCW_NETWORK_47  = 47,
		RFM69HCW_NETWORK_48  = 48,
		RFM69HCW_NETWORK_49  = 49,
		RFM69HCW_NETWORK_50  = 50,
		RFM69HCW_NETWORK_51  = 51,
		RFM69HCW_NETWORK_52  = 52,
		RFM69HCW_NETWORK_53  = 53,
		RFM69HCW_NETWORK_54  = 54,
		RFM69HCW_NETWORK_55  = 55,
		RFM69HCW_NETWORK_56  = 56,
		RFM69HCW_NETWORK_57  = 57,
		RFM69HCW_NETWORK_58  = 58,
		RFM69HCW_NETWORK_59  = 59,
		RFM69HCW_NETWORK_60  = 60,
		RFM69HCW_NETWORK_61  = 61,
		RFM69HCW_NETWORK_62  = 62,
		RFM69HCW_NETWORK_63  = 63,
		RFM69HCW_NETWORK_64  = 64,
		RFM69HCW_NETWORK_65  = 65,
		RFM69HCW_NETWORK_66  = 66,
		RFM69HCW_NETWORK_67  = 67,
		RFM69HCW_NETWORK_68  = 68,
		RFM69HCW_NETWORK_69  = 69,
		RFM69HCW_NETWORK_70  = 70,
		RFM69HCW_NETWORK_71  = 71,
		RFM69HCW_NETWORK_72  = 72,
		RFM69HCW_NETWORK_73  = 73,
		RFM69HCW_NETWORK_74  = 74,
		RFM69HCW_NETWORK_75  = 75,
		RFM69HCW_NETWORK_76  = 76,
		RFM69HCW_NETWORK_77  = 77,
		RFM69HCW_NETWORK_78  = 78,
		RFM69HCW_NETWORK_79  = 79,
		RFM69HCW_NETWORK_80  = 80,
		RFM69HCW_NETWORK_81  = 81,
		RFM69HCW_NETWORK_82  = 82,
		RFM69HCW_NETWORK_83  = 83,
		RFM69HCW_NETWORK_84  = 84,
		RFM69HCW_NETWORK_85  = 85,
		RFM69HCW_NETWORK_86  = 86,
		RFM69HCW_NETWORK_87  = 87,
		RFM69HCW_NETWORK_88  = 88,
		RFM69HCW_NETWORK_89  = 89,
		RFM69HCW_NETWORK_90  = 90,
		RFM69HCW_NETWORK_91  = 91,
		RFM69HCW_NETWORK_92  = 92,
		RFM69HCW_NETWORK_93  = 93,
		RFM69HCW_NETWORK_94  = 94,
		RFM69HCW_NETWORK_95  = 95,
		RFM69HCW_NETWORK_96  = 96,
		RFM69HCW_NETWORK_97  = 97,
		RFM69HCW_NETWORK_98  = 98,
		RFM69HCW_NETWORK_99  = 99,
		RFM69HCW_NETWORK_100 = 100,
		RFM69HCW_NETWORK_101 = 101,
		RFM69HCW_NETWORK_102 = 102,
		RFM69HCW_NETWORK_103 = 103,
		RFM69HCW_NETWORK_104 = 104,
		RFM69HCW_NETWORK_105 = 105,
		RFM69HCW_NETWORK_106 = 106,
		RFM69HCW_NETWORK_107 = 107,
		RFM69HCW_NETWORK_108 = 108,
		RFM69HCW_NETWORK_109 = 109,
		RFM69HCW_NETWORK_110 = 110,
		RFM69HCW_NETWORK_111 = 111,
		RFM69HCW_NETWORK_112 = 112,
		RFM69HCW_NETWORK_113 = 113,
		RFM69HCW_NETWORK_114 = 114,
		RFM69HCW_NETWORK_115 = 115,
		RFM69HCW_NETWORK_116 = 116,
		RFM69HCW_NETWORK_117 = 117,
		RFM69HCW_NETWORK_118 = 118,
		RFM69HCW_NETWORK_119 = 119,
		RFM69HCW_NETWORK_120 = 120,
		RFM69HCW_NETWORK_121 = 121,
		RFM69HCW_NETWORK_122 = 122,
		RFM69HCW_NETWORK_123 = 123,
		RFM69HCW_NETWORK_124 = 124,
		RFM69HCW_NETWORK_125 = 125,
		RFM69HCW_NETWORK_126 = 126,
		RFM69HCW_NETWORK_127 = 127,
		RFM69HCW_NETWORK_128 = 128,
		RFM69HCW_NETWORK_129 = 129,
		RFM69HCW_NETWORK_130 = 130,
		RFM69HCW_NETWORK_131 = 131,
		RFM69HCW_NETWORK_132 = 132,
		RFM69HCW_NETWORK_133 = 133,
		RFM69HCW_NETWORK_134 = 134,
		RFM69HCW_NETWORK_135 = 135,
		RFM69HCW_NETWORK_136 = 136,
		RFM69HCW_NETWORK_137 = 137,
		RFM69HCW_NETWORK_138 = 138,
		RFM69HCW_NETWORK_139 = 139,
		RFM69HCW_NETWORK_140 = 140,
		RFM69HCW_NETWORK_141 = 141,
		RFM69HCW_NETWORK_142 = 142,
		RFM69HCW_NETWORK_143 = 143,
		RFM69HCW_NETWORK_144 = 144,
		RFM69HCW_NETWORK_145 = 145,
		RFM69HCW_NETWORK_146 = 146,
		RFM69HCW_NETWORK_147 = 147,
		RFM69HCW_NETWORK_148 = 148,
		RFM69HCW_NETWORK_149 = 149,
		RFM69HCW_NETWORK_150 = 150,
		RFM69HCW_NETWORK_151 = 151,
		RFM69HCW_NETWORK_152 = 152,
		RFM69HCW_NETWORK_153 = 153,
		RFM69HCW_NETWORK_154 = 154,
		RFM69HCW_NETWORK_155 = 155,
		RFM69HCW_NETWORK_156 = 156,
		RFM69HCW_NETWORK_157 = 157,
		RFM69HCW_NETWORK_158 = 158,
		RFM69HCW_NETWORK_159 = 159,
		RFM69HCW_NETWORK_160 = 160,
		RFM69HCW_NETWORK_161 = 161,
		RFM69HCW_NETWORK_162 = 162,
		RFM69HCW_NETWORK_163 = 163,
		RFM69HCW_NETWORK_164 = 164,
		RFM69HCW_NETWORK_165 = 165,
		RFM69HCW_NETWORK_166 = 166,
		RFM69HCW_NETWORK_167 = 167,
		RFM69HCW_NETWORK_168 = 168,
		RFM69HCW_NETWORK_169 = 169,
		RFM69HCW_NETWORK_170 = 170,
		RFM69HCW_NETWORK_171 = 171,
		RFM69HCW_NETWORK_172 = 172,
		RFM69HCW_NETWORK_173 = 173,
		RFM69HCW_NETWORK_174 = 174,
		RFM69HCW_NETWORK_175 = 175,
		RFM69HCW_NETWORK_176 = 176,
		RFM69HCW_NETWORK_177 = 177,
		RFM69HCW_NETWORK_178 = 178,
		RFM69HCW_NETWORK_179 = 179,
		RFM69HCW_NETWORK_180 = 180,
		RFM69HCW_NETWORK_181 = 181,
		RFM69HCW_NETWORK_182 = 182,
		RFM69HCW_NETWORK_183 = 183,
		RFM69HCW_NETWORK_184 = 184,
		RFM69HCW_NETWORK_185 = 185,
		RFM69HCW_NETWORK_186 = 186,
		RFM69HCW_NETWORK_187 = 187,
		RFM69HCW_NETWORK_188 = 188,
		RFM69HCW_NETWORK_189 = 189,
		RFM69HCW_NETWORK_190 = 190,
		RFM69HCW_NETWORK_191 = 191,
		RFM69HCW_NETWORK_192 = 192,
		RFM69HCW_NETWORK_193 = 193,
		RFM69HCW_NETWORK_194 = 194,
		RFM69HCW_NETWORK_195 = 195,
		RFM69HCW_NETWORK_196 = 196,
		RFM69HCW_NETWORK_197 = 197,
		RFM69HCW_NETWORK_198 = 198,
		RFM69HCW_NETWORK_199 = 199,
		RFM69HCW_NETWORK_200 = 200,
		RFM69HCW_NETWORK_201 = 201,
		RFM69HCW_NETWORK_202 = 202,
		RFM69HCW_NETWORK_203 = 203,
		RFM69HCW_NETWORK_204 = 204,
		RFM69HCW_NETWORK_205 = 205,
		RFM69HCW_NETWORK_206 = 206,
		RFM69HCW_NETWORK_207 = 207,
		RFM69HCW_NETWORK_208 = 208,
		RFM69HCW_NETWORK_209 = 209,
		RFM69HCW_NETWORK_210 = 210,
		RFM69HCW_NETWORK_211 = 211,
		RFM69HCW_NETWORK_212 = 212,
		RFM69HCW_NETWORK_213 = 213,
		RFM69HCW_NETWORK_214 = 214,
		RFM69HCW_NETWORK_215 = 215,
		RFM69HCW_NETWORK_216 = 216,
		RFM69HCW_NETWORK_217 = 217,
		RFM69HCW_NETWORK_218 = 218,
		RFM69HCW_NETWORK_219 = 219,
		RFM69HCW_NETWORK_220 = 220,
		RFM69HCW_NETWORK_221 = 221,
		RFM69HCW_NETWORK_222 = 222,
		RFM69HCW_NETWORK_223 = 223,
		RFM69HCW_NETWORK_224 = 224,
		RFM69HCW_NETWORK_225 = 225,
		RFM69HCW_NETWORK_226 = 226,
		RFM69HCW_NETWORK_227 = 227,
		RFM69HCW_NETWORK_228 = 228,
		RFM69HCW_NETWORK_229 = 229,
		RFM69HCW_NETWORK_230 = 230,
		RFM69HCW_NETWORK_231 = 231,
		RFM69HCW_NETWORK_232 = 232,
		RFM69HCW_NETWORK_233 = 233,
		RFM69HCW_NETWORK_234 = 234,
		RFM69HCW_NETWORK_235 = 235,
		RFM69HCW_NETWORK_236 = 236,
		RFM69HCW_NETWORK_237 = 237,
		RFM69HCW_NETWORK_238 = 238,
		RFM69HCW_NETWORK_239 = 239,
		RFM69HCW_NETWORK_240 = 240,
		RFM69HCW_NETWORK_241 = 241,
		RFM69HCW_NETWORK_242 = 242,
		RFM69HCW_NETWORK_243 = 243,
		RFM69HCW_NETWORK_244 = 244,
		RFM69HCW_NETWORK_245 = 245,
		RFM69HCW_NETWORK_246 = 246,
		RFM69HCW_NETWORK_247 = 247,
		RFM69HCW_NETWORK_248 = 248,
		RFM69HCW_NETWORK_249 = 249,
		RFM69HCW_NETWORK_250 = 250,
		RFM69HCW_NETWORK_251 = 251,
		RFM69HCW_NETWORK_252 = 252,
		RFM69HCW_NETWORK_253 = 253,
		RFM69HCW_NETWORK_254 = 254,
		RFM69HCW_NETWORK_255 = 255
	};

	enum RFM69HCW_NETWORK_ADDRESSES : RFM69HCWNetworkAddress
	{
		RFM69HCW_NETWORK_ADDRESS_0         = 0,
		RFM69HCW_NETWORK_ADDRESS_1         = 1,
		RFM69HCW_NETWORK_ADDRESS_2         = 2,
		RFM69HCW_NETWORK_ADDRESS_3         = 3,
		RFM69HCW_NETWORK_ADDRESS_4         = 4,
		RFM69HCW_NETWORK_ADDRESS_5         = 5,
		RFM69HCW_NETWORK_ADDRESS_6         = 6,
		RFM69HCW_NETWORK_ADDRESS_7         = 7,
		RFM69HCW_NETWORK_ADDRESS_8         = 8,
		RFM69HCW_NETWORK_ADDRESS_9         = 9,
		RFM69HCW_NETWORK_ADDRESS_10        = 10,
		RFM69HCW_NETWORK_ADDRESS_11        = 11,
		RFM69HCW_NETWORK_ADDRESS_12        = 12,
		RFM69HCW_NETWORK_ADDRESS_13        = 13,
		RFM69HCW_NETWORK_ADDRESS_14        = 14,
		RFM69HCW_NETWORK_ADDRESS_15        = 15,
		RFM69HCW_NETWORK_ADDRESS_16        = 16,
		RFM69HCW_NETWORK_ADDRESS_17        = 17,
		RFM69HCW_NETWORK_ADDRESS_18        = 18,
		RFM69HCW_NETWORK_ADDRESS_19        = 19,
		RFM69HCW_NETWORK_ADDRESS_20        = 20,
		RFM69HCW_NETWORK_ADDRESS_21        = 21,
		RFM69HCW_NETWORK_ADDRESS_22        = 22,
		RFM69HCW_NETWORK_ADDRESS_23        = 23,
		RFM69HCW_NETWORK_ADDRESS_24        = 24,
		RFM69HCW_NETWORK_ADDRESS_25        = 25,
		RFM69HCW_NETWORK_ADDRESS_26        = 26,
		RFM69HCW_NETWORK_ADDRESS_27        = 27,
		RFM69HCW_NETWORK_ADDRESS_28        = 28,
		RFM69HCW_NETWORK_ADDRESS_29        = 29,
		RFM69HCW_NETWORK_ADDRESS_30        = 30,
		RFM69HCW_NETWORK_ADDRESS_31        = 31,
		RFM69HCW_NETWORK_ADDRESS_32        = 32,
		RFM69HCW_NETWORK_ADDRESS_33        = 33,
		RFM69HCW_NETWORK_ADDRESS_34        = 34,
		RFM69HCW_NETWORK_ADDRESS_35        = 35,
		RFM69HCW_NETWORK_ADDRESS_36        = 36,
		RFM69HCW_NETWORK_ADDRESS_37        = 37,
		RFM69HCW_NETWORK_ADDRESS_38        = 38,
		RFM69HCW_NETWORK_ADDRESS_39        = 39,
		RFM69HCW_NETWORK_ADDRESS_40        = 40,
		RFM69HCW_NETWORK_ADDRESS_41        = 41,
		RFM69HCW_NETWORK_ADDRESS_42        = 42,
		RFM69HCW_NETWORK_ADDRESS_43        = 43,
		RFM69HCW_NETWORK_ADDRESS_44        = 44,
		RFM69HCW_NETWORK_ADDRESS_45        = 45,
		RFM69HCW_NETWORK_ADDRESS_46        = 46,
		RFM69HCW_NETWORK_ADDRESS_47        = 47,
		RFM69HCW_NETWORK_ADDRESS_48        = 48,
		RFM69HCW_NETWORK_ADDRESS_49        = 49,
		RFM69HCW_NETWORK_ADDRESS_50        = 50,
		RFM69HCW_NETWORK_ADDRESS_51        = 51,
		RFM69HCW_NETWORK_ADDRESS_52        = 52,
		RFM69HCW_NETWORK_ADDRESS_53        = 53,
		RFM69HCW_NETWORK_ADDRESS_54        = 54,
		RFM69HCW_NETWORK_ADDRESS_55        = 55,
		RFM69HCW_NETWORK_ADDRESS_56        = 56,
		RFM69HCW_NETWORK_ADDRESS_57        = 57,
		RFM69HCW_NETWORK_ADDRESS_58        = 58,
		RFM69HCW_NETWORK_ADDRESS_59        = 59,
		RFM69HCW_NETWORK_ADDRESS_60        = 60,
		RFM69HCW_NETWORK_ADDRESS_61        = 61,
		RFM69HCW_NETWORK_ADDRESS_62        = 62,
		RFM69HCW_NETWORK_ADDRESS_63        = 63,
		RFM69HCW_NETWORK_ADDRESS_64        = 64,
		RFM69HCW_NETWORK_ADDRESS_65        = 65,
		RFM69HCW_NETWORK_ADDRESS_66        = 66,
		RFM69HCW_NETWORK_ADDRESS_67        = 67,
		RFM69HCW_NETWORK_ADDRESS_68        = 68,
		RFM69HCW_NETWORK_ADDRESS_69        = 69,
		RFM69HCW_NETWORK_ADDRESS_70        = 70,
		RFM69HCW_NETWORK_ADDRESS_71        = 71,
		RFM69HCW_NETWORK_ADDRESS_72        = 72,
		RFM69HCW_NETWORK_ADDRESS_73        = 73,
		RFM69HCW_NETWORK_ADDRESS_74        = 74,
		RFM69HCW_NETWORK_ADDRESS_75        = 75,
		RFM69HCW_NETWORK_ADDRESS_76        = 76,
		RFM69HCW_NETWORK_ADDRESS_77        = 77,
		RFM69HCW_NETWORK_ADDRESS_78        = 78,
		RFM69HCW_NETWORK_ADDRESS_79        = 79,
		RFM69HCW_NETWORK_ADDRESS_80        = 80,
		RFM69HCW_NETWORK_ADDRESS_81        = 81,
		RFM69HCW_NETWORK_ADDRESS_82        = 82,
		RFM69HCW_NETWORK_ADDRESS_83        = 83,
		RFM69HCW_NETWORK_ADDRESS_84        = 84,
		RFM69HCW_NETWORK_ADDRESS_85        = 85,
		RFM69HCW_NETWORK_ADDRESS_86        = 86,
		RFM69HCW_NETWORK_ADDRESS_87        = 87,
		RFM69HCW_NETWORK_ADDRESS_88        = 88,
		RFM69HCW_NETWORK_ADDRESS_89        = 89,
		RFM69HCW_NETWORK_ADDRESS_90        = 90,
		RFM69HCW_NETWORK_ADDRESS_91        = 91,
		RFM69HCW_NETWORK_ADDRESS_92        = 92,
		RFM69HCW_NETWORK_ADDRESS_93        = 93,
		RFM69HCW_NETWORK_ADDRESS_94        = 94,
		RFM69HCW_NETWORK_ADDRESS_95        = 95,
		RFM69HCW_NETWORK_ADDRESS_96        = 96,
		RFM69HCW_NETWORK_ADDRESS_97        = 97,
		RFM69HCW_NETWORK_ADDRESS_98        = 98,
		RFM69HCW_NETWORK_ADDRESS_99        = 99,
		RFM69HCW_NETWORK_ADDRESS_100       = 100,
		RFM69HCW_NETWORK_ADDRESS_101       = 101,
		RFM69HCW_NETWORK_ADDRESS_102       = 102,
		RFM69HCW_NETWORK_ADDRESS_103       = 103,
		RFM69HCW_NETWORK_ADDRESS_104       = 104,
		RFM69HCW_NETWORK_ADDRESS_105       = 105,
		RFM69HCW_NETWORK_ADDRESS_106       = 106,
		RFM69HCW_NETWORK_ADDRESS_107       = 107,
		RFM69HCW_NETWORK_ADDRESS_108       = 108,
		RFM69HCW_NETWORK_ADDRESS_109       = 109,
		RFM69HCW_NETWORK_ADDRESS_110       = 110,
		RFM69HCW_NETWORK_ADDRESS_111       = 111,
		RFM69HCW_NETWORK_ADDRESS_112       = 112,
		RFM69HCW_NETWORK_ADDRESS_113       = 113,
		RFM69HCW_NETWORK_ADDRESS_114       = 114,
		RFM69HCW_NETWORK_ADDRESS_115       = 115,
		RFM69HCW_NETWORK_ADDRESS_116       = 116,
		RFM69HCW_NETWORK_ADDRESS_117       = 117,
		RFM69HCW_NETWORK_ADDRESS_118       = 118,
		RFM69HCW_NETWORK_ADDRESS_119       = 119,
		RFM69HCW_NETWORK_ADDRESS_120       = 120,
		RFM69HCW_NETWORK_ADDRESS_121       = 121,
		RFM69HCW_NETWORK_ADDRESS_122       = 122,
		RFM69HCW_NETWORK_ADDRESS_123       = 123,
		RFM69HCW_NETWORK_ADDRESS_124       = 124,
		RFM69HCW_NETWORK_ADDRESS_125       = 125,
		RFM69HCW_NETWORK_ADDRESS_126       = 126,
		RFM69HCW_NETWORK_ADDRESS_127       = 127,
		RFM69HCW_NETWORK_ADDRESS_128       = 128,
		RFM69HCW_NETWORK_ADDRESS_129       = 129,
		RFM69HCW_NETWORK_ADDRESS_130       = 130,
		RFM69HCW_NETWORK_ADDRESS_131       = 131,
		RFM69HCW_NETWORK_ADDRESS_132       = 132,
		RFM69HCW_NETWORK_ADDRESS_133       = 133,
		RFM69HCW_NETWORK_ADDRESS_134       = 134,
		RFM69HCW_NETWORK_ADDRESS_135       = 135,
		RFM69HCW_NETWORK_ADDRESS_136       = 136,
		RFM69HCW_NETWORK_ADDRESS_137       = 137,
		RFM69HCW_NETWORK_ADDRESS_138       = 138,
		RFM69HCW_NETWORK_ADDRESS_139       = 139,
		RFM69HCW_NETWORK_ADDRESS_140       = 140,
		RFM69HCW_NETWORK_ADDRESS_141       = 141,
		RFM69HCW_NETWORK_ADDRESS_142       = 142,
		RFM69HCW_NETWORK_ADDRESS_143       = 143,
		RFM69HCW_NETWORK_ADDRESS_144       = 144,
		RFM69HCW_NETWORK_ADDRESS_145       = 145,
		RFM69HCW_NETWORK_ADDRESS_146       = 146,
		RFM69HCW_NETWORK_ADDRESS_147       = 147,
		RFM69HCW_NETWORK_ADDRESS_148       = 148,
		RFM69HCW_NETWORK_ADDRESS_149       = 149,
		RFM69HCW_NETWORK_ADDRESS_150       = 150,
		RFM69HCW_NETWORK_ADDRESS_151       = 151,
		RFM69HCW_NETWORK_ADDRESS_152       = 152,
		RFM69HCW_NETWORK_ADDRESS_153       = 153,
		RFM69HCW_NETWORK_ADDRESS_154       = 154,
		RFM69HCW_NETWORK_ADDRESS_155       = 155,
		RFM69HCW_NETWORK_ADDRESS_156       = 156,
		RFM69HCW_NETWORK_ADDRESS_157       = 157,
		RFM69HCW_NETWORK_ADDRESS_158       = 158,
		RFM69HCW_NETWORK_ADDRESS_159       = 159,
		RFM69HCW_NETWORK_ADDRESS_160       = 160,
		RFM69HCW_NETWORK_ADDRESS_161       = 161,
		RFM69HCW_NETWORK_ADDRESS_162       = 162,
		RFM69HCW_NETWORK_ADDRESS_163       = 163,
		RFM69HCW_NETWORK_ADDRESS_164       = 164,
		RFM69HCW_NETWORK_ADDRESS_165       = 165,
		RFM69HCW_NETWORK_ADDRESS_166       = 166,
		RFM69HCW_NETWORK_ADDRESS_167       = 167,
		RFM69HCW_NETWORK_ADDRESS_168       = 168,
		RFM69HCW_NETWORK_ADDRESS_169       = 169,
		RFM69HCW_NETWORK_ADDRESS_170       = 170,
		RFM69HCW_NETWORK_ADDRESS_171       = 171,
		RFM69HCW_NETWORK_ADDRESS_172       = 172,
		RFM69HCW_NETWORK_ADDRESS_173       = 173,
		RFM69HCW_NETWORK_ADDRESS_174       = 174,
		RFM69HCW_NETWORK_ADDRESS_175       = 175,
		RFM69HCW_NETWORK_ADDRESS_176       = 176,
		RFM69HCW_NETWORK_ADDRESS_177       = 177,
		RFM69HCW_NETWORK_ADDRESS_178       = 178,
		RFM69HCW_NETWORK_ADDRESS_179       = 179,
		RFM69HCW_NETWORK_ADDRESS_180       = 180,
		RFM69HCW_NETWORK_ADDRESS_181       = 181,
		RFM69HCW_NETWORK_ADDRESS_182       = 182,
		RFM69HCW_NETWORK_ADDRESS_183       = 183,
		RFM69HCW_NETWORK_ADDRESS_184       = 184,
		RFM69HCW_NETWORK_ADDRESS_185       = 185,
		RFM69HCW_NETWORK_ADDRESS_186       = 186,
		RFM69HCW_NETWORK_ADDRESS_187       = 187,
		RFM69HCW_NETWORK_ADDRESS_188       = 188,
		RFM69HCW_NETWORK_ADDRESS_189       = 189,
		RFM69HCW_NETWORK_ADDRESS_190       = 190,
		RFM69HCW_NETWORK_ADDRESS_191       = 191,
		RFM69HCW_NETWORK_ADDRESS_192       = 192,
		RFM69HCW_NETWORK_ADDRESS_193       = 193,
		RFM69HCW_NETWORK_ADDRESS_194       = 194,
		RFM69HCW_NETWORK_ADDRESS_195       = 195,
		RFM69HCW_NETWORK_ADDRESS_196       = 196,
		RFM69HCW_NETWORK_ADDRESS_197       = 197,
		RFM69HCW_NETWORK_ADDRESS_198       = 198,
		RFM69HCW_NETWORK_ADDRESS_199       = 199,
		RFM69HCW_NETWORK_ADDRESS_200       = 200,
		RFM69HCW_NETWORK_ADDRESS_201       = 201,
		RFM69HCW_NETWORK_ADDRESS_202       = 202,
		RFM69HCW_NETWORK_ADDRESS_203       = 203,
		RFM69HCW_NETWORK_ADDRESS_204       = 204,
		RFM69HCW_NETWORK_ADDRESS_205       = 205,
		RFM69HCW_NETWORK_ADDRESS_206       = 206,
		RFM69HCW_NETWORK_ADDRESS_207       = 207,
		RFM69HCW_NETWORK_ADDRESS_208       = 208,
		RFM69HCW_NETWORK_ADDRESS_209       = 209,
		RFM69HCW_NETWORK_ADDRESS_210       = 210,
		RFM69HCW_NETWORK_ADDRESS_211       = 211,
		RFM69HCW_NETWORK_ADDRESS_212       = 212,
		RFM69HCW_NETWORK_ADDRESS_213       = 213,
		RFM69HCW_NETWORK_ADDRESS_214       = 214,
		RFM69HCW_NETWORK_ADDRESS_215       = 215,
		RFM69HCW_NETWORK_ADDRESS_216       = 216,
		RFM69HCW_NETWORK_ADDRESS_217       = 217,
		RFM69HCW_NETWORK_ADDRESS_218       = 218,
		RFM69HCW_NETWORK_ADDRESS_219       = 219,
		RFM69HCW_NETWORK_ADDRESS_220       = 220,
		RFM69HCW_NETWORK_ADDRESS_221       = 221,
		RFM69HCW_NETWORK_ADDRESS_222       = 222,
		RFM69HCW_NETWORK_ADDRESS_223       = 223,
		RFM69HCW_NETWORK_ADDRESS_224       = 224,
		RFM69HCW_NETWORK_ADDRESS_225       = 225,
		RFM69HCW_NETWORK_ADDRESS_226       = 226,
		RFM69HCW_NETWORK_ADDRESS_227       = 227,
		RFM69HCW_NETWORK_ADDRESS_228       = 228,
		RFM69HCW_NETWORK_ADDRESS_229       = 229,
		RFM69HCW_NETWORK_ADDRESS_230       = 230,
		RFM69HCW_NETWORK_ADDRESS_231       = 231,
		RFM69HCW_NETWORK_ADDRESS_232       = 232,
		RFM69HCW_NETWORK_ADDRESS_233       = 233,
		RFM69HCW_NETWORK_ADDRESS_234       = 234,
		RFM69HCW_NETWORK_ADDRESS_235       = 235,
		RFM69HCW_NETWORK_ADDRESS_236       = 236,
		RFM69HCW_NETWORK_ADDRESS_237       = 237,
		RFM69HCW_NETWORK_ADDRESS_238       = 238,
		RFM69HCW_NETWORK_ADDRESS_239       = 239,
		RFM69HCW_NETWORK_ADDRESS_240       = 240,
		RFM69HCW_NETWORK_ADDRESS_241       = 241,
		RFM69HCW_NETWORK_ADDRESS_242       = 242,
		RFM69HCW_NETWORK_ADDRESS_243       = 243,
		RFM69HCW_NETWORK_ADDRESS_244       = 244,
		RFM69HCW_NETWORK_ADDRESS_245       = 245,
		RFM69HCW_NETWORK_ADDRESS_246       = 246,
		RFM69HCW_NETWORK_ADDRESS_247       = 247,
		RFM69HCW_NETWORK_ADDRESS_248       = 248,
		RFM69HCW_NETWORK_ADDRESS_249       = 249,
		RFM69HCW_NETWORK_ADDRESS_250       = 250,
		RFM69HCW_NETWORK_ADDRESS_251       = 251,
		RFM69HCW_NETWORK_ADDRESS_252       = 252,
		RFM69HCW_NETWORK_ADDRESS_253       = 253,
		RFM69HCW_NETWORK_ADDRESS_254       = 254,
		RFM69HCW_NETWORK_ADDRESS_BROADCAST = 255
	};

	typedef uint32 RFM69HCWFrequency;

	enum RFM69HCW_FREQUENCIES : RFM69HCWFrequency
	{
		RFM69HCW_FREQUENCY_MINIMUM = 902000000,
		RFM69HCW_FREQUENCY_MAXIMUM = 928000000,
		RFM69HCW_FREQUENCY_DEFAULT = 915000000
	};

	typedef Collections::Array<uint8[16]> RFM69HCWEncryptionKey;

	// @throw AL::Exception
	// @return False to stop receiving
	typedef Function<Bool(RFM69HCWNetworkAddress source, Void* lpBuffer, size_t size, size_t numberOfBytesReceived)> RFM69HCWRXCallback;

	// TODO: Debug this
	class RFM69HCW
		: public IDriver<Void, Void, Void>
	{
		enum _FRAME_HEADER_FLAGS : uint8
		{
			_FRAME_HEADER_FLAG_NONE             = 0x00,

			_FRAME_HEADER_FLAG_ACK              = 0x01,
			_FRAME_HEADER_FLAG_REQUIRE_ACK      = 0x02,
			_FRAME_HEADER_FLAG_EXTENDED_PAYLOAD = 0x04
		};

#pragma pack(push, 1)
		struct _FrameHeader
		{
			uint8               Size;
			uint8               Destination;
			uint8               Source;
			_FRAME_HEADER_FLAGS Flags;
		};
#pragma pack(pop)

		enum class _Modes : uint8
		{
			Sleep                = 0b000,
			Standby              = 0b001,
			Receiver             = 0b100,
			Transmitter          = 0b011,
			FrequencySynthesizer = 0b010
		};

		enum class _Registers : uint8
		{
			RegFifo          = 0x00,
			RegOpMode        = 0x01,
			RegDataModul     = 0x02,
			RegBitrateMsb    = 0x03,
			RegBitrateLsb    = 0x04,
			RegFdevMsb       = 0x05,
			RegFdevLsb       = 0x06,
			RegFrfMsb        = 0x07,
			RegFrfMid        = 0x08,
			RegFrfLsb        = 0x09,
			RegOsc1          = 0x0A,
			RegAfcCtrl       = 0x0B,
			RegReserved0C    = 0x0C,
			RegListen1       = 0x0D,
			RegListen2       = 0x0E,
			RegListen3       = 0x0F,
			RegVersion       = 0x10,
			RegPaLevel       = 0x11,
			RegPaRamp        = 0x12,
			RegOcp           = 0x13,
			Reserved14       = 0x14,
			Reserved15       = 0x15,
			Reserved16       = 0x16,
			Reserved17       = 0x17,
			RegLna           = 0x18,
			RegRxBw          = 0x19,
			RegAfcBw         = 0x1A,
			RegOokPeak       = 0x1B,
			RegOokAvg        = 0x1C,
			RegOokFix        = 0x1D,
			RegAfcFei        = 0x1E,
			RegAfcMsb        = 0x1F,
			RegAfcLsb        = 0x20,
			RegFeiMsb        = 0x21,
			RegFeiLsb        = 0x22,
			RegRssiConfig    = 0x23,
			RegRssiValue     = 0x24,
			RegDioMapping1   = 0x25,
			RegDioMapping2   = 0x26,
			RegIrqFlags1     = 0x27,
			RegIrqFlags2     = 0x28,
			RegRssiThresh    = 0x29,
			RegRxTimeout1    = 0x2A,
			RegRxTimeout2    = 0x2B,
			RegPreambleMsb   = 0x2C,
			RegPreambleLsb   = 0x2D,
			RegSyncConfig    = 0x2E,
			RegSyncValue1    = 0x2F,
			RegSyncValue2    = 0x30,
			RegSyncValue3    = 0x31,
			RegSyncValue4    = 0x32,
			RegSyncValue5    = 0x33,
			RegSyncValue6    = 0x34,
			RegSyncValue7    = 0x35,
			RegSyncValue8    = 0x36,
			RegPacketConfig1 = 0x37,
			RegPayloadLength = 0x38,
			RegNodeAdrs      = 0x39,
			RegBroadcastAdrs = 0x3A,
			RegAutoModes     = 0x3B,
			RegFifoThresh    = 0x3C,
			RegPacketConfig2 = 0x3D,
			RegAesKey1       = 0x3E,
			RegAesKey2       = 0x3F,
			RegAesKey3       = 0x40,
			RegAesKey4       = 0x41,
			RegAesKey5       = 0x42,
			RegAesKey6       = 0x43,
			RegAesKey7       = 0x44,
			RegAesKey8       = 0x45,
			RegAesKey9       = 0x46,
			RegAesKey10      = 0x47,
			RegAesKey11      = 0x48,
			RegAesKey12      = 0x49,
			RegAesKey13      = 0x4A,
			RegAesKey14      = 0x4B,
			RegAesKey15      = 0x4C,
			RegAesKey16      = 0x4D,
			RegTemp1         = 0x4E,
			RegTemp2         = 0x4F,
			RegTestLna       = 0x58,
			RegTestPa1       = 0x5A,
			RegTestPa2       = 0x5C,
			RegTestDagc      = 0x6F,
			RegTestAfc       = 0x71,
			RegTest          = 0x50
		};

		static constexpr Double _FREQUENCY_STEP          = 61.03515625;

		Bool                     isLnaEnabled        = False;
		Bool                     isEncryptionEnabled = False;

		SPIDevice                device;

		int8                     rssi             = -127;

		RFM69HCWLnaGains         lnaGain          = RFM69HCWLnaGains::G1;
		RFM69HCWLnaImpedances    lnaImpedance     = RFM69HCWLnaImpedances::OHM_200;

		RFM69HCWOutputPowerModes outputPowerMode  = RFM69HCWOutputPowerModes::PA_0;
		RFM69HCWOutputPowerLevel outputPowerLevel = RFM69HCW_OUTPUT_POWER_LEVEL_MINIMUM;

		RFM69HCWNetwork          network;
		RFM69HCWNetworkAddress   networkAddress;

		RFM69HCWFrequency        frequency;

		RFM69HCWEncryptionKey    encryptionKey;

		uint16                   preambleLength   = 3;

	public:
		typedef RFM69HCWLnaGains         LnaGains;
		typedef RFM69HCWLnaImpedances    LnaImpedances;

		typedef RFM69HCWOutputPowerLevel OutputPowerLevel;
		typedef RFM69HCWOutputPowerModes OutputPowerModes;

		typedef RFM69HCWNetwork          Network;
		typedef RFM69HCWNetworkAddress   NetworkAddress;

		typedef RFM69HCWFrequency        Frequency;

		typedef RFM69HCWEncryptionKey    EncryptionKey;

		typedef RFM69HCWRXCallback       RXCallback;

		static constexpr SPISpeed DEFAULT_SPEED           = 10000000;

		static constexpr size_t   PACKET_OVERHEAD         = sizeof(_FrameHeader) - 2;

		static constexpr size_t   PACKET_SIZE_MAXIMUM     = 255 - sizeof(_FrameHeader);
		static constexpr size_t   PACKET_SIZE_MAXIMUM_AES = 64 - sizeof(_FrameHeader);

		RFM69HCW(RFM69HCW&& rfm69hcw)
			: isLnaEnabled(
				rfm69hcw.isLnaEnabled
			),
			isEncryptionEnabled(
				rfm69hcw.isEncryptionEnabled
			),
			device(
				Move(rfm69hcw.device)
			),
			rssi(
				rfm69hcw.rssi
			),
			lnaGain(
				rfm69hcw.lnaGain
			),
			lnaImpedance(
				rfm69hcw.lnaImpedance
			),
			outputPowerMode(
				rfm69hcw.outputPowerMode
			),
			outputPowerLevel(
				rfm69hcw.outputPowerLevel
			),
			network(
				rfm69hcw.network
			),
			networkAddress(
				rfm69hcw.networkAddress
			),
			frequency(
				rfm69hcw.frequency
			),
			encryptionKey(
				Move(rfm69hcw.encryptionKey)
			),
			preambleLength(
				rfm69hcw.preambleLength
			)
		{
			rfm69hcw.isLnaEnabled = False;
			rfm69hcw.isEncryptionEnabled = False;

			rfm69hcw.rssi = -127;
			rfm69hcw.lnaGain = LnaGains::G1;
			rfm69hcw.lnaImpedance = LnaImpedances::OHM_200;
			rfm69hcw.outputPowerMode = OutputPowerModes::PA_0;
			rfm69hcw.outputPowerLevel = RFM69HCW_OUTPUT_POWER_LEVEL_MINIMUM;
			rfm69hcw.preambleLength = 3;
		}

#if defined(AL_PLATFORM_PICO)
		RFM69HCW(::spi_inst_t* spi, GPIOPin miso, GPIOPin mosi, GPIOPin sclk, GPIOPin cs, Network network, NetworkAddress networkAddress, Frequency frequency = RFM69HCW_FREQUENCY_DEFAULT, SPISpeed speed = DEFAULT_SPEED)
			: device(
				spi,
				miso,
				mosi,
				sclk,
				cs,
				SPIModes::Zero,
				speed,
				8
			),
			network(
				network
			),
			networkAddress(
				networkAddress
			),
			frequency(
				frequency
			)
		{
		}
#else
		RFM69HCW(FileSystem::Path&& path, Network network, NetworkAddress networkAddress, Frequency frequency = RFM69HCW_FREQUENCY_DEFAULT, SPISpeed speed = DEFAULT_SPEED)
			: device(
				Move(path),
				SPIModes::Zero,
				speed,
				8
			),
			network(
				network
			),
			networkAddress(
				networkAddress
			),
			frequency(
				frequency
			)
		{
		}
		RFM69HCW(const FileSystem::Path& path, Network network, NetworkAddress networkAddress, Frequency frequency = RFM69HCW_FREQUENCY_DEFAULT, SPISpeed speed = DEFAULT_SPEED)
			: RFM69HCW(
				FileSystem::Path(
					path
				),
				network,
				networkAddress,
				frequency,
				speed
			)
		{
		}
#endif

		virtual ~RFM69HCW()
		{
		}

		virtual Bool IsOpen() const override
		{
			return device.IsOpen();
		}

		Bool IsLnaEnabled() const
		{
			return isLnaEnabled;
		}

		Bool IsEncryptionEnabled() const
		{
			return isEncryptionEnabled;
		}

		auto GetRSSI() const
		{
			return rssi;
		}

		auto GetNetwork() const
		{
			return network;
		}

		auto GetNetworkAddress() const
		{
			return networkAddress;
		}

		auto GetFrequency() const
		{
			return frequency;
		}

		auto GetLnaGain() const
		{
			return lnaGain;
		}

		auto GetLnaImpedance() const
		{
			return lnaImpedance;
		}

		auto GetOutputPowerMode() const
		{
			return outputPowerMode;
		}

		auto GetOutputPowerLevel() const
		{
			return outputPowerLevel;
		}

		auto GetPreambleLength() const
		{
			return preambleLength;
		}

		// @throw AL::Exception
		virtual Void Open() override
		{
			try
			{
				device.Open();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error opening SPIDevice"
				);
			}

			try
			{
				while (!Wait(_Registers::RegIrqFlags1, 0x80, 0x00, TimeSpan::Infinite))
				{
				}
			}
			catch (Exception& exception)
			{
				device.Close();

				throw Exception(
					Move(exception),
					"Error waiting for ModeReady"
				);
			}

			try
			{
				WriteConfig();
			}
			catch (Exception& exception)
			{
				device.Close();

				throw Exception(
					Move(exception),
					"Error writing config"
				);
			}
		}

		virtual Void Close() override
		{
			if (IsOpen())
			{

				device.Close();
			}
		}

		// @throw AL::Exception
		Void EnableLna(Bool set = True)
		{
			if (IsOpen())
			{
				WriteLnaConfig(
					set,
					GetLnaGain(),
					GetLnaImpedance()
				);
			}

			isLnaEnabled = set;
		}

		// @throw AL::Exception
		Void EnableEncryption(Bool set = True)
		{
			if (IsOpen())
			{
				uint8 value;

				ReadRegister(
					_Registers::RegPacketConfig2,
					value
				);

				value &= 0xFE;
				value |= set ? 0x01 : 0x00;

				WriteRegister(
					_Registers::RegPacketConfig2,
					value
				);
			}

			isEncryptionEnabled = set;
		}

		// @throw AL::Exception
		Void SetFrequency(Frequency value)
		{
			if (value < RFM69HCW_FREQUENCY_MINIMUM)
			{

				value = RFM69HCW_FREQUENCY_MINIMUM;
			}
			else if (value > RFM69HCW_FREQUENCY_MAXIMUM)
			{

				value = RFM69HCW_FREQUENCY_MAXIMUM;
			}

			if (IsOpen())
			{
				WriteFrequency(
					value
				);
			}

			frequency = value;
		}

		// @throw AL::Exception
		Void SetLnaGain(LnaGains value)
		{
			if (IsOpen())
			{
				WriteLnaConfig(
					IsLnaEnabled(),
					value,
					GetLnaImpedance()
				);
			}

			lnaGain = value;
		}

		// @throw AL::Exception
		Void SetLnaImpedance(LnaImpedances value)
		{
			if (IsOpen())
			{
				WriteLnaConfig(
					IsLnaEnabled(),
					GetLnaGain(),
					value
				);
			}

			lnaImpedance = value;
		}

		// @throw AL::Exception
		Void SetOutputPowerMode(OutputPowerModes value)
		{
			if (IsOpen())
			{
				WriteOutputPower(
					value,
					GetOutputPowerLevel()
				);
			}

			outputPowerMode = value;
		}

		// @throw AL::Exception
		Void SetOutputPowerLevel(OutputPowerLevel value)
		{
			// TODO: debug values under 20 with power modes

			if (IsOpen())
			{
				WriteOutputPower(
					GetOutputPowerMode(),
					value
				);
			}

			outputPowerLevel = value;
		}

		// @throw AL::Exception
		Void SetEncryptionKey(const EncryptionKey& value)
		{
			if (IsOpen())
			{
				WriteRegister(
					_Registers::RegAesKey1,
					&value[0],
					value.GetSize()
				);
			}

			encryptionKey = value;
		}

		// @throw AL::Exception
		Void SetPreambleLength(uint16 value)
		{
			if (IsOpen())
			{
				WriteRegister(
					_Registers::RegPreambleMsb,
					static_cast<uint8>((value & 0xFF) << 8)
				);

				WriteRegister(
					_Registers::RegPreambleLsb,
					static_cast<uint8>(value & 0xFF)
				);
			}

			preambleLength = value;
		}

		// @throw AL::Exception
		Void   RX(const RXCallback& callback, Void* lpBuffer, size_t size)
		{
			AL_ASSERT(
				IsOpen(),
				"RFM69HCW not open"
			);

			try
			{
				RX_Begin();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error beginning RX"
				);
			}

			NetworkAddress source;
			size_t         numberOfBytesReceived;

		try_rx_once:
			try
			{
				numberOfBytesReceived = RX_Frames(
					source,
					lpBuffer,
					size,
					TimeSpan::Infinite
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading frames"
				);
			}

			if (numberOfBytesReceived != 0)
			{
				try
				{
					if (callback(source, lpBuffer, size, numberOfBytesReceived))
					{

						goto try_rx_once;
					}
				}
				catch (Exception&)
				{
					try
					{
						RX_End();
					}
					catch (const Exception&)
					{
					}

					throw;
				}
			}

			try
			{
				RX_End();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error ending RX"
				);
			}
		}
		// @throw AL::Exception
		// @return number of bytes received
		size_t RX(NetworkAddress& source, Void* lpBuffer, size_t size)
		{
			auto numberOfBytesReceived = RX(
				source,
				lpBuffer,
				size,
				TimeSpan::Infinite
			);

			return numberOfBytesReceived;
		}
		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes received
		size_t RX(NetworkAddress& source, Void* lpBuffer, size_t size, TimeSpan timeout)
		{
			AL_ASSERT(
				IsOpen(),
				"RFM69HCW not open"
			);

			try
			{
				RX_Begin();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error beginning RX"
				);
			}

			size_t numberOfBytesReceived;

			try
			{
				numberOfBytesReceived = RX_Frames(
					source,
					lpBuffer,
					size,
					timeout
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error receiving frames"
				);
			}

			try
			{
				RX_End();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error ending RX"
				);
			}

			return numberOfBytesReceived;
		}

		// @throw AL::Exception
		// @return number of bytes transmitted
		size_t TX(NetworkAddress destination, const Void* lpBuffer, size_t size)
		{
			auto numberOfBytesTransmitted = TX(
				destination,
				lpBuffer,
				size,
				TimeSpan::Zero
			);

			return numberOfBytesTransmitted;
		}
		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes transmitted
		size_t TX(NetworkAddress destination, const Void* lpBuffer, size_t size, TimeSpan timeout)
		{
			AL_ASSERT(
				IsOpen(),
				"RFM69HCW not open"
			);

			try
			{
				TX_Begin();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error beginning TX"
				);
			}

			size_t numberOfBytesTransmitted;

			try
			{
				numberOfBytesTransmitted = TX_Frames(
					destination,
					lpBuffer,
					size,
					timeout
				);
			}
			catch (Exception& exception)
			{
				try
				{
					TX_End();
				}
				catch (const Exception&)
				{
				}

				throw Exception(
					Move(exception),
					"Error writing frames"
				);
			}

			try
			{
				TX_End();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error ending TX"
				);
			}

			return numberOfBytesTransmitted;
		}

		// @throw AL::Exception
		// @return number of bytes transmitted
		size_t Broadcast(const Void* lpBuffer, size_t size)
		{
			auto numberOfBytesTransmitted = TX(
				RFM69HCW_NETWORK_ADDRESS_BROADCAST,
				lpBuffer,
				size
			);

			return numberOfBytesTransmitted;
		}

		RFM69HCW& operator = (RFM69HCW&& rfm69hcw)
		{
			isLnaEnabled = rfm69hcw.isLnaEnabled;
			rfm69hcw.isLnaEnabled = False;

			isEncryptionEnabled = rfm69hcw.isEncryptionEnabled;
			rfm69hcw.isEncryptionEnabled = False;

			device = Move(
				rfm69hcw.device
			);

			rssi = rfm69hcw.rssi;
			rfm69hcw.rssi = -127;

			lnaGain = rfm69hcw.lnaGain;
			rfm69hcw.lnaGain = LnaGains::G1;

			lnaImpedance = rfm69hcw.lnaImpedance;
			rfm69hcw.lnaImpedance = LnaImpedances::OHM_200;

			outputPowerMode = rfm69hcw.outputPowerMode;
			rfm69hcw.outputPowerMode = OutputPowerModes::PA_0;

			outputPowerLevel = rfm69hcw.outputPowerLevel;
			rfm69hcw.outputPowerLevel = RFM69HCW_OUTPUT_POWER_LEVEL_MINIMUM;

			network = rfm69hcw.network;
			networkAddress = rfm69hcw.networkAddress;

			frequency = rfm69hcw.frequency;

			encryptionKey = Move(
				rfm69hcw.encryptionKey
			);

			preambleLength = rfm69hcw.preambleLength;
			rfm69hcw.preambleLength = 3;

			return *this;
		}

		Bool operator == (const RFM69HCW& rfm69hcw) const
		{
			if (GetNetwork() != rfm69hcw.GetNetwork())
			{

				return False;
			}

			if (GetNetworkAddress() != rfm69hcw.GetNetworkAddress())
			{

				return False;
			}

			if (GetFrequency() != rfm69hcw.GetFrequency())
			{

				return False;
			}

			if (device != rfm69hcw.device)
			{

				return False;
			}

			return True;
		}
		Bool operator != (const RFM69HCW& rfm69hcw) const
		{
			if (operator==(rfm69hcw))
			{

				return False;
			}

			return True;
		}

	private:
		// @throw AL::Exception
		Void RX_Begin()
		{
			try
			{
				WriteMode(
					_Modes::Receiver
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing mode (Receiver)"
				);
			}
		}

		// @throw AL::Exception
		Void RX_End()
		{
			try
			{
				WriteMode(
					_Modes::Standby
				);
			}
			catch (Exception& exception)
			{
				try
				{
					FIFO_Clear();
				}
				catch (const Exception&)
				{
				}

				throw Exception(
					Move(exception),
					"Error writing mode (Standby)"
				);
			}

			try
			{
				FIFO_Clear();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error clearing FIFO"
				);
			}
		}

		// @throw AL::Exception
		// @return False on timeout
		Bool RX_Frame(_FrameHeader& header, Void* lpPayload, size_t payloadCapacity, TimeSpan timeout)
		{
			Bool isPayloadReady;

			try
			{
				isPayloadReady = Wait_PayloadReady(
					timeout
				);
			}
			catch (Exception& exception)
			{
				try
				{
					FIFO_Clear();
				}
				catch (const Exception&)
				{
				}

				throw Exception(
					Move(exception),
					"Error waiting for Payload"
				);
			}

			if (!isPayloadReady)
			{
				try
				{
					FIFO_Clear();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error clearing FIFO"
					);
				}

				return False;
			}

			try
			{
				ReadRSSI(
					rssi
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading RSSI"
				);
			}

			try
			{
				FIFO_Read(
					&header,
					sizeof(_FrameHeader)
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error reading FIFO (_FrameHeader)"
				);
			}

			uint8 payloadSize;

			if ((payloadSize = (header.Size - 3)) > 0)
			{
				if (payloadSize > payloadCapacity)
				{

					header.Size = static_cast<uint8>(
						payloadCapacity + 3
					);
				}

				try
				{
					FIFO_Read(
						lpPayload,
						payloadSize
					);
				}
				catch (Exception& exception)
				{
					try
					{
						FIFO_Clear();
					}
					catch (const Exception&)
					{
					}

					throw Exception(
						Move(exception),
						"Error reading FIFO (Payload)"
					);
				}

				try
				{
					FIFO_Clear();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error clearing FIFO"
					);
				}
			}

			if ((header.Flags & _FRAME_HEADER_FLAG_REQUIRE_ACK) == _FRAME_HEADER_FLAG_REQUIRE_ACK)
			{
				_FrameHeader ackHeader =
				{
					.Size        = 3,
					.Destination = header.Source,
					.Source      = GetNetworkAddress(),
					.Flags       = _FRAME_HEADER_FLAG_ACK
				};

				try
				{
					TX_Begin();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error beginning TX"
					);
				}

				try
				{
					TX_Frame(
						ackHeader,
						nullptr,
						0,
						TimeSpan::Zero
					);
				}
				catch (Exception& exception)
				{
					try
					{
						TX_End();
					}
					catch (const Exception&)
					{
					}

					throw Exception(
						Move(exception),
						"Error transmitting _FrameHeader"
					);
				}

				try
				{
					TX_End();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error ending TX"
					);
				}
			}

			return True;
		}
		// @throw AL::Exception
		// @return number of bytes received
		size_t RX_Frame(NetworkAddress& source, Void* lpPayload, size_t payloadCapacity, TimeSpan timeout, Bool& isExtendedPayload)
		{
			_FrameHeader header;

			if (!RX_Frame(header, lpPayload, payloadCapacity, timeout))
			{

				return 0;
			}

			source            = header.Source;
			isExtendedPayload = (header.Flags & _FRAME_HEADER_FLAG_EXTENDED_PAYLOAD) == _FRAME_HEADER_FLAG_EXTENDED_PAYLOAD;

			return header.Size - 3;
		}

		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes received
		size_t RX_Frames(NetworkAddress& source, Void* lpPayload, size_t payloadCapacity, TimeSpan timeout)
		{
			Bool   isExtendedPayload;
			size_t totalBytesReceived = 0;
			size_t numberOfBytesReceived;

			auto lpPayloadBuffer = reinterpret_cast<uint8*>(
				lpPayload
			);

			do
			{
				if ((numberOfBytesReceived = RX_Frame(source, lpPayloadBuffer, payloadCapacity - totalBytesReceived, timeout, isExtendedPayload)) == 0)
				{

					return 0;
				}

				lpPayloadBuffer    += numberOfBytesReceived;
				totalBytesReceived += numberOfBytesReceived;
			} while (isExtendedPayload);

			return totalBytesReceived;
		}

		// @throw AL::Exception
		Void TX_Begin()
		{
			try
			{
				RestartRX();
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error restarting RX"
				);
			}
		}

		// @throw AL::Exception
		Void TX_End()
		{
		}

		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes transmitted
		size_t TX_Frame(const _FrameHeader& header, const Void* lpPayload, size_t payloadSize, TimeSpan timeout)
		{
			try
			{
				FIFO_Write(
					&header,
					sizeof(_FrameHeader)
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing FIFO (_FrameHeader)"
				);
			}

			if (lpPayload && payloadSize)
			{
				try
				{
					FIFO_Write(
						lpPayload,
						payloadSize
					);
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error writing FIFO (Payload)"
					);
				}
			}

			try
			{
				WriteMode(
					_Modes::Transmitter
				);
			}
			catch (Exception& exception)
			{
				try
				{
					FIFO_Clear();
				}
				catch (const Exception&)
				{
				}

				throw Exception(
					Move(exception),
					"Error writing mode (Transmitter)"
				);
			}

			try
			{
				FIFO_Flush();
			}
			catch (Exception& exception)
			{
				try
				{
					WriteMode(
						_Modes::Standby
					);
				}
				catch (const Exception&)
				{
				}

				throw Exception(
					Move(exception),
					"Error flushing FIFO"
				);
			}

			try
			{
				WriteMode(
					_Modes::Standby
				);
			}
			catch (Exception& exception)
			{

				throw Exception(
					Move(exception),
					"Error writing mode (Standby)"
				);
			}

			if (timeout != TimeSpan::Zero)
			{
				try
				{
					RX_Begin();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error beginning RX"
					);
				}

				_FrameHeader ackHeader;
				Bool         isAckHeaderReceived;

				try
				{
					isAckHeaderReceived = RX_Frame(
						ackHeader,
						nullptr,
						0,
						timeout
					);
				}
				catch (Exception& exception)
				{
					try
					{
						RX_End();
					}
					catch (const Exception&)
					{
					}

					throw Exception(
						Move(exception),
						"Error receiving _FrameHeader"
					);
				}

				try
				{
					RX_End();
				}
				catch (Exception& exception)
				{

					throw Exception(
						Move(exception),
						"Error ending RX"
					);
				}

				if (!isAckHeaderReceived || ((ackHeader.Flags & _FRAME_HEADER_FLAG_ACK) != _FRAME_HEADER_FLAG_ACK))
				{

					return 0;
				}
			}

			return payloadSize;
		}
		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes transmitted
		size_t TX_Frame(NetworkAddress destination, const Void* lpPayload, size_t payloadSize, TimeSpan timeout)
		{
			size_t maxPayloadSize;
			Bool   isExtendedPayload = False;

			if (payloadSize > (maxPayloadSize = (IsEncryptionEnabled() ? PACKET_SIZE_MAXIMUM_AES : PACKET_SIZE_MAXIMUM)))
			{

				payloadSize = maxPayloadSize;
			}

			_FrameHeader header =
			{
				.Size        = static_cast<decltype(_FrameHeader::Size)>(3 + payloadSize),
				.Destination = destination,
				.Source      = GetNetworkAddress(),
				.Flags       = _FRAME_HEADER_FLAGS(
					(isExtendedPayload ? _FRAME_HEADER_FLAG_EXTENDED_PAYLOAD : _FRAME_HEADER_FLAG_NONE) |
					((timeout == TimeSpan::Zero) ? _FRAME_HEADER_FLAG_NONE : _FRAME_HEADER_FLAG_REQUIRE_ACK)
				)
			};

			auto numberOfBytesTransmitted = TX_Frame(
				header,
				lpPayload,
				payloadSize,
				timeout
			);

			return numberOfBytesTransmitted;
		}

		// @throw AL::Exception
		// @return 0 on timeout
		// @return number of bytes transmitted
		size_t TX_Frames(NetworkAddress destination, const Void* lpPayload, size_t payloadSize, TimeSpan timeout)
		{
			size_t totalBytesTransmitted = 0;
			size_t numberOfBytesTransmitted;

			auto lpPayloadBuffer = reinterpret_cast<const uint8*>(
				lpPayload
			);

			do
			{
				if ((numberOfBytesTransmitted = TX_Frame(destination, lpPayloadBuffer, payloadSize - totalBytesTransmitted, timeout)) == 0)
				{

					return 0;
				}

				lpPayloadBuffer       += numberOfBytesTransmitted;
				totalBytesTransmitted += numberOfBytesTransmitted;
			} while (totalBytesTransmitted != payloadSize);

			return totalBytesTransmitted;
		}

	private:
		// @throw AL::Exception
		Void ReadMode(_Modes& value)
		{
			uint8 mode;

			ReadRegister(
				_Registers::RegOpMode,
				mode
			);

			value = static_cast<_Modes>(
				(mode & 0x1C) >> 2
			);
		}

		// @throw AL::Exception
		Void WriteMode(_Modes value)
		{
			if (value == _Modes::Receiver)
			{

				WriteOutputPower(
					OutputPowerModes::PA_0,
					GetOutputPowerLevel()
				);
			}

			uint8 mode;

			ReadRegister(
				_Registers::RegOpMode,
				mode
			);

			mode &= 0xE3;
			mode |= static_cast<uint8>(value) << 2;

			WriteRegister(
				_Registers::RegOpMode,
				mode
			);

			while (!Wait(_Registers::RegIrqFlags1, 0x80, 0x00, TimeSpan::Infinite))
			{
			}

			if (value == _Modes::Transmitter)
			{

				WriteOutputPower(
					GetOutputPowerMode(),
					GetOutputPowerLevel()
				);
			}
		}

		// @throw AL::Exception
		Void WriteConfig()
		{
			// Default config overrides
			{
				FIFO_Clear();

				// Set SyncOn to 1 (On)
				// Set SyncSize to 000 (0)
				WriteRegister(_Registers::RegSyncConfig, 0x88);

				// Set SyncValue1 to network
				WriteRegister(_Registers::RegSyncValue1, GetNetwork());

				// Set PacketFormat to 1 (Variable Length)
				// Set CrcOn to 1 (On)
				// Set AddressFiltering to 10 (NodeAddress/BroadcastAddress)
				WriteRegister(_Registers::RegPacketConfig1, 0x94);

				// Set NodeAddress to network address
				WriteRegister(_Registers::RegNodeAdrs, GetNetworkAddress());

				// Set BroadcastAddress to RFM69HCW_NETWORK_ADDRESS_BROADCAST
				WriteRegister(_Registers::RegBroadcastAdrs, RFM69HCW_NETWORK_ADDRESS_BROADCAST);

				// Set TxStartCondition to FifoNotEmpty
				WriteRegister(_Registers::RegFifoThresh, 0x8F);

				// Unset AutoRxRestartOn
				WriteRegister(_Registers::RegPacketConfig2, 0x00);

				// Set ContinuousDagc to Improved margin
				WriteRegister(_Registers::RegTestDagc, 0x30);
			}

			// Frequency
			WriteFrequency(GetFrequency());

			// Encryption Key + State
			if (IsEncryptionEnabled())
			{
				SetEncryptionKey(
					encryptionKey
				);

				EnableEncryption();
			}

			// Refresh lna state
			WriteLnaConfig(IsLnaEnabled(), GetLnaGain(), GetLnaImpedance());

			// Refresh preamble length
			WriteRegister(_Registers::RegPreambleMsb, static_cast<uint8>((GetPreambleLength() & 0xFF) << 8));
			WriteRegister(_Registers::RegPreambleLsb, static_cast<uint8>(GetPreambleLength() & 0xFF));
		}

		// @throw AL::Exception
		Void RestartRX()
		{
			uint8 value;

			ReadRegister(
				_Registers::RegPacketConfig2,
				value
			);

			WriteRegister(
				_Registers::RegPacketConfig2,
				static_cast<uint8>((value & 0xFB) | 0x04)
			);
		}

		// @throw AL::Exception
		Bool Wait(_Registers reg, uint8 mask, uint8 value, TimeSpan timeout)
		{
			OS::Timer timer;
			uint8     _value;

			do
			{
				if (timer.GetElapsed() >= timeout)
				{

					return False;
				}

				ReadRegister(
					reg,
					_value
				);
			} while ((_value & mask) == value);

			return True;
		}
		// @throw AL::Exception
		Bool Wait_PllLock(TimeSpan timeout)
		{
			if (!Wait(_Registers::RegIrqFlags1, 0x10, 0x00, timeout))
			{

				return False;
			}

			return True;
		}
		// @throw AL::Exception
		Bool Wait_PayloadReady(TimeSpan timeout)
		{
			if (!Wait(_Registers::RegIrqFlags2, 0x04, 0x00, timeout))
			{

				return False;
			}

			return True;
		}

		// @throw AL::Exception
		Void WriteLnaConfig(Bool enable, LnaGains gain, LnaImpedances impedance)
		{
			auto value = static_cast<uint8>(
				gain
			);

			if (impedance == LnaImpedances::OHM_200)
			{

				value |= 0x80;
			}

			WriteRegister(
				_Registers::RegLna,
				value
			);

			WriteRegister(
				_Registers::RegTestLna,
				enable ? 0x2D : 0x1B
			);
		}

		// @throw AL::Exception
		Void WriteFrequency(Frequency value)
		{
			value = value / _FREQUENCY_STEP;

			WriteRegister(
				_Registers::RegFrfMsb,
				static_cast<uint8>((value >> 16) & 0xFF)
			);

			WriteRegister(
				_Registers::RegFrfMid,
				static_cast<uint8>((value >> 8) & 0xFF)
			);

			WriteRegister(
				_Registers::RegFrfLsb,
				static_cast<uint8>(value & 0xFF)
			);
		}

		// @throw AL::Exception
		Void WriteOutputPower(OutputPowerModes mode, OutputPowerLevel level)
		{
			auto mask = static_cast<uint8>(
				mode
			);

			Bool pa0         = mask & 0b1000;
			Bool pa1         = mask & 0b0100;
			Bool pa2         = mask & 0b0010;
			Bool pa_boost    = mask & 0b0001;
			auto outputPower = level;

			// Write default state (PA0 + OutputPower)
			{
				// Pa0On       1
				// Pa1On       0
				// Pa2On       0
				// OutputPower outputPower & 0x1F
				WriteRegister(_Registers::RegPaLevel, 0b10000000 | (outputPower & 0x1F));

				// Write low power PA
				WriteRegister(_Registers::RegTestPa1, 0x55);
				WriteRegister(_Registers::RegTestPa2, 0x70);

				// Write default OCP
				WriteRegister(_Registers::RegOcp, 0x1A);
			}

			// PA0
			if (pa0)
			{
				// Do nothing
				// This is the default state
			}
			// PA1
			else if (pa1)
			{
				// Pa0On       0
				// Pa1On       1
				// Pa2On       0
				// OutputPower outputPower & 0x1F
				WriteRegister(_Registers::RegPaLevel, 0b01000000 | (outputPower & 0x1F));
			}
			// PA1 + PA2
			else if (pa1 && pa2 && !pa_boost)
			{
				// Pa0On       0
				// Pa1On       1
				// Pa2On       1
				// OutputPower outputPower & 0x1F
				WriteRegister(_Registers::RegPaLevel, 0b01100000 | (outputPower & 0x1F));
			}
			// PA1 + PA2 + PA_BOOST
			else if (pa1 && pa2 && pa_boost)
			{
				// Disable OCP
				WriteRegister(_Registers::RegOcp, 0x0F);

				// Pa0On       0
				// Pa1On       1
				// Pa2On       1
				// OutputPower outputPower & 0x1F
				WriteRegister(_Registers::RegPaLevel, 0b01100000 | (outputPower & 0x1F));

				// Write high power PA
				WriteRegister(_Registers::RegTestPa1, 0x5D);
				WriteRegister(_Registers::RegTestPa2, 0x7C);
			}
		}

		// @throw AL::Exception
		Void ReadRSSI(int8& value)
		{
			uint8 _value;

			ReadRegister(
				_Registers::RegRssiValue,
				_value
			);

			value = -_value / 2;
		}

		// @throw AL::Exception
		Void FIFO_Clear()
		{
			WriteRegister(
				_Registers::RegIrqFlags2,
				0x10
			);
		}

		// @throw AL::Exception
		Void FIFO_Flush()
		{
			while (!Wait(_Registers::RegIrqFlags2, 0x08, 0x00, TimeSpan::Infinite))
			{
			}
		}

		// @throw AL::Exception
		Void FIFO_Read(Void* lpBuffer, size_t size)
		{
			ReadRegister(
				_Registers::RegFifo,
				reinterpret_cast<uint8*>(lpBuffer),
				size
			);
		}

		// @throw AL::Exception
		Void FIFO_Write(const Void* lpBuffer, size_t size)
		{
			WriteRegister(
				_Registers::RegFifo,
				reinterpret_cast<const uint8*>(lpBuffer),
				size
			);
		}

		// @throw AL::Exception
		Void ReadRegister(_Registers reg, uint8& value)
		{
			auto _reg = static_cast<uint8>(
				static_cast<uint8>(reg) & 0x7F
			);

			SPITransaction transactions[] =
			{
				{
					.lpRX       = nullptr,
					.lpTX       = &_reg,
					.BufferSize = sizeof(uint8)
				},
				{
					.lpRX       = &value,
					.lpTX       = nullptr,
					.BufferSize = sizeof(uint8)
				}
			};

			device.Execute(
				transactions
			);
		}
		// @throw AL::Exception
		Void ReadRegister(_Registers reg, uint8* lpValues, size_t count)
		{
			auto _reg = static_cast<uint8>(
				static_cast<uint8>(reg) & 0x7F
			);

			SPITransaction transactions[] =
			{
				{
					.lpRX       = nullptr,
					.lpTX       = &_reg,
					.BufferSize = sizeof(uint8)
				},
				{
					.lpRX       = lpValues,
					.lpTX       = nullptr,
					.BufferSize = sizeof(uint8) * count
				}
			};

			device.Execute(
				transactions
			);
		}

		// @throw AL::Exception
		Void WriteRegister(_Registers reg, uint8 value)
		{
			auto _reg = static_cast<uint8>(
				static_cast<uint8>(reg) | 0x80
			);

			SPITransaction transactions[] =
			{
				{
					.lpRX       = nullptr,
					.lpTX       = &_reg,
					.BufferSize = sizeof(uint8)
				},
				{
					.lpRX       = nullptr,
					.lpTX       = &value,
					.BufferSize = sizeof(uint8)
				}
			};

			device.Execute(
				transactions
			);
		}
		// @throw AL::Exception
		Void WriteRegister(_Registers reg, const uint8* lpValues, size_t count)
		{
			auto _reg = static_cast<uint8>(
				static_cast<uint8>(reg) | 0x80
			);

			SPITransaction transactions[] =
			{
				{
					.lpRX       = nullptr,
					.lpTX       = &_reg,
					.BufferSize = sizeof(uint8)
				},
				{
					.lpRX       = nullptr,
					.lpTX       = lpValues,
					.BufferSize = sizeof(uint8) * count
				}
			};

			device.Execute(
				transactions
			);
		}
	};
}
