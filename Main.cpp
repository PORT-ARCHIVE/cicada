
#include <iostream>
#include "SemiCrf.hpp"
#include "AppReqs.hpp"

void buildTmpTrainingDatas(SemiCrf::Datas datas)
{
	SemiCrf::Data data0(new SemiCrf::Data_());
	data0->getStrs()->push_back("AAA");
	data0->getStrs()->push_back("BBB");
	data0->getStrs()->push_back("CCC");
	data0->getStrs()->push_back("DDD");

	SemiCrf::Segment s0(new SemiCrf::Segment_( 0, 1, AppReqs::Label::Campany ));
	data0->getSegments()->push_back(s0);

	SemiCrf::Segment s1(new SemiCrf::Segment_( 2, 3, AppReqs::Label::Location ));
	data0->getSegments()->push_back(s1);

	datas->push_back(data0);

	SemiCrf::Data data1(new SemiCrf::Data_());
	data1->getStrs()->push_back("AAA");
	data1->getStrs()->push_back("BBB");
	data1->getStrs()->push_back("CCC");
	data1->getStrs()->push_back("DDD");

	SemiCrf::Segment s2(new SemiCrf::Segment_( 0, 1, AppReqs::Label::Campany ));
	data1->getSegments()->push_back(s2);

	SemiCrf::Segment s3(new SemiCrf::Segment_( 2, 3, AppReqs::Label::Location ));
	data1->getSegments()->push_back(s3);

	datas->push_back(data1);
}

void buildTmpInferenceDatas(SemiCrf::Datas datas)
{
	SemiCrf::Data data0(new SemiCrf::Data_());
	data0->getStrs()->push_back("AAA");
	data0->getStrs()->push_back("BBB");
	data0->getStrs()->push_back("CCC");
	data0->getStrs()->push_back("DDD");
	datas->push_back(data0);

	SemiCrf::Data data1(new SemiCrf::Data_());
	data1->getStrs()->push_back("AAA");
	data1->getStrs()->push_back("BBB");
	data1->getStrs()->push_back("CCC");
	data1->getStrs()->push_back("DDD");
	datas->push_back(data1);
}

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

		ffs->read();
		weights->read();

		SemiCrf::Datas trainingDatas(new SemiCrf::Datas_());
		trainingDatas->read();
	
		SemiCrf::Algorithm learner(new SemiCrf::Learner());
		learner->setLabels(labels);
		learner->setMaxLength(maxLength);

		buildTmpTrainingDatas(trainingDatas);
		learner->setDatas(trainingDatas);

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
		//weights->read();

		SemiCrf::Datas inferenceDatas(new SemiCrf::Datas_());
		inferenceDatas->read();

		SemiCrf::Algorithm inferer(new SemiCrf::Inferer());
		inferer->setLabels(labels);
		inferer->setMaxLength(maxLength);

		buildTmpInferenceDatas(inferenceDatas);
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
