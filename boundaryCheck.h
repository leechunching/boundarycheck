#ifndef __BOUNDARYCHECK_H__
#define __BOUNDARYCHECK_H__

#include "ffplay.h"

#define MACRO_BLOCK_MAX_HEIGHT	1080 // 1080/ 8 = 140
#define MACRO_BLOCK_MAX_WIDTH	1920 // 1920/ 8 = 240  ( if macroblock>8 )
#define MAX_FRAME_BUFFER	30

#define BLOCK_SIZE_W		1	// min macro block must >8 
#define BLOCK_SIZE_H		1

#define DIFF_LIMIT 10

#define QUANTIZATION_TOTAL	255
#define QUANTIZATION_RANGE	10

#define getQuant( x ) (x/(QUANTIZATION_TOTAL/QUANTIZATION_RANGE))

#define YUV_SIZE 256

#define MODE_Y		1
#define MODE_R		2
#define MODE_G		3
#define MODE_B		4
#define MODE_S		5
#define MODE_SH		6
#define MODE_SW		7
#define MODE_ALL	10


struct QUANTIZATION_STRUCT{
	int	brightness;
	int	num;
};

enum WINDOW_ENUM{
	FULL_SCREEN=0,
	WINDOW_SCREEN,
	WINDOW_MAX,

};
struct SUGGEST_STRUCT{
	int	max, min, avg, mode1, mode3;
};

#define JE_ARRAY_SIZE 5000
struct CTT{
	int	table[ YUV_SIZE ][ YUV_SIZE ];
	int	t[ YUV_SIZE ];
	int	t1[ YUV_SIZE ];
	float	mi;
	float	je_org, je_hg2, je_hg1;
	int	je_count;
        float	je_total ;
        float	je_array[ JE_ARRAY_SIZE	] ;
	int	not_zero;
	time_t	begin, end;
	int	H1, H2, W1, W2;

	float	je_org_avg, je_hg1_avg, je_hg2_avg;
	float	stdev_org, stdev_hg1, stdev_hg2;
        int		raw_val;
	float		brightness;
	
};

struct MACRO_BLOCK_DATA{
	unsigned int		frameNum;
	unsigned int		diff[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int		y[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int		u[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int	        v[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int		r[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int		g[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int		b[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned char		r_data[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned char		g_data[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned char		b_data[ MACRO_BLOCK_MAX_HEIGHT ][ MACRO_BLOCK_MAX_WIDTH ];
	unsigned int    	isBoundary;
	//unsigned int 		 zeroCount;
	int			max_h;
	int			max_w;
	struct QUANTIZATION_STRUCT	quantization[ QUANTIZATION_RANGE ];
	struct SUGGEST_STRUCT	suggest[ WINDOW_MAX ];
	int			suggest_quandrant[4];

	//for histogram
	unsigned int		his_y[ YUV_SIZE ];
	unsigned int		his_u[ YUV_SIZE ];
	unsigned int		his_v[ YUV_SIZE ];
	unsigned int		his_r[ YUV_SIZE ];
	unsigned int		his_g[ YUV_SIZE ];
	unsigned int		his_b[ YUV_SIZE ];
	unsigned int		his_s[ YUV_SIZE ];
};


int boundaryCheck( VideoState *is, AVPicture *pict, const char * input_filename );
void open_all_dat_file();
void close_all_dat_file();

#endif

