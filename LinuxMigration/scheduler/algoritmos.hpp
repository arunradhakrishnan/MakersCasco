#include <stdio.h>
#include "floatfann.h"

class NeuralNetwork{
	
	
	struct fann *ann;
	public:
	void initNN(char file_name[])
	{
		ann = fann_create_from_file(file_name);
	}
	
	fann_type* run(fann_type input[])
	{
		return fann_run(ann, input);
	}
	
	
};
