#include "RocketSchedule/fake_clocks.hpp"
#include "cameras.h"
#include "scheduler.hpp"
#include "tasks/restart_camera.hpp"
#include "gtest/gtest.h"

TEST(RestartCamera, Restart) {
    int taskID = static_cast<int>(TaskID::RestartCamera);
    RocketSchedule::StoppingClock stopping_clock(22005);
    Camera cam;
    RestartCamera restart_camera{cam};
    Scheduler::Task restart_camera_task(RestartCamera::togglePower,
                                        &restart_camera, 0);
    Scheduler::preregisterTask(taskID, restart_camera_task, true, false);
    Scheduler::scheduleTask(0, taskID);

    try {
        Scheduler::runNext();
        // REVIEW: restart_camera_task.period always reports 0
        ASSERT_EQ(Scheduler::get_task(taskID).period,
                  CAMERA_POWEROFF_DURATION_MS);
        ASSERT_EQ(cam.is_recording, false);
        Scheduler::runNext();
        ASSERT_EQ(Scheduler::get_task(taskID).period,
                  CAMERA_RESTART_INTERVAL_MS);
        ASSERT_EQ(cam.is_recording, true);
        Scheduler::runNext();
        ASSERT_EQ(Scheduler::get_task(taskID).period,
                  CAMERA_POWEROFF_DURATION_MS);
        ASSERT_EQ(cam.is_recording, false);
    } catch (RocketSchedule::ClockFinishedExc &) {
    }
}
