
#include "YieldCurve.h"
#include <cmath>

using std::string;

#define DEFAULT_LIBOR_GRID 0.0027, 0.0192, 0.0833, 0.1667, 0.2500, 0.5000, 1
#define DEFAULT_LIBOR_GRID_NUM 7
#define NUM_OF_INTPLTDSWPGRD 59
#define NUM_OF_DEFAULT_ZRRTGRD 65


//constructor
YieldCurve::YieldCurve(){
	libor = NULL;
	liborGrid = NULL;
	swapRate = NULL;
	swapGrid = NULL;
	designatedDiscountFactor = NULL;
	designatedZeroRate = NULL;
 	designatedGrid = NULL;
	double tmpArray[] = {DEFAULT_LIBOR_GRID};
	for(int i = 0; i < DEFAULT_LIBOR_GRID_NUM; i++){
		defaultLiborGrid[i] = tmpArray[i];
	}
	buildZeroRateFlag = 0;
}

YieldCurve::YieldCurve(string cur, const double *liborValue, int liborGridNum, const double *liborgrid, 
						const double *swapValue, int swapGridNum, const double *swapgrid){
	//Currency set
	currency = cur;
	//Libor set
	numOfLiborGrid = liborGridNum;
	libor = new double[numOfLiborGrid];
	for(int i = 0; i < numOfLiborGrid; ++i){
		libor[i] = liborValue[i];
	}
	//LiborGrid set
	liborGrid = new double[numOfLiborGrid];
	for(int i = 0; i < numOfLiborGrid; ++i){
		liborGrid[i] = liborgrid[i];
	}
	//Swap set
	numOfSwapGrid = swapGridNum;
	swapRate = new double[numOfSwapGrid];
	for(int i = 0; i < numOfSwapGrid; i++){
		swapRate[i] = swapValue[i];
	}
	//SwapGrid set
	swapGrid = new double[numOfSwapGrid];
	for(int i = 0; i < numOfSwapGrid; ++i){
		swapGrid[i] = swapgrid[i];
	}

	designatedDiscountFactor = NULL;
	designatedZeroRate = NULL;
 	designatedGrid = NULL;

	double tmpArray[] = {DEFAULT_LIBOR_GRID};
	for(int i = 0; i < DEFAULT_LIBOR_GRID_NUM; i++){
		defaultLiborGrid[i] = tmpArray[i];
	}

	buildZeroRateFlag = 0;
}

//Destructor
YieldCurve::~YieldCurve(){
	if(libor != NULL){
		delete[] libor;
	}
	if(liborGrid != NULL){
		delete[] liborGrid;
	}
	if(swapRate != NULL){
		delete[] swapRate;
	}
	if(swapGrid != NULL){
		delete[] swapGrid;
	}
	if(designatedDiscountFactor != NULL){
		delete[] designatedDiscountFactor;
	}
	if(designatedZeroRate != NULL){
		delete[] designatedZeroRate;
	}
	if(designatedGrid != NULL){
		delete[] designatedGrid;
	}
}


//setter
void YieldCurve::setcur(string cur){
	currency = cur;
}

void YieldCurve::setlibor(const double *liborValue, const double *liborgrid, int liborGridNum){
	numOfLiborGrid = liborGridNum;
	if(libor != NULL){
		delete[] libor;
	}
	libor = new double[numOfLiborGrid];
	for(int i = 0; i < numOfLiborGrid; ++i){
		libor[i] = liborValue[i];
	}
	if(liborGrid != NULL){
		delete[] liborGrid;
	}
	liborGrid = new double[numOfLiborGrid];
	for(int i = 0; i < numOfLiborGrid; ++i){
		liborGrid[i] = liborgrid[i];
	}
	
	buildZeroRateFlag = 0;
}

void YieldCurve::setswapRate(const double *swapValue, int swapGridNum, const double *swapgrid){
	numOfSwapGrid = swapGridNum;
	if(swapRate != NULL){
		delete[] swapRate;
	}
	swapRate = new double[numOfSwapGrid];
	for(int i = 0; i < numOfSwapGrid; i++){
		swapRate[i] = swapValue[i];
	}
	if(swapGrid != NULL){
		delete[] swapGrid;
	}
	swapGrid = new double[numOfSwapGrid];
	for(int i = 0; i < numOfSwapGrid; ++i){
		swapGrid[i] = swapgrid[i];
	}
	
	buildZeroRateFlag = 0;
}


//getter
string YieldCurve::getcur(){
	return currency;
}
void YieldCurve::getLibor(double *liborVal){
	for(int i = 0; i < numOfLiborGrid; i++){
		liborVal[i] = libor[i];
	}
}
void YieldCurve::getSwapRate(double *swapRateVal){
	for(int i = 0; i < numOfSwapGrid; i++){
		swapRateVal[i] = swapRate[i];
	}
}
void YieldCurve::getinterpolatedSwapRate(double *interpolatedSwapRateVal){
	if(buildZeroRateFlag != 1){
		buildYieldCurve();
	}
	for(int i = 0; i < NUM_OF_INTPLTDSWPGRD; i++){
		interpolatedSwapRateVal[i] = interpolatedSwapRate[i];
	}
}
void YieldCurve::getDefaultGrid(double *defaultZRGrid){
	for(int i = 0; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		defaultZRGrid[i] = defaultZeroRateGrid[i];
	}
}

void YieldCurve::getZeroRate(double *rtnZeroRate){
	if(buildZeroRateFlag != 1){
		buildYieldCurve();
	}
	for(int i = 0; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		rtnZeroRate[i] = defaultZeroRate[i];
	}
	return;
}

void YieldCurve::getDiscountFactor(double *DF){
	if(buildZeroRateFlag != 1){
		buildYieldCurve();
	}
	for(int i = 0; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		DF[i] = defaultDiscountFactor[i];
	}
	return;
}


//GetZeroRate(designation grid)
void YieldCurve::getZeroRate(double *rtnZeroRate, int gridNum, const double *grid){
	NumOfDesignatedGrid = gridNum;
	if(designatedGrid != NULL){
		delete[] designatedGrid;
	}
	designatedGrid = new double[NumOfDesignatedGrid];
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		designatedGrid[i] = grid[i];
	}
	if(buildZeroRateFlag != 1){
		buildYieldCurve();
	}
	//Interpolation to designated grid
	//ZeroRate
	if(designatedZeroRate != NULL){
		delete[] designatedZeroRate;
	}
	designatedZeroRate = new double[NumOfDesignatedGrid];
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		designatedZeroRate[i] = interpolateRange(designatedGrid[i], defaultZeroRateGrid, defaultZeroRate, NUM_OF_DEFAULT_ZRRTGRD);
	}
	
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		rtnZeroRate[i] = designatedZeroRate[i];
	}
	return;
}

void YieldCurve::getDiscountFactor(double *DF, int gridNum, const double *grid){
	NumOfDesignatedGrid = gridNum;
	if(designatedGrid != NULL){
		delete[] designatedGrid;
	}
	designatedGrid = new double[NumOfDesignatedGrid];
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		designatedGrid[i] = grid[i];
	}
	if(buildZeroRateFlag != 1){
		buildYieldCurve();
	}
	//Interpolation to designated grid
	//DF
	if(designatedDiscountFactor != NULL){
		delete[] designatedDiscountFactor;
	}
	designatedDiscountFactor = new double[NumOfDesignatedGrid];
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		designatedDiscountFactor[i] = interpolateRange(designatedGrid[i], defaultZeroRateGrid, defaultDiscountFactor, NUM_OF_DEFAULT_ZRRTGRD);
	}
	
	for(int i = 0; i < NumOfDesignatedGrid; i++){
		DF[i] = designatedDiscountFactor[i];
	}
	return;
}

//Interpolate Func
double YieldCurve::interpolate(double preGrid, double postGrid, double preValue, double postValue, double targetGrid){
	double targetValue = 0;
	targetValue = preValue + (postValue - preValue) / (postGrid - preGrid) * (targetGrid - preGrid);
	return targetValue;
}

//InterpolateRange Func
double YieldCurve::interpolateRange(double targetGrid, double *gridArray, double *valueArray, int numOfArray){
	int i;
	//if targetgrid is out of argument range, return extrapolation value
	if(targetGrid <= gridArray[0]){
		return valueArray[0];
	}else if(targetGrid >= gridArray[numOfArray-1]){
		return valueArray[numOfArray-1];
	}
	
	for(i = 1; i < numOfArray; i++){
		if(gridArray[i] >= targetGrid) break;
	}
	return interpolate(gridArray[i-1], gridArray[i], valueArray[i-1], valueArray[i], targetGrid);
}

void YieldCurve::buildYieldCurve(){
	liborInterpolation();
	bootstrap();
	buildZeroRateFlag = 1;
}


void YieldCurve::liborInterpolation(){
	//Interpolation LIBOR
	for(int i = 0; i < DEFAULT_LIBOR_GRID_NUM; i++){
		defaultGridLibor[i] = interpolateRange(defaultLiborGrid[i], liborGrid, libor, DEFAULT_LIBOR_GRID_NUM);
	}
	//LIBOR to DF
	for(int i = 0; i < DEFAULT_LIBOR_GRID_NUM; i++){
		defaultDiscountFactor[i] = 1 / (1 + defaultGridLibor[i] * defaultLiborGrid[i]);
	}
}


//bootstrap Func
void YieldCurve::bootstrap(){

	using std::log;

	//Interpolate SwapRate every half a year
	for(int i = 0; i < NUM_OF_INTPLTDSWPGRD; i++){
		interpolatedSwapGrid[i] = 1 + i *0.5;
	}
	interpolatedSwapRate[0] = swapRate[0];
	int j = 1;
	for(int i = 1; i < NUM_OF_INTPLTDSWPGRD; i++){
		if(interpolatedSwapGrid[i] == swapGrid[j]){
			interpolatedSwapRate[i] = swapRate[j];
			j++;
		}else{
			interpolatedSwapRate[i] = interpolate(swapGrid[j-1], swapGrid[j], swapRate[j-1], swapRate[j], interpolatedSwapGrid[i]);
		}
	}
	
	//BootStrap(create DF)
	double sumDF = defaultDiscountFactor[5];
	int k = 1;
	for(int i = DEFAULT_LIBOR_GRID_NUM; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		sumDF = sumDF + defaultDiscountFactor[i-1];
		defaultDiscountFactor[i] = (1 - sumDF * interpolatedSwapRate[k] / 2)
							 / (1 + interpolatedSwapRate[k] / 2);
		k++;
	}
	
	//from DF to ZeroRate
	for(int i = 0; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		defaultZeroRateGrid[i] = liborGrid[i];
	}	
	for(int i = DEFAULT_LIBOR_GRID_NUM; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		defaultZeroRateGrid[i] = interpolatedSwapGrid[i-6];
	}
	for(int i = 0; i < NUM_OF_DEFAULT_ZRRTGRD; i++){
		defaultZeroRate[i] = (log(defaultDiscountFactor[i])) / (defaultZeroRateGrid[i] * -1);
	}
	
}


