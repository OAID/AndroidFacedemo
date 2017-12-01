
#include "face_demo.hpp"
#include "caffe_mtcnn.hpp"
#include "feature_extractor.hpp"
#include "lightened_cnn.hpp"
#include "face_verify.hpp"

#include <string.h>
#include <stdlib.h>
#include <android/log.h>

#ifndef MODEL_DIR
#define MODEL_DIR "/mnt/sdcard/openailab/models"
#endif
#include "conf/stdtostring.h"
#define UNKNOWN_FACE_ID_MAX 1000

#define  LOG_TAG    "JNI_PART"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG_TAG, __VA_ARGS__)

int Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest);
void draw_box_and_title(cv::Mat& frame, face_box& box, char * title);

class only_for_extractor_auto_register
{
public:
   only_for_extractor_auto_register(const std::string& name, extractor_factory::creator creator)
   {
      extractor_factory::register_creator(name,creator);
   } 
      
};

static mtcnn * caffe_creator(void)
{
	return new caffe_mtcnn();
}

static feature_extractor * lightened_cnn_creator(const std::string& name)
{
      return new lightened_cnn(name);
}

static face_verifier * cosine_distance_verifier_creator(const std::string& name)
{
	return  new cosine_distance_verifier();
}

int main()
{
	FaceDemo face_demo;
	cv::Mat frame = cv::imread("test.jpg");
	face_demo.Init(0.9,0.9,0.9,0.6,32);
	cv::waitKey(5);
	
	std::string ret = face_demo.Recognize(frame);
	std::cout << "1: " << ret << std::endl;
	face_demo.Register(frame, std::string("She"));
	ret = face_demo.Recognize(frame);
	std::cout << "2: " << ret << std::endl;
	face_demo.LocalSave("");
	ret = face_demo.Recognize(frame);
	std::cout << "3: " << ret << std::endl;
	face_demo.Delete("She");
	ret = face_demo.Recognize(frame);
	std::cout << "4: " << ret << std::endl;
	face_demo.LocalLoad("");
	ret = face_demo.Recognize(frame);
	std::cout << "5: " << ret << std::endl;	
	cv::imshow("test",frame);
	face_demo.Quit();
}

/*****************************************************************************************************/
int FaceDemo::Init(double threshold_p, double threshold_r, double threshold_o, double factor, int mim_size)
{
	const char * type="caffe";
	
	REGISTER_MTCNN_CREATOR(caffe,caffe_creator);
	
	p_mtcnn=mtcnn_factory::create_detector(type);
	std::string model_dir = MODEL_DIR;	

	if(p_mtcnn==nullptr)
	{
		std::cerr<<type<<" is not supported"<<std::endl;
		std::cerr<<"supported types: ";
		std::vector<std::string> type_list=mtcnn_factory::list();

		for(int i=0;i<type_list.size();i++)
			std::cerr<<" "<<type_list[i];

		std::cerr<<std::endl;

		LOGD("[%s] p_mtcnn is NULL.....\n", __FUNCTION__);
		return -1;
	}
	p_mtcnn->load_model(model_dir);
	p_mtcnn->set_threshold(threshold_p,threshold_r,threshold_o);
	p_mtcnn->set_factor_min_size(factor,mim_size);

	/* alignment */

	/* extractor */

	static only_for_extractor_auto_register dummy_instance("lightened_cnn",lightened_cnn_creator);
	
	//extractor_factory::register_creator("lightened_cnn",lightened_cnn_creator);
	
	const std::string extractor_name("lightened_cnn");

	p_extractor=extractor_factory::create_feature_extractor(extractor_name);

	if(p_extractor==nullptr)
	{
		std::cerr<<"create feature extractor: "<<extractor_name<<" failed."<<std::endl;
		
		LOGD("[%s] p_extractor is NULL.....\n", __FUNCTION__);

		return -2;
	}
	
	p_extractor->load_model(model_dir);

    REGISTER_SIMPLE_VERIFIER(cosine_distance,cosine_distance_verifier_creator);

	/* verifier*/
	p_verifier=get_face_verifier("cosine_distance");
	
	
	p_verifier->set_feature_len(p_extractor->get_feature_length());	

	/* store */
	p_mem_store=new face_mem_store(256,10);
	
	return 0;
}

std::string FaceDemo::Recognize(cv::Mat &frame)
{
	std::vector<face_box> face_info;
	std::string result = "";
	current_frame_count++;
	frame.copyTo(p_cur_frame);
	int face_count = 0;

	p_mtcnn->detect(frame,face_info);

	for(unsigned int i=0;i<face_info.size();i++)
	{
		face_box& box=face_info[i];
		get_face_title(frame,box,current_frame_count);
	}
	for(unsigned int i=0;i<face_win_list.size();i++)
	{
		if(face_win_list[i]->frame_seq!= current_frame_count)
			continue;
		draw_box_and_title(frame,face_win_list[i]->box,face_win_list[i]->title);
		result = result + "[box, x0: <" + std::to_string(face_win_list[i]->box.x0) + ">, y0: <" \
				+ std::to_string(face_win_list[i]->box.y0) + ">, x1: <" + std::to_string(face_win_list[i]->box.x1) \
				+ ">, y1: <" + std::to_string(face_win_list[i]->box.y1) + ">, title: <" \
				+ face_win_list[i]->title + ">] ";
		face_count++;
	}
	result = std::string("faces: <") + std::to_string(face_count) + ">; " + result;
	drop_aged_win(current_frame_count);
	return result;
}

int FaceDemo::Rename(int face_id, std::string name)
{
	if(face_id<0 || name=="")
	{
		//"bad arguments
		return -1;
	}

	/* check if face_id is a registered one */

	face_info * p_info=p_mem_store->find_record(face_id);

	if(p_info ==nullptr)
	{
		//"No such face id
		return -2;
	}

	if(p_info->name == name)
	{
		//"Nothing needs to do
		return 0;
	}

	p_info->name=name;

	/* update win */
	for(int i=0;i<face_win_list.size();i++)
	{
		face_window * p_win=face_win_list[i];

		if(p_win->face_id == face_id)
		{
			p_win->name=p_info->name;
		}
	}

	return 0;
}

int FaceDemo::Register(int face_id, std::string name)
{	
	if(face_id<0 || name=="")
	{
		//"bad arguments\n"
		return -1;
	}

	/* check if face_id is a registered one */

	face_info * p_info=p_mem_store->find_record(face_id);

	if(p_info && p_info->name != name)
	{
		//"do not support change name
		return -2;
	}

	unsigned int i;
	face_window * p_win;

	for(i=0;i<face_win_list.size();i++)
	{
		if(face_win_list[i]->face_id == face_id &&
				face_win_list[i]->frame_seq == current_frame_count)
			break;
	}

	if(i==face_win_list.size())
	{
		std::cout<<"cannot find face with id: "<<face_id<<std::endl;
		return -3;
	}

	p_win=face_win_list[i];

	/* extract feature first */

	face_info info;

	info.p_feature=(float *)malloc(256*sizeof(float));

	cv::Mat aligned;

	/* align face */
	get_aligned_face(p_cur_frame,(float *)&p_win->box.landmark,5,128,aligned);

	/* get feature */
	p_extractor->extract_feature(aligned,info.p_feature);

	if(face_id<UNKNOWN_FACE_ID_MAX)
		info.face_id=get_new_registry_id();
	else
		info.face_id=face_id;

	info.name=name;
	info.feature_len=256;

	/* insert feature into mem db */

	p_mem_store->insert_new_record(info);

	/* insert feature into verifier */

	p_verifier->insert_feature(info.p_feature,info.face_id);   
	
	free(info.p_feature);
	return 0;
}

int FaceDemo::Register(cv::Mat &frame, std::string name)
{	
	std::vector<face_box> face_boxes;
	std::string result = "";
	current_frame_count++;
	frame.copyTo(p_cur_frame);
	int face_count = 0;

	p_mtcnn->detect(frame,face_boxes);

	if(face_boxes.size() == 0)
		return -1; // no face 
	else if(face_boxes.size() != 1)
		return -2; // more than 1 face
	else
	{
		face_box& box=face_boxes[0];
		current_frame_count++;
		get_face_title(frame, box, current_frame_count);
		int face_id;
		for(unsigned int i=0;i<face_win_list.size();i++)
		{
			if(face_win_list[i]->frame_seq != current_frame_count)
				continue;
			std::vector<std::string> splited_words;
			std::string title = face_win_list[i]->title;
			Split(title, " ", splited_words);
			face_id = atoi(splited_words[0].c_str());
		}


		if(name=="")
		{
			//"bad arguments\n"
			return -3;
		}

		/* check if face_id is a registered one */

		face_info* p_info=p_mem_store->find_record(face_id);

		if(p_info && p_info->name != name)
		{
			//"do not support change name
			return -4;
		}

		unsigned int i;
		face_window * p_win;

		for(i=0;i<face_win_list.size();i++)
		{
			if(face_win_list[i]->face_id == face_id &&
					face_win_list[i]->frame_seq == current_frame_count)
				break;
		}

		if(i==face_win_list.size())
		{
			std::cout<<"cannot find face with id: "<<face_id<<std::endl;
			return -3;
		}

		p_win=face_win_list[i];

		/* extract feature first */

		face_info info;

		info.p_feature=(float *)malloc(256*sizeof(float));

		cv::Mat aligned;

		/* align face */
		get_aligned_face(p_cur_frame,(float *)&p_win->box.landmark,5,128,aligned);

		/* get feature */
		p_extractor->extract_feature(aligned,info.p_feature);

		if(face_id<UNKNOWN_FACE_ID_MAX)
			info.face_id=get_new_registry_id();
		else
			info.face_id=face_id;

		info.name=name;
		info.feature_len=256;

		/* insert feature into mem db */

		p_mem_store->insert_new_record(info);

		/* insert feature into verifier */

		p_verifier->insert_feature(info.p_feature,info.face_id);   

		//free(info.p_feature);
		return 0;
	}
}


std::string FaceDemo::ListStored()
{
	std::string result = "";
	std::vector<face_info *> list;
	int n=p_mem_store->get_all_records(list);

	for(int i=0;i<n;i++)
	{
		face_info * p_info=list[i];
		result = result + " [face_id: <" + std::to_string(p_info->face_id) + ">, face_name: <" + p_info->name + "> ] ";
	}
	result = std::string("total: <") + std::to_string(n) + ">, " + result;
	return result;
}

int FaceDemo::Delete(int face_id)
{
	if(face_id<0)
	{
		//"bad arguments
		return -1;
	}

	/* cv thread is parking now */

	std::vector<face_info *> list;

	face_info  * p0=p_mem_store->find_record(face_id);

	if(p0!=nullptr)
		list.push_back(p0);

	if(list.size()==0)
	{
		//No target face found
		return -2;
	}

	for(int i=0;i<list.size();i++)
	{
		face_info * p=list[i];
		int face_id=p->face_id;

		p_verifier->remove_feature(face_id);
		p_mem_store->remove_record(face_id);

		/* change the name in face_win_list to unknown */

		for(int l=0;l<face_win_list.size();l++)
		{
			face_window * p_win=face_win_list[l];

			if(p_win->face_id==face_id)
			{
				p_win->face_id=get_new_unknown_face_id();
				p_win->name="unknown";
			}
		}

	}
	return 0;

}

int FaceDemo::Delete(std::string name)
{	
	if(name=="")
	{
		//"bad arguments
		return -1;
	}

	/* cv thread is parking now */

	std::vector<face_info *> list;

	p_mem_store->find_record(name,list);

	if(list.size()==0)
	{
		//No target face found
		return -2;
	}

	for(int i=0;i<list.size();i++)
	{
		face_info * p=list[i];
		int face_id=p->face_id;

		p_verifier->remove_feature(face_id);
		p_mem_store->remove_record(face_id);

		/* change the name in face_win_list to unknown */

		for(int l=0;l<face_win_list.size();l++)
		{
			face_window * p_win=face_win_list[l];

			if(p_win->face_id==face_id)
			{
				p_win->face_id=get_new_unknown_face_id();
				p_win->name="unknown";
			}
		}
	}
	return 0;
}


int FaceDemo::LocalSave(std::string path)
{
	std::vector<face_info *> list;
	std::ofstream local_store(path + database_name,std::ios::trunc);
	if(!local_store.is_open())
		return -1;   //open failed
	int n_faces = p_mem_store->get_all_records(list);
	
	for(int i=0; i<n_faces; i++)
	{
		local_store << list[i]->name << std::endl;
		local_store << list[i]->face_id << std::endl;
		local_store << list[i]->feature_len << std::endl;
		for(int j=0; j<list[i]->feature_len; j++)
		{
			local_store << list[i]->p_feature[j] << " ";
		}
		local_store << std::endl;
	}
	return 0;
}

int FaceDemo::LocalLoad(std::string path)
{
	face_info * p_info =  new face_info();
	std::ifstream local_store(path + database_name);
	if(!local_store.is_open())
		return -1;   //open failed
	std::string db_name, db_id, db_len, db_feature;
	std::vector<std::string> features;
	while( std::getline(local_store,db_name) && std::getline(local_store,db_id) && std::getline(local_store,db_len) && std::getline(local_store,db_feature))
	{
		features.clear();
		p_info->name = db_name;
		p_info->face_id = atoi(db_id.c_str());
		p_info->feature_len = atoi(db_len.c_str());
		p_info->p_feature = (float *)malloc(sizeof(float)*(p_info->feature_len));
		Split(db_feature, " ", features);
		for(int i=0; i<p_info->feature_len; i++)
		{
			p_info->p_feature[i] = atof(features[i].c_str());
		}
		/* insert feature into mem db */

		p_mem_store->insert_new_record(*p_info);

		/* insert feature into verifier */

		p_verifier->insert_feature(p_info->p_feature,p_info->face_id); 

		free(p_info->p_feature);
	}
	return 0;	
}

void FaceDemo::Quit()
{
	delete p_mtcnn;
	delete p_extractor;
	delete p_mem_store;
	p_mtcnn = NULL;
	p_extractor = NULL;
	p_verifier = NULL;
	p_mem_store = NULL;
}

/*****************************************************************************/


int FaceDemo::get_new_unknown_face_id(void)
{
	static unsigned int current_id=0;

	return  (current_id++%UNKNOWN_FACE_ID_MAX);
}

unsigned int FaceDemo::get_new_registry_id(void)
{
	static unsigned int register_id=10000;

	register_id++;

	if(register_id==20000)
		register_id=10000;

	return register_id;
}



void FaceDemo::get_face_name_by_id(unsigned int face_id, std::string& name)
{
	face_info * p_info;

	p_info=p_mem_store->find_record(face_id);

	if(p_info==nullptr)
	{
		name="nullname";
	}
	else
	{
		name=p_info->name;
	}
}


void FaceDemo::drop_aged_win(unsigned int frame_count)
{
	std::vector<face_window *>::iterator it=face_win_list.begin();

	while(it!=face_win_list.end())
	{
		if((*it)->frame_seq+win_keep_limit<frame_count)
		{
			delete (*it);
			face_win_list.erase(it);
		}
		else
			it++;
	}
}

face_window * FaceDemo::get_face_id_name_by_position(face_box& box,unsigned int frame_seq)
{
	int found=0;
	float center_x=(box.x0+box.x1)/2;
	float center_y=(box.y0+box.y1)/2;
	face_window * p_win;

	std::vector<face_window *>::iterator it=face_win_list.begin();

	while (it!=face_win_list.end())
	{
		p_win=(*it);
		float offset_x=p_win->center_x-center_x;
		float offset_y=p_win->center_y-center_y;

		if((offset_x<trace_pixels) &&
				(offset_x>-trace_pixels) &&
				(offset_y<trace_pixels) &&
				(offset_y>-trace_pixels) &&
				(p_win->frame_seq+win_keep_limit)>=frame_seq)
		{
			found=1;
			break;
		}
		it++;
	}


	if(!found)
	{
		p_win=new face_window();
		p_win->name="unknown";
		p_win->face_id=get_new_unknown_face_id();
	}

	p_win->box=box;
	p_win->center_x=(box.x0+box.x1)/2;
	p_win->center_y=(box.y0+box.y1)/2;
	p_win->frame_seq=frame_seq;

	if(!found)
		face_win_list.push_back(p_win);

	return  p_win;

}

void FaceDemo::get_face_title(cv::Mat& frame,face_box& box,unsigned int frame_seq)
{
	float feature[256];
	int face_id;
	float score;
	face_window * p_win;

	p_win=get_face_id_name_by_position(box,frame_seq);

	cv::Mat aligned;

	/* align face */
	get_aligned_face(frame,(float *)&box.landmark,5,128,aligned);

	/* get feature */
	p_extractor->extract_feature(aligned,feature);

	/* search feature in db */

	int ret=p_verifier->search(feature,&face_id,&score);


	/* found in db*/
	if(ret==0 && score>0.8 && p_win->face_id != face_id)
	{
		p_win->face_id=face_id;
		get_face_name_by_id(face_id,p_win->name);
	}

	sprintf(p_win->title,"%d %s",p_win->face_id,p_win->name.c_str());
}

int Split(const std::string& src, const std::string& separator, std::vector<std::string>& dest)
{
    std::string str = src;
    std::string substring;
    std::string::size_type start = 0, index;
    start = str.find_first_not_of(separator,start);
    do
    {
        index = str.find_first_of(separator,start);
        if (index != std::string::npos)
        {    
            substring = str.substr(start,index-start);
            dest.push_back(substring);
            start = str.find_first_not_of(separator,index);
            if (start == std::string::npos) 
		return -1;
        }
    }while(index != std::string::npos);
    
    //the last token
    substring = str.substr(start);
    dest.push_back(substring);
}

void draw_box_and_title(cv::Mat& frame, face_box& box, char * title)
{

	float left,top;

	left=box.x0;
	top=box.y0-10;

	if(top<0)
	{
		top=box.y1+20;
	}

	cv::putText(frame,title,cv::Point(left,top),CV_FONT_HERSHEY_PLAIN, 1.5, cv::Scalar(0, 255, 0), 2);

	cv::rectangle(frame, cv::Point(box.x0, box.y0), cv::Point(box.x1, box.y1), cv::Scalar(0, 255, 0), 1);

	for(int l=0;l<5;l++)
	{
		cv::circle(frame,cv::Point(box.landmark.x[l],box.landmark.y[l]),1,cv::Scalar(0, 0, 255),2);
	}

}
