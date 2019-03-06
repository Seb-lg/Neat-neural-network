//
// Created by seb on 24/02/19.
//

#pragma once

struct MutationRate {
	MutationRate():
		connectionMutateChance(2.0),
		weightMutationChance(2.0),
		perturbChance(0.15),
		nodeMutationChance(0.01),
		stepSize(0.40),
		disableMutationChance(0.2),
		enableMutationChance(0.2){
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
		population(240),
		deltaDisjoint(2.0),
		deltaWeights(0.4),
		deltaThreshold(1.3),
		staleSpecies(15) {

	}
	unsigned int	population;
	double		deltaDisjoint;
	double		deltaWeights;
	double		deltaThreshold;
	unsigned int	staleSpecies;
};

struct NetworkInfo {
	unsigned int	inputSize;
	unsigned int	biasSize;
	unsigned int	outputSize;
};