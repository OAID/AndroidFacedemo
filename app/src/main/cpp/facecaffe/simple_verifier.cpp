#include <malloc.h>
#include <string.h>

#include <cmath>
#include <vector>

#include "face_verify.hpp"

cosine_distance_verifier::~cosine_distance_verifier(void)
{
    for(int i=0;i<feature_db_.size();i++)
    {
          free(feature_db_[i].p_feature);
    }
}

/* cosine distance */
float cosine_distance_verifier::compare(float * f0, float * f1, int len)
{
	float  product=0;
	float l2_sum0=0;
	float  l2_sum1=0;

	float score;


	for(int i=0;i<len;i++)
	{
		product+=f0[i]*f1[i];
		l2_sum0+=f0[i]*f0[i];
		l2_sum1+=f1[i]*f1[i];
	}

	score=product/sqrt(l2_sum0)/sqrt(l2_sum1);

	return score;
}

int cosine_distance_verifier::search(float * f, int * p_face_id, float * p_score)
{

	p_score[0]=0;

	for(int i=0;i<feature_db_.size();i++)
	{
                face_pair& e=feature_db_[i];

		float score=compare(f,e.p_feature,feature_len_);      

		if(score>p_score[0])
		{
			p_score[0]=score;
			p_face_id[0]=e.face_id;
		}

	}

         if(p_score[0]>0.55 && p_score[0]<0.81)
                p_score[0]=0.81;

	return 0;
}


int cosine_distance_verifier::insert_feature(float * feature, unsigned int face_id)
{
      face_pair fp;
    
      //enable override old record of the same face_id
      remove_feature(face_id);

      fp.p_feature=(float *)malloc(sizeof(float)*feature_len_);
      memcpy(fp.p_feature,feature,feature_len_*sizeof(float));
      fp.face_id=face_id;

      feature_db_.push_back(fp);

      return 0;
}


void cosine_distance_verifier::remove_feature(unsigned int face_id)
{
    std::vector<face_pair>::iterator it=feature_db_.begin();

   while(it!=feature_db_.end())
   {
       if(it->face_id==face_id)
       {
           free(it->p_feature);
           feature_db_.erase(it);
           break;
       }  
       it++;
   }
}


static face_verifier * cosine_distance_verifier_creator(const std::string& name)
{
	return  new cosine_distance_verifier();
}

REGISTER_SIMPLE_VERIFIER(cosine_distance,cosine_distance_verifier_creator);

