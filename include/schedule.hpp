#ifndef SCHEDULE_HPP_60517DD319DD4B338F58D1F4669BBD6A
#define SCHEDULE_HPP_60517DD319DD4B338F58D1F4669BBD6A

#include <algorithm> // for heap algorithms
#include <array>
#include <bitset>
#include <cassert>
#include <utility> // for pair

// TODO: Make a utility template that takes a class and provides a static method
// that can be used as a TaskFunction

/**
 * \tparam Clock satisfies a looser form of the C++ STL named requirement for
 * Clock - specifically, there's no requirement on the types used internally; it
 * just needs to have a ::duration type, ::time_point type, and a ::now()
 * function.
 *
 * Additionally, if Clock::idle(Duration wait_time) exists, then this function
 * will be called when waiting for the next task to be ready. If Clock::idle()
 * does not exist, then the code will simply busy wait.
 *
 * \tparam MaxTasks is the maximum number of tasks that can be registered. Note
 * that each task can only be on the schedule once, so it's also the number of
 * tasks that can be scheduled.
 *
 * Call clear(), registerTask(), then run().
 */
template <typename Clock, int MaxTasks> class ScheduleBase {
  public:
    typedef typename Clock::duration Duration;
    typedef typename Clock::time_point TimePoint;
    typedef void (*TaskFunction)(void *);

    struct Task {
        TaskFunction action;
        void *data;
        Duration period;

        Task(TaskFunction act, void *dat, Duration peri)
            : action(act), data(dat), period(peri) {}
        Task() : Task(nullptr, nullptr, Duration(0)) {}
    };

  public:
    /**
     * \brief Preregisters a task, but does not schedule it.
     *
     * \param id What task ID the task should be given
     * \param task Task to register
     * \param repeat True if task should be immediately enabled
     * \param run_early True if task can be run early, if it is first in line
     */
    static void preregisterTask(int id, Task task, bool repeat = true,
                                bool run_early = false) {
        tasks_[id] = task;
        repeat_.set(id, repeat);
        run_early_.set(id, run_early);
    }

    // TODO
    // Make it possible to register task by directly passing in
    // parameters, rather than needing to construct a Task

    /**
     * \brief Registers a task (see preregisterTask) but also schedules it to
     * run immediately. Same parameters as preregisterTask.
     */
    static void registerTask(int id, Task task, bool repeat = true,
                             bool run_early = false) {
        preregisterTask(id, task, repeat, run_early);
        scheduleTaskNow(id);
    }

    /**
     * \brief Runs the scheduler, which will continually run tasks, i.e.
     * constantly calls runNext(). This function will not return, unless there
     * are no more tasks to run (i.e. all repeating is disabled).
     *
     * If it's really important for you to exit, you could technically raise an
     * exception - but that would most likely invalidate class invariants, so
     * then call clear() to reset the scheduler.
     */
    static void run() {
        while (todo_count_ > 0) {
            runNext();
        }
    }
    /**
     * \brief Runs the scheduler once - take next task from priority queue, and
     * run it, possibly rescheduling it if repeating was enabled. Undefined
     * behaviour if there are no more remaining tasks.
     */
    static void runNext() {
        std::pop_heap(todo_.begin(), todo_.begin() + todo_count_, heapCompare_);
        todo_count_--;

        TimePoint start_time = todo_[todo_count_].first;
        // aliases
        const int &task_id = todo_[todo_count_].second;
        const Task &task = tasks_[task_id];

        if (!run_early_.test(task_id)) {
            while (start_time > Clock::now())
                IdleIfAvailable<Clock>::idle(start_time - Clock::now());
        } else if (Clock::now() < start_time) {
            start_time = Clock::now();
        }

        task.action(task.data); // actually run the task

        if (not repeat_.test(task_id)) {
            return; // task does not repeat - don't reschedule
        }
        // reschedule task, assuming ideally that task started at start_time
        // Note that task.action() could have rescheduled tasks, so there's no
        // guarentee on current heap structure
        const TimePoint current_time = Clock::now();
        const TimePoint new_task_time = start_time + task.period;
        // Earliest it can be scheduled is now.
        if (new_task_time > current_time) {
            scheduleTask_(TaskTodo(new_task_time, task_id));
        } else {
            scheduleTask_(TaskTodo(current_time, task_id));
        }
    }

    /**
     * \brief Enable task repeating and schedules the task. Undefined behaviour
     * if task is already scheduled, or if the task is not registered.
     */
    static void commenceTask(int id) {
        repeat_.set(id);
        scheduleTask_(TaskTodo(tasks_[id].period + Clock::now(), id));
    }

    /**
     * \brief Schedule a task to run at a certain time. WARNING - this should
     * only be used if the task is not scheduled yet, or if it is
     * currently running but will not repeat. It is very important to only
     * call this function once. If for whatever reason you want to schedule the
     * same task twice, duplicate the task in another task slot.
     *
     * \param time * What time to schedule the task for.
     * \param id What task ID to run.
     */
    static void scheduleTask(TimePoint time, int id) {
        scheduleTask_(TaskTodo(time, id));
    }

    /**
     * \brief Schedule the task to run now; see scheduleTask.
     */
    static void scheduleTaskNow(int id) { scheduleTask(Clock::now(), id); }

    /**
     * \brief Check if task repeats.
     */
    static bool checkRepeat(int id) { return repeat_.test(id); }

    /**
     * \brief Set task to repeat (i.e. after it finishes executing, it gets
     * rescheduled).
     */
    static void enableRepeat(int id) { repeat_.set(id); }

    /**
     * \brief Disable repeating for any task. Note that if the task is still
     * scheduled, it will run one last time - if this is unacceptable, call
     * unschedule().
     */
    static void disableRepeat(int id) { repeat_.reset(id); }

    /**
     * \brief Unschedule any task. If the task specified is not scheduled or is
     * currently running, nothing happens. This means that if id is the
     * currently running task, and id is set to repeat, the task will be
     * rescheduled. It is generally preferable to disable repeating instead of
     * unscheduling, since this method must scan through the entire scheduled
     * queue to find the task.
     */
    static void unschedule(int id) {
        int i = get_todo_index_(id);
        if (i >= todo_count_) {
            return; // not found
        }
        do { // sift down, eventually remove
            const int child1 = heap_child_1_(i);
            const int child2 = child1 + 1;
            if (todo_[child1].first < todo_[child2].first) { // Use child 1
                todo_[i] = todo_[child1];
                i = child1;
            } else {
                todo_[i] = todo_[child2];
                i = child2;
            }
        } while (i < todo_count_);
        todo_count_--;
    }

    /**
     * Check if task can be run early.
     */
    static bool checkRunEarly(int id) { return run_early_.test(id); }

    /**
     * \brief Allow task to run early
     */
    static void enableRunEarly(int id) { run_early_.set(id); }

    /**
     * \brief Disallow task to run early
     */
    static void disableRunEarly(int id) { run_early_.reset(id); }

    /**
     * \brief Returns how many tasks are in the queue.
     */
    static int countTodo() { return todo_count_; }

    /**
     * \brief Clears all data, effectively reinitializing it to the starting
     * clean slate.
     */
    static void clear() {
        todo_count_ = 0; // Implicitly clears run queue
        run_early_.reset();
        repeat_.reset();
        // No need to clear out old tasks - they're left unininitialized
    }

  private:
    static constexpr int MAX_TASKS = MaxTasks; // adjustable

    static std::bitset<MAX_TASKS> run_early_;  // If set, task can run early
    static std::bitset<MAX_TASKS> repeat_;     // If set, task gets repeated
    static std::array<Task, MAX_TASKS> tasks_; // Where tasks are stored

    typedef std::pair<TimePoint, int> TaskTodo;   // <time, id>
    static std::array<TaskTodo, MAX_TASKS> todo_; // min-heap (priority queue)
    static int todo_count_; // Not quite repeat_.count(), during task scheduling

    /**
     * SFINAE check for Clock::idle()
     * IdleIfAvailable::idle() will call Clock::idle() if it exists, otherwise
     * it returns immediately.
     */
    template <class T, class = void> class IdleIfAvailable {
      public:
        static void idle(Duration wait_time) {}
    };

    template <class T>
    class IdleIfAvailable<T, decltype(T::idle(Clock::now() - Clock::now()),
                                      void())> {
      public:
        static void idle(Duration wait_time) { Clock::idle(wait_time); }
    };

    // static int currently_running_; // task

    static bool heapCompare_(const TaskTodo &a, const TaskTodo &b) {
        // less-than compare, for use in std heap algorithms which use max-heap
        return a.first > b.first;
    };

    static void scheduleTask_(TaskTodo new_task) {
        todo_[todo_count_] = new_task;
        todo_count_++;
        std::push_heap(todo_.begin(), todo_.begin() + todo_count_,
                       heapCompare_);
    }

    /**
     * \brief Returns the index of a task in the todo_ heap.
     * If not found, todo_count_ is returned.
     */
    static int get_todo_index_(int id) {
        int i = 0;
        for (; i < todo_count_; i++) {
            if (todo_[i].second == id) {
                break;
            }
        }
        return i;
    }

    /**
     * \brief Helper function - returns the index of the 1st child of a heap.
     * 2nd child is at one higher index.
     */
    static int heap_child_1_(int index) { return 2 * index + 1; }
};

template <typename Clock, int MaxTasks>
int ScheduleBase<Clock, MaxTasks>::todo_count_;

template <typename Clock, int MaxTasks>
std::bitset<ScheduleBase<Clock, MaxTasks>::MAX_TASKS>
    ScheduleBase<Clock, MaxTasks>::repeat_;

template <typename Clock, int MaxTasks>
std::bitset<ScheduleBase<Clock, MaxTasks>::MAX_TASKS>
    ScheduleBase<Clock, MaxTasks>::run_early_;

template <typename Clock, int MaxTasks>
std::array<typename ScheduleBase<Clock, MaxTasks>::TaskTodo,
           ScheduleBase<Clock, MaxTasks>::MAX_TASKS>
    ScheduleBase<Clock, MaxTasks>::todo_;

template <typename Clock, int MaxTasks>
std::array<typename ScheduleBase<Clock, MaxTasks>::Task,
           ScheduleBase<Clock, MaxTasks>::MAX_TASKS>
    ScheduleBase<Clock, MaxTasks>::tasks_;

#endif
