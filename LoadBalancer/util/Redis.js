/**
 * Created by KaeulHan on 2016. 2. 22..
 */

var redis = require('redis');
var redis_client = redis.createClient(6800,'127.0.0.1');

redis_client.on('error', function(error)
{
    console.error("error in redis");
});

exports.getRedisConnection = function() {
    return redis_client;
}


exports.initRedis = function() {

    for(var index =1; index<=8; index++) {
        this.setSession('sessionList1', 'port'+index, null, function (result) {
            console.log("redis init() = " + result);
        });

        this.setSession('sessionList2', 'port'+index, null, function (result) {
            console.log("redis init() = " + result);
        });
    }
}



exports.getSession = function(category, key, callback)
{
    redis_client.hget(category, key,
        function(err,reply)
    {
        if(err)
        {
            console.error(err.message);
        }
        else{
            callback(reply);
        }
    });
}

exports.getSessionList = function(category, callback)
{
    redis_client.hgetall(category,
        function(err,reply)
        {
            if(err)
            {
                console.error(err.message);
            }
            else{
                callback(reply);
            }
        });
}

exports.setSession = function(category, key, value, callback)
{
    redis_client.hset(category,
        key,value, function(err,reply)
    {
        if(err)
        {
            console.error(err.message);
        }
        else{
            callback(reply);
        }
    });
}

exports.delSession = function(category, key, callback)
{
    redis_client.hdel(category, key, function(reply)
    {
        callback(reply);
    });
}

exports.getPortLB = function(category,index,callback)
{
    this.getSession(category, "port"+index ,function(result)
    {
        if(result==null)
            callback(index);
        else{
            callback(null);
        }
    });
}