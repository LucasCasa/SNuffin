#ifndef _DB_CONST_H
#define _DB_CONST_H

#define MUTEX_OFFSET 1024
#define COND_OFFSET  1024 + sizeof(pthread_mutex_t)
#define COND2_OFFSET COND_OFFSET + sizeof(pthread_cond_t)
#define SH_SIZE      COND2_OFFSET + sizeof(pthread_cond_t)

#define ISUSER          '1'
#define ISPASSWORD      '2'
#define GETHIGHSCORE    '3'
#define SETHIGHSCORE    '4'
#define CREATEUSER      '5'
#define READY           0

#define RETURNPOS       256
#define FIRSTARGUMENT   1
#define SECONDARGUMENT  64
#define TYPEPOS         0
#endif
