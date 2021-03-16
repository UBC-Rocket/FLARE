#ifndef FAKE_TASKS_HPP_5DF011EA55614771AC70289249DFEE44
#define FAKE_TASKS_HPP_5DF011EA55614771AC70289249DFEE44

#include "RocketSchedule/fake_clocks.hpp"
#include "schedule_defn.hpp"
#include "task_tracking.hpp"

class SimpleTask;
inline ITaskLogger &operator<<(ITaskLogger &logger, SimpleTask *task);

class SimpleTask {
  public:
    SimpleTask(int id, ITaskLogger &log) : call_count(0), id_(id), log_(log) {}
    void action() {
        call_count++;
        log_ << this;
    }
    int call_count;
    int id_;
    ITaskLogger &log_;
    static void externAct(void *me) { static_cast<SimpleTask *>(me)->action(); }
};

inline ITaskLogger &operator<<(ITaskLogger &logger, SimpleTask *task) {
    logger.log(task->id_, task->call_count);
    return logger;
}

/**
 * \brief This task appears to take time, by adding some time to the clock
 * before returning.
 */
class LongTask {
  public:
    LongTask(int id, ITaskLogger &log, Duration task_length)
        : task_(id, log), length_(task_length) {}

    void action() {
        task_.action();
        FakeClockWrapper::impl->incrementTime(length_);
    }

    static void externAct(void *me) { static_cast<LongTask *>(me)->action(); }

  private:
    SimpleTask task_;
    Duration length_;
};

/**
 * In hindsight I should've just done this earlier, a single task type with
 * several features. But oh well.
 */
class AdvancedTask {
  private:
    typedef AdvancedTask &AT; // convenience alias
  public:
    AdvancedTask(int id, ITaskLogger &log) : task_(id, log), length_(0) {}

    // Named parameter idiom
    /**
     * \brief Set task length (time)
     */
    AT length(Duration len) {
        length_ = len;
        return *this;
    }

    /**
     * \brief Set other task that this task will unschedule, and at which call
     * count it should be disabled at. Call counts start counting from 1; call
     * counts of zero or below are equivalent to 1.
     */
    AT unscheduleOtherAt(int other_id, int at_call_count) {
        unschedule_others_ = true;
        unschedule_id_ = other_id;
        unschedule_count_ = at_call_count;
        return *this;
    }

    /**
     * \brief Setup task to disable repeating for another task, at a particular
     * call count.
     */
    AT disableRepeatAt(int id, int call_count) {
        dis_rep_ = true;
        dis_rep_id_ = id;
        dis_rep_count_ = call_count;
        return *this;
    }

    void action() {
        task_.action();
        FakeClockWrapper::impl->incrementTime(length_);
        if (unschedule_others_ && task_.call_count >= unschedule_count_) {
            Schedule::unschedule(unschedule_id_);
        }
        if (dis_rep_ && task_.call_count >= dis_rep_count_) {
            Schedule::disableRepeat(dis_rep_id_);
        }
    }

    static void externAct(void *me) {
        static_cast<AdvancedTask *>(me)->action();
    }

  private:
    SimpleTask task_;
    Duration length_;
    bool unschedule_others_ = false;
    int unschedule_id_ = 0;
    int unschedule_count_ = 0;

    bool dis_rep_ = false;
    int dis_rep_id_ = 0;
    int dis_rep_count_ = 0;
};

#endif
