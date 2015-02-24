#include "rdwr_lock.h"

int pthread_rdwr_init_np(pthread_rdwr_t *rdwrp, pthread_rdwrattr_t  *attrp )	
{	
          rdwrp->readers_reading = 0;	
          rdwrp->writer_writing = 0;	
          pthread_mutex_init(&(rdwrp->mutex), NULL);	
          pthread_cond_init(&(rdwrp->lock_free), NULL);	
          return 0;	
}

int pthread_rdwr_rlock_np(pthread_rdwr_t *rdwrp)	
{	  int retlock;
          if((retlock = pthread_mutex_lock(&(rdwrp->mutex))) != 0) {
          	errno = retlock;
          	perror("rdwr_lock.c:Error in acquiring mutex lock;");
          	return -1;
          }	
          while(rdwrp->writer_writing) {	
                  pthread_cond_wait(&(rdwrp->lock_free), &(rdwrp->mutex));	
          }	
          rdwrp->readers_reading++;	
          if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
          	errno = retlock ;
          	perror("rdwr_lock.c: Error in unlocking mutex lock");
          	return -1;
          }	
          return 0;	
}

int pthread_rdwr_wlock_np(pthread_rdwr_t *rdwrp)	
{	  int retlock;
	if((retlock = pthread_mutex_lock(&(rdwrp->mutex))) != 0) {
          	errno = retlock;
          	perror("rdwr_lock:Error in acquiring mutex lock;");
          	return -1;
          }		
          while (rdwrp->writer_writing || rdwrp->readers_reading) {	
                   pthread_cond_wait(&(rdwrp->lock_free), &(rdwrp->mutex));	
          }	
          rdwrp->writer_writing++;	
          if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
          	errno = retlock ;
          	perror("rdwr_lock.c: Error in unlocking mutex lock");
          	return -1;
          }
          return 0;	
}

int pthread_rdwr_runlock_np(pthread_rdwr_t *rdwrp) {	
          int retlock;
          if((retlock = pthread_mutex_lock(&(rdwrp->mutex))) != 0) {
          	errno = retlock;
          	perror("rdwr_lock:Error in acquiring mutex lock;");
          	return -1;
          }	
          
          if (rdwrp->readers_reading == 0) {	
		if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
        	  	errno = retlock ;
        	  	perror("rdwr_lock.c: Error in unlocking mutex lock");
        	  	return -1;
        	}
	        return -1;	
          } else {	
                    rdwrp->readers_reading--;	
                    if (rdwrp->readers_reading == 0)	
                              pthread_cond_signal(&(rdwrp->lock_free));	
		    if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
          		errno = retlock ;
          		perror("rdwr_lock.c: Error in unlocking mutex lock");
          		return -1;
          	}
                    return 0;	
          }	
}

int pthread_rdwr_wunlock_np(pthread_rdwr_t *rdwrp) {	
         
         int retlock;
         
          if((retlock = pthread_mutex_lock(&(rdwrp->mutex))) != 0) {
          	errno = retlock;
          	perror("rdwr_lock:Error in acquiring mutex lock;");
          	return -1;
          }	          
          if (rdwrp->writer_writing == 0) {	
                    if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
          		errno = retlock ;
          		perror("rdwr_lock.c: Error in unlocking mutex lock");
          		return -1;
          	}
                    return -1;	
          } else {	
                    rdwrp->writer_writing = 0;	
                    pthread_cond_broadcast(&(rdwrp->lock_free));	
		    if((retlock = pthread_mutex_unlock(&(rdwrp->mutex))) != 0) {
          		errno = retlock ;
          		perror("rdwr_lock.c: Error in unlocking mutex lock");
          		return -1;
          	    }
                    return 0;	
          }	
}
	
	
