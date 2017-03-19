/*
Navicat MySQL Data Transfer

Source Server         : test
Source Server Version : 50716
Source Host           : localhost:3306
Source Database       : nodemysql

Target Server Type    : MYSQL
Target Server Version : 50716
File Encoding         : 65001

Date: 2017-03-19 22:40:39
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `env`
-- ----------------------------
DROP TABLE IF EXISTS `env`;
CREATE TABLE `env` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `tem` int(11) DEFAULT NULL,
  `hum` int(11) DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of env
-- ----------------------------
INSERT INTO `env` VALUES ('3', '23', '66');
INSERT INTO `env` VALUES ('4', '21', '67');
INSERT INTO `env` VALUES ('5', '25', '50');
INSERT INTO `env` VALUES ('6', '25', '50');
