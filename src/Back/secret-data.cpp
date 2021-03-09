#include "Back/secret-data.h"

#include <QDebug>
#include <utility>

Network::UserData::UserData(QObject *parent) : QObject(parent) {}

Network::UserData::UserData(QString username, QString accessToken, QString refreshToken, QObject *parent)
    : QObject(parent),
      username_(std::move(username)),
      accessToken_(std::move(accessToken)),
      refreshToken_(std::move(refreshToken)) {}

QString Network::UserData::getUsername() {
  return username_;
}

void Network::UserData::setUsername(QString username) {
  this->username_ = username;
}

QString Network::UserData::getAccessToken() {
  return accessToken_;
}

void Network::UserData::setAccessToken(QString accessToken) {
  this->accessToken_ = accessToken;
}

QString Network::UserData::getRefreshToken() {
  return refreshToken_;
}

void Network::UserData::setRefreshToken(QString refreshToken) {
  this->refreshToken_ = refreshToken;
}

void Network::UserData::initSecretData(QString username, QString accessToken, QString refreshToken) {
  this->username_ = std::move(username);
  this->accessToken_ = std::move(accessToken);
  this->refreshToken_ = std::move(refreshToken);
}
