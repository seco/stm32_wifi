var Weibo = require('nodeweibo');
var setting = require('./setting.json');

Weibo.init(setting);

Weibo.authorize();