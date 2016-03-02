
#include <iostream>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"

int main(int argc, char *argv[])
{
	int maxLength = 5;
	SemiCrf::Weights weights( new SemiCrf::Weights_() );
	SemiCrf::FeatureFunctions ffs( new SemiCrf::FeatureFunctions_() );
	SemiCrf::Labels labels( new SemiCrf::Labels_() );

	try { // 素性関数,Labelを準備する

		// T.B.D.
		typedef SemiCrf::FeatureFunction SFF;	
		SFF ff0(new AppReqs::AppReqF0());
		SFF ff1(new AppReqs::AppReqF1());

		ffs->push_back(ff0);
		ffs->push_back(ff1);

		labels->push_back(AppReqs::Label::None);
		labels->push_back(AppReqs::Label::Campany);
		labels->push_back(AppReqs::Label::Location);
		// T.B.D.	

	} catch(...) {
		// T.B.D.
	}

	try { // 学習
	
		SemiCrf::Datas trainingDatas(new SemiCrf::Datas_());
		//trainingData->read(); T.B.D.
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->setLabels(labels);
		learner->setMaxLength(maxLength);
		//learner->setData(trainingData);
		learner->setDatas(trainingDatas);
		learner->setFeatureFunctions(ffs);
		learner->setWeights(weights);
		learner->compute();

		ffs->write();
		weights->write();

	} catch(...) {
		// T.B.D.
	}

	try { // 推論

		ffs->read();
		weights->read();

		SemiCrf::Datas inferenceDatas(new SemiCrf::Datas_());
		inferenceDatas->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->setLabels(labels);
		inferer->setMaxLength(maxLength);
		inferer->setDatas(inferenceDatas);
		inferer->setDatas(inferenceDatas);
		inferer->setFeatureFunctions(ffs);
		inferer->setWeights(weights);		
		inferer->compute();
		
		//inferenceData->write(); T.B.D.

	} catch(...) {
		// T.B.D.		
	}
		
	return (0);
}
