#ifndef __FACE_DEMO__HPP__
#define __FACE_DEMO__HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <unistd.h>
#include <signal.h>

#include "mtcnn.hpp"
#include "face_align.hpp"
#include "feature_extractor.hpp"
#include "face_verify.hpp"
#include "face_mem_store.hpp"
#include "face_demo.hpp"

#include "utils.hpp"


struct face_window
{
	face_box box;
	unsigned int face_id;
	unsigned int frame_seq;
	float center_x;
	float center_y;
	std::string name;
	char title[128];
};


class FaceDemo {
	public:
		int Init(double threshold_p, double threshold_r, double threshold_o, double factor, int mim_size);
		std::string Recognize(cv::Mat &frame);
		int Rename(int face_id, std::string name);
		std::string ListStored();
		//Register a face in last frame by face_id
		int Register(int face_id, std::string name);
		//Register a face by a frame which must contain only 1 face
		int Register(cv::Mat &frame, std::string name);
		int Delete(int face_id);
		int Delete(std::string name);
		int LocalSave(std::string path);
		int LocalLoad(std::string path);
		void Quit();
	protected:
		int get_new_unknown_face_id(void);
		unsigned int get_new_registry_id(void);
		void get_face_name_by_id(unsigned int face_id, std::string& name);
		void drop_aged_win(unsigned int frame_count);
		face_window * get_face_id_name_by_position(face_box& box,unsigned int frame_seq);
		void get_face_title(cv::Mat& frame,face_box& box,unsigned int frame_seq);

		std::vector<face_window*> face_win_list;
		mtcnn * p_mtcnn = NULL;
		feature_extractor * p_extractor = NULL;
		face_verifier   * p_verifier = NULL;
		face_mem_store * p_mem_store = NULL;
		cv::Mat p_cur_frame;
		int current_frame_count=0;
		int win_keep_limit=10;
		int trace_pixels=100;
		std::string database_name = "face_demo.dat";
};



#endif
