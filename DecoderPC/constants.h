//#define FRAME_COUNT 15			// [   ]
//#define FRAME_RATE 255			// [FPS]
//#define EXPOSURE_TIME 3750		// [us ]
//#define FPS 30

//Camera Parameters
#define FRAME_WIDTH 640
#define FRAME_HEIGHT 480
#define X_OFF 704
#define Y_OFF 304
//#define FRAME_WIDTH 1024
//#define FRAME_HEIGHT 544
//#define X_OFF 512
//#define Y_OFF 272
#define EXPOSURE 10000           // in microseconds --> 20000 = 50Hz, 10000 = 100Hz, 5000 = 200Hz, ... (50Hz and 100Hz will avoid LightningTube flickering)
#define AUTO_EXP_GAIN false     // we want to control exposure on our own to get the desired fixed framerates
#define IMG_FORMAT 5            // 5 is RAW8 (1channel 0..255) whereas 2 is RGB24 (3channels 0..255) ---> 5 (RAW8) allows highest framerates (still depending on resolution)
#define T_MAX 0
// MeasureTime Parameters
#define START_FRAMES 100			// [   ]
#define MEASUREMENT_COUNT 2000	// [   ]
#define MEASUREMENT_TYPES 15	// [   ]

// Communivation Parameters
#define CAMPORT 7755
//#define BOTADDRESS "10.42.0.59"
#define BOTADDRESS "192.168.1.101"
//#define BOTADDRESS "127.0.0.1"
#define BOTPORT 8888

// Angle Prediction Parameter
#define NUMBER_PRED_ANGLES 10
