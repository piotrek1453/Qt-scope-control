#pragma once

#include "CommandParser.hpp"
#include "InstrumentControl.hpp"
#include "oscilloscope_utils.h"
#include <QApplication>
#include <QFileDialog>
#include <QMainWindow>
#include <QTextEdit>
#include <QTextStream>
#include <memory>
#include <mutex>
#include <regex>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  void setupLogging(QTextEdit *textEdit);
  void scopeSetup(ViChar scope_string[]);

private slots:
  void on_AutoscalePushbutton_clicked();

  void on_DisconnectPushButton_clicked();

  void on_ConnectPushButton_clicked();

  void on_FrequencyPushbutton_clicked();

  void on_VrmsPushbutton_clicked();

  void on_ChannelSpinbox_valueChanged();

  void on_AcqModePushbutton_clicked();

  void on_ViClearPushButton_clicked();

  void on_ChannelVisibilityEnablePushButton_clicked();

  void on_ChannelVisibilityDisablePushButton_clicked();

  void on_VScaleDial_valueChanged(int value);

  void on_VOffsetDial_valueChanged(int value);

  void on_HScaleDial_valueChanged(int value);

  void on_HOffsetDial_valueChanged(int value);

private:
  Ui::MainWindow *ui;
  QString commands_filename;
  InstrumentControl::InstrumentControl scope;
  CommandParser::CommandParser commands_tree;
};
// MAINWINDOW_H
