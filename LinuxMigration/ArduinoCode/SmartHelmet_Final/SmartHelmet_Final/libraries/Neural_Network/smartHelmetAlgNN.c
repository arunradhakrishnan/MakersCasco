#include <stdio.h>
#include "floatfann.h"

const int nInputs = 50;
int main()
{
	fann_type *calc_out;
	fann_type input[nInputs];
    int noi = 0; //number of lines, number of inputs
	FILE* fd = NULL;
	FILE* out= NULL;
	
	struct fann *ann = fann_create_from_file("algoritmos.net");
    

	
	fd = fopen("neuralInput.txt","r");
	
	
	
    
    
    
	
    if(fd){
		
		out = fopen("neuralOutput.txt","w");
		
		for(noi = 0; noi < nInputs ; noi++){
			fscanf(fd, "%f", &input[noi]);
				
		}


		calc_out = fann_run(ann, input);
		fprintf(out,"%f %f",  calc_out[0], calc_out[1]);
		
		fclose(fd);
		fclose(out);
    }
	
    
    
	fann_destroy(ann);
	return 0;
}
