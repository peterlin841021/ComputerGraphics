#include "Application.h"
#include "qt_opengl_framework.h"
#include <vector>
#include <cmath>
using namespace std;
Application::Application()
{

}
Application::~Application()
{

}
//****************************************************************************
//
// * ��l�e���A�����Ntust.png����
// 
//============================================================================
void Application::createScene( void )
{
	
	ui_instance = Qt_Opengl_Framework::getInstance();
	
}

//****************************************************************************
//
// * ���}���w����
// 
//============================================================================
void Application::openImage( QString filePath )
{
	mImageSrc.load(filePath);
	mImageDst.load(filePath);

	renew();

	img_data = mImageSrc.bits();
	img_width = mImageSrc.width();
	img_height = mImageSrc.height();

	ui_instance->ui.label->setFixedHeight(img_height);
	ui_instance->ui.label->setFixedWidth(img_width);
}
//****************************************************************************
//
// * ��s�e��
// 
//============================================================================
void Application::renew()
{
	ui_instance = Qt_Opengl_Framework::getInstance();

	ui_instance->ui.label->clear();
	ui_instance->ui.label->setPixmap(QPixmap::fromImage(mImageDst));

	//std::cout << "Renew" << std::endl;
}

//****************************************************************************
//
// * �e����l��
// 
//============================================================================
void Application::reload()
{
	ui_instance = Qt_Opengl_Framework::getInstance();

	ui_instance->ui.label->clear();
	ui_instance->ui.label->setPixmap(QPixmap::fromImage(mImageSrc));
}

//****************************************************************************
//
// * �x�s����
// 
//============================================================================
void Application::saveImage(QString filePath )
{
	mImageDst.save(filePath);
}

//****************************************************************************
//
// * �N���ɸ���ഫ��RGB��m���
// 
//============================================================================
unsigned char* Application::To_RGB( void )
{
	unsigned char *rgb = new unsigned char[img_width * img_height * 3];
	int i, j;

	if (! img_data )
		return NULL;

	// Divide out the alpha
	for (i = 0; i < img_height; i++)
	{
		int in_offset = i * img_width * 4;
		int out_offset = i * img_width * 3;

		for (j = 0 ; j < img_width ; j++)
		{
			RGBA_To_RGB(img_data + (in_offset + j*4), rgb + (out_offset + j*3));
		}
	}

	return rgb;
}

void Application::RGBA_To_RGB( unsigned char *rgba, unsigned char *rgb )
{
	const unsigned char	BACKGROUND[3] = { 0, 0, 0 };

	unsigned char  alpha = rgba[3];

	if (alpha == 0)
	{
		rgb[0] = BACKGROUND[0];
		rgb[1] = BACKGROUND[1];
		rgb[2] = BACKGROUND[2];
	}
	else
	{
		float	alpha_scale = (float)255 / (float)alpha;
		int	val;
		int	i;

		for (i = 0 ; i < 3 ; i++)
		{
			val = (int)floor(rgba[i] * alpha_scale);
			if (val < 0)
				rgb[i] = 0;
			else if (val > 255)
				rgb[i] = 255;
			else
				rgb[i] = val;
		}
	}
}
//------------------------Color------------------------
///////////////////////////////////////////////////////////////////////////////
//
//  Convert image to grayscale.  Red, green, and blue channels should all 
//  contain grayscale value.  Alpha channel shoould be left unchanged.  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Gray()
{
	unsigned char *rgb = To_RGB();

	for (int i=0; i<img_height; i++)
	{
		for (int j=0; j<img_width; j++)
		{
			int offset_rgb = i*img_width*3+j*3;
			int offset_rgba = i*img_width*4+j*4;
			unsigned char gray = 0.3 * rgb[offset_rgb + rr] + 0.59 * rgb[offset_rgb + gg] + 0.11 * rgb[offset_rgb + bb];

			for (int k=0; k<3; k++)
				img_data[offset_rgba+k] = gray;
			img_data[offset_rgba + aa] = WHITE;
		}
	}
	
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using uniform quantization.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
int graphic_round(const float &g) {
	return int(g>0 ?g+0.5f:g-0.5f);
}
void Application::Quant_Uniform()
{
	unsigned char *rgb = this->To_RGB();
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			img_data[offset_rgba + 0] = (rgb[offset_rgb+0] / 32)*32 ;   // 256/32=8
			img_data[offset_rgba + 1] = (rgb[offset_rgb+1] / 32)*32 ;   // 256/32=8
			img_data[offset_rgba + 2] = (rgb[offset_rgb+2] / 64)*64 ;   // 256/64=4			
			img_data[offset_rgba + aa] = WHITE;
		}
	}
	
	
	
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using populosity quantization.  
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
class colorTuple {
public:
	unsigned char red, green, blue;
	int count;
	colorTuple() {

	}
	colorTuple(unsigned char r, unsigned char g, unsigned char b,int c) {
		red = r;
		green = g;
		blue = b;
		count = c;
	}
	
};
bool cmp(colorTuple c1, colorTuple c2) {
	return c1.count > c2.count;
}
void Application::Quant_Populosity()
{
	unsigned char *rgb = this->To_RGB();
	vector<colorTuple> colors;	
	//colors.reserve(256);
	int shades = 8,count=0;
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			img_data[offset_rgba + 0] = rgb[offset_rgb + 0] / shades*shades;
			img_data[offset_rgba + 1] = rgb[offset_rgb + 1] / shades*shades;
			img_data[offset_rgba + 2] = rgb[offset_rgb + 2] / shades*shades;
			if (count == 0) {
				colorTuple color = colorTuple(img_data[offset_rgba + 0], img_data[offset_rgba + 1], img_data[offset_rgba + 2],1);
				colors.push_back(color);
				count++;
			}
			else {
				bool duplicate = false;
				//find color
				for (int k = 0; k < colors.size(); k++) {
					if (img_data[offset_rgba + 0] == colors[k].red
						&& img_data[offset_rgba + 1] == colors[k].green
						&& img_data[offset_rgba + 2] == colors[k].blue){
						duplicate = true;
						colors[k].count += 1;
						break;
					}
				}
				//new color
				if (!duplicate) {
					colorTuple color = colorTuple(img_data[offset_rgba + 0], img_data[offset_rgba + 1], img_data[offset_rgba + 2], 1);
					colors.push_back(color);
					count++;
				}
			}
		}
	}		
		
	sort(colors.begin(),colors.end(),cmp);
	if (count > 256) {
		for (int i = 0; i<img_height; i++)
		{
			for (int j = 0; j<img_width; j++)
			{
				int offset_rgb = i*img_width * 3 + j * 3;
				int offset_rgba = i*img_width * 4 + j * 4;
				bool find = false;
				for (int k = 0; k < 256; k++) {
					if (img_data[offset_rgba + 0] == colors.at(k).red &&
						img_data[offset_rgba + 1] == colors.at(k).green &&
						img_data[offset_rgba + 2] == colors.at(k).blue) {
						find = true;
						break;
					}
				}

				if (!find) {
					float dis = sqrt(
						pow(img_data[offset_rgba + 0] - colors.at(0).red, 2) +
						pow(img_data[offset_rgba + 1] - colors.at(0).green, 2) +
						pow(img_data[offset_rgba + 2] - colors.at(0).blue, 2));
					float min = dis;
					int closet = 0;
					for (int k = 1; k < 256; k++) {
						dis = sqrt(
							pow(img_data[offset_rgba + 0] - colors.at(k).red, 2) +
							pow(img_data[offset_rgba + 1] - colors.at(k).green, 2) +
							pow(img_data[offset_rgba + 2] - colors.at(k).blue, 2));
						if (min > dis) {
							min = dis;
							closet = k;
						}
					}
					img_data[offset_rgba + 0] = colors.at(closet).red;
					img_data[offset_rgba + 1] = colors.at(closet).green;
					img_data[offset_rgba + 2] = colors.at(closet).blue;
					img_data[offset_rgba + aa] = 255;
				}	

			}
		}
	}	
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Dithering------------------------

///////////////////////////////////////////////////////////////////////////////
//
//  Dither the image using a threshold of 1/2.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Threshold()
{
	unsigned char *rgb = this->To_RGB();	
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			unsigned char gray = 0.3 * rgb[offset_rgb + rr] + 0.59 * rgb[offset_rgb + gg] + 0.11 * rgb[offset_rgb + bb];
			for (int k = 0; k < 3; k++) {
				img_data[offset_rgba + k] = gray;
				if (img_data[offset_rgba + k] < 128) {
					img_data[offset_rgba + k] = 0;
				}
				else {
					img_data[offset_rgba + k] = 255;
				}
			}										
			img_data[offset_rgba + aa] = WHITE;
		}
	}


	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Dither image using random dithering.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Random()
{
	unsigned char *rgb = this->To_RGB();
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			unsigned char gray = 0.3 * rgb[offset_rgb + rr] + 0.59 * rgb[offset_rgb + gg] + 0.11 * rgb[offset_rgb + bb];
			for (int k = 0; k < 3; k++) {
				img_data[offset_rgba + k] = gray;								
			}
			float ran = ((rand() % 51) + 1);//1~51
			int op = ((rand() % 3) + 1);//1~2
			if (op == 1)
				ran = -ran;
			for (int k = 0; k < 3; k++) {				
				if (img_data[offset_rgba + k] < 128 + ran) {
					img_data[offset_rgba + k] = 0;
				}
				else {
					img_data[offset_rgba + k] = 255;
				}
			}
			img_data[offset_rgba + aa] = WHITE;
		}
	}


	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform Floyd-Steinberg dithering on the image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
bool DitherFS_BlackWhite(int x, int y, unsigned char *img_data, int img_width, int img_height, double amount) {
	if (x < 0 || x >= img_width )
		return false;
	if(y < 0 || y >= img_height)
		return false;
	int offset = x * 4 + y * img_width * 4;	
	double value = img_data[offset] + amount;
	if (value > 255.0)
		value = 255.0;	
	if (value < 0)
		value = 0;
	img_data[offset + 0] = value;
	img_data[offset + 1] = value;
	img_data[offset + 2] = value;
	img_data[offset + 3] = 255;
	return true;
}

void Application::Dither_FS()
{
	unsigned char *rgb = this->To_RGB();
	Gray();
	//ofstream log;
	//log.open("D:\\log.txt");
	int direction = 1;//zig-zag scan 1:r -1:L
	for (int i = 0; i < img_height; i++)
	{
		for (int j = (direction == 1) ? 0 : img_width-1; (direction == 1) ? j < img_width : j >= 0; j += direction)
		{
			int offset_rgba = i*img_width * 4 + j * 4;			
			unsigned char v = img_data[offset_rgba];
			unsigned char whiteBlack = 0;
			if (v / 255. > 0.5)
				whiteBlack = 255;
			float error = v - whiteBlack;
			for (int k = 0; k < 3; k++) {
				img_data[offset_rgba+k] = whiteBlack;
			}			
			img_data[offset_rgba + aa] = WHITE;
			double errorDiffusion = error * 7. / 16.;
			DitherFS_BlackWhite(j + direction, i, img_data, img_width, img_height, errorDiffusion);				
			errorDiffusion = error * 3. / 16.;
			DitherFS_BlackWhite(j - direction, i + 1, img_data, img_width, img_height, errorDiffusion);
			errorDiffusion = error * 5. / 16.;
			DitherFS_BlackWhite(j, i + 1, img_data, img_width, img_height, errorDiffusion);
			errorDiffusion = error * 1. / 16.;
			DitherFS_BlackWhite(j + direction, i + 1, img_data, img_width, img_height, errorDiffusion);
		}
		if (direction == 1)
			direction = -1;
		else
			direction = 1;
	}
	//log.close();
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Dither the image while conserving the average brightness.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Bright()
{
	unsigned char *rgb = this->To_RGB();
	Gray();
	long bSum = 0;
	int threshold = 0;
	vector<unsigned char> v;

	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{			
			int offset_rgba = i*img_width * 4 + j * 4;						
			bSum += img_data[offset_rgba];					
			v.push_back(img_data[offset_rgba]);
		}
	}
	sort(v.begin(),v.end());
	threshold = v.at((1 - (bSum / (img_height*img_width)) / 255.f)*(v.end() - v.begin()));
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			
			for (int k = 0; k < 3; k++) {
				img_data[offset_rgba + k] = img_data[offset_rgba + k] > threshold ? 255 : 0;
			}
		}
	}

	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform clustered differing of the image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Dither_Cluster()
{
	unsigned char *rgb = this->To_RGB();
	float matrix[4][4] = {0.7059f,0.3529f,0.5882f,0.2353f,0.0588f,0.9412f,0.8235f,0.4118f,0.4706f,0.7647f,0.8824f,0.1176f,0.1765f,0.5294f,0.2941f,0.6471f};
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			unsigned char gray = 0.3 * rgb[offset_rgb + rr] + 0.59 * rgb[offset_rgb + gg] + 0.11 * rgb[offset_rgb + bb];
			for (int k = 0; k < 3; k++) {
				img_data[offset_rgba + k] = gray;
			}
			
			for (int k = 0; k < 3; k++) {
				if (img_data[offset_rgba + k]/255.f < matrix[i%4][j%4]) {
					img_data[offset_rgba + k] = 0;
				}
				else {
					img_data[offset_rgba + k] = 255;
				}
			}
			img_data[offset_rgba + aa] = WHITE;
		}
	}


	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Convert the image to an 8 bit image using Floyd-Steinberg dithering over
//  a uniform quantization - the same quantization as in Quant_Uniform.
//  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
bool DitherFS_Color(int x, int y, unsigned char *img_data, int img_width, int img_height, float re,float ge,float be) {
	if (x < 0 || x >= img_width || y < 0 || y >= img_height)
		return false;
	int offset = x * 4 + y * img_width * 4;	
	img_data[offset + 0] += re;
	img_data[offset + 1] += ge;
	img_data[offset + 2] += be;
	img_data[offset + 3] = 255;
	return true;
}
void Application::Dither_Color()
{
	unsigned char *rgb = this->To_RGB();
	unsigned char r8[] = { 0,36,73,109,146,182,219,255};
	unsigned char g8[] = { 0,36,73,109,146,182,219,255 };
	unsigned char b4[] = { 0,85,170,255 };
	int direction = 1;//zig-zag scan 1:r -1:L
	for (int i = 0; i < img_height; i++)
	{
		for (int j = (direction == 1)?0:img_width-1; (direction == 1) ?j <= img_width-1 : j >= 0; j+=direction)
		{			
			int offset_rgba = i*img_width * 4 + j * 4;
			int re = img_data[offset_rgba + 0] - r8[(int)img_data[offset_rgba + 0] / 32],
				ge = img_data[offset_rgba + 1] - g8[(int)img_data[offset_rgba + 1] / 32],
				be = img_data[offset_rgba + 2] - b4[(int)img_data[offset_rgba + 2] / 64];
			img_data[offset_rgba + 0] = r8[(int)img_data[offset_rgba + 0] / 32];
			img_data[offset_rgba + 1] = g8[(int)img_data[offset_rgba + 1] / 32];
			img_data[offset_rgba + 2] = b4[(int)img_data[offset_rgba + 2] / 64];
			img_data[offset_rgba + aa] = WHITE;
			//Floyd-Steinberg
			float div = 7 / 16.;
			DitherFS_Color(j+ direction,i,img_data,img_width,img_height,re*div,ge*div,be*div);
			div = 3 / 16.;
			DitherFS_Color(j - direction, i+1, img_data, img_width, img_height, re*div, ge*div, be*div);
			div = 5 / 16.;
			DitherFS_Color(j , i+1, img_data, img_width, img_height, re*div, ge*div, be*div);
			div = 1 / 16.;
			DitherFS_Color(j + direction, i+1, img_data, img_width, img_height, re*div, ge*div, be*div);
		}
		direction = -direction;		
	}
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

//------------------------Filter------------------------

///////////////////////////////////////////////////////////////////////////////
//
//     Filtering the img_data array by the filter from the parameters
//
///////////////////////////////////////////////////////////////////////////////
void doFileter(unsigned char *img_data,int img_height,int img_width,double **filter) {	
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i*img_width * 3 + j * 3;
			int offset_rgba = i*img_width * 4 + j * 4;
			//matrix
			
			for (int m = 0; m < 3; m++) {//RBG
				double sum = 0;
				for (int k = -2; k <= 2; k++) {//height
					for (int L = -2; L <= 2; L++) {//width
						if (k + i < 0 || k + i >= img_height) {//=img_height.....
							//filter[k + 2][L + 2] = 0;
							continue;
						}							
						if (L + j < 0 || L + j >= img_width) {//=img_width.....
							//filter[k + 2][L + 2] = 0;
							continue;
						}
							
						int offset = (i+k)*img_width * 4 + (j+L) * 4;
						unsigned char c = img_data[offset + m];
						sum += c* filter[k + 2][L + 2];
						img_data[offset + 3] = 255;
					}
				}
				//				
				if (sum > 255)
					sum = 255;
				if (sum < 0)
					sum = 0;
				img_data[offset_rgba + m] = sum;				
			}					
			//
		}
	}
}
void Application::filtering(double **filter)
{
	unsigned char *rgb = this->To_RGB();		
	doFileter(img_data,img_height,img_width,filter);
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
bool pixelcomparator(unsigned char c1, unsigned char c2) {
	return (c1 > c2);
}
void Application::filter(int n,bool centerweighted) {
	int step = n / 2;
	/*vector<unsigned char> v;
	v.reserve(n*n+n);*/
	int w = n * n - 2 * n + 1;
	unsigned char *mask = new unsigned char[n*n];
	unsigned char *list = new unsigned char[n*n + w]{0};
	int start = clock();
	//printf("Time:%d mask size:%dx%d centerweighted.\n", clock() - start, n, n);
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int offset_rgba = i * img_width * 4 + j * 4;
			for (int m = 0; m < 3; m++) 
			{//RGB
				for (int k = -step; k <= step; k++)
				{//height
					for (int L = -step; L <= step; L++)
					{//width
						if (k + i < 0 || k + i >= img_height)
						{
							if (k + i < 0){
								//v.push_back(img_data[0 * img_width * 4 + j * 4 + m]);
								mask[(k + step)*n+L + step] = img_data[0 * img_width * 4 + j * 4 + m];
							}
							else{
								//v.push_back(img_data[(img_height-1) * img_width * 4 + j * 4 + m]);
								mask[(k + step)*n + L + step] = img_data[(img_height - 1) * img_width * 4 + j * 4 + m];
							}
						}						
						else if (L + j < 0 || L + j >= img_width)
						{
							if (L + j < 0){
								//v.push_back(img_data[i* img_width * 4 + m]);
								mask[(k + step)*n + L + step] = img_data[i* img_width * 4 + m];
							}
							else{
								//v.push_back(img_data[i * img_width * 4 + (img_width - 1) * 4 + m]);
								mask[(k + step)*n + L + step] = img_data[i * img_width * 4 + (img_width - 1) * 4 + m];
							}
						}
						else {
							int offset = (i + k)*img_width * 4 + (j + L) * 4;
							//v.push_back(img_data[offset]);
							mask[(k + step)*n + L + step] = img_data[offset];
						}						
					}
				}				
				if (centerweighted) {
					unsigned char center = mask[n*n / 2];					
										
					int length = n*n;
					for (size_t k = 0; k < length; k++)
					{
						if (k < length / 2) {
							list[k] = mask[k];
						}
						else {
							list[k+w] = mask[k];
						}
					}
					for (size_t k = 0; k < w; k++) {
						list[(length / 2)+k] = center;
					}
					//sorting
					length = n*n + w;
					/*for (size_t k = 0; k < length; k++)
					{
						printf("%d ",list[k]);
					}
					printf("\n");*/
					for (size_t k = 0; k < length ; k++)
					{
						for (size_t L = 0; L < length-1; L++) {
							if (list[L] > list[L + 1]) {
								unsigned char temp = list[L];
								list[L] = list[L + 1];
								list[L + 1] = temp;
							}
						}						
					}								
					img_data[offset_rgba + m] = list[length/2];
					img_data[offset_rgba + 3] = 255;
				}
				else {
					int mean = 0;
					for (size_t k = 0; k < n*n; k++) {
						mean +=(mask[k]);
					}					
					img_data[offset_rgba + m] = (mean)/(n*n);
					img_data[offset_rgba + 3] = 255;
				}								
			}			
		}
	}
	if (centerweighted) 
		printf("Time:%d mask size:%dx%d centerweighted.\n",clock() - start,n,n);
	else
		printf("Time:%d mask size:%dx%d mean.\n", clock() - start, n, n);
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
void Application::filtering_N( double **filter, int n )
{	
	//unsigned char *rgb = this->To_RGB();
	int step = n / 2;
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			int offset_rgb = i * img_width * 3 + j * 3;			
			int offset_rgba = i * img_width * 4 + j * 4;
			//matrix
			for (int m = 0; m < 3; m++) {//RGB
				double sum = 0;
				for (int k = -step; k <= step; k++) {//height
					for (int L = -step; L <= step; L++) {//width
						if (k + i < 0 || k + i >= img_height) {															   
							continue;
						}
						if (L + j < 0 || L + j >= img_width) {															  
							continue;
						}
						int offset = (i + k)*img_width * 4 + (j + L) * 4;
						
						unsigned char c = img_data[offset + m];
						sum += c * filter[k + step][L + step];						
						img_data[offset + 3] = 255;
					}
				}
				//				
				if (sum > 255)
					sum = 255;
				if (sum < 0)
					sum = 0;
				img_data[offset_rgba + m] = sum;
			}			
			//
		}
	}
	//delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 box filter on this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Box()
{
	double **filter = new double*[5];
	for (int i = 0; i < 5; i++) {
		filter[i] = new double[5];
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			filter[i][j] = 1 / 25.0;
		}
	}
	filtering(filter);
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 Bartlett filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Bartlett()
{
	double **bartlettFilter = new double*[5];
	bartlettFilter[0] = new double[5]{ 1,2,3,2,1 };
	bartlettFilter[1] = new double[5]{ 2,4,6,4,2 };
	bartlettFilter[2] = new double[5]{ 3,6,9,6,3 };
	bartlettFilter[3] = new double[5]{ 2,4,6,4,2 };
	bartlettFilter[4] = new double[5]{ 1,2,3,2,1 };
	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			bartlettFilter[i][j] = bartlettFilter[i][j] / 81.0;
		}
	}
	filtering(bartlettFilter);
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Gaussian()
{

	double **gaussianFilter = new double*[5];
	gaussianFilter[0] = new double[5]{ 1,4,6,4,1 };
	gaussianFilter[1] = new double[5]{ 4,16,24,16,4 };
	gaussianFilter[2] = new double[5]{ 6,24,36,24,6 };
	gaussianFilter[3] = new double[5]{ 4,16,24,16,4 };
	gaussianFilter[4] = new double[5]{ 1,4,6,4,1 };	
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			gaussianFilter[i][j] = gaussianFilter[i][j] / 256.0;
			//printf("%f ", gaussianFilter[i][j]);
		}
		//printf("\n");
	}
	filtering(gaussianFilter);
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform NxN Gaussian filter on this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
double Binomial(int n, int s)// cal Binomial coefficient
{
	double res =0.0;

	res = 1;
	for (int i = 1; i <= s; i++)
		res = (n - i + 1) * res / i;

	return res;
}
int binomialCoeff(int n, int k)
{
	int *C =new int[k+1];
	memset(C, 0, sizeof(C));

	C[0] = 1;  // nC0 is 1 

	for (int i = 1; i <= n; i++)
	{
		for (int j = min(i, k); j > 0; j--)
			C[j] = C[j] + C[j - 1];
	}
	return C[k];
}
void Application::Filter_Gaussian_N( unsigned int N =0)
{		
	if (N % 2 != 0) {
		double *gaussian1d = new double[N];
		double **gaussian2d = new double*[N];
		for (int i = 0; i < N; i++) {
			gaussian2d[i] = new double[N];
		}

		int sum = 0;
		for (int i = 0; i < N; i++) {
			gaussian1d[i] = Binomial(N-1, i);		
			//gaussian1d[i] = binomialCoeff(i,);
		}
	
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				gaussian2d[i][j] = gaussian1d[i] * gaussian1d[j];
				sum += gaussian2d[i][j];				
			}			
		}
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				//printf("%f ", gaussian2d[i][j]);
				gaussian2d[i][j] /= sum;				
				
			}
			//printf("\n");
		}

		unsigned char *rgb = this->To_RGB();
		//doFileter(img_data, img_height, img_width, gaussian2d,N);
		filtering_N(gaussian2d, N);
		delete[] rgb;
		mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
		renew();
	}	
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform 5x5 edge detect (high pass) filter on this image.  Return 
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Edge()
{	
	int N = 5;
	double *gaussian1d = new double[N];
	double **gaussian2d = new double*[N];
	for (int i = 0; i < N; i++) {
		gaussian2d[i] = new double[N];
	}
	
	float sum = 0;
	for (int i = 0; i < N; i++) {
		gaussian1d[i] = Binomial(N - 1, i);
	}
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			gaussian2d[i][j] = gaussian1d[i] * gaussian1d[j];
			sum += gaussian2d[i][j];			
		}		
	}
		
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			gaussian2d[i][j] /= sum;
		}		
	}	

	//copy img
	unsigned char *source = new unsigned char[img_width*img_height * 4];//4:RGBA
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int pixel = (i * (img_width * 4)) + (j * 4);
			try {
				for (int k = 0; k < 4; k++) {
					source[pixel + k] = img_data[pixel + k];
				}
			}
			catch (int e) {

			}
		}
	}
	//edge
	filtering_N(gaussian2d, N);	
		
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int pixel = (i * (img_width * 4)) + (j * 4);
			try {
				for (int k = 0; k < 3; k++) {
					int temp = source[pixel + k] - img_data[pixel + k];
					if (temp < 0) {
						source[pixel + k] = 0;
					}
					else if(temp > 255){
						source[pixel + k] = 255;
					}
					else {
						source[pixel + k] = temp;
					}					
				}
			}
			catch (int e) {

			}
		}
	}
	img_data = source;
	//delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Perform a 5x5 enhancement filter to this image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Filter_Enhance()
{
	//copy img
	unsigned char *source = new unsigned char[img_width*img_height * 4];//4:RGBA
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int pixel = (i * (img_width * 4)) + (j * 4);
			try {
				for (int k = 0; k < 4; k++) {
					source[pixel + k] = img_data[pixel + k];
				}
			}
			catch (int e) {

			}
		}
	}
	Filter_Edge();
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int pixel = (i * (img_width * 4)) + (j * 4);
			try {
				for (int k = 0; k < 4; k++) {
					int temp = source[pixel + k] + img_data[pixel + k];
					if (temp > 255)
						source[pixel + k] = 255;
					else
						source[pixel + k] = temp;					
				}
			}
			catch (int e) {

			}
		}
	}
	img_data = source;	
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
//------------------------Size------------------------

///////////////////////////////////////////////////////////////////////////////
//
//  Halve the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Half_Size()
{
	double factor = 0;
	unsigned char *rgb = this->To_RGB();

	factor = 0.5;
	int dest_w = factor * img_width, dest_h = factor * img_height;
	unsigned char *dest_img = new unsigned char[dest_w*dest_h * 4];
	for (int i = 0; i<dest_h; i++)
	{
		for (int j = 0; j<dest_w; j++)
		{
			int pixel = (i * (dest_w * 4)) + (j * 4);
			int nearestMatch = (((int)(i / factor) * (img_width * 4)) + ((int)(j / factor) * 4));
			try {
				for (int k = 0; k<4; k++)
					dest_img[pixel + k] = img_data[nearestMatch + k];
			}
			catch (int e) {

			}
		}
	}

	delete[] rgb;
	ui_instance->ui.label->setFixedHeight(dest_h);
	ui_instance->ui.label->setFixedWidth(dest_w);
	img_data = dest_img;
	img_height = dest_h;
	img_width = dest_w;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  Double the dimensions of this image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void matrixCal(unsigned char *img_data,int px,int py,int img_w,int img_h,double **matrix,int n) {		
	int sIn = -(n / 2),eIn = n/2;	
	int offset_rgba = px * 4 + py*img_w * 4;
	for (int m = 0; m < 4; m++) {
		double sum = 0;
		for (int k = sIn; k <= eIn; k++) {//height
			for (int L = sIn; L <= eIn; L++) {//width

				if (k + py< 0 || k + py >= img_h) {//=img_height.....												  
					continue;
				}
				if (L + px< 0 || L + px >= img_w) {//=img_width.....												  
					continue;
				}

				int offset = (py + k)*img_w * 4 + (px + L) * 4;
				sum += img_data[offset + m] * matrix[k + eIn][L + eIn];
			}
		}
		img_data[offset_rgba + m] = sum;
	}	
}
void Application::Double_Size()
{
	double factor = 0;
	unsigned char *rgb = this->To_RGB();
	
	factor = 2;
	int dest_w = factor * img_width,dest_h = factor * img_height;
	unsigned char *dest_img = new unsigned char[dest_w*dest_h *4];
	for (int i = 0; i<dest_h; i++)
	{
		for (int j = 0; j<dest_w; j++)
		{			
			int pixel = (i * (dest_w * 4)) + (j * 4);
			int nearestMatch = (((int)(i / factor) * (img_width * 4)) + ((int)(j / factor) * 4));
			try{
			for(int k=0;k<4;k++)
				dest_img[pixel+k] = img_data[nearestMatch+k];						
			}
			catch (int e) {

			}
		}
	}
	
	delete[] rgb;
	ui_instance->ui.label->setFixedHeight(dest_h);
	ui_instance->ui.label->setFixedWidth(dest_w);
	img_data = dest_img;
	img_height = dest_h;
	img_width = dest_w;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
///////////////////////////////////////////////////////////////////////////////
//
//  resample_src for resize and rotate
//
///////////////////////////////////////////////////////////////////////////////
void Application::resample_src(int u, int v, float ww, unsigned char* rgba)
{

}

///////////////////////////////////////////////////////////////////////////////
//
//  Scale the image dimensions by the given factor.  The given factor is 
//	assumed to be greater than one.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Resize( float scale )
{
	float factor = scale;
	int dest_w = factor * img_width, dest_h = factor * img_height;
	unsigned char *dest_img = new unsigned char[dest_w*dest_h * 4];//4:RGBA
	for (int i = 0; i<dest_h; i++)
	{
		for (int j = 0; j<dest_w; j++)
		{
			int pixel = (i * (dest_w * 4)) + (j * 4);
			int nearestMatch = (((int)(i / factor) * (img_width * 4)) + ((int)(j / factor) * 4));
			try {
				for (int k = 0; k<4; k++)
					dest_img[pixel + k] = img_data[nearestMatch + k];
			}
			catch (int e) {

			}
		}
	}	
	
	ui_instance->ui.label->setFixedHeight(dest_h);
	ui_instance->ui.label->setFixedWidth(dest_w);
	img_data = dest_img;
	img_height = dest_h;
	img_width = dest_w;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();	
}

//////////////////////////////////////////////////////////////////////////////
//
//  Rotate the image clockwise by the given angle.  Do not resize the 
//  image.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Rotate( float angleDegrees )
{
	float rad = (angleDegrees*M_PI) / 180.0;
	float sinV = sin(rad);
	float cosV = cos(rad);
	float centerX = (img_width - 1) / 2;
	float centerY = (img_height - 1) / 2;

	unsigned char *dest_img = new unsigned char[(img_width * img_height) * 4];
	for (int i = 0; i<img_height; i++)
	{
		for (int j = 0; j<img_width; j++)
		{
			float u = (j - centerX);
			float v = (i - centerY);
			int offsetRGBA = (i)*img_width * 4 + (j) * 4;
			int x = (int)(centerX + u*cosV - v*sinV );
			int y = (int)(centerY + u*sinV + v*cosV );
			int rotateRGBA = x * 4 + y*img_width * 4;
			if (x > 0 && x < img_width && y > 0 && y < img_height) {
				for (int k = 0; k < 4; k++) {
					dest_img[rotateRGBA + k] = img_data[offsetRGBA + k];
				}
			}			
		}
	}
	//back
	rad = -(angleDegrees*M_PI) / 180.0;
	sinV = sin(rad);
	cosV = cos(rad);
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			float u = (j - centerX);
			float v = (i - centerY);
			
			int offsetRGBA = (i)*img_width * 4 + (j) * 4;
			int x = (int)(centerX + u*cosV - v*sinV);
			int y = (int)(centerY + u*sinV + v*cosV);
			int rotateRGBA = x * 4 + y*img_width * 4;
			
			if (x > 0 && x < img_width && y > 0 && y < img_height) {
				for (int k = 0; k < 4; k++) {
					dest_img[offsetRGBA + k] = img_data[rotateRGBA + k];
				}
			}
		}
	}
	img_data = dest_img;
	
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}

//------------------------Composing------------------------


void Application::loadSecondaryImge( QString filePath )
{
	mImageSrcSecond.load(filePath);

	renew();

	img_data2 = mImageSrcSecond.bits();
	img_width2 = mImageSrcSecond.width();
	img_height2 = mImageSrcSecond.height();
}

//////////////////////////////////////////////////////////////////////////
//
//	Composite the image A and image B by Over, In, Out, Xor and Atom. 
//
//////////////////////////////////////////////////////////////////////////
void Application::Comp_image( int tMethod )
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite the current image over the given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Over()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "in" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_In()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image "out" the given image.  See lecture notes for 
//  details.  Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Out()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite current image "atop" given image.  Return success of 
//  operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Atop()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//      Composite this image with given image using exclusive or (XOR).  Return
//  success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::Comp_Xor()
{
	if (img_height == img_height2 && img_width == img_width2)
	{

	}
	else
	{
		std::cout << "Images not the same size" << std::endl;
	}
}

//------------------------NPR------------------------

///////////////////////////////////////////////////////////////////////////////
//
//      Run simplified version of Hertzmann's painterly image filter.
//      You probably will want to use the Draw_Stroke funciton and the
//      Stroke class to help.
// Return success of operation.
//
///////////////////////////////////////////////////////////////////////////////
void Application::NPR_Paint()
{
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32 );
	renew();
}

void Application::NPR_Paint_Layer( unsigned char *tCanvas, unsigned char *tReferenceImage, int tBrushSize )
{

}

///////////////////////////////////////////////////////////////////////////////
//
//      Helper function for the painterly filter; paint a stroke at
// the given location
//
///////////////////////////////////////////////////////////////////////////////
void Application::Paint_Stroke( const Stroke& s )
{
	int radius_squared = (int)s.radius * (int)s.radius;
	for (int x_off = -((int)s.radius); x_off <= (int)s.radius; x_off++) 
	{
		for (int y_off = -((int)s.radius); y_off <= (int)s.radius; y_off++) 
		{
			int x_loc = (int)s.x + x_off;
			int y_loc = (int)s.y + y_off;

			// are we inside the circle, and inside the image?
			if ((x_loc >= 0 && x_loc < img_width && y_loc >= 0 && y_loc < img_height)) 
			{
				int dist_squared = x_off * x_off + y_off * y_off;
				int offset_rgba = (y_loc * img_width + x_loc) * 4;

				if (dist_squared <= radius_squared) 
				{
					img_data[offset_rgba + rr] = s.r;
					img_data[offset_rgba + gg] = s.g;
					img_data[offset_rgba + bb] = s.b;
					img_data[offset_rgba + aa] = s.a;
				} 
				else if (dist_squared == radius_squared + 1) 
				{
					img_data[offset_rgba + rr] = (img_data[offset_rgba + rr] + s.r) / 2;
					img_data[offset_rgba + gg] = (img_data[offset_rgba + gg] + s.g) / 2;
					img_data[offset_rgba + bb] = (img_data[offset_rgba + bb] + s.b) / 2;
					img_data[offset_rgba + aa] = (img_data[offset_rgba + aa] + s.a) / 2;
				}
			}
		}
	}
}





///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke() {}

///////////////////////////////////////////////////////////////////////////////
//
//      Build a Stroke
//
///////////////////////////////////////////////////////////////////////////////
Stroke::Stroke(unsigned int iradius, unsigned int ix, unsigned int iy,
	unsigned char ir, unsigned char ig, unsigned char ib, unsigned char ia) :
radius(iradius),x(ix),y(iy),r(ir),g(ig),b(ib),a(ia)
{
}
void swap(int&a, int&b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}
void quickSort(int arr[], int left, int right)
{
	int temp;
	if (left < right)
	{
		int pivot = arr[left];//假設pivot在第一個的位置
		int l = left;
		int r = right + 1;

		while (1)
		{
			while (l < right && arr[++l] < pivot);//向右找小於pivot的數值的位置
			while (r > 0 && arr[--r] > pivot);//向左找大於pivot的數值的位置

			if (l >= r)//範圍內pivot右邊沒有比pivot小的數,反之亦然
			{
				break;
			}

			swap(arr[l], arr[r]);//比pivot大的數移到右邊，比pivot小的數換到左邊
		}

		swap(arr[left], arr[r]);//將pivot移到中間

		quickSort(arr, left, r - 1);//左子數列做遞迴
		quickSort(arr, r + 1, right);//右子數列做遞迴
	}

}

void Application::Filter_Mean(int k)
{
	unsigned char *rgb = To_RGB();
	filter(k,false);
}

void Application::Filter_Median_Quick(int k)
{
	unsigned char *rgb = this->To_RGB();
	int step = k / 2;
	//rgb_data = new float[img_height*img_width * 3];
	int start = clock();	
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int offset_rgb = i * img_width * 3 + j * 3;
			int offset_rgba = i * img_width * 4 + j * 4;

			double Rsum = 0, Gsum = 0, Bsum = 0;
		
			for (int q = 0; q < 3; q++)
			{
				int count = 0;
				int *num = new int[k*k];
				for (int k = -step; k <= step; k++)
				{
					for (int g = -step; g <= step; g++)
					{
						if (k + i < 0 || k + i >= img_height)
							continue;
						if (g + j < 0 || g + j >= img_width)
							continue;

						num[count] = rgb[(i + k)*img_width * 3 + (j + g) * 3 + q];
						count++;
					
					}
				}
				quickSort(num, 0, count-1);
				img_data[offset_rgba + q] = num[count / 2];
			}			
		}
	}
	printf("Time:%d mask size:%dx%d Quick sort median.\n", clock() - start, k, k);
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}

void Application::Filter_Median_Bubble(int k)
{
	unsigned char *rgb = this->To_RGB();
	int step = k / 2;
	//rgb_data = new float[img_height*img_width * 3];
	int start = clock();
	for (int i = 0; i < img_height; i++)
	{
		for (int j = 0; j < img_width; j++)
		{
			int offset_rgb = i * img_width * 3 + j * 3;
			int offset_rgba = i * img_width * 4 + j * 4;

			double Rsum = 0, Gsum = 0, Bsum = 0;

			for (int q = 0; q < 3; q++)
			{
				int count = 0;
				int *num = new int[k*k];
				for (int k = -step; k <= step; k++)
				{
					for (int g = -step; g <= step; g++)
					{
						if (k + i < 0 || k + i >= img_height)
							continue;
						if (g + j < 0 || g + j >= img_width)
							continue;

						num[count] = rgb[(i + k)*img_width * 3 + (j + g) * 3 + q];
						count++;

					}
				}
				for (int c = 0; c < (count - 1); c++)
				{
					for (int d = 0; d < count - c - 1; d++)
					{
						if (num[d] > num[d + 1]) /* For decreasing order use < */
						{
							int swap = num[d];
							num[d] = num[d + 1];
							num[d + 1] = swap;
						}
					}
				}
				img_data[offset_rgba + q] = num[count / 2];
			}
		}
	}
	printf("Time:%d mask size:%dx%d Bubble sort median.\n", clock() - start, k,k);
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}

void Application::Filter_Windyga(int n)
{
	unsigned char *rgb = this->To_RGB();
	int start = clock();
	for (int i = 0; i < img_height; i++){
		for (int j = 0; j < img_width; j++){
			int offset_rgb;
			int offset_rgba = i*img_width * 4 + j * 4;
			//float newColor[3] = { 0 };
			float record[3][100] = { 0 };
			int record_count = 0;
			for (int ni = i - 2; ni <= i + 2; ++ni) {
				if (ni >= img_height || ni < 0)continue;
				for (int nj = j - 2; nj <= j + 2; ++nj) {
					if (nj >= img_width || nj < 0)continue;
					offset_rgb = ni*img_width * 3 + nj * 3;

					record[0][record_count] = rgb[offset_rgb + rr];
					record[1][record_count] = rgb[offset_rgb + gg];
					record[2][record_count] = rgb[offset_rgb + bb];
					record_count++;
				}
			}
			float max[3],min[3];
			int isMax[3] = { 0 }, isMin[3] = { 0 };

			for (int QQ = 0; QQ < 3; QQ++){
				max[QQ] = record[QQ][((n*n) / 2)];
				min[QQ] = record[QQ][((n*n) / 2)];
				for (int k = 0; k < (n*n); k++){
					if (k != (n*n) / 2){
						if (max[QQ] > record[QQ][k]){
							isMax[QQ]++;
						}
						else if (min[QQ] < record[QQ][k]){
							isMin[QQ]++;
						}
					}
				}

				if (isMax[QQ] == (n*n)-1){
					float min_distance = 1000;
					for (int k = 0; k < n*n; k++){
						if (k != (n*n)/2){
							if (record[QQ][(n*n)/2] - record[QQ][k] < min_distance){
								min_distance = record[QQ][(n*n)/2] - record[QQ][k];
							}
						}
					}
					record[QQ][(n*n)/2] -= min_distance;
				}
				else if (isMin[QQ] == (n*n)-1){
					float min_distance = 1000;
					for (int k = 0; k < n*n; k++){
						if (k != (n*n)/2){
							if (record[QQ][k] - record[QQ][(n*n)/2]  < min_distance){
								min_distance = record[QQ][k] - record[QQ][(n*n)/2];
							}
						}
					}
					record[QQ][(n*n)/2] += min_distance;
				}
				img_data[offset_rgba + QQ] = record[QQ][(n*n)/2];
				//img_data[offset_rgba + aa] = WHITE;
			}
			img_data[offset_rgba + aa] = WHITE;
		}
	}
	printf("Time:%d mask size:%dx%d Windyga.\n", clock() - start, n, n);
	delete[] rgb;
	mImageDst = QImage(img_data, img_width, img_height, QImage::Format_ARGB32);
	renew();
}
void Application::Filter_CentralWeightedMedian(int k) {		
	if (img_width > 0) {
		unsigned char **mask = new unsigned char*[k];
		for (size_t i = 0; i < k; i++)
		{
			mask[i] = new unsigned char[k];
		}
		filter(k,true);	
	}	
}


