#include "config.hh"



/// Macros for number of rows per bank in a hugepage
#define N_ROW_HUGEPAGE HUGEPAGE_SIZE/(ROW_SIZE*N_BANKS)

#define RUN_SIZE 1


FILE* hammer_log;

void fill_row(rowmap rm, int bit_value){
	uint64_t fill_value, value;
	volatile uint64_t* start_addr;
	if(bit_value){
		fill_value = 0xffffffffffffffff;
	} else { fill_value = 0;}
	start_addr = (uint64_t*)rm.v_addr;

	for( int i=0 ; i< ROW_SIZE/sizeof(fill_value); i++){
		*(start_addr + i) = fill_value;
		//cross check the value written to make sure compiler does not optimize any write operation
		value = *(start_addr+i);
		assert(value == fill_value);

	}

	
}

void check_row(rowmap rm, int bit_value){
	uint64_t check_value, value;
	volatile uint64_t* start_addr;

	if(bit_value){
		check_value = 0xffffffffffffffff;
	} else {
		check_value = 0;
	}

	start_addr = (uint64_t*) rm.v_addr;
	for( int i=0; i< ROW_SIZE/sizeof(check_value); i++){
		value = *(start_addr +i);
		if(check_value != value){
			fprintf(hammer_log, "[FLIP] Bit flip observed for row =%lx, bank =%lx, col=%lx, Expected Value = %lx, Actual Value = %lx\n", rm.row, rm.bank, i,  check_value, value);
			fflush(hammer_log);
		}

	}
}
	


void hammer(rowmap agg1, rowmap agg2, rowmap victim, int rounds, int pattern){
	uint64_t time ,t1, t2;
	uint64_t* haddr1;
	uint64_t* haddr2;
	assert(agg1.bank == agg2.bank);
	assert(agg1.bank == victim.bank);
	if(pattern == IOI){
		fprintf(hammer_log, "HAMMER_START ::  Bank=%lx, agg_row1 =%lx , agg_row2 =%lx, victim_row =%lx, pattern=%s\n",agg1.bank, agg1.row, agg2.row, victim.row, "101");
	} else if(pattern == OIO){
		fprintf(hammer_log, "HAMMER_START  ::  Bank=%lx agg_row1 =%lx , agg_row2 =%lx, victim_row =%lx, pattern=%s\n",agg1.bank, agg1.row, agg2.row, victim.row, "010" );
	}
	fflush(hammer_log);
	if(pattern == IOI){
		fill_row(victim, 0);
		fill_row(agg1, 1);
		fill_row(agg2, 1);
	} else if(pattern == OIO){
		fill_row(victim, 1);
		fill_row(agg1, 0);
		fill_row(agg2, 0);
	}

	//Hammering and profiling
	haddr1 = (uint64_t*)agg1.v_addr;
	haddr2 = (uint64_t*)agg2.v_addr;
	
	mfence();
	t1 = realtime_now();
	for ( int i = 0; i < rounds;  i++) {
		mfence();
		*(volatile char*) haddr1;
		*(volatile char*) haddr2;
		clflushopt(haddr1);
		clflushopt(haddr2);
	}
	t2 = realtime_now();
	mfence();


	//Checking 
	if(pattern == IOI){
		check_row(victim, 0);
	} else if(pattern == OIO){
		check_row(victim, 1);
	}

	time = (t2-t1)/1000000;
	if(pattern == IOI){
		fprintf(hammer_log, "HAMMER_END  ::  Bank=%lx, agg_row1 =%lx , agg_row2 =%lx, victim_row =%lx, pattern=%s, time taken =%ld ms \n",agg1.bank, agg1.row, agg2.row, victim.row, "101", time  );
		fprintf(hammer_log, "==============================================================================================================================================================\n\n"  );
	} else if(pattern == OIO){
		fprintf(hammer_log, "HAMMER_END  ::  Bank=%lx, agg_row1 =%lx , agg_row2 =%lx, victim_row =%lx, pattern=%s, time taken =%ld ms \n",agg1.bank, agg1.row, agg2.row, victim.row, "010", time  );
	}
	fflush(hammer_log);


}
	

// Hammering all the rows in a hugepage.
int	vulnerable_rows[] = {
				8730 ,
				36656 ,
				43942 ,
				10404 ,
				43941 ,
				31892 ,
				49198 ,
				32911 ,
				78354 ,
				56465 ,
				23962 ,
				39348 ,
				21771 ,
				17678 ,
				35363 ,
				63669 ,
				75286 ,
				56716 ,
				17674 ,
				9897 ,
				37546 ,
				5898 ,
				36245 ,
				37408 ,
				48796 ,
				430519 ,
				14632 ,
				57897 ,
				46769 ,
				7732 ,
				3612 ,
				47637 ,
				33807 ,
				49461 ,
				17946 ,
				37266 ,
				28564 ,
				6664 ,
				52395 ,
				21512 ,
				52404 ,
				44698 ,
				35377 ,
				36377 ,
				36897 ,
				35339 ,
				29613 ,
				8738 ,
				36006 ,
				44825 ,
				29456 ,
				27945 ,
				11784 ,
				52003 ,
				10519 ,
				72611 ,
				8712 ,
				32530 ,
				64272 ,
				53512 ,
				34579 ,
				37259 ,
				21515 ,
				2845 ,
				34594 ,
				34592 ,
				17708 ,
				43560 ,
				35244 ,
				23344 ,
				37260 ,
				23348 ,
				12305 ,
				41227 ,
				42396 ,
				45452 ,
				40885 ,
				29869 ,
				38822 ,
				57245 ,
				37286 ,
				27690 ,
				7181 ,
				444982 ,
				42637 ,
				36873 ,
				36507 ,
				37281 ,
				7084 ,
				27671 ,
				13842 ,
				34572 ,
				4766 ,
				30758 ,
				42540 ,
				427276 ,
				35113 ,
				6696 ,
				62984 ,
				35346 ,
				};



bool check_vulnerable_row(rowmap victim){

	for(int i=0; i< 100; i++){
		int bank = vulnerable_rows[i]%8;
		int row = int(vulnerable_rows[i]/8);
		if((bank == victim.bank)&& (row==victim.row)) {
			fprintf(hammer_log, "Vulnerable Row Found Row =%0d, Bank=%0d\n", victim.row, victim.bank);
			fflush(hammer_log);
			
			return true;
		}
	}
	return false;
}	



int main(int argc, char** argv) {
	uint64_t* start_addr;
	FILE* addr_log;
	FILE* coverage_log;
	int rounds;
	int run_size;



	if(argc>1){
		rounds = atoi(argv[1]);
	} else{
		rounds = HCOUNT;
	}

	if(argc>2){
		run_size = atoi(argv[2]);
	} else {
		run_size = RUN_SIZE;
	}

	start_addr = new uint64_t [run_size];

	coverage_log = fopen("run_data/run_coverage.log", "w");
	if(coverage_log ==0){
		fprintf(hammer_log, "Not able to open coverage_log .....  exiting\n");
		fflush(hammer_log);
		exit(1);
	}

	addr_log = fopen("run_data/addr.log", "w");
	if(addr_log ==0){
		fprintf(hammer_log, "Not able to open addr_log .....  exiting\n");
		fflush(hammer_log);
		exit(1);
	}
	assert(run_size< N_HUGEPAGE);
	hammer_log= fopen("run_data/hammer.log", "w");
	if(hammer_log ==0){
		printf("Not able to open hammer_log .....  exiting\n");
		exit(1);
	}

	
	//Allocate hugepages using mmap
	for( int num=0; num<run_size; num++){
		start_addr[num] = (uint64_t)mmap(NULL, HUGEPAGE_SIZE, PROT_READ|PROT_WRITE,MAP_PRIVATE | MAP_POPULATE | MAP_ANONYMOUS | MAP_HUGETLB | (21 << MAP_HUGE_SHIFT),  NULL, 0) ;
	
		if((uint64_t*)start_addr[num] == NULL){
			fprintf(hammer_log, "Mapping failed for index = %0d\n", num);
			fflush(hammer_log);
			exit(1);
		}

		if((start_addr[num]%ROW_SIZE) != 0){
			fprintf(hammer_log, "Addr from mmap %lx is not aligned to row boundary %ll\n", start_addr[num], ROW_SIZE);
			fflush(hammer_log);
			exit(1);
		}

			
	}
	for (int num=0; num< run_size; num++){
		fprintf(addr_log, "===================================================== Run Index =%0d ================================================================\n", num);
		fflush(addr_log);
		rowmap  rm[N_BANKS][N_ROW_HUGEPAGE];
		rowmap rm_temp;
		int row =0;
		int bank =0;

		rm_temp = get_physical_mapping(start_addr[num]);
		int base_row = rm_temp.row;
		for(uint64_t addr= start_addr[num]; addr< start_addr[num]+ HUGEPAGE_SIZE; addr = addr + ROW_SIZE){
		       //printf(" Row = %0d, Bank = %0d\n", row, bank);
		       rm_temp = get_physical_mapping(addr);
		       row= rm_temp.row - base_row;
		       bank = rm_temp.bank;
		       rm[bank][row] = rm_temp;
		       fprintf(addr_log, "%0d|%x|%lx|%lx|%lx|%lx\n",bank, row, rm[bank][row].v_addr, rm[bank][row].p_addr, rm[bank][row].bank, rm[bank][row].row);
		       fflush(addr_log);

	       	}
		//cross check the mappings using bank conflicts
		//check_mappings(rm);
		for(row =0; row< N_ROW_HUGEPAGE-2; row++){
			rowmap agg1, agg2, victim;
			for(bank=0; bank<N_BANKS; bank++){
				agg1 = rm[bank][row];
				victim = rm[bank][row+1];
				agg2 = rm[bank][row+2];
				//if(check_vulnerable_row(victim)){
				//	for(int times=0; times<50; times++){
						hammer(agg1, agg2, victim, rounds, OIO);
						hammer(agg1, agg2, victim, rounds,  IOI);
				//	}
				//}
			}
			fprintf(coverage_log, "%lx\n", victim.row);
		}
 	}
	
	
	
		

	fclose(addr_log);
	fclose(hammer_log);
	fclose(coverage_log);
	delete [] start_addr;
}
		




	
	



