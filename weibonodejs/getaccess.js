var Weibo = require('nodeweibo');
var setting = require('./setting.json');

Weibo.init(setting);

var paras={
	code:"a45ce4c7065792f00d50bb2daf7ae20a",
	grant_type:"authorization_code"
};

Weibo.OAuth2.access_token(paras,function(data){
	console.log(data);
})