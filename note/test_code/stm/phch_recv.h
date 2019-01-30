#ifndef PHCH_RECV_H
#define PHCH_RECV_H
#include <stdio.h>
#include <pthread.h>

class sync_state {
   public:
    typedef enum {
      IDLE = 0,
      CELL_SEARCH,
      SFN_SYNC,
      CAMPING,
    } state_t;

    /* Run_state is called by the main thread at the start of each loop. It updates the state
     * and returns the current state
     */
    state_t run_state() {
      pthread_mutex_lock(&inside);
      cur_state = next_state;
      if (state_setting) {
        state_setting  = false;
        state_changing = true;
      }
      pthread_cond_broadcast(&cvar);
      pthread_mutex_unlock(&inside);
      return cur_state;
    }

    // Called by the main thread at the end of each state to indicate it has finished.
    void state_exit(bool exit_ok = true) {
      pthread_mutex_lock(&inside);
      if (cur_state == SFN_SYNC && exit_ok == true) {
        next_state = CAMPING;
      } else {
        next_state = IDLE;
      }
      state_changing = false;
      pthread_cond_broadcast(&cvar);
      pthread_mutex_unlock(&inside);
    }
    void force_sfn_sync() {
      pthread_mutex_lock(&inside);
      next_state = SFN_SYNC;
      pthread_mutex_unlock(&inside);
    }

    /* Functions to be called from outside the STM thread to instruct the STM to switch state.
     * The functions change the state and wait until it has changed it.
     *
     * These functions are mutexed and only 1 can be called at a time
     */
    void go_idle() {
      pthread_mutex_lock(&outside);
      go_state(IDLE);
      pthread_mutex_unlock(&outside);
    }
    void run_cell_search() {
      pthread_mutex_lock(&outside);
      go_state(CELL_SEARCH);
      wait_state_change(CELL_SEARCH);
      pthread_mutex_unlock(&outside);
    }
    void run_sfn_sync() {
      pthread_mutex_lock(&outside);
      go_state(SFN_SYNC);
      wait_state_change(SFN_SYNC);
      pthread_mutex_unlock(&outside);
    }


    /* Helpers below this */
    bool is_idle() {
      return cur_state == IDLE;
    }
    bool is_camping() {
      return cur_state == CAMPING;
    }

    const char *to_string() {
      switch(cur_state) {
        case IDLE:
          return "IDLE";
        case CELL_SEARCH:
          return "SEARCH";
        case SFN_SYNC:
          return "SYNC";
        case CAMPING:
          return "CAMPING";
        default:
          return "UNKNOWN";
      }
    }

    sync_state() {
      pthread_mutex_init(&inside, NULL);
      pthread_mutex_init(&outside, NULL);
      pthread_cond_init(&cvar, NULL);
      cur_state = IDLE;
      next_state = IDLE;
      state_setting = false;
      state_changing = false;
    }
   private:

    void go_state(state_t s) {
      pthread_mutex_lock(&inside);
      next_state = s;
      state_setting = true;
      while(state_setting) {
        pthread_cond_wait(&cvar, &inside);
      }
      pthread_mutex_unlock(&inside);
    }

    /* Waits until there is a call to set_state() and then run_state(). Returns when run_state() returns */
    void wait_state_change(state_t prev_state) {
      pthread_mutex_lock(&inside);
      while(state_changing) {
        pthread_cond_wait(&cvar, &inside);
      }
      pthread_mutex_unlock(&inside);
    }

    bool state_changing, state_setting; 
    state_t cur_state, next_state;
    pthread_mutex_t inside, outside;
    pthread_cond_t  cvar;

  };
#endif

