#include "tasks/restart_camera.hpp"

void RestartCamera::togglePower() {
    Scheduler::Task &this_task =
        Scheduler::get_task(static_cast<int>(TaskID::RestartCamera));

    if (camera_on_) {
        cam_.stop_record();
        camera_on_ = false;
        this_task.period = Scheduler::Duration{CAMERA_POWEROFF_DURATION_MS};
    } else {
        cam_.start_record();
        camera_on_ = true;
        this_task.period = Scheduler::Duration{CAMERA_RESTART_INTERVAL_MS};
    }
}
