#include "GrdCC.h"


inline double myCostGrd( double* lC, double* rC,
	double* lG, double* rG )
{
	double clrDiff = 0;
	// three color
	for( int c = 0; c < 3; c ++ ) {
		double temp = fabs( lC[ c ] - rC[ c ] );
		clrDiff += temp;
	}
	clrDiff *= 0.3333333333;
	// gradient diff
	double grdDiff = fabs( lG[ 0 ] - rG[ 0 ] );
	clrDiff = clrDiff > TAU_1 ? TAU_1 : clrDiff;
	grdDiff = grdDiff > TAU_2 ? TAU_2 : grdDiff;
	return ALPHA * clrDiff + ( 1 - ALPHA ) * grdDiff;
}
// specail handle for border region
inline double myCostGrd( double* lC, double* lG )
{
	double clrDiff = 0;
	// three color
	for( int c = 0; c < 3; c ++ ) {
		double temp = fabs( lC[ c ] - BORDER_CONSTANT);
		clrDiff += temp;
	}
	clrDiff *= 0.3333333333;
	// gradient diff
	double grdDiff = fabs( lG[ 0 ] - BORDER_CONSTANT );
	clrDiff = clrDiff > TAU_1 ? TAU_1 : clrDiff;
	grdDiff = grdDiff > TAU_2 ? TAU_2 : grdDiff;
	return ALPHA * clrDiff + ( 1 - ALPHA ) * grdDiff;
}
void GrdCC::buildCV( const Mat& lImg, const Mat& rImg, const int maxDis, Mat* costVol )
{
	// for TAD + Grd input image must be CV_64FC3
	CV_Assert( lImg.type() == CV_64FC3 && rImg.type() == CV_64FC3 );

	int hei = lImg.rows;
	int wid = lImg.cols;
	Mat lGray, rGray;
	Mat lGrdX, rGrdX;
	Mat tmp;
	lImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, lGray, CV_RGB2GRAY );
	rImg.convertTo( tmp, CV_32F );
	cvtColor( tmp, rGray, CV_RGB2GRAY );
	// X Gradient
	// sobel size must be 1
	Sobel( lGray, lGrdX, CV_64F, 1, 0, 1 );
	Sobel( rGray, rGrdX, CV_64F, 1, 0, 1 );
	lGrdX += 0.5;
	rGrdX += 0.5;
	// build cost volume! start from 1
	// try 0
	for( int d = 0; d < maxDis; d ++ ) {
		printf( "-c-c-" );
		for( int y = 0; y < hei; y ++ ) {
			double* lData = ( double* ) lImg.ptr<double>( y );
			double* rData = ( double* ) rImg.ptr<double>( y );
			double* lGData = ( double* ) lGrdX.ptr<double>( y );
			double* rGData = ( double* ) rGrdX.ptr<double>( y );
			double* cost   = ( double* ) costVol[ d  ].ptr<double>( y );
			for( int x = 0; x < wid; x ++ ) {
				if( x - d >= 0 ) {
					double* lC = lData + 3 * x;
					double* rC = rData + 3 * ( x - d );
					double* lG = lGData + x;
					double* rG = rGData + x - d;
					cost[ x ] = myCostGrd( lC, rC, lG, rG );
				} else {
					double* lC = lData + 3 * x;
					double* lG = lGData + x;
					cost[ x ] = myCostGrd( lC, lG );
				}

			}
		}
	}
}