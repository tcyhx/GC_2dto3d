/* gc_2dto3d.cpp */

#include <stdio.h>
#include <iostream>
#include "graph.h"
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

using namespace cv;
using namespace std;

#ifdef _DEBUG
#pragma comment(lib, "opencv_video246d.lib")
#pragma comment(lib, "opencv_imgproc246d.lib")
#pragma comment(lib, "opencv_highgui246d.lib")
#pragma comment(lib, "opencv_core246d.lib")
#pragma comment(lib, "opencv_contrib246d.lib")
#pragma comment(lib, "opencv_calib3d246d.lib")
#else
#pragma comment(lib, "opencv_video246.lib")
#pragma comment(lib, "opencv_imgproc246.lib")
#pragma comment(lib, "opencv_highgui246.lib")
#pragma comment(lib, "opencv_core246.lib")
#pragma comment(lib, "opencv_contrib246.lib")
#pragma comment(lib, "opencv_calib3d246.lib")
#endif

double	FindB(Mat img, int y1, int x1, int y2, int x2);

int main(int argc, char* argv[])
{
	if (argc < 4)
	{
		printf("Not enough arguments.\n" );
		return -1;
	}

	char *imgFilename  = argv[1];
	char *maskFilename = argv[2];
	char *lblFilename  = argv[3];

	Mat orgImg = imread(imgFilename, CV_LOAD_IMAGE_COLOR);
	Mat maskImg = imread(maskFilename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat lblImg = imread(lblFilename, CV_LOAD_IMAGE_GRAYSCALE);
	Mat map = lblImg.clone();
	//Mat smImg;
	//GaussianBlur(orgImg,smImg,Size(3,3),1.5);
	//Mat labImg(lblImg.rows, lblImg.cols, CV_8UC3);
	//cvtColor(orgImg, labImg, CV_RGB2GRAY);

	if (orgImg.empty() || lblImg.empty()){
		cout<<"Could not open or find the image!"<<endl;
		return -1;
	}
	//init graph type
	typedef Graph<double,double,double> GraphType;
	//init graph nodes
	//Mat node(lblImg.rows, lblImg.cols,int);

	//get label numbers
	bool lblUsed[256];
	vector<uchar> lbls;  
	int lblNum = 0;
	memset(lblUsed, 0, sizeof(bool)*256);
	for( size_t nrow = 0; nrow < lblImg.rows; nrow++)  {  
       for(size_t ncol = 0; ncol < lblImg.cols; ncol++) {
		   uchar mask = maskImg.at<uchar>(nrow, ncol);
		   uchar val  = lblImg.at<uchar>(nrow, ncol);
		   if (mask==0)
			   continue;
		   else if (lblUsed[val])
			   continue;
		   else {
			   lbls.push_back(val);
			   lblUsed[val] = true;
			   lblNum++;
		   }
	   }
	}
	sort(lbls.begin(),lbls.end());

	int nodeNum = lblImg.rows*lblImg.cols;
	for(size_t lbl = 0; lbl < lbls.size(); lbl++ ) {
		GraphType::node_id **node;
		node = new GraphType::node_id*[lblImg.rows]; //apply for row
		for(size_t nrow=0;nrow<lblImg.rows;nrow++)
			node[nrow] = new GraphType::node_id[lblImg.cols]; //apply for col

		GraphType *g = new GraphType(nodeNum, nodeNum*4);
		printf("%d ", lbls[lbl]);
		for( size_t nrow = 0; nrow < lblImg.rows; nrow++)  {  
			for(size_t ncol = 0; ncol < lblImg.cols; ncol++) {
				node[nrow][ncol] = g->add_node();
			}
		}

		double	K = 0;
		for(int i = 0; i < lblImg.rows; i++) {
			for(int j = 0; j < lblImg.cols; j++) {
				for(int k = -1; k < 2; k++) {
					for(int l = -1; l < 2; l++) {
						if(i+k >= 0 && i+k < lblImg.rows && j+l >= 0 && j+l < lblImg.cols) {
							double B;
							if(k == 0 && l == 0) continue;
							B = FindB(orgImg,i,j,i+k,j+l);
							g->add_edge(node[i][j],node[i+k][j+l],B,B);
							if(1+B > K) K = 1+B;
						}
					}
				}
			}
		}

		for(int i = 0; i < lblImg.rows; i++) {
			for(int j = 0; j < lblImg.cols; j++) {
				if(map.at<uchar>(i,j) == lbls[lbl])
					g->add_tweights(node[i][j],0,K);
				else 
					if(maskImg.at<uchar>(i,j) != 0)
						g->add_tweights(node[i][j],K,0);
					else
						g->add_tweights(node[i][j],K,K);
			}
		}

		double flow = g->maxflow();

		for(int i = 0; i < lblImg.rows; i++) {
			for(int j = 0; j < lblImg.cols; j++) {
				if((g->what_segment(node[i][j]) == GraphType::SINK)&&(map.at<uchar>(i,j)==0))
					map.at<uchar>(i,j) = lbls[lbl];
			}
		}

		delete g;
		//memory clear
		for(size_t nrow=0;nrow<lblImg.rows;nrow++)
			delete[] node[nrow];
		delete[] node;
	}
	printf("\n");

	//create window
	namedWindow("depth", CV_WINDOW_AUTOSIZE);
	//display image
	imshow("depth", map);
	imwrite("depth_gc.png", map);
	//wait key to return
	waitKey(-1);

	return 0;
}

double	FindB(Mat img, int y1, int x1, int y2, int x2)
{
	double d;
	double B;
	double factor = 20;
	
	Vec3b Ip = img.at<Vec3b>(y1, x1);
	Vec3b Iq = img.at<Vec3b>(y2, x2);
	d = (Ip.val[0]-Iq.val[0])*(Ip.val[0]-Iq.val[0]) + (Ip.val[1]-Iq.val[1])*(Ip.val[1]-Iq.val[1]) + (Ip.val[2]-Iq.val[2])*(Ip.val[2]-Iq.val[2]);
	d = factor * sqrt(d);
	B = exp(-d);
	//B = 2.0/(exp(d) + 1);

	/*double B;
	double	factor = 90;
	Vec3b Ip = img.at<Vec3b>(y1, x1);
	Vec3b Iq = img.at<Vec3b>(y2, x2);
	
	double p = (11.0*Ip.val[0]+16.0*Ip.val[1]+5.0*Ip.val[2])/32.0;
	double q = (11.0*Iq.val[0]+16.0*Iq.val[1]+5.0*Iq.val[2])/32.0;
	double d = factor*(p-q)*(p-q);
	B = 2.0/(exp(d) + 1);*/
	
	return B;
}