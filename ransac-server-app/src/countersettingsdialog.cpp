#include "countersettingsdialog.h"
#include "ui_countersettingsdialog.h"

CounterSettingsDialog::CounterSettingsDialog(QWidget* parent,
                                             ransac::Settings* settings,
                                             size_t total_points_num)
    : QDialog(parent)
    , ui_(new Ui::CounterSettingsDialog)
    , settings_(settings)
{
    ui_->setupUi(this);

    // Подгружаем данные из settings_
    UploadFromSettings(total_points_num);

    // Обновляем показания и доступность значений
    slotValueChanged(0);
    slotUpdateAvaliability(0);

    ConnectSlotsAndSignals();
}

CounterSettingsDialog::~CounterSettingsDialog() {
    delete ui_;
}

void CounterSettingsDialog::slotValueChanged(int) {
    settings_->inliers_size = ui_->inliers_size_sldr->value();
    ui_->inliers_size_lbl->setText(QString::number(settings_->inliers_size));

    settings_->iterations_num = ui_->iterations_num_sldr->value();
    ui_->iterations_num_lbl->setText(QString::number(settings_->iterations_num));

    settings_->max_y_diff = static_cast<double>(ui_->max_diff_sldr->value());
    ui_->max_diff_lbl->setText(QString::number(settings_->max_y_diff));

    settings_->is_async_required = ui_->async_chbox->isChecked();
}

void CounterSettingsDialog::slotUpdateAvaliability(int) {
    if (ui_->inliers_size_chbox->isChecked()) {
        ui_->inliers_size_lbl->setEnabled(false);
        ui_->inliers_size_sldr->setEnabled(false);
        settings_->auto_inliers_size = true;
    }
    else {
        ui_->inliers_size_lbl->setEnabled(true);
        ui_->inliers_size_sldr->setEnabled(true);
        settings_->auto_inliers_size = false;
    }

    if (ui_->max_diff_chbox->isChecked()) {
        ui_->max_diff_lbl->setEnabled(false);
        ui_->max_diff_sldr->setEnabled(false);
        settings_->auto_max_y_diff = true;
    }
    else {
        ui_->max_diff_lbl->setEnabled(true);
        ui_->max_diff_sldr->setEnabled(true);
        settings_->auto_max_y_diff = false;
    }
}

void CounterSettingsDialog::ConnectSlotsAndSignals() {
    // Изменения значений слайдеров
    connect(ui_->inliers_size_sldr, SIGNAL(valueChanged(int)),
            SLOT(slotValueChanged(int)));
    connect(ui_->iterations_num_sldr, SIGNAL(valueChanged(int)),
            SLOT(slotValueChanged(int)));
    connect(ui_->max_diff_sldr, SIGNAL(valueChanged(int)),
            SLOT(slotValueChanged(int)));
    connect(ui_->async_chbox, SIGNAL(stateChanged(int)),
            SLOT(slotValueChanged(int)));

    // Установка флажка "Автоматически"
    connect(ui_->inliers_size_chbox, SIGNAL(stateChanged(int)),
            SLOT(slotUpdateAvaliability(int)));
    connect(ui_->max_diff_chbox, SIGNAL(stateChanged(int)),
            SLOT(slotUpdateAvaliability(int)));
}

void CounterSettingsDialog::UploadFromSettings(size_t total_points_num) {
    // Задаем максимальное значения для начального числа входящих точек
    ui_->inliers_size_sldr->setMaximum(
                std::max(total_points_num / 3, static_cast<size_t>(3)));

    // Задаем значения из переданных настроек
    ui_->inliers_size_sldr->setValue(settings_->inliers_size);
    ui_->iterations_num_sldr->setValue(settings_->iterations_num);
    ui_->max_diff_sldr->setValue(settings_->max_y_diff);

    // Задаем статусы флажков
    ui_->inliers_size_chbox->setChecked(settings_->auto_inliers_size);
    ui_->max_diff_chbox->setChecked(settings_->auto_max_y_diff);
    ui_->async_chbox->setChecked(settings_->is_async_required);
}

void CounterSettingsDialog::accept() {
    this->done(this->Accepted);
}

