#include "execution_context.h"
#include "showbox.h"
#include <QCoreApplication>
#include <QMetaObject>
#include <QThread>

ExecutionContext::ExecutionContext(ShowBox *dialogBox)
    : m_dialogBox(dialogBox) {}

ShowBox *ExecutionContext::dialogBox() const { return m_dialogBox; }

void ExecutionContext::executeOnGui(std::function<void()> func) {
  if (!m_dialogBox)
    return;

  // If we are already on the thread the dialog lives in, call directly
  if (QThread::currentThread() == m_dialogBox->thread()) {
    func();
  } else {
    // Use blocking queued connection
    QMetaObject::invokeMethod(
        m_dialogBox, [func]() { func(); }, Qt::BlockingQueuedConnection);
  }
}