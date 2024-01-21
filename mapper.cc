#include "config.hh"
int get_bit(uint64_t p_addr, uint64_t bit_no){
	return (p_addr>>bit_no)& 0x1;
}

	

rowmap get_physical_mapping( uint64_t v_addr){
	uint64_t p_addr;
	rowmap rm;

	rm.v_addr = v_addr;
	
	p_addr = virt_to_phys(v_addr);
	rm.p_addr = p_addr;
	rm.bank =0;


#ifdef CONFIG_2x8x16x10

	//Mapping for 2 x 8 x 16 x 10
	rm.bank =  (get_bit(p_addr, 13) ^ get_bit(p_addr, 17)) | ((get_bit(p_addr, 14) ^ get_bit(p_addr, 18)) <<1) | ((get_bit(p_addr, 15) ^ get_bit(p_addr, 19))<<2) | ((get_bit(p_addr, 16) ^ get_bit(p_addr, 20))<<3)   ;
	rm.row = ((p_addr>>17) & (0xffff));
#endif

#ifdef CONFIG_1x8x16x10
	//Mapping for 1 x 8 x 16 x 10
	rm.bank =  (get_bit(p_addr, 13) ^ get_bit(p_addr, 16) | ((get_bit(p_addr, 14) ^ get_bit(p_addr, 17)) <<1) | ((get_bit(p_addr, 15) ^ get_bit(p_addr, 18))<<2)  ;
	rm.row = ((p_addr>>16) & (0xffff));

#endif 


	return rm;



}










