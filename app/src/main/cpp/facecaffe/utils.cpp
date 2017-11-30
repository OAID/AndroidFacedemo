#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>

#include <opencv2/opencv.hpp>

#include <sys/time.h>


unsigned long get_cur_time(void)
{
	struct timeval tv;
	unsigned long ts;

	gettimeofday(&tv,NULL);

	ts=tv.tv_sec*1000000+tv.tv_usec;

	return ts;
}

void save_float(const char * name, const float * data, int size)
{
	char fname[128];

	sprintf(fname,"%s",name);

	std::cout<<"save data to "<<fname<<"   size " <<size<<std::endl;
	std::ofstream of;
	of.open(fname);

	for(int i=0;i<size;i++)
        {
		of<<std::setprecision(6)<<data[i]<<","<<std::endl;
        }

	of.close();
}


void save_img(const char * name, void * p_img)
{
	const cv::Mat& img= *(cv::Mat *)p_img;
	int row=img.rows;
	int col=img.cols;
	int chan=img.channels();

	int sz=row*col*chan;
	char fname[128];

	int data;

	sprintf(fname,"%s",name);

	std::cout<<"save data to "<<fname<<"   size " <<sz<<std::endl;
	std::ofstream of;
	of.open(fname);


	col=col*chan;

	if(img.isContinuous())
	{
		col=col*row;
		row=1;
	}

	for(int i=0;i<row;i++)
	{
		const unsigned char  * p=img.ptr<unsigned char >(i);

		for(int j=0;j<col;j++)
		{
			data=p[j];

			of<<data<<","<<std::endl;
		}
	}

	of.close();
}



