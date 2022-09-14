#include "floatx.h"
#include <assert.h>
#include <limits.h> // for CHAR_BIT - number of bits per byte
#include <math.h> // for isinf and isnan
#include <stdio.h>

union hexDouble {
   	double dbl;
   	long lng;
}unionVal;
   
floatx doubleToFloatx(double val,int totBits,int expBits) {

	// First, make some assertions to ensure the totBits and expBits parameters are OK
	// Then, implement the algorithm
	
	//For 0 //32 bits of 0s
	//For Infinity Exps all 1s Mantissa all 0s
	//For NAN Exps all 1s, Mantissa has atleast one 1

	assert(totBits < 65 && totBits >= 3);
	assert(expBits > 0 && expBits <= 62);
	assert(expBits < totBits);
	
	int mantissa = ((totBits - expBits)-1);
	
	assert(mantissa != 0);
	
	if(val == 0.0f){ //pure float 0 INPUT
		unionVal.dbl = val;
		unsigned long mask = unionVal.lng;
		int i = 0;
		while(i < 63){ //Isolate leftmost bit
			mask = mask >> 1;
			i++;
		
		}
		
		if(mask == 0){
			return 0;
		}
		else{ // for -0
			unsigned long floatx = 1;
			i = 0;
			while(i < (totBits -1)){ 
				floatx = floatx << 1;
				i++;
			}
				
			return floatx;
		}
	}
	else if(isinf(val)){ //infinity direct INF INPUT
		
		unsigned long max_bits = pow(2,expBits)-1;
		unsigned long floatx = 0;
		int i = 0;
		
		while(i < mantissa){
			max_bits = max_bits << 1;
			i++;
		}
		if(val == -INFINITY){
			floatx = 1;
			i = 0;
			while(i < (totBits -1)){ 
				floatx = floatx << 1;
				i++;
			}
			
			return floatx + max_bits;
		}
	
		return floatx + max_bits;
	}
	else if(isnan(val)){ //nan direct NAN INPUT
		
		unsigned long max_bits = pow(2,expBits)-1;
		unsigned long floatx = 0;
		int i = 0;
		
		while(i < mantissa){
			max_bits = max_bits << 1;
			i++;
		}

		return floatx+ max_bits+pow(2,(mantissa-1));
		
	} 
	
	//1L<<pos & value
	
	int bias, i, sign;
	unionVal.dbl = val; //because c messed up bits
	unsigned long mask = 0; //64 bit unsigned
	unsigned long floatx = 0; //64 bit unsigned

	bias = pow(2,expBits-1) - 1;
	//printf("\nBias: %d", bias);
	mask = unionVal.lng;
	//Get Sign Bit from double 'val'
	i = 0;
	while(i < 63){ //Isolate leftmost bit
		mask = mask >> 1;
		i++;
	}
	if(mask == 1){
		sign = 1; //negative!
		//printf("NEGATIVE\n");
		
	}
	else{
		sign = 0; //postive!
		//printf("POSITIVE\n");
	}	
	
	floatx = sign;
	i= 0;
	while(i < (totBits -1)){ //Install sign bit at pos 32 willdo nothing if +
		floatx = floatx << 1;
		i++;
	}
	
	
	//Find Exp Sum of Double and unbias it
	int doubleBias = 1023;
	i = 0;
	mask = unionVal.lng;
	
	while(i < 53){ //Isolate exponential bits
		if(i == 0){
			mask = mask << 1; //truncate sign
		}
		mask = mask >> 1;
		
		i++;
	}
	int unbiased = mask-doubleBias;
	int rebiased = unbiased + bias;
	
	if(rebiased >= (pow(2,expBits)-1)){ //infinity naturally /overflow
	
		unsigned long max_bits = pow(2,expBits)-1;
		i = 0;	
		while(i < mantissa){
			max_bits = max_bits << 1;
			i++;
		}
		//floatx already has the sign bit installed
		
		return floatx + max_bits;
		
	}
	//else if(rebiased == 0){ //subnormal //optional
		//return 0;
		//subnormal case //super annoying and difficult to deal with may come back to this!
		//subnormal == rebiased == 0
	//}
	else if(rebiased < 0){ //underflow 0 //subnormal ex 32 8 float -126 sacrafices other subnormals but whatever
	//else if(unbiased <= -bias){
		//printf("\nRebiased Exp: %d ", rebiased); 
		//printf("Hello");
		//underflow < 0
		return 0;
	}
	
	i = 0;
	unsigned long biased_bits = rebiased;
	//while(i < 23){ //Add exponential bits besides sign bit (end)
	
	while(i < mantissa){
		biased_bits = biased_bits << 1;
		i++;
	}
	floatx= biased_bits + floatx; //now only frac should be missing
	
	
	
	//NOW finally rip 23 bits out of the double mantissa for regular floats!
	mask = unionVal.lng; //reset mask
	i = 0;
	//while(i < 29){ //Add exponential bits besides sign bit (end)
	
	if(mantissa == 52){ // For 64 Bit, 11 Exp exception //When mantissa is same
		mask = mask << 12;
		mask = mask >> 12; 
	}
	else if(mantissa > 52){ // When mantissa is greater, rarer
		mask = mask << 12;
		mask = mask >> 12; 	
		int surplus = mantissa - 52;
		
		while(i < surplus){
			mask = mask << 1;
			i++;
		}
	}
	else if(mantissa < 52){
		while(i < (52-mantissa)){ // When mantissa is smaller, almost always case
			if(i ==0){
				mask = mask << 12;
				mask = mask >> 12; //truncate the exponential bits and sign
			} // now we have a bunch of mantissa bits, 51
			mask = mask >> 1;
			i++;
		}
	}
	floatx = mask + floatx;
	/*
	printf("\nTotal Bits: %d", totBits);
	printf("\nExp Bits: %d", expBits);
	printf("\nFloatx: %ld ", floatx);
	printf("\nBiased Exp: %ld ", mask);
	printf("\nUnbiased Exp: %d ", unbiased);
	printf("\nRebiased Exp: %d ", rebiased); 
	printf("\nBiased_bits: %ld ", biased_bits);
	printf("\nFloatx2: %ld ", floatx); //checks out*/
	
	return floatx; 
}