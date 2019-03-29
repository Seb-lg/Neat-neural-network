//
// Created by seb on 24/02/19.
//

#pragma once

struct MutationRate {
	MutationRate():
		connectionMutateChance(2.0),
		weightMutationChance(3.0),
		perturbChance(0.2),
		nodeMutationChance(0.75),
		stepSize(0.3),
		disableMutationChance(0.1),
		enableMutationChance(0.3){
	}
	double connectionMutateChance;
	double weightMutationChance;
	double perturbChance;
	double nodeMutationChance;
	double stepSize;
	double disableMutationChance;
	double enableMutationChance;

/*///	The disabled bois

//	double crossoverChance;
//	double linkMutationChance;
//	double biasMutationChance;*/
};

struct SpeciatingParameter {
	SpeciatingParameter():
		population(100),
		deltaDisjoint(2.0),
		deltaWeights(0.4),
		deltaThreshold(1.3),
		parentPercent(10.0),
		staleSpecies(15) {

	}
	unsigned int	population;
	double		deltaDisjoint;
	double		deltaWeights;
	double		deltaThreshold;
	double		parentPercent;
	unsigned int	staleSpecies;
};

struct NetworkInfo {
	unsigned int	inputSize;
	unsigned int	biasSize;
	unsigned int	outputSize;
};