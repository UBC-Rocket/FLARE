// push_heap
static int parentIdx_(int n) { return (n - 1) / 2; }
int start = countEnabled();
while (start != 0) {
    int candidate = parent_idx_(start);
    if (todo_[candidate].first > new_task.first) {
        // sift up
        todo_[start] = todo_[candidate];
        start = candidate;
    } else {
        todo_[candidate] = new_task;
        break;
    }
};
