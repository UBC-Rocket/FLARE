/**
 * State Machine Header
 *
 * @file    state_list.h
 * @author  UBC Rocket Avionics 2018/2019
 * @description   Convenience header including all of the rocket states.
 *
 * @section LICENSE
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * Distributed as-is; no warranty is given.
 */

#pragma once
#include "states/armed.h"
#include "states/ascent_to_apogee.h"
#include "states/drogue_descent.h"
#include "states/landed.h"
#include "states/mach_lock.h"
#include "states/main_descent.h"
#include "states/powered_ascent.h"
#include "states/pre_air_start_coast_timed.h"
#include "states/pressure_delay.h"
#include "states/standby.h"
#include "states/winter_contingency.h"
