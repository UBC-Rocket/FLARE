#ifndef CAMERAS_H_2D865819C2084936A8B94C592E41FE26
#define CAMERAS_H_2D865819C2084936A8B94C592E41FE26

class Camera {
public:
    bool is_recording = true;
    void start_record() {
        is_recording = true;
    }

    void stop_record() {
        is_recording = false;
    }
};
#endif
