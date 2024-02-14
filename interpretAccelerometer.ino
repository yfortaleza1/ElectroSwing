#define DEFAULT_FORCE 0
#define DEFAULT_MOTOR_STRENGTH 0
#define NUM_ACCELEROMETER_FLOATS 3 // (x,y,z)
#define X_INDEX 0
#define Y_INDEX 1
#define Z_INDEX 2

#include <math.h> 

double forceToMotorStrength(double force) 
{
    // in case we want to test by supplying Newtons or something similar,
    // we could convert that to motor strength 
    return DEFAULT_FORCE;
}

/* Source: https://wizmoz.blogspot.com/2013/01/simple-accelerometer-data-conversion-to.html 
Written for a ADXL345 accelerometer.
*/
double accelerationToAngle(double accelerometer_reading[NUM_ACCELEROMETER_FLOATS]) {
    /* example accelerometer_reading = adxl.get_Gxyz(xyz); */
    double xyz[3] = {accelerometer_reading[X_INDEX],
                     accelerometer_reading[Y_INDEX],
                     accelerometer_reading[Z_INDEX]};
    double ax, ay, az;
    ax = xyz[0];
    ay = xyz[1];
    az = xyz[2];
    double xAngle = atan( ax / (sqrt(pow(ay, 2) + pow(az, 2))));
    double yAngle = atan( ay / (sqrt(pow(ax, 2) + pow(az, 2))));
    double zAngle = atan( sqrt(pow(ax, 2) + pow(ay, 2)) / az);

    xAngle *= 180.00;   yAngle *= 180.00;   zAngle *= 180.00;
    xAngle /= 3.141592; yAngle /= 3.141592; zAngle /= 3.141592;
    // todo: figure out which angle our motor is acting in
    // for now just assuming its x
    return xAngle;
}

bool areLengthsMatching(int array1[], int array2[], int length) 
{
    if ((sizeof(array1) / sizeof(int)) != 
        (sizeof(array2) / sizeof(int)) != 
        length)  {
        return false;
    } else {
        return true;
    }
} 

float getMotorStrength(int angle_ranges_arr[], int strengths_arr[],
                       int arr_length, 
                       double accelerometer_reading[NUM_ACCELEROMETER_FLOATS]) 
{
    /*
    Use defined theta ranges to determine what motor strength to apply
    Example `angle_ranges_arr`: {0, 15, 30, 45, 60}
        I will just use every two with the first being non-inclusive 
        and the last being inclusive.
        i.e. [0, 15), [15, 30), [30, 45)
    */
    return DEFAULT_MOTOR_STRENGTH;
}