#ifndef neuronal_networkH
#define neuronal_networkH

#include "../general/random.h"
#include "vector.h"

template <count_t InSize, count_t OutSize>
	class NeuronalLayer
	{
	public:
			TVec<float,OutSize>					col[InSize],
												output;
			TVec<float,InSize>					desired_input;
	
	
			void								randomize(Random&random)
			{
				for (index_t i = 0; i < ARRAYSIZE(col); i++)
					for (index_t j = 0; j < OutSize; j++)
						col[i].v[j] = random.getFloat();
			}
			
			void								evaluate(const TVec<float,InSize>&input)
			{
				Vec::clear(output);
				for (index_t i = 0; i < InSize; i++)
					Vec::mad(output,col[i],input.v[i]);
				for (index_t i = 0; i < OutSize; i++)
					output.v[i] = convertOut(output.v[i]);
			}
			
			
			void								adjust(const TVec<float,OutSize>&desired_output, float learning_factor)
			{
				
			
			
			
			
			}
			
			
			
	};


template <count_t InSize, count_t HiddenSize, count_t OutSize>
	class NeuronalNetwork
	{
	public:
			NeuronalLayer<InSize,HiddenSize>	matrix0;
			NeuronalLayer<HiddenSize,OutSize>	matrix1;
	
	
			const TVec<float,OutSize>&			evaluate(const TVec<float,InSize>&input)
			{
				matrix0.evaluate(input);
				matrix1.evaluate(matrix0.output);
				return matrix1.output;
			}
	
	
			void								adjust(const TVec<float,OutSize>&desired_output, float learning_factor)
			{
				matrix1.adjust(desired_output, learning_factor);
				matrix0.adjust(matrix1.desired_input,learning_factor);
			}
	
	
	};








#endif
