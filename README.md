# Sensor Fusion with Complementary Filter

## Objective

Fuse accelerometer and gyroscope data from an MPU6050 into a stable pitch and roll estimate using a complementary filter. The project shows a practical, low-compute alternative to a full Kalman filter that is still accurate enough for many balancing and orientation tasks.

## Strategy

- Read the MPU6050 at a fixed rate and convert raw values to degrees per second and g.
- Integrate the gyroscope to obtain a short-term angle.
- Compute an accelerometer-based angle from the gravity vector.
- Blend the two with a complementary filter: high weight on the gyro for fast changes, high weight on the accelerometer for long-term stability.
- Stream the filtered angles over serial for visualization or closed-loop control.

## What worked

- A filter coefficient around 0.98 for the gyro and 0.02 for the accelerometer removed most gyro drift while still tracking rapid motion.
- Computing the accelerometer angle with atan2 kept the result continuous across quadrant boundaries.
- A fixed 10 ms loop time made the integration step consistent without needing a high-resolution timer.

## What failed and how it was resolved

- Pure gyro integration drifted several degrees per minute. Resolution: continuous correction from the accelerometer via the complementary filter.
- Using a simple complementary filter without converting units produced incorrect scaling. Resolution: apply the proper sensitivity factors from the MPU6050 datasheet before fusion.
- Reading the sensor as fast as possible without a controlled sample period made the filter gain inconsistent. Resolution: enforce a fixed delta-t with millis().

## Engineering principles and frameworks used

- Complementary filtering as a lightweight sensor-fusion method.
- Fixed sample rate for discrete-time integration.
- Direct I2C register access.
- Observability of both raw and filtered quantities.

## Hardware

- Arduino Uno or compatible
- MPU6050 module

## Wiring

| MPU6050 | Arduino |
|---------|---------|
| VCC     | 3.3 V or 5 V |
| GND     | GND     |
| SCL     | A5      |
| SDA     | A4      |

## Software

Upload complementary_filter.ino. Serial Monitor at 115200 baud shows pitch and roll in degrees.

## Possible extensions

- Replace the complementary filter with a 1-D Kalman filter for comparison.
- Use the filtered angles to drive a balancing controller.
- Add magnetometer data for yaw.
