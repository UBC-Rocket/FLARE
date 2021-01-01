#pragma once

#include <algorithm> // for heap algorithms
#include <array>
#include <bitset>
#include <cassert>
#include <utility> // for pair

#include "time_tmp.h"

template <typename Dummy> class ScheduleBase {
  private:
    static constexpr int MAX_TASKS = 32; // adjustable

  public:
    // don't want to deal with std::chrono yet, but also want to reduce further
    // compatibility work
    typedef uint32_t Duration;
    typedef uint32_t Time;
    typedef void (*TaskFunction)(void *);

    struct Task {
        TaskFunction action;
        void *data;
        Time period;

        Task(TaskFunction act, void *dat, Time peri)
            : action(act), data(dat), period(peri) {}
    };
    /**
     * \brief Registers a task.
     *
     * \param id What task ID the task should be given
     * \param task Task to register
     * \param enable True if task should be immediately enabled
     * \param run_early True if task can be run early, if it is first in line
     */
    static void registerTask(int id, Task task, bool enable, bool run_early) {
        tasks_[id] = task;
        if (enable) {
            repeat_.set(id);
        } else {
            repeat_.reset(id);
        }
        if (run_early) {
            run_early_.set(id);
        } else {
            run_early_.reset(id);
        }
    }

    /**
     * \brief Registers a task, with default to run immediately but do not run
     * early. Equivalent to registerTask(id, task, true, false)
     *
     * \param id What task ID the task should be given
     * \param task Task to register
     */
    static void registerTask(int id, Task task) {
        tasks_[id] = task;
        repeat_.set(id);
        run_early_.reset(id);
    }

    /**
     * \brief Runs the scheduler, which will continually run tasks. This
     * function will not return, unless there are no more tasks to run (i.e. all
     * repeating is disabled).
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
        std::pop_heap(todo_.begin(), todo_.begin() + todo_count_);
        todo_count_--;
        // aliases
        const Time &start_time = todo_[todo_count_].first;
        const int &task_id = todo_[todo_count_].second;
        const Task &task = tasks_[task_id];

        if (!run_early_.test(task_id)) {
            while (Hal::millis() < start_time)
                ; // TODO don't busy wait
        }

        // currently_running_ = task_id;
        const Time initial_time = Hal::millis();
        task.action(task.data); // actually run the task

        if (not repeat_.test(task_id)) {
            return; // task was disabled - don't reschedule
        }
        // reschedule task
        // Note that task.action() could have rescheduled tasks, so there's no
        // guarentee on current heap structure
        const Time current_time = Hal::millis();
        const Time new_task_time = initial_time + task.period;
        // Earliest it can be scheduled is now.
        if (new_task_time > current_time) {
            scheduleTask_(TaskTodo(new_task_time, task_id));
        } else {
            scheduleTask_(TaskTodo(current_time, task_id));
        }
    }

    /**
     * \brief Enable task repeating and schedules the task. Undefined behaviour
     * if task is already enabled, or if the task is not registered.
     */
    static void commenceTask(int id) {
        assert(!repeat_.test(id));
        repeat_.set(id);
        scheduleTask_(TaskTodo(tasks_[id].period + Hal::millis(), id));
    }

    /**
     * \brief Schedule a task to run at a certain time. WARNING - this should
     * only be used if the task is not scheduled for some reason, or if it is
     * currently running but will not repeat. The main application for this is
     * to run a non-repeating task, only once - it is very important to only
     * call this function once.
     * \param time What time to schedule the task for.
     * \param id What task ID to run.
     */
    static void scheduleTask(Time time, int id) {
        scheduleTask_(TaskTodo(time, id));
    }

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
     * \brief Disable repeating the currently running task - i.e. a
     * self-disable. Note that the task will continue to run until its end, but
     * simply won't be rescheduled.
     *
     * Note that calling this function is only allowed if id is the currently
     * running task, since it relies on the assumption that the task is not in
     * the waiting queue.
     */
    static void disableRepeatSelf(int id) { repeat_.reset(id); }

    /**
     * \brief Disable repeating for any task. Technically this will even work
     * for invalid task IDs, but doing so is strongly discouraged. A more
     * efficient version for disabling yourself (the current running task) is
     * disableRepeatSelf.
     */
    static void disableRepeat(int id) {
        // Lazy disabling might not work, if task gets re-enabled
        repeat_.reset(id);
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

  private:
    static std::bitset<MAX_TASKS> run_early_;
    static std::bitset<MAX_TASKS> repeat_; // If set, task gets repeated.
    static std::array<Task, MAX_TASKS> tasks_;

    typedef std::pair<Time, int> TaskTodo;        // <time, id>
    static std::array<TaskTodo, MAX_TASKS> todo_; // min-heap (priority queue)
    static int todo_count_; // Not quite repeat_.count(), during task scheduling

    // static int currently_running_; // task

    static bool heapCompare_(const TaskTodo &a, const TaskTodo &b) {
        // less-than compare, for use in std heap algorithms which use max-heap
        return a.first > b.first;
    };

    static void scheduleTask_(TaskTodo new_task) {
        todo_[todo_count_] = new_task;
        std::push_heap(todo_.begin(), todo_.begin() + todo_count_,
                       heapCompare_);
        todo_count_++;
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

template <class Dummy> int ScheduleBase<Dummy>::todo_count_;

template <class Dummy>
std::bitset<ScheduleBase<Dummy>::MAX_TASKS> ScheduleBase<Dummy>::repeat_;

template <class Dummy>
std::bitset<ScheduleBase<Dummy>::MAX_TASKS> ScheduleBase<Dummy>::run_early_;

template <typename Dummy>
std::array<typename ScheduleBase<Dummy>::TaskTodo,
           ScheduleBase<Dummy>::MAX_TASKS>
    ScheduleBase<Dummy>::todo_;

class Schedule : public ScheduleBase<void> {};
