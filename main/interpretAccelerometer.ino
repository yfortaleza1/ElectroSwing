// todo: ensure we are compatible with https://seeeddoc.github.io/Grove-3-Axis_Digital_Accelerometer-16g/

// todo: consider constant acceleration and decelleration 
// i.e. from this person building a robot arm:
// https://www.youtube.com/watch?v=Im2Dnc9_Ifo

#define DEFAULT_FORCE 0
#define DEFAULT_MOTOR_STRENGTH 0
#define NUM_ACCELEROMETER_FLOATS 3 // (x,y,z)
#define INVALID_DOMAINS -1

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
todo: verify if this is similar to the accelerometer we actually have 
https://seeeddoc.github.io/Grove-3-Axis_Digital_Accelerometer-16g/
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

bool areLengthsMatching(int angle_ranges_arr[], int strengths_arr[], int num_strengths) 
{
    // todo: fix to work with {t1, t2, t3} and {s1, s2}. 
    if ((sizeof(angle_ranges_arr) / sizeof(int)) - 1 != 
        (sizeof(strengths_arr) / sizeof(int)) != 
        num_strengths)  {
        return false;
    } else {
        return true;
    }
} 

float getMotorStrength(int angle_ranges_arr[], int strengths_arr[],
                       int strengths_length, 
                       double accelerometer_reading[NUM_ACCELEROMETER_FLOATS]) 
{
    /*
    Use defined theta ranges to determine what motor strength to apply
    todo: if using this {t1, t2, ...} strat instead of pairs, figure out how to make sure lengths match.
    Example `angle_ranges_arr`: {0, 15, 30, 45, 60}
        I will just use every two with the first being non-inclusive 
        and the last being inclusive.
        i.e. [0, 15), [15, 30), [30, 45)
    Example `strengths_arr`: {5000, 4000, 3000, 2000, 1000}
        So with the example angles above it would apply the following strengths 
        for the given ranges
        5000: [0, 15)
        4000: [15, 30)
        3000: [30, 45)
        2000: [45, 60)
    */
    if (! areLengthsMatching(angle_ranges_arr, strengths_arr, strengths_length)) {  
        return INVALID_DOMAINS;
    }

    double angle_reading = accelerationToAngle(accelerometer_reading);
    for (int i = 0; i < strengths_length; i ++) {
        int curr_min_angle = angle_ranges_arr[i];
        int curr_max_angle = angle_ranges_arr[i+1];
        int curr_strength = strengths_arr[i];
        if (curr_min_angle <= angle_reading < curr_max_angle) {
            return curr_strength;
        }
    }
    // if its not in any of the specified range, 
    return DEFAULT_MOTOR_STRENGTH;
}