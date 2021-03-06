#pragma once

#define RDS_SYMBOL_RATE 1187.5f
#define RDS_SUBCARRIER_FREQ 57000
#define RDS_PLL_BW_HZ 12
#define M_PI 3.14159265359f

#define FM_STEREO_MIXING_MIN 0.01f // TODO: Fine tune these
#define FM_STEREO_MIXING_MAX 0.02f // TODO: Fine tune these

#define FM_DEVIATION_US 75000
#define DEEMPHASIS_TIME_US 75

#define FM_DEVIATION_EU 75000
#define DEEMPHASIS_TIME_EU 50

#define FM_DEVIATION_DEFAULT FM_DEVIATION_US
#define DEEMPHASIS_TIME_DEFAULT DEEMPHASIS_TIME_US