#ifndef ROVER5_H
#define ROVER5_H

#include <Arduino.h>
#include <Wire.h>

/**
 * @brief A class to control our Rover5 robot platform.
 *
 * This class could theoretically control any four wheeled robot with
 * mecanum wheels and encoders on each one with an interface arduino
 * that communicates with a protocol defined in the protocol file
 */
class Rover5 {
public:
   /**
    * Constants used to specify which motor to use or to index any array of
    * motors in this class
    */
    enum mtrNum_t { BR = 0, FR = 1, BL = 2, FL = 3 };

   /**
    * @param i2caddress The i2c address of the interface arduino.
    */
    Rover5(uint8_t i2caddress = 12);

   /**
    * @brief Sets up class.
    *
    * This should be called after init() (ie. in setup()).
    */
    void begin();


   /**
    * @brief Run motors directly.
    *
    * @param frontLeft  The power, from -255 to 255, of the front left motor.
    * @param frontRight The power, from -255 to 255, of the front right motor.
    * @param backLeft   The power, from -255 to 255, of the back left motor.
    * @param backRight  The power, from -255 to 255, of the back right motor.
    */
    void Run(int frontLeft, int frontRight, int backLeft, int backRight);
   /**
    * @brief Run motors directly.
    *
    * @param powers An array of powers, from -255 to 255 for each motor..
    */
    void Run(int powers[4]);
   /**
    * @brief Run the robot in a direction.
    *
    * This should be the main movement function used by a programmer.
    * Note that there is scaling in this function (and this function only)
    *  so that even a power of 1 will move the robot, so there is no deadband
    * @param x The power, from -255 to 255, of the lateral direction.
    * @param y The power, from -255 to 255, of the forward direction.
    * @param z The power, from -255 to 255, of rotation.
    */
    void Run(int x, int y, int z = 0);
   /**
    * @brief Run one motor.
    *
    * @param power The power to move the motor.
    * @param num   The motor to run.
    */
    void Run(int power, mtrNum_t num);

   /**
    * @brief Gets the most recently specified powers for each motor.
    *
    * @param powers The array to put the powers into.
    */
    void GetPowers(int powers[4]);

   /**
    * @brief Gets the current encoder counts for each motor.
    *
    * This function populates the ticks array with the current encoder counts
    * for each motor.
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param ticks The array to put the ticks into.
    */
    void GetTicks(long ticks[4]);


   /**
    * @brief Gets the current speeds for each motor.
    *
    * This function populates the speeds array with the current speeds in
    * milliinches (mills) per second for each motor.
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param speeds The array to put the speeds into.
    */
    void GetSpeeds(int speeds[4]);
    
   /**
    * @brief Gets the current speeds for each motor.
    *
    * This function populates the speeds array with the current speeds in
    * encoder ticks per second for each motor
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param speeds The array to put the speeds into.
    */
    void GetTickSpeeds(int speeds[4]);

   /**
    * @brief Get new encoder values and update current position and speed
    *
    * Call this function before calling any methods that use the encoders.
    * @return whether or not encoders were updated (ie no problems).
    */
    bool UpdateEncoders();
    
   /**
    * @brief Gets the current distances for each motor.
    *
    * This function populates the dists array with the current scaled encoder
    * counts for each motor, in milliinches (mills).
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param dists The array to put the distances into.
    */
    void GetDists(long dists[4]);

   /**
    * @brief Gets the current distance the robot has gone.
    *
    * This function just uses the current encoder counts.
    *
    * The UpdateEncoders() method should be called before calling this
    * method.
    *
    * @return The forward distance the robot has gone in mills
    */
    long GetDist();
   /**
    * @brief Gets the current distance the robot has gone.
    *
    * This function just uses the current encoder counts.
    *
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param xdist The variable to store the lateral distance the robot has
    *              gone in mills.
    * @param ydist The variable to store the forward distance the robot has
    *              gone in mills.
    */
    void GetDist(long* xdist, long* ydist);


   /**
    * @brief Gets the current (x,y) position of the robot
    *
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param xpos The variable to store the x position of the robot in mills.
    * @param xpos The variable to store the x position of the robot in mills.
    */
    void GetPos(long* xpos, long* ypos);
   /**
    * @brief Gets the current position and angle of the robot
    *
    * The UpdateEncoders() method should be called before calling this
    * method.
    * @param xpos The variable to store the x position of the robot in mills.
    * @param xpos The variable to store the x position of the robot in mills.
    * @param angle The variable to store the current angle of the robot in
    *              milliradians
    */
    void GetPos(long* xpos, long* ypos, unsigned int* angle);



    /**
    * @brief Sets the i2c address of the interface arduino
    */
    void SetI2CAddress(uint8_t address) { interfaceAddress = address; }

private:
    /// Minimum power for a motor to move
    static const int minPower = 25;
    /// The radius of the wheels in mills
    static const int wheelRadius = 2000;

    /// Factor to multiple ticks by to get mills
    // 2 pi * radius = circumference
    // there are 3 rotations of the wheel per 1000 ticks [rover5 manual]
    // Ends up being 37.699111843077518861551720599354034610366032792501269
#if __cplusplus >= 201103L
    constexpr
#endif
    static const double ticksToMills = (TWO_PI * wheelRadius * 3.0)/1000.0;
    //static const double ticksToMills = 1;

    /// i2c Address of the interface arduino
    uint8_t interfaceAddress;

    /// Current number of encoder ticks for each motor
    long ticks[4];

    /// Current speeds for each motor in encoder ticks/second
    /// Maximum realistic value is about 25000
    int speeds[4];

    /// Current powers for each motor from -255 to 255
    int powers[4];

    /// Current position of the robot, obtained from encoder data, in ticks
    ///  and milliradians for angle
    //struct { long x; long y; unsigned int angle; } pos;
    struct { float x; long y; float angle; } pos;

    // Circular buffer class holding the last ten tick counts and times
    template <uint8_t bufsz> struct TickLogs {
        long ticks[bufsz][4];
        unsigned long times[bufsz];
        uint8_t nextEntry;

        TickLogs() : nextEntry(0) {}

        void Put(long iticks[4], unsigned long time) {
            memcpy(ticks[nextEntry], iticks, sizeof(ticks[0]));
            times[nextEntry] = time;

            nextEntry = (nextEntry >= bufsz-1)? 0 : nextEntry+1;
        }
    };

    static const uint8_t spdLogLen = 10;
    //static const uint8_t spdLogLen = 1;
    TickLogs<spdLogLen> tickLogs;

    // Scales array down to be under maximum but the same relative to
    //  each other
   /**
    * @brief Scales array down to be under maximum but same relative to each
    *        other.
    * @param nums Array to scale down
    * @param maximum Maximum value that any number in nums can be
    */
    void Normalize4(int nums[4], int maximum);

   /**
    * @brief Runs motors using values stored in powers array
    */
    void Run();

   /**
    * @brief Updates the speeds as part of the UpdateEncoders function
    * @param ticks The current value of the ticks array
    */
    void UpdateSpeeds(long ticks[4]);

   /**
    * @brief Updates the pos array to reflect the current position
    */
    void UpdatePosition();
};

#endif // ROVER5_H
