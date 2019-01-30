#include "phch_recv.h"
#include <stdio.h>
	sync_state phy_state;
void * run_thread(void){
	
	bool running = true;

	while(running){
		switch(phy_state.run_state()){
			case sync_state::CELL_SEARCH:
				printf("I'm in cell search\n");
				phy_state.state_exit();
				break;
			case sync_state::SFN_SYNC:
				printf("I'm in sfn_sync\n");
				phy_state.state_exit();
				break;
			case sync_state::CAMPING:
				printf("I'm in camping\n");
				break;
			case sync_state::IDLE:
				printf("I'm in idle\n");
				break;		
		}
		
	}

}
