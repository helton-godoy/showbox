#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <functional>

class ShowBox;

class ExecutionContext {
public:
  explicit ExecutionContext(ShowBox *dialogBox);
  ShowBox *dialogBox() const;

  // Execute function on the GUI thread (blocking)
  void executeOnGui(std::function<void()> func);

private:
  ShowBox *m_dialogBox;
};

#endif // EXECUTION_CONTEXT_H