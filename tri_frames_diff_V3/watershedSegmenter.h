#if !defined WATERSHS
#define WATERSHS

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class WatershedSegmenter {

private:
	//������ʾ��ǣ�ͼ��
	cv::Mat markers;

public:
	//���ñ��ͼ
	void setMarkers(const cv::Mat& markerImage) {

		//watershed()�������������Ϊһ��32λ�з��ŵı�ǣ�����Ҫ�Ƚ���ת�� 
		markerImage.convertTo(markers, CV_32S);
	}
	//ִ��watershed����
	cv::Mat process(const cv::Mat &image) {

		// Apply watershed
		cv::watershed(image, markers);

		return markers;
	}

	// ��ͼ����ʽ���ؽ��
	cv::Mat getSegmentation() {

		cv::Mat tmp;
		// ��32S��8U��0-255������б������㣬�������ظ���255��һ�ɸ���Ϊ255
		markers.convertTo(tmp, CV_8U);//

		return tmp;
	}

	// ��ͼ����ʽ���ط�ˮ�루�������Ƿָ��ߣ�
	cv::Mat getWatersheds() {

		cv::Mat tmp;
		//�����ñ��ͼ�񣬼�ִ��setMarkers�����󣬱�Ե�����ػᱻ��ֵΪ-1������������������ʾ
		//��������ת�������ñ�Ե���ر�Ϊ-1*255+255=0������ɫ��������������ֵΪ255������ɫ��
		markers.convertTo(tmp, CV_8U, 255, 255);
		return tmp;
	}
};


#endif