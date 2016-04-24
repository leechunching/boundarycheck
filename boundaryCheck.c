#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include "boundaryCheck.h"
  FILE * fd_y = NULL ;
  FILE * fd_r = NULL ;
  FILE * fd_g = NULL ;
  FILE * fd_b = NULL ;
  FILE * fd_s = NULL ;
  FILE * fd_all = NULL ;

  int total_begin, total_end, MI_begin, MI_end, One_begin, One_end;
  int total_time, MI_time, One_time;

#define MIN( a, b ) ( a < b ) ? a : b
#define MAX( a, b ) ( a > b ) ? a : b
int cmpint(const void *a, const void *b){
	return (((struct QUANTIZATION_STRUCT*)b)->num > ((struct QUANTIZATION_STRUCT*)a)->num) ;
}

void cal_running_time(){
	total_time = total_end - total_begin;
     	MI_time = MI_end - MI_begin;
     	One_time = One_end - One_begin;

	printf("---total_begin=%d total_end=%d  total_time=%d\n", total_begin, total_end, total_time);
	printf("---MI_begin=%d MI_end=%d  total_time=%d\n", MI_begin, MI_end, MI_time);
	printf("---One_begin=%d One_end=%d  total_time=%d\n", One_begin, One_end, One_time);

}
#include <sys/stat.h>
#include <sys/types.h>

int open_fn( FILE **fd, char *path, char *fn ){
	char tmp[128];

	sprintf( tmp, "%s/%s", path, fn );
	if ( *fd == NULL ) *fd = fopen( tmp, "wb" ) ;
	return 0;
}
void open_all_dat_file( char *in_fn){
	char fn[128], *p=NULL, tmp[128];
	
	sprintf( fn, "%s", in_fn );

	p = strrchr( fn, '/' );
	if( p == NULL ) sprintf( tmp, "data/%s", fn );
	else		sprintf( tmp, "data/%s", p+1 );

	p = strchr( tmp, '.' );
	if( p != NULL ) *p ='\0';
	mkdir( tmp, 0777 );

	open_fn( &fd_y, tmp, "y.dat" );
	open_fn( &fd_r, tmp, "r.dat" );
	open_fn( &fd_g, tmp, "g.dat" );
	open_fn( &fd_b, tmp, "b.dat" );
	open_fn( &fd_s, tmp, "s.dat" );
	open_fn( &fd_all, tmp, "all.dat" );
	return ;
}
void close_all_dat_file(){
    fclose( fd_y ) ;
    fclose( fd_r ) ;
    fclose( fd_g ) ;
    fclose( fd_b ) ;
    fclose( fd_s ) ;
    fclose( fd_all ) ;
}

int block_luminance_calculation( VideoState *is, AVPicture *pict, struct MACRO_BLOCK_DATA *this_frame, int count ){
	unsigned char	*y = pict->data[0];
	unsigned char	*u = pict->data[1];
	unsigned char	*v = pict->data[2];
	int		width = is->video_st->codec->width;
	int		height= is->video_st->codec->height;
	int 		w = 0, h = 0;

	//printf("y[%x] u[%x] v[%x] ysize=%d usize=%d\n", (int)y,(int)u,(int)v, v-y, u-v);

	this_frame->max_h = height/BLOCK_SIZE_H;
	this_frame->max_w = width/BLOCK_SIZE_W;
	if( this_frame->max_h % 2 ) this_frame->max_h--;
	if( this_frame->max_w % 2 ) this_frame->max_w--;


	memset( this_frame->y, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );
	memset( this_frame->u, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );
        memset( this_frame->v, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );
	memset( this_frame->r, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );
	memset( this_frame->g, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );
	memset( this_frame->b, 0, sizeof(int )* MACRO_BLOCK_MAX_HEIGHT * MACRO_BLOCK_MAX_WIDTH );

        // V's Macro Block total value
        for( h = 0; h < height; h++){
                for( w = 0; w < width; w++ ){
			unsigned char *ptr_y, *ptr_u, *ptr_v;
			unsigned char *ptr_r, *ptr_g, *ptr_b;

			ptr_y = y+ h*width+w;
			ptr_u = u+ ((h*width)>>2) + (w>>1);
			ptr_v = v+ ((h*width)>>2) + (w>>1);
			ptr_r = &(this_frame->r_data[ h ][ w ]);
			ptr_g = &(this_frame->g_data[ h ][ w ]);
			ptr_b = &(this_frame->b_data[ h ][ w ]);

			//R= Y+ 1.13983*(V-128)
			//G= Y-0.39465*(U-128) - 0.58060*(V-128)
			//B=Y+2.03211*(U-128)
			*ptr_r = *ptr_y + ( 1.13983 * ( *ptr_v - (float)128 ));
			*ptr_g = *ptr_y - ( 0.39465 * ( *ptr_u - (float)128 )) - ( 0.58060*( *ptr_v - (float)128 )) ;
			*ptr_b = *ptr_y + ( 2.03211 * ( *ptr_u - (float)128 ));
			if( *ptr_r < 0 ) printf(" r %d\n", *ptr_r );
			if( *ptr_g < 0 ) printf(" g %d\n", *ptr_g );
			if( *ptr_b < 0 ) printf(" b %d\n", *ptr_b );
                }
        }
	// Y's Macro Block total value
                        //yuv_h = ( h / 2 ) * 2 ;
                        //yuv_w = w / 2 ;
                        //*tmp_v  += *( v+ yuv_h + yuv_w );
	for( h = 0; h < height; h++){
		for( w = 0; w < width; w++ ){
			unsigned int *tmp_y  = &(this_frame->y[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
                        unsigned int *tmp_u = &(this_frame->u[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
                        unsigned int *tmp_v = &(this_frame->v[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
			unsigned int *tmp_r  = &(this_frame->r[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
                        unsigned int *tmp_g = &(this_frame->g[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
                        unsigned int *tmp_b = &(this_frame->b[ h/BLOCK_SIZE_H ][ w/BLOCK_SIZE_W ]);
			*tmp_y  += *( y+ h*width+w) ;
			*tmp_u  += *( u+ ((h*width)>>2) + (w>>1) );
			*tmp_v  += *( v+ ((h*width)>>2) + (w>>1) ) ;
			*tmp_r  += this_frame->r_data[ h ][ w ];
			*tmp_g  += this_frame->g_data[ h ][ w ];
			*tmp_b  += this_frame->b_data[ h ][ w ];

                }
        }
	// Y's Macro Block average 
	for( h = 0; h < this_frame->max_h; h++){
		for( w = 0; w < this_frame->max_w; w++ ){
			unsigned int *ptr_y, *ptr_u, *ptr_v;
			unsigned int *ptr_r, *ptr_g, *ptr_b;

			ptr_y = &(this_frame->y[ h ][ w ]);
			ptr_u = &(this_frame->u[ h ][ w ]);
			ptr_v = &(this_frame->v[ h ][ w ]);
			ptr_r = &(this_frame->r[ h ][ w ]);
			ptr_g = &(this_frame->g[ h ][ w ]);
			ptr_b = &(this_frame->b[ h ][ w ]);
			*ptr_y /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );
			*ptr_u /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );
			*ptr_v /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );
			*ptr_r /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );
			*ptr_g /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );
			*ptr_b /= ( BLOCK_SIZE_H * BLOCK_SIZE_W );

			if ( (*ptr_y >= 0) && ( *ptr_y < YUV_SIZE) ) this_frame->his_y[ *ptr_y ]++;
			if ( (*ptr_u >= 0) && ( *ptr_u < YUV_SIZE) ) this_frame->his_u[ *ptr_u ]++;
			if ( (*ptr_v >= 0) && ( *ptr_v < YUV_SIZE) ) this_frame->his_v[ *ptr_v ]++;
			if ( (*ptr_r >= 0) && ( *ptr_r <YUV_SIZE) ) this_frame->his_r[ *ptr_r ]++;
			if ( (*ptr_g >= 0) && ( *ptr_g <YUV_SIZE) ) this_frame->his_g[ *ptr_g ]++;
			if ( (*ptr_b >= 0) && ( *ptr_b <YUV_SIZE) ) this_frame->his_b[ *ptr_b ]++;
		}
	}
	this_frame->frameNum	= count;
	return 0;
}
int cal_raw_y75( struct MACRO_BLOCK_DATA *this_frame, int mode){
	int point75 = this_frame -> max_h * this_frame -> max_w * 0.75 ;
	int raw_val = 0, over75 = 0, i;

	for ( i = 0 ; over75 == 0 ; i++  ){
		switch( mode ){
			case MODE_Y:
				raw_val += this_frame->his_y[i]; break;
			case MODE_R:
				raw_val += this_frame->his_r[i]; break;
			case MODE_G:
				raw_val += this_frame->his_g[i]; break;
			case MODE_B:
				raw_val += this_frame->his_b[i]; break;
			case MODE_S:
				raw_val += this_frame->his_y[i]; break;
			case MODE_SH:
				raw_val += this_frame->his_y[i]; break;
			case MODE_SW:
				raw_val += this_frame->his_y[i]; break;
			default:
				printf("%s:Call mode error!n", __FUNCTION__);
				break;
		}
		if ( raw_val >= point75 ) {
			raw_val = i ; 
			over75 = 1 ;
		}
	}
	//printf( "%s:mode[%d]>>point75:%d, >>raw_y:%d\n",__FUNCTION__, mode, point75, raw_val);
	return raw_val;
}
#define JEMI_WIDTH 40
int init_ctt( struct MACRO_BLOCK_DATA *this_frame, struct MACRO_BLOCK_DATA *old_frame, struct CTT *p, int mode){
	int	h, w;
	int	i, j;
	memset( p, 0, sizeof( struct CTT ) );

	p->H1 = this_frame->max_h/2 - JEMI_WIDTH/2;
	p->H2 = this_frame->max_h/2 + JEMI_WIDTH/2;
	p->W1 = this_frame->max_w/2 - JEMI_WIDTH/2;
	p->W2 = this_frame->max_w/2 + JEMI_WIDTH/2;

	for ( h = 0 ; h < this_frame->max_h ; h++ ) {
		for ( w = 0 ; w < this_frame->max_w; w++ ) {
			switch( mode ){
				case MODE_Y:
					i = old_frame->y[h][w];
					j = this_frame->y[h][w];
					break;
				case MODE_R:
					i = old_frame->r[h][w];
					j = this_frame->r[h][w];
					break;
				case MODE_G:
					i = old_frame->g[h][w];
					j = this_frame->g[h][w];
					break;
				case MODE_B:
					i = old_frame->b[h][w];
					j = this_frame->b[h][w];
					break;
				default:
					printf("%s:Call mode error [%d]\n", __FUNCTION__, mode);
			}
				
			
			if ( i > 255 || j > 255 );
			else if ( i < 0 || j < 0 ) printf ( "%s: mode[%d] 0! i[%d] j[%d]\n", __FUNCTION__, mode, i, j ) ;
			else {
				p->table[i][j]++;
				p->not_zero++;
			} 
		} 
	}
	//printf ( "%s: mode[%d] how many c not zero:%d\n", __FUNCTION__, mode, p->not_zero );

	for ( h = 0 ; h < YUV_SIZE ; h++ ) {
		for ( w = 0 ; w < YUV_SIZE ; w++ ) {
			p->t[w]	+= p->table[w][h]; //ct
			p->t1[h]+= p->table[w][h]; //ct+1`
		} 
	}

	return 0;
}

void cal_je_avg_stdev( struct MACRO_BLOCK_DATA *this_frame, int one_count, struct CTT *p, float je_org, float *je_avg, float *stdev);
#define MAX_JE 3
int cal_je_mi( struct MACRO_BLOCK_DATA *this_frame, struct MACRO_BLOCK_DATA *old_frame, int one_count, struct CTT *p, int mode){
	int	i, j, o, t;
	float	nume_mi = 0, deno_mi = 0, log_result_mi;	// numerator & denominator
	float	nume_je=0;
	float	deno_je=0;
	float	log_result_je;	// numerator & denominator
	float	log2 = log(2);

	float	wxh=	this_frame->max_h * this_frame->max_w;

	p->begin = time((time_t*)NULL);
	for( i = 0; i < 256 ; i++) {
		for( j = 0; j < 256 ; j++ ){
			nume_mi = p->table[i][j] / wxh;
			deno_mi = ( p->t[i] / wxh) * ( p->t1[j] / wxh); 

			if ( deno_mi == 0 || nume_mi == 0) log_result_mi = 0 ;
			else{
				log_result_mi = log( nume_mi/deno_mi ) / log2;
				p->mi = p->mi + (nume_mi *  log_result_mi); 
			}
		}
	}
	p->end = time((time_t*)NULL);

#if 0//remove JE
	for( i = 0; i < 256 ; i++) {
		for( j = 0; j < 256 ; j++ ){
			nume_je = p->table[i][j] / wxh;
			deno_je = (p->t[i]/ wxh) * (p->t1[j]/ wxh) ;

			log_result_je	= (deno_je == 0 || nume_je == 0)?	0 : log( nume_je ) / log2;

			p->je_org = p->je_org + (nume_je  *  log_result_je);
		} 
	}
	p->je_org *= -1;
#endif
	//p->raw_val = cal_raw_y75( this_frame, mode);
	//p->brightness = (( 255 - p->raw_val ) + ( 2 *( p->je_org_avg - ( MAX_JE/ 2 )))/MAX_JE) ;
	//p->brightness = ( p->raw_val );

	return 0;
}
int cal_histogram( struct MACRO_BLOCK_DATA *this_frame, struct MACRO_BLOCK_DATA *old_frame){
	int	i, o, t;
	float diff_hg=0, nume_ai_t=0, nume_ai_o=0, deno_ai_t=0, deno_ai_o=0;
	float nume_sum_ai_t=0, nume_sum_ai_o=0, deno_sum_ai_t=0, deno_sum_ai_o=0;
	
	for( i = 0; i < 256 ; i++) {
		
		o = old_frame->his_y[i];
		t = this_frame->his_y[i];

		//summarize HG difference, Di
		diff_hg = fabsf(t - o);
		this_frame->hg_d = this_frame->hg_d + diff_hg;
		
		
		//summarize (nume and deno) ofHG intensity AI 
		nume_ai_t = i*t;
		nume_ai_o = i*o;
		nume_sum_ai_t = nume_sum_ai_t + nume_ai_t;
		//nume_sum_ai_o = nume_sum_ai_o + nume_ai_o;
		deno_sum_ai_t = deno_sum_ai_t + t;
		//deno_sum_ai_o = deno_sum_ai_o + o;
	}
	
	//Calculate AID (Average Intensity Difference)
	this_frame->hg_ai = (nume_sum_ai_t/deno_sum_ai_t);
	//printf( "FRAME=%d, HG_D=%f, HG_AID=%f \n",this_frame->frameNum, this_frame->hg_d, this_frame->hg_aid) ;
	return 0;
}

int diff_frame( VideoState *is, struct MACRO_BLOCK_DATA *this_frame, struct MACRO_BLOCK_DATA *old, int limit,  int writeFlag){
	static int	zero_count_old=0;
	int		w = 0, h = 0, zero_count = 0, bBoundary=0;

	One_begin = time((time_t*)NULL);

	for( h = 0; h < this_frame->max_h; h++){
		for( w = 0; w < this_frame->max_w; w++ ){
			this_frame->diff[ h ][ w ] = abs( this_frame->y[ h ][ w ] - old->y[ h ][ w ] );
			if( this_frame->diff[ h ][ w ] < limit ) this_frame->diff[ h ][ w ] = 0;
			zero_count += ( this_frame->diff[ h ][ w ] >= limit)? 0 : 1;
		}
	}

	if( ( zero_count_old - zero_count ) > 500 )			bBoundary = 1;
	if( zero_count < (this_frame->max_h*this_frame->max_w/2) )	bBoundary = 1;
	if( writeFlag ) this_frame->isBoundary = bBoundary;
	zero_count_old = zero_count;

	One_end = time((time_t*)NULL);

	return this_frame->max_h* this_frame->max_w - zero_count;
}



int write_all_dat_info( struct MACRO_BLOCK_DATA *this_frame, int one_count, struct CTT *r, struct CTT *g, struct CTT *b, float duration, FILE *fd ){
	if( this_frame->frameNum == 1 )
		fprintf( fd, "%s", "Num,ONE,RGB_MI,Brightness,Y_Slope,HG_DIFF,HG_AI,Duration\n" );
	fprintf( fd, "%-6d, %d, %f,%f,%f,%f,%f,%f\n",
		this_frame->frameNum, one_count,
		r->mi + g->mi + b->mi,
		this_frame->brightness, this_frame->slope_brightness, 
		this_frame->hg_d, this_frame->hg_ai,duration) ;
	return 0;
}

float cal_slope_y( struct MACRO_BLOCK_DATA *this_frame, struct MACRO_BLOCK_DATA *old_frame){
	
	float slope_brightness;
	slope_brightness = (this_frame->brightness - old_frame->brightness) ;
	return slope_brightness;


	} 
	
float timedifference_msec(struct timeval t0, struct timeval t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0f + (t1.tv_usec - t0.tv_usec) / 1000.0f;
}
int boundaryCheck( VideoState *is, AVPicture *pict, const char *input_filename, enum MType m_type){
	static struct MACRO_BLOCK_DATA	this_frame, old_frame;
	static	int			fCount = 0;
	int				one_count = 0;
	struct CTT			ctt_y, ctt_r, ctt_g, ctt_b, ctt_s, ctt_sh, ctt_sw;
	float slope_y;
	struct timeval t0;
	struct timeval t1;
	float elapsed;
	memset( &this_frame, 0, sizeof( struct MACRO_BLOCK_DATA ) );

	if (m_type == M_TYPE_ISDF){
		gettimeofday(&t0, 0);
		block_luminance_calculation( is, pict, &this_frame, ++fCount );		
  	one_count = diff_frame( is, &this_frame, &old_frame, DIFF_LIMIT, 1);
  		this_frame.brightness = cal_raw_y75( &this_frame, MODE_Y);
		this_frame.slope_brightness = cal_slope_y(&this_frame,&old_frame);	  		
		gettimeofday(&t1, 0);
		elapsed = timedifference_msec(t0, t1);
		//printf("Code executed in %f milliseconds.\n", elapsed);
  	}else if (m_type == M_TYPE_MI){
  		gettimeofday(&t0, 0);
  		block_luminance_calculation( is, pict, &this_frame, ++fCount );			
	init_ctt( &this_frame, &old_frame, &ctt_r, MODE_R );
	init_ctt( &this_frame, &old_frame, &ctt_g, MODE_G );
	init_ctt( &this_frame, &old_frame, &ctt_b, MODE_B );
	cal_je_mi( &this_frame, &old_frame, one_count, &ctt_r, MODE_R );
	cal_je_mi( &this_frame, &old_frame, one_count, &ctt_g, MODE_G );
	cal_je_mi( &this_frame, &old_frame, one_count, &ctt_b, MODE_B );
		gettimeofday(&t1, 0);
		elapsed = timedifference_msec(t0, t1);	
		//printf("Code executed in %f milliseconds.\n", elapsed);				
  	}else if (m_type == M_TYPE_AID){
  		gettimeofday(&t0, 0);
  		block_luminance_calculation( is, pict, &this_frame, ++fCount );	
  		cal_histogram( &this_frame, &old_frame);
  		gettimeofday(&t1, 0);
  		elapsed = timedifference_msec(t0, t1);
  	}


	write_all_dat_info( &this_frame, one_count, &ctt_r, &ctt_g, &ctt_b, elapsed, fd_all);
	memcpy( &old_frame, &this_frame, sizeof( struct MACRO_BLOCK_DATA ) );
	return 0;
}

