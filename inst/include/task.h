#include <R.h>
#include <Rinternals.h>
#include "later.h"
#include "tinythread.h"

namespace later {

class BackgroundTask {

public:
  BackgroundTask() {}
  ~BackgroundTask() {}
  
  void begin() {
    tthread::thread t(&BackgroundTask::task_main, this);
  }

protected:
  // The task to be executed on the background thread.
  // Neither the R runtime nor any R data structures may be
  // touched from the background thread; any values that need
  // to be passed into or out of the Execute method must be
  // included as fields on the Task subclass object.
  virtual void execute() = 0;
  
  // A short task that runs on the main R thread after the
  // background task has completed. It's safe to access the
  // R runtime and R data structures from here.
  virtual void complete() = 0;

private:
  static void task_main(void* data) {
    BackgroundTask* task = reinterpret_cast<BackgroundTask*>(data);
    // TODO: Error handling
    task->execute();
    execLaterNative(&BackgroundTask::result_callback, task, 0);
  }
  
  static void result_callback(void* data) {
    BackgroundTask* task = reinterpret_cast<BackgroundTask*>(data);
    // TODO: Error handling
    task->complete();
    // TODO: Resolve a promise with this result
  }
};

} // namespace later