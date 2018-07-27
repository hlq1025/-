#include "match.h"
Match::Match(bool try_gpu, double match_conf)
{
	this->try_gpu = try_gpu;
	this->match_conf = match_conf;
}
Match::~Match()
{

}
void Match::feature_match(vector<ImageFeatures> features)
{
	
	BestOf2NearestMatcher matcher(try_gpu, match_conf);
	matcher(features, pairwise_matches);
}