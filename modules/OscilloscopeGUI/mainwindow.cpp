#include "mainwindow.h"
#include "./ui_mainwindow.h"

// Custom sink to log to QTextEdit
template <typename Mutex>
class QTextEditSink : public spdlog::sinks::base_sink<Mutex> {
public:
  explicit QTextEditSink(QTextEdit *textEdit) : m_textEdit(textEdit) {}

protected:
  void sink_it_(const spdlog::details::log_msg &msg) override {
    // Format the log message
    spdlog::memory_buf_t formatted;
    this->formatter_->format(msg, formatted);

    // Append the message to the QTextEdit in the GUI thread
    QString logMessage = QString::fromStdString(fmt::to_string(formatted));
    QMetaObject::invokeMethod(m_textEdit, [this, logMessage]() {
      m_textEdit->append(logMessage);
    });
  }

  void flush_() override {}

private:
  QTextEdit *m_textEdit;
};

using QTextEditSink_mt = QTextEditSink<std::mutex>;

// Usage in MainWindow
void MainWindow::setupLogging(QTextEdit *textEdit) {
  auto textEditSink = std::make_shared<QTextEditSink_mt>(textEdit);
  auto logger = std::make_shared<spdlog::logger>("gui_logger", textEditSink);

  // Set the global logger or use it directly
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  setupLogging(ui->logTextEdit);
  // Open a file dialog to select a .yml file
  commands_filename = QFileDialog::getOpenFileName(
      nullptr,
      "Wybierz plik z komendami SCPI oscyloskopu", // Window title
      QDir::currentPath(),        // Start in the user's home directory
      "YAML Files (*.yml *.yaml)" // File filter to restrict to YAML files
  );

  if (!commands_filename.isEmpty()) {
    spdlog::info("User selected file: {}", commands_filename.toStdString());
  } else {
    spdlog::warn("No file selected.");
  }
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::on_AutoscalePushbutton_clicked() {
  auto command = commands_tree.GetCommandTree()["utils"]["autoscale"].val();
  if (scope.Write((ViChar *)std::string(command.data(), command.len).c_str())) {
    spdlog::info("Autoscale set!");
  } else {
    spdlog::error("Error setting autoscale");
  }
}

void MainWindow::scopeSetup(ViChar scope_string[]) {
  scope.Connect(scope_string);
  commands_tree.ReadYaml(commands_filename.toUtf8().constData());
}

void MainWindow::on_DisconnectPushButton_clicked() {
  scope.Disconnect();
  ui->ConnectPushButton->setEnabled(true);
}

void MainWindow::on_ConnectPushButton_clicked() {
  scopeSetup((ViChar *)qPrintable(ui->InstrumentStringTextEdit->toPlainText()));
  ui->ConnectPushButton->setEnabled(false);
}

void MainWindow::on_FrequencyPushbutton_clicked() {
  ViChar *result;
  std::tuple<double, int> result_to_display;
  auto set_meas_type_command =
      commands_tree.GetCommandTree()["measurements"]["frequency"].val();
  auto get_meas_result_command =
      commands_tree.GetCommandTree()["measurements"]["get_result"].val();
  std::string command_to_write;
  if (get_meas_result_command.len > 0) {
    command_to_write =
        std::string(set_meas_type_command.data(), set_meas_type_command.len) +
        ";" +
        std::string(get_meas_result_command.data(),
                    get_meas_result_command.len) +
        '?';
  } else {
    command_to_write =
        std::string(set_meas_type_command.data(), set_meas_type_command.len) +
        '?';
  }

  result = std::get<ViChar *>(scope.Query((ViChar *)command_to_write.c_str()));

  try {
    result_to_display = oscilloscope_utils::convertMeasurementResult(
        oscilloscope_utils::viCharArrToString(result));
  } catch (const std::exception &e) {
    spdlog::error("Exception converting scientific notation:\n{}", e.what());
    return;
  } catch (...) {
    spdlog::error("Unknown error occurred converting scientific notation");
    return;
  }

  ui->FrequencyLCD->display(std::get<0>(result_to_display));
  std::string exponent =
      oscilloscope_utils::convertExponentToSI(std::get<1>(result_to_display)) +
      "Hz";
  ui->FrequencyResultLabel->setText(QString::fromStdString(exponent));
}

void MainWindow::on_VrmsPushbutton_clicked() {
  ViChar *result;
  std::tuple<double, int> result_to_display;
  auto set_meas_type_command =
      commands_tree.GetCommandTree()["measurements"]["voltage_rms"].val();
  auto get_meas_result_command =
      commands_tree.GetCommandTree()["measurements"]["get_result"].val();
  std::string command_to_write;
  if (get_meas_result_command.len > 0) {
    command_to_write =
        std::string(set_meas_type_command.data(), set_meas_type_command.len) +
        ";" +
        std::string(get_meas_result_command.data(),
                    get_meas_result_command.len) +
        '?';
  } else {
    command_to_write =
        std::string(set_meas_type_command.data(), set_meas_type_command.len) +
        '?';
  }

  result = std::get<ViChar *>(scope.Query((ViChar *)command_to_write.c_str()));

  try {
    result_to_display = oscilloscope_utils::convertMeasurementResult(
        oscilloscope_utils::viCharArrToString(result));
  } catch (const std::exception &e) {
    spdlog::error("Exception converting scientific notation:\n{}", e.what());
    return;
  } catch (...) {
    spdlog::error("Unknown error occurred converting scientific notation");
    return;
  }

  ui->VrmsLCD->display(std::get<0>(result_to_display));
  std::string exponent =
      oscilloscope_utils::convertExponentToSI(std::get<1>(result_to_display)) +
      "V";
  ui->VrmsResultLabel->setText(QString::fromStdString(exponent));
}

void MainWindow::on_ChannelSpinbox_valueChanged() {
  auto command =
      commands_tree.GetCommandTree()["measurements"]["source_channel"].val();
  const int channel = ui->ChannelSpinbox->value();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string channel_to_write = std::to_string(channel);

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);

  scope.Write((ViChar *)command_to_write.c_str());
}

void MainWindow::on_AcqModePushbutton_clicked() {
  // read commands from file
  auto acq_count =
      commands_tree.GetCommandTree()["acquisition"]["acq_count"].val();
  auto command = commands_tree.GetCommandTree()["acquisition"]["mode"].val();
  auto acq_mode = commands_tree
                      .GetCommandTree()["acquisition"]["types"]
                                       [ui->AcqModeComboBox->currentIndex()]
                      .val();

  // convert to std::string
  std::string acq_count_to_write = std::string(acq_count.data(), acq_count.len);
  std::string command_to_write = std::string(command.data(), command.len);
  std::string acq_mode_to_write = std::string(acq_mode.data(), acq_mode.len);

  // replace placeholder with actual value
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{acquire_mode\\}"),
                                        acq_mode_to_write);
  acq_count_to_write =
      std::regex_replace(acq_count_to_write,
                         std::regex("\\{count\\}"),
                         std::to_string(ui->AcqCountSpinBox->value()));

  // convert std::string to ViChar* for VISA to handle and write
  scope.Write((ViChar *)acq_count_to_write.c_str());
  scope.Write((ViChar *)command_to_write.c_str());
}

void MainWindow::on_ViClearPushButton_clicked() {
  scope.ViClear();
}

void MainWindow::on_ChannelVisibilityEnablePushButton_clicked() {
  auto command =
      commands_tree.GetCommandTree()["channels"]["display_state"].val();
  auto state_disable =
      commands_tree.GetCommandTree()["channels"]["states"]["on"].val();
  const int channel = ui->ChannelSpinbox->value();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string state_to_write =
      std::string(state_disable.data(), state_disable.len);
  std::string channel_to_write = std::to_string(channel);

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{display_state\\}"),
                                        state_to_write);

  scope.Write((ViChar *)command_to_write.c_str());
}

void MainWindow::on_ChannelVisibilityDisablePushButton_clicked() {
  auto command =
      commands_tree.GetCommandTree()["channels"]["display_state"].val();
  auto state_disable =
      commands_tree.GetCommandTree()["channels"]["states"]["off"].val();
  const int channel = ui->ChannelSpinbox->value();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string state_to_write =
      std::string(state_disable.data(), state_disable.len);
  std::string channel_to_write = std::to_string(channel);

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{display_state\\}"),
                                        state_to_write);

  scope.Write((ViChar *)command_to_write.c_str());
}

void MainWindow::on_VScaleDial_valueChanged(int value) {
  auto command =
      commands_tree.GetCommandTree()["channels"]["scale"]["vertical"].val();
  const int channel = ui->ChannelSpinbox->value();
  std::string exponent = ui->VScaleComboBox->currentText().toStdString();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string channel_to_write = std::to_string(channel);
  std::string exponent_to_write =
      std::to_string(oscilloscope_utils::convertSIToExponent(exponent));
  std::string scale_to_write = std::to_string(value) + "E" + exponent_to_write;

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{scale_value\\}"),
                                        scale_to_write);

  scope.Write((ViChar *)command_to_write.c_str());

  if (exponent_to_write == "0") {
    ui->VScaleDial->setMaximum(100);
    ui->VScaleDial->setSingleStep(1);
    ui->VScaleDial->setPageStep(1);
  } else {
    ui->VScaleDial->setMaximum(1001);
    ui->VScaleDial->setSingleStep(10);
    ui->VScaleDial->setPageStep(10);
  }

  ui->VScaleLCD->display(value);
}

void MainWindow::on_VOffsetDial_valueChanged(int value) {
  auto command =
      commands_tree.GetCommandTree()["channels"]["offset"]["vertical"].val();
  const int channel = ui->ChannelSpinbox->value();
  std::string exponent = ui->VOffsetComboBox->currentText().toStdString();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string channel_to_write = std::to_string(channel);
  std::string exponent_to_write =
      std::to_string(oscilloscope_utils::convertSIToExponent(exponent));
  std::string offset_to_write = std::to_string(value) + "E" + exponent_to_write;

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{offset_value\\}"),
                                        offset_to_write);

  scope.Write((ViChar *)command_to_write.c_str());

  if (exponent_to_write == "0") {
    ui->VOffsetDial->setMaximum(100);
    ui->VOffsetDial->setMinimum(-100);
    ui->VOffsetDial->setSingleStep(1);
    ui->VOffsetDial->setPageStep(1);
  } else {
    ui->VOffsetDial->setMaximum(1001);
    ui->VOffsetDial->setMinimum(-1001);
    ui->VOffsetDial->setSingleStep(10);
    ui->VOffsetDial->setPageStep(10);
  }

  ui->VOffsetLCD->display(value);
}

void MainWindow::on_HScaleDial_valueChanged(int value) {
  auto command =
      commands_tree.GetCommandTree()["channels"]["scale"]["horizontal"].val();
  const int channel = ui->ChannelSpinbox->value();
  std::string exponent = ui->HScaleComboBox->currentText().toStdString();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string channel_to_write = std::to_string(channel);
  std::string exponent_to_write =
      std::to_string(oscilloscope_utils::convertSIToExponent(exponent));
  std::string scale_to_write = std::to_string(value) + "E" + exponent_to_write;

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{scale_value\\}"),
                                        scale_to_write);

  scope.Write((ViChar *)command_to_write.c_str());

  if (exponent_to_write == "0") {
    ui->HScaleDial->setMaximum(10);
    ui->HScaleDial->setSingleStep(1);
    ui->HScaleDial->setPageStep(1);
  } else {
    ui->HScaleDial->setMaximum(1001);
    ui->HScaleDial->setSingleStep(10);
    ui->HScaleDial->setPageStep(10);
  }

  ui->HScaleLCD->display(value);
}

void MainWindow::on_HOffsetDial_valueChanged(int value) {
  auto command =
      commands_tree.GetCommandTree()["channels"]["offset"]["horizontal"].val();
  const int channel = ui->ChannelSpinbox->value();
  std::string exponent = ui->HOffsetComboBox->currentText().toStdString();

  std::string command_to_write = std::string(command.data(), command.len);
  std::string channel_to_write = std::to_string(channel);
  std::string exponent_to_write =
      std::to_string(oscilloscope_utils::convertSIToExponent(exponent));
  std::string offset_to_write = std::to_string(value) + "E" + exponent_to_write;

  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{channel_number\\}"),
                                        channel_to_write);
  command_to_write = std::regex_replace(command_to_write,
                                        std::regex("\\{offset_value\\}"),
                                        offset_to_write);

  scope.Write((ViChar *)command_to_write.c_str());

  if (exponent_to_write == "0") {
    ui->HOffsetDial->setMaximum(100);
    ui->HOffsetDial->setMinimum(-100);
    ui->HOffsetDial->setSingleStep(1);
    ui->HOffsetDial->setPageStep(1);
  } else {
    ui->HOffsetDial->setMaximum(1001);
    ui->HOffsetDial->setMinimum(-1001);
    ui->HOffsetDial->setSingleStep(10);
    ui->HOffsetDial->setPageStep(10);
  }

  ui->HOffsetLCD->display(value);
}
