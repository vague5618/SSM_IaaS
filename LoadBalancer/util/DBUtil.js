var Mongolian = require('mongolian')
    ,server =new Mongolian
    , db = server.db('node_test')
    , members = db.collection('members')
    , memberIndexers = db.collection('memberIndexers')
    , imageIndexers = db.collection('imageIndexers')
    , images = db.collection('images');

exports.getMembersConnection = function()
{
    return members;
}

exports.getMemberIndexersConnection = function()
{
    return memberIndexers;
}

exports.getImageIndexersConnection = function()
{
    return imageIndexers;
}


exports.getImagesConnection = function()
{
    return images;
}

exports.getDb = function()
{
    return db;
}
