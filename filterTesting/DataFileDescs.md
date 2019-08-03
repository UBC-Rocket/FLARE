## SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout
Raw pressure data collected from Sky-Pilot at IREC 2019, using the body system. Only data after motor burnout is kept, and to reduce file size data has been trimmed at some point after apogee.

## SkyPilot_28-07-2019_Sim[X]
Datasets from simulation of Sky Pilot, just before IREC 2019. It was converted into pressure and time data. Data has been adjusted to match the 20 Hz polling speed. Apogee is known to occur at t = 39.23 seconds, although since data is polled at 20 Hz, there can't be an exact match.

### Sim 0
Adds a random Gaussian distribution each pressure value (std.dev of 0.4 mbar, >10 times that of typical on ground = 0.035, and the resulting data looks a lot more noisy than what was observed on Sky-Pilot), and rounds to the nearest 0.027 mbar to simulate quantization error (this is the resolution of the value returned from the barometer on each read). Initial values are intentionally inaccurate, as a test for how well data converges. Data starts around 10 seconds before apogee.

This data set serves as a worst case scenario, given the extreme levels of noise and poor initial values. It is not representative of typical flight data.

### Sim 1 through 5
Adds a random Gaussian distribution each pressure value (std.dev of typical on ground = 0.035), and rounds to the nearest 0.027 mbar to simulate quantization error (this is the resolution of the value returned from the barometer on each   read). These data sets are idealized data sets.


