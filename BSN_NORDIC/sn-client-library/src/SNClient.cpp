/*
 * SNClient.cpp
 *
 *  Created on: 9 Jun 2020
 *      Author: musahl
 */

#include "SNClient.h"
#include "SNClientImpl.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Debug.h"

SNClient::SNClient(const std::string &deviceName, uint32_t baudRate)
    :
    pimpl { std::make_unique < impl > (std::make_unique < SNClientTransportSerialPort > (deviceName, baudRate)) } {
  DEBUG() << this << " created " << std::endl;

}

SNClient::SNClient(std::unique_ptr<SNClientTransport> alternativeTransport)
    :
    pimpl { std::make_unique < impl > (std::move(alternativeTransport)) } {
  DEBUG() << this << " created " << std::endl;
}

SNClient::~SNClient() {
  DEBUG() << this << " deleted " << std::endl;
}

void SNClient::attachListener(SNClientListener &listener) {
  pimpl->attachListener(listener);
}

void SNClient::detachListener(SNClientListener &listener) {
  pimpl->detachListener(listener);
}

SNCLientState SNClient::getstate() {
  return pimpl->getstate();
}

bool SNClient::detectSensors() {
  return pimpl->setState(SNCLientState_Discovery);
}

bool SNClient::writeEEPROMData(std::string& sensorID,const uint8_t *data, const uint_fast32_t length){
  return pimpl->writeEEPROMData( sensorID, data, length);
}

bool SNClient::requestEEPROMData(std::string& sensorID){
  return pimpl->requestEEPROMData(sensorID);
}


bool SNClient::startMeasuring() {
  return pimpl->setState(SNCLientState_Running);
}

bool SNClient::stopMeasuring() {
  return pimpl->setState(SNCLientState_Idle);
}

bool SNClient::setOption(std::string optionName, std::string optionValue, std::string sensorID){
  return pimpl->setOption(optionName,optionValue,sensorID);
}

std::string SNClient::getOption(std::string optionName, std::string sensorID)
{
  return pimpl->getOption(optionName,sensorID);
}

const std::vector<std::shared_ptr<SNSensorInformation>> SNClient::getSensors() {
  return pimpl->getSensors();
}

const uint32_t SNClient::getVersion() {
  return pimpl->getVersion();
}

const bool SNClient::isCompatibleVersion(uint32_t version) {
  return pimpl->isCompatibleVersion(version);
}

void SNClient::sendRawMessage(const uint8_t *data, const uint_fast32_t length) {
  pimpl->sendRawMessage(data, length);
}

void SNClient::setDFKIProtocol(bool active) {
  pimpl->setDFKIProtocol(active);
}

bool SNClient::isDFKIProtocolActive(void) {
  return pimpl->isDFKIProtocolActive();
}

