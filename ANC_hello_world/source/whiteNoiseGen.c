#include "whiteNoiseGen.h"
#include "fsl_rnga.h"

#define WHITE_NOISE_SAMPLES 1000

//1000 muestras de ruido blanco gausiano generadas en matlab (std=1 mean=0)
const float whiteNoise[WHITE_NOISE_SAMPLES] = {0.26942,0.53839,1.1118,-2.0833,-0.54715,-0.61919,0.057711,-0.54293,-0.91678,
							 -0.64711,-1.6241,-0.15985,0.51328,-0.42012,-1.4613,0.75129,-0.52478,-0.5979,
							 -1.3707,-0.0026424,0.21768,-1.315,-0.63576,0.096236,1.2384,-0.2732,-0.60386,
							 0.24484,-1.3314,-0.88558,0.93587,-0.49797,1.0859,0.9995,-1.9508,1.0047,-1.6164,
							 0.85293,0.064407,0.16525,-0.13488,1.7098,0.2989,-0.10796,-1.4173,-0.65389,0.080155,
							 -1.1877,-0.90691,2.2083,0.17428,0.60182,0.86705,0.91592,-0.35196,1.4752,-1.689,
							 -0.19291,-0.40819,-0.11853,0.70078,1.5726,-1.1272,2.3811,-0.24694,0.68954,-0.57871,0.21573,1.891,-0.25225,0.15246,0.14401,3.1179,1.5876,1.1364,3.0969,0.83187,-0.44712,-0.48839,0.16751,0.83517,0.1679,-0.77871,-0.86438,-1.4314,0.60928,-0.48716,1.4672,-1.9966,-0.34193,-1.3866,1.4995,0.62464,0.64466,-1.13,0.42263,1.1678,0.30201,-0.28723,0.60432,2.2749,0.61026,-0.36837,0.0071512,-1.954,-0.5017,-0.47553,0.62267,0.578,-0.33318,-0.091401,-1.1511,0.04491,-0.31237,-0.64996,-0.041341,1.4232,-0.0081429,0.71804,0.4651,0.18298,0.49002,-0.20504,-1.0354,0.75592,0.60607,-0.1365,0.084949,-0.20304,1.9195,0.27823,-1.1114,0.42317,0.8522,0.14571,-1.1454,-0.9037,1.3449,-0.039961,-0.25872,0.60684,-0.30405,0.49741,-0.098972,-0.39034,-1.6544,-0.90568,-1.0101,-0.016149,0.87294,-0.78713,-0.13814,0.98642,-0.82218,-0.95494,-0.14482,0.42347,-2.0574,-1.3449,0.17871,1.0555,0.095154,0.8075,0.57902,0.55975,0.22773,1.0074,-0.546,1.2735,-1.2734,-0.27854,-0.67653,0.74149,-0.54929,0.19809,0.95184,-0.62461,0.096334,0.24342,0.74313,0.70337,-0.0408,-0.60458,0.30106,0.073061,0.31373,0.033577,1.0734,0.55905,0.67295,0.5274,1.4884,0.22922,-1.67,0.18017,-0.66618,0.70515,-1.2698,2.2582,-0.18001,0.85067,-0.3715,1.3954,0.98323,-0.53695,-0.73074,-1.6305,-1.1589,-0.30251,-0.71518,0.79666,-1.1054,1.5076,0.047133,0.29446,-1.0785,-0.59149,-1.711,-0.92623,-0.31758,0.026886,0.3564,-0.76981,-1.4095,2.3096,0.088119,-2.0195,1.6896,1.1185,0.28553,-1.3219,-1.1164,0.46173,-0.9357,-0.064639,0.95556,-1.1982,-1.4114,-0.70071,2.2884,-1.2326,0.3946,0.71633,-0.40968,1.1731,0.92192,2.4296,1.3183,0.73424,-1.0531,1.261,-2.4786,1.3388,-2.4086,0.056697,-0.47118,1.7479,-0.71634,-1.1566,-0.094508,1.3393,-1.3578,1.7854,0.24746,1.5869,1.5642,0.44189,1.5179,-1.3139,0.21276,0.67683,-0.068143,1.557,0.54496,0.76739,0.083041,-0.77876,-0.21127,-0.55516,-1.4432,-1.4122,1.3663,-0.65085,-1.2682,0.34495,-0.96752,-1.2034,1.0703,0.30705,1.3524,1.0019,0.22453,-0.041356,-0.88026,-1.7941,0.18655,-0.078055,-0.0094781,0.21402,2.2795,1.0791,-0.55633,1.2657,0.024534,-0.63365,0.00069536,-0.028491,-0.66927,-1.3343,-1.7835,1.7398,-0.69962,1.6984,-0.12448,-1.4675,0.95158,1.5643,0.74261,0.39479,0.39224,0.70055,-1.836,-1.9746,-0.43022,-0.60856,1.3952,-0.80135,-0.21273,0.23363,-0.41283,-0.80061,-1.4572,1.9674,-0.499,-0.65533,0.12932,-1.7863,-0.44943,-0.31943,-1.3864,1.0564,0.64321,0.14998,1.2283,0.084613,0.49112,-0.71979,-1.1401,-0.43831,0.93335,-0.32817,-0.563,1.002,-0.54011,-0.76888,-0.65508,0.037556,1.076,-0.85463,0.034622,0.12656,1.3187,-1.6694,0.15278,0.011728,-1.0855,-0.53537,-0.66237,-0.4908,-0.0414,-0.62049,-0.36784,1.4199,0.34111,1.7356,2.1427,0.7179,-0.88182,-0.20134,0.63003,0.96221,0.58304,0.35234,0.72811,0.25398,-1.7015,-0.56677,0.71078,-0.15817,1.1281,0.1711,-0.26365,0.2823,-0.77581,-0.57211,0.26261,0.44044,-0.77353,-0.0094632,1.0539,-1.8711,-1.1322,-2.0809,0.16058,1.1445,0.18188,-0.39333,0.8861,-0.15524,-0.58676,0.17296,1.3028,1.1719,0.95371,0.34845,-0.38068,0.6384,0.7448,-0.8823,0.14917,-0.74798,-0.95291,0.87357,0.45831,0.98683,-0.13691,0.067219,0.13104,1.5216,1.3059,-1.4095,0.37947,-1.4702,-0.053158,0.42527,-0.332,0.034712,-0.22108,0.38893,-2.4954,0.24257,0.43217,-1.201,1.7181,-1.3426,1.319,-0.10377,0.43779,-0.70417,-0.071785,-1.493,0.78002,-0.84232,1.0244,-1.6488,-1.3379,-1.3665,-1.4448,-0.1725,-0.83009,-1.2751,-0.091755,0.80425,1.5444,-0.83507,-0.20546,0.016717,-1.6386,1.6304,-1.2446,-0.79521,1.9863,-0.072239,1.1862,0.5677,-2.7111,2.0037,-0.52565,-0.64022,-0.26009,-1.1344,1.1309,0.88772,-0.62894,-0.30164,-0.012902,1.0479,-0.34449,-0.74712,-1.0223,-2.5824,2.7814,-0.12454,-0.67494,0.4302,-1.1627,-0.43956,0.45962,-1.0892,0.81399,1.5507,-0.16055,0.041452,1.8634,0.55134,-1.2622,-1.1426,-0.78151,0.22554,-1.111,-0.13709,0.33224,1.5188,-0.88371,-0.74618,-0.58141,1.8535,-0.23081,1.259,-0.14457,0.40527,0.51859,-0.86443,1.1401,-1.1896,-1.1456,0.085802,-1.1362,-0.72015,1.3655,0.38657,-1.7231,0.31736,-0.096114,1.115,1.4033,0.41579,-2.1164,0.87617,0.81926,1.5305,2.1602,0.38922,1.7855,0.1103,0.32042,-0.026922,1.1355,1.7127,0.076496,-0.97924,-2.4156,-0.25048,-0.26667,2.6356,0.37549,1.1514,0.39444,0.674,0.36865,-0.4118,-1.3124,-0.14046,0.61671,1.4157,-0.24336,-0.44252,0.023107,0.12562,-0.085809,1.3459,-1.4563,0.84606,0.22843,-0.81081,-0.016063,-0.57582,-1.9019,-0.79617,0.79452,3.434,-0.50077,-0.055771,-1.3476,-0.044145,1.6753,-1.0354,-0.15851,0.10179,0.37136,0.7579,1.8783,-0.18798,-0.097178,0.34063,0.091818,1.1943,1.7925,0.1031,1.1115,0.43777,-1.247,1.3548,1.9806,-0.50475,-0.44323,-0.32185,1.0372,0.57389,1.2405,-0.9471,-1.6957,-1.3076,-2.4647,-0.83949,-0.98831,-0.96494,0.038864,0.046791,0.87328,0.31085,-0.51057,0.12735,-0.21783,0.33691,0.025916,-0.28202,0.82386,0.56235,0.57341,-1.1331,0.17711,1.9475,0.99886,0.014173,0.15757,-2.586,-1.1739,-1.0633,1.1173,1.2197,-0.20349,-0.30576,1.2783,1.1921,0.345,-1.1384,-0.13133,-1.404,-1.7149,-0.62247,-0.52292,0.91925,-0.14943,0.41005,-0.99499,-0.47192,-0.58837,-0.47323,0.20728,1.6694,0.75915,-1.4731,0.15858,-1.1019,1.5518,0.98187,0.12739,0.27602,0.11802,-0.76698,0.099201,-1.0209,0.31919,1.4431,-1.1518,1.2073,0.69219,1.8837,0.25049,0.14873,0.90263,-0.31509,0.93765,0.059611,0.30305,-1.327,-0.44957,0.19106,0.46556,-0.27803,-0.26242,2.1711,-0.98039,-0.22173,0.95368,-0.60576,-0.026543,-0.95025,-0.39279,-0.11437,1.3735,-0.42502,-0.32173,-0.0023875,-0.99748,0.2523,2.4996,-0.35905,0.73607,2.1445,0.40809,1.5931,-0.25008,1.559,-2.2079,-0.44051,0.90229,-0.81224,-0.26172,0.53617,-0.54773,-0.45519,1.7359,0.73269,0.70337,-1.35,0.93548,2.1722,-0.92742,-0.1167,0.12066,-1.444,-0.19857,-0.64576,-0.73537,-1.9148,0.2194,-0.071328,0.65474,-0.94796,-0.60087,0.27813,-0.85308,-2.3546,0.66979,-1.1781,-0.072242,-0.42717,-1.2453,-1.0287,-1.0006,-0.56443,-0.29043,0.4129,-0.89575,0.81086,0.44597,1.0051,0.92196,0.96557,-0.16677,0.84801,-0.60178,-1.221,-0.92764,-0.63474,0.8438,-0.88494,-0.9273,1.0383,1.7709,-0.79356,-1.125,-0.65016,-0.93368,1.2469,-0.26938,-1.4776,-1.3334,0.30088,0.3414,-0.95975,1.387,-0.89626,-0.25174,1.0439,-0.85881,-0.061813,-0.86622,-0.11153,1.6952,-0.35318,0.18512,0.55256,-0.056231,0.36307,-1.1767,-0.15611,0.96669,0.90664,-0.25353,1.1501,-1.0685,-0.62889,2.0555,0.57248,-0.33792,0.0028013,-0.31318,-1.2487,-0.14193,0.29194,1.9913,-1.2509,-0.69966,1.2581,0.4188,-0.44778,0.69212,0.10664,-0.26432,-0.55857,0.51773,-0.43669,-1.1871,-0.64848,-0.33531,-0.36411,-0.50143,0.63042,0.11621,-0.4152,-0.72328,-1.9585,-1.5167,0.51575,0.092951,-0.96478,0.70162,0.49615,-0.040688,-1.1127,2.4456,-0.40402,-0.50409,-0.59203,-2.0941,0.9896,0.87313,0.21696,-0.079065,-0.31334,0.25938,-1.0728,1.2855,-1.3458,-0.14762,0.2301,-2.1838,0.49352,1.2582,0.19238,1.1177,-0.86441,-0.36692,1.4243,1.2591,1.4194,-0.73545,0.050692,-0.24951,0.37489,0.11092,-0.46364,-0.83568,-0.40593,0.70032,0.013571,-3.115,0.17486,-0.50946,0.58551,-0.23714,1.4628,0.033732,1.4201,2.5725,-1.2931,-0.52816,-0.41937,-1.038,0.95574,1.0533,0.30036,-0.036705,-1.8466,-1.3628,-1.0561,-0.5459,1.7146,-0.55718,-0.86049,0.093654,-0.22585,0.5166,-1.4578,-0.46745,-0.85801,-2.7135,-0.4858,-0.75754,-0.51436,0.054588,-0.39221,-1.7499,0.12671,0.61857,-0.00092435,0.094773,-0.8297,-0.47039,3.6762,-1.013,-1.2922,0.17397,0.73484,1.5595,-0.10823,-0.23038,-1.6242,0.39863,-0.33913,1.3259,-0.2634,0.73572,0.16776,-0.31677,-0.074955,0.24831,-1.3726,0.13096,0.94619,-0.16538,1.5454,-0.6143,-0.63894,-1.5989,-0.081821,1.2338,-0.39861,-1.8102,0.63196,0.9091,1.4461,2.0153,0.30403,-0.43838,2.2407,-1.0831,1.0382,-1.0275,0.2857,-0.18781,0.94912,0.21877,1.9984,-1.3323,-0.72707,-0.11578,-0.49176,-0.9946,-0.20983,-0.29752,1.8967,-0.41923,-0.096567,-0.12155,2.0648,-1.0951,0.42755,0.11854,-0.51101,-0.81111,1.3877,-1.168,-0.43325,0.79311,-0.31466,-0.36736,0.86721,-1.6362,0.81487,-0.075343,-0.56786,0.56776,0.43762,0.35412,-0.55449,-1.2236};



void initWNG()
{
	RNGA_Init(RNG);
}

float ranf(){
	uint32_t uintRand[2];
	RNGA_GetRandomData(RNG, uintRand, sizeof(uintRand)); //Lo puse con un arreglo porque sino me tiraba un error que no pude arreglar
	float aux= (float)uintRand[0] / (float)UINT32_MAX;
	return aux;
}

float box_muller(float m, float s)	/* normal random variate generator */
{				        /* mean m, standard deviation s */
	float x1, x2, w, y1;
	static float y2;
	static int use_last = 0;

	if (use_last)		        /* use value from previous call */
	{
		y1 = y2;
		use_last = 0;
	}
	else
	{
		do {
			x1 = 2.0 * ranf() - 1.0;
			x2 = 2.0 * ranf() - 1.0;
			w = x1 * x1 + x2 * x2;
		} while ( w >= 1.0 );

		w = sqrt( (-2.0 * log( w ) ) / w );
		y1 = x1 * w;
		y2 = x2 * w;
		use_last = 1;
	}

	return( m + y1 * s );
}

float32_t whiteNoiseGen(float32_t s)
{				        /* mean m, standard deviation s */
	unsigned int index =(unsigned int)(ranf()*WHITE_NOISE_SAMPLES); //Elige una de las samples al azar
	return whiteNoise[index]*s;
}


