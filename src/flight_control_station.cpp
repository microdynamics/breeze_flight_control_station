/***********************************************************************
 *  Software License Agreement (BSD License)
 *
 *  Copyright (c) 2016, Team MicroDynamics
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of the Team MicroDynamics nor the names
 *    of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ***********************************************************************

 ***********************************************************************
 *  History:
 *  <Authors>        <Date>        <Operation>
 *  myyerrol         2016.7.12     Finish the basic gui
 *
 *  Description:
 *  This .cpp file implements the gui of flight control station.
 **********************************************************************/

#include <QDebug>
#include <QKeyEvent>
#include <flight_control_station.h>
#include <ui_flight_control_station.h>

FlightControlStation::FlightControlStation(QWidget *parent,
                                           std::string serial_url,
                                           std::string config_addr) :
    QMainWindow(parent),
    ui(new Ui::FlightControlStation),
    serial_interface_(serial_url, config_addr)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Flight Control Station V1"));

    this->setFixedSize(width(), height());
    this->setFocus();

    timer_ = new QTimer(this);
    timer_->start(1000);

    flight_attitude_indicator_ = new FlightAttitudeIndicator(this);
    flight_altitude_indicator_ = new FlightAltitudeIndicator(this);
    flight_compass_indicator_  = new FlightCompassIndicator(this);

    ui->verticalLayoutAttitude->addWidget(flight_attitude_indicator_);
    ui->verticalLayoutAltitude->addWidget(flight_altitude_indicator_);
    ui->verticalLayoutCompass->addWidget(flight_compass_indicator_);

    connect(timer_, SIGNAL(timeout()), this, SLOT(updateTimerOperation()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->actionAbout, SIGNAL(triggered()), this,
            SLOT(openAboutWidget()));
}

FlightControlStation::~FlightControlStation()
{
    delete ui;
}

void FlightControlStation::openAboutWidget(void)
{
    about_widget_ = new QMessageBox(this);
    about_widget_->setWindowTitle(tr("About"));
    about_widget_->setText(
        QString("Breeze quadcopter flight control station can implement ") +
        QString("the real-time monitoring and control on the computer ")   +
        QString("client.\n\n\t\t\t                Team MicroDynamics"));
    about_widget_->setStandardButtons(QMessageBox::Yes);
    about_widget_->setDefaultButton(QMessageBox::Yes);
    about_widget_->exec();
}

void FlightControlStation::updateTimerOperation(void)
{

}

void FlightControlStation::keyPressEvent(QKeyEvent *event)
{
    double value;

    switch (event->key()) {
        case Qt::Key_W: {
            value = flight_attitude_indicator_->getPitch();
            flight_attitude_indicator_->setPitch(value + VALUE_STEP_ATTITUDE);
            break;
        }
        case Qt::Key_S: {
            value = flight_attitude_indicator_->getPitch();
            flight_attitude_indicator_->setPitch(value - VALUE_STEP_ATTITUDE);
            break;
        }
        case Qt::Key_A: {
            value = flight_attitude_indicator_->getRoll();
            flight_attitude_indicator_->setRoll(value - VALUE_STEP_ATTITUDE);
            break;
        }
        case Qt::Key_D: {
            value = flight_attitude_indicator_->getRoll();
            flight_attitude_indicator_->setRoll(value + VALUE_STEP_ATTITUDE);
            break;
        }
        case Qt::Key_Q: {
            value = flight_compass_indicator_->getYaw();
            flight_compass_indicator_->setYaw(value + VALUE_STEP_COMPASS);
            break;
        }
        case Qt::Key_E: {
            value = flight_compass_indicator_->getYaw();
            flight_compass_indicator_->setYaw(value - VALUE_STEP_COMPASS);
            break;
        }
        case Qt::Key_U: {
            value = flight_altitude_indicator_->getAltitude();
            flight_altitude_indicator_->setAltitude(
                value + VALUE_STEP_ALTITUDE);
            break;
        }
        case Qt::Key_I: {
            value = flight_altitude_indicator_->getAltitude();
            flight_altitude_indicator_->setAltitude(
                value - VALUE_STEP_ALTITUDE);
            break;
        }
        case Qt::Key_J: {
            value = flight_altitude_indicator_->getHeight();
            flight_altitude_indicator_->setHeight(value + VALUE_STEP_HEIGHT);
            break;
        }
        case Qt::Key_K: {
            value = flight_altitude_indicator_->getHeight();
            flight_altitude_indicator_->setHeight(value - VALUE_STEP_HEIGHT);
            break;
        }
        default: {
            break;
        }
    }
}

void FlightControlStation::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
}

void FlightControlStation::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
}

void FlightControlStation::updateBufferRead(void)
{
    acc_x_ = serial_interface_.getDataType()->robot_imu_actual_.acc.acc_x;
    acc_y_ = serial_interface_.getDataType()->robot_imu_actual_.acc.acc_y;
    acc_z_ = serial_interface_.getDataType()->robot_imu_actual_.acc.acc_z;

    att_r_ = serial_interface_.getDataType()->robot_imu_actual_.att.att_r;
    att_p_ = serial_interface_.getDataType()->robot_imu_actual_.att.att_p;
    att_y_ = serial_interface_.getDataType()->robot_imu_actual_.att.att_y;

    motor_speed_a_ =
        serial_interface_.getDataType()->motor_speed_actual_.motor_a;
    motor_speed_b_ =
        serial_interface_.getDataType()->motor_speed_actual_.motor_b;
    motor_speed_c_ =
        serial_interface_.getDataType()->motor_speed_actual_.motor_c;
    motor_speed_d_ =
        serial_interface_.getDataType()->motor_speed_actual_.motor_d;

    motor_mileage_a_ =
        serial_interface_.getDataType()->motor_mileage_actual_.motor_a;
    motor_mileage_b_ =
        serial_interface_.getDataType()->motor_mileage_actual_.motor_b;
    motor_mileage_c_ =
        serial_interface_.getDataType()->motor_mileage_actual_.motor_c;
    motor_mileage_d_ =
        serial_interface_.getDataType()->motor_mileage_actual_.motor_d;

    motor_thrust_ = serial_interface_.getDataType()->motor_thrust_actual_.thrust;

    robot_alt_ = serial_interface_.getDataType()->robot_height_actual_.alt;
    robot_hei_ = serial_interface_.getDataType()->robot_height_actual_.hei;

    battery_capacity_ =
        serial_interface_.getDataType()->robot_system_info_actual_.battery_capacity;
}

void FlightControlStation::updateBufferWrite(void)
{

}
