## SkyPilot_IREC_2019_Dataset_Post_Motor_Burnout
Raw pressure data collected from Sky-Pilot at IREC 2019, using the body
system. Only data after motor burnout is kept, and to reduce file size data has been trimmed at some point after apogee.

## SkyPilot_28-07-2019_Sim[X]
Datasets from simulation of Sky Pilot, just before IREC 2019. It was converted
into pressure and time data. Data has been adjusted to match the 20 Hz polling
speed.

### Sim 1
Adds a uniform random value between -2.5 and 2.5 mbar to each pressure value
(these are the limits given on the datasheet), and rounds to the nearest
0.027 mbar to simulate quantization error (this is the resolution of the value
returned from the barometer on each read).
