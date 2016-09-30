/**
 * Created by KaeulHan on 2016. 2. 22..
 */

var redis = require('redis');
var redis_client = redis.createClient(6800,'211.189.19.30');

redis_client.on('error', function(error)
{
    console.error("error in redis");
});

exports.getRedisConnection = function() {
    return redis_client;
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
    redis_client.hdel(category, key, function(err,reply)
    {
        if(err)
        {
            console.error(err.message);
        }
        else {
            callback(reply);
        }
    });
}