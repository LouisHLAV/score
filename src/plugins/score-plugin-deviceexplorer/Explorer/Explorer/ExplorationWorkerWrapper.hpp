#pragma once
#include "ExplorationWorker.hpp"

#include <Device/Protocol/DeviceInterface.hpp>

#include <Explorer/Explorer/DeviceExplorerWidget.hpp>

#include <score/widgets/MessageBox.hpp>

#include <QApplication>
#include <QThread>

namespace Explorer
{
class DeviceExplorerWidget;

/**
 * Utility class to get a node from the DeviceExplorerWidget.
 */
template <typename OnSuccess>
class ExplorationWorkerWrapper final : public QObject
{
  QThread* thread = new QThread;
  ExplorationWorker* worker{};
  DeviceExplorerWidget& m_widget;

  OnSuccess m_success;

public:
  template <typename OnSuccess_t>
  ExplorationWorkerWrapper(
      OnSuccess_t&& success, DeviceExplorerWidget& widg, Device::DeviceInterface& dev)
      : worker{new ExplorationWorker{dev}}
      , m_widget{widg}
      , m_success{std::move(success)}
  {
    QObject::connect(
        thread, &QThread::started, worker,
        [&]() { on_start(); }, // so that it runs on thread.
        Qt::QueuedConnection);

    QObject::connect(
        worker, &ExplorationWorker::finished, this, &ExplorationWorkerWrapper::on_finish,
        Qt::QueuedConnection);

    QObject::connect(
        worker, &ExplorationWorker::failed, this, &ExplorationWorkerWrapper::on_fail,
        Qt::QueuedConnection);
  }

  void start()
  {
    m_widget.blockGUI(true);
    worker->moveToThread(thread);
    thread->start();
  }

  ~ExplorationWorkerWrapper()
  {
    thread->quit();
    thread->wait();
    delete thread;
  }

private:
  void on_start()
  {
    try
    {
      worker->node = worker->dev.refresh();
      worker->finished();
    }
    catch(std::runtime_error& e)
    {
      worker->failed(e.what());
    }
  }

  void on_finish()
  {
    m_widget.blockGUI(false);
    m_success(std::move(worker->node));

    cleanup();
  }

  void on_fail(const QString& str)
  {
    score::warning(
        QApplication::activeWindow(), QObject::tr("Unable to refresh the device"),
        QObject::tr("Unable to refresh the device: ") + worker->dev.settings().name
            + QObject::tr(".\nCause: ") + str);

    m_widget.blockGUI(false);
    cleanup();
  }

  void cleanup()
  {
    thread->quit();
    worker->deleteLater();
    this->deleteLater();
  }
};

template <typename OnSuccess_t>
static auto make_worker(
    OnSuccess_t&& success, DeviceExplorerWidget& widg, Device::DeviceInterface& dev)
{
  return new ExplorationWorkerWrapper<OnSuccess_t>{std::move(success), widg, dev};
}
}
