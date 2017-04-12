var Weibo = require('nodeweibo');
var setting = require('./setting.json');

Weibo.init(setting);
// access_token应该是不变的，status里面试要发送的内容
var para = {
	"access_token":"2.00IlvKmF05EX2B7c5e17d246vKldGE",
	"status":"可能再给我发一条。。。"
}
Weibo.Statuses.update(para,function(data){
	console.log(data);
})