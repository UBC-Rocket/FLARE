/*GPS Venus638FLPx Driver Header*/

#ifndef Venus638FLPx_h
#define Venus638FLPx_h

/*Includes------------------------------------------------------------*/

/*Constants------------------------------------------------------------*/
#define SENTENCE_SIZE   100
#define GPS_TIMEOUT     100

/*Variables------------------------------------------------------------*/

/*Functions------------------------------------------------------------*/
bool updateGPS(void);
void getGPS(float*);

#endif
